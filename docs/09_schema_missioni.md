# Schema delle missioni (bozza, versione di formato 1)

Bozza del 23/10/2026 per il motore Enhanced. Le 22 scene originali sono convertite in questo formato in
`data/missioni/<SCENA>.lua` da `tools/re/missioni_lua.py`, a partire dalle schede (`re/tok/schede/`), dal
bytecode `.TOK` (`docs/04_vm_missioni.md`) e dalle traduzioni (`docs/traduzione/scene/`).

## 1. Principi

- **Una missione è un file Lua 5.4** che restituisce una tabella. Il file gira in un ambiente isolato (niente
  `io`, `os`, `require`, `load`): vede soltanto la tabella `m688`, che contiene le funzioni del motore, e le
  funzioni di base sicure (`math`, `string`, `table`, `pairs`, `ipairs`).
- **La parte dichiarativa** (teatro, unità, timer, testi) si legge senza eseguire codice. Il motore la valida
  e la carica; l'editor la modifica.
- **I comportamenti sono funzioni Lua** associate a eventi dell'unità. Chiamano solo funzioni `m688.*` e
  ricevono `u`, l'unità che esegue.
- **Grandezze**: posizioni in metri sul teatro (x verso est, y verso nord, origine nel centro della
  proiezione del teatro, come le heightmap di `data/teatri/`); quote in metri (positive verso il basso);
  rotte in gradi bussola (0 = nord, senso orario), virate e ratei di virata con il positivo a dritta; velocità in
  nodi; tempi in secondi; distanze di soglia in miglia nautiche. L'originale usa angoli matematici (0 = est, senso
  antiorario): il convertitore li traduce dal 24/10/2026 (`docs/originale/numeri_eseguibile.md` §3.3–§4).
- **Fedeltà all'originale**: ogni unità conserva anche le coordinate originali e i valori grezzi del modello
  (sezione 4). Il motore può quindi giocare la versione «classica» o ricollocare la missione.

## 2. Struttura del file

```lua
return {
  formato = 1,
  id = "SCENE1A",                 -- nome della scena originale
  missione = 1,                   -- numero nel manuale (0 = missione 10)
  titolo = { it = "TORPEX '89", en = "TORPEX '89" },
  lato = "usa",                   -- "usa" | "urss" | "due_giocatori"
  teatro = "faroe",               -- cartella in data/teatri/
  posa = { ... },                 -- sezione 3
  data = nil, ora_utc = nil,      -- l'originale non li ha: li scelgono l'editor o il motore
  ambiente = { termoclini_m = { 222.9, 522.9, 845.0 } },
  dotazione = { mk48 = 22, harpoon = 2, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = { mk48 = { ... }, ... }, -- modelli delle armi lanciate (sezione 5)
  varianti = { ... },             -- disposizioni iniziali con probabilità (sezione 4)
  gestori_giocatore = { ... },    -- script che l'originale esegue sui comandi del giocatore
  briefing = { it = { "...", ... }, en = { ... } },
  esiti = { [1] = { it = "...", en = "..." }, ... },
  script = { L_0214 = function(u) ... end, ... },   -- comportamenti (sezione 6)
}
```

## 3. Posa sul teatro

Il gioco originale usa una mappa di 10240 × 6400 unità per teatro (1 nm di gioco = 50 unità, y verso nord).
La conversione in metri si dichiara nella missione:

```lua
posa = {
  metodo = "ricollocata",         -- "georef" | "ricollocata" | "scala_di_gioco"
  centro = { lat = 61.92, lon = -6.90 },
  metri_per_unita = { x = 23.335, y = 23.335 },
  moto_classico_m_s_per_nodo = 1.1216,   -- velocità del moto dell'originale (×3,46) in questa posa
  affidabilita = "media",
  nota = "...",
}
correzioni = { unita = 0, punti = 12, spostamento_max_m = 168.8, non_risolte = 0 },   -- vedi sotto
```

- **georef**: le carte originali sono collocate sul teatro reale da `data/teatri/associazione_surf.json`
  (Gibilterra alta, Otranto e Kunashir media). La mappa intera copre il riquadro trovato; le distanze reali
  sono circa il 60% di quelle di gioco.
- **ricollocata** (Fær Øer e Baltico, 23/10/2026): `tools/re/ricolloca_surf.py` sovrappone le terre emerse
  della carta originale (livello 15) alla terra reale di ETOPO, con traslazione e scala isotropa e senza
  rotazione, così le rotte degli script restano valide.
  - Poi affina la soluzione per mettere in acqua le unità navali, tenendo l'aderenza alle coste ad almeno il
    90% della migliore.
  - **Fær Øer:** 23,3 m per unità (×0,63). L'arcipelago della carta coincide con quello reale; TORPEX
    parte a sud di Suðuroy, come dice il briefing.
  - **Baltico:** 45,9 m per unità (×1,24), sulla heightmap allargata del 23/10/2026 (53,9–56,4 N,
    10,5–16,2 E). L'isola centrale è Bornholm, con la Svezia a nord, la Pomerania a sud e la Selandia a
    nord-ovest.
    - Il 688 parte al largo di Usedom e il bersaglio dei missili è nell'entroterra.
    - La flotta sovietica sbarra la via verso l'Øresund.
    - La destinazione di fuga degli script è nel Kattegat, a 56,21 N e 11,35 E: le «acque di Copenaghen».
  - Risultati e confronto visivo in `re/surf/` (`ricollocazione.json`, `SURF*_ricollocata.png`).
- **scala_di_gioco**: Barents e Celtica, dove la carta non ha coste riconoscibili. Si usa la scala del gioco
  (1 nm = 50 unità, 37,04 m per unità), centrata sul centro del teatro. Lì tutte le unità sono già in acqua.
- Formula: `x_m = Pproj(lon(x_u), lat(y_u))` per **georef**; `x_m = x_c + (x_u − 5120) · mx`, `y_m = y_c + (y_u −
  3200) · my` per **ricollocata** e **scala_di_gioco**. Nel secondo caso `(x_c, y_c)` = (0, 0), il centro del
  teatro.
- **Correzioni sul fondale.** Le carte originali sono stilizzate, quindi il convertitore controlla ogni punto
  sulla heightmap e sposta nell'acqua più vicina, entro un riquadro di ±10 km:
  - le **unità navali** che cadono su terra o su un fondale troppo basso, contando il minimo entro ±400 m: per le
    navi almeno il pescaggio della classe + 3 m (mai meno di 10 m), per i sottomarini 25 m; i falsi bersagli restano
    dove sono;
  - i punti delle loro **rotte**;
  - le **destinazioni** `vai_a` degli script.

  Inoltre riduce la **quota iniziale dei sottomarini** a fondale − 10 m. Ogni unità corretta riporta
  `spostamento_m` e, se serve, `quota_originale_m`; il totale per missione è in `correzioni`. Rettangoli e
  soglie degli script non vengono toccati.
- **Nel motore** (`engine/README.md`, «Fondale e incagli») la collocazione si ripete con il pescaggio della
  classe, anche per le unità che gli script creano o spostano: acqua comoda (pescaggio + 3 m per le navi, + 10 m
  per i sottomarini) e collegata al mare aperto. Le quote restano limitate dal fondo per tutta la missione.
- **Moto classico.** L'originale muove le unità 3,46 volte più veloci del vero
  (`docs/originale/sensori_e_navigazione.md` §6). Per giocare la versione classica con i tempi
  dell'originale, una velocità in nodi si converte con `moto_classico_m_s_per_nodo` (0,048065 ×
  metri per unità), non con 0,5144.

## 4. Unità e varianti

L'originale sceglie la disposizione iniziale a caso (`IF_RND`); le probabilità sono esatte (frazioni di 256).

```lua
varianti = {
  { probabilita = 0.33203125, unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 0x14, nome_originale = "US 688 CLASS ATTACK SUBMARINE",
        categoria = "sottomarino", schieramento = "usa", flag = 0x09, tipo = 1, giocatore = true,
        posizione = { x = -1234.5, y = 678.9 }, originale = { x = 5472, y = 2464 },
        quota_m = 45.7, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.49,
                    quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil,
                    schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = ..., y = ... }, ... }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0214", reazione = "L_021c", arrivo = nil },
      }, ... },
    timer = { { dopo_s = 120, unita = 3, gestore = "L_0290" }, ... },
    variabili = { [1] = 0, ... },
  }, ... },
```

- `classe` è la chiave di `data/motore/classi.toml`. Il tipo 00h dell'originale (Forrest Sherman) diventa
  `spruance`. Le classi non ancora nel database sono indicate in `classi_mancanti`.
- `categoria` e `schieramento` vengono dai bit del flag (`docs/originale/comportamenti_ia.md` §1):
  `sottomarino`, `nave`, `aereo`, `arma`; `usa`, `urss`, `neutrale`; più `non_combattente` e
  `falso_bersaglio`.
- `tipo` è il `+04` dell'originale (opcode `TYPE`): l'unità osserva una volta ogni `tipo` giri della
  rotazione dei sensori (`docs/originale/sensori_e_navigazione.md` §3.1). Vale 1 quasi sempre, 2, 3 o 20 per
  alcune unità; senza il campo vale 1. Le unità con il flag 40h (`non_combattente`) non osservano mai.
- `controllo_fondale` (bit 0 di `+4E`) dice se l'originale controlla fondale e terra per quell'unità: sì per i
  sottomarini (non i Foxtrot) e per parte dei siluri, mai per le navi. `periodo_controlli` (`+4C`) è ogni
  quante visite si fanno i controlli (`docs/originale/sensori_e_navigazione.md` §9).
- `modello` riporta i valori dell'originale convertiti (`docs/originale/prestazioni.md`). Il remake li sostituirà
  con quelli realistici della classe: servono per la versione classica e per i confronti.
- Per le scene con molte varianti (SCENE9A, SCENE9B) si tengono le 50 più probabili, rinormalizzate; la
  somma originale è nel campo `copertura`.
- `modello.schiera_trainata = true` su una Perry o uno Spruance (Enhanced): la nave fila la SQR-19 e il suo cavo è un
  ostacolo che s'impiglia nell'elica dei sottomarini (S10; eventi `cavo_impigliato` e `cavo_toccato`).
- `batteria = 0..1` (Enhanced, M6, facoltativo): la carica iniziale di un diesel dell'IA (Kilo, Foxtrot).
  Predefinita 0,8.
  - Sotto 0,3 il battello va a snorkel a quota periscopica, se non è in caccia.
  - A 0,05 emerge per forza, con l'evento `emersione_forzata` nel registro e per `se.evento` della radio.
  - Lo leggono `Missione::batteria(id)` e `snorkel(id)`; per una regola dell'editor basta `m688.in_superficie(u)`
    sull'unità.
- **Armi da esercitazione** (schede I03 e S01): inerti, senza testata. Il colpo si registra (evento
  `colpo_esercitazione`, obiettivo `colpire`, messaggi 456 a segno e 457 colpiti) ma non fa danni né scoppi: l'arma
  esce dal gioco come all'impatto, nel classico e nell'Enhanced.
  - Chiave della missione `esercitazione = true`: tutte le armi.
  - Chiave d'unità `armi_esercitazione = true`: le armi che quell'unità lancia.
  - Chiave `esercitazione = true` su un'arma della missione, per esempio un siluro della riserva.
  - Script: `m688.esercitazione(u [, si])`, sull'arma o sul lanciatore.
- `stato = "relitto"` (chiave d'unità, scheda I10): un'unità affondata, ferma nel suo punto con la chiglia sul fondo.
  Non sente niente e nel classico non si sente (rumore 0). Si trova con l'eco del sonar attivo, che si confonde con la
  riverberazione del fondo, o col MAD. `Missione::relitto(id)`.
- `danni_iniziali = { ... }` (chiave della missione, Enhanced con la regola di simulazione; scheda I07): il battello
  del giocatore comincia danneggiato.
  - `urto = { tnt_kg, distanza_m, angolo_gradi }`: un'esplosione vicina, come l'opzione di prova `--urto-giocatore`.
  - `sistemi = { propulsione = 0.4, schiera = 0 }`: l'efficienza di partenza, da 0 a 1, coi nomi di `nucleo/battello`
    (calcolatore_sonar, schiera, periscopio, sonar_tiro, motori, antenna, idroplani, timone, propulsione, aria, pompe,
    sonar_prua). Un nome sconosciuto è un errore.
  - `acqua = { { compartimento = "macchine", m3 = 10 } }`, `falle = { { compartimento, area_m2 } }`,
    `incendi = { { compartimento, intensita } }`. Il compartimento si indica col nome o con l'uso dello scafo
    (`data/motore/scafi`): sul 688 C1 deposito, C2 reattore, C3 macchine.
  - Li applica `DanniSottomarini` al primo passo; i guasti vanno nel rapporto della stazione Stato.
- `sonar_speciale = { sl_db, frequenza_hz, impulso_s, risoluzione_s, ogni_s, dimensione_m }` (chiave d'unità,
  Enhanced con l'acustica; scheda I09).
  - Al ping dell'unità prende il posto del suo sensore attivo, o ne fa uno se la classe non ne ha. La frequenza va
    alla banda più vicina del modello.
  - `risoluzione_s`: FM con compressione (1/B). Contro la riverberazione serve più dell'SL.
  - `dimensione_m`: l'antenna (0: quella della classe); `antenna`: sfera, lineare o scafo. Con `ogni_s` > 0 pinga
    da sola con quella cadenza.
  - Nel modello di oggi una lineare sulla nave rende peggio della sfera contro la riverberazione (fascio verticale largo,
    trasduttore a quota 0): a 1 kHz con 150 m, −21 dB a 40 km.
  - Esempio: 230 dB a 3,5 kHz, antenna di 15 m, FM a 5 ms: lo Slava vede l'eco del 688 a 150 m a 40 km. Col sonar
    della classe, sotto i 20 km.
- `acque_territoriali = { { nome = "Svezia", lato = "neutrale", cerchio = { x, y, r } }, ... }` (chiave della missione,
  Enhanced; al posto del cerchio anche `vertici = { {x, y}, ... }`). `lato` è il paese costiero: usa, urss o neutrale.
  - Vale in pace, se il giocatore è di un altro paese ed entra.
  - Le navi del paese costiero sparano colpi d'avvertimento al battello esposto. Se è immerso e ne conoscono la
    posizione, lanciano bombe d'avvertimento.
  - Registro: `acque_territoriali`. Messaggi 453-455. Script: `m688.in_acque_territoriali(u)`.
  - Dettagli in docs/16.
- `stato = "avaria"` o `"incagliata"` (M6, facoltativo; `missioni/src/missione_avaria.cpp`): l'unità dell'IA comincia
  in avaria o sugli scogli. Non vale per il giocatore. Lo stato iniziale non è un evento del registro.
  - In avaria la propulsione è persa: l'unità si ferma, qualunque cosa ordini il suo script. Ogni 1,5-5 minuti fa un
    rumore di riparazione, che i sonar sentono come un transitorio.
  - Incagliata: resta dove si trova finché non la prendono a rimorchio.
  - Script: `m688.avaria(u [, si])`, `m688.incaglia(u)`, `m688.rimorchia(u, id [, velocita_kn, cavo_m])`,
    `m688.molla_rimorchio(u)`, `m688.in_avaria(u)`, `m688.rimorchiata(u)`.
  - Il rimorchio: il rimorchiato tiene il cavo teso sulla scia della nave (300 m per difetto); la nave non supera la
    velocità data (6 nodi per difetto, al più 12). Un sottomarino rimorchiato viaggia in superficie. Il rimorchio stacca
    l'unità dagli scogli e finisce con `molla_rimorchio` o quando una delle due esce di gioco o affonda.
  - Registro: `avaria_ia` (un'avaria nuova) e `rimorchio` (livello 0 all'inizio, 1 alla fine).

## 5. Armi

Dai gestori di lancio (HANDLERS_C), per tipo: `mk48`, `type53`, `sea_lance`, `tomahawk`, `harpoon`, `esca`.

```lua
armi = { mk48 = { velocita_kn = 50.3, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17,
                  testa_dopo_filo = 13, virata_gradi_s = 9.0, quota_max_m = 548.6 }, ... }
```

Il TLAM-C dell'Enhanced (famiglia `attacco_terra`, carico M2) non chiama mai il gestore del `tomahawk`, che nelle
missioni è il TASM antinave. Una missione che vuole contare gli attacchi a terra dichiara `attacco_terra = { gestore =
"..." }` in `armi`: il gestore parte a ogni lancio del giocatore, e un suo `m688.lancio_missile(u)` non conta un
secondo lancio. Senza la chiave il lancio è solo l'evento del registro (28/10/2026).

### 5.0 Paesaggio sonoro dell'Enhanced

`paesaggio_sonoro = false` nella tabella della missione spegne le sorgenti ambientali (balene, pesci, ghiaccio,
pescherecci) attorno al giocatore: serve alle scene di prova che vogliono i sensori isolati. Manca o vale true: acceso.

### 5.1 Messaggi radio dell'Enhanced (facoltativi)

**Formato provvisorio** (24/10/2026): il ruolo della radio in partita e il formato definitivo dei contenuti
aspettano il piano su `docs/ricerca/10_radio_ruolo_in_partita.md`; per ora nessuna missione ha questo file.

Un file `data/missioni/radio/<ID>.lua` accanto alle missioni, se esiste, aggiunge alla sala radio dell'Enhanced
(engine/README, «Sala radio dell'Enhanced») i messaggi della missione. Restituisce una lista:

```lua
return {
  { dopo_s = 600, precedenza = "immediate", da = "COMSUBLANT", oggetto = "NUOVA ZONA", classifica = "SECRET",
    testo = { it = "...", en = "..." }, gestore = "L_0520", autentico = true },
}
```

- `precedenza`: flash, immediate, priority, routine (predefinita);
- `gestore`: funzione di `script` della missione eseguita alla ricezione (ordini che cambiano), per l'unita' del giocatore;
- `autentico = false`: il gruppo d'autenticazione non torna (inganno).
I file Lua delle missioni convertite non si toccano: i messaggi stanno in questo file a parte. Il livello classico lo
ignora.

**Chiavi della fase R1** (dossier 10 §5; letti dal motore, nella stessa tabella della lista dei messaggi):
- `parametri = { ciclo_s, fondo_ogni_s, finestra_ogni_s, finestra_primo_s, tolleranza_s }` (predefiniti: ciclo 900);
- `stato_iniziale = { roe = { navi = "trattenuto", ... }, neutrali = { 8 } }` (categorie sottomarini, navi,
  mercantili, neutrali, aerei; stati libero, stretto, trattenuto);
- `rapporti = { contatto = { gestore, ritardo_s, una_volta } }`; `slot = { boe, ritardo_min_s, ritardo_max_s, portata_vhf_m }`;
- `gestori = { NOME = function(u) ... end }`: si aggiungono agli script della missione;
- nei messaggi: `id`, `roe = { ... }`, `contatto = { unita = { 4, 5 }, eta_s, errore_m, sensore, rotta_err,
  velocita_err_kn, falso }`.

**Chiavi della fase R2:**
- `stato_iniziale.zone = { { id, tipo, cerchio = { x, y, r } | vertici = { { x, y }, ... } | unita, avanti_m,
  dietro_m, lato_m, dal_s, al_s } }`, con `tipo` azione, libera_asw, protetta, protetta_mobile;
- `finestre = { ogni_s, primo_s, tolleranza_s, obbligatorie, ricevuta, saltate_max, gestore_saltate }`;
- nei messaggi: `zone = { aggiungi = { ... }, togli = { "ID" } }`, `scade_s`, `gestore_scaduto`; `autentico = false`
  per un inganno.

**Chiavi della fase R3 e di E6** (26/10/2026, tutte facoltative):
- **La radio nella missione:** `radio = { ... }` nella tabella della missione (rapide, editor, libreria), con la stessa
  forma del file. Se c'è, vale al posto del file `data/missioni/radio/<ID>.lua`.
- `ordini = { da, a, oggetto, classifica, data = { giorno, mese, anno, ora_utc } }`: l'intestazione degli ordini di
  missione (serie 0) e il giorno e l'ora del DTG. Il corpo resta quello del briefing. Senza: FM/TO/SUBJ del briefing,
  SECRET e la data della missione.
- **Nei messaggi:**
  - `se = { ... }`: va in onda alla prima volta che la condizione è vera, non prima di `dopo_s`. Tutte le chiavi date
    devono valere insieme:
    - `ricevuto = "ID"`;
    - `unita_colpita = { id, ... }`: una qualunque, colpita o fuori dal gioco;
    - `unita_fuori = { ... }`;
    - `variabile = { indice, "op", valore }`: le variabili degli script, con op fra `==`, `~=`, `>=`, `<=`, `>`, `<`;
    - `trasmesso = "contatto"`: un nome di trasmissione, anche attacco, situazione, conferma, richiesta, previsione;
    - `evento = "<nome>"`: un evento del registro M1, per esempio «lancio_nemico» o «avvistato».
  - `su_richiesta = true`: non va in onda da solo, ma solo con `m688.radio(u, "ID")`.
  - `annulla = "ID"` («disregard»): quando questo messaggio va in onda la prima volta, quello con l'id dato, se non è
    ancora andato in onda, non ci va più.
  - `ricevuta = true`: il comando vuole la ricevuta; il giocatore deve trasmettere dopo averlo ricevuto. Il
    debriefing della radio elenca le ricevute mancate.
  - `ripetizioni = n`: le messe in onda al posto di quelle della precedenza.
- `m688.radio(u, "ID")` dagli script: il messaggio con quell'id, dalla lista della radio della missione, va in onda
  subito. È falso senza la sala radio, se il messaggio non c'è o se è già in coda.

### 5.1b Meteo fissato e messaggi di bordo propri (E6)

- `meteo = { fisso = { regime, vento_m_s, vento_da_gradi, raffica_m_s, stato_mare, hs_swell_m, swell_da_gradi,
  nubi_ottavi, plafond_m, pioggia_mm_h, neve_mm_h, visibilita_m, nebbia, lampi_al_minuto, t_aria_c, t_mare_c,
  pressione_hpa, ghiaccio } }`: i campi dati valgono sopra il meteo del teatro all'inizio e restano fermi per tutta la
  missione, anche col meteo semplificato delle opzioni. `nebbia`: nessuna, avvezione, fumo_di_mare, radiazione,
  precipitazione. `stato_mare` dà il mare da vento (Douglas).
- `messaggi = { [n] = { it = "...", en = "..." } }`: messaggi di bordo propri della missione, col codice **1000 + n**,
  fuori dalla tabella globale (`messaggi_bordo.tsv`, tutta sotto 1000).
  - Si mandano con `m688.messaggio(u, 1000 + n)` o `m688.messaggio_al_giocatore(u, 1000 + n)` e non sono esiti.
  - L'app ne prende il testo con `Missione::testo_messaggio_missione(codice)`.

### 5.1c Mine dell'IA (Enhanced; docs/17 §3, S06 e I04)

- `mine = { { tipo = "mk67", x = ..., y = ..., lato = "urss", armata = true }, ... }`: un campo già posato all'inizio,
  sul fondo nel punto dato.
  - `tipo` è una voce di `[mine.*]` di carico.toml (mk67, mk60, pmr1, pmr2, mdt), con le sue profondità: fuori dalle
    profondità la mina è inerte.
  - `lato`: urss, usa o neutrale. Le mine non distinguono amici e nemici.
  - `armata` (vero per difetto): già armata, altrimenti si arma col tempo del tipo.
  - Il campo non compare fra gli eventi del registro.
- `m688.posa_mina(u, "tipo" [, quante, ogni_s])`: l'unità u, una nave o un sottomarino dell'IA, posa `quante` mine
  (1 per difetto) sotto di sé, una ogni `ogni_s` secondi (60 per difetto).
  - È vero se la posa è partita.
  - Le mine posate entrano nel registro come `mina_posata` del posatore, non note al giocatore.
- Solo con i siluri dell'Enhanced: nel classico non succede niente.
- Il giocatore le scopre col BQS-15 (ordine `bqs15`, n = 1 acceso, 0 spento; `missioni/src/missione_bqs15.cpp`).
  - Una spazzata ogni 8 s, nel settore di prua: ±60° in azimut, da −30° a +45° in elevazione.
  - Portata: le mine ormeggiate (mk60) entro ~1,1 km, quelle sul fondo entro ~450 m. La velocità la riduce: −30% a
    15 nodi, −60% oltre i 20. Sono stime del dossier 06 §1.1.
  - Ogni mina vista entra in `Missione::mine_scoperte()`, con la posizione stimata (~5 m e ~1° d'errore). Va nel
    registro come `mina_scoperta` e dà il messaggio 451.
  - Ogni spazzata è un impulso di 205 dB a 25 kHz. Chi ha un ricevitore d'intercettazione (WLR-9, sovietico) lo
    sente: nel fascio a pieno livello, fuori coi lobi laterali a −25 dB.
  - Script: `m688.bqs15(u [, acceso])` e `m688.mine_scoperte()`. Ce l'hanno solo le classi `los_angeles`.

### 5.2 Obiettivi e categorie degli esiti (registro M1)

Servono al debriefing e alla carriera (docs/12, M1 e M3). Per le missioni rapide e dell'editor stanno nella missione
(chiavi `obiettivi` e `categorie_esiti`); per le 22 originali nel file `data/missioni/obiettivi/<ID>.lua`, che
restituisce `{ categorie_esiti = ..., obiettivi = ... }`. Se nessuno dei due c'e', l'obiettivo e' solo sopravvivere.

```lua
categorie_esiti = { [1] = "battello_perso", [6] = "successo", [8] = "successo_parziale", [19] = "fuoco_amico" },
obiettivi = {
  { id = "petroliere", tipo = "affondare", unita = { 4, 5, 6, 7 }, minimo = 4,
    testo = { it = "Affondare le quattro petroliere libiche", en = "Sink the four Libyan tankers" } },
  { id = "fregate", tipo = "proteggere", classi = { "koni" }, colpire = false, primario = false, testo = { ... } },
  { id = "vivo", tipo = "sopravvivere", testo = { ... } },
},
```

- **Categorie:** successo, successo_parziale, fallimento, battello_perso, richiamato, fuoco_amico. Il codice e' l'n
  dell'esito, cioe' il messaggio 100 + n.
- **Tipi:**
  - `affondare` e `proteggere`: `unita` (id come negli script) o `classi`, con `minimo` (predefinito: tutte);
    `colpire = false` vuol dire non colpire nemmeno;
  - `raggiungere`: `cerchio = { x, y, r }` in metri come le posizioni, con `entro_s`; oppure, senza cerchio,
    `unita` (o `classi`) con `distanza_m`: arrivare a quella distanza da un'unità in moto;
  - `scoprire`: classificare le unita' indicate;
  - `non_essere_scoperto`: nessuna contro-scoperta e nessun fix;
  - `sopravvivere`;
  - `variabile`: `indice` con `uguale`, `almeno` o `al_piu`;
  - `esito`: `codici`;
  - `minare` (M2b, Enhanced): `minimo` mine posate (1 se manca) e arrivate sul fondo non inerti, dentro
    `cerchio = { x, y, r }` se c'e', con `entro_s`.
  - `contatto`: almeno `minuti` di contatto sonar visibile del giocatore, cumulativi, con ognuna delle `unita` (o
    `classi`), `minimo` unità (predefinito: tutte), con `entro_s`.
  - `foto` (Enhanced, periscopi P2): le `unita` (o `classi`) nel campo di una foto del giocatore, cioè l'ordine
    `foto_periscopio` col periscopio che fotografa (il Type 18 del 688). Vale `minimo` unità (predefinito: tutte),
    entro `distanza_m` se c'è e con `entro_s`.
  - `colpire`: le `unita` (o `classi`) colpite dal giocatore, con armi vere o da esercitazione, anche senza affondarle;
    `minimo`, `entro_s`.
  - `zona_inviolata`: nessuna unità della `parte` (`ostile` per difetto, `amica` o `neutrale`, rispetto al
    giocatore) nel `cerchio = { x, y, r }`.
    - Contano navi e sottomarini, non armi e aerei; `unita` o `classi` restringono a quelle.
    - Con `minuti` un'unità può restarci fino a quel tempo.
    - Fallisce alla prima violazione. Si raggiunge a fine missione, o a `entro_s` se c'è.
- **Campi comuni:**
  - `id`, `testo = { it, en }`, `primario` (vale true se manca), `peso` (1 se manca);
  - `varianti` (indici da 1) se l'obiettivo vale solo in alcune;
  - `da_radio = "<id del messaggio radio>"`: un ordine che arriva con la sala radio; vale solo se il messaggio e'
    ricevuto.

### 5.3 Carico della Sala siluri e carriera (Enhanced, M2 e M3)

Tutti facoltativi; il livello classico li ignora e parte con `dotazione` e `tubi`. Dati delle armi e dei locali in
`data/motore/carico.toml`, regole della carriera in `data/motore/carriera.toml` (`engine/README.md`, «Carico e Sala
siluri» e «Carriera e fascicolo»).

```lua
deposito = { mk48 = 21, mk48_adcap = 4, harpoon = 6, tasm = 4, mk67 = 0, esca = 6 },  -- massimo per tipo
tipi_negati = { "tomahawk" },        -- famiglie o varianti
tipo_missione = "antinave",          -- antisom | antinave | attacco_terra | minamento | scorta: carico proposto
variante_battello = "flight2",       -- flight1 (predefinita) | flight2 | 688i: pozzi verticali
ospiti = 0,                          -- posti tolti alla rastrelliera (incursori: 6 per squadra)
what_if = false,                     -- consente il Sea Lance e le armi «what-if»
carriera = {                         -- rapide ed editor: senza il blocco la missione non conta per la carriera
  situazione = "crisi",              -- pace | crisi | guerra
  avversario = "veterano",           -- impreparato | regolare | veterano | elite (vale il minore fra questo e
                                     -- quello calcolato dalle unita' ostili)
  difficolta = 3,                    -- 1-5
  operazione_speciale = false,       -- motivazioni vaghe, PUC, Ordine di Lenin «incarico speciale»
  artico = false,                    -- Navy Arctic Service Ribbon
},
```

- Senza `deposito` il deposito è la `dotazione`, che fa da massimo e da carico predefinito.
- Le chiavi del deposito sono famiglie dell'originale (`mk48`, `type53`, `harpoon`, `tomahawk`, `sea_lance`, `esca`)
  o varianti di `carico.toml`; una famiglia vale per la sua variante predefinita e per quelle più nuove dal loro anno
  di servizio (l'anno è quello di `data`).
- TLAM-C (`tlam_c`), USET-80, TEST-71M e le mine (`mk67`, `mk60`, `pmr1`, `pmr2`, `mdt`) solo se dichiarati per nome
  o, per le mine, con la famiglia `mina`.
- Le 22 missioni originali hanno situazione, avversario e difficoltà in `carriera.toml` (`[missioni_originali]`).
- **Mine** (M2b): con `mina`, `mk67`, `mk60`, `pmr1`, `pmr2` o `mdt` nel deposito il giocatore le carica nei tubi e
  le posa con il lancio del tubo. La Mk 67 corre lungo la rotta e per la distanza (`rte`) impostate per il tubo; le
  altre si posano sotto il battello. Per il campo minato serve il fondale del teatro: la Mk 67 funziona fino a 100 m,
  la MDT fino a 50 m, la CAPTOR fra 200 e 900 m, le PMR-1 e PMR-2 fra 200 e 1000 m (`data/motore/carico.toml`,
  `[mine]`). La MDT occupa mezzo posto: due per tubo.

### 5.4 ASW aereo (Enhanced, docs/13_asw_aereo.md)

Con la regola di simulazione ogni aereo la cui classe e' fra i mezzi di `data/motore/asw_aereo.toml` (sh60b, sh3h,
sh60f, p3c, s3a, s3b, ka25, ka27, be12, il38, tu142) passa a un controllore con la dottrina del dossier 17. Lo
script decide il quando: la rotta di pattuglia, e `vai_al_bersaglio` che diventa un datum. Il controllore decide il
come: posa delle boe, dip, MAD, attacco, cambio del turno. Per le rapide e l'editor la configurazione sta nella
chiave `aereo_asw` della missione; per le originali nel file `data/missioni/asw/<ID>.lua`, che restituisce lo stesso
elenco. Senza configurazione l'aereo caccia i sottomarini dell'altra nazione, con la situazione del blocco `carriera`
(senza blocco: guerra).

```lua
aereo_asw = {
  { unita = 1,                        -- oppure classe = "ka27": vale per tutti gli aerei della classe
    missione = "caccia",              -- caccia | ricognizione (passa le coordinate, marca il fix con una DICASS,
                                      -- niente armi) | pattuglia (per ora come caccia)
    situazione = "guerra",            -- pace (SUS al posto delle armi) | crisi (solo rapporti) | guerra
    bersagli = { 2, 7 },              -- id dei sottomarini cercati, o "giocatore"; senza: quelli dell'altra nazione
    armi = true,                      -- false: niente attacco nemmeno in guerra
    carico = "sosus",                 -- un carico del mezzo in asw_aereo.toml (P-3C: sosus, barriera, guerra)
    equipaggio = "esperto",           -- esperto | medio | scarso; senza: secondo la difficolta' ASW
    base = { x = 0, y = -300000 },    -- o base_unita = id (la nave madre); senza: la posizione di partenza
    zona = { x = 0, y = 0, raggio_m = 20000 },   -- senza: la meta dello script o la posizione di partenza
    rotta_presunta = 180,             -- per le barriere senza cue
    cue = { tipo = "sosus", x = 0, y = 2000, raggio_m = 3000, ritardo_s = 300, rotta = 0, velocita_kn = 8 },
    gruppo = { capo = 1, indice = 0, numero = 4 },   -- Ka-27 in linea, coppia di Be-12
    sensore_classico = false,         -- le originali: lo script decide ancora col sensore classico dell'aereo
  },
}
```

- **Cue:** `tipo` sosus (predefinito), nave_colpita, siluro_sentito, radar_esm, contatto_nave, immersione_vista,
  zona; anche un elenco di cue. Dallo script: `m688.cue_asw(u, aereo, x, y, raggio_m, ritardo_s, rotta, velocita_kn)`.
- **Avvisi** per il giocatore (messaggi 380-389): aereo ASW in zona (solo in «facile»), tonfo in superficie, aereo in
  sorvolo, sonar a immersione, fumogeno in vista, attacco simulato, ping di boa attiva, siluro aviolanciato, radar
  a modo periscopio, contatto passato dall'aereo.
- La boa marcatrice degli script (missione 7) diventa una SSQ-62B DICASS posata dall'aereo controllato piu' vicino.

## 6. Comportamenti

Ogni etichetta del bytecode diventa una funzione in `script`. Le condizioni diventano
`if m688.<condizione>(u, ...) then goto L_xxxx end`, i salti `goto`, la fine dello script `do return end`.
Una sottoroutine (`CALL`) è una chiamata `script.L_xxxx(u)`. I riferimenti a script (`BEHAVIOR`, `ENGAGE`,
`PURSUE`, `LAUNCH`, `AT`) passano il nome dell'etichetta come stringa.

Eventi dell'unità e corrispondenza con la VM:

| Evento del motore | Campo VM | Quando |
|---|---|---|
| `bersaglio` | `+10` | una volta, quando un contatto diventa bersaglio: alla prima valutazione di un contatto nuovo, o dopo *n* aggiornamenti del contatto agganciato con `ingaggia(n, …)`; poi il gestore si azzera (prima si chiamava `periodico`, nome sbagliato: `docs/originale/sensori_e_navigazione.md` §8) |
| `reazione` | `+56` | quando l'unità perde il contatto agganciato con `ingaggia` o quello inseguito con `insegui`/`lancia`, cioè il bersaglio perso |
| `arrivo` | `+0E` | quando raggiunge il punto o il bersaglio di `GOTO_XY`, `GOTO_TGT`, `PURSUE`, `LAUNCH` |
| `timer` | `AT` | dopo il tempo indicato |

### API `m688` (livello classico)

Sono le operazioni della VM originale con nomi italiani e grandezze convertite. L'elenco completo, con
l'opcode di provenienza, è generato in `data/missioni/API.md`. Gruppi:

- **condizioni** (restituiscono vero o falso): `traccia_solida`, `traccia_lunga`, `traccia_media`,
  `bersaglio_vicino(nm)`, `bersaglio_id(id)`, `bersaglio_arma`, `bersaglio_nave`, `bersaglio_aereo`,
  `bersaglio_nemico`, `bersaglio_neutrale`, `bersaglio_mi_punta`, `caso(p)`, `variabile_uguale/maggiore/minore`,
  `unita_esiste(id)`, `unita_colpita(id)`, `in_rettangolo`, `livello_standard`, …;
- **manovra**: `rotta`, `accosta_verso_bersaglio`, `accosta_opposto`, `virata_relativa`, `telegrafo(frazione)`,
  `quota(m, rateo)`, `vai_a(x, y, …)`, `vai_al_bersaglio`, `insegui`, `uguaglia_velocita`, `uguaglia_quota`,
  `ferma_navigazione`, `riprendi_navigazione`;
- **sensori**: `schiera_trainata(0/1)` (per l'IA: ascolto a ±90°), `periscopio(0/1)`, `antenna(0/1)` (bit
  0–2 di `+54`); eventi acustici `impulso_sonar` (ping attivo, 33 s), `lancio_missile` (100 s),
  `rumore_transitorio` (11 s). Prima dei nomi corretti erano `sonar_attivo`, `flag_sonar_2`, `flag_sonar_4`,
  `distrutta`, `persa` e `registra_evento` (`docs/originale/sensori_e_navigazione.md` §2 e §4);
- **ingaggio**: `ingaggia(…)`, `lancia(arrivo, reazione)`, `attacca(nm, danno)`, `attacco_casuale(danno)`,
  `marca_contatti`;
- **stato e script**: `comportamento(nome)`, `nessun_comportamento`, `dopo(secondi, nome)`, `annulla_timer(nome)`,
  variabili e flag di scena, `messaggio(codice)` (i codici ≥ 100 sono gli esiti);
- **modello e creazione** (costruzione di nuove unità come nell'originale): `modello_*`, `nuova_unita`,
  `salva_modello`, `seleziona_*`, `collega`, `teletrasporta_al_bersaglio`.

Le soglie d'arrivo di `vai_a`, `vai_al_bersaglio` e `insegui` sono i valori grezzi dell'originale, in
**quarti di unità per asse**: in metri valgono `soglia / 4 × metri_per_unita`, con entrambi gli assi
richiesti. I dettagli, compreso il difetto della terza condizione, sono in
`docs/originale/sensori_e_navigazione.md` §5.

### 6.1 Gestori del giocatore

`gestori_giocatore` collega i comandi del giocatore agli script, come il dispatcher dell'originale:
`punto_di_rotta`, `telegrafo`, `quota`, `timone`, `pilota_automatico_off`, `periscopio`,
`carica_tubo_1..7`, `schiera_cala`, `schiera_recupera`, `schiera_persa`, `designa_bersaglio`, `posizione`,
`antenna`, `guida_e0..e2`, `f0`, `f1`. **`quota` e `timone` erano scambiati fino al 23/10/2026.**

L'originale scrive i parametri del comando negli argomenti della prima chiamata del gestore, per esempio
`telegrafo(u, 0.5)`, `rateo_virata(u, −3.6 … 3.6)` per il timone, `quota(u, m, rateo)` per i timoni
orizzontali. Tabella completa con codici e valori in `docs/originale/sensori_e_navigazione.md` §7.

**Gestori parametrici** (convertitore aggiornato il 24/10/2026, fase C1 di `docs/11_comandi_e_stazioni.md`):
- **Firma:** i 9 gestori con parametri diventano `function(u, p)`, con `p` = tabella dei parametri del comando:
  `punto_di_rotta`, `telegrafo`, `quota`, `timone`, `periscopio`, `antenna`, `designa_bersaglio`, `posizione`,
  `guida_e2`.
- **Argomenti:** nell'istruzione che riceve i parametri (la prima; la seconda per `guida_e2`) diventano
  `par(p, i, valore originale)`. Senza `p` il gestore fa esattamente cio' che faceva prima.
- **Prova:** `tools/re/prova_gestori_parametrici.lua` chiama ogni gestore con e senza parametri, con un `m688`
  finto che registra le chiamate.
- **Stato:** missioni in `data/missioni/` rigenerate il 24/10/2026. Il motore le chiama con `Missione::comando`
  (dispatcher) e `Missione::applica` (ordini del giocatore).

Il livello classico serve a giocare le missioni originali così come sono. Le missioni nuove dell'editor e la
versione Enhanced delle originali useranno un'API di livello più alto (dottrina, sensori realistici),
documentata a parte quando il motore avrà l'IA.

## 7. Validazione

- `tools/lua/src/luac -p data/missioni/*.lua` (sintassi, Lua 5.4.7 compilato nel progetto);
- `tools/re/missioni_lua.py <radice> --uscita CARTELLA` scrive le missioni convertite altrove, per confrontarle
  con `data/missioni/` senza toccarle;
- `tools/lua/src/lua tools/re/prova_gestori_parametrici.lua data/missioni/SCENE*.lua` (gestori del giocatore con
  i parametri);
- `tools/re/missioni_lua.py <radice> --verifica`:
  - conta le unità navali che cadono in acqua nella heightmap del teatro;
  - esegue `tools/re/verifica_missioni.lua`, che carica ogni file in un ambiente isolato con un `m688` finto,
    controlla che ogni script citato (gestori, timer, armi, comandi del giocatore) esista e che le
    probabilità sommino a 1, ed esegue ogni script 200 volte con condizioni casuali.

Stato al 23/10/2026: 22 file, 0 errori.
- **Nel motore** (`engine/missioni/`, `melchior688 --missione <SCENA>`): runtime del livello classico con tutte le
  funzioni m688; le prove fanno girare le 22 scene per 15 minuti senza errori ne' funzioni mancanti (le
  approssimazioni sono elencate in `engine/README.md`, sezione «Missioni»).
- **Fondale (23/10/2026, dopo la ricollocazione e le correzioni):** in tutte le 22 missioni le unità navali
  sono in acqua e dentro la carta, e i sottomarini hanno fondale ≥ quota + 10 m. Le correzioni sono:
  - Fær Øer: fino a 10 unità e 99 punti per missione, al massimo 2,2 km;
  - Kunashir: 3 unità, compreso il 688 in una variante, spostato di 12,6 km;
  - Baltico: 2 unità e 33 punti, al massimo 2,5 km.
- **Baltico, acque basse.** Nella baia di Pomerania (10–17 m) i sottomarini hanno come seconda scelta un
  fondale di 15 m, e due posizioni restano dove sono, in 16–17 m d'acqua con quota 6–7 m.
- Nell'angolo della carta originale (0, 6400) l'originale «parcheggia» le unità che entrano più tardi con un
  timer. Non si correggono.
- **Angoli (24/10/2026).** Rotte in gradi bussola e virate con il positivo a dritta, convertite dagli angoli
  matematici dell'originale. Il runtime ferma la virata sulla rotta ordinata come l'originale. Le navi partono in
  acqua di almeno pescaggio della classe + 3 m, con lo stesso margine nel raggio di ±400 m. L'effetto sugli esiti è
  in `engine/README.md`, «Danni nelle missioni»: 22 esiti su 110 cambiano, a giocatore passivo.

## 8. Classi da aggiungere al database

Le missioni usano chiavi non ancora presenti in `data/motore/classi.toml` (elenco per file in
`classi_mancanti`, aggiornato a ogni rigenerazione): al 23/10/2026 `alfa`, `akula`, `biologico`, `mk48`,
`type53`, `charlie2`, `trafalgar`, `foxtrot`, `typhoon`, `oscar`.
Il tipo 08h resta `koni` (il nome del gioco), ma lo sprite dell'originale è una Nanuchka: la chiave
`nanuchka` è già nel database, se si preferisce la sagoma.
