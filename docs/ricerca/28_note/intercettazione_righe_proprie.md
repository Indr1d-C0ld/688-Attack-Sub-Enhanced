# 28 — Note: ricevitori d'intercettazione acustica, emissioni attive e righe del proprio battello (1985-1991)

> Appunti di ricerca per il remake di *688 Attack Sub*. Completano `01` (WLR-9 in tabella), `02` §1.5 (SL dei sonar
> attivi) e §2.5 (rumore proprio), `06` §1.5 (contro-scoperta, «Enisej»), `17` (AQS-13, boe attive), `18_note/
> transitori_armi_r7.md` §2.4 e §6.3 (teste dei siluri) e `18_note/ascolto_storico.md` §4.3 e §4.6.
> **Quello che è già lì non è ripetuto**: c'è solo una riga di richiamo, quando un dato nuovo lo conferma o lo smentisce.
> Compilato il 28/10/2026. Ho letto solo pagine web aperte e non ho scaricato file. Due PDF sono finiti da soli nella
> cache dello strumento di lettura (Hildebrand 2009 e un articolo vietnamita sul VGS-3): **non li ho letti né usati**.

**Convenzioni** (come in `sovietici.md`).
- **[alta]:** documento ufficiale, brevetto letto, oppure due fonti serie e indipendenti che concordano.
- **[media]:** una buona fonte secondaria.
- **[bassa]:** un forum, un database di un gioco o una pagina a pagamento letta solo in parte.
- *(estr.)*: letto solo nell'estratto del motore di ricerca, perché la pagina è chiusa o è un PDF.
- **STIMA:** nessun dato pubblico. Il valore è una proposta motivata e va trattato come parametro di bilanciamento.

**Che cosa usa già il gioco** (`data/motore/acustica.toml`, `[transitori.ping_testa_*]`). Tutte le teste sono impulsi
larghi 1 kHz, lunghi 10 ms, con `ritmo = { processo = "singolo" }` e sono marcate STIMA:

| Voce | Banda | SL |
|---|---|---|
| Mk 48 | 29,5-30,5 kHz | 210 dB |
| Mk 46 | 34,5-35,5 kHz | 205 dB |
| USET-80 | 24,5-25,5 kHz | 210 dB |
| SET-65 / TEST-71 | 24,5-25,5 kHz | 205 dB |
| AT-1 | 29,5-30,5 kHz | 200 dB |

Non ci sono ancora voci per i sonar di scafo, i sonar a immersione o le boe come sorgenti d'intercettazione. Il ritmo
«singolo» non riproduce l'accorciamento del PRI (§3.3).

---

## 0. Le scoperte in breve

1. **Banda del WLR-9: nessuna fonte aperta.** Di certo si sa che il WLR-9 è il **primo** di una serie di ricevitori
   d'intercettazione per sottomarini (AIR). Ha idrofoni in due cupole, una nella vela. **Classifica i segnali per
   frequenza, durata dell'impulso, livello e rilevamento relativo**, e «le versioni correnti sono automatiche».
   La serie è WLR-9 → WLR-9A/B → WLR-12 → WLR-17. Il WLY-1 la sostituisce dal 1999 e aggiunge il riconoscimento
   automatico delle piattaforme e dei siluri, più la misura della distanza.
2. **Disposizione degli idrofoni** (un forum, che cita Friedman):
   - un idrofono **a bassa frequenza a prua**;
   - piccole carenature **ad alta frequenza sopra la vela e lungo la chiglia**;
   - la stessa disposizione dal WLR-9 al WLR-17.

   Friedman nota che il Los Angeles «non mostra una cupola separata» e pensa a finestre acustiche nella vela.
3. **Il lato sovietico è documentato meglio di quanto si pensasse.** Kazakov (2019) descrive la storia della funzione
   **ОГС** (обнаружение гидроакустических сигналов, «scoperta dei segnali idroacustici», cioè l'intercettazione).
   Le tappe:
   - **«Свет» MG-13** (SSK): misura rilevamento e frequenza, con 4 ricevitori a 0°, ±90° e 180°;
   - **«Рубин»**: 2 bande e 20 filtri, allarme a tiratroni, troppi falsi allarmi;
   - **«Рубикон»**: 12 filtri, CAG, potenzialoscopio;
   - **«Скат»** (671RTM, 945, 971). È il primo che misura **frequenza portante, deviazione di frequenza (FM), durata,
     intervallo di ripetizione e livello**. Ha un rilevatore di precisione, un registratore ad anello di 5 s e raggruppa
     i segnali per bersaglio.
4. **«Enisej» sull'Alfa: le fonti non concordano.** `06` §1.5 lo chiama ricevitore d'intercettazione, seguendo
   en.wikipedia. Le fonti russe usate in `19` §3.3 lo chiamano **stazione passiva di scoperta** dentro il complesso
   «Океан». Lo stesso vale per l'**MT-70** dell'Akula: en.wikipedia lo dà come «sonar intercept receiver», `18_note`
   come sonar per il ghiaccio. In entrambi i casi la funzione ОГС esisteva comunque dentro il complesso sonar.
5. **Testa dei siluri: i brevetti USN danno l'ordine di grandezza.**
   - Wells, 1960: 20-80 kHz, larghezza 5 kHz, ping di 17 ms, ascolto di ~1 s.
   - Heinz, 1964 (già in r7): 2 ms ogni 1,25 s, con la cadenza che **raddoppia** quando l'eco torna sotto ~0,5 s.

   Le frequenze reali di Mk 46, Mk 48, ADCAP e delle teste sovietiche **non sono pubbliche**.
6. **Righe del proprio battello:**
   - Miasnikov conferma le righe di rete a **50 Hz (URSS) e 60 Hz (USA)** legate ai generatori, con fino a 5 armoniche
     registrate sugli SSN di prima generazione;
   - la riduzione delle tolleranze dei denti del riduttore da 0,1 a 0,01 mm vale **30-40 dB** (Parkhomenko, 1993);
   - sotto ~8 kn domina il macchinario, sopra domina il flusso, che cresce come V⁶.

   Le pompe del primario sono «la sorgente acustica più vulnerabile» di un battello nucleare. La circolazione
   naturale le rende superflue solo in una fascia intermedia di potenza.
7. **Accoppiamento:**
   - la sfera del 688 sta più a prua possibile, «lontana dal rumore proprio» (FAS);
   - la schiera trainata sente il proprio battello attraverso l'acqua, **vicino al fuoco anteriore** (endfire) e per
     percorsi multipli. In acqua profonda il rumore può spezzarsi su rilevamenti fra 10° e 90° dal fuoco;
   - la cancellazione adattiva toglie 20-25 dB.

   Isolamento delle macchine: zattera ~35 dB *(estr.)*, doppio montaggio ~25 dB.

---

## 1. Ricevitori d'intercettazione acustica statunitensi

### 1.1 Che cosa si sa (fonti)

| Dato | Valore | Fonte | Conf. |
|---|---|---|---|
| Natura | «first of a series of acoustic intercept receivers (AIR) for submarines» | [MPWLR9] | media (pagina a pagamento, letta solo l'apertura) |
| Composizione | idrofoni in **due cupole, una nella vela**; ricevitore e processore; display | [MPWLR9] | media |
| Parametri misurati | **frequenza, durata dell'impulso, livello, rilevamento relativo**; «current versions are automatic» | [MPWLR9] | media |
| Compito | scoperta dei sonar attivi di ricerca e dei siluri a autoguida acustica | [WPLA] che cita Polmar, *Proceedings*, ott. 1979, p. 137; [USNI79] *(estr.)* | media |
| Serie | WLR-9, WLR-9A, WLR-9B (database del gioco), WLR-12, WLR-17; tutti sui 688, poi su SSN-21 e NSSN | [FASWLY1]; [CMANO] | alta per WLR-9/12/17, bassa per le lettere A e B |
| Idrofoni | **un idrofono LF a prua** (dal WLR-9 almeno fino al WLR-17); carenature HF più piccole sopra la vela e lungo la chiglia; la stessa disposizione dal WLR-9 al WLR-17 | [RCSUB] (utente «CC Clarke», post 46 e 53) | bassa |
| Friedman | «Los Angeles shows no separate sonar dome for an acoustic intercept receiver»; ipotesi di schiere del WLR-12 dietro finestre nella vela, forse anche a poppa della vela | [RCSUB] che cita Friedman, *U.S. Submarines Since 1945* | media (citazione di seconda mano) |
| Successore | **AN/WLY-1**, contratto LRIP del 4-2-1999 a Northrop Grumman Norden. Aggiunge «threat platform sonar and torpedo recognition», «radius of curvature and multipath ranging» e il comando automatico delle contromisure | [FASWLY1] | alta |
| Portata | 27,8 km (CMANO); 15 nmi (Baloogan) | [CMANO] [BALO] | bassa (database di gioco, numeri in disaccordo) |
| Banda di frequenza | **non trovata** in nessuna fonte aperta. In `09` §5 c'è «3,5-25 kHz in catalogo», senza fonte | — | n.v. |

**Che cosa ne segue** (deduzione, media):
- il WLY-1 aggiunge il riconoscimento dei siluri e la distanza. Quindi il WLR-9 e i suoi successori davano soprattutto
  **misure** (f, durata, livello, rilevamento) e lasciavano la classificazione «siluro o nave» all'operatore e a regole
  semplici;
- la parola «automatic» indica scoperta e misura automatiche, con un allarme, non un riconoscimento del tipo d'arma.

### 1.2 Funzioni d'intercettazione del BQQ-5

FAS indica solo un sonar «low frequency passive and active» e, fra le modifiche dell'ECP 7001, la «Low Frequency
Active Interference Rejection» [FASBQQ5] (alta). **Nessuna fonte aperta descrive una funzione d'intercettazione
dentro il BQQ-5.**

Sul piano fisico, la sfera e la schiera conforme vedono comunque gli impulsi attivi a media frequenza (1-10 kHz) come
lampi sul BTR e righe brevi sul gram (`09` §5.1). Gli impulsi sopra la banda passiva, cioè le teste dei siluri a
20-40 kHz, restano invece al WLR-9.

**STIMA:** nel gioco l'intercettazione MF compare sia sul BQQ-5 sia sul WLR-9; quella HF solo sul WLR-9.

### 1.3 Modello di gioco proposto per il WLR-9 (STIMA)

Il riferimento è il «Свет» sovietico (§2), che misurava già nel 1950-60 il rilevamento con 4 ricevitori a quadranti e
la frequenza.

| Parametro | Proposta | Motivo |
|---|---|---|
| Bande | **B1** 1-6 kHz (sonar di scafo LF/MF; idrofono di prua) · **B2** 6-15 kHz (sonar a immersione, DICASS, sonar di fregata) · **B3** 15-45 kHz (boe SSQ-47, teste dei siluri, BQS-15, ecoscandagli) · **B4** 45-100 kHz (sonar antimine, canali di scia dei siluri sovietici). Il 1989 si ferma a B3 + B4 «grezza» | i sonar realmente in mare (§3); brevetto Wells: teste a 20-80 kHz [Wel60] |
| Rilevamento | B1: ±10-15° (un solo idrofono di prua più la sfera); B2-B4: **±5-8°** (confronto d'ampiezza fra carenature di vela e chiglia) | Свет a 4 ricevitori [KAZ19]; nessun numero USN |
| Misure per intercettazione | ora, frequenza (risoluzione ~100 Hz), durata (ms), PRI (s), livello (dB sopra soglia), rilevamento, CW o FM (dal 1989 in poi, come lo «Скат») | [MPWLR9] [KAZ19] |
| Display | elenco a righe in fosforo verde, una per emittente: «BRG 047 · 3.5 kHz · 1.0 s · PRI 24 s · +18 dB · CW». Un istogramma di livello per banda e una lampada con suono per allarme | stile di `19` §2.3 e §7 |
| Soglia | DT ≈ 10 dB su banda di ~1 kHz; portata con `06` §1.5 | `02` §1.5 |
| Allarme siluro | regola in §3.3 | — |

---

## 2. Ricevitori d'intercettazione sovietici (funzione ОГС)

Fonte principale: Kazakov, «Развитие аппаратуры обнаружения гидроакустических сигналов в период с 1950 по 2005 годы»,
2019 [KAZ19]. È una storia aziendale (Морфизприбор): alta sul testo, media come prova di prestazioni.

| Stazione o complesso | Piattaforme | Che cosa misurava e come | Note |
|---|---|---|---|
| **«Свет» MG-13** | SSK diesel (antenna in carenatura sul ponte di prua) | **due parametri: angolo di rotta (КУ) e frequenza**; 4 ricevitori magnetostrittivi con assi a 0°, ±90° e 180°; CRT, frequenza letta su una scala circolare | — |
| **ОГС di «Рубин»** | 661, 671 (prove nel 1965 su un 611 modificato) | gamma in **2 bande**; canali spaziali da 4 a 20; **20 filtri a pari larghezza relativa**; scoperta automatica con soglie a tiratroni (si accendeva il tiratrone, una scala dava il rilevamento) | alle prime prove **troppi falsi allarmi**, per gli impulsi del rumore reale |
| **ОГС di «Рубикон» (MGK-400)** | diesel e nucleari, export | 6 canali spaziali per banda, 12 filtri, CAG, potenzialoscopio | — |
| **ОГС di «Скат»** (dal 1968; 671RTM, 945, 941) | prove di Stato nel Mare di Norvegia | **rilevatore di precisione** (accuratezza «vicina a quella del rilevamento passivo»); misura **frequenza portante, deviazione, durata, intervallo di ripetizione, livello** «con alta precisione»; **registratore di guardia ad anello di 5 s** per ogni segnale; un calcolatore raggruppa i segnali per bersaglio; monitor NPO «Mars» con moduli e spettri | «supera le zone d'ombra» che fermano i sonar attivi di ricerca. Le versioni Скат-БДРМ e Плавник non hanno il rilevatore di precisione |
| **«Скат-3»** (dal 1980; 971) | Akula | — | all'inizio **falsi allarmi frequenti da rumore impulsivo**, poi ricondotti a **sorgenti del proprio battello** (cfr. §5) |
| MGK-400EM (moderno) | 877/636 export | ~100 componenti spettrali in FFT (prima 12); soglia regolabile dall'operatore | fuori periodo |
| «Енисей» (705) | Alfa | conflitto: stazione passiva di scoperta [TTX in `19`] o intercettatore [WPAlfa in `06`] | media e bassa |
| MT-70 | Akula | «Sonar intercept receiver» [WPAkula], ma in `18_note` è sonar da ghiaccio | bassa, da chiarire |

**Per il gioco** (deduzione, media):
- dal Victor III (Скат) in poi i sovietici misurano **durata, PRI e FM** e registrano ogni intercettazione;
- dal Victor I/II (Рубин) in giù hanno solo frequenza, rilevamento e un allarme, con molti falsi allarmi;
- per la **simmetria dell'IA**, l'Alfa e i Victor I/II classificano peggio un ping del 688, mentre Victor III e Akula
  lo classificano bene;
- frequenze, bande e portate sovietiche: **n.v.**

---

## 3. Emissioni attive intercettabili (1985-1991)

### 3.1 Tabella d'insieme

Le righe marcate «→ `NN`» riprendono dati già raccolti nel dossier, con la loro confidenza.

| Emittente | Piattaforma | Frequenza | Impulso (tipo / durata / PRI) | SL (dB re 1 µPa @1 m) | Fonte | Conf. |
|---|---|---|---|---|---|---|
| **AN/SQS-53C** | CG-47, DDG-51, DD-963 | 2,6 e/o 3,3 kHz; «circa 3 kHz di centro» | **FM 1-2 s**; ripetizione **20-30 s**; fascio verticale ~40° inclinato di 3° | **235** nominale | [DOSITS] [SBIR53]; → `09` [Hild09] | alta (SL, f) / media (PRI) |
| AN/SQS-26 (AXR/BX/CX) | FF-1052, CG più vecchi | attivo 3-4 kHz (3,5 kHz nei rapporti DTIC sul *bottom bounce*), passivo 1,5 kHz | modi DP, BB, BB a tre frequenze, CZ, omni | ≈ 230-235 STIMA | [WPSQS26] → `02` §1.5 | media / STIMA |
| **AN/SQS-56** | FFG-7 | la variante polacca ammodernata: **6,7 / 7,5 / 8,4 kHz**, ricezione 6,2-8,9 kHz; per la USN «classified» | CW o FM, PRI da scala di distanza STIMA | ≈ 220-225 STIMA | [MARSZAL] *(estr.)*; [GOAEIS] *(estr.)* | media (f, variante estera) |
| AN/BQQ-5 attivo (sfera) | 688 | ~3,5 kHz | ping singolo «per il tiro» | ≈ 220-230 | → `02` §1.5 | bassa |
| AN/BQS-15 | 688 (antimine e ghiaccio) | «**> 10 kHz**», classificata | impulsi brevi HF, settore prodiero | ≈ 205-215 STIMA | [NCBQS15] *(estr.)*; [FASBQS15] | media (limite inferiore) |
| **AN/AQS-13B** | SH-3H | **9,230 / 10,000 / 10,770 kHz** (±15 Hz), singole o in sequenza | **3,5 o 35 ms**; ping ogni 6-10 s; tonfo → 1-3 min di silenzio → ping → recupero | ≥ 211 | → `17` [MILAQS13B] | alta |
| **SSQ-47B** | P-3, S-3, SH | una di 6 frequenze fra **13 e 19 kHz** | **CW ogni 10 s**, ~30 min | ≈ 200 STIMA | → `17` [HOLLER] | alta |
| **SSQ-62B DICASS** | P-3, S-3, SH-60B | **6,5 / 7,5 / 8,5 / 9,5 kHz** | **CW o FM** a comando; 100 ping-secondi | ≈ 200-205 STIMA | → `17` [FASSSQ62] | alta (f) |
| Mk 46 (testa) | aerei, navi | n.v. (il gioco usa 35 kHz) | ricerca a serpentina o a cerchio, acquisizione ~1600 yd | n.v. | → r7 [VP4] | — |
| Mk 48 / ADCAP | 688 | n.v. (il gioco usa 30 kHz); l'ADCAP CBASS ha «frequency agility», larga banda | acquisizione Mod 0 ~4000 yd (non ufficiale) | n.v. | [NW-US]; → r7 | media (qualitativo) |
| Teste generiche USN, ~1960 | — | **20-80 kHz**, larghezza ~5 kHz | **ping di 17 ms**, 17 ms di riverbero, **~1 s di ascolto**; dwell minimo dell'eco 3 ms | — | [Wel60] | alta (brevetto, valori «presunti») |
| Teste generiche, 1964 | — | — | **2 ms ogni 1,25 s**; sotto ~0,5 s d'eco (≈ 380 m) **cadenza doppia** | — | → r7 [Hei64] | media |
| Titan-2 MG-332, Platina MGK-335, Polinom | Kara, Krivak, Slava, Udaloy, Kirov | n.v. (STIMA 3-8 kHz) | n.v. | 220-235 STIMA | → `02` §1.5, `01` | bassa |
| VGS-2 «Oka» / VGS-3 | Ka-25 / Ka-27 | n.v. | VGS-3: 7 modi, fino a 150 m | n.v. | → `01`, `17` | — |
| RGB-55A (boa attiva) | Tu-142M, Il-38 | 4 frequenze, n.v. | fino a 1 h | n.v. | → `17` | media |
| SET-65, TEST-71, USET-80, AT-1, APR-2 | sovietici | n.v. (il gioco usa 25-30 kHz) | portata d'aggancio 0,5-1 km | n.v. | → r7 [NW-RU] | — |
| 53-65K (scia) | sovietici | canale di scia rivolto **verso l'alto**: dal sottomarino non si sente come ping diretto | — | — | → r7 | media (principio) |

### 3.2 Tipi d'impulso (riassunto)

- **CW:** SSQ-47B (sempre), AQS-13 (impulsi a toni fissi di 3,5 e 35 ms), DICASS (a scelta), ricerca delle teste.
- **FM:** SQS-53C (FM di 1-2 s documentato), DICASS (a scelta), ADCAP (larga banda), Mk 50 (FM, solo da forum).
- **Su un ricevitore che misura la deviazione, come lo Скат:** un CW identifica una piattaforma vecchia o una boa; un FM
  identifica un sonar moderno o una testa d'arma in fase finale.

### 3.3 Cadenza delle teste e logica d'allarme siluro

**Fatti** [alta, brevetti]:
- in ricerca l'intervallo è fissato dalla portata massima voluta: ~1-1,25 s per le teste leggere degli anni '60
  [Wel60] [Hei64];
- dopo l'acquisizione la finestra si accorcia: la cadenza **raddoppia** sotto ~380 m [Hei64];
- il PRI segue la distanza, `PRI ≈ 1,2·2R/c` (r7 §2.4, deduzione).

**Logica proposta per l'allarme del WLR-9** (STIMA, dalla fisica e dai brevetti). Il WLY-1 aggiunge il
riconoscimento dei siluri, quindi il WLR-9 usava regole semplici più l'orecchio dell'operatore. Le regole:

1. **Banda:** f ≥ 15 kHz e durata ≤ 50 ms → «possibile testa d'arma». Sotto i 15 kHz si classifica come sonar di
   piattaforma.
2. **Cadenza:** PRI ≤ 3 s → ricerca di un'arma. Se tre PRI consecutivi **calano** di oltre il 15% → «ACQUISIZIONE».
3. **Livello:** aumento ≥ 3 dB in 10 s, oppure livello ricevuto ≥ SL − 20·log(1000 m) → testa vicina.
4. **Rilevamento:** deriva < 2°/min con livello in aumento → rotta di collisione (la logica del brevetto Atlas usa
   invece la cotangente del rilevamento e un'apertura fra 80° e 120° per i siluri passivi [ATLAS07]).
5. **Doppler:** per l'intercettazione (sola andata) la frequenza ricevuta sale di Δf/f ≈ v/c. Con v = 25 m/s e
   f = 30 kHz fa circa **+0,5 kHz**, sopra la risoluzione proposta di 100 Hz. Il 2v/c vale solo per l'eco. Nel gioco
   basta il segno: frequenza sopra la nominale = arma in avvicinamento.
6. **Livelli d'allarme:** 1 + 2 → «TORPEDO IN WATER» (lampada gialla e suono); 1 + 2 + 3 → «TORPEDO ACQUIRED»
   (lampada rossa e suono continuo).

Il «suono» d'allarme del WLR-9 non è documentato in fonti aperte. Il ping si rende in cuffia **trasposto a 1-3 kHz**,
con la cadenza vera (r7).

---

## 4. Righe del proprio battello su un LOFAR di SSN

### 4.1 Fonti e principi

| Fatto | Fonte | Conf. |
|---|---|---|
| Quattro meccanismi: vibrazione dello scafo, apparato motore, eliche, flusso. A bassa velocità dominano eliche e apparato motore, in transito domina il flusso | [MIA] | media |
| Righe di **rete a 50 Hz (URSS) e 60 Hz (USA)** dai generatori; righe di scafo e di apparato motore «da pochi Hz a qualche centinaio di Hz»; **fino a 5 armoniche** sugli SSN di 1ª generazione | [MIA] (cita Gorbachev 1994) | media |
| Battelli di 4ª generazione (fine anni '80): nessuna riga discreta sopra 100 Hz, il che vale meno sotto 8 kn | [MIA] | bassa (affermazione russa) |
| Le righe discrete più forti stanno ~20 dB sopra il fondo continuo a 1 kHz in 1 Hz | [MIA] → già in `01` | media |
| **Riduttore**: tolleranza dei denti da 0,1 a 0,01 mm = **−30-40 dB** | [MIA] che cita Parkhomenko 1993 | media |
| Il rumore cresce in media di ~5 dB nella vita del battello (usura e squilibri) | [MIA] | bassa |
| Velocità: ~4 kn «ultrasilenzioso»; ~8 kn massima velocità silenziosa (+5-10 dB); ~15 kn transito (+15-20 dB), flusso ∝ V⁶ (+18 dB per raddoppio) | [MIA] | media |
| **Pompe del primario**: «possono essere la sorgente acustica più vulnerabile» di un battello nucleare. Servono a bassissima potenza e ad alta potenza; in una fascia intermedia (Narwhal, S5G) si può fare a meno | [TWZ20] | media |
| Riduttore: «ogni accoppiamento metallo su metallo è una sorgente di rumore»; il Narwhal aveva la turbina in presa diretta, non ripresa per dimensioni e costo | [TWZ20] | media |
| La circolazione naturale passa all'S8G (Ohio), all'S6W e all'S9G; **l'S6G del 688 ha pompe** (non risulta la circolazione naturale) | [TWZ20]; [WPS5G] | media |
| Il 688 ha i **turbogeneratori di servizio (SSTG)** in sala macchine | [FAS688] | alta |
| Il rumore proprio cresce con la velocità e **dipende dall'assetto dell'impianto e dalla «quiet ship condition»**. Il **Quiet Ship Bill** dice quali macchine tenere in moto a ogni velocità, in base ai confronti di rumore proprio fra configurazioni | [FASDOT] (manuale USN per ufficiali di superficie) | alta (concetto) |
| Isolamento: supporti resilienti, DIM, collegamenti flessibili; **verifiche semestrali**; i dispositivi non vanno verniciati | [FASDOT] | alta |
| Monitoraggio del rumore per via strutturale: DTNSRDC, 1978: **50-200 accelerometri** fissi, analizzatori in 1/3 d'ottava e a banda stretta | [TRID78] *(estr.)* | media |

### 4.2 Catalogo delle righe di un 688 (STIMA ragionata)

**Nessuna frequenza di macchina del 688 è pubblica.** Sotto ci sono formule fisiche alte e valori di gioco STIMA. Le
formule per rete, riduttore e numero di poli sono in `26_note/rumore_navi.md`.

| Sorgente | Riga | Dipende da | Valore proposto (STIMA) | Nota |
|---|---|---|---|---|
| SSTG (turbina e alternatore a 2 poli) | rotazione = **60 Hz**, rete 60 Hz, **120 Hz** (magnetostrizione) e 180 Hz | carico elettrico, non velocità; ±0,3 Hz (`18` [MIL1399]) | 60 / 120 / 180 Hz, −0, −6, −12 dB | 3600 giri/min è la velocità di un alternatore a 2 poli a 60 Hz [alta, fisica]. Che gli SSTG del 688 girino così è n.v. |
| Gruppi motore-generatore 60→400 Hz | 400 Hz e 800 Hz; rotazione del gruppo | sempre in moto con l'elettronica accesa | 400 Hz, −10 dB rispetto alla 60 | `02` §2.1: USN a 60 Hz e 400 Hz [media] |
| **Pompe del primario (RCP)**, 2 velocità | rotazione (motore a induzione 60 Hz: ~29,5 Hz veloce / ~14,8 Hz lenta) e passo delle pale della girante (×5-7: **~150-210 Hz** veloce / **~75-105 Hz** lenta) | **regime scelto** (lenta fino a ~15-20 kn, veloce sopra), non la velocità continua | lenta: righe deboli; veloce: +8-12 dB e righe nuove | regimi «spente/lente/veloci» già previsti in `18_audio_reale.md` §3 e `02` §10.3; numeri di poli e pale STIMA |
| Turbine principali e riduttore (doppia riduzione) | rotazione della turbina (~50-100 Hz a piena potenza) e **ingranamento** (denti × giri: centinaia di Hz, fino a ~1-2 kHz) | **proporzionali ai giri dell'asse**, quindi alla velocità | a 32 kn mesh 1° stadio ~1,2 kHz, 2° stadio ~250 Hz; a 5 kn ~1/6 | i giri della turbina navale stanno fra 3000 e 6000 giri/min [EUGENE] (navi di superficie) |
| Asse ed elica | rotazione dell'asse 0,1-3 Hz; **linea di pala = 7 × giri/60** (688: 7 pale, `01`) | velocità; più forte con la cavitazione | 32 kn ≈ 180 giri/min → BR ≈ 21 Hz; 5 kn → ~3,3 Hz | sul proprio LOFAR esce soprattutto in **DEMON** e nei baffles |
| Pompe dell'acqua di mare (circolazione dei condensatori) | rotazione e passo delle pale | regime (lenta/veloce) e quota (contropressione) | 20-30 Hz e 100-200 Hz | — |
| Condizionamento (compressori centrifughi R-114 o R-12) | rotazione e passo delle pale; on/off ciclico | carico termico | ~60 Hz e armoniche, ciclo di 10-30 min | n.v. |
| Impianto idraulico (pompe principali e accumulatori) | transitorio di ricarica degli accumulatori; righe della pompa | uso dei timoni, del periscopio e dei portelli | rampa di 5-30 s dopo ogni manovra | già nei transitori del proprio battello (r7 §5) |
| Pompe di assetto e di esaurimento, compressori d'aria HP, ventilatori | righe e banda larga | solo quando in moto | da spegnere in «ultra quiet» | [FASDOT] (elenco delle macchine) |
| Righe di flusso (FIR) e risonanze | righe che compaiono a certe velocità | velocità | 1-2 righe in finestre di ±1 kn | il «singing» delle eliche esiste solo in una banda stretta di velocità [FASDOT] |

### 4.3 Come le gestisce il sonarista (fonti e proposta)

- **Fatti** [alta, concetto]:
  - Quiet Ship Bill e scelta dell'assetto (§4.1);
  - verifiche periodiche del rumore proprio (per le navi di superficie: fondali > 600 ft, mare ≤ 2) [FASDOT];
  - sistemi moderni di monitoraggio del rumore proprio (Atlas CIRCE, B&K): idrofoni di rumore proprio esterni allo
    scafo e accelerometri, correlati con lo stato delle macchine; analisi a larga e a banda stretta e DEMON
    [CIRCE] [BK] (moderni, media).
- **Errore tipico già in `09` §3.9:** scambiare le righe del proprio battello «non tolte» per un contatto.
- **Proposta per il gioco (STIMA):**
  - **catalogo del rumore proprio** in una pagina della consolle. Per ogni macchina in moto: frequenza, livello e
    «MASK ON/OFF». Le righe mascherate restano sul gram in grigio tenue;
  - le righe del catalogo cambiano con l'assetto (pompe lenta/veloce, SSTG 1 o 2, condizionamento);
  - **una riga che entra fuori catalogo** sulla sfera, a tutti i rilevamenti con lo stesso livello, è proprio rumore:
    il sonarista lo riconosce perché è omnidirezionale. Una riga di un contatto invece ha un rilevamento;
  - per la simmetria dell'IA, anche i sonaristi sovietici hanno il problema (lo Скат-3 con falsi allarmi da sorgenti
    di bordo [KAZ19]).

---

## 5. Vie d'accoppiamento verso i sensori del proprio battello

| Sensore | Via dominante | Dati | Fonte | Conf. |
|---|---|---|---|---|
| Sfera di prua (BQS-13) | **strutturale** (macchine → scafo → struttura della cupola) sotto ~250 Hz; **flusso** sulla cupola sopra velocità medie; elica nei baffles | la sfera sta più a prua possibile, «as far as possible from the ship's own noise» | [FAS688]; [DAGA21] (strutturale < 250 Hz, navi); `02` §2.5 | alta (principio) |
| Schiera conforme di scafo | come la sfera, ma **più vicina alle macchine** → righe proprie più forti (STIMA +6-10 dB) | — | concetto da [AT19] («the noise of the submarine picked up by the sensors mounted on the hull») | media / STIMA |
| Intercettatori nella vela e sulla chiglia (WLR-9) | strutturale e flusso; le teste HF (> 15 kHz) stanno sopra quasi tutte le righe di macchina | — | deduzione | media |
| **Schiera trainata (TB-16)** | **attraverso l'acqua**, diretta e per riflessione sulla superficie e sul fondo; massima **vicino al fuoco anteriore**; in acqua profonda può **dividersi su rilevamenti fra 10° e 90° dal fuoco** | il rumore del rimorchiatore si vede «at, and near, the forward end-fire direction»; cancellazione adattiva **20-25 dB** (simulazione); canceller a banda stretta «oltre 25 dB» (1977) | [CURTIN] *(estr.)*; [CPB22] *(estr.)*; [DRDO] *(estr.)*; [HSTA77] *(estr.)* | media |
| Schiera trainata: rumore meccanico | vibrazione del cavo («strum») che entra nel fascio anteriore e **dipende fortemente dalla velocità di traino** | — | [LL-ADA422438] *(estr.)* | media |

**Numeri d'isolamento** (navali, non del 688):

| Misura | Guadagno | Fonte | Conf. |
|---|---|---|---|
| Zattera flottante (USA pionieri sui sottomarini) | **~35 dB** sul rumore irradiato | [RAMS22] *(estr.; pagina non aperta)* | bassa |
| Doppio montaggio di un compressore HP | 25 dB di vibrazione | [DRDO-HP] *(estr.)* | media |
| Supporto elastico integrato della linea d'assi (sottomarino, 15-100 Hz) | 10,9 dB | [ATL18] *(estr.)* | bassa |
| Tolleranze del riduttore 0,1 → 0,01 mm | 30-40 dB | [MIA] | media |

**Proposta per il motore (STIMA)**, coerente con `02` §2.5:
- `NL_proprio,sfera(f) = max(L_riga_i − I_sfera, NL_flusso(V))`, con I_sfera ≈ 25-35 dB fra sorgente e idrofono;
- per la conforme I ≈ 18-25 dB;
- per la trainata il rumore proprio entra solo nei fasci entro **±30° dal fuoco anteriore**, attenuato di
  `20·log(L_cavo)` più 10-20 dB di direttività. Con cavo di 800 m vale circa −58 dB più la direttività. Il gioco
  aggiunge un'eco su un rilevamento falso a 10-90° in acqua profonda (fenomeno [CPB22]).

---

## 6. Lacune

1. **Banda, numero di bande, accuratezza e sensibilità del WLR-9 e del WLR-12.** Nessuna fonte aperta. Da cercare:
   Friedman, *U.S. Submarines Since 1945* e *Naval Institute Guide to World Naval Weapon Systems* (1989); Polmar,
   *Proceedings*, ott. 1979, p. 137 (403); Military Periscope (a pagamento).
2. **Display e allarme del WLR-9** (aspetto, suono, lampade): nulla. Il WLR-8, l'ESM radar, è descritto in `19`;
   non va confuso.
3. **Frequenze delle teste dei siluri** (Mk 46, Mk 48, ADCAP, SET-65, USET-80, AT-1, APR-2) e loro SL: n.v. Ci sono
   solo forchette da brevetti (20-80 kHz).
4. **Sonar sovietici attivi** (Titan-2, Platina, Polinom, Vega, Oka, VGS-3, RGB-25 e RGB-55A): nessuna frequenza
   aperta. Da tentare: deepstorm.ru (codifica windows-1251), Kazakov per altri titoli, Морфизприбор.
5. **«Енисей» e MT-70**: che cosa sono davvero (intercettatore o stazione passiva / da ghiaccio).
6. **PRI dei sonar di superficie per scala di distanza** (SQS-53, SQS-56): solo «20-30 s» generico.
7. **Righe di macchina del 688**: giri degli SSTG, numero di RCP e loro velocità, rapporto del riduttore, numero di
   denti, giri dell'asse per nodo. Tutto classificato. Fonti possibili: memorie di sottomarinisti, NAVEDTRA 14xxx
   (manuali di addestramento di macchinisti) se aperti.
8. **Numeri d'isolamento sottomarini** (zattera, cupola, anelli di disaccoppiamento della sfera) e livello di rumore
   proprio della sfera e della TB-16 in dB contro velocità: nulla di verificato. I PDF DTIC (ADA422438, ADA048954)
   e Cote (ADA421957) sono da chiedere.
9. **Funzione d'intercettazione del BQQ-5**: non documentata.

---

## 7. Fonti

- [MPWLR9] Military Periscope, AN/WLR-9 (solo l'apertura visibile) — https://www.militaryperiscope.com/weapons/sensorselectronics/electronic-support-measureselectronic-warfare/anwlr-9/ — media.
- [FASWLY1] FAS, AN/WLY-1 (agg. 4-2-1999, R. Sherman, J. Pike) — https://man.fas.org/dod-101/sys/ship/weaps/an-wly-1.htm — alta.
- [RCSUB] Forum rc-sub.com, «USS Tucson», post 46, 52 e 53 (CC Clarke; Will Rogers che cita Friedman) — https://forum.rc-sub.com/forum/builder-threads/162521-uss-tucson?p=167841 — bassa (Friedman: media).
- [USNI79] N. Polmar, «The U.S. Navy: Electronic Warfare (Part 1)», *Proceedings*, ott. 1979 — https://www.usni.org/magazines/proceedings/1979/october/u-s-navy-electronic-warfare-part-1 *(estr., 403)*.
- [WPLA] en.wikipedia, Los Angeles-class submarine (via `18_note/ascolto_storico.md`).
- [CMANO] cmano-db.com, sensore 4652 «AN/WLR-9, AN/BLR-14» — https://cmano-db.com/sensor/4652/ — bassa (database di gioco).
- [BALO] Baloogan Campaign Wiki, AN/WLR-9 — https://wiki.baloogancampaign.com/index.php/DataSensor?ID=1000349 *(estr.)* — bassa.
- [FASBQQ5] FAS, AN/BQQ-5 — https://man.fas.org/dod-101/sys/ship/weaps/an-bqq-5.htm — alta.
- [FAS688] FAS, SSN-688 — https://man.fas.org/dod-101/sys/ship/ssn-688.htm — alta.
- [FASBQS15] FAS, AN/BQS-15 — https://man.fas.org/dod-101/sys/ship/weaps/an-bqs-15.htm *(estr.)*; [NCBQS15] nuclearcompanion.com, «AN/BQS-15» — https://nuclearcompanion.com/data/an-bqs-15-close-contact-avoidance-and-under-ice-navigation-sonar/ *(estr.)* — media.
- [KAZ19] Kazakov, «Развитие аппаратуры обнаружения гидроакустических сигналов в период с 1950 по 2005 годы», 2019 — https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody — alta sul testo.
- [WPAkula] en.wikipedia, Akula-class submarine — https://en.wikipedia.org/wiki/Akula-class_submarine *(estr.)* — bassa per MT-70.
- [DOSITS] DOSITS, «Mid-frequency Active Sonar» — https://dosits.org/galleries/technology-gallery/locating-objects-using-sonar/mfa/ *(estr.)* — alta.
- [SBIR53] Navy SBIR N242-D11 — https://www.navysbir.com/n24_2/N242-D11.htm *(estr.)* — alta (SL e f nominali).
- [WPSQS26] en.wikipedia, AN/SQS-26 — https://en.wikipedia.org/wiki/AN/SQS-26 *(estr.)* — media.
- [MARSZAL] Marszal et al., ammodernamento del sonar dell'ORP «Kościuszko» (ex FFG-7), *Hydroacoustics* 2010-11 — https://yadda.icm.edu.pl/baztech/element/bwmeta1.element.baztech-article-BWM1-0007-0023 *(estr.)* — media.
- [GOAEIS] Gulf of Alaska Navy Training EIS, App. H, 2011 — https://peer.org/wp-content/uploads/attachments/6_15_15_Acoustic_systems.pdf *(estr., 403)*.
- [Wel60] W. E. Wells, US 4.216.534 «Decoying acoustic homing torpedoes», depositato il 28-6-1960 — https://patents.google.com/patent/US4216534A/en — alta.
- [PAS50] L. C. Paslay, US 3.943.870 «Pinging controlled anti-torpedo device», dep. 24-10-1950: pinger antisiluro a ~20 kHz, battimenti Doppler 50-500 Hz — https://patents.google.com/patent/US3943870A/en — alta (non riguarda le teste).
- [Hei64] US 4.306.512 (via r7) — https://patents.google.com/patent/US4306512.
- [ATLAS07] Atlas Elektronik, WO 2007/065535 «Method for production of a danger warning against an attacking torpedo» — https://patents.google.com/patent/WO2007065535A1/en — alta (moderno, schiera trainata passiva).
- [NW-US] NavWeaps, «USA Torpedoes since WWII» — http://www.navweaps.com/Weapons/WTUS_PostWWII.php — media.
- [MIA] E. V. Miasnikov, *The Future of Russia's Strategic Nuclear Forces*, App. 1 (fonti 1983-1995) — https://spp.fas.org/eprint/snf03221.htm — media.
- [TWZ20] A. Amick, «How The Experimental USS Narwhal Ended Up Being The Navy's Stealthiest Nuclear Submarine», The War Zone, 30-4-2020 — https://twz.com/33244/how-the-experimental-uss-narwhal-ended-up-being-the-navys-stealthiest-nuclear-submarine — media.
- [WPS5G] en.wikipedia, S5G reactor *(estr.)* — https://en.wikipedia.org/wiki/S5G_reactor — media.
- [FASDOT] US Navy SWOS, Division Officer training, «Noise / self-noise» — https://man.fas.org/dod-101/navy/docs/swos/stu2/NEWIS9_7.html — alta (concetti; navi di superficie).
- [TRID78] DTNSRDC, «Machinery noise and vibration monitoring system», 1978 — https://trid.trb.org/View/74582 *(estr.)* — media.
- [CIRCE] Atlas Elektronik CIRCE — https://atlas-elektronik.com/solutions/submarine-systems/circe.html *(estr.)*; [BK] Brüel & Kjær, Self-noise Monitoring System *(estr.)* — media, moderni.
- [AT19] *Acoustics Today*, primavera 2019, p. 13 (articolo sulle schiere trainate; figure da Ferguson 1990-98) — https://acousticstoday.org/issues/2019AT/Spring2019/files/basic-html/page13.html — media.
- [LASKY64] M. Lasky, L. F. Whicker, US 6.598.554 «Submarine towed measuring system», dep. 31-8-1964: rumore proprio da eliche, attrito sullo scafo e macchine interne, misura in campo vicino — https://patents.google.com/patent/US6598554B1/en — alta (concetto).
- [CURTIN] Curtin Univ./DSTO, misura del rumore irradiato con la propria schiera trainata — https://cmst.curtin.edu.au/wp-content/uploads/sites/4/2016/05/2000-26.pdf *(estr.)* — media.
- [CPB22] *Chinese Physics B* 31(5) 054301, 2022, «bearing splitting» del rumore del rimorchiatore — https://cpb.iphy.ac.cn/EN/10.1088/1674-1056/ac364e *(estr.)* — media.
- [DRDO] *Defence Science Journal*, «Cancellation of Towing Ship Interference in Passive SONAR in a Shallow Ocean Environment» — https://publications.drdo.gov.in/ojs/index.php/dsj/article/view/17370 *(estr.)* — media.
- [LL-ADA422438] MIT Lincoln Lab, cancellazione dello strum del cavo — https://apps.dtic.mil/sti/pdfs/ADA422438.pdf *(estr.)*; [HSTA77] DTIC ADA048954 *(estr.)* — media e bassa.
- [DAGA21] DAGA 2021, rumore strutturale sotto ~250 Hz — https://pub.dega-akustik.de/DAGA_2021/data/articles/000068.pdf *(estr.)*; [RAMS22] De Gruyter, *Rev. Adv. Mater. Sci.* 2022-0235 *(estr.)*; [DRDO-HP] https://drdo.gov.in/drdo/hi/node/944 *(estr.)*; [ATL18] https://atlantis-press.com/proceedings/meees-18/25898836 *(estr.)* — bassa e media.
- [EUGENE] eugeneleeslover.com, «Chapter 7 — Reduction gears» (manuale navale generico) *(estr.)* — media.
