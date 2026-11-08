# 14 — Carico delle armi e sala siluri (dossier di riferimento)

Progetto: remake realistico di *688 Attack Sub* (1989). Scopo: dare i numeri verificati e le regole per la **Sala siluri**
della Fase 4 (`docs/00_ROADMAP.md`): la schermata di carico prima della missione, sul modello di *F-117A*, con la pianta
del locale siluri, le armi del deposito e la scheda dell'arma. Riguarda il 688 Flight I e II (e il 688I per confronto),
l'Alfa (progetto 705/705K) e, dove serve alle missioni, Victor III, Sturgeon e Trafalgar, nel 1989 circa.

---

## 0. Convenzioni

- Come in `02_acustica_oceanografia_dottrina.md` §0: **[alta]** = dato standard verificato su fonte primaria o secondaria
  autorevole; **[media]** = letteratura aperta credibile non controllata sulla primaria, o derivata con fisica standard;
  **[bassa]** = stima plausibile, da fonti divulgative o da giochi. **STIMA** = valore di taratura del gioco, non un dato;
  **PROPOSTA** = scelta di progetto da discutere.
- Le sigle fra parentesi quadre rimandano alla bibliografia (§7). Le fonti sono riassunte con parole mie; le poche
  citazioni testuali sono brevi e fra virgolette.
- Unità: t metriche, kg, m, kn, km, nm. «Posto» = una posizione di stivaggio da siluro pesante (tubo o rastrelliera).
- I dossier del progetto si citano con il numero: `01` = piattaforme e armi, `02` = acustica, `05` = missili e
  contromisure, `09` = sonar e interfacce. Per il gioco originale: `docs/11_comandi_e_stazioni.md` (qui `11`).

---

## 1. Il locale siluri per classe

### 1.1 Quadro riassuntivo

| Classe | Tubi | Armi da tubo (tubi + rastrelliere) | Armi in più | Mine al posto delle armi | Disl. in immersione | Conf. |
|---|---|---|---|---|---|---|
| 688 Flight I (SSN-688…718) | 4 × 533 mm a centro nave | ~25 (24–26 nelle fonti) | — | sì, Mk 67 e Mk 60 (§2.6–2.7) | 6.927 t | [alta] per tubi e disl., [media] per il carico |
| 688 Flight II (SSN-719…750) | come sopra | ~25 | **12 pozzi verticali** per Tomahawk | come sopra | 6.927 t (circa) | [alta] |
| 688I (SSN-751…773) | come sopra | ~25 | 12 pozzi | **minamento dai tubi** dichiarato come novità | 6.927 t (circa) | [alta] |
| Sturgeon (SSN-637) | 4 × 533 mm a centro nave | 4 + fino a 21 di riserva (~23–25) | — | Mk 67, Mk 60 al posto dei siluri | 4.714 t | [alta] |
| Trafalgar | 5 × 533 mm | fino a 25–30 | — | sì (§1.4) | 5.200–5.300 t | [media] |
| Victor III (671RTM) | 4 × 533 + 2 × 650 mm a prua | 24 | — | 36 mine | ~7.250 t | [media] |
| **Alfa (705/705K)** | **6 × 533 mm a prua** | **18** (12 in una fonte) | — | 24 mine (36 in una fonte) | 3.200 t | [media] |

Fonti: 688 [WP688] [FAS688] [SubSup] [NI688] [NAVY-FF]; Flight II [WPProv] [WP688]; Sturgeon [WPSturgeon]; Trafalgar
[WPTraf] e `01` §2.3; Victor III [WPVictor] [NEVictor] e `01` §2.5; Alfa [WPAlfa] [GS705] [HSAlfa] [Legion705]
[NavEnc705]. Dettagli e contraddizioni sotto.

### 1.2 Los Angeles (688)

- **Tubi:** quattro da 533 mm (21 pollici) [FAS688] [WP688], a centro nave e angolati, perché la prua è occupata
  dalla sfera del sonar (`01` §2.1). [alta]
- **Carico da tubo:** le fonti più citate danno **circa 25 armi** fra tubi e rastrelliere [SubSup] [NI688]; Wikipedia
  nel testo dice **24 armi lanciate dai tubi** (citando Winnefeld, *Proceedings* 2023), mentre nella scheda riassuntiva
  scrive «37 × Mk 48», un numero che nessun'altra fonte conferma [WP688]. Il dossier `01` §2.1 riportava già 22–26.
  **Valore da usare: 25** (4 nei tubi + 21 in rastrelliera), [media]. Il 37 va scartato: probabilmente somma armi
  di generazioni diverse o confonde con i pozzi verticali.
- **Il locale:** il compartimento prodiero dei due stagni contiene i «locali di movimentazione delle armi» [WP688]. La
  descrizione di un filmato del Naval Underwater Systems Center (1978 circa) sul *carrello automatico* dei Mk 48 per i
  688 mostra gli elevatori dei siluri e una catena di spinta idraulica che porta il siluro nel tubo; parla di una
  densità di stivaggio progettata di 35 armi, poi ridotta a 30 [PF-AWC]. Questo è il massimo fisico del locale, non il
  carico tipico. [media]
- **Pozzi verticali:** dal SSN-719 (*Providence*, in servizio dal luglio 1985) dodici tubi verticali per i Tomahawk,
  a prua fuori dallo scafo resistente, fra la sfera e lo scafo [WPProv] [FAS688] [NAVY-FF]. Sono **in più** dei 25 da
  tubo: non tolgono posti al locale siluri. I primi due battelli (*Providence* e *Pittsburgh*) hanno una disposizione
  dei pozzi diversa dagli altri [WP688]. [alta]
- **Varianti:** Flight I = 31 battelli senza pozzi; Flight II = 8 battelli con i pozzi e il nocciolo migliorato;
  688I (Flight III) = 23 battelli più silenziosi, con BSY-1, configurazione sotto il ghiaccio e **minamento dai tubi**
  [WP688] [FAS688]. Nel 1989 i 688I erano appena all'inizio: la serie fu ordinata fra il 1982 e il 1989 [WP688].
  [alta per le varianti]
- **Contraddizione sulle mine:** Wikipedia elenca Mk 67 e Mk 60 per tutta la classe [WP688]; la FAS dice che le mine
  stanno nel locale siluri e che il 688I «può posarle dai tubi» come novità [FAS688]; Wikipedia, sulle Mk 67, scrive
  che oggi solo i 688I le possono impiegare [WPSLMM]. Lettura proposta: tutti i 688 potevano imbarcare e posare mine dai
  tubi (lo faceva già lo Sturgeon, [WPSturgeon]); il 688I ha un sistema di posa migliorato. [bassa per la distinzione]

### 1.3 Alfa (progetto 705 e 705K)

- **Tubi:** sei da 533 mm a prua [WPAlfa] [HSAlfa] [RG19] [Legion705]. Una fonte divulgativa elenca anche quattro
  tubi da 400 mm, contraddicendo la propria tabella [NavEnc705]: da scartare. [alta per i sei tubi]
- **Tubi pneumoidraulici** progettati apposta, con il lancio possibile a tutte le quote operative: il primo
  sottomarino sovietico a poterlo fare [HSAlfa] [RG19]. [media]
- **Caricamento:** «completamente automatizzato» [HSAlfa]; due operatori nella centrale scelgono, caricano, impostano e
  lanciano [NavEnc705]. Il locale siluri si apriva solo per la manutenzione [HSAlfa]. Nessuna fonte aperta trovata dà
  il tempo di ricarica. [media per l'automazione, lacuna per il tempo]
- **Carico:** 18 siluri (SET-65 o 53-65K) secondo la maggioranza [WPAlfa] [GS705]; 18–20 SET-65A o SAET-60A nel testo
  di Wikipedia; **12** (6 nei tubi + 6 di riserva) in una guida divulgativa [Legion705] e 12 di riserva in un'altra
  [NavEnc705]. Lettura proposta: **6 nei tubi + 12 di riserva = 18** (la «riserva di 12» di [NavEnc705] torna così).
  [media]
- **Alternative:** 20 VA-111 Shkval «al posto» dei siluri [WPAlfa] [GS705] [NavEnc705]; 18–20 missili SS-N-15 nel
  testo di Wikipedia, ma la loro presenza sull'Alfa è incerta [NavEnc705] [WPVyuga] (§2.11–2.12); **24 mine** (36 nel
  testo di [NavEnc705]). [bassa per le alternative]
- **Equipaggio:** 31–32, tutti ufficiali e sottufficiali [WPAlfa] [HSAlfa]: nessuna squadra di siluristi.
- **705 contro 705K:** differiscono solo per il reattore (OK-550 contro BM-40A) [WPAlfa]: il locale siluri è lo
  stesso.

### 1.4 Victor III, Sturgeon, Trafalgar

- **Victor III:** 4 × 533 mm + 2 × 650 mm a prua; i 650 per i siluri 65-76 e i missili SS-N-16, i 533 per i Type 53 e
  gli SS-N-15/SS-N-21. **24 armi o 36 mine** è la cifra più concorde; la ripartizione 18 + 6 fra 533 e 650 mm è in
  [WPVictor], un'altra pagina dà 16 + 6 [WPVictor]. [media]
- **Sturgeon:** quattro tubi a centro nave come il 688; fino a 21 armi di riserva; tipicamente 4 Harpoon, fino a 8
  Tomahawk, 2–4 SUBROC, mine Mk 67 e Mk 60 in configurazione da minamento al posto dei siluri; 4.714 t in immersione
  [WPSturgeon]. [alta]
- **Trafalgar:** cinque tubi; oggi «stivaggio fino a 30 armi» [WPTraf]; il dossier `01` §2.3 riportava 25–30. Nel 1989
  Tigerfish, Mk 8 e Sub-Harpoon, niente Spearfish né Tomahawk (`01` §2.3). Due lanciatori di esche SSE Mk 8 [WPTraf].
  Il rapporto «mine per arma» della Royal Navy (circa 2 mine per posto) non è verificato qui. [media]

### 1.5 Sistemi di caricamento e tempi

- **Prima dei 688:** paranchi e manovra a mano, **10–20 minuti per tubo** secondo l'abilità della squadra [WPTube].
  [media]
- **688 e successivi:** dal 1976 un sistema idraulico (carrello automatico delle armi, AWC) porta la ricarica a
  **«5 minuti o meno»** per tubo [WPTube]. Il carrello spinge il siluro lungo la rastrelliera su rulli, con una prolunga
  che colma lo spazio fino alla culatta aperta [TML] [PF-AWC]. [media; nessun dato di classe pubblicato]
- **Quante ricariche insieme:** nessuna fonte aperta trovata. Il dossier `01` §2.1 stimava 2–4 min per tubo con una
  squadra. **STIMA: due ricariche contemporanee** sul 688 (una per lato del locale), quattro tubi ricaricati in
  10–12 min. [bassa]
- **Alfa:** caricamento automatico senza squadra; tempo non pubblicato. **STIMA: 60–90 s per tubo, sei tubi
  ricaricabili in parallelo a coppie**, quindi la salva completa ricaricata in 3–5 min; è l'idea del progetto, un
  «intercettore» che spara una salva e si allontana [RG19]. [bassa]
- **Il gioco originale:** armamento e ricarica durano **20 s** per tubo, uguali per tutte le armi (`11` §1.1, il timer
  di `carica_tubo_n`). È il tempo di **armamento** di un tubo già carico (allagare, equalizzare, aprire la porta), non
  quello di una ricarica dalla rastrelliera.

### 1.6 Rumore e transitori della ricarica e del lancio

- I transitori di bordo sono la firma tattica di un lancio: porta del tubo, allagamento, lancio, sbattere di portelli
  (`02` §2.1); +10–30 dB per pochi secondi (`01` §3.3); 120–150 dB per 0,05–5 s, STIMA DI GIOCO, nella libreria di
  `09` §5.1.
- Il **lancio a espulsione d'acqua** (impulso, «water ram») è meno rumoroso del vecchio lancio ad aria compressa, e un
  siluro elettrico può uscire da solo («swim-out») [WPTube] [WPMk48]. Il Mk 48 del 1989 è espulso; l'uscita autonoma
  per il Mk 48 è un programma del 2003 [WPMk48]. [media]
- La **ricarica** dalla rastrelliera è movimento meccanico dentro lo scafo (carrello, catena, culatta): più lenta e più
  lunga del lancio, ma meno forte. Nessuna misura aperta. **STIMA:** colpi di 110–125 dB, 1–3 s, alcuni per ricarica,
  trasmessi dallo scafo; un equipaggio disciplinato li evita a velocità silenziosa. In *Cold Waters* la ricarica si
  ferma in silenzio assoluto [CW-Steam]: è la regola di gioco da copiare (§5.4).
- **Compensazione:** dopo il lancio il tubo si riempie d'acqua di mare, del peso circa uguale a quello dell'arma; a
  porta chiusa l'acqua scende nella cassa di compensazione accanto ai tubi (WRT), quindi peso e assetto quasi non
  cambiano [COD] [WPTube]. [alta per il principio]

---

## 2. Le armi

Per ogni arma: guida, velocità, portata, testata, ingombro (posti) e peso. Lo stato nel 1989 è quello di `01` §1 e §6
salvo dove indicato.

### 2.1 USA: siluro Mk 48 (Mod 4) e Mk 48 ADCAP (Mod 5)

| Voce | Mk 48 Mod 4 | Mk 48 ADCAP |
|---|---|---|
| Servizio | Mk 48 dal 1972; Mod 4 come risposta all'Alfa (1977 in poi) | produzione dal 1985, **in servizio dal 1988** |
| Guida | filoguidato, poi testa attiva o passiva | come il Mod 4, testa a scansione elettronica |
| Velocità | ufficialmente «oltre 28 kn»; stime 55 kn | stime 55–65 kn |
| Portata | ufficialmente «oltre 5 miglia»; stime 38 km a 55 kn, 50 km a 40 kn | 27 km a 65 kn |
| Testata | 293 kg, spoletta di prossimità | come il Mod 4 |
| Lunghezza, diametro | 5,8 m, 533 mm | 5,8 m, 533 mm |
| Peso | **1.558 kg** | **1.676 kg** |
| Posti | 1 | 1 |

Fonti: [WPMk48]; `01` §6.1; la tabella `siluri.toml` del motore (`tools/pipeline/siluri.py`) usa già questi valori.
[alta per peso e dimensioni, media per prestazioni]

### 2.2 USA: UGM-84 Harpoon (Sub-Harpoon)

- Operativo sui sottomarini d'attacco dal **1981**; Block 1B (radente, senza cabrata finale) dal 1982; Block 1C dal 1985
  [DSM84]. [alta]
- **Incapsulato:** il missile sta in una capsula che dopo il lancio dal tubo plana in superficie, ne espelle le
  calotte e accende il razzo [DSM84]. Un posto da tubo per missile.
- Missile: 4,63 m, **690 kg** (Block 1C), Mach 0,85, ~140 km, testata 221 kg; guida inerziale e radar attivo in banda
  J [DSM84]. Il peso della capsula non è pubblicato: **STIMA: capsula e missile circa 1.200–1.400 kg**, cioè vicino al
  peso di un Mk 48 per la compensazione. [alta per il missile, bassa per la capsula]
- Nel 1989 si imbarcava ancora; la FAS oggi lo dà «non più imbarcato» [FAS688].

### 2.3 USA: UGM-109 Tomahawk (TASM e TLAM)

| Variante | Servizio | Peso con booster | Portata | Testata | Guida |
|---|---|---|---|---|---|
| UGM-109B **TASM** (antinave) | marzo 1983, ritirato all'inizio degli anni '90 | 1.590 kg | 460 km | 450 kg | radar simile all'Harpoon, radente |
| UGM-109C **TLAM-C** (attacco a terra) | 1986 | 1.490 kg | 1.250 km | 450 kg | inerziale + TERCOM + DSMAC |
| UGM-109A **TLAM-N** (nucleare) | 1983, ritirato all'inizio degli anni '90 | 1.450 kg | 2.500 km | W80, 5–200 kt | inerziale + TERCOM |

- Lunghezza 6,25 m con il booster. **Dal tubo** in un contenitore che viene espulso, poi il booster porta il missile
  in superficie; dai **pozzi verticali** sui Flight II e successivi [DSM109]. [alta]
- Un posto da tubo per missile; nei pozzi verticali non occupa posti del locale.
- PROPOSTA per il gioco: TLAM-N fuori (arma strategica nucleare); TASM e TLAM-C dentro.

### 2.4 USA: UUM-44 SUBROC (in ritiro)

- Razzo antisommergibile con testata nucleare di profondità, lanciato dal tubo da 21 pollici; primo impiego sul
  *Permit* nel 1964 [WPSubroc]. Servizio dato come **1964–1989**; in altre fonti il ritiro e la cancellazione della
  testata W89 di ricambio arrivano nel 1990 [WPSubroc]. Potenza dichiarata in modo discordante (1–5 kt o 25 kt).
  [media]
- Nel 1989: ancora sugli Sturgeon (2–4) [WPSturgeon], marginale sui 688. PROPOSTA: **fuori dal remake**, come le
  altre armi nucleari.

### 2.5 USA: UUM-125 Sea Lance (non entrato in servizio)

- Concepito nel 1980 come successore di SUBROC e ASROC; contratto Boeing nel 1982; capsula da 21 pollici; la versione
  nucleare (UUM-125A) rinviata, quella convenzionale (UUM-125B) con il siluro leggero Mk 50; **cancellato nel 1990**
  in sviluppo avanzato [WPSeaLance] [DSM125]. [alta]
- **Era nel gioco originale:** due missioni hanno `sea_lance = 8` nella dotazione e il tubo missili caricato con il Sea
  Lance (§4.2). Nel 1989 non esisteva in servizio: è un'arma **«what-if»** da tenere solo con un'opzione (§5.2).

### 2.6 USA: mina Mk 67 SLMM (mobile, lanciata da sottomarino)

- Un Mk 37 modificato fa da veicolo; esce dal tubo e percorre **circa 13 km** (8 miglia) prima di posarsi sul fondo;
  testata 150 kg [WPSLMM]. Lunghezza 4,0–4,1 m, diametro 485 mm, peso **754–800 kg** (le fonti discordano) [FASMk67]
  [WPSLMM]. Servizio dal 1979 (prima produzione) o 1983 (schieramento) [WPSLMM] [FASMk67]. [alta per le dimensioni]
- **Quante per posto:** nessuna fonte aperta trovata. È lunga circa il 70% di un Mk 48 e pesa la metà: **STIMA:
  1 per posto** (un posto da tubo resta un posto), con la variante PROPOSTA di 2 per posto in rastrelliera se si
  vuole il vantaggio delle mine corte delle altre marine (§3.3). [bassa]

### 2.7 USA: mina Mk 60 CAPTOR

- Un siluro leggero Mk 46 in un contenitore ancorato, che si libera quando passa un sottomarino; versione da
  sottomarino **3,35 m e 935 kg** (da aereo e nave 3,68 m e 1.077 kg); diametro 533 mm [WPCaptor]. [alta]
- **STIMA: 1 per posto.** Nel 1989 in servizio (`01` §2.1–2.2).

### 2.8 USA: esche e contromisure

- **Insieme acustico CSA Mk 2:** il sistema con cui i 688 lanciano le contromisure, che servono a deviare i siluri in
  arrivo [GSCSA]. [media]
- **ADC Mk 2:** esca espendibile a batteria, per navi e sottomarini, che si tiene verticale a una quota preimpostata
  con un'elica intubata ed emette il segnale che inganna il siluro [MAEADC]; dati di gioco in `05` §5.1 e nella C6b
  (150 dB, 10 minuti, 60 m). [media]
- **MOSS Mk 70:** veicolo da 10 pollici senza testata che imita il battello con eco attiva e firma passiva, in
  servizio dal 1976, imbarcato su tutti gli SSBN dagli anni '80 fino a metà anni '90 [WPMOSS]. Sui 688 non confermato.
  [media]
- **Lanciatori:** i lanciatori da 3 pollici dei 688 non sono descritti nelle fonti aperte trovate. Le esche non
  occupano posti da siluro: hanno la loro scorta (6 nel gioco originale, §4.2). [bassa]

### 2.9 URSS: siluri da 533 mm per l'Alfa

| Siluro | Tipo | Velocità | Portata | Testata | Note | Conf. |
|---|---|---|---|---|---|---|
| **SET-65** | antisom, testa attiva/passiva | 40 kn | ~16 km | 205 kg | dal 1965 | [media] [NavEnc705] [Legion705] |
| **53-65K** | antinave, segui-scia | 45 kn | 19 km (50 km in [Legion705], dubbio) | 300 kg (400 in [Legion705]) | immune alle esche | [media] `01` §6.1 |
| **SAET-60M** | antisom/antinave passivo | 40 kn | 15 km | 300 kg (100 in [Legion705]) | dal 1969 | [bassa] [NavEnc705] |
| TEST-71M | filoguidato | 40 kn | 15 km | 205 kg | sull'Alfa non confermato | [bassa] `01` §2.4 |
| USET-80 | antisom | 45 kn | 18 km | 250 kg | dal 1980; sull'Alfa non trovato | [bassa] `01` §6.1 |

- Pesi e lunghezze dei siluri sovietici da 533 mm non trovati nelle fonti consultate. **STIMA: 1.800–2.100 kg, 7,2–8 m**
  (sono più lunghi del Mk 48). [bassa]
- La tabella `siluri.toml` del motore ha già SET-65 come TEST-71M e 53-65K e USET-80 (C6b): per l'Alfa del 1989 il
  carico da usare è **SET-65 e 53-65K**, con SAET-60M; USET-80 e TEST-71M solo come varianti.

### 2.10 URSS: VA-111 Shkval

- Siluro supercavitante a razzo, circa 200 kn, portata ~11 km, testata 210 kg [Legion705] [NavEnc705]; servizio dal
  **1977** ma reso noto solo negli anni '90 [WPShkval]. Sull'Alfa «20 Shkval al posto dei siluri» nelle schede
  [WPAlfa] [GS705]; nessuna fonte nomina un battello che li abbia imbarcati davvero, e una fonte lo descrive come arma
  non guidata contro i siluri in arrivo [WPShkval]. [bassa per l'impiego sull'Alfa]
- **Nel remake:** arma segreta del 1989, che il giocatore americano non conosce. PROPOSTA: solo per l'IA in missioni
  scelte, e non nella scheda della libreria americana.

### 2.11 URSS: RPK-2 Vyuga (SS-N-15)

- Missile antisom dal tubo da 533 mm, come il SUBROC; 35–45 km; carica di profondità o testata nucleare da 200 kt;
  servizio dal 1969 (Wikipedia) o dal 1973 (altre fonti) [WPVyuga]. L'elenco delle piattaforme comprende l'Alfa solo in
  Wikipedia; le altre fonti nominano Akula, Oscar, Typhoon, Delta, Kilo e Charlie [WPVyuga]. [bassa per l'Alfa]
- Il successore RPK-6/7 (SS-N-16) usa i tubi da 650 mm (Victor III, Akula, Oscar).
- **Nel remake:** con la testata convenzionale (un siluro) è un'arma dell'IA del Victor III; sull'Alfa come variante.

### 2.12 URSS: mine

- Alfa: 24 mine (36 in una fonte) al posto delle armi [WPAlfa] [NavEnc705]; Victor III 36 [WPVictor]. Tipo citato:
  la mina-siluro **PMR-1** (1972), che dal fondo rileva il bersaglio e lancia un siluro da 400 mm fino a 600 m
  [NavEnc705]. [bassa]
- **Misure del PMR-1 (ricerca del 25/10/2026).** Le fonti russe lo chiamano «mina-razzo antisom» (ПМР-1 «Кальмар»,
  NII-400, oggi TsNII «Gidropribor», in servizio dal 1970): **533 mm, 7.800 mm, 1.700 kg**, carica 200 kg, guida
  acustica; si posa dai tubi da 533 mm dei sottomarini; colpisce sottomarini a 6–30 nodi fra 30 e 400 m [RuPMR1].
  Il successore **PMR-2** (ПМК-1, 1972 o 1973) misura **533 mm, 7.830 mm, 1.850 kg**, carica 175 kg (l'export PMR-2E
  7.890 mm, 300–350 kg); si posa su fondali di 200–1.000 m restando a 200–400 m; colpisce sottomarini a 50–200 m entro
  un settore di 160° verso l'alto con raggio di circa 200 m; il razzo va a 80–100 m/s e l'attacco dura al più 7 s.
  Il raggio del PMR-2 è il doppio di quello del PMR-1, che per coprire le stesse quote voleva due file di mine
  [AMPMR2] [WFPMR2]. [media per le misure: le due mine e le due fonti concordano]
- **Contraddizione con [NavEnc705].** Con queste fonti il PMR-1 è lungo come un siluro da 533 mm (1 posto, non 0,75),
  lancia un razzo e non un siluro da 400 mm, e non attacca le navi. Le 24 mine dell'Alfa al posto di 18 armi
  tornano solo con mine più corte (da fondo o ancorate, due per posto in lunghezza), non con il PMR-1. [media]
  **Correzione applicata (25/10/2026):** nel remake PMR-1 e PMR-2 sono mine-razzo solo antisom da 1 posto
  (`carico.toml`, `[mine.pmr1]` e `[mine.pmr2]`); il dato di [NavEnc705] sul siluro da 400 mm è scartato.
- **Mina corta: MDT** (мина донная трубная). Mina da fondo acustico-induttiva del TsKB-145, in servizio dal 1953:
  **534,5 mm, 2.573-2.575 mm, 1.050 kg**, carica 600 kg (TGAG-5), raggio della spoletta 50 m, fondali **fino a
  50 m**; si posa dai tubi da 533 mm a 4 nodi in superficie e 8 in immersione; **due mine per tubo**, unite da un
  aggancio che si separa al primo lancio [WPMDT] [AMMDT]. Ancora in Sychev (1984) [WPMDT]. Con 0,5 posti l'Alfa ne
  porta 36 al posto di 18 armi, la cifra della seconda fonte di §2.12. [media]
- Rapporto mine/armi: **1,33** (Alfa 24/18), **1,5** (Victor III 36/24), **2** (Trafalgar, se 50/25): le mine
  sovietiche sono più corte dei siluri e ne entrano più di una per posto. [media per i rapporti]

### 2.13 URSS: contromisure

- MG-84 «Korund-705», esca da 400 mm che disturba o imita la firma del battello (1974) [NavEnc705]. Nel gioco
  originale l'Alfa ha 4 esche nel sesto tubo (`11` §1.1). [bassa]

---

## 3. Su cosa influisce il carico

### 3.1 Peso e assetto

- 688: 25 armi da circa 1,6 t (Mk 48 1.558–1.676 kg [WPMk48]; capsule e mine fra 0,8 e 1,6 t) fanno **35–42 t**: lo
  **0,5–0,6%** di 6.927 t in immersione [WP688]. [media, calcolo]
- Alfa: 18 armi da circa 2 t (STIMA §2.9) fanno ~36 t su 3.200: **1,1%**. [bassa, calcolo]
- **Compensazione:** a ogni lancio entra nel tubo acqua di peso circa uguale all'arma, che a porta chiusa va nella
  cassa accanto ai tubi (WRT): peso e assetto restano quasi invariati [COD]. Gli spostamenti dalla rastrelliera al tubo
  si compensano con travasi fra casse o con casse dedicate [COD]. Effetto per il gioco: **nessuna variazione d'assetto
  visibile** con un equipaggio normale. [alta per il principio]
- Con la fisica del battello (C6a, kit D6) il peso netto è già un ingresso della dinamica: un lancio senza
  compensazione (cassa danneggiata o allagata, `docs/10_danni_affondamento_esplosioni.md` §10) potrebbe dare un
  appruamento di qualche decimo di grado.
  PROPOSTA: solo come conseguenza di un guasto, non nel carico normale.

### 3.2 Velocità e manovra

- Lo scarto di massa (0,5–1%) e la posizione delle armi vicino al baricentro (tubi a centro nave sul 688) non cambiano
  in modo percepibile velocità, accelerazione o virata. **Nessun effetto nel gioco.** [media, ragionamento]

### 3.3 Colpi per tipo e posti condivisi

- **È questo il vero effetto del carico:** siluri, missili incapsulati e mine si dividono gli stessi posti. Ogni
  Harpoon o Tomahawk da tubo, ogni mina, è un siluro in meno [WP688] `01` §2.1.
- 688 Flight I: 25 posti per tutto; i Tomahawk da attacco a terra in missioni di colpo tolgono siluri. Flight II: i 12
  Tomahawk stanno nei pozzi, quindi i 25 posti restano per siluri, Harpoon e mine. Sturgeon: fino a 8 Tomahawk e 4
  Harpoon nei 23–25 posti [WPSturgeon].
- Le mine **corte** di alcune marine valgono più di una per posto (rapporti 1,33–2, §2.12); per le Mk 67 e Mk 60 USA
  il rapporto non è pubblicato (§2.6–2.7).

### 3.4 Armi pronte nei tubi

- Al primo contatto conta che cosa c'è nei tubi, perché una ricarica dura minuti (§1.5) e fa rumore (§1.6). La
  prassi aperta non è documentata per il 1989; è ragionevole che in pattugliamento i tubi siano carichi ma non allagati
  (il gioco originale: armamento in 20 s). [bassa]
- Scelte tipiche (PROPOSTA): 3 Mk 48 e 1 Harpoon nei tubi in missioni antinave; 4 Mk 48 in caccia antisom; 2 Mk 48 e 2
  mine all'inizio di un minamento.

### 3.5 Tempi e rumore delle ricariche

- 688: circa 5 min per tubo con il carrello idraulico [WPTube], due alla volta STIMA (§1.5); un transitorio meccanico a
  ogni ricarica, STIMA (§1.6). Alfa: STIMA 60–90 s per tubo, automatico, nessuna squadra da organizzare.
- Effetto tattico: dopo una salva il 688 resta per una decina di minuti con meno tubi pronti, e la ricarica a
  velocità silenziosa è lenta o rumorosa; l'Alfa ricarica in pochi minuti. È la differenza di progetto fra i due
  battelli.

### 3.6 Capacità di missione

- **Minamento:** servono mine a bordo, quindi posti tolti a siluri e missili. Una missione di minamento con 8 mine
  lascia al 688 17 posti (STIMA 1 per posto).
- **Attacco a terra:** TLAM-C dal tubo sui Flight I (posti tolti), dai pozzi sui Flight II. Una salva di 8 Tomahawk da
  tubo su un Flight I lascia 17 posti; su un Flight II non toglie nulla.
- **Antinave a distanza:** TASM (460 km) e Harpoon (140 km) dal tubo, con le regole di lancio di `11` §1.1 (contatto
  solido, almeno 5 nm, quota periscopica).

### 3.7 Nei giochi

- ***Cold Waters* (2017):** si sceglie il tubo e si cicla l'arma da caricare con un tasto; la ricarica richiede tempo e
  **si ferma in silenzio assoluto**; nelle missioni con incursori il carico si riduce (una missione lascia 4 siluri)
  perché gli incursori occupano gli spazi [CW-Steam]. Si può caricare anche trascinando i siluri dal deposito al tubo
  nella vista in sezione [CW-Steam]. [media]
- ***688(I) Hunter/Killer* (1997):** la scheda del gioco dichiara la gestione delle risorse del battello, compreso il
  carico delle armi [688I-Steam]; il manuale non è stato consultato. [bassa]
- ***Sub Command* (2001):** nessuna fonte aperta trovata sulla schermata di carico. [lacuna]
- ***688 Attack Sub* (1989):** il carico è deciso dalla missione (dotazione e tubi), con la ricarica automatica in 20 s
  (§4).

---

## 4. Il motore oggi

### 4.1 Armi e dati già presenti

- `data/motore/siluri.toml` (generato da `tools/pipeline/siluri.py`, letto da `carica_siluri`): Mk 48, ADCAP,
  Tigerfish, Spearfish, USET-80, TEST-71M, 53-65K, Mk 46, AT-1, con anno di servizio, prestazioni, filo e testa. Manca
  SET-65 (l'Alfa usa la classe `type53` delle missioni, mappata su USET-80 nella C6b: §2.9 suggerisce di aggiungere
  SET-65 e SAET-60M per il 1989). Esca ADC Mk 2.
- `data/motore/acustica.toml`: le firme dei siluri in corsa (150–160 dB), utili anche per i transitori del lancio.

### 4.2 Dotazioni delle missioni originali

Conteggio delle `dotazione` nelle 22 missioni (`data/missioni/*.lua`):

| Dotazione | Missioni |
|---|---|
| `mk48 = 20, harpoon = 4, esca = 6` | 8 |
| `type53 = 18, esca = 4` (Alfa) | 5 |
| `mk48 = 16, sea_lance = 8, esca = 6` | 2 |
| `mk48 = 22, harpoon = 2, esca = 6` | 2 |
| `mk48 = 24, esca = 6` | 2 |
| `mk48 = 20, tomahawk = 4, esca = 6` | 1 |
| `mk48 = 8, harpoon = 4, esca = 6` | 1 |
| `type53 = 8, esca = 4` (Alfa) | 1 |

- I tubi del 688 nell'originale sono **sette posizioni**: 4 tubi per i siluri, 2 per le esche e 1 per i missili
  (`{"mk48" ×4, "esca" ×2, "harpoon"|"tomahawk"|"sea_lance"}`); l'Alfa ha 5 tubi per i siluri e il sesto per le esche
  (`11` §1.1). Il «tubo 7» dei missili non esiste sul 688 vero: è una semplificazione del 1989.
- I totali tornano con le fonti: **24 armi da tubo** (20 + 4, 22 + 2, 24 + 0, 16 + 8, 20 + 4) per il 688 e **18** per
  l'Alfa; la missione con 8 + 4 (12) è un carico ridotto voluto dallo scenario.

### 4.3 Regole del classico da non toccare

- Armamento e ricarica in 20 s, ricarica automatica finché c'è scorta, al più 4 armi in acqua (6 sull'Alfa), regole
  dei missili (`11` §1.1). Tutto il carico della Sala siluri vale solo nell'Enhanced e nelle missioni rapide o
  dell'editor; il classico parte con la dotazione della missione.

---

## 5. Proposte per la schermata e le regole

### 5.1 Carichi standard per tipo di missione (PROPOSTA, 688 Flight I, 25 posti)

| Missione | Nei tubi alla partenza | In rastrelliera | Note |
|---|---|---|---|
| Caccia antisom | 4 Mk 48 | 19 Mk 48 + 2 Harpoon | il carico «di default» |
| Antinave | 3 Mk 48 + 1 Harpoon | 13 Mk 48 + 4 Harpoon + 4 TASM | TASM solo fino ai primi anni '90 |
| Attacco a terra (Flight I) | 2 Mk 48 + 2 TLAM-C | 13 Mk 48 + 6 TLAM-C + 2 Harpoon | sui Flight II i TLAM-C vanno nei 12 pozzi |
| Minamento | 2 Mk 48 + 2 Mk 67 | 9 Mk 48 + 10 Mk 67 + 2 Harpoon | 12 mine (STIMA 1 per posto) |
| Scorta e sorveglianza | 4 Mk 48 | 17 Mk 48 + 4 Harpoon | come l'originale (20 + 4, più uno) |
| Alfa (intercettore) | 6 SET-65 / 53-65K | 6 SET-65 + 6 53-65K | 18 armi; nessuna scelta di missili |

- Le esche hanno la loro scorta (6 sul 688, 4 sull'Alfa come nell'originale) e non toccano i posti.
- ADCAP: solo nelle missioni ambientate dal 1988, e in numero limitato (PROPOSTA: al più 6 nel 1989).

### 5.2 Limiti decisi dalla missione

- **Deposito disponibile:** ogni missione dichiara quante armi di ogni tipo ci sono al deposito (originali: la
  `dotazione`, che diventa il **massimo** per tipo e il carico predefinito). PROPOSTA di campo Lua:
  `deposito = { mk48 = 25, harpoon = 4, mk67 = 0, ... }` con `tipi_negati = { "tomahawk" }`.
- **Armi negate o datate:** l'anno della missione nega le armi non ancora in servizio (ADCAP prima del 1988, Spearfish
  sempre nel 1989, `in_servizio_dal` della C6b); il Sea Lance solo con l'opzione «what-if» (§2.5); armi nucleari
  sempre fuori (PROPOSTA di §2.3–2.4).
- **Classe:** i pozzi verticali solo sui Flight II e successivi; il minamento solo se la missione lo prevede (le mine
  si vedono al deposito solo allora); l'Alfa con il suo carico fisso di siluri e mine.
- **Ospiti a bordo:** come in *Cold Waters*, una missione con incursori toglie posti (PROPOSTA: 6 posti per una
  squadra).

### 5.3 Pianta del locale e scheda (PROPOSTA)

- Al centro la pianta: 4 tubi del 688 a centro nave, le rastrelliere ai lati (21 posti), i 12 pozzi a prua sui Flight
  II; 6 tubi dell'Alfa a prua con la rastrelliera del caricatore automatico (12 posti). Trascinare un'arma su un posto
  la carica; un posto da tubo mostra «nel tubo alla partenza».
- A sinistra il deposito con le quantità; in basso la scheda dell'arma con sagoma e dati di §2 (guida, velocità,
  portata, testata, peso, posti). Le schede dicono anche la fonte e la confidenza, come la libreria del sonar (S5b).
- In alto i totali: posti usati / 25, peso totale (informativo, §3.1), pronti nei tubi.

### 5.4 Ricarica come azione (PROPOSTA per l'Enhanced)

- **Ordine:** «ricarica il tubo n con l'arma X» dalla stazione Armi; durata dal tipo di battello e d'arma: **688
  300 s** per tubo, due insieme (STIMA §1.5); **Alfa 75 s** per tubo, sei in parallelo a coppie (STIMA). Missili e
  mine: +20% (capsule più pesanti o manovre diverse, STIMA).
- **Rumore:** durante la ricarica del 688 un transitorio meccanico (STIMA 115 dB, 2 s) ogni 60 s, che entra nel modello
  dei transitori dell'acustica (`Sorgente::transitorio_db`) e quindi nei sensori nemici. In **silenzio** (velocità
  silenziosa ordinata) la ricarica si sospende, come in *Cold Waters* [CW-Steam], oppure va avanti al doppio del tempo
  senza transitori (scelta del giocatore: «ricarica silenziosa»).
- **Armamento:** resta il passo dell'originale, 20 s per allagare, equalizzare e aprire la porta, con il suo transitorio
  (porta del tubo, `02` §2.1).
- **Lancio:** espulsione con il transitorio del lancio; la compensazione automatica (§3.1), salvo guasti.
- Classico: invariato (20 s, automatica, nessun rumore in più).

### 5.5 Legami con il debriefing (PROPOSTA)

- Il registro della partita salva il carico alla partenza e quello che resta. Il debriefing confronta il carico con la
  missione: «partito con 4 Harpoon, nessun bersaglio di superficie», «missione di minamento con 0 mine posate»,
  «ricarica rumorosa durante l'avvicinamento: rilevato 2 minuti dopo».
- Armi negate usate (what-if) e carichi diversi da quelli standard compaiono come nota, senza penalità nel classico.

### 5.6 Dati proposti per il motore (PROPOSTA)

- In `data/motore/` un file `carico.toml` (o una sezione di `siluri.toml`), generato dalla pipeline come gli altri:
  - `[classi.<id>]`: tubi (numero e calibro), posti in rastrelliera, pozzi verticali, secondi di ricarica, ricariche
    contemporanee, transitorio della ricarica, scorta di esche;
  - `[armi.<id>]`: tipo (siluro, missile, mina, esca), posti occupati (1, 0,5 per le mine corte), peso, anno di
    servizio, scheda (guida, velocità, portata, testata), «solo IA», «what-if»;
  - `[carichi.<missione_tipo>]`: i carichi standard di §5.1.
- Le armi già in `siluri.toml` (siluri) e nelle firme (`acustica.toml`) restano lì; `carico.toml` aggiunge solo
  quello che serve alla Sala siluri.

---

## 6. Lacune note

1. **Tempi di ricarica** misurati del 688 (il «5 minuti o meno» è generico per gli SSN dal 1976) e numero di ricariche
   contemporanee; **tempo del caricatore automatico dell'Alfa**: nessuna fonte aperta.
2. **Mine USA per posto** (Mk 67, Mk 60): nessuna fonte aperta.
3. **Carico dell'Alfa**: 18 contro 12, e presenza reale di SS-N-15 e Shkval a bordo.
4. **Pesi e lunghezze dei siluri sovietici** da 533 mm, e del contenitore dell'Harpoon e del Tomahawk.
5. **Rumore della ricarica**: nessuna misura aperta (le stime di §1.6 sono di taratura).
6. **Lanciatori di esche dei 688** (calibro e numero) e uso della MOSS sui 688.
7. **Manuali dei giochi** (*688(I)*, *Sub Command*) per confrontare le regole di carico.

Fonti da cercare per colmarle: Friedman, *U.S. Submarines Since 1945* e *World Naval Weapons Systems* (1989/1991);
Polmar e Moore, *Cold War Submarines* (`01` [PM04]); Apalkov, *Podvodnye lodki* (2003) per il 705; i manuali dei giochi.

---

## 7. Bibliografia

Pagine consultate il 24/10/2026; le cifre riportate sono quelle lette in quella data.

- **[WP688]** Wikipedia (en), *Los Angeles-class submarine* (scheda, sezioni Design e Weapons; cita Winnefeld,
  *Proceedings* 2023; FAS 2000; Polmar 2013). https://en.wikipedia.org/wiki/Los_Angeles-class_submarine
- **[FAS688]** FAS Military Analysis Network, *SSN-688 Los Angeles-class*.
  https://man.fas.org/dod-101/sys/ship/ssn-688.htm
- **[SubSup]** Submarine Industrial Base Council, *Los Angeles class*. https://submarinesuppliers.org/?p=134
- **[NI688]** *The National Interest*, articolo sulla classe Los Angeles. https://nationalinterest.org/node/207223
- **[NAVY-FF]** U.S. Navy, *Fact File: Submarines (SSN, SSBN, SSGN)*.
  https://www.navy.mil/Resources/Fact-Files/Display-FactFiles/Article/2169558/submarines-ssn-ssbn-ssgn/
- **[WPProv]** Wikipedia (en), *USS Providence (SSN-719)*. https://en.wikipedia.org/wiki/USS_Providence_(SSN-719)
- **[WPTube]** Wikipedia (en), *Torpedo tube* (tempi di caricamento, lancio, compensazione).
  https://en.wikipedia.org/wiki/Torpedo_tube
- **[PF-AWC]** Periscope Film, descrizione del filmato *U.S. Navy Automated Mark 48 Torpedo Carrier, Los Angeles 688
  class submarines* (NUSC, 1978 circa). https://stock.periscopefilm.com/?p=356712
- **[TML]** Messaggio sulla lista Torpedo Mailing List (descrizione informale del carrello idraulico).
  https://lists.simplelists.com/tml/cache/11682073/2.html
- **[COD]** USS Cod Submarine Memorial, *Maintaining trim while firing torpedoes*. https://usscod.org/torpedotrim.html
- **[WPAlfa]** Wikipedia (en), *Alfa-class submarine* (cita Apalkov 2003 e *Tayfun* 2002).
  https://en.wikipedia.org/wiki/Alfa-class_submarine
- **[HSAlfa]** H. I. Sutton, Covert Shores, *Alfa class submarine*. https://www.hisutton.com/Alfa_Class_Submarine.html
- **[GS705]** GlobalSecurity, *Project 705 Lira / Alfa class* (risultato di ricerca; pagina di dettaglio non leggibile).
  https://www.globalsecurity.org/military/world/russia/705-design.htm
- **[Legion705]** legion.wplus.net, guida alla classe 705. https://legion.wplus.net/guide/navy/su/705.shtml
- **[NavEnc705]** Naval Encyclopedia, *Project 705 Lira (NATO «Alfa»)*. https://naval-encyclopedia.com/?p=60181
- **[RuPMR1]** Ruwiki, *ПМР-1 (противолодочная мина-ракета)* (scheda: massa, lunghezza, diametro, carica, anno).
  https://ru.ruwiki.ru/wiki/ПМР-1_(противолодочная_мина-ракета) — letta attraverso i risultati di ricerca (la
  pagina rifiuta la lettura diretta).
- **[AMPMR2]** Forum Allmines, *Мина ПМР-2* (messaggi del 15/02/2018 e del 22/02/2018: TTX, storia, confronto con
  il PMR-1; fonte amatoriale). https://forum.allmines.net/viewtopic.php?p=655
- **[WFPMR2]** Warfiles, *Противолодочная мина-ракета ПМР-2Э (ПМК-1)* (dal prospetto d'esportazione).
  https://warfiles.ru/14968-protivolodochnaya-mina-raketa-pmr-2e-pmk-1.html
- **[WPMDT]** Wikipedia (ru), *МДТ (мина)* (scheda; cita V. A. Sychev, *Korabel'noe oruzhie*, DOSAAF 1984).
  https://ru.wikipedia.org/wiki/Мина_МДТ
- **[AMMDT]** Allmines, catalogo, *Мина донная трубная МДТ* (due mine per tubo, velocità di posa).
  https://allmines.net/catalog/russia/mines/mdt/
- **[RG19]** *Rossiyskaya Gazeta*, 22/04/2019, «Podvodnyj istrebitel'».
  https://rg.ru/2019/04/22/podvodnyj-istrebitel-kak-sssr-postroil-ubijcu-podlodok.html
- **[WPSturgeon]** Wikipedia (en), *Sturgeon-class submarine*. https://en.wikipedia.org/wiki/Sturgeon-class_submarine
- **[WPTraf]** Wikipedia (en), *Trafalgar-class submarine*. https://en.wikipedia.org/wiki/Trafalgar-class_submarine
- **[WPVictor]** Wikipedia (en), *Victor-class submarine*; *Soviet submarine K-324*.
  https://en.wikipedia.org/wiki/Victor-class_submarine
- **[NEVictor]** Naval Encyclopedia, *Victor III class submarine (1977)*.
  https://naval-encyclopedia.com/cold-war/ussr/victor-iii-class.php
- **[WPMk48]** Wikipedia (en), *Mark 48 torpedo*. https://en.wikipedia.org/wiki/Mark_48_torpedo
- **[DSM84]** A. Parsch, *Directory of U.S. Military Rockets and Missiles*, AGM/RGM/UGM-84 Harpoon.
  https://designation-systems.net/dusrm/m-84.html
- **[DSM109]** A. Parsch, *Directory of U.S. Military Rockets and Missiles*, AGM/BGM/RGM/UGM-109 Tomahawk.
  https://designation-systems.net/dusrm/m-109.html
- **[WPSubroc]** Wikipedia (en), *UUM-44 SUBROC*. https://en.wikipedia.org/wiki/UUM-44_SUBROC
- **[WPSeaLance]** Wikipedia (en), *UUM-125 Sea Lance*. https://en.wikipedia.org/wiki/UUM-125_Sea_Lance
- **[DSM125]** A. Parsch, *Directory of U.S. Military Rockets and Missiles*, UUM-125 Sea Lance.
  https://designation-systems.net/dusrm/m-125.html
- **[WPSLMM]** Wikipedia (en), *Submarine Launched Mobile Mines*.
  https://en.wikipedia.org/wiki/Submarine_Launched_Mobile_Mines
- **[FASMk67]** FAS Military Analysis Network, *Mk 67 Submarine Launched Mobile Mine*.
  https://man.fas.org/dod-101/sys/ship/weaps/mk-67.htm
- **[WPCaptor]** Wikipedia (en), *Mark 60 CAPTOR*. https://en.wikipedia.org/wiki/Mark_60_CAPTOR
- **[GSCSA]** GlobalSecurity, comunicato NAVSEA del 25/10/2002 sul CSA Mk 2 Mod 1.
  https://www.globalsecurity.org/military/library/news/2002/10/mil-021025-navsea01.htm
- **[MAEADC]** *Military & Aerospace Electronics*, articolo sull'esca ADC Mk 2.
  https://www.militaryaerospace.com/sensors/article/14279356/torpedo-defense-decoy-surface-warships
- **[WPMOSS]** Wikipedia (en), *Mobile submarine simulator*. https://en.wikipedia.org/wiki/Mobile_submarine_simulator
- **[WPVyuga]** Wikipedia (en), *RPK-2 Vyuga*. https://en.wikipedia.org/wiki/RPK-2_Vyuga
- **[WPShkval]** Wikipedia (en), *VA-111 Shkval*. https://en.wikipedia.org/wiki/VA-111_Shkval
- **[CW-Steam]** Discussioni della comunità di *Cold Waters* su Steam (caricamento dei tubi, ricarica in silenzio,
  carico ridotto con gli incursori). https://steamcommunity.com/app/541210/discussions/0/1471969431599926240 e
  https://steamcommunity.com/app/541210/discussions/0/1620600279678729841
- **[688I-Steam]** Pagina Steam di *688(I) Hunter/Killer*. https://store.steampowered.com/app/2900/
- Dossier del progetto: `01_piattaforme_e_armi.md` (§1, §2, §3.3, §6), `02_acustica_oceanografia_dottrina.md` (§2.1),
  `05_missili_e_contromisure.md` (§5), `09_sonar_sistemi_e_interfacce.md` (§5.1); `docs/11_comandi_e_stazioni.md`
  (§1.1); dati del motore `data/motore/siluri.toml`, `acustica.toml`, `data/missioni/*.lua`.
