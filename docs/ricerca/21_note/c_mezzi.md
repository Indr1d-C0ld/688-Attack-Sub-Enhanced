# 21-C — Meteo e limiti dei mezzi (elicotteri, aerei ASW, navi, sottomarini, armi)

Nota di ricerca per il dossier "Meteo dinamico realistico" del remake di *688 Attack Sub* (ambientazione 1985-1990).
Tema: **come vento, stato del mare, nebbia/cielo basso, temporali, ghiaccio (icing), neve e banchisa limitano e degradano le piattaforme del gioco.**

Convenzioni:
- **Confidenza**: **alta** = NATOPS / manuale ufficiale / standard NATO / articolo peer-reviewed / ente governativo; **media** = fonte secondaria seria (Wikipedia ben referenziata, GlobalSecurity, industria, riviste di settore); **bassa** = flashcard, forum, siti di gioco.
- **[GIOCO]** = proposta di design per il remake, non un fatto storico.
- **STIMA** = numero inventato o estrapolato da me, plausibile ma non documentato.
- **n.v.** = non trovato in fonti aperte (quasi sempre perché sta in NATOPS/SHOL/РЛЭ classificati o a distribuzione limitata).
- Durante la ricerca il server DTIC (apps.dtic.mil) era **in manutenzione**: diversi report sono citati solo dall'abstract o dal titolo. Vanno riletti quando torna disponibile (elenco in §11).

---

## 0. Riepilogo per chi implementa

1. **La variabile di base è lo stato del mare NATO (SS 0-9)**, derivato dal vento sostenuto con un ritardo (fetch/durata). Le fonti sovietiche usano i **баллы** della propria scala di moto ondoso, che **non coincide** né con Beaufort né con la scala NATO (vedi §1). Va convertita.
2. **Limiti "duri" quasi universali documentati:**
   - Standard NATO STANAG 4154 (fregata/corvetta): roll **4° RMS**, pitch **1,5° RMS**, velocità verticale del ponte di volo **2,0 m/s RMS**, vento relativo sul ponte **35 kn**, slamming **20/ora**, acqua verde a prua **30/ora** (alta).
   - FFG-7 + elicottero (NATOPS 00-80T-122): appontaggio entro **±8° di rollio, ±3° di beccheggio** (media, citato da terzi).
   - Ka-25 (requisito di progetto): appontaggio con rollio **10°**, beccheggio **3°**, vento **18 m/s (35 kn)** (media).
   - Ka-27: operazioni fino a **mare 5 баллов** (onde 2-3,5 m), giorno e notte, IMC (media).
   - Be-12: "мореходность" **3 баллa** (onde 0,75-1,25 m) (media).
   - Nanuchka (pr. 1234): uso delle armi fino a **5 баллов** (media).
   - RAST (LAMPS III): requisito operativo fino a **SS5**, rollio "circa 30°", onde **13 ft (4 m)** (alta-media, abstract SAE).
   - Beartrap canadese (Sea King su cacciatorpediniere): **SS6, 30° rollio, 9° beccheggio** (media).
   - Boe AN/SSQ-53D: funzionanti fino a **SS6**; lancio 30-370 kn, 100-30.000 ft (media-alta, costruttore).
3. **Tabelle per ogni stato del mare WMO 0-9** (sottomarini per classe, navi per dislocamento, aerei, armi, mine, danni e soccorso): **§12**.
4. **Il punto chiave per il gameplay**: il maltempo **colpisce di più chi caccia il sottomarino** (elicotteri, aerei bassi, sonar di scafo, boe, vedette) che il sottomarino profondo. Un SSN a 100+ m in SS7 è quasi immune al moto ondoso, mentre la scorta perde elicotteri, velocità e sonar. In cambio il sottomarino **non può stare a quota periscopica** con sicurezza e non riceve/trasmette facilmente.

---

## 1. Scale di riferimento (necessarie per convertire le fonti)

### 1.1 Stato del mare NATO (Nord Atlantico) — confidenza alta
Fonte: USNA EN400 cap. 8, tabella 8.1 "NATO Sea State Numeral Table for the Open Ocean North Atlantic" — https://usna.edu/NAOE/_files/documents/Courses/EN400/02.08%20Chapter%208.pdf

| SS | Hs (ft) campo | Hs medio (ft / m) | Vento sostenuto (kn) | Prob. % N.Atl. | Periodo modale più probabile (s) |
|---|---|---|---|---|---|
| 0-1 | 0-0,3 | 0,2 / 0,06 | 0-6 | 0 | — |
| 2 | 0,3-1,5 | 1,0 / 0,3 | 7-10 | 7,2 | 7,5 |
| 3 | 1,5-4 | 2,9 / 0,9 | 11-16 | 22,4 | 7,5 |
| 4 | 4-8 | 6,2 / 1,9 | 17-21 | 28,7 | 8,8 |
| 5 | 8-13 | 10,7 / 3,3 | 22-27 | 15,5 | 9,7 |
| 6 | 13-20 | 16,4 / 5,0 | 28-47 | 18,7 | 12,4 |
| 7 | 20-30 | 24,6 / 7,5 | 48-55 | 6,1 | 15,0 |
| 8 | 30-45 | 37,7 / 11,5 | 56-63 | 1,2 | 16,4 |
| >8 | >45 | >13,7 m | >63 | <0,05 | 20,0 |

### 1.2 Scala sovietica/russa del moto ondoso (баллы) — confidenza media
Fonte: dispensa di idrometeorologia militare, https://studizba.com/lectures/4-voennye-discipliny/159-gidrometeorologiya-morskaya-i-rechnaya-sistemy/2136-shkala-boforta-i-priznaki-pogody.html

| Балл | Altezza onda (m) | ≈ SS NATO (mia corrispondenza, STIMA) |
|---|---|---|
| 0 | 0 | 0 |
| 1 | <0,25 | 1 |
| 2 | 0,25-0,75 | 2 |
| 3 | 0,75-1,25 | 3 |
| 4 | 1,25-2,0 | 4 |
| 5 | 2,0-3,5 | 5 (bassa) |
| 6 | 3,5-6,0 | 6 |
| 7 | 6,0-8,5 | 7 |
| 8 | 8,5-11 | 8 |
| 9 | >11 | 9 |

Attenzione: la scala russa usa un'altezza a probabilità del 3% (onde più alte), mentre l'americana usa la H1/3. Quindi "5 баллов" sovietici sono un po' **meno severi** della SS5 NATO. Lo dice anche neftegaz.ru (media), che riporta invece la scala WMO/Douglas (3 = 0,5-1,25 m; 5 = 2,5-4 m): https://neftegaz.ru/tech-library/suda-neftegazovye-i-morskoe-oborudovanie-dlya-bureniya/141644-klassifikatsiya-sily-vetra-volneniya-na-more-i-vidimosti-na-more/
[GIOCO] Nel motore teniamo internamente Hs in metri e convertiamo verso le scale per la UI (NATO per il giocatore USA, баллы nei messaggi sovietici intercettati).

### 1.3 Beaufort (m/s e kn) — confidenza alta (stessa fonte studizba)
B4 = 11-16 kn, B5 = 17-21, B6 = 22-27, B7 = 28-33, B8 = 34-40, B9 = 41-47, B10 = 48-55, B11 = 56-63, B12 ≥ 64 kn.

### 1.4 Moto del mare in profondità (fisica, confidenza alta: teoria lineare delle onde)
L'ampiezza del moto orbitale e della pressione dinamica decade come **exp(−2π·z/λ)**, con λ ≈ 1,56·T² (m, T in s, acque profonde). Usando il periodo modale NATO (§1.1), il fattore residuo è:

| SS (T modale) | λ (m) | z = 18 m (quota periscopica) | 30 m | 60 m | 100 m |
|---|---|---|---|---|---|
| 4 (8,8 s) | 121 | 0,39 | 0,21 | 0,05 | 0,006 |
| 5 (9,7 s) | 147 | 0,46 | 0,28 | 0,08 | 0,014 |
| 6 (12,4 s) | 240 | 0,62 | 0,46 | 0,21 | 0,07 |
| 7 (15 s) | 351 | 0,72 | 0,58 | 0,34 | 0,17 |
| 8 (16,4 s) | 420 | 0,76 | 0,64 | 0,41 | 0,22 |

Velocità orbitale alla superficie ≈ π·H/T: in SS6 (H 5 m, T 12,4 s) è ≈ 1,3 m/s; a 30 m è ≈ 0,6 m/s.
**[GIOCO]** Questa tabella è il cuore del modello "moto a bassa quota" del sottomarino (§5): il disturbo di quota/assetto è proporzionale a Hs × fattore. Mostra anche perché con onde lunghe (SS7-8) bisogna scendere **oltre 100 m** per stare tranquilli.

---

## 2. Elicotteri ASW (SH-60B, SH-60F, SH-3H, Ka-25, Ka-27)

### 2.1 Dati trovati

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| FFG-7 + elicottero: limiti di moto nave all'appontaggio | ±8° rollio, ±3° beccheggio | arXiv 2202.13005, che cita NAVAIR "Helicopter Operating Procedures for Air-Capable Ships NATOPS" (00-80T-122, 2003) — https://arxiv.org/pdf/2202.13005 | media |
| Requisito LAMPS III / RAST | operare fino a SS5, "rollio nave circa 30°", onde 13 ft | SAE 811080 (1981), abstract — https://saemobilus.sae.org/papers/lamps-iii-recovery-assist-securing-traversing-rast-system-811080 | alta-media |
| Panoramica RAST (seconda fonte) | SS5 = vento 33 kn, onda 13 ft, 6° pitch, 15° roll | risultato di ricerca (GlobalSecurity, pagina a pagamento non letta) — https://www.globalsecurity.org/military/systems/aircraft/sh-60b.htm | bassa-media |
| Beartrap (Sea King canadese su DDH) | SS6 (4-6 m), giorno e notte, 30° rollio, 9° beccheggio | Lockheed Martin "Cyclone goes to sea" — https://lockheedmartin.com/content/dam/lockheed-martin/rms/photo/cyclone/CycloneGoesToSea-AHS1.pdf ; variante 31°/8° su migflug — https://migflug.com/afterburner/beartrap-hauldown-canadian-navy-sea-king-destroyer/ | media |
| STANAG 4154, recupero elicottero (fregata/corvetta) | velocità verticale del ponte di volo 2,0 m/s RMS; roll 4° RMS; pitch 1,5° RMS; vento relativo medio 35 kn | Nausivios Chora 2010 (Accademia Navale Ellenica), tab. 1 — https://nausivios.hna.gr/docs/c8_2010.pdf | alta (riportato) |
| Esempio di operabilità | fregata in SS5 (Hs 3,8 m, T0 12,4 s): indice di operabilità per l'elicottero **33%** delle combinazioni rotta/velocità | stessa fonte, fig. 1 | alta |
| Prassi del comandante | col maltempo si mette la nave con mare al giardinetto o in poppa per migliorare la scia d'aria sul ponte, restringendo l'inviluppo | stessa fonte | alta |
| Prassi offshore norvegese (civile) | appontaggio su ponti piccoli solo con pitch e roll < 2° e heave rate < 1 m/s | futurewavesradar — https://www.futurewavesradar.com/shipboard-helicopter-operations/ | bassa-media |
| H-60, avvio/arresto rotore | 45 kn da qualsiasi direzione; con raffiche ≥10 kn di spread, limiti ridotti di 10 kn | flashcard che citano A1-H60CA-NFM-000 — https://brainscape.com/flashcards/limits-8045343/packs/13423276 | bassa |
| Fenomeno del "blade sailing" (flessione delle pale ai bassi giri con vento) | restringe l'inviluppo di avvio/arresto; la scia della nave è un fattore principale | Soton eprints — https://eprints.soton.ac.uk/458800/1/86973.pdf | alta |
| Test DI SH-60B su DDG-993 (Kidd) | inviluppi basati sui voti dei piloti; limitati da carico di lavoro, pitch/roll eccessivi, vento relativo estremo; tabelle n.v. (DTIC giù) | NAVAIRTESTCEN 1984 — https://apps.dtic.mil/sti/tr/pdf/ADA383636.pdf | alta (contenuto non letto) |
| SHOL specifici per coppia nave/elicottero, basati su voti DIPES | metodologia | NATO STO MP-MSG-159-02 — https://www.sto.nato.int/publications/STO%20Meeting%20Proceedings/STO-MP-MSG-159/MP-MSG-159-02.pdf | alta |
| Ka-25, requisito di progetto | appontaggio con rollio 10°, beccheggio 3°, vento fino a 18 m/s; antighiaccio elettrico delle pale; parabrezza con liquido alcolico | ru.wikipedia Ка-25 — https://ru.wikipedia.org/wiki/Ка-25 | media |
| Ka-27 | operazioni "при волнении моря до 5 баллов днём и ночью в простых и сложных метеоусловиях" | ru.wikipedia Ка-27 — https://ru.wikipedia.org/wiki/Ка-27 ; anche GlobalSecurity — https://www.globalsecurity.org/military/world/russia/ka-27.htm | media |
| Ka-27, incidenti legati al meteo | 1982 rollio forte di notte; 1988 sbattuto contro la murata in tempesta; 1988 IMC; 1994 entrato in nubi basse; 2009 pale contro la sovrastruttura all'appontaggio | ru.wikipedia Ка-27 (elenco incidenti) | media |
| Ka-27PL, sonar a immersione | hovering a **circa 30 m**; il vento inclina il cavo e falsa la direzionalità; 3-10 min per stazione | techinsider.ru — https://www.techinsider.ru/diy/14361-okhota-na-krupnuyu-rybu/ | media |
| AQS-13 (SH-3H/SH-60F): SOP di stormo | "should not" immergere con onde > 10 ft, vento oltre 17-21 kn, correnti > 4 kn (addestramento/esercitazioni) | flashcard — https://brainscape.com/flashcards/limits-8045343/packs/13423276 | bassa |
| Hover accoppiato col cavo | ±5 kn di ground speed, quota entro 10 ft, hover accoppiato a 70 ft; traino 8 kn col trasduttore chiuso, 4 kn aperto | flashcard (stessa famiglia) | bassa |
| SH-3H | "very limited potential" per la copertura ASW del gruppo portaerei a causa di **deficienze di prestazioni in hover** (caldo/peso) | tesi UTK 1989 — https://trace.tennessee.edu/utk_gradthes/13063 | media |
| SH-60B, ghiaccio | ha anti-ghiaccio motori/prese; un caso documentato di emergenza per ghiaccio tra 5.000 e 12.000 ft | Naval Aviation News (Grampaw Pettibone 2000) — https://history.navy.mil/research/histories/naval-aviation-history/naval-aviation-news/grampaw-pettibone-collection/collection/2000-s/2000/may-june.html | media |
| SH-60B / SH-3H: volo in ghiaccio noto | proibito/limitato (niente sghiacciamento delle pale) | n.v. in fonte ufficiale; è la mia conoscenza → **STIMA** | bassa |
| Ammaraggio forzato: sopravvivenza in acqua a 5 °C, mare grosso (modello) | 1,9 h nudo; 4,8 h con tuta anti-esposizione; 12,6 h con tuta stagna | DTIC ADA305730 (abstract) — https://apps.dtic.mil/sti/pdfs/ADA305730.pdf | alta (modello) |
| Sopravvivenza in acqua ferma senza tuta (modello) | ~136 min a 5 °C, ~100 min a 0 °C; in acqua mossa ~119 e ~81 min | Heliyon 2024 — https://doaj.org/article/1a35c5f71179464590921875b3ac9bf7 | alta |
| Fasi del rischio da immersione | shock da freddo nei primi minuti, incapacità di nuotare entro ~30 min, ipotermia dopo | Transport Canada via Argus — https://safetymaterials.argus.aero/wp-content/uploads/2023/07/968_Cold-water-survival-Final.pdf | media |

**Nota sulla contraddizione RAST 30°/15°.** Il "30° di rollio" dell'abstract SAE è quasi certamente un requisito strutturale (fissaggio dell'elicottero e traslazione nell'hangar su ponte che rolla), non un limite di appontaggio. Il limite pratico di appontaggio sulla FFG-7 è ±8° rollio / ±3° beccheggio (00-80T-122). Con l'RAST/Beartrap si arriva più vicino al limite e lo si fa di notte, mentre **senza haul-down** (navi sovietiche, Ka-25/27 che appontano "liberi") il limite reale è più basso. Il Ka-25 era progettato per 10°/3°.

### 2.2 Tabella regole elicotteri [GIOCO], valori STIMA salvo dove indicato

Legenda: N = normale, D = degradato, X = impossibile/vietato.

| Condizione | SH-60B su FFG/DD con RAST | SH-60F/SH-3H da portaerei | Ka-25 / Ka-27 da nave sovietica (senza haul-down) |
|---|---|---|---|
| **Moto nave all'appontaggio** | N: roll ≤ 5°, pitch ≤ 2°; D: roll 5-8°, pitch 2-3° (servono quiescent period, tempo +50%, 10% di wave-off); X: roll > 8° o pitch > 3° (fonte ±8/±3: media) | portaerei quasi sempre entro limiti fino a SS6; X: pitch ponte > ±2° (STIMA) | N: roll ≤ 5°, pitch ≤ 2°; D: 5-10° / 2-3°; X: roll > 10° o pitch > 3° (requisito Ka-25, media) |
| **Stato del mare** (indicativo per una fregata da 130 m) | N: SS ≤ 4; D: SS5 (limite di progetto RAST); X: SS ≥ 7, SS6 solo con rotta favorevole (STIMA) | N: SS ≤ 5; D: SS6; X: SS ≥ 7 (STIMA) | N: ≤ 4 баллов; D: 5 баллов (limite dichiarato Ka-27); X: ≥ 6 баллов |
| **Vento relativo sul ponte** | N: ≤ 30 kn entro ±30° dalla prua; D: 30-40 kn o vento al traverso/poppa; X: > 45 kn (avvio/arresto rotore, bassa) o > 35 kn medi per il personale (STANAG, alta) | come a sinistra | N: ≤ 12 m/s; D: 12-18 m/s; X: > 18 m/s (35 kn, requisito Ka-25, media) |
| **Raffiche** | spread ≥ 10 kn: tutti i limiti di vento −10 kn (bassa) | idem | idem (STIMA) |
| **Sonar a immersione** | n/a (SH-60B non ha sonar a immersione; LAMPS III = boe + MAD) | N: onde ≤ 10 ft (3 m), vento ≤ 20 kn; D: 3-4 m / 20-30 kn (cavo inclinato, errore di rilevamento ±10-20°, dip più corti); X: Hs > 4 m o vento > 35 kn (STIMA; la soglia 10 ft è da flashcard, bassa) | N: ≤ 3 баллов; D: 4-5; X: ≥ 6 (STIMA); hover a 30 m (media) |
| **Notte/IMC** | N (LAMPS III è ognitempo; avvicinamento con radar/TACAN della nave); tempi di recupero +30% | N con approccio automatico all'hover; dip notturni D (STIMA) | Ka-27 N (media); Ka-25 D (STIMA) |
| **Ghiaccio** | D: traccia/leggero solo con anti-ghiaccio motori, uscire subito; X: moderato o superiore (STIMA) | SH-3H: X in ghiaccio noto (STIMA) | D: leggero/moderato (pale riscaldate elettricamente, documentato sul Ka-25); X: forte (STIMA) |
| **Temporali** | X: volo entro 10 nm da celle attive; dip e verricello sospesi con fulmini entro 5 nm (STIMA; la regola civile FAA è ≥ 20 mi dai temporali severi, media) | idem | idem |
| **Neve/ghiaccio sul ponte** | D: traslazione RAST e rizzaggio più lenti (+5 min), rischio di scivolamento in rollio | D | D: senza haul-down rischio di scivolamento sul ponte (STIMA) |

**Modello di recupero proposto [GIOCO]:** ogni ciclo di avvicinamento (2-3 min) estrae roll/pitch dal moto nave simulato. Se per 10-15 s di "quiescent period" restano sotto i limiti, l'appontaggio riesce, altrimenti c'è wave-off e nuovo tentativo. Senza RAST la probabilità di incidente cresce linearmente da 0% al 70% del limite fino al 5% al 100% del limite (STIMA). Questo crea il comportamento giusto: il giocatore che vuole riprendere l'elicottero deve **mettersi con il mare al giardinetto e rallentare**, perdendo velocità e posizione nello schermo ASW.

**Ammaraggio [GIOCO]:** il tempo di sopravvivenza dell'equipaggio decide se il SAR "salva" i punti/la reputazione. Con acqua a 0-5 °C senza tuta stagna si ha < 2 h (alta). In SS ≥ 6 l'elicottero galleggia per < 1 min (STIMA).

---

## 3. Aerei ASW (P-3C, S-3A/B, Il-38, Tu-142, Be-12)

### 3.1 Dati trovati

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Recupero su portaerei CASE I | ceiling ≥ 3.000 ft, visibilità ≥ 5 nm, di giorno | theaviationist — https://theaviationist.com/2018/02/19/cockpit-video-shows-f-a-18e-super-hornet-performing-case-ii-recovery-with-low-visibility-and-pitching-deck/ ; simtuts — https://simtuts.com/guides/carrier-case-operations-guide | media |
| CASE II | ceiling ≥ 1.000 ft, visibilità ≥ 5 nm | stesse fonti | media |
| CASE III | sotto i minimi CASE II, oppure di notte (avvicinamento strumentale con controllo CATCC) | FM 1-564 cap. 5 — https://www.globalsecurity.org/military/library/policy/army/fm/1-564/CH5.HTM | media |
| Ponte che beccheggia | se si superano i limiti di stabilizzazione della FLOLS (mare molto grosso) si passa al MOVLAS manuale dell'LSO; numeri n.v. | flashcard CATCC — https://blog.brainscape.com/flashcards/5659024/packs/7936398 ; incidente A-7E Midway 1984 — https://theaviationist.com/2020/05/20/footage-shows-the-a-7e-corsair-iis-ramp-strike-on-uss-midway-in-1984/ | bassa/media |
| Boe AN/SSQ-53D(3) DIFAR | funzionante fino a **SS6**; inviluppo di lancio 30-370 KIAS, 100-30.000 ft | datasheet Ultra — https://www.ultra-hyperspike.com/media/2448/anssq-53d3-datasheet_final.pdf | media-alta (costruttore, modello post-1990) |
| Boa attiva moderna (SSQ-963D) | opera fino a SS5, **sopravvive fino a SS7** | datasheet Ultra Maritime — https://umaritime.com/wp-content/uploads/2024/02/SSQ-963D-Command-Active-Multi-Beam-Sonobuoy-CAMBS-Feb-2024.pdf | media |
| MAD | portata breve: bisogna passare sopra o molto vicino al bersaglio, a bassa quota | Wikipedia P-3 — https://en.wikipedia.org/wiki/Lockheed_P-3_Orion | media |
| Turbolenza a bassa quota sul mare (P-3C RAAF) | circa **metà** di quella prevista dai modelli; i trasferimenti in quota sopra la terraferma sono ~5 volte più severi del previsto | DSTO/ARL — https://apps.dtic.mil/sti/pdfs/ADA214000.pdf (abstract) | alta |
| Il-38 | progettato per cercare "в любых метеоусловиях в любое время года и суток"; antighiaccio elettrotermico su ala, impennaggi ed eliche; incidente del 1994 a Severomorsk-1 in neve forte e poземка | ru.wikipedia Ил-38 — https://ru.wikipedia.org/wiki/Ил-38 | media |
| Be-12 | **мореходность 3 балла** (onde ≈ 0,75-1,25 m); motori alti sull'ala a gabbiano per proteggere le eliche dagli spruzzi | ru.wikipedia Бе-12 — https://ru.wikipedia.org/wiki/Бе-12 | media |
| Be-12, distanze | decollo da acqua ~2.300 m (terra 2.000), ammaraggio ~2.300 m (terra 1.800) | GlobalSecurity — https://www.globalsecurity.org/military/world/russia/be-12-specs.htm | media |
| Idrovolanti in genere (dati Be-200) | con onde lunghe rispetto allo scafo (λ/L ≈ 1,15-1,20) la corsa diventa instabile fino a sovraccarichi e rimbalzi limite | Вестник МГТУ ГА — https://avia.mstuca.ru/jour/article/download/845/724 | alta (aereo diverso) |
| Be-12 e sonar calato in mare | "in condizioni relativamente calme" poteva ammarare e usare il proprio sonar | Weapons & Warfare — https://weaponsandwarfare.com/?p=11071 | bassa-media |
| Fulmini | qualsiasi superficie esterna dell'aereo può essere colpita (programma NASA, ~1.500 penetrazioni di temporali) | NASA NTRS — https://ntrs.nasa.gov/citations/19900062779 | alta |

### 3.2 Tabella regole aerei [GIOCO], valori STIMA salvo dove indicato

| Condizione | P-3C / Il-38 / Tu-142 (basi a terra) | S-3A/B (portaerei) | Be-12 (anfibio) |
|---|---|---|---|
| **Decollo/atterraggio alla base** | N: ceiling ≥ 500 ft, vis ≥ 1,5 km; D: 200-500 ft (ritardi, 20% di dirottamenti); X: < 200 ft / 800 m o vento al traverso > 30 kn | vedi riga portaerei | da pista come P-3; da acqua vedi sotto |
| **Recupero su portaerei** | n/a | CASE I/II/III come da fonte (media). X: ceiling < 200 ft e vis < 0,5 nm (minimi CCA, STIMA); D: pitch del ponte > ±1,5° → MOVLAS, 20% di bolter/wave-off; X: pitch > ±2,5-3° → divert/bingo (STIMA) | n/a |
| **Acqua (ammaraggio/decollo)** | n/a | n/a | N: ≤ 2 баллов; D: 3 баллa (limite dichiarato); X: ≥ 4 баллов (Hs > 1,25 m) |
| **Ricerca visiva (periscopio/snorkel/scia)** | portata visiva = min(visibilità, portata geometrica); nebbia < 1 km → visiva X; SS ≥ 4 → scia del periscopio e "feather" quasi invisibili (−70%) | idem | idem |
| **Radar contro periscopio/snorkel** | N: SS ≤ 2; D: SS3-4 (clutter di mare, portata −50%); X: SS ≥ 5 per un periscopio, D per uno snorkel (STIMA, coerente con la fisica del clutter) | idem | idem |
| **Lancio boe** | N: SS ≤ 5; D: SS6 (10-20% di guasti; rumore ambiente alto, portata di scoperta ridotta); X: SS ≥ 7 (guasti > 50%, sopravvivenza della boa) — SS6 operativo per SSQ-53D (media) | idem | idem (boe RGB sovietiche: STIMA un gradino peggio) |
| **Passaggio MAD** (quota ~150-300 ft, STIMA) | N: turbolenza assente/leggera; D: turbolenza moderata → precisione di passaggio peggiore, P(contatto) × 0,7, fatica +50%; X: turbolenza forte, temporali o ceiling < 300 ft di notte | idem | idem |
| **Ghiaccio in volo basso** | P-3C/Il-38/Tu-142: D anche con ghiaccio moderato (antighiaccio completo; Il-38 documentato) → consumo +5%, MAD/boe normali; X: ghiaccio forte persistente sotto 3.000 ft | S-3: D con leggero/moderato (STIMA) | D: leggero; X: moderato o superiore (STIMA) |
| **Temporali/linee di groppo** | X: sorvolo diretto della cella; deviazione ≥ 10-20 nm (regola civile 20 mi per celle severe, media) → zone di ricerca "bucate" | idem | idem |
| **Vento forte a quota di ricerca** | deriva delle boe: il campo si sposta con il vento (paracadute) e la corrente; D: vento > 30 kn → errore di posizionamento 0,5-1 nm (STIMA) | idem | idem |

---

## 4. Navi di superficie (Perry, Spruance, Kidd, Krivak II, Kara, Kirov, Slava, Udaloy, Koni, Nanuchka, portaerei, mercantili)

### 4.1 Dati trovati

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Criteri NATO STANAG 4154 (monoscafo, fregata/corvetta) | slamming 20/h; acqua verde a prua 30/h; emersione dell'elica (¼ D) 90/h; pitch 1,5° RMS; roll 4° RMS; accelerazione verticale in plancia 0,2 g RMS; laterale 0,1 g; MSI 20% dell'equipaggio in 4 h; MII 1/min | Nausivios Chora 2010, tab. 1 — https://nausivios.hna.gr/docs/c8_2010.pdf | alta (riportato) |
| Le fregate (Knox, Perry) d'inverno nel Nord Atlantico | "severely limited" nella disponibilità per scorta ASW di task force e convogli a causa dei limiti di tenuta al mare, comprese le operazioni con l'elicottero | Comstock et al., Naval Engineers Journal 1983 (abstract) — https://onlinelibrary.wiley.com/doi/abs/10.1111/j.1559-3584.1983.tb01643.x | alta |
| Sondaggio tra comandanti USN (velocità massima con mare di prua per stato del mare) | dati per classe presenti ma non letti (pagina bloccata) | USNI Proceedings, settembre 1983, "Seakeeping" — https://www.usni.org/magazines/proceedings/1983/september/seakeeping | alta (non letto) |
| Cacciatorpediniere torpediniere del 1903 | 30 kn solo in calma piatta; ~15 kn in SS4 | IMarEST — https://library.imarest.org/record/8727/files/v31b1p02a.pdf | media |
| Coefficienti di direzione della perdita involontaria di velocità (Kwon 2008) | prua (0-30°) 1; mascone (30-60°) [1,7−0,03(BN−4)²]/2; traverso (60-150°) [0,9−0,06(BN−6)²]/2; poppa (150-180°) [0,4−0,03(BN−8)²]/2 | Lang & Mao (Chalmers), tab. 5 — https://web-test.research.chalmers.se/publication/525425/file/525425_Fulltext.pdf | alta |
| Perdita di velocità misurata (mercantile, BN6) | fino al 21% della velocità in calma | stessa fonte | alta |
| Udaloy (pr. 1155) | bulbo del sonar lungo e forte svasatura delle ordinate → slamming del fondo e prua bagnata, "затрудняя использование корабельного оружия"; il pr. 1155.1 fu modificato per migliorare la tenuta al mare e l'uso delle armi | ru.wikipedia — https://ru.wikipedia.org/wiki/Большие_противолодочные_корабли_проекта_1155 | media |
| Nanuchka (pr. 1234), tenuta al mare | buona manovrabilità con mare di prua; con mare di poppa "плохо слушаются руля", forte imbardata; a bassa velocità in 4-5 баллов poca acqua in coperta; **oltre 14 kn gli spruzzi arrivano al tetto della timoneria** | ru.wikipedia — https://ru.wikipedia.org/wiki/Малые_ракетные_корабли_проекта_1234 | media |
| Nanuchka, armi | uso delle armi **fino a 5 баллов**; con mare forte di poppa salve molto limitate; non potendo tenere la rotta di lancio, intervallo tra missili fino a 1,5 min | stessa fonte; GlobalSecurity ("5 punti e 24 kn") — https://www.globalsecurity.org/military/world/russia/1234.htm | media |
| Sonar di scafo: slamming del bulbo | in mare grosso la prua emerge e rientra con picchi di pressione sotto il bulbo | brevetto US4307457 — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4307457 | media |
| Sonar di scafo: strato di bolle | bolle di frangenti e scia limitano i sonar attivi HF (>10 kHz) orizzontali | DRDC/JCAA — https://jcaa.caa-aca.ca/index.php/jcaa/article/download/2781/2504/3435 | media-alta |
| Ghiaccio di spruzzo sulle navi (NOAA/Overland) | basso 0-0,7 cm/h; moderato 0,7-2 cm/h; severo > 2 cm/h (nave a ~20 kn) | NOAA OPC Freezing Spray — https://ocean.weather.gov/freezingSpray/ ; Overland 1990, *Weather and Forecasting* 5(1):62-77 | alta |
| Pescherecci in Alaska | > 2 cm/h nel 30% delle osservazioni di icing | NOAA — https://repository.library.noaa.gov/view/noaa/9493/noaa_9493_DS1.pdf | alta |

**Predittore di Overland (da memoria, da verificare sull'articolo 1990):** PR = V·(Tf − Ta) / (1 + 0,3·(Tw − Tf)), con V = vento in m/s, Ta = aria °C, Tw = mare °C, Tf = −1,7 °C. Classi (memoria): PR < 0 niente; 0-22,4 leggero (< 0,7 cm/h); 22,4-53,3 moderato (0,7-2); 53,3-83 forte (2-4); > 83 estremo (> 4 cm/h). Confidenza **media** sulla forma, **bassa** sulle soglie esatte. Le soglie in cm/h coincidono con NOAA (alta).

**Formula di perdita involontaria di Kwon (2008), parte Cf, da memoria (bassa):** per navi portacontainer (Cb 0,55-0,60): ΔV/V (%) = Cβ·Cα·[0,7·BN + BN^6,5 / (22·∇^(2/3))], con ∇ = volume di dislocamento in m³. Per navi piene a pieno carico: [0,5·BN + BN^6,5/(2,7·∇^(2/3))]. **Non usarla per le navi da guerra veloci**: Cα diventa negativo ad alti Froude, e per ∇ piccoli il termine BN^6,5 esplode (per una FFG da ~4.000 m³ in BN8 dà > 100%). Va bene per i **mercantili** del gioco.

### 4.2 Velocità sostenibile con mare di prua [GIOCO] — tutto STIMA
La perdita reale nei mari forti è soprattutto **volontaria**: il comandante rallenta per restare sotto i criteri STANAG (slamming < 20/h, acqua verde < 30/h, accelerazione in plancia < 0,2 g). Proposta per classe di lunghezza (velocità massima sostenibile in kn, mare entro ±30° dalla prua):

| Classe (esempi, Lpp) | SS4 | SS5 | SS6 | SS7 | SS8 |
|---|---|---|---|---|---|
| < 65 m (Nanuchka 59 m) | 28 | 18 | 12 | 8 (sopravvivenza) | 5, rischio di avarie |
| 90-100 m (Koni 96 m) | 27 | 22 | 15 | 10 | 6 |
| 120-140 m (Perry 136, Krivak 123) | 29 | 25 | 18 | 12 | 8 |
| 160-190 m (Spruance/Kidd 172, Kara 173, Udaloy 163, Slava 186) | 32 | 28 | 22 | 15 | 10 |
| 250 m (Kirov 252) | 31 | 30 | 26 | 20 | 14 |
| portaerei 300+ m | 30 | 30 | 28 | 24 | 18 |
| mercantile 150-250 m | Kwon (§4.1) con Cα≈0,9 | | | | |

Modificatori: mascone ×0,85 della perdita; traverso ×0,5 ma rollio massimo (vedi armi ed elicotteri); poppa: perdita minima ma rischio di **broaching** per le navi piccole (Nanuchka) in SS ≥ 6 [GIOCO: probabilità di straorzata con danni/avarie].
**[GIOCO] Velocità sopra il limite:** consentita, ma ogni minuto rischia danni da slamming (sonar di prua fuori uso, attrezzature in coperta strappate, feriti), con P = 2%/min per nodo oltre il limite (STIMA).

### 4.3 Armi e sensori di bordo [GIOCO]: N / D / X per stato del mare — STIMA salvo Nanuchka

| Sistema | N | D (effetto) | X |
|---|---|---|---|
| Sonar di scafo (SQS-26/53 Spruance/Kidd, SQS-56 Perry, bulbi sovietici Titan/Polinom su Kara/Kirov/Udaloy) | SS ≤ 3 | SS4: portata ×0,85; SS5: ×0,7; SS6: ×0,5 e "buchi" quando la prua emerge (più frequenti ad alta velocità con mare di prua); rumore proprio +3 dB per gradino SS | SS ≥ 7 con mare di prua a velocità > limite §4.2: inutilizzabile |
| Sonar trainato (SQR-19 su Perry/Spruance, dal 1985 circa) | SS ≤ 5 | SS6-7: rumore di strumming/ondulazione, portata ×0,8 | messa a mare/recupero X in SS ≥ 7 |
| VDS (Krivak, Udaloy, Kara) | SS ≤ 4 | SS5: ×0,8; filare/recuperare è rischioso | SS ≥ 6 recupero vietato (rischio di perdita) |
| ASROC (Mk 16 / Mk 26) | SS ≤ 5 | SS6: tempo di reazione +30 s; errore di caduta maggiore col vento | SS ≥ 8 o vento > 50 kn (STIMA) |
| Tubi lanciasiluri Mk 32 / sovietici 533 mm in coperta | SS ≤ 4 | SS5-6: lancio solo sul lato sottovento, siluro può "saltare" all'ingresso in acqua (P malfunzionamento 10-20%) | SS ≥ 7 |
| RBU-6000/1000 | SS ≤ 5 | SS6: dispersione ×1,5, ricarica più lenta (ghiaccio/spruzzi) | SS ≥ 7 o icing forte (ricarica bloccata) |
| Cannoni (Mk 45, Mk 75, AK-100, AK-130, AK-726) | SS ≤ 4 | SS5-6: precisione ×0,6 (stabilizzazione), acqua verde sulle torri di prua | SS ≥ 7 per i pezzi di prua delle navi < 140 m |
| SSM ingombranti (SS-N-9 Nanuchka, SS-N-12 Slava, SS-N-19 Kirov, SS-N-14 Krivak/Kara) | Nanuchka ≤ 4 баллов | Nanuchka: 5 баллов, intervallo di salva fino a 1,5 min (fonte, media) | Nanuchka > 5 баллов (fonte, media); per gli altri STIMA: SS ≥ 8 |
| Harpoon (Mk 141 / Mk 13 / ABL) | SS ≤ 6 | SS7: P di malfunzionamento al lancio/ingresso nella fase radente +10% | SS ≥ 8 (STIMA) |
| Operazioni con elicottero | vedi §2.2 | | |
| Icing (tutte le armi in coperta) | icing basso | moderato: tempo di ricarica ×1,5, antenne/radar degradati; serve rompere il ghiaccio | severo prolungato: armi di coperta fuori uso finché non si toglie il ghiaccio; perdita di stabilità per navi < 100 m (Nanuchka, Koni, pescherecci) |

**Navi piccole e capacità di stare in mare [GIOCO]:** la Nanuchka in SS ≥ 6 dovrebbe essere costretta dall'IA a cercare ridosso, a rallentare o a mettersi con il mare di prua. È coerente con la fonte (sotto 5 баллов poca acqua solo a bassa velocità, armi fino a 5). Questo crea "finestre" in cui la minaccia missilistica costiera sparisce.

---

## 5. Sottomarini (688, SSN/SSK sovietici, Kilo, Foxtrot)

### 5.1 Dati trovati

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Forze di risucchio a quota periscopica | le forze costanti (suction) dipendono da velocità, profondità, frequenza e direzione dell'onda e **spesso limitano l'inviluppo operativo a quota periscopica** | Crook, NPS 1994 (DTIC ADA288546, abstract) — https://apps.dtic.mil/sti/pdfs/ADA288546.pdf | alta |
| Criteri di operabilità vicino alla superficie | tre criteri: immersione del periscopio, **affioramento della vela (broaching)**, collisione col fondo; indice di operabilità statistico | Toprak, NPS 1996 — https://calhoun.nps.edu/entities/publication/cbe37109-8ec5-49c9-bce8-7441712580fc | alta |
| Forza verticale verso la superficie | vicino alla superficie la forza di heave è diretta verso l'alto e aumenta il rischio di broaching | Conway/Valentinis/Seil 2018 (DST/AMC) — https://people.eng.unimelb.edu.au/imarusic/proceedings/21/Contribution_766_final.pdf | alta |
| Andamento del risucchio | cresce in modo **esponenziale** avvicinandosi alla superficie; gli algoritmi di stima non sono abbastanza robusti in mare vero | brevetto US6376831 — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6376831 | media |
| Squadra di controllo della quota | "wave generated forces are extremely high", segnali di quota e assetto molto rumorosi, dinamica del battello lenta | NASA, Ware & Best 1982 — https://ntrs.nasa.gov/citations/19820005816 | alta |
| Snorkel: valvola di testa | tenuta aperta ad aria compressa, si chiude quando le onde bagnano i contatti; in mare grosso i diesel aspirano l'aria dal battello → forte depressione, dolore alle orecchie, a volte timpani rotti; oggi ci sono sensori di alto vuoto che fermano i diesel | Wikipedia Submarine snorkel — https://en.wikipedia.org/wiki/Submarine_snorkel | media |
| Velocità con snorkel alzato (U-Boot WWII) | ≤ 6 kn per non danneggiare il tubo | stessa fonte | media |
| Snorkel: limite numerico di stato del mare per Kilo/Foxtrot | n.v. | — | — |
| Racconto di prima mano (Guerra Fredda) | in mare formato l'assetto rispetto alla superficie varia, l'albero si immerge; i dispositivi di sicurezza limitano il vuoto | The Lean Submariner — https://theleansubmariner.com/2026/01/23/cold-war-stories-prepare-to-snorkel/ | bassa-media |
| 688 sotto il ghiaccio | solo i **688I (SSN-751 → 773)** sono "configured for under-ice operations": timoni di prora a prua e retrattili invece che sulla vela | Wikipedia — https://en.wikipedia.org/wiki/Los_Angeles-class_submarine | media |
| Emersione nel ghiaccio, record della classe Los Angeles | USS Charlotte al Polo Nord attraverso **61 in (≈155 cm)** di ghiaccio; ogni classe ha un suo spessore massimo; emersione verticale a velocità zero; il sonar rivolto in alto cerca polynie o ghiaccio sottile | Business Insider (US Navy) — https://www.businessinsider.nl/how-navy-submarines-carefully-smash-surface-through-arctic-ice-2020-6/ | media |
| Typhoon | vela e scafo resistente sotto la vela progettati per rompere il ghiaccio; eliche intubate; in Artico non si lancia in immersione per paura del ghiaccio galleggiante | USNI 1999 — https://www.usni.org/magazines/proceedings/1999/february/world-naval-developments-typhoon-saga-ends | media |
| Limite generico citato per i sottomarini russi | fino a ~2,5 m (9 ft) con manovra accurata; nel 2021 tre SSN sono emersi insieme attraverso 1,5 m | Live Science — https://www.livescience.com/russian-nuclear-subs-emerge-arctic-ice.html | bassa-media |
| Creste di pressione (chiglie di ghiaccio) | chiglia media artica **4,5 m**, massima registrata **45 m**; chiglia ≈ 3-5 volte l'altezza della vela; creste distanti 100-200 m | Wikipedia Pressure ridge — https://en.wikipedia.org/wiki/Pressure_ridge_(ice) | media |
| Studio sull'emersione nel ghiaccio (1962) | due criteri: impossibilità di emergere / sfondamento possibile, statico o con velocità verticale | DTIC AD0286658 — https://openpolar.no/Record/ftdtic:AD0286658 | alta (abstract) |

### 5.2 Regole sottomarino [GIOCO] — STIMA salvo dove indicato

**Quota periscopica (688: chiglia ≈ 18 m / 60 ft, STIMA media)**

| SS | Effetto |
|---|---|
| 0-2 | N: tenuta quota ±0,5 m; periscopio con "feather" visibile in calma (rischio di avvistamento visivo/radar **più alto** col mare piatto) |
| 3 | N: ±1 m; lieve immersione del periscopio |
| 4 | D: ±1,5-2 m; il periscopio si immerge per il 20-30% del tempo (perdita di contatto visivo, ESM a intermittenza); serve velocità 5-8 kn; preferire mare di prua o di poppa |
| 5 | D: ±2-3 m; immersione 40-50%; rischio di **broaching della vela** 1%/min se velocità < 4 kn o mare al traverso; antenne di comunicazione a intermittenza (messaggi lunghi falliscono) |
| 6 | D→X: ±3-5 m; broaching 5%/min con mare al traverso; il comandante dovrebbe scendere a 25-30 m (fuori quota periscopica). Vantaggio: il clutter radar nasconde quasi del tutto un albero esposto |
| ≥ 7 | X per la quota periscopica; rollio e beccheggio avvertibili fino a 60-100 m (fattore §1.4); trasmissioni solo con boa filata/VLF trainato |

Moto a profondità z: ampiezza ∝ Hs × exp(−2π·z/λ) (§1.4, alta). Il risucchio verso l'alto cresce esponenzialmente avvicinandosi alla superficie e con velocità bassa (alta). [GIOCO] Il pilota automatico di quota ha un errore ∝ Hs·exp(−2πz/λ)/(1 + v/5 kn).

**Snorkel diesel (Kilo pr. 877, Foxtrot pr. 641) — STIMA**

| баллы / SS | Effetto |
|---|---|
| ≤ 3 / ≤ 3 | N: ricarica al 100% |
| 4 / 4 | D: chiusure della valvola di testa ogni 1-3 min → ricarica al 70%; disagio dell'equipaggio |
| 5 / 5 | D: chiusure continue, arresti per alto vuoto → ricarica al 40%; quota difficile da tenere; P(guasto/allagamento del mast) 0,5%/h |
| ≥ 6 / ≥ 6 | X (o ricarica al 15% con grave rischio); il comandante va in profondità e consuma batteria → **finestra tattica per l'ASW**: il diesel non può ricaricare durante la burrasca, ma è anche meno rilevabile al radar mentre ci prova |

Rilevabilità: SS alto → ESM/visiva su snorkel/periscopio molto difficile (clutter, spruzzi, schiuma). SS basso → scia del mast e fumi visibili. È un equilibrio "realistico" che il gioco può sfruttare.

**Sotto il ghiaccio (Mare di Barents, di Okhotsk) — mix di fonti/STIMA**

| Voce | Regola |
|---|---|
| Classi abilitate | 688 Flight I/II (timoni sulla vela, 1985-87): sotto il ghiaccio **D** (permesso ma niente emersione attraverso il ghiaccio, solo nelle polynie) [GIOCO, deduzione da Wikipedia]. 688I (dal 1988): emersione ≤ ~1,5 m (record Charlotte 155 cm, media). Sturgeon: nato per l'Artico (Wikipedia, media). Typhoon: progettato per rompere il ghiaccio, ≤ 2-2,5 m (bassa-media). Delta IV/Akula: ≤ 1-1,5 m (STIMA) |
| Procedura di emersione | velocità zero, risalita verticale; prima un sonar rivolto in alto per cercare polynia o ghiaccio sottile (media). [GIOCO] 10-20 min di manovra, rumore di sfondamento udibile a decine di km |
| Chiglie di ghiaccio | quota minima di sicurezza sotto la banchisa: 60 m in Barents / 45-50 m in Okhotsk, più in basso dove le creste sono fitte (max registrato 45 m, media). [GIOCO] mappa procedurale di creste con profondità tratta da una distribuzione esponenziale con media 4,5 m e coda fino a 45 m |
| Acustica sotto il ghiaccio | banchisa compatta e fredda: rumore ambiente basso; ghiaccio che si spezza/creste attive (vento, maree): rumore alto e impulsivo; il sottosquadra del ghiaccio riflette e disperde (rimando al file di acustica 02) |
| Missili | in Artico niente lancio in immersione sotto/vicino al ghiaccio (USNI, media); [GIOCO] lancio solo in polynia o dopo l'emersione |
| Comunicazioni | sotto il ghiaccio niente antenne di superficie; solo VLF/ELF in ricezione (STIMA) |

---

## 6. Armi e meteo

| Arma | Dati / regola | Fonte | Conf. |
|---|---|---|---|
| Missili radenti (Harpoon, Exocet, SS-N-2/9/22) | in mare grosso la traiettoria va comandata **considerevolmente più alta** per avere margine sulle creste; meglio adattarla allo stato del mare locale | brevetto EP0250039B1 — https://patents.google.com/patent/EP0250039B1/de | media |
| Quota tipica di un radente (Exocet) | ~10 m in crociera, ~3 m nel terminale | brevetto US5400688 — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5400688 | bassa-media |
| Radioaltimetro disturbato dalle onde | il missile "segue" le onde, traiettoria complessa e consumo maggiore | eArticle — https://www.earticle.net/Article/A284216 | media |
| **[GIOCO] Radenti** | quota di crociera = max(quota base, 1,5·Hs + 2 m) (STIMA). Più alto = radar di bordo lo vede prima: orizzonte radar +~1-2 nm in SS6. Il seeker attivo soffre di clutter in SS ≥ 5: P(aggancio) ×0,85; in SS ≥ 7 P(impatto in mare) +5-10% per i radenti piccoli | — | STIMA |
| Tomahawk (TLAM/TASM) da sottomarino | lancio dal tubo/VLS indipendente dal meteo in superficie, salvo SS ≥ 7 durante l'uscita dall'acqua (STIMA); DSMAC (TLAM-C) degradato da nebbia/neve fresca che alterano la scena; TASM come i radenti | n.v. | STIMA |
| Sub-Harpoon (capsula) | stesse regole dei radenti dopo l'uscita dall'acqua; [GIOCO] P(guasto all'affioramento della capsula) +5% in SS ≥ 6 | n.v. | STIMA |
| SS-N-12/19 (Slava/Kirov/Oscar) | grandi missili ad alta quota fino al terminale: poco sensibili al mare; dipendono dal targeting (Tu-95RTs, satelliti) che il meteo può degradare (nubi per EO, mare per radar) | n.v. | STIMA |
| Siluri in acqua bassa/vicino alla superficie (Mk 46, Mk 48, 53-65, SET-65) | il rumore delle onde e la riverberazione di superficie degradano l'homing attivo/passivo; in SS ≥ 5 un siluro con quota di tetto bassa può **affiorare** e perdere il bersaglio | n.v. | STIMA |
| **[GIOCO] Siluri** | quota di tetto minima automatica = 10 m + 1,0·Hs; contro bersagli in superficie in SS ≥ 6: P(acquisizione) ×0,8, P(broach) 5% | — | STIMA |
| Siluri aviolanciati (Mk 46 da P-3/S-3/SH-60, APR/AT-1 sovietici) | il paracadute e l'ingresso in acqua soffrono di vento forte e frangenti: [GIOCO] P(guasto all'ingresso in acqua) 3% in SS ≤ 4, 10% in SS6, 25% in SS ≥ 7; deriva del punto d'impatto col vento | n.v. | STIMA |
| Boe | SSQ-53D fino a SS6 (media); attive moderne operano fino a SS5 e sopravvivono fino a SS7 (media); rumore ambiente in crescita con vento/stato del mare (curve di Wenz/Knudsen, vedi file 02) → portata ridotta | §3.1 | media |
| Mine | mine ormeggiate strappate dalle tempeste e alla deriva; caccia mine (sonar, sommozzatori) **X** in SS ≥ 4; spinta ondosa sui sensori a pressione (le mine a pressione sono progettate per scartare le onde lunghe, ma in mare grosso aumentano i falsi trigger/le disattivazioni) | n.v. | STIMA |

---

## 7. Nebbia, cielo basso, precipitazioni — trasversale

| Fenomeno | Chi colpisce | Regola [GIOCO] (STIMA) |
|---|---|---|
| Nebbia (vis < 1 km) | ricerca visiva da aerei/navi/periscopio; recuperi su portaerei (CASE III); decollo da basi | visiva X oltre la visibilità; radar, sonar ed ESM invariati; CASE III con tempi di recupero ×1,5 e 5% di dirottamenti |
| Cielo basso (< 1.000 ft) | MAD e ricerca visiva costringono a stare sotto lo strato; P-3/Il-38 in IMC a bassa quota | ceiling < 300 ft di notte → niente passaggi MAD sotto le nubi (X); di giorno D |
| Pioggia forte | radar banda X (APS-115/APS-124, radar di navigazione): attenuazione e clutter di pioggia | portata radar ×0,6-0,8 nei rovesci; periscopi sotto la pioggia più difficili da avvistare (×0,5) |
| Temporali | tutti gli aeromobili; operazioni sul ponte (rifornimento, armi) | celle come aree di esclusione mobili di 10-20 nm; con fulmini entro 5 nm sospendere la movimentazione di armi/carburante sul ponte (STIMA; documentata solo la regola civile FAA dei 20 mi, media) |
| Neve | visibilità, ponte di volo scivoloso, ghiaccio sulle armi | come nebbia leggera + effetti icing §4.3; neve fresca su terra degrada il DSMAC del TLAM |
| Banchisa / ghiaccio galleggiante | navi senza classe ghiaccio, boe, sonar di scafo | navi da guerra: X dentro la banchisa compatta, D nel ghiaccio sparso (velocità ≤ 10 kn, sonar ×0,5 per rumore e urti); boe X sul ghiaccio compatto (solo nelle polynie/aperture); elicotteri: nessun ammaraggio d'emergenza sicuro |

---

## 8. Matrice compatta finale [GIOCO] — piattaforma × condizione (N/D/X)

Stato del mare NATO. Vento = sostenuto. Valori documentati in **grassetto**, il resto STIMA.

| Piattaforma | SS3 | SS4 | SS5 | SS6 | SS7+ | Ghiaccio moderato | Temporale vicino | Nebbia |
|---|---|---|---|---|---|---|---|---|
| SH-60B (FFG/DD con RAST) | N | N | D (**limite di progetto**) | D/X (dipende dal moto nave **±8°/±3°**) | X | D | X | D |
| SH-60F / SH-3H (CV) dipping | N | D | D | X (**onde > 10 ft**, bassa) | X | X | X | D |
| Ka-25 / Ka-27 | N | N | D (**Ka-27 fino a 5 баллов**) | X | X | D (pale riscaldate) | X | D |
| P-3C / Il-38 / Tu-142 boe e MAD | N | N | N | D (**SSQ-53 fino a SS6**) | D/X | D | X localmente | D (visiva X) |
| S-3A/B (CV) | N | N | N | D (ponte che beccheggia) | X | D | X | D/CASE III |
| Be-12 su acqua | D (**3 баллa**) | X | X | X | X | D in volo | X | D |
| Fregata 130 m: elicottero | N | N | D (**operabilità ~33%** in SS5) | X quasi sempre | X | D | D | N |
| Fregata 130 m: sonar di scafo | N | D | D | D | X a velocità alta | N | N | N |
| Nanuchka: armi | N | N | D (**fino a 5 баллов**) | X | X (cerca ridosso) | D/X stabilità | N | N |
| 688 a quota periscopica | N | D | D | X consigliato | X | — | N (fulmini ok) | periscopio visivo X |
| Kilo/Foxtrot allo snorkel | N | D | D | X | X | — | N | N |
| Missili radenti | N | N | D (quota più alta) | D | D/X | N | N | N |

---

## 9. Proposte di design riassuntive [GIOCO]

1. **Simulare il moto della nave (roll/pitch/heave)** con un modello semplice: risposta della nave allo spettro d'onda in funzione di lunghezza, rotta relativa e velocità. Da lì derivano i limiti degli elicotteri (±8°/±3° FFG; 10°/3° Ka-25), dei cannoni e del sonar. Così il giocatore capisce *perché* l'elicottero non decolla.
2. **Limiti come soglie "morbide"** (N → D → X) con probabilità d'incidente crescente, non come muri: rende plausibili gli incidenti del tipo Ka-27 del 1988.
3. **Asimmetrie storiche da sfruttare:**
   - le navi NATO con RAST/Beartrap fanno volare l'elicottero in condizioni in cui i sovietici (senza haul-down) restano a terra;
   - gli elicotteri sovietici hanno pale riscaldate (Ka-25, media) → più tolleranti al ghiaccio dell'SH-3H;
   - i 688 Flight I/II non sono adatti all'emersione nel ghiaccio, il 688I (1988+) sì;
   - la Nanuchka è letale col mare calmo e quasi neutralizzata oltre 5 баллов.
4. **Il maltempo come "copertura" per il sottomarino:** clutter radar, boe rumorose, elicotteri a terra, scorte rallentate. Il prezzo: niente quota periscopica, comunicazioni difficili, diesel che non ricaricano.
5. **Messaggi radio/briefing** (collegamento con il file del briefing): "Flight ops suspended – deck motion exceeds limits", "Weather below CASE II minimums", "Нелётная погода, волнение 6 баллов".

---

## 10. Lacune (n.v.) e dove cercarle

- SHOL numerici per SH-60B su FFG-7/DD-963/CG-47 (vento relativo per settore): NAVAIR 00-80T-122 + i report di NAVAIRTESTCEN sui test DI (DTIC ADA383636). Australiani: DSTO-TR-0463, TR-0093 (S-70B-2 su FFG Adelaide).
- Limiti di pitch del ponte per gli appontaggi su portaerei (MOVLAS/stop dei recuperi): CV NATOPS 00-80T-105 (non trovato in chiaro).
- Limiti del sonar a immersione AQS-13 e del Ka-27 "Ros-V" per stato del mare: NATOPS SH-60F/SH-3H; РЛЭ Ка-27.
- Be-12: altezza d'onda esatta per il decollo (РЛЭ Бе-12). Abbiamo solo "мореходность 3 балла".
- Snorkel di Kilo/Foxtrot: limite in баллы (memorie di sommergibilisti sovietici, "РДП при волнении").
- Velocità sostenibili misurate per classe con il mare di prua: USNI Proceedings sett. 1983 (bloccato), DTIC ADA045443 (analisi tenuta al mare FFG-7), Kehoe 1973 (DD-963).
- Limiti di lancio di ASROC, Mk 32, RBU, Harpoon da nave per stato del mare: manuali OP/OD (non pubblici).
- Formula esatta di Overland 1990 e coefficienti Cf/Cα di Kwon 2008: verificare sugli articoli originali.

## 11. Report DTIC da recuperare (server in manutenzione durante la ricerca)

- ADA383636 — SH-60B Dynamic Interface su DDG-993 (1984)
- ADA288546 — Crook 1994, effetti della superficie libera a quota periscopica
- ADA343523 — Danko 1997, ausilio decisionale per la rotta a quota periscopica
- ADA045443 — analisi di tenuta al mare FFG-7
- ADA305730 — modello di sopravvivenza in acqua fredda
- ADA214000 — turbolenza P-3C RAAF
- AD0286658 — emersione di sottomarini attraverso il ghiaccio (1962)

---

## 12. ESTENSIONE — Meteo e stato del mare come sistema unico: soglie per stato del mare Douglas/WMO 0-9

Richiesta del coordinatore: per **ogni** stato del mare (scala Douglas / codice WMO 3700) dare le soglie per piattaforma e sistema. Valgono le convenzioni di sopra. Dove la tabella dice STIMA è una regola di gioco calibrata sui pochi dati duri (in grassetto).

### 12.1 Scala Douglas/WMO (confidenza alta: è lo standard WMO; i valori di 3 e 5 sono confermati da neftegaz.ru, media)

| WMO | Descrizione | Hs (m) | Vento tipico se completamente sviluppato (kn, STIMA da Beaufort) | ≈ SS NATO (§1.1) |
|---|---|---|---|---|
| 0 | calmo, a specchio | 0 | 0-3 | 0 |
| 1 | calmo, increspato | 0-0,1 | 4-6 | 1 |
| 2 | poco mosso | 0,1-0,5 | 7-10 | 2 |
| 3 | mosso | 0,5-1,25 | 11-16 | 3 |
| 4 | molto mosso | 1,25-2,5 | 17-21 | 4 |
| 5 | agitato | 2,5-4 | 22-27 | 5 |
| 6 | molto agitato | 4-6 | 28-40 | 6 |
| 7 | grosso | 6-9 | 41-55 | 7 |
| 8 | molto grosso | 9-14 | 56-63 | 8 |
| 9 | tempestoso | >14 | ≥64 | >8 |

Il mare segue il vento con ritardo: [GIOCO] Hs tende a Hs_eq(vento) con costante di tempo di 6-12 h (fetch lungo, oceano) o 2-4 h (mari chiusi: Baltico, Okhotsk sotto costa). Il **mare lungo (swell)** resta dopo che il vento cala e aumenta λ: i suoi effetti in profondità (§1.4) durano più del vento.

### 12.2 Sottomarini immersi: quota periscopica, broaching, profondità di fuga

**Fisica (confidenza alta).**
- *Profondità di fuga ("wave base")*: sotto **λ/2** il moto dell'acqua è < 4% di quello in superficie (fattore e^−π) — Wikipedia "Wave base", https://en.wikipedia.org/wiki/Wave_base (media, fisica standard); i sottomarini si sottraggono ai moti scendendo sotto la wave base — Springer Math. Ann., https://link.springer.com/article/10.1007/s00208-019-01910-8 (alta). Con il periodo modale NATO: SS4 λ/2 ≈ 60 m, SS5 ≈ 75 m, SS6 ≈ 120 m, SS7 ≈ 175 m, SS8 ≈ 210 m. Per il comfort ne basta di solito circa metà (fattore 0,2, §1.4).
- *Risucchio (suction)* verso la superficie, costante nel tempo, che aumenta avvicinandosi alla superficie, con la velocità e con l'altezza d'onda (§5.1, alta). Si somma a una forza oscillante alla frequenza d'incontro.
- *Momento di Munk*: un corpo affusolato con angolo d'attacco ha un momento instabilizzante che tende ad aumentare l'angolo. A quota periscopica la vicinanza della superficie rende il flusso asimmetrico (più veloce sopra) e aggiunge un momento di beccheggio, oltre al risucchio. Per questo a bassa velocità il battello "non tiene la quota" e a velocità alta il periscopio fa scia (fisica: alta; numeri n.v.).
- *Compromesso sulla velocità*: l'autorità dei timoni cresce con v², il che spinge ad andare veloci; ma oltre **~5 kn** il periscopio fa un "feather" ben visibile (StrategyPage, https://www.strategypage.com/htmw/htsub/articles/19991109.aspx, bassa-media) e l'esposizione dovrebbe restare di **6-7 s** per volta (stessa fonte). Le scie di periscopi e snorkel sono rilevabili da radar, sonar e a vista (UTAS 2015, https://ecite.utas.edu.au/105242, alta). L'RCS dell'albero è piccola e la sua scia aiuta il radar a scoprirlo (US Navy SBIR N251-014, https://navysbir.com/n25_1/N251-014.htm, alta).

**Classi del gioco: fattore di taglia per la quota periscopica [GIOCO].** Dimensioni da Wikipedia (media); fattori STIMA. "Kpd" moltiplica l'errore di quota del §5.2: < 1 = più stabile. I battelli lunghi e pesanti filtrano le onde corte ma risentono di più del mare lungo (risucchio ∝ superficie del dorso), quelli piccoli e corti "ballano" con le onde corte.

| Classe | L × B (m) | Disloc. imm. (t) | Timoni orizzontali | Kpd (SS ≤ 5) | Kpd (SS ≥ 6, mare lungo) | Note |
|---|---|---|---|---|---|---|
| 688 (Flight I/II) | 110 × 10 | ~6.900 | sulla vela | 1,0 | 1,0 | riferimento; timoni sulla vela meno efficaci vicino alla superficie quando la vela è in cavo d'onda (STIMA) |
| 688I (1988+) | 110 × 10 | ~6.900 | a prua, retrattili | 0,9 | 1,0 | |
| Sturgeon | 89 × 9,7 | ~4.700 | sulla vela | 1,1 | 0,9 | |
| Trafalgar | 85 × 9,8 | ~5.200 | a prua | 1,1 | 0,9 | |
| Alfa | 81 × 9,5 | ~3.200 | a prua | 1,3 | 0,9 | piccolo e leggero: molto mosso a quota periscopica |
| Victor III | 107 × 10,6 | ~7.250 | a prua | 1,0 | 1,0 | |
| Akula | 110 × 13,6 | ~12.800 | a prua | 0,85 | 1,1 | grande riserva di galleggiamento |
| Oscar II | 155 × 18,2 | ~24.000 | a prua | 0,7 | 1,3 | dorso enorme → forte risucchio con mare lungo |
| Typhoon | 175 × 23 | ~48.000 | a prua | 0,6 | 1,4 | idem; la quota periscopica serve poco (SSBN) |
| Charlie II | 104 × 10 | ~5.500 | a prua | 1,0 | 1,0 | |
| Kilo (877) | 73 × 9,9 | ~3.950 | a prua | 1,2 | 0,9 | snorkel, §5.2 |
| Foxtrot (641) | 91 × 7,5 | ~2.475 | a prua | 1,4 | 1,0 | scafo stretto, rolla molto anche immerso a bassa quota (STIMA) |

**Tabella per stato del mare WMO — sottomarino immerso (STIMA salvo indicazione)**

| WMO | Quota periscopica (688 di riferimento) | Broaching (vela affiora, v < 4 kn o mare al traverso) | Profondità minima per "calma" (fattore < 0,1) | Lancio di armi a quota periscopica / bassa quota |
|---|---|---|---|---|
| 0-2 | ±0,5 m; feather visibilissimo oltre 3 kn, periscopio avvistabile a vista/radar fino a 6-10 nm da aerei | 0 | 20 m | tutto N |
| 3 | ±1 m | 0 | 40 m | N |
| 4 | ±1,5-2 m; il periscopio si immerge per il 20-30% del tempo | 0,2%/min | 60 m | siluri N; missili da tubo (Sub-Harpoon/TASM/SS-N-15/16) N |
| 5 | ±2-3 m; immerso per il 40-50% | 1%/min | 80 m | missili da tubo D: P(guasto all'uscita/affioramento) +5%; antenna di comunicazione a intermittenza |
| 6 | ±3-5 m; inutilizzabile per la sorveglianza | 5%/min | 120 m | missili D/X: P +15%; il comandante dovrebbe lanciare da 30-40 m (lancio sotto quota periscopica, se l'arma lo consente) |
| 7 | X (il battello non tiene la quota) | > 10%/min | 150+ m | missili lanciati a profondità maggiore se possibile; altrimenti X |
| 8-9 | X | — | 200+ m | X tranne i lanci profondi (SLBM n.v.; Granit si lancia in immersione, profondità n.v.) |

**Avviso di lancio dei missili sovietici:** il P-700 Granit (Oscar) è progettato per il lancio in immersione (Weapons & Warfare, https://weaponsandwarfare.com/?p=13069, media). Profondità e stato del mare limite **n.v.** [GIOCO] Lanciabile fino a WMO 6 da 30-40 m (STIMA).

### 12.3 Sottomarini in superficie (navigazione, plancia sulla vela, avarie)

| Dato | Fonte | Conf. |
|---|---|---|
| Un sottomarino in superficie rolla di continuo, anche col mare calmo; in mare grosso è difficile muoversi o dormire | The War Zone (testimonianza di un sommergibilista) — https://www.twz.com/43481/damaged-submarine-likely-had-a-nightmare-voyage-to-san-diego-says-veteran-submariner.md | media |
| Le vedette sono imbragate in plancia; in tempesta onde più alte della vela hanno quasi annegato una vedetta e la plancia è stata sgomberata | stessa fonte + RallyPoint — https://www.rallypoint.com/answers/why-do-lookouts-wear-construction-type-harnesses | bassa-media |
| K-278 Komsomolets (aprile 1989, Mar di Norvegia): emerso dopo 11 min di incendio, affondato circa 4-5 h dopo; acqua a **2 °C**; le zattere lanciate dagli aerei erano **inutilizzabili per vento e mare**; 42 morti su 69, di cui **30 in acqua** per ipotermia/ferite; il primo soccorso navale è arrivato **81 min dopo l'affondamento** | Wikipedia — https://en.wikipedia.org/wiki/Soviet_submarine_K-278_Komsomolets | media |
| K-8 (aprile 1970, Golfo di Biscaglia): incendio, battello in superficie; affondato durante il rimorchio con mare grosso dopo circa 80 h di lotta per la sopravvivenza della nave "in stormy conditions"; 52 morti tra quelli tornati a bordo | Wikipedia — https://en.wikipedia.org/wiki/Soviet_submarine_K-8 | media |

**Regole per il sottomarino in superficie [GIOCO], STIMA**

| WMO | Plancia/vedette | Rollio in superficie (688; Foxtrot/Kilo ×1,3; Typhoon/Oscar ×0,6) | Rischio di allagamento dai portelli/presa d'aria | Velocità massima in superficie |
|---|---|---|---|---|
| 0-3 | N | ±3-8° | 0 | piena |
| 4 | N, imbragati | ±10-15° | piccolo, portello di plancia sorvegliato | ×0,9 |
| 5 | D: vedette ridotte, plancia bagnata | ±15-25° | 0,5%/h se c'è un portello aperto | ×0,7 |
| 6 | D/X: plancia sgomberata, governo dalla camera di manovra, solo periscopio/radar | ±25-35° | 2%/h con un portello aperto; i battelli danneggiati con poca riserva di galleggiamento perdono stabilità | ×0,5 |
| 7+ | X | ±35-45°, rischio di perdita dell'assetto per i battelli danneggiati | 5-10%/h per un battello avariato e appoppato/appruato (lezione di K-8/K-278) | ×0,3; meglio immergersi se si può |

**Emersione d'emergenza (soffiata rapida) [GIOCO], STIMA:** in WMO ≤ 4 il battello affiora e si stabilizza. In WMO 5-6 l'emersione con grande angolo fa "saltare" la prua e il battello ricade, poi resta fermo al traverso rispetto al mare con rollio > 30°: allagamenti del §12.3 ×2 e feriti a bordo. In WMO ≥ 7 l'emersione d'emergenza è l'ultima risorsa: l'equipaggio fuori dallo scafo in mare ha una sopravvivenza da §2.1 e §12.7.

### 12.4 Navi di superficie: ampiezze di moto per dislocamento e stato del mare

**Dati duri**
- DD-963 (Spruance, ~8.000 t), simulazione MIT a 5 gradi di libertà per la NASA: con **SS5 (H 10 ft, ωm 0,72 rad/s), 15,5 ft/s (~9 kn), mare a 45°** → rollio **4,56° RMS**, sbandamento laterale 0,60 ft RMS, imbardata 0,23° RMS. Pubblicazione: Triantafyllou/Athans, MIT LIDS per NASA Langley, 1982 — https://ntrs.nasa.gov/api/citations/19820023761/downloads/19820023761.pdf (alta). **Supera già il criterio STANAG di 4° RMS**: una nave da 8.000 t a bassa velocità con mare al mascone in SS5 è al limite per l'elicottero.
- Criteri STANAG 4154 e indice di operabilità del 33% per una fregata in SS5 (§2.1, alta).
- Statistica di Rayleigh (alta, teoria standard): ampiezza significativa (media del terzo più alto) ≈ **2 × RMS**; massimo atteso su N oscillazioni ≈ RMS·√(2 ln N) → in 1 h (~500 rollii) ≈ **3,5 × RMS**. Quindi 4° RMS ≈ ±8° di ampiezza significativa, coerente con il limite NATOPS della FFG-7 (±8°/±3°).
- Flusso sul bulbo sonar: oltre **18 kn** il rumore di flusso degrada gravemente il sonar e "in high sea state at such speeds, sonar is virtually inoperative" — brevetto US4333169, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4333169 (media).
- Bolle trascinate sotto lo scafo (sweep-down): perdite di dati sonar "completely correlated" con le bolle sui trasduttori — UNOLS, https://www.unols.org/sites/default/files/AcousticIssuesOnResearchVessels.pdf (media; dati su navi di ricerca).

**Tabella delle ampiezze [GIOCO], STIMA calibrata sul dato DD-963.** Rollio RMS con mare al traverso o al mascone a velocità media; beccheggio RMS con mare di prua. Per avere l'ampiezza significativa moltiplicare per 2, per il massimo orario per 3,5.

| WMO | Nanuchka ~700 t, 59 m | Fregata ~3.600 t (Perry, Krivak, Koni ×1,2) | Incrociatore ~8.000 t (Spruance/Kidd, Kara, Udaloy, Slava) | Kirov ~24.000 t | Portaerei ~80.000 t |
|---|---|---|---|---|---|
| | roll / pitch (° RMS) | roll / pitch | roll / pitch | roll / pitch | roll / pitch |
| 3 | 2,5 / 1,5 | 1,0 / 0,5 | 0,8 / 0,4 | 0,5 / 0,2 | 0,2 / 0,1 |
| 4 | 4 / 2,5 | 2,5 / 1,2 | 2,0 / 0,9 | 1,2 / 0,5 | 0,5 / 0,3 |
| 5 | 7 / 4 | 5 / 2,0 | **4,5** / 1,5 | 2,5 / 0,9 | 1,0 / 0,6 |
| 6 | 10 / 5,5 | 7 / 3,0 | 6 / 2,3 | 4 / 1,5 | 2,0 / 1,0 |
| 7 | 14 / 7 (sopravvivenza) | 10 / 4,0 | 8 / 3,2 | 6 / 2,3 | 3,0 / 1,6 |
| 8-9 | > 18 (rischio di perdita della nave) | 13 / 5 | 10 / 4 | 8 / 3 | 4 / 2,2 |

Modificatori: stabilizzatori a pinne attive (Perry: no; Spruance: no; Udaloy pr. 1155.1: sì, ru.wiki; portaerei: no) → roll ×0,6. Mare in poppa → pitch ×0,6, roll ×0,8. Velocità sopra il limite del §4.2 → slamming.

**Matrice nave per stato del mare WMO [GIOCO], valori della fregata da ~3.600 t salvo indicazione**

| WMO | Velocità sostenibile (§4.2) | Slamming / acqua verde (criteri **20/h e 30/h**) | Sonar di scafo | Sonar trainato / VDS | Armi | Elicottero imbarcato (**±8°/±3°**, **2 m/s** RMS ponte, **35 kn**) |
|---|---|---|---|---|---|---|
| 0-3 | piena | 0 | N (sopra 18 kn: −3 dB per il flusso) | N | N | N |
| 4 | 29 kn | sotto le soglie fino a 25 kn | ×0,85; sopra 18 kn ×0,6 | N | N | N |
| 5 | 25 kn | alle soglie a 25 kn con mare di prua | ×0,7; sopra 18 kn "quasi inoperativo" (**brevetto**) | trainato N, VDS D | cannoni di prua D, tubi D | D (operabilità ~**33%**: serve scegliere rotta e velocità) |
| 6 | 18 kn | alle soglie a 18 kn | ×0,5 + quench quando la prua emerge (STIMA: 10-30% del tempo) | trainato D (strumming), VDS X recupero | RBU/ASROC D, tubi X sopravvento | X quasi sempre (roll 7° RMS ≈ ±14° significativi) |
| 7 | 12 kn | sopra le soglie a qualsiasi velocità > 12 kn | ×0,3, a intermittenza | trainato D/X (filare/recuperare X) | quasi tutto X tranne SAM/SSM | X |
| 8-9 | 8 kn, cappa | danni strutturali possibili | X | X | X | X |

Per Nanuchka spostare tutto **di un gradino e mezzo verso il peggio** (armi X oltre 5 баллов ≈ WMO 5, fonte media). Per Kirov/portaerei di un gradino verso il meglio.

### 12.5 Aerei ASW per stato del mare WMO [GIOCO]

| WMO | Sonar a immersione (SH-60F/SH-3H/Ka-27) | Boe (SSQ-53/41/62, RGB) | MAD (turbolenza correlata al vento) | Hover sopra il mare (vento) | Be-12 in acqua |
|---|---|---|---|---|---|
| 0-2 | N | N (rumore ambiente basso: portata massima) | N | N | N |
| 3 | N | N | N | N | D (**3 баллa = limite**) |
| 4 | D: cavo inclinato, errore di rilevamento ±10° | N, portata ×0,8 per il rumore | N/D | D (vento 17-21 kn) | X |
| 5 | D/X (Ka-27 fino a **5 баллов**; SOP USA **onde > 10 ft** = 3 m → no) | N (SSQ-53D **fino a SS6**), portata ×0,6 | D: turbolenza moderata vicino al mare | D (22-27 kn) | X |
| 6 | X | D (limite SSQ-53D), portata ×0,45; attive moderne X (**operano fino a SS5**) | D/X | X per il sonar a immersione; hover per SAR D | X |
| 7 | X | X funzionali, sopravvivenza (**SS7** per SSQ-963D) | X sotto 300 ft | SAR solo D/X | X |
| 8-9 | X | X | X | X | X |

### 12.6 Armi per stato del mare WMO [GIOCO], STIMA salvo indicazione

| WMO | Siluri vicino alla superficie | Missili radenti (quota di crociera) | Missili da sottomarino (§12.2) | Mine ormeggiate | Mine alla deriva |
|---|---|---|---|---|---|
| 0-3 | N | quota base (Exocet ~**10 m**, terminale ~**3 m**, bassa-media) | N | N | rare |
| 4 | tetto ≥ 10 m + Hs; contro bersagli in superficie N | +2 m | N | N | |
| 5 | tetto ≥ 14 m; P(broach) 2%; homing ×0,9 per il rumore di superficie | +4 m (**traiettoria "considerevolmente più alta" in mare grosso**, brevetto, media) | D | la mina "beccheggia"; profondità di ormeggio effettiva variabile | |
| 6 | P(broach) 5%; homing ×0,8 | +6 m; radar del bersaglio la vede 1-2 nm prima | D/X | carichi sull'ormeggio alti; P(rottura) 0,5%/giorno per le mine vecchie | dopo la tempesta compaiono mine alla deriva (P per campo minato 1-5%) |
| 7 | aviolancio P(guasto all'ingresso) 25%; siluri da nave X | +8 m; P(impatto in mare) +5% | X | rotture 2%/giorno; mine poco profonde visibili nei cavi d'onda | idem, rischio per le navi amiche |
| 8-9 | X | radenti piccoli D/X | X | rotture 5%/giorno | |

Base documentale per le mine: studio DTMB 1966 sul beccheggio RMS delle mine ormeggiate in **SS4-7** a varie profondità — DTIC AD0632077, https://apps.dtic.mil/sti/pdfs/AD0632077.pdf (alta, solo abstract); misure WHOI su un modello CAPTOR in scala reale: tensioni dell'ormeggio e strumming in funzione della corrente — https://agris.fao.org/search/en/records/647367bd2c1d629bc9801dbb (alta, abstract). Soglie di rottura **n.v.**

### 12.7 Danni, abbandono nave e soccorso per stato del mare WMO [GIOCO]

Dati: Komsomolets (zattere inutilizzabili per vento e mare, acqua a 2 °C, 30 morti in acqua: media); sopravvivenza a 5 °C 1,9 h senza protezione, 4,8 h con tuta anti-esposizione, 12,6 h con tuta stagna (DTIC, alta); salire su una zattera col mare grosso è difficile e spesso si entra dall'acqua (Ocean Safety, https://www.oceansafety.com/latest-news/527-what-s-it-like-to-have-to-use-a-liferaft ; Marine Insight, https://www.marineinsight.com/video-how-to-launch-and-board-an-inflatable-life-raft/ — media); i limiti delle gru delle lance in mare alto "not fully explored" (TransNav, https://yadda.icm.edu.pl/yadda/element/bwmeta1.element.baztech-3666baa9-590b-4fd9-99bb-4ff843de0486/c/Use_Transnav_1_24s.pdf — alta). Limiti numerici di verricello e lance **n.v.**

| WMO | Allagamento di una nave avariata | Abbandono nave (zattere/lance) | Soccorso con elicottero (verricello) | Soccorso navale (recupero naufraghi) |
|---|---|---|---|---|
| 0-3 | progressivo solo dal danno | N | N | N |
| 4 | +10% | N (lance con gru D) | N | N |
| 5 | +25% (acqua in coperta, nei boccaporti danneggiati) | D: 10% di zattere perse/capovolte | D: tempo per persona ×1,5 | D: tempo ×1,5 |
| 6 | +50%; le navi < 100 m con falla a prua rischiano di perdere stabilità | D/X: 30% delle zattere perse; lance X | D/X di notte | D: recupero lento, 5% di perdite durante il recupero |
| 7 | ×2; rimorchio quasi impossibile (lezione di K-8) | X praticamente (come Komsomolets) | X di notte, D di giorno | X tranne navi grandi a ridosso |
| 8-9 | ×3 | X | X | X |

Sopravvivenza in acqua: usare il §2.1 con la temperatura del mare dal modello oceanografico. Mare grosso → tempo ×0,8 (DTIC: il modello in "heavy seas" dà valori più bassi del mare calmo, alta).

### 12.8 Lacune aggiunte (n.v.)
- Rollio dei sottomarini in superficie per classe; limiti della plancia (manuali SSN/SSORM, n.v.).
- Profondità e stato del mare limite per il lancio di Sub-Harpoon/TASM/Granit/SS-N-21 (n.v.).
- Soglie di aerazione/quench dei sonar SQS-26/53/56 e dei sovietici Titan-2/Polinom (n.v.: dati di prove classificati).
- Ampiezze di moto misurate di FFG-7/DD-963 in SS6: DTIC AD0694049 ("most severe recorded ship motions, SS6"), DSTO TR-0093 (rilevi da riprendere quando DTIC torna).
- Carichi di rottura degli ormeggi delle mine e tassi di deriva dopo le tempeste (n.v.).
- Limiti di verricello SAR (SH-3/SH-60/Ka-27PS) per vento e mare (n.v.).
