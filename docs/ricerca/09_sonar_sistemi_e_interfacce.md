# 09 — Sonar: sistemi, interfacce, strumenti dell'operatore e libreria delle firme

Progetto: remake realistico di *688 Attack Sub* (1989).
Scopo: base di ricerca per la **revisione completa della stazione Sonar (S3+)** chiesta dall'utente (`docs/11_comandi_e_stazioni.md` §11): cascate che si leggano come quelle dei programmi per radio SDR, una vasta libreria di firme acustiche, un mare più vario da ascoltare, e le funzioni dei sonaristi veri (annotazioni, slot, confronto e altro).

Convenzioni come in `02_acustica_oceanografia_dottrina.md` §0:
- **[alta]** = fonte primaria o autorevole verificata, oppure fisica standard.
- **[media]** = fonte aperta credibile ma secondaria, oppure calcolo dell'autore su dati pubblicati.
- **[bassa]** = forum, giochi, fonti divulgative, interpolazioni. **STIMA DI GIOCO** = nessun dato pubblico: parametro di taratura.
- Le sigle tra parentesi quadre rimandano alla bibliografia (§7). Le sigle di `02` ([U83], [Waite], [Cote], [NRC03], [W62], [Front21], [GZ94], [Milne72], [BQQ5], [TB16], [PM04]) e di `04` ([NC-BQR15], [RU-SKAT]) valgono anche qui e non sono ripetute.
- **Epoca** di ogni funzione: **1989** = ciò che un 688 Flight I/II o un battello sovietico di 3ª generazione aveva nel periodo del gioco; **moderno** = dagli anni '90 in poi (A‑RCI, BYG‑1, Sonar 2076, Virginia).

**Non ripetuto qui** (già in altri dossier): equazione del sonar, DT e guadagno di elaborazione (`02` §1.2); principio di LOFAR, DEMON e BTR (`02` §2.7); frequenze d'asse e di pala (`02` §2.2); livelli irradiati per classe (`02` §2.6); rumore ambiente di Wenz/Knudsen, pioggia, ghiaccio, gamberi e balene in sintesi (`02` §4); sonar attivi e loro frequenze (`02` §1.5); schiere rimorchiate (`04`).

---

## 0. Punto di partenza: cosa c'è già nel motore

**Motore** (`engine/nucleo/include/nucleo/analisi_acustica.hpp`, modulo puro, un istante per chiamata):
- `righe_emesse`: tonali dei macchinari per classe e velocità, più l'elica (asse, pala, 2ª armonica; più forti in cavitazione; −10 dB se posato).
- `spettro_fascio`: LOFAR di un fascio (continuo + righe spostate per Doppler, risposta del fascio con lo specchio della schiera), S/N per bin.
- `cascata`: una riga BTR su N fasci (72 × 5°), con settori ciechi e grana di 1,5 dB.
- `demon` / `leggi_demon`: inviluppo 2–8 kHz, righe d'asse e di pala, numero di pale.
- `righe_osservate` + `confronta_firma`: picchi sopra soglia confrontati con le righe dei macchinari della banca dati (tolleranza bin o 0,6% per il Doppler), punteggio e softmax.

**Catalogo** (`data/motore/acustica.toml`, generato da `tools/pipeline/acustica.py`): 40 firme. Sottomarini (688, Sturgeon, Trafalgar, Victor III, Alfa, Akula, Oscar, Charlie II, Typhoon, Foxtrot, Kilo, generico), navi militari (Perry, Kidd, Spruance, Krivak II, Kara, Slava, Koni, Nanuchka, Kirov, Iowa, Nimitz, Sacramento), tre civili (mercantile, cargo, petroliera), una nave generica, **un solo biologico** (righe a 20 e 40 Hz), l'esca ADC Mk 2 e 10 siluri. Ogni firma ha `punti` (livello a banda larga per velocità), `righe = [hz, dB relativi, tipo, v_min, v_max]` con `TipoRiga ∈ {rete, pompa, turbina, riduttore, ausiliario, diesel, elica, biologico}`, pale, assi, giri per nodo, nazionalità, tipo d'unità, TS.

**Stazione S3 di oggi** (`docs/11` §11): BTR a 72 fasci da 5°, una riga ogni 2 s, 5 min di storia; LOFAR del fascio puntato, bin 0,1 Hz, 10–410 Hz; DEMON; ANALIZZA con le prime tre ipotesi; scelta del sensore.

**Cosa manca** rispetto alla richiesta: sorgenti non navali (il mare è quasi muto oltre alle navi), sorgenti impulsive e a schema temporale (canti, treni d'impulsi, eventi), strumenti dell'operatore (cursori, tracker, annotazioni, slot, confronto), resa visiva regolabile (normalizzazione, mappe di colore, zoom), ascolto.

---

## 1. Sistemi reali per epoca

### 1.1 Stati Uniti

| Sistema | Epoca | Cosa sappiamo (aperto) | Conf. / fonte |
|---|---|---|---|
| **AN/BQQ‑5** (A–E) | 1976–anni 2000; su 637 e 688 | Sonar "passivo e attivo di ricerca e attacco a bassa frequenza" di IBM; sfera BQS‑11/12/13 di prua + schiera conforme di scafo + schiera rimorchiata. Successore digitale del BQQ‑2: il calcolatore sceglie gli idrofoni e forma i fasci, con meno rumore interno della commutazione manuale. Una versione B con console di visualizzazione migliorate e l'elaborazione integrata della schiera rimorchiata era in acquisto nel 1981; il personale di guardia scendeva a circa quattro operatori. | [alta] [BQQ5]; [media] [USNI81] (via estratto) |
| BQQ‑5E + TB‑29 + CCS Mk 2 ("QE‑2") | OPEVAL 1998 | Versione finale; ECP 7001 con reiezione delle interferenze dell'attivo LF, elaborazione di due schiere, "Full Spectrum Processing". | [alta] [BQQ5] |
| **AN/BQR‑15 + BQR‑23** | dal 1974 (SSBN) | Processore della schiera rimorchiata: almeno **64 canali LOFAR** in tempo reale, presentazioni in formato **LOFAR** e **frequenza‑azimut** (FRAZ); la versione BQR‑23A aggiunge **tracker a interpolazione di rilevamento**. | [media] [NC-BQR23] |
| TB‑16 / TB‑23 | 1989 | Vedi `04` §1. Strumento a banda stretta (LOFAR). | `04` |
| **AN/BSY‑1** (ex SUBACS) | 688I da SSN‑751 (1988) | Sistema integrato sonar + direzione del tiro. In 1989 è "marginale" (`01` §1). | [media] [MP-BSY1]; `01` |
| **AN/BQQ‑10 (A‑RCI)** | dal 1997 circa | Conversione per fasi di BQQ‑5, BQQ‑6 e BSY‑1 a processori commerciali e architettura aperta; fasi I–II sulla schiera rimorchiata, fase III sulla sfera; poi cicli biennali di software (**APB**) e di hardware (**TI**). Tra gli algoritmi citati: tracciamento con **integrazione delle righe di frequenza** (FLIT) che usa il Doppler delle tonali per le soluzioni di tiro sui battelli silenziosi. I tracker producono rilevamento, angolo d'arrivo e frequenza. | [alta] [BQQ5]; [media] [WP-BQQ10]; [media] [DOTE] |
| **AN/BYG‑1** | moderno (688, Seawolf, Virginia) | Sistema di comando e controllo: fusione dei sensori (sonar, ESM, radar, periscopio, navigazione, comunicazioni), **TMA**, gestione dei contatti ad alta densità, controllo delle armi; stessi cicli APB/TI. | [alta] [GD-BYG1] |
| **Virginia: LAB, LWWAA** | dal 2010 circa | La sfera ad aria è sostituita dalla **Large Aperture Bow**, schiera a ferro di cavallo "con acqua dietro" (parte passiva + parte attiva a media frequenza). Sui fianchi la **Lightweight Wide Aperture Array**: sei pannelli a fibra ottica, pensata per localizzare i diesel‑elettrici silenziosi (distanza passiva per triangolazione lungo lo scafo). | [media] [VA-LAB] |

### 1.2 URSS

| Sistema | Epoca | Cosa sappiamo | Conf. / fonte |
|---|---|---|---|
| **MGK‑400 "Rubikon"** (NATO "Shark Gill") | anni '70–'80; Kilo, Victor I/II ammodernati, altri | A semiconduttori; dati di portata riportati da fonti secondarie (sottomarino a 0,05 Pa/Hz a 16 km, nave a 10 Pa/Hz a 100 km) senza condizioni. La versione MGK‑400EM traccia automaticamente fino a 12 bersagli (moderno). | [media] nomi; [bassa] numeri [MP-MGK400] |
| **MGK‑500 "Skat"**, **MGK‑503 "Skat‑KS"** | dal 1979; Victor III, due Sierra, Typhoon | Su circuiti integrati sovietici; **quattro calcolatori "Karat"**: due per il rilevamento a bassa frequenza del rumore, **uno dedicato alla classificazione** del bersaglio, uno condiviso. Skat‑KS aggiunge la schiera flessibile rimorchiata con portata 2–3 volte il Rubikon. | [alta] [RU-SKAT] |
| **MGK‑540 "Skat‑3"** | prove 1986–87, revisioni 1988; Akula, Sierra II, Oscar | Elaborazione digitale; antenna di prua, **due antenne laterali** a sviluppo verticale, schiera rimorchiata. | [alta] [RU-SKAT] |

Conseguenza per il gioco: nel 1989 un Akula o un Victor III ha un **classificatore automatico** (calcolatore dedicato) e, sull'Akula, antenne laterali; l'IA sovietica può quindi classificare per conto suo, ma con risoluzione LOFAR più grossolana (`sovietico_3gen`, bin 0,2 Hz, è coerente). [media]

### 1.3 NATO e moderni europei

- **Regno Unito, Sonar 2076** (Thales; dagli anni '90, Trafalgar ammodernati e Astute): suite integrata di schiere di prua, di falsatorre, di fianco e rimorchiata; processori commerciali aggiornabili; BAE dichiara **circa 13 000 idrofoni** (2010). Quattro Trafalgar (*Torbay*, *Trenchant*, *Talent*, *Triumph*) l'hanno ricevuta. Il successore **Sonar 2176** è stato presentato al DSEI 2025. [media] [WP-2076]; [Thales-2076]
- Nel 1989 i Trafalgar avevano la generazione precedente (Type 2020 di prua, schiera rimorchiata 2026 con processore 2046): **nomi da verificare** su Friedman 1989. [bassa]
- La Royal Navy forma ancora i sonaristi con corsi di **analisi aurale** di base e avanzata (§3.8). [media] [RN-aural]

### 1.4 Quadro sintetico per epoca (per scegliere cosa mettere nel gioco)

| Funzione | 1989 (688/BQQ‑5, Skat) | Moderno (A‑RCI/BYG‑1, 2076) |
|---|---|---|
| BTR a banda larga | sì, monocromatico | sì, a colori, più bande, più schiere |
| LOFAR per fascio | sì; molti canali (BQR‑23: ≥ 64) | sì; tutte le schiere, tutti i fasci |
| FRAZ (frequenza‑azimut) | sì sulla rimorchiata (BQR‑23) | sì |
| DEMON | probabile (concetto noto) ma non documentato apertamente per il BQQ‑5 [bassa] | sì |
| Tracker automatici | a banda larga e a interpolazione di rilevamento (BQR‑23A) [media] | tracker per ogni schiera, banda larga e stretta, fusione |
| Classificazione automatica | calcolatore dedicato su Skat [alta]; USA: assistita, umana [bassa] | aiuti automatici, allarmi, librerie digitali |
| Colore | studi 1979–1990 (TRACOR, Kraiss, NUSC) [media]; in flotta raro | standard |
| Gestione contatti e TMA | carta, MATE, Mk 117 | BYG‑1 integrato |

---

## 2. Presentazioni e flusso di lavoro

### 2.1 BTR a banda larga (*Bearing‑Time Recorder*)
- **Cosa mostra:** l'energia totale ricevuta dalla schiera per rilevamento nel tempo, a cascata che scorre dall'alto in basso; l'operatore ascolta i rilevamenti dove il fondo si alza per decidere se è un bersaglio. [media] [Amick20]
- **Bande:** più BTR per banda (per es. bassa, media, alta) aiutano a distinguere una nave che cava (forte in alto) da un battello silenzioso (solo in basso). Il motore lo prevede già (`banda_hz`, un'ottava). [media, concetto]
- **Lettura:** la pendenza della traccia è il *bearing rate* (`02` §5.1); una traccia che si allarga in rilevamento indica vicinanza o sorgente estesa; incroci di tracce = rischio di scambio dei tracker (dove il LOFAR aiuta a separare, §2.2). [alta, concetto] [GI10]
- **Valori di gioco:** riga ogni 1–4 s; storia visibile 10–30 min con compressione temporale selezionabile (×1, ×4, ×16, cioè 5 min, 20 min, 80 min sullo stesso schermo); 72–180 fasci. **STIMA DI GIOCO**

### 2.2 Banda stretta: LOFARgram, FRAZ, integrazione delle righe
- **LOFARgram:** frequenza in ascissa, tempo in ordinata, intensità come luminosità; le tonali sono linee. In origine su carta elettrostatica: ogni passata dello stilo era uno spettro, la carta avanzava nel tempo. Le righe che si piegano indicano cambi di velocità o di rotta (Doppler). Nella rete SOSUS c'era un registratore per ogni fascio, e gli analisti (OT) leggevano la carta; la carta fu sostituita da stazioni digitali solo dal 1992. [media] [LOFAR-wiki]; [SOSUS-wiki]
- **FRAZ:** spettro a due dimensioni, frequenza × rilevamento (per un istante, oppure integrato): una tonale è un punto, che nel tempo disegna una traccia in rilevamento. Era già una presentazione operativa sulla rimorchiata (BQR‑23). Serve per capire **a quale fascio appartiene una riga**, cosa che il LOFAR di un solo fascio non dice. [media] [NC-BQR23]; [PAT-FRAZ]
- **Integrazione automatica delle righe (ALI):** sommare gli spettri nel tempo fa crescere l'energia di una riga stabile più del rumore, perché la riga è ordinata e il rumore casuale; serve a tirare fuori tonali deboli. Prezzo: le righe che derivano (Doppler, cambio di velocità) si spalmano. [media] [PAT-ALI]
- **Valori:** larghezza del bin `b = 1/T` (T = durata della FFT). 0,1 Hz ⇔ 10 s; 0,05 Hz ⇔ 20 s; 0,01 Hz ⇔ 100 s. Con finestra di Hann la banda equivalente è circa 1,5 bin. Guadagno su un rivelatore a banda W: `10·log(W/b)` (`02` §1.2). [alta, fisica]
- **Banda di zoom (verniero):** gli operatori scorrono e ingrandiscono le pagine del narrowband per cercare relazioni fra le righe [media] [Amick20]. Nel gioco: una banda larga (10–2000 Hz a 1 Hz), una media (10–410 Hz a 0,1 Hz, quella di oggi) e un **verniero** di 2–10 Hz attorno a una riga a 0,01–0,02 Hz (misura fine del Doppler). **STIMA DI GIOCO**

### 2.3 DEMON
- Già descritto in `02` §2.7 e nel motore. Aggiunte utili: presentarlo **a cascata** come il LOFAR (0–100 Hz in ascissa) e non solo come lettura istantanea, così si vede quando il bersaglio cambia giri; e indicare la **profondità di modulazione** (quanto è "battuto" il rumore) come indizio di cavitazione. [media, concetto]
- Il DEMON vale anche per siluri e motoscafi (alta velocità di rotazione, molte pale): in un siluro la riga d'asse sta a decine di Hz. [bassa]

### 2.4 Tempi d'integrazione e persistenza

| Presentazione | Integrazione tipica | Persistenza/storia | Nota |
|---|---|---|---|
| BTR | 1–10 s per riga | 10–60 min | integrazione lunga = più portata ma tracce mosse sfocate [media, [U83]] |
| LOFAR | T_FFT 5–20 s; 1–8 righe sommate | 15–60 min | `02` §2.7 |
| ALI / integrazione lunga | 2–10 min | — | solo righe stabili [media] |
| DEMON | 2–10 s | 5–15 min | — |
| Ascolto | tempo reale | — | l'orecchio integra ~0,1–1 s [bassa] |

La regola `DT = 5·log(d·W/t)` (`02` §1.2) dice che quadruplicare il tempo d'integrazione vale circa 3 dB: è il prezzo/beneficio che il giocatore deve sentire quando cambia la scala dei tempi. [alta, formula]

### 2.5 Normalizzazione e guadagno della presentazione
- **Perché serve:** il fondo varia con la frequenza (pendenza di Wenz), con il rilevamento (traffico, baffles) e nel tempo. Senza normalizzare, lo schermo è dominato dalle zone rumorose e le tracce deboli scompaiono.
- **Normalizzatore a finestra divisa (SWN):** per ogni cella si stima il rumore locale con la media delle celle vicine, escludendo una piccola zona centrale (la "fessura") dove può stare il segnale, e si divide la cella per quella stima. [media] [Morris]; [SWN-wiki]
- **Varianti passive classiche** (Struzinski e Lowe, 1984): *two‑pass split window* (TPSW: prima passata, tosatura dei valori oltre r volte la media, seconda passata), *order‑truncate average* (OTA), *split‑average‑exclude‑average*. Il TPSW è il riferimento per LOFAR e BTR; il difetto noto è la sensibilità alla scelta della finestra e della soglia. [media] [SL84]; [JAC13]
- **Parametri di gioco proposti:** LOFAR: finestra di 2×16 bin, fessura di 2×2 bin, soglia r = 2,0; BTR: finestra di 2×6 fasci, fessura di 2×1. Così una tonale isolata resta brillante, mentre un tappeto largo (traffico) si appiattisce: è anche un **effetto collaterale realistico**, perché una sorgente molto larga può essere normalizzata via. **STIMA DI GIOCO**
- **Guadagno e AGC visivo:** oltre al normalizzatore, una manopola di **luminosità/soglia** e una di **contrasto** (gamma); Spectrum Lab chiama "visual AGC" la regolazione automatica della luminosità al variare del rumore. [media] [SpecLab]

### 2.6 Colore o fosforo verde
- Un osservatore distingue in modo affidabile solo **circa otto livelli** di luminosità su una scala di grigi: è il motivo per cui la Marina USA studiò il colore già nel 1979 (rapporto TRACOR). [media] [TRACOR79]
- Kraiss e Küttelwesch (1984) confrontarono codici di colore e scale di grigi su LOFARgram: le prestazioni (curve ROC) cambiano in modo significativo tra i codici, e **alcuni codici di colore peggiorano molto quando il fondo sale**, altri restano stabili. Non c'è un vincitore assoluto: conta il disegno della scala. [media] [Kraiss84]
- NUSC (Salafia, 1988 e 1990) studiò **scale di colore regolabili dall'operatore** per il sonar passivo. [media] [HFS91]
- **Per il gioco:** modalità **1989** = monocromatica (verde fosforo o ambra, 8–16 livelli percettivi, leggera persistenza del fosforo); modalità **moderna** = mappe percettive uniformi (viridis, cividis per i daltonici, turbo per il massimo contrasto) [alta] [Viridis]; [Turbo]. Evitare l'arcobaleno "jet", che crea falsi bordi. [alta]

### 2.7 Tracker e designazione dei contatti
- **Assegnazione:** quando un operatore conferma una rilevazione, il sistema comincia a tracciare la sorgente e ne conserva la storia; si assegnano tracker **da ogni schiera** che correla con la rilevazione (sfera, scafo, rimorchiata), poi si fondono. [media] [Amick20]
- **Tracker a banda stretta:** seguono le singole righe in frequenza e rilevamento; separano bersagli vicini in rilevamento e permettono la TMA durante gli incroci, quando la banda larga da sola non basta. [media] [GI10]; FLIT [media] [WP-BQQ10]
- **Nomi:** nelle memorie, nei romanzi tecnici e nei simulatori il contatto sonar è **"Sierra" + numero** (S1, S2…, cioè "Sierra uno"), progressivo nella pattuglia; un contatto tenuto da più sensori diventa **"Master"** (M1…); per analogia **Victor** (visivo), **Romeo** (radar), **Echo** (ESM). **Nessuna fonte ufficiale aperta trovata**: uso diffuso ma da trattare come convenzione di gioco. [bassa] [CW-sierra]
- **Per il gioco:** S‑numero assegnato dal giocatore o dall'operatore IA alla conferma; il tracker conserva rilevamento, frequenze delle righe agganciate, ora di prima rilevazione, schiera; un contatto perso resta in elenco come "PERSO" con l'ultima ora; due tracker sullo stesso bersaglio si possono **unire** ("S3 e S5 sono la stessa sorgente").

### 2.8 Ruoli a bordo
- **Operatori:** davanti alle console (banda larga, banda stretta, schiere diverse). La divisione sonar di un SSN USA conta circa **14 uomini**, metà di guardia per volta su turni di 8 ore; i cicli di ricerca durano fino a 6 ore. [media] [Amick20]
- **Supervisore sonar** (di solito il più esperto): sta dietro agli operatori come "secondo paio d'occhi", è il collegamento tra i dati acustici grezzi e l'ufficiale d'attacco, e calcola una **soluzione indipendente** per controllare quella del gruppo di tiro (*Fire Control Tracking Party*). [media] [Amick20]
- **Specialista ACINT** (*Acoustic Intelligence*): programma della Marina USA dal 1962 per riconoscere le navi straniere dal suono; pochi specialisti imbarcati in missioni speciali; le firme raccolte entrano nella banca dati e tornano a bordo come aiuto alla classificazione. [media] [ACINT]
- **Comando:** riceve il quadro su circuiti a telefono autoalimentato; la riunione quotidiana informa il comandante sull'ambiente e sugli incontri attesi; i contatti finiscono su un *fusion plot* in centrale. [media] [Amick20]. Gli ufficiali giudicano il **riassunto dei dati di contatto** come l'informazione sonar più importante per chi ha la manovra, prima delle presentazioni grezze. [media] [ADA135857]

### 2.9 Classificazione: passi e parole
- **Passo 1, impressione iniziale:** categorie larghe: biologico, sismico, mercantile, aereo, peschereccio, nave militare, sottomarino; più transitori e impulsi attivi. [media] [Amick20]
- **Passo 2, analisi:** circa **30–45 s** per identificare tipo di motore, pompe idrauliche in linea e configurazione dell'elica; conferma o corregge il passo 1. Lo spostamento Doppler registrato nel tempo aiuta a localizzare. [media] [Amick20]
- **Livelli di confidenza** (lessico ASW NATO/USN): **NONSUB**, **POSSUB** (possibile sottomarino), **PROBSUB** (probabile), **CERTSUB** (certo). Una prima rilevazione subacquea genera un rapporto POSSUB; il passivo da solo non dice la marina né il tipo. [bassa] (glossari di studio, non pubblicazione ufficiale) [POSSUB]. Per i contatti di superficie la stessa scala si usa in modo naturale come "possibile/probabile/certo mercantile". **STIMA DI GIOCO**
- **Indizi non infallibili:** nessuna firma è una prova assoluta sottomarino/non‑sottomarino. [media] [ADA066750]
- **Rapporto al comando (formula di gioco):** «Comandante, sonar: nuovo contatto, rilevamento 245, designato Sierra 3, classificato possibile sottomarino, rilevamento in deriva a sinistra, debole». Elementi: rilevamento, designazione, classificazione con confidenza, tendenza (deriva, lento/veloce), qualità, schiera. **STIMA DI GIOCO** (gli elementi derivano da [Amick20] e da [ADA135857]; la frase esatta non è documentata).

---

## 3. Strumenti dell'operatore

### 3.1 Cursori e marcatori
- **Cursore di lettura:** frequenza, livello (S/N), ora, rilevamento sotto il puntatore. In Spectrum Lab i cursori "agganciano" il picco più vicino. [media] [SpecLab]
- **Marcatori a coppia (delta):** SDRangel ne ha due sul grafico: il primo dà frequenza e livello, il secondo la **differenza** dal primo; sulla cascata un marcatore dà tempo e frequenza, il secondo Δt e Δf. Esattamente ciò che serve per misurare una deriva Doppler o la durata di un evento. [alta] [SDRangel]
- **Marcatori che seguono il picco o il massimo** (SDRangel: "Cur" e "Max", con ricerca dei picchi in ordine di ampiezza). [alta] [SDRangel-mk]
- **Cursore armonico** (proposta, comune nei software di analisi vibrazioni): si fissa una fondamentale f₁ e compaiono marcatori a 2f₁, 3f₁…; se le righe cadono sotto i marcatori, appartengono alla stessa famiglia. Varianti: **bande laterali** (f ± k·f_s, tipiche della modulazione d'asse) e **rapporto** (marcatore a f·p/q). [media, concetto]
- **Proprietà chiave:** il Doppler moltiplica **tutte** le frequenze della stessa sorgente per lo stesso fattore `(1 + v_r/c)`, quindi i **rapporti** tra le righe non cambiano. Un cursore armonico che "scala" insieme alla fondamentale identifica la famiglia anche con il bersaglio in moto, e il confronto per rapporti è robusto al Doppler. [alta, fisica]

### 3.2 Annotazioni sulle cascate
- **Modello SDR:** SDRangel ha **marcatori di annotazione** con testo (fino a 36 caratteri), colore, frequenza d'inizio o centro e larghezza di banda, quattro stati di visibilità, ordinamento e **import/export CSV** (inizio, larghezza, testo, mostra, R, G, B). [alta] [SDRangel-mk]
- **Modello bioacustico:** Raven Pro salva le **selezioni** (rettangoli tempo × frequenza) in tabelle di testo con inizio, fine, durata, frequenze minima, massima e banda; dalla 1.5 ha uno strumento di revisione a griglia per classificare a mano le selezioni [media] [Raven]. PAMGuard segna i suoni sullo spettrogramma con un rettangolo tempo‑frequenza scritto in una tabella del database (modulo "Spectrogram Annotation"). [media] [PAMGuard]
- **Per il gioco:** tre tipi. (1) **riga** (verticale sul LOFAR, con S‑numero e nota: «S3 pompa?»); (2) **rettangolo** tempo × frequenza o tempo × rilevamento («canto balenottera», «transitorio metallico»); (3) **evento** (orizzontale, ora: «virata a 090», «S2 accelera»). Le annotazioni **scorrono con la cascata**, sopravvivono al cambio di scala dei tempi e finiscono nel replay e nel registro di fine missione. Alcune si creano da sole (manovre proprie, filata della schiera, lanci). **STIMA DI GIOCO**

### 3.3 Congelare e salvare (slot)
- **Modelli SDR:** pausa che congela la presentazione per piazzare i marcatori (SDRangel *Play/Pause*); memorie **M1 e M2** che conservano uno spettro (o la sua media), si mostrano sovrapposte, si sottraggono o si sommano, si esportano in CSV; esportazione dell'immagine; **buffer di scorrimento** della cascata con barra laterale per tornare indietro. Spectrum Lab salva periodicamente il contenuto della cascata. [alta] [SDRangel]; [media] [SpecLab]
- **Per il gioco: 8 slot** per missione. Uno slot contiene: il segmento di cascata (LOFAR, DEMON o BTR) tra due istanti, la banda, il sensore, il fascio/rilevamento, le annotazioni, lo spettro medio del segmento, la lista delle righe osservate, l'ora di gioco e il contatto associato. Si crea congelando la presentazione e trascinando un rettangolo. **STIMA DI GIOCO**

### 3.4 Confronto con la libreria e tra slot
- **Sovrapposizione del "piano delle righe":** come i programmi SDR sovrappongono il piano delle bande o i segnalibri sulla scala delle frequenze (SDR#: *Frequency Manager* con «mostra sullo spettro» [media] [SDRsharp]; SDR++: *bandplan*), il giocatore sovrappone al LOFAR **le righe della classe scelta in libreria**, scalate da un **fattore Doppler regolabile** (o agganciate a una riga di riferimento). Se le linee cadono sulle tracce, la classe è compatibile. [media, concetto]
- **Affiancato:** due pannelli con lo slot e il gram di riferimento della libreria alla stessa scala di frequenza.
- **Differenza:** SDRangel offre modi "differenza dalla memoria M1/M2" in dB [alta] [SDRangel]: utile per vedere **cosa è cambiato** in un contatto (una pompa accesa, un cambio di giri) tra due slot.
- **Punteggio di somiglianza:** il motore ha già `confronta_firma`; nella stazione serve mostrare **perché** (quali righe concordano, quali mancano, quali sono in più) invece del solo numero.

### 3.5 Famiglie armoniche, conteggio di asse e pala
- **Righe d'elica:** asse `f_s = RPM/60`, pala `f_BR = Z·f_s` con armoniche (`02` §2.2). Nel DEMON il rapporto tra le due righe forti dà Z.
- **Conteggio a orecchio (storico):** nell'era del periscopio e del dopoguerra l'operatore contava i battiti dell'elica (*turn count*) e convertiva con tabelle in velocità secondo il tipo di nave; il supervisore riconciliava con gli altri dati. [media] [GUPPY]. Il corso aurale della Royal Navy insegna ancora a contare i giri d'asse e le pale. [media] [RN-aural]
- **Righe di rete:** 50/60 Hz e armoniche, 400 Hz (`02` §2.2). Un generatore sotto carico deriva leggermente: la riga di rete non è un riferimento perfetto. [bassa]
- **Elica a passo variabile (CPP):** con giri d'asse fissi la velocità cambia variando il passo; le tonali cambiano d'intensità con il passo (in uno studio, massime al 50%, dove il larga banda è minimo). Indizio di classificazione: **la nave accelera ma la riga d'asse non si muove** → elica a passo variabile (molti pescherecci, traghetti, alcune navi militari). [media] [CPP22]

### 3.6 Doppler e passaggio al punto di minima distanza (CPA)
- `f(t) = f₀·(1 + v_r(t)/c)`. Passivo: 0,103 Hz/kt a 300 Hz (`02` §1.2).
- **Curva a S:** per una sorgente in moto rettilineo la frequenza scende da `f₀(1 + v/c)` a `f₀(1 − v/c)` passando per f₀ al CPA, dove la **pendenza è massima**: `|df/dt|_CPA = f₀·v²/(c·R_CPA)`, quindi `R_CPA = f₀·v²/(c·|df/dt|)` (v = velocità relativa). [alta, fisica; derivazione dell'autore, coerente con [Felgate]]
- Esempio: f₀ = 100 Hz, v = 10 kt (5,14 m/s), R_CPA = 1000 m → salto totale 0,69 Hz, pendenza al CPA 0,0018 Hz/s, cioè 0,1 Hz in circa 57 s: visibile solo con il verniero (§2.2). [media, calcolo]
- **Strumento di gioco:** due marcatori sulla riga (prima e dopo il CPA) → Δf totale e quindi `v ≈ c·Δf/(2f₀)`; il marcatore di pendenza al CPA → R_CPA. È la "telemetria passiva" che un buon sonarista fa sul gram, ed entra nella TMA (§3.10).
- **Correzione per il moto proprio:** il proprio moto sposta tutte le righe di tutti i contatti in funzione del rilevamento; il sistema lo compensa se si conosce la propria velocità. Nel gioco: opzione "Doppler proprio compensato" sempre attiva in modalità moderna, manuale in modalità 1989. **STIMA DI GIOCO**

### 3.7 Ascolto (aurale)
- **Cosa si ascolta:** il fondo del mare; si cercano **transitori metallici secchi**, spostamenti delle tonali, sonar attivi lontani; gli algoritmi segnalano e l'operatore verifica. [media] [Amick20]
- **Cosa si riconosce a orecchio** (programma del corso RN di analisi aurale di base): sibilo (*whine*), *futter* (battito modulato), sfregamento d'asse (*shaft rub*), rombo di diesel, **rumore di rete a strascico** (*trawl noise*), cavitazione; conteggio di giri e pale. Il corso avanzato aggiunge sonar attivi, classificazione dei contatti e collegamento della firma al tipo di nave. [media] [RN-aural]
- **Storia dei biologici:** nella Seconda guerra mondiale gli operatori sentivano martellamenti e colpi metallici inspiegati; dal 1946 Marie Poland Fish catalogò per la Marina i suoni degli animali (oltre 180 specie entro il 1954) e le registrazioni servirono ad addestrare i sonaristi. [media] [Fish]
- **Ascolto accelerato:** i suoni sotto ~30 Hz (balenottere, T‑phase, ghiaccio) sono poco udibili; NOAA PMEL li pubblica **accelerati da 3 a 16 volte**. Nel gioco: tasto "riproduci ×4/×16" sul segmento salvato. [alta, prassi] [PMEL-ice]
- **Per il gioco:** audio sintetico del fascio puntato: rumore colorato secondo lo spettro del fascio + sinusoidi delle tonali più forti (traslate di +1 ottava se sotto 50 Hz, opzione) + modulazione d'ampiezza DEMON (battito dell'elica) + campioni per gli eventi (clic, canti, colpi). Filtri passa banda come sul pannello di oggi. **STIMA DI GIOCO**

### 3.8 Registrazioni
- A bordo si registra per l'analisi successiva e per l'ACINT [media] [ACINT]. Nel gioco il **registratore** conserva l'intera storia acustica (già deterministica per il replay, `docs/11` §11) e permette di tornare indietro su qualunque presentazione: la cascata non si "perde" uscendo dalla stazione.
- **Gioco d'intelligence:** registrare a lungo e da vicino una classe rara (per es. un Akula in pattugliamento) aggiunge alla libreria del giocatore righe che prima erano ignote (§6.1, punto 12).

### 3.9 Errori tipici dell'operatore (da rendere possibili)
- Biologici scambiati per tonali (balenottera comune a 20 Hz, `02` §4.5), scambio di tracker negli incroci, righe del proprio battello (rumore proprio non tolto), righe di un mercantile lontano dentro il fascio di un sottomarino, specchio della schiera (`04`), alias di righe forti in bin vicini. [media, concetti]

### 3.10 Consegna alla TMA
- Ciò che passa dal sonar alla direzione del tiro: serie di **rilevamenti** con ora e qualità, **frequenze** delle righe (Doppler), **giri/velocità** dal DEMON o a orecchio, classificazione. [media] [WP-BQQ10]; [GI10]
- Le storie di rilevamento con le frequenze delle righe permettono la TMA **senza manovra propria** durante gli incroci. [media] [GI10]
- Metodi manuali storici: diagramma tempo‑rilevamento e "strisce di velocità" di Ekelund, con un secondo gruppo che tracciava in parallelo la deriva del rilevamento [media] [GUPPY]; uomini meglio del calcolatore sulle curve complesse con rilevamenti rumorosi [media] [NSMRL716]. Il modello di TMA è in `02` §5.1 e §10.5.
- **Per il gioco:** pulsante «manda alla TMA» sul tracker (rilevamenti) e sulla riga (frequenza); la TMA riceve anche **v e R_CPA** dal §3.6 quando misurati.

---

## 4. Modello visivo: le cascate dei programmi SDR

### 4.1 Corrispondenze

| Idea SDR (programmi) | Equivalente sonar | Si adatta? |
|---|---|---|
| Dimensione della FFT (SDR#: *Resolution*; SDRangel: 64–32k) [SDRsharp]; [SDRangel] | larghezza del bin LOFAR `b = 1/T` | **sì**: è la scelta risoluzione/tempo del §2.2 |
| Finestra (Hann, Blackman‑Harris, Kaiser, flat‑top) [SDRangel] | stessa cosa | sì, ma basta un'opzione "righe strette / livelli esatti" |
| Sovrapposizione (*overlap*) [SDRangel] | righe di cascata più fitte a pari risoluzione | sì |
| Media mobile, a blocchi, **massimo**, **minimo** [SDRangel] | integrazione LOFAR/ALI; massimo per i transitori; minimo per stimare il fondo | **sì, molto** |
| Attacco/decadimento separati per spettro e cascata (SDR#) [SDRsharp]; media che agisce solo sul grafico, non sulla cascata (GQRX) [GQRX] | persistenza della traccia A‑scan sopra il gram | sì |
| Riferimento e intervallo dB, contrasto, offset, scala automatica [SDRangel]; [SDRsharp] | soglia del fondo e tetto della scala dopo il normalizzatore | **sì** (§2.5) |
| Mappe di colore e editor di palette [SDRConsole]; [SpecLab] | fosforo 1989 / percettive moderne | sì (§2.6) |
| Mantenimento del picco, fosforo con decadimento, "stroke" [SDRangel] | picco su A‑scan; persistenza del fosforo | sì |
| Zoom e spostamento (rotella, ×1–×10, ricentra) [SDRangel] | verniero, zoom in rilevamento sul BTR | **sì** |
| Marcatori singoli e delta, annotazioni con CSV [SDRangel-mk] | cursori, annotazioni, export dei dati della missione | **sì** (§3.1–3.2) |
| Segnalibri / Frequency Manager con gruppi e «mostra sullo spettro» [SDRsharp] | piano delle righe della libreria (§3.4) | **sì** |
| Griglia del tempo e etichette orarie sulla cascata [SpecLab]; asse del tempo in ora locale/UTC con buffer di scorrimento [SDRangel] | ora di gioco sulla cascata, scorrimento all'indietro | **sì** |
| Pausa/congela [SDRangel]; [SpecLab] | slot (§3.3) | sì |
| Spettro 3D [SDRangel] | — | no (bello, ma non si legge meglio) |
| Demodulazione, IQ, frequenze negative, calibrazione RF | — | no |
| Velocità della cascata (~40 righe/s, SDR Console) [SDRConsole] | 1 riga ogni 1–5 s | **no**: il sonar è 100 volte più lento; serve la compressione temporale |

### 4.2 Regole di resa proposte (STIMA DI GIOCO salvo dove indicato)
1. **Pipeline per pixel:** potenza per bin → media (modo scelto) → **normalizzatore** (§2.5) → dB sopra il fondo → mappa `[pavimento, tetto]` → gamma → palette. Il motore dà già S/N per bin (0 dB = solo rumore), quindi il pavimento predefinito è 0 dB e il tetto +18 dB.
2. **Pavimento e tetto** regolabili con due cursori (come *Range/Offset* di SDR#) e un pulsante **AUTO** che li ricalcola dai minimi e dal picco (SDRangel: minimo come media di 1/32 dei minimi, tetto 10 dB sopra il picco [alta] [SDRangel]).
3. **A‑scan sopra il gram** (lo spettro istantaneo), con traccia di **massimo** e di **minimo** commutabili, come la vista spettro + cascata delle SDR. La GQRX lascia la media solo sul grafico; la cascata resta grezza: buona scelta anche per il sonar, perché il gram è già integrato. [media] [GQRX]
4. **Scala dei tempi della cascata:** ×1/×4/×16 con decimazione per **massimo** (non per media), così un transitorio breve resta visibile quando si comprime. [alta, prassi SDRangel "Max"]
5. **Griglia del tempo:** linea ogni minuto, etichetta ogni 5 min in ora di gioco; annotazioni automatiche degli eventi propri a sinistra (§3.2).
6. **Palette:** 1989 = verde fosforo (monocromatica, 16 livelli, leggera persistenza); moderna = viridis predefinita, alternative cividis, turbo, grigio, "caldo". [alta] [Viridis]; [Turbo]
7. **Zoom:** rotella sulla scala delle frequenze (×1–×16) e sulla scala dei rilevamenti del BTR; clic con modificatore per ricentrare; la risoluzione vera cambia solo con il verniero, lo zoom puro non inventa dettaglio (SDR# avverte che lo zoom abbassa la risoluzione apparente). [media] [SDRsharp]
8. **Segnalibri delle righe** sulla scala delle frequenze: 50/60/400 Hz e le righe della classe sovrapposta (§3.4), con colore per tipo di riga (`TipoRiga`).

### 4.3 Disposizione suggerita della stazione (640×480 classica, più grande nell'Enhanced)
- **Sinistra:** BTR (rilevamento × tempo) con tracker S‑numerati e settori ciechi.
- **Destra in alto:** A‑scan del fascio scelto; **destra al centro:** LOFAR (o FRAZ con un tasto); **destra in basso:** DEMON a cascata.
- **Fascia inferiore:** lista dei contatti (S‑numero, rilevamento, classe e confidenza, tendenza), slot, pulsanti (analizza, confronta, manda alla TMA, ascolta).
- **Pagina libreria** a schermo intero: schede per classe con gram di riferimento sintetico, rapporti armonici, note aurali.

---

## 5. Libreria delle firme: categorie e caratteristiche

Livelli in dB re 1 µPa @1 m (rms se non indicato; "pp" = picco‑picco, circa 9 dB sopra l'rms per segnali continui [alta] [Hild09]).

### 5.1 Militari
- **Sottomarini per famiglia:** la libreria di oggi copre bene le classi del gioco (`02` §2.6, `acustica.toml`). Da aggiungere per varietà: **sovietici di 1ª generazione** (November, Echo II, Hotel: molto rumorosi, ottimi bersagli d'esercitazione), **Yankee/Delta I‑III** (SSBN nei bastioni), **Sierra**, **Tango** e **Kilo a snorkel** separati; NATO: **Upholder/Oberon**, **Type 209/206** (Baltico, Mediterraneo), **Agosta**, **Rubis**. Livelli [bassa], come tutta `02` §2.6.
- **Indizi di famiglia** (per la classificazione a passi): 50 Hz = sovietico, 60 Hz = USA/NATO [media]; pompe del refrigerante a due regimi (gradino) = nucleare [bassa]; diesel e cariche di batteria a snorkel = SSK [alta, concetto]; elica tandem o pump‑jet = DEMON debole o anomalo [bassa].
- **Navi di superficie:** turbine a gas (LM2500 su Spruance, Perry, Kidd) con righe di riduttore e alta frequenza; turbine a vapore sovietiche (Kara, Kirov in parte), diesel (Koni). **Prairie‑Masker** su Perry e Spruance: Masker soffia aria da cinture attorno allo scafo vicino all'apparato motore e Prairie dai bordi d'attacco delle pale; riduzioni misurate su modelli e navi civili fino a ~20 dB (Masker) e ~10 dB (Prairie). Effetto sul gram: **meno tonali, più un fruscio a banda larga**. [media] [Masker]
- **Sonar attivi:** frequenze e livelli in `02` §1.5 (SQS‑53/26 a ~3 kHz, SQS‑56 ~7 kHz, AQS‑13, DICASS, teste dei siluri 20–30 kHz). Lo SQS‑53C emette impulsi FM di **1–2 s** in **1–5 kHz** a ≥ 235 dB, fascio verticale di circa 40° inclinato di 3° in giù [alta] [Hild09]. Sul gram: segmenti brevi e periodici (intervallo di ripetizione 10–60 s secondo la scala di distanza) [bassa]; sul BTR: un lampo sul rilevamento della nave.
- **Siluri:** firme in catalogo (10). Da aggiungere come **eventi**: transitori di lancio (`02` §2.1), avvio del motore, "ping" della testa (§1.5), detonazione (formula sotto).
- **Esplosioni:** `SL = 269 + 7,53·log w` (w = libbre di TNT); una testa di Mk 46 (98 lb, 44 kg) dà circa 289 dB totali con la bolla, contenuto quasi costante tra 10 e 200 Hz; prova d'urto di 10 000 lb: 304 dB, 0,5–50 Hz. Gli impulsi delle esplosioni si sentono a scala di bacino. [alta] [Hild09]. Bombe di profondità e cariche di segnalazione (*seal bomb* 2,3 g: 205 dB, 30 ms) sono la stessa fisica in piccolo. [alta] [Hild09]
- **Transitori di bordo** (propri e nemici): porta del tubo, allagamento del tubo, lancio, sbattere di portelli, attrezzi caduti, cambio delle pompe, timoni e idroplani, soffiata delle casse (fortissima), avvio del diesel, ritiro della schiera; **"scricchiolii" dello scafo** al cambio di quota (anelli di fissaggio delle sovrastrutture non lubrificati: una sola testimonianza di forum, [bassa]). Livelli 120–150 dB, durate 0,05–5 s (soffiata: 10–60 s), **STIMA DI GIOCO**.

### 5.2 Civili

| Sorgente | Spettro e struttura | Livelli | Note per la classificazione | Fonte / conf. |
|---|---|---|---|---|
| Portacontainer moderna (54 kGT, 21,7 kt) | energia sotto 40 Hz; pala a pochi Hz | 188 dB (20–1000 Hz) | poppa 5–10 dB più forte della prua | [alta] [McKenna12] |
| Petroliera chimichiera (26 kGT) | sotto 40 Hz | 177 dB | — | [alta] [McKenna12] |
| Rinfusiera | picco attorno a 100 Hz | 180–186 dB | — | [media] [McKenna12] |
| Superpetroliera veloce | picco spettrale 10–50 Hz | fino a 195 dB re µPa²/Hz | — | [alta] [Hild09] |
| Cargo di 173 m a 16 kt | 40–100 Hz | 192 dB | — | [alta] [Hild09] |
| Peschereccio piccolo | 10–50 Hz | ~140 dB re µPa²/Hz | in deriva: righe armoniche di motore e generatore; in moto: continuo sopra ~50 Hz dall'elica | [alta] [Hild09]; [media] [Suisan95] |
| Peschereccio a strascico al lavoro | come sopra + verricelli, cavi, rete sul fondo | — | **rumore di strascico** riconosciuto a orecchio; elica CPP (§3.5) | [media] [RN-aural]; [bassa] livelli |
| Imbarcazione con fuoribordo (20 kt) | 1–5 kHz | 150–180 dB (160 a 5 kHz, terzi d'ottava) | molto locale (~10 km) | [alta] [Hild09] |
| Traghetto / nave veloce | larga banda + alta frequenza da cavitazione, rotte fisse e orari | ~175–190 dB [bassa] | Baltico: Rostock–Gedser, Sassnitz–Trelleborg (`03` A7) | [bassa] |
| Rompighiaccio | larga banda forte, colpi del ghiaccio | — | Barents, Okhotsk, Baltico d'inverno | [bassa] |
| Piattaforma di perforazione fissa | 700–1400 Hz | 184 dB | continua | [alta] [Hild09] |
| Nave di perforazione (posizionamento dinamico) | eliche di manovra, toni e larga banda | più alta delle piattaforme | — | [alta] [Hild09] |
| Ecoscandaglio multifascio profondo (EM122) | 11,5–12,5 kHz | 245 dB | fascio stretto verso il fondo | [alta] [Hild09] |
| Profilatore del sottofondo | 3–7 kHz | 230 dB | impulsi brevi regolari | [alta] [Hild09] |
| Telemetria acustica | 7–45 kHz (25–26,5 kHz HTL 300) | ≤ 190 dB | continua | [alta] [Hild09] |
| Dissuasori acustici (ADD/AHD) | 5–160 kHz | 150 / 205 dB | allevamenti, impulsi 2–2000 ms | [alta] [Hild09] |

**Ecoscandagli da pesca e da navigazione** (38, 50, 120, 200 kHz) stanno sopra la banda del sonar passivo, ma una nave che "pinga" ogni secondo a 12–50 kHz è un indizio aurale per l'intercettatore WLR‑9 (bande in catalogo 3,5–25 kHz). [media, concetto]

### 5.3 Biologici

| Specie / gruppo | Banda | Struttura temporale | Livello | Teatri del gioco / stagione | Fonte / conf. |
|---|---|---|---|---|---|
| **Balenottera comune** | impulsi discendenti 40→13 Hz, centro ~20 Hz | < 1 s; singoli, doppiette o terzine con intervalli 6–37 s (tipico 7–26 s); serie fino a 32,5 h; pause di 120–150 s ogni 10–15 min | 175–189 dB | Faroe, Norvegia, Piattaforma Celtica, Gibilterra/Mediterraneo (popolazione con due tipi d'impulso) | [alta] [DOSITS-fin]; [Watkins87]; [Miksis19]; [MedFin22] |
| **Balenottera azzurra** | 15–40 Hz | unità di 15–20 s ripetute in frasi e serie; chiamate "D" irregolari | ~189 dB | Atlantico N profondo (Faroe/Islanda), rara | [alta] [DOSITS-blue] |
| **Megattera** | 30 Hz–8 kHz | canto: sequenze ripetute di unità tonali e impulsive, cambia di anno in anno, soprattutto d'inverno nelle aree di riproduzione; chiamate d'alimentazione ritmiche di ~2,5 s a ~500 Hz alle alte latitudini | media 137 dB (113–157) | Norvegia, Barents d'estate (alimentazione); canto d'inverno solo occasionale | [alta] [DOSITS-hump]; [media] [Fournet18]; `02` §4.5 |
| **Balenottera minore** (Atlantico N) | treni d'impulsi a bassa frequenza | impulsi ~0,25 s spaziati ~1 s; treni più lunghi a sud di 40°N, rari d'estate | — | tutti i teatri atlantici | [media] [Minke14]; [NavyMinke] |
| **Capodoglio** | < 100 Hz–30 kHz, energia 5–25 kHz | clic regolari di ecolocalizzazione, "creak" rapidi alla cattura, **code** sociali | ~230 dB pp (`02`) | Faroe, Gibilterra, Mediterraneo, Okhotsk | [alta] [DOSITS-sperm] |
| **Orca** | chiamate impulsive (energia ~1–6 kHz [bassa]), fischi, clic | gruppi, sequenze di minuti | 135–176 dB rms (1–40 kHz) | Faroe, Norvegia (aringhe), Okhotsk | [alta] [Holt11] |
| Delfini e globicefali | fischi ~2–20 kHz; clic fino a > 100 kHz | branchi, ore | clic > 215 dB pp | Piattaforma Celtica (delfino comune, altissima densità), Gibilterra, Faroe | [media] [ADA037620]; `03` |
| Focena | clic stretti ~130 kHz | — | — | Baltico | [bassa]: fuori banda, solo colore |
| **Foca barbata** | trilli 0,2–6 kHz (estremi 20 Hz–11 kHz) | trilli di 10 s–3 min; picco in primavera, stop a fine giugno; udibili 5–10 km (fino a 25) | — | Barents settentrionale, Okhotsk | [alta] [Frouin16]; [DOSITS-bs] |
| Tricheco | colpi e "campane" | — | — | Barents/Svalbard | [bassa] |
| **Merluzzo** (*Gadus morhua*) | grugniti a ~50 Hz con 2ª armonica ~103 Hz | 57–360 ms | ~123 dB | Barents, Norvegia (riproduzione fine inverno), Faroe | [alta] [Cod11]; [CodSL] |
| **Eglefino** | colpi e ronzii < 600 Hz; coro 30–400 Hz | coro notturno, +12 dB di notte | — | Barents, Faroe, Celtica | [media] [Haddock] |
| **Aringa** | impulsi a larga banda 1,7–22 kHz ("FRT") | treni di 0,6–7,6 s, soprattutto di notte, più fitti con banchi densi | — | Mar di Norvegia (stock enormi) | [alta] [Wilson04] |
| Cori di pesci (sciaenidi ecc.) | 50–2000 Hz (soprattutto 100–500); sciaenidi ~300–800 Hz | crepuscolo e notte, picchi in ~1 h dal tramonto, stagioni calde | **+20–30 dB** sul fondo a bassa frequenza | Mediterraneo (Otranto, Gibilterra), Golfo Persico | [alta] [Hild09]; [media] [Chorus] |
| **Gamberi schioccatori** | picco 2–5 kHz, fino a 200 kHz | crepitio continuo, più forte di notte [bassa] | 183–189 dB pp | acque basse calde (< 60 m): Golfo Persico, coste mediterranee rocciose [bassa] | [alta] [Hild09]; `02` §4.5 |

**Uso nel gioco:** i biologici devono produrre (a) **falsi tonali** (comune a 20 Hz, che sul LOFAR a bassa risoluzione sembra una riga), (b) **mascheramento** (cori serali che alzano il fondo in 100–800 Hz proprio quando un battello sale a quota periscopica al tramonto), (c) **colore** (canti udibili, accelerati se lenti), (d) **disturbo tattico** (un capodoglio o un branco di delfini che clicca nel fascio del bersaglio sporca la banda 5–25 kHz dell'intercettatore e dell'attivo). [bassa per gli effetti di gioco]

### 5.4 Geofisici
- **T‑phase dei terremoti:** il canale SOFAR fa da guida d'onda sopra circa **2,5 Hz**; i T‑phase tettonici sono lunghi (decine di secondi o minuti, inviluppo a fuso); quelli **vulcanici** sono più brevi e più acuti, con picchi attorno a **6 Hz**. Un'eruzione sottomarina (Monowai) è stata registrata a **15 800 km**. [media] [Volc99]; [Monowai16]; [Fox]
- **Vulcani sottomarini:** rombi con tono di ~125 Hz e armoniche di 50 Hz tra 50 e 300 Hz, più sibili della lava che si spegne in acqua (vulcano Bárcena, anni '50). [media] [Barcena]
- **Ghiaccio:**
  - **distacco di iceberg** (*calving*): larga banda 1–440 Hz, breve; un distacco in Groenlandia ha dato circa 215–225 dB pp, tra i suoni artici più forti [alta] [PMEL-ice]; [Podolskiy22]
  - **tremore armonico d'iceberg** a contatto con il fondo o con un altro iceberg: fondamentale 40 Hz e armoniche ogni 40 Hz (una "famiglia armonica" perfetta, **ottimo falso contatto**) [alta] [PMEL-ice]
  - **incaglio d'iceberg:** suono che scende di frequenza in ~7 min [alta] [PMEL-ice]
  - **icequake** ("Bloop"): rilevabile oltre 5000 km [alta] [PMEL-ice]
  - **fusione** di ghiaccio glaciale: bolle d'aria a 1–3 kHz; le baie glaciali sono rumorose come un mare moderato [media] [IGF]
  - **fessurazione termica e margine del ghiaccio:** `02` §4.6.
- **Teatri:** Curili/Okhotsk (sismicità e vulcani attivissimi; ghiaccio d'inverno), Mediterraneo (arco ellenico, vicino a Otranto; Etna e Tirreno), Faroe (dorsale di Reykjanes e Islanda), Gibilterra (zona Azzorre–Gibilterra), Barents (iceberg dalle Svalbard e dalla Terra di Francesco Giuseppe solo molto a nord). [media per la geologia, bassa per la frequenza degli eventi]

### 5.5 Meteo e mare
- **Vento/onde:** dominano 500 Hz–50 kHz; `NL = 56 + 19·log(ss) − 17·log f` (f in Hz, 1–25 kHz, ss = stato del mare 1–6). [alta] [Hild09] (è un'altra forma della Knudsen di `02` §4.2)
- **Pioggia:** impatto della goccia + pulsazione della bolla, picchi tra 500 Hz e 10 kHz; fino a +20 dB a media frequenza con pioggia forte. Pioggerella: picco 13–25 kHz (`02` §4.4). [alta] [Hild09]
- **Risacca e frangenti** vicino alla costa e sui bassifondi: larga banda a bassa e media frequenza, forte dipendenza dal moto ondoso. [bassa]
- **Infragravità:** onde lunghe di superficie dominano sotto 10 Hz. [alta] [Hild09]
- **Rumore termico:** `NL = −15 + 20·log f` (f in kHz), domina sopra ~50 kHz. [alta] [Hild09]

### 5.6 Attività umane impulsive e industriali

| Sorgente | Banda | Ritmo | Livello | Nota d'epoca e teatro | Fonte |
|---|---|---|---|---|---|
| **Cannoni ad aria (sismica)** | 5–300 Hz (fino a 3 kHz) | colpi ogni **10–20 s**, nave a ~5 kt, per giorni | 260 dB (equivalente), 220–230 pp in campo vicino | Mare del Nord e piattaforma norvegese nel 1989 [media]; udibili a > 3000 km in Atlantico | [alta] [Hild09] |
| **Battitura di pali** | 100–1000 Hz | impulsi di 30–50 ms ogni **2–4 s** | `SL = 205,9 + 10,2·log H` (H in kJ); 237 dB per 1000 kJ | porti, piattaforme in costruzione | [alta] [Hild09] |
| Sonar LFA (SURTASS) | 100–500 Hz | sequenze di 6–100 s ogni 6–15 min | 235 dB | **solo moderno** (2000 e oltre) | [alta] [Hild09] |
| Esplosioni | 0,5–200 Hz | singole | formula §5.1 | esercitazioni, pesca con esplosivo | [alta] [Hild09] |

### 5.7 Che cosa si sente in ciascun teatro (sintesi per popolare le missioni)

| Teatro (`03`) | Traffico e industria | Biologici | Geofisici e meteo |
|---|---|---|---|
| Faroe / GIUK | moderato: rotte nordeuropee, pescherecci faroesi e scozzesi | capodogli, globicefali, orche, balenottere comuni e azzurre, merluzzo/eglefino | mare forte d'inverno; T‑phase dalla dorsale di Reykjanes |
| Barents / Kola | traffico militare sovietico intenso, flottiglie di Murmansk, rompighiaccio | merluzzo (fine inverno), megattere e minori d'estate, foca barbata e trichechi molto a nord | margine del ghiaccio lontano (74–77°N), polar low |
| Gibilterra | altissimo (rotte Atlantico–Mediterraneo), petroliere | delfini, globicefali, capodogli, comuni mediterranee | onde interne; Levante e Poniente |
| Otranto | medio‑alto (Brindisi, Bari–Grecia), strascico italiano | cori di pesci serali d'estate, delfini, gamberi sulle coste | Scirocco con pioggia; T‑phase dall'arco ellenico |
| Kunashir / Okhotsk | pesca intensissima (salmone, granchio, saury; flotte giapponesi e sovietiche) | cetacei, orche | **sismicità e vulcani**; ghiaccio gennaio–aprile |
| Piattaforma Celtica | altissimo (Ouessant), pesca franco‑spagnola al ciglio | **delfino comune ad altissima densità**, balenottere comuni | burrasche, pioggia |
| Baltico SW | altissimo, traghetti di linea | poca biologia (focene fuori banda, foche) | acqua bassa: riverbero; ghiaccio raro (1985, 1987) |
| (Golfo Persico, se aggiunto) | petroliere, piattaforme | **gamberi schioccatori**, cori di pesci | acqua bassa e calda |

Fonti della tabella: `03` parte A e tabella riassuntiva, più §5.3–5.6. [media]

---

## 6. Proposte per il gioco

### 6.1 Funzioni in ordine di priorità

| # | Cosa | Perché (realismo / gioco) | Epoca | Sforzo |
|---|---|---|---|---|
| 1 | **Resa SDR delle cascate**: normalizzatore TPSW, pavimento/tetto/AUTO, palette 1989 e moderne, A‑scan con massimo/minimo | è la richiesta centrale; rende leggibili i battelli silenziosi senza barare | entrambe | S |
| 2 | **Cursori**: lettura, coppia delta (Δf, Δt, Δrilevamento), **armonico** con bande laterali | misure vere (Doppler, durate, famiglie); gioco di abilità | entrambe | S |
| 3 | **Tracker S‑numerati** (manuali, più automatici in moderno), unione e perdita, lista contatti | struttura tutto il resto (annotazioni, slot, TMA, rapporti) | entrambe | M |
| 4 | **Libreria consultabile + sovrapposizione delle righe** con fattore Doppler regolabile; confronto per **rapporti** | classificazione per ragionamento invece del solo ANALIZZA | entrambe | M |
| 5 | **Paesaggio sonoro**: sorgenti ambientali come entità (biologici, geofisici, industriali, meteo) con processi temporali (§6.2) e popolazione per teatro/stagione/ora (§5.7) | mare vario, falsi contatti, mascheramento realistico | entrambe | L |
| 6 | **Annotazioni** (riga, rettangolo, evento) persistenti, automatiche per gli eventi propri | memoria del giocatore; replay e registro più ricchi | entrambe | M |
| 7 | **Slot (8)** e confronto affiancato/sovrapposto/differenza | richiesta esplicita; mostra cosa cambia in un contatto | entrambe (in 1989 = "carta strappata" dal registratore) | M |
| 8 | **Scala dei tempi** ×1/×4/×16 con decimazione per massimo; buffer di storia lungo (registratore) | si vedono tracce lente e transitori brevi | entrambe | S |
| 9 | **Verniero** e strumento **Doppler/CPA** (v e R_CPA → TMA) | telemetria passiva vera; premia la pazienza | entrambe | M |
| 10 | **FRAZ** sulla rimorchiata | dice a quale fascio appartiene una riga; tipico della schiera | 1989 (BQR‑23) e moderno | M |
| 11 | **Classificazione a passi** con confidenza (NONSUB/POSSUB/PROBSUB/CERTSUB) e rapporti al comando con formula fissa | lessico e ritmo veri; l'IA dell'operatore parla come un sonarista | entrambe | S |
| 12 | **Registratore e ACINT**: registrare da vicino una classe poco nota completa la sua scheda in libreria | obiettivo secondario di missione, motivo per pedinare | entrambe | M |
| 13 | **Ascolto** sintetico per fascio, ×4/×16 per i suoni lenti | ciò che i sonaristi fanno davvero; atmosfera | entrambe | L |
| 14 | **Modalità d'epoca**: 1989 (monocromatico, poca automazione, Doppler proprio manuale) / moderna (colore, autotracker, aiuti) | coerenza storica e scelta di difficoltà | — | M |
| 15 | **Operatore IA** con abilità (±3 dB, `02` §10.5) che propone tracker e classificazioni con errori plausibili (§3.9) | il giocatore può delegare; errori credibili | entrambe | M |
| 16 | **DEMON a cascata** e profondità di modulazione | si vede quando il bersaglio cambia giri | entrambe | S |

S = giorni, M = una o due settimane, L = più settimane (stima per una persona, con il motore attuale). **STIMA**

**Prima tappa consigliata (massimo effetto per sforzo):** 1 + 2 + 3 + 8 + 11, poi 4 + 6 + 7, poi 5 (che richiede lo schema dati), poi il resto.

### 6.2 Estensione dello schema dati della libreria

Principio: le `[firme.*]` di oggi restano valide (navi e sottomarini); si aggiunge una sezione **`[sorgenti.*]`** per tutto ciò che non è un'unità navale, e alcuni campi facoltativi alle firme. Tutto generato da `tools/pipeline/acustica.py`, come oggi.

**Campi comuni a ogni sorgente**

| Campo | Tipo | Significato |
|---|---|---|
| `categoria` | `"militare" \| "civile" \| "biologico" \| "geofisico" \| "meteo" \| "industriale"` | primo livello della classificazione (passo 1, §2.9) |
| `sottocategoria` | stringa | per es. `"misticeto"`, `"pesce"`, `"ghiaccio"`, `"sismica"` |
| `nome`, `nome_breve` | stringa | scheda della libreria |
| `teatri`, `mesi`, `ore` | liste | dove e quando compare (`ore` = `"giorno"`, `"crepuscolo"`, `"notte"`) |
| `densita` | numero per 1000 nm² | quante istanze per cella (`03` B6) |
| `profondita_m` | `[min, max]` | quota della sorgente |
| `moto` | `{v_kn = [min, max], deriva = bool}` | balene e branchi si muovono; eventi geofisici no |
| `estensione` | `"puntiforme" \| "diffusa"` | un coro è diffuso (settore largo sul BTR) |
| `livello_db`, `livello_tipo` | numero, `"rms" \| "pp"` | livello di sorgente |
| `componenti` | lista (sotto) | lo spettro e il tempo |
| `aurale` | `{descrizione, accelera = 1..16, campione = "file"}` | ascolto (§3.7) |
| `indizi` | lista di stringhe | ciò che la scheda mostra per il confronto |
| `confonde_con` | lista di classi | per i falsi contatti voluti |
| `fonte`, `confidenza` | stringa | come oggi |

**Tipi di componente** (ognuno con `livello_rel_db`):

| `tipo` | Campi | Esempi |
|---|---|---|
| `tonale` | `hz`, `stabilita_hz` (deriva casuale), `v_min/v_max`, `riga` (TipoRiga) | come le `righe` di oggi |
| `famiglia` | `f1_hz`, `armoniche = n`, `decadimento_db` per armonica, `pari_attenuate` | rete 50/60 Hz, tremore d'iceberg 40 Hz, grugnito del merluzzo |
| `banda` | `f_lo`, `f_hi`, `pendenza_db_ott`, `picco_hz` | cavitazione, coro, pioggia |
| `sweep` | `f_da`, `f_a`, `durata_s`, `forma = "lin" \| "log"` | impulso della comune 40→13 Hz, incaglio d'iceberg |
| `impulso` | `banda`, `durata_s`, `inviluppo = {attacco_s, decadimento_s}` | clic del capodoglio, colpo di pala, cannone ad aria |
| `modulazione` | `asse_hz`, `pala_hz`, `profondita` (0–1), `banda_portante` | DEMON |
| `canto` | `unita` (lista di componenti), `frase` (ordine), `ripetizioni`, `variazione` | megattera, azzurra |

**Schema temporale** (per `impulso`, `sweep`, `canto`; in una tabella `ritmo`):

| Campo | Significato |
|---|---|
| `processo` | `"periodico" \| "poisson" \| "treno" \| "continuo"` |
| `intervallo_s` | `[min, max]` (periodico) oppure `tasso_h` (Poisson) |
| `gruppi` | `{dimensione = [1, 3], intervallo_interno_s = [6, 37]}` (doppiette e terzine) |
| `serie` | `{durata_min = [30, 1950], pausa_s = [120, 150], ogni_min = [10, 15]}` |
| `diurno_db` | aumento del livello per ora del giorno (per es. coro +20 dB a 1 h dal tramonto) |

**Esempio** (balenottera comune, valori da §5.3):

```toml
[sorgenti.balenottera_comune]
categoria = "biologico"
sottocategoria = "misticeto"
nome = "Balenottera comune (Balaenoptera physalus)"
teatri = ["faroe", "celtica", "gibilterra", "otranto"]
mesi = [10, 11, 12, 1, 2, 3]
ore = ["giorno", "crepuscolo", "notte"]
densita = 0.5
profondita_m = [10, 60]
moto = { v_kn = [1, 6], deriva = false }
estensione = "puntiforme"
livello_db = 185.0
livello_tipo = "rms"
componenti = [{ tipo = "sweep", f_da = 40.0, f_a = 15.0, durata_s = 0.9, forma = "lin", livello_rel_db = 0.0 }]
ritmo = { processo = "treno", intervallo_s = [12.0, 25.0], gruppi = { dimensione = [1, 2], intervallo_interno_s = [6.0, 12.0] }, serie = { durata_min = [30, 600], pausa_s = [120, 150], ogni_min = [10, 15] } }
aurale = { descrizione = "tonfi profondi e regolari", accelera = 10 }
indizi = ["impulsi a ~20 Hz molto regolari", "nessuna riga d'elica", "nessun DEMON"]
confonde_con = ["sottomarino_generico"]
fonte = "09 §5.3: DOSITS, Watkins 1987, Miksis-Olds 2019"
confidenza = "alta"
```

**Campi facoltativi da aggiungere alle `[firme.*]`** (unità navali):
- `famiglie` (come sopra) per le armoniche di rete e dei diesel, invece di elencare ogni armonica;
- `elica_cpp = true` (le righe d'asse non seguono la velocità, §3.5);
- `direttivita = { poppa_db = +6, prua_db = -3 }` [McKenna12];
- `transitori = [{ evento = "porta_tubo", livello_db = 135, durata_s = 1.5, banda = [200, 4000] }, ...]` (oggi solo nel modello generale, `02` §10.3);
- `masker = { riduzione_tonali_db = 10, banda_aggiunta = [500, 5000] }` per Perry e Spruance [Masker];
- `aurale` e `indizi` come le sorgenti;
- `scheda_nota` (testo breve per la libreria) e `nota_acint` (riga rivelata solo dopo una registrazione completa, §6.1 punto 12).

**`TipoRiga` da estendere:** `generatore_400`, `idraulica`, `ventilazione`, `compressore`, `cavitazione` (righe di pala in cavitazione), `ghiaccio`, `geofisico`, `industriale`; il colore dei segnalibri (§4.2 punto 8) segue il tipo.

**Campo ambiente nel motore** (non sorgenti puntiformi): vento, pioggia, traffico lontano, cori diffusi, ghiaccio restano termini di NL per cella (`02` §10.4), ma con **spettro** e **direzionalità** (traffico lontano dall'orizzonte e lungo le rotte, `03`; cori dalla costa) invece di un numero solo, così il BTR mostra settori di fondo più alto. [media, concetto] [Hild09]

### 6.3 Note di giocabilità
- **Aiuti regolabili:** da "sonarista vero" (niente sovrapposizione automatica, Doppler proprio manuale) a "assistito" (l'operatore IA propone tracker e ipotesi con il loro perché). Lo stesso motore, solo l'interfaccia cambia.
- **Falsi contatti dosati:** al massimo 1–2 biologici o geofisici ingannevoli per ora di gioco nei teatri ricchi; mai durante le fasi d'attacco scriptate. **STIMA DI GIOCO**
- **Tempi reali ma compressi:** la classificazione al passo 2 dura 30–45 s di gioco [Amick20]; con la compressione temporale del gioco va bene così.
- **Coerenza con il classico:** nulla di questo tocca la modalità classica (regola del progetto).


### 6.4 S5d nel motore: misure, ACINT, operatore e ascolto (25/10/2026)

Moduli puri in `engine/nucleo` (`misure_sonar`, `acint`, `sonarista`, `ascolto`; verniero e righe ricevute in
`analisi_acustica`). Le scelte e le stime:

- **Verniero** (§2.2): zoom di 4 Hz a 0,01-0,02 Hz di bin, con la riga sparsa sui bin vicini secondo la finestra di
  Hann; il picco si stima con una parabola sui dB di tre bin. Incertezza dal limite di Cramér-Rao per un tono
  `σ_f = √3 / (π T √SNR)` [RB74], con T = 1/bin e SNR il rapporto fra il picco e il fondo locale (la mediana della
  finestra), moltiplicato per il numero di spettri sommati (ALI). [alta per la formula]
- **Doppler e CPA** (§3.6): la curva a S `f(t) = f0 (1 − v² (t − t_cpa) / (c R(t)))` del moto relativo rettilineo,
  risolta ai minimi quadrati (Levenberg-Marquardt) a partire dalla stima classica (f0 fra gli estremi, v dal salto,
  t_cpa alla pendenza massima, R = f0 v² / (c |df/dt|)). Senza il CPA dentro i dati (a un decimo dagli estremi)
  distanza e velocità non si separano: la misura non vale. Alla TMA passa come confronto con la soluzione proposta
  (scarti di t_cpa, R_cpa e v, coerente entro 2σ con un minimo di 30 s, 100 m, 0,3 m/s). **STIMA DI GIOCO** per
  soglie e minimi.
- **FRAZ** (§2.2): oltre alla presentazione frequenza × rilevamento, la distanza dal moto relativo rettilineo:
  `d²R/dt² = v_t²/R` e `v_t = R dB/dt`, quindi `R = −c (df/dt) / (f0 (dB/dt)²)`. [alta per la fisica; derivazione
  dell'autore come al §3.6] Limite: nessuna manovra nella finestra, df/dt e rateo a 3σ, errore relativo entro il 50%;
  lontano dal CPA il rateo cala e la stima si perde. **STIMA DI GIOCO** per le soglie.
- **Registratore e ACINT** (§2.8, §3.8): la storia di ogni contatto (righe, livello, rilevamento, DEMON), al più 6 ore a
  un campione ogni 5 s; oltre, la parte più vecchia si dimezza. Righe stabili: presenti in almeno il 30% dei campioni,
  entro lo 0,4%. Classificazione col confronto per rapporti e la scala del §2.9: CERTSUB con confidenza ≥ 0,8 e
  ≥ 5 righe concordi, PROBSUB con ≥ 0,55 e ≥ 3, POSSUB per un sottomarino meno sicuro o senza classe, NONSUB se la
  migliore non è un sottomarino; con meno di due righe concordi niente classe. Le righe stabili che la classe non
  spiega vanno nel rapporto come «non in libreria» (il gioco d'intelligence del §3.8). **STIMA DI GIOCO**
- **Operatore IA** (§2.7-2.9, §3.9): profili per esperienza, **STIMA DI GIOCO** costruita sul passo 2 di 30-45 s
  [Amick20] e sugli errori del §3.9:

| Esperienza | Soglia (dB di SE) | Ritardo del rapporto | Analisi | Classe sbagliata | Biologico come POSSUB | Rumore in più sul rilevamento | Tenuta | Scambio a un incrocio |
|---|---|---|---|---|---|---|---|---|
| recluta | 3 | 60 s | 90 s | 30% | 25% | 2,0° | 60 s | 40% |
| medio | 1 | 30 s | 45 s | 15% | 10% | 1,0° | 120 s | 20% |
| esperto | 0 | 15 s | 30 s | 5% | 3% | 0,5° | 180 s | 5% |

  La classe sbagliata è la seconda ipotesi della libreria. Un incrocio vale quando le sorgenti stanno a meno di 2° e
  finisce oltre i 5°; si conta sulla geometria vera, perché il tracker leviga il rumore. Le tracce si tengono fino a
  3 dB sotto la soglia di scoperta. I rapporti seguono la formula del §2.9: «Comandante, sonar: nuovo contatto,
  rilevamento 245, designato Sierra 3».
- **Ascolto** (§3.7): rumore colorato come lo spettro del fascio (rumore del mare e proprio meno il DI, per il S/N
  dello spettro), sintetizzato in frequenza con fasi casuali dal seme; righe come sinusoidi alla frequenza ricevuta,
  con la potenza del loro S/N; battito d'elica al 40% alla frequenza di pala del DEMON per chi cava (**STIMA**);
  passa banda del pannello; righe sotto 50 Hz su di ottave (opzione); ascolto accelerato ×4/×16 come NOAA PMEL
  [PMEL-ice]. Fondo a −24 dBFS rms, senza saturare.

---

## 7. Bibliografia

**Sistemi e dottrina**
- **[RB74]** D. C. Rife, R. R. Boorstyn, «Single-tone parameter estimation from discrete-time observations», *IEEE
  Transactions on Information Theory* 20 (1974), 591-598.
- **[Amick20]** A. Amick, "How Submarine Sonarmen Tirelessly Hunt For Enemies They Can't Even See", *The War Zone*, 2020 (ex sonarista di 688 e Ohio). https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see
- **[USNI81]** "U.S. Navy Sonars, Part 2", *Proceedings*, settembre 1981 (consultato tramite estratti di ricerca; pagina non accessibile). https://www.usni.org/magazines/proceedings/1981/september/us-navy-sonars-part-2
- **[USNI88]** "Improving Submarine Sonar Displays", *Proceedings*, febbraio 1988 (non consultato: da leggere). https://www.usni.org/magazines/proceedings/1988/february/improving-submarine-sonar-displays
- **[NC-BQR23]** Nuclear Companion, AN/BQR‑23 Towed Array Processor. https://nuclearcompanion.com/data/an-bqr-23-towed-array-processor/
- **[MP-BSY1]** Military Periscope, AN/BSY‑1 (estratto). https://militaryperiscope.com/weapons/sensorselectronics/sonars/anbsy-1/overview
- **[WP-BQQ10]** Wikipedia, AN/BQQ‑10 (A‑RCI, FLIT, schiere). https://en.wikipedia.org/wiki/AN/BQQ-10
- **[DOTE]** DOT&E, rapporti annuali A‑RCI e AN/BYG‑1, 2011–2022 (citati tramite estratti). https://www.dote.osd.mil/Portals/97/pub/reports/FY2012/navy/2012arci.pdf
- **[GD-BYG1]** General Dynamics Mission Systems, AN/BYG‑1. https://gdmissionsystems.com/submarine-systems/combat-systems/an-byg-1
- **[VA-LAB]** GlobalSecurity, SSN‑774 Virginia design; Defense Industry Daily, "Virginia Block III: the revised bow"; seaforces.org. https://www.globalsecurity.org/military/systems/ship/ssn-774-design.htm ; https://www.defenseindustrydaily.com/?p=240366
- **[MP-MGK400]** Military Periscope, MGK‑400EM (estratto) e voci secondarie su MGK‑400/Shark Gill. https://www.militaryperiscope.com/weapons/sensorselectronics/sonars/mgk-400em-passiveactive-sonar-system/
- **[WP-2076]** Wikipedia, Sonar 2076; **[Thales-2076]** Thales, "30 years of Sonar 2076 / Sonar 2176" e servizio d'addestramento a HMS Raleigh. https://en.wikipedia.org/wiki/Sonar_2076 ; https://www.thalesgroup.com/en/news-centre/insights/united-kingdom/thales-celebrates-30-years-sonar-2076-and-unveils-next
- **[RN-aural]** Royal Navy International Defence Training, corsi "Basic Aural Analysis" e "Advanced Aural Analysis" (contenuti da estratti; pagine con accesso limitato). https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/advanced-aural-analysis
- **[ACINT]** Wikipedia, Acoustical intelligence; DVIDS, "Elite U.S. Navy Program Qualifies 300th Specialist" (2025). https://en.wikipedia.org/wiki/Acoustical_intelligence ; https://www.dvidshub.net/news/printable/493889
- **[ADA135857]** Studio della Marina USA sull'informazione per l'ufficiale di manovra (anni '80). https://apps.dtic.mil/sti/pdfs/ADA135857.pdf ; **[ADA066750]** indizi di classificazione non infallibili. https://apps.dtic.mil/sti/pdfs/ADA066750.pdf
- **[POSSUB]** glossari di brevità ASW NATO (fonte debole). https://brainscape.com/flashcards/nato-code-words-asw-18015791/packs/22589756 ; https://harpgamer.com/harpforum/tags/tactics%20101
- **[CW-sierra]** PMC Tactical, "Sierra vs Master contacts" (*Cold Waters*), e discussioni dei giocatori: solo convenzione. https://cold-waters.pmctactical.org/sierra-vs-master-contacts.php
- **[GUPPY]** "The Era of the GUPPY, Part II" (Ekelund, conteggio dei giri). https://warhistory.org/article/the-era-of-the-guppy-part-ii ; **[NSMRL716]** Olson e Laxar, NSMRL Report 716 (diagrammi tempo‑rilevamento), citato tramite estratti.
- **[LOFAR-wiki]** Wikipedia, Low Frequency Analyzer and Recorder. https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder ; **[SOSUS-wiki]** https://en.wikipedia.org/wiki/SOSUS
- **[Masker]** FAS, Prairie/Masker. https://man.fas.org/dod-101/sys/ship/weaps/prairie.htm ; GlobalSecurity https://www.globalsecurity.org/military/systems/ship/systems/masker.htm ; MARIN/SATURN (riduzioni misurate). https://www.webb.edu/wp-content/uploads/2024/02/Lafeber-SISDO23-edited.pdf

**Elaborazione e presentazione**
- **[SL84]** W. A. Struzinski, E. D. Lowe, "A performance comparison of four noise background normalization schemes proposed for signal detection systems", *JASA* 76(6), 1984 (citato tramite letteratura successiva). **[JAC13]** confronto con S3PM e OTA su BTR. https://jac.ac.cn/article/doi/10.15949/j.cnki.0371-0025.2013.04.010
- **[Morris]** D. Morris, "Some results using a split‑window CFAR processor to normalise sonar returns", Institute of Acoustics. https://www.ioa.org.uk/system/files/publications/D%20MORRIS%20SOME%20RESULTS%20USING%20A%20SPLIT-WINDOW%20CFAR%20PROCESSOR%20TO%20NORMALISE%20SONAR%20RETURNS.pdf ; **[SWN-wiki]** descrizione del SWN in US5349567. https://patents.google.com/patent/US5349567
- **[PAT-ALI]** US 5 008 630, "Stationary probability integrator system" (integratore automatico di righe). https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5008630
- **[PAT-FRAZ]** US 5 432 753 e US 6 525 994 (spettro frequenza‑azimut); *Remote Sensing* 2024 (deconvoluzione FRAZ). https://doaj.org/article/4a78928e5edc4d33ab39f32a734c1be3
- **[GI10]** "Narrowband passive sonar tracking", GI Jahrestagung 2010. https://dl.gi.de/items/0ee2c2af-88c2-4d34-a931-d64ed1d6124e
- **[Felgate]** N. Felgate, "Time‑domain Doppler correction algorithm for organic noise ranging", Institute of Acoustics. https://www.ioa.org.uk/system/files/proceedings/n_felgate_time-domain_doppler_correction_algorithm_for_organic_noise_ranging.pdf
- **[Kraiss84]** K. F. Kraiss, K. H. Küttelwesch, "Color and grey scale in sonar displays", 20th Annual Conference on Manual Control, NASA Ames, 1984. https://ntrs.nasa.gov/citations/19850006243
- **[TRACOR79]** Rapporto TRACOR sul colore nei display sonar simulati. https://apps.dtic.mil/sti/tr/pdf/ADA066691.pdf ; **[HFS91]** scale di colore regolabili per il sonar passivo, *Proc. Human Factors Society* 35 (1991), con rimandi a Salafia (NUSC 1988, 1990). https://journals.sagepub.com/doi/pdf/10.1177/154193129103502011
- **[Viridis]** N. Smith, S. van der Walt, mappe percettive di matplotlib (viridis, cividis), 2015. https://bids.github.io/colormap/ ; **[Turbo]** A. Mikhailov, "Turbo, an improved rainbow colormap", Google Research, 2019. https://research.google/blog/turbo-an-improved-rainbow-colormap-for-visualization/

**Programmi SDR e bioacustici**
- **[SDRangel]** F4EXB, SDRangel, documentazione dello spettro e della cascata. https://github.com/f4exb/sdrangel/blob/master/sdrgui/gui/spectrum.md ; **[SDRangel-mk]** marcatori e annotazioni. https://github.com/f4exb/sdrangel/blob/master/sdrgui/gui/spectrummarkers.md
- **[SDRsharp]** RTL‑SDR.com, *SDRSharp Users Guide* (risoluzione, finestra, contrasto, range, offset, marcatori di tempo, Frequency Manager). https://www.rtl-sdr.com/sdrsharp-users-guide/
- **[GQRX]** GQRX, "Practical tricks and tips" (FFT, media, picchi, zoom). https://gqrx.dk/doc/practical-tricks-and-tips
- **[SDRConsole]** SDR‑Radio.com, Spectrum and Waterfall. https://www.sdr-radio.com/SpectrumAndWaterfall
- **[SpecLab]** W. Büscher (DL4YHF), Spectrum Lab, "Spectrum displays". https://www.qsl.net/dl4yhf/speclab/specdisp.htm
- **[SDRpp]** SDR++ User Guide (indice: menu Display, Min/Max, bandplan). https://www.sdrpp.org/manual.pdf
- **[Raven]** Cornell, Raven Pro (tutorial UMD e note della versione 1.5). https://science.umd.edu/faculty/wilkinson/SFT/Raven_tutorial.pdf ; **[PAMGuard]** https://github.com/PAMGuard/PAMGuard ; annotazioni in PAMpal https://search.r-project.org/CRAN/refmans/PAMpal/html/markAnnotated.html

**Paesaggio sonoro**
- **[Hild09]** J. A. Hildebrand, "Anthropogenic and natural sources of ambient noise in the ocean", *Mar. Ecol. Prog. Ser.* 395, 5–20 (2009), tabella 1 e testo. https://www.cetus.ucsd.edu/docs/publications/HildebrandMEPS2009.pdf
- **[McKenna12]** M. F. McKenna et al., "Underwater radiated noise from modern commercial ships", *JASA* 131(1), 2012. https://tethys.pnnl.gov/publications/underwater-radiated-noise-modern-commercial-ships
- **[CPP22]** studio del rumore di una nave con elica a passo variabile (2022), tramite estratto. https://par.nsf.gov/search/award_ids:1736749 ; **[Suisan95]** rumore di pescherecci e cianciolanti, *Nippon Suisan Gakkaishi* 61. https://dx.doi.org/10.2331/suisan.61.553
- **[DOSITS-fin]** https://dosits.org/galleries/audio-gallery/marine-mammals/baleen-whales/fin-whale ; **[DOSITS-blue]** https://dosits.org/galleries/audio-gallery/marine-mammals/baleen-whales/blue-whale/ ; **[DOSITS-hump]** https://dosits.org/galleries/audio-gallery/marine-mammals/baleen-whales/humpback-whale/ ; **[DOSITS-sperm]** https://dosits.org/galleries/audio-gallery/marine-mammals/toothed-whales/sperm-whale/ ; **[DOSITS-bs]** https://dosits.org/?p=15970
- **[Watkins87]** W. A. Watkins et al., "The 20‑Hz signals of finback whales", *JASA* 82 (1987). https://research-portal.st-andrews.ac.uk/en/publications/the-20-hz-signals-of-finback-whales-balaenoptera-physalus/ ; **[Miksis19]** Miksis‑Olds et al., *JASA* 2019. https://research-repository.st-andrews.ac.uk/bitstream/10023/18854/1/Miksis_Olds_2019_JASA_20_Hz_CC.pdf ; **[MedFin22]** canto della comune mediterranea. https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9363496/
- **[Fournet18]** livelli delle chiamate di megattera (113–157 dB), via estratto. https://www.science.gov/topicpages/c/call+source+level
- **[Minke14]** "Seasonal migrations of North Atlantic minke whales", *Movement Ecology* 2014. https://pure.uhi.ac.uk/en/publications/seasonal-migrations-of-north-atlantic-minke-whales-novel-insights/ ; **[NavyMinke]** rapporto di monitoraggio della Marina USA (Jacksonville). https://navymarinespeciesmonitoring.us/index.php/download_file/840/287
- **[Holt11]** livelli delle chiamate delle orche residenti meridionali (135–176 dB). https://wwhandbook.iwc.int/fr/downloadable-resources/searchable-database-of-scientific-literature/effects-of-noise-levels-and-call-types-on-the-source-levels-of-killer-whale-calls
- **[ADA037620]** rapporto militare sui suoni dei mammiferi marini (clic di focena > 215 dB pp). https://apps.dtic.mil/sti/pdfs/ADA037620.pdf
- **[Frouin16]** Frouin‑Mouy et al., foca barbata nel Chukchi, *Marine Mammal Science* 2016; JASCO. https://www.jasco.com/journal-publications/underwater-acoustic-behavior-of-bearded-seals-erignathus-barbatus-in-the-northeastern-chukchi-sea-20072010-pdf
- **[Cod11]** monitoraggio acustico del merluzzo nella baia del Massachusetts. https://pure.uhi.ac.uk/en/publications/acoustic-monitoring-of-atlantic-cod-gadus-morhua-in-massachusetts/ ; **[CodSL]** livello dei grugniti in gabbia (123 dB). https://bioacoustics.info/article/characterisation-grunt-sound-pressure-level-spawning-atlantic-cod-gadus-morhua ; **[Haddock]** https://ecomagazine.com/news/fisheries-aquaculture/vessel-traffic-noise-reduces-communication-ranges-of-atlantic-cod-haddock/
- **[Wilson04]** B. Wilson, R. S. Batty, L. M. Dill, "Pacific and Atlantic herring produce burst pulse sounds", *Proc. R. Soc. B* 271 (Suppl. 3), 2004. https://pmc.ncbi.nlm.nih.gov/articles/PMC1809969
- **[Chorus]** cori di sciaenidi e cori serali (rapporti e articoli vari via estratti). https://apps.dtic.mil/sti/pdfs/ADA048863.pdf ; https://thescholarship.ecu.edu/bitstreams/962e0ad2-567f-4a59-98fd-903a83dbe0a0/download
- **[Fish]** B. Goldfarb, "Biologist Marie Fish catalogued the sounds of the ocean", *Smithsonian*, 2021. https://www.smithsonianmag.com/science-nature/biologist-marie-fish-catalogued-sounds-ocean-world-hear-180977152/
- **[PMEL-ice]** NOAA PMEL, suoni criogenici e "Bloop". https://pmel.noaa.gov/acoustics/specs_cryogenic.html ; https://pmel.noaa.gov/acoustics/sounds/bloop.html
- **[Podolskiy22]** Podolskiy et al., distacco al ghiacciaio Bowdoin (Groenlandia), 2022. https://eprints.lib.hokudai.ac.jp/dspace/handle/2115/86725 ; DOSITS https://dosits.org/glacial-calving-in-the-arctic-produces-record-source-levels ; **[IGF]** rumore di fusione 1–3 kHz. https://www.igf.edu.pl/?p=1256
- **[Volc99]** T‑phase vulcanici con picchi a ~6 Hz (JpGU 2000). https://www2.jpgu.org/meeting/2000/pdf/sd/sd-p008_e.pdf ; **[Monowai16]** osservazioni a 15 800 km. https://pure.lib.usf.edu/en/publications/ultralongrange-hydroacoustic-observations-of-submarine-volcanic-a-2/ ; **[Fox]** rassegna sul monitoraggio dei T‑phase (SOFAR > 2,5 Hz), via estratto. https://www.earth.northwestern.edu/people/emile/PDF/EAO130.pdf ; **[Barcena]** lezione GG711 (Hawaii) sul vulcano Bárcena. https://www.soest.hawaii.edu/krubin/GG711/GG711-lect13-fred.pdf

---

## 8. Lacune note (da colmare)
1. **Presentazioni del BQQ‑5 nel 1989:** nessuna descrizione aperta di console, colori, numero di bande BTR, risoluzioni LOFAR, presenza del DEMON. Da cercare: [USNI88] (accesso a pagamento), Friedman 1989, manuali tecnici declassificati, foto d'epoca.
2. **Designazioni Sierra/Master** e formule dei rapporti al comando: solo fonti indirette; servirebbero memorie di sonaristi (per es. Amick, altre su Proceedings o Naval Submarine League).
3. **Parametri reali dei normalizzatori e degli ALI** (finestre, soglie, tempi): solo brevetti e letteratura generale; i valori del §2.5 sono di taratura.
4. **Sistemi sovietici:** nessun dato aperto sulle presentazioni di Rubikon e Skat né sul funzionamento del calcolatore di classificazione.
5. **Pinnipedi artici (tricheco), minore, orca:** mancano bande e livelli verificati su fonte primaria.
6. **Pescherecci al lavoro** (verricelli, cavi, rete): nessun livello pubblicato trovato.
7. **Sonar NATO 1989** (Type 2020/2026/2046, DSUV, CSU‑83): nomi e caratteristiche da verificare.
8. **Livelli delle firme militari:** restano [bassa] come in `02` §12.
