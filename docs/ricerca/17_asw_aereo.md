# 17 — Mezzi aerei antisommergibile della Guerra Fredda, circa 1985-1991 (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. SSN-688 *Los Angeles* Flight I contro unità sovietiche, e il lato sovietico per giocare l'Alfa.
> Scopo: dare all'IA, alla grafica e al sonar dell'Enhanced elicotteri e pattugliatori ASW con sensori, armi, dottrina e
> procedure reali, e una macchina a stati con le probabilità. Il classico resta com'è.
> Mezzi: SH-60B, SH-60F, SH-3H, P-3C (Update II e III), S-3A/B; Ka-25PL, Ka-27PL, Il-38, Tu-142M/MK, Be-12.
> Compilato: 25 ottobre 2026, da tre ricerche parallele (mezzi USA, mezzi sovietici, il battello) con la sintesi della sessione sul runtime.

---

## 0. Convenzioni e avvertenze

**Unità.** nm = miglia nautiche; kn = nodi; ft = piedi; kyd = 1000 yarde (914 m); KTAS/KIAS = velocità vera/indicata.
Quote in m, con i piedi tra parentesi se la fonte li usa. Livelli sonori in dB re 1 µPa a 1 m (sorgente) o re 1 µPa
(ricevuto), sempre con la banda.

**Confidenza.**
- **alta**: fonte primaria o ufficiale (specifiche MIL, GAO, NHHC, regolamento sovietico tradotto dalla CIA, tesi con
  prove in volo, testimone diretto con dati);
- **media**: una fonte secondaria seria (FAS, Wikipedia con fonti, memorie, riviste);
- **bassa**: fonte amatoriale, wargame, dato isolato, o *n.v.* (non riverificato qui).

**STIMA** = valore proposto, non storico, da tarare. **[GIOCO]** = proposta per il remake. `[CALC]` = calcolo.

**Sigle delle fonti** tra parentesi quadre; l'elenco con gli URL è al §11.

**La fonte sovietica principale.** Quasi tutto ciò che è nuovo sulle procedure sovietiche viene da quattro articoli di
**A. M. Artem'ev**, ripresi da airwar.ru. Artem'ev è colonnello a riposo dell'aviazione navale sovietica (AV-MF),
pilota e ricercatore del 33° Centro d'impiego, testimone diretto e molto critico. I fatti sono **media-alta**, i
giudizi **media**. A fianco ci sono due documenti CIA declassificati: il rapporto sulla ASW sovietica del 1972 e la
traduzione del 1986 del **Regolamento di combattimento della Marina sovietica del 1983**, capitolo 7.

**Accesso.** Il 25-10-2026 DTIC e la CIA Reading Room erano in manutenzione: i documenti CIA si sono letti tramite
archive.org; le tesi della Naval Postgraduate School dall'archivio Calhoun.

**Cosa c'è già altrove.** Questo dossier completa e corregge:
- 01 §5 (dati dei mezzi);
- 02 §7.6-7.7 (boe, MAD, EMCON, periscopio);
- 11 §1.4, §3.3-3.4, §4-6 (radar, EMCON, ESM, scoperta radar dell'albero);
- la nota del requisito 13 di `docs/00_ROADMAP.md`.

I dati già verificati non si ripetono, se non per correggerli.

---

## 1. Correzioni e verifiche ai dossier esistenti

| Dove | Affermazione attuale | Correzione | Fonte | Conf. |
|---|---|---|---|---|
| 01 §5.4 | AQS-13: frequenze «9,25/10/10,75 kHz» non confermate | **Confermate: 9,230 / 10,000 / 10,770 kHz** (±15 Hz), singole o in sequenza | `[MILAQS13B]` | alta |
| 01 §5.4 | Cavo AQS-13 «450 ft» | Potenza misurata «con il cavo da 500 ft»: cavo utile ~450-500 ft sull'SH-3H. L'AQS-13F (SH-60F) ha **1575 ft** | `[MILAQS13B]` `[FASSH60]` | alta / media |
| 01 §5.4 | Hover di dip a 15-20 m | **40 ft (12 m)** sull'SH-3H (profilo NATC 1987); ~60 ft sull'SH-60F | `[RUDZIS]` `[FASSH60]` | alta / media |
| 01 §5.4 | Ciclo di dip 6-12 min | **Ricerca: dip di 15 min, salti di 10 nm. Inseguimento: ciclo di 4-6 min, salti di 5-7 nm** | `[RUDZIS]` | alta |
| 01 §5.4 | SH-3H max 144 kn, crociera 118 (dati SH-3D) | SH-3H a 20 800 lb: **max 122 KTAS, crociera 114, massima autonomia a 68 KTAS (3,8 h)**, raggio max 168 nm | `[RUDZIS]` | alta |
| 01 §5.4 | Boe: ricevitori ARR-75 | **12 scivoli per boe, 24 lanciatori di fumogeni**; carico di progetto **2 Mk 46 + 10 boe** | `[RUDZIS]` | alta |
| 01 §5.3 | Tempo in zona LAMPS III «1-3 h a 50 nm» n.v. | **3 h a 50 nm, 1 h a 150 nm** | `[NHHCSH60B]` | media-alta |
| 01 §5.3 | MAD ASQ-81(V)2 n.v. | MAD rimorchiato a **~180 ft** dietro e sotto l'elicottero | `[SBIRMAD]` | media |
| 01 §5.5 | Pattugliamento ~200 kn n.v. | **206 kt a 1500 ft**; raggio 845 nm con 3 h in zona | `[FASP3]` | media |
| 01 §5.5, 11 §1.4 | APS-137 «opzione 1990+» sul P-3 | Sul P-3C arriva con l'**AIP, consegne dal 1998**: nel 1989 tutti i P-3C hanno l'**APS-115** | `[FASP3]` | alta |
| 11 §1.4, §3.3 | APS-137 sull'S-3 nel 1989 | Le **S-3B** (APS-137, ALR-76, Harpoon) sono operative **dal luglio 1988** (VS-30). Le S-3A della costa est vengono convertite fra il 1987 e il 1991, quelle della costa ovest nel 1992-94. Nel 1989 una portaerei atlantica può avere S-3B, una pacifica ha le S-3A | `[FASS3]` `[WPS3]` | alta |
| 02 §7.6 | SSQ-41: 60/1000 ft | SSQ-41 (1965) a **60 o 300 ft**; la **SSQ-41B** a un idrofono ha quote 60-1000 ft e vita 1/3/8 h | `[HOLLER]` `[FASSSQ41]` | alta / media |
| 02 §7.6 | SSQ-47: dati mancanti | **Attiva, solo distanza, ping CW ogni 10 s**, una di 6 frequenze fra **13 e 19 kHz**, quote 60/800 ft, vita **~30 min** | `[HOLLER]` | alta |
| 01 §5.3 | Armi 2 Mk 46 Mod 5 | Confermato: il Mod 5A nasce da un programma **avviato nel 1989**, quindi nel 1989 c'è solo il Mod 5 | `[FASMK46]` | alta |
| — | SH-60F (non trattato) | **Niente radar e niente MAD**; 6 tubi di boe a gravità; in linea dal **22-6-1989** solo col reparto addestrativo HS-10: **anacronistico come elicottero operativo nel 1989** | `[SIKS70B]` `[WPSH60]` | media |
| 01 §5.1, §5.8 | Boe del Ka-25: «sgancio ≤ 200 m, 20 min, ricezione 0,3-25 km» | Sono i dati della **boa radar-faro RLB «Poplavok-1A»** (3 in una cassetta, guidano l'elicottero d'attacco), non delle RGB acustiche | `[RW-Ka25]` | alta |
| 01 §5.6 | Tu-142MK: RGB-1 (22/cassetta), RGB-2 | È la dotazione del **Tu-142 base** («Berkut-95»). Il Tu-142M/MK «Korshun-K» usa **RGB-75, RGB-15, RGB-25, RGB-55A** e le sorgenti esplosive MGAB | `[ART-Tu142]` | alta |
| 01 §5.6 | Tu-142M e Tu-142MK distinti | **Sono lo stesso aereo** (sistema accettato nel 1980 come «Korshun-Kaira»). Il **Tu-142MZ** entra in servizio **dopo il 1991** | `[ART-Tu142]` `[RW-Tu142]` | alta / media |
| 01 §5.6 | Il-38: 144 RGB-1 + 10 RGB-2 + **3 RGB-3** | La RGB-3 **in pratica non si usò**. Carico «ricerca e attacco»: **144 RGB-1 + 10 RGB-2 + 10 bombe + 2 siluri** | `[ART-Il38]` | alta |
| 01 §5.9 | Ka-25/27 in coppia da Kara, Kirov, Slava | Kresta II, Kara, Slava e Sovremennyj imbarcano **un solo** elicottero (Kirov 3, Udaloy 2). Il regolamento del 1983 assegna la **ricerca** alle navi con un **gruppo** di elicotteri (Moskva, Kiev, coppia dell'Udaloy); l'elicottero singolo **classifica e passa il contatto** | `[CIA-REG86]` §355 | alta |
| 03 | Salti del sonar a immersione «3-5 nm» | Ka-27: **passo 4-6 km (2,2-3,2 nm)**; **3-3,5 km fra elicotteri** del gruppo in linea | `[ART-Ka27]` | media-alta |
| 01 §5.2 | VGS-3: cavo n.v. | La fonte dà la **profondità massima del corpo immerso: 150 m**. Il VGS-2 ha un cavo di 125 m | `[ART-Ka27]` `[RW-Ka25]` | media-alta |
| 11 §1.4 | Ka-25: radar «Puff Ball» | Initsiativa-2K, **55 kW** di picco. La CIA (1972) lo chiama **«Short Horn»**, quindi il nome NATO è incerto | `[RW-Ka25]` `[CIA72]` | media |
| 11 §3.4 | Radar del Tu-142M | **Agilità di frequenza impulso per impulso**, polarizzazione e fascio variabili: difficile da identificare all'ESM. A bordo l'ELINT **SRS-4 «Kvadrat-2»** e l'allarme radar **SPO-10**: il Bear-F sente il radar del sottomarino | `[ART-Tu142]` | media / alta |
| 01 | APR-3 sul Ka-27 | Nel 1985-91 l'arma è l'**APR-2 «Yastreb-M»** (1976); l'APR-3 è successiva | `[RW-APR2]` | media |

---

## 2. Mezzi della US Navy (fit 1989)

### 2.1 SH-60B Seahawk (LAMPS Mk III) — elicottero delle scorte

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Ruolo | «sensore remoto» della nave: la nave rileva (SQR-19 TACTAS, SQS-53/56) in 2ª-3ª zona di convergenza, l'elicottero vola sul rilevamento per classificare, localizzare e, se ordinato, attaccare | `[NAVSRC]` | media |
| Data link | **Hawk Link** in banda C: **AN/ARQ-44** a bordo, **AN/SRQ-4** sulla nave; audio delle boe e dati radar/ESM in tempo reale; l'acustica si elabora **sulla nave** (SQQ-28 nell'SQQ-89) | `[FASSH60]` `[DESIGSYS]` | media |
| Portata del link | non pubblicata. STIMA: ~50 nm a 500 ft, ~100 nm sopra 3000 ft (link a vista) | STIMA | bassa |
| Boe | **25 tubi pneumatici** sul fianco sinistro, carico pieno 25 | `[GAOALFS]` `[SIKS70B]` | alta |
| MAD | ASQ-81 rimorchiato a ~180 ft, sul pilone destro | `[SBIRMAD]` | media |
| Radar / ESM | APS-124 / ALQ-142 (dossier 11) | — | — |
| FLIR | **nessuno nel 1989** (retrofit pianificato per il 1996) | `[FASSH60]` | media |
| Armi | **2 Mk 46 Mod 5** (BRU-14/A); Penguin solo dal 1993 | `[FASSH60]` | alta |
| Tempo in zona | **TOS ≈ 4 − d/50 h** (d in nm dalla nave): 3 h a 50 nm, 1 h a 150 nm | `[NHHCSH60B]` + `[CALC]` | media-alta |
| Transito | STIMA 120-130 kt (dash 131 kt a 5000 ft in giornata tropicale) | `[SIKS70B]` | media |
| Quote | posa delle boe e transito STIMA 500-1500 ft; passaggio MAD ~150-200 ft | STIMA | bassa |
| Imbarco | 1-2 per nave (FFG-7, DD-963, DDG-993, CG-47); con 2 elicotteri **~1 in volo per 8-10 h al giorno** | STIMA | bassa |
| Vincoli | con mare alto le onde «spengono» le boe e bloccano la VHF verso la nave; l'elicottero è spesso distolto per la sorveglianza di superficie | `[WPSH60]` | media |

**Dotazione tipica ASW (STIMA, media):** 2 Mk 46, 25 boe (14-16 SSQ-53B DIFAR, 4-6 SSQ-62B DICASS, 1-2 SSQ-36 BT,
2-4 SSQ-41B), MAD, fumogeni e marcatori Mk 25. Privilegia la localizzazione perché lavora su contatti già rilevati
dalla nave.

### 2.1b SH-2F Seasprite (LAMPS Mk I) — elicottero delle Knox

Aggiunto il 30/10/2026 (runtime, per l'elicottero imbarcato dell'IA). Le fonti generali sull'SH-2 non sempre
distinguono le sottoversioni: i dati di bordo sono dell'SH-2F, autonomia e velocità variano fra le fonti.

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Ruolo | LAMPS Mk I: sensore remoto delle scorte (Knox; Kidd, Spruance e prime Perry prima del LAMPS III); l'acustica delle boe va alla nave | `[FASSH2]` `[GSSH2]` | media |
| Sensori | radar di superficie LN-66HP (non nel catalogo del dossier 11); MAD ASQ-81 rimorchiato; **nessun sonar a immersione** | `[ACSH2F]` `[GSSH2]` | media |
| Boe | circa 15: SSQ-41 (LOFAR) e SSQ-47 (attive di distanza); alcune fonti citano DIFAR e DICASS. Nel gioco: 9 SSQ-41B, 5 SSQ-47B, 1 SSQ-36 | `[ACSH2F]` `[GSSH2]` | media |
| Armi | 2 piloni esterni: 2 Mk 46, oppure 1 Mk 46 e un serbatoio supplementare | `[FASSH2]` `[GSSH2]` | alta |
| Velocità | massima ~140 kt, crociera ~120 kt | `[GSSH2]` | media |
| Autonomia | 3,5-4,5 h nelle fonti. Nel gioco TOS ≈ 2,5 − d/50 h (STIMA, più corto dell'SH-60B) | STIMA | bassa |
| Canali delle boe | non pubblicati. STIMA: 4 | STIMA | bassa |

### 2.2 SH-3H Sea King — zona interna della portaerei

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Sonar | **AN/AQS-13B** (13E con FFT dai primi anni '80): attivo, passivo, telefonia, classificazione «ASPECT», batitermografo | `[MILAQS13B]` | alta |
| Attivo | **9,23 / 10,00 / 10,77 kHz**; impulso CW **3,5 o 35 ms**; scale **1, 3, 5, 8, 12, 20 kyd** (fissano la cadenza); sorgente **≥ 211 dB re 1 µPa a 1 yd** in alta potenza, **≥ 199 dB** in bassa (automatica con trasduttore sopra 35 ft o sulla scala da 1 kyd) | `[MILAQS13B]` | alta |
| Ricevitore | banda di 500 Hz; **16 fasci preformati**; MTI Doppler a 1/2/3 kt | `[MILAQS13B]` | alta |
| Telefonia | portante **8,087 kHz**, compatibile UQC-1 | `[MILAQS13B]` | alta |
| Portata utile | profilo NATC: restare **entro 3000 yd** dal sottomarino per le prestazioni ottimali | `[RUDZIS]` | alta |
| Altri sensori | MAD, **12 scivoli per boe**, **24 fumogeni**; radar leggero su parte degli esemplari n.v. | `[RUDZIS]` | alta / bassa |
| Armi | **2 Mk 46** (o bombe di profondità); **B57** nucleare; in giornata calda 1 Mk 46 | `[RUDZIS]` `[WPSH3]` | alta / media |
| Prestazioni | 122 KTAS max, 114 crociera, 68 per la massima autonomia (3,8 h); raggio max 168 nm | `[RUDZIS]` | alta |
| Profilo di missione ufficiale | salita a 150 ft → transito di 60 nm → **hover a 40 ft, 15 min di attivo** → uscita a 150 ft / 100 kt, **salto di 10 nm** → 6 dip in tutto → rientro di 40 nm; 4678 lb di carburante, **3,43 h** | `[RUDZIS]` | alta |
| Tempo in zona | STIMA **TOS_dip ≈ 3,0 − d/57 h** (d in nm dalla portaerei): a 20 nm ~2,6 h, a 60 nm ~1,9 h | `[CALC]` da `[RUDZIS]` | media |
| Giornata calda | con **40 °C** non tiene l'hover fuori effetto suolo, quindi **non può calare il sonar**; in giornata standard il peso per l'hover lascia combustibile per **3 dip invece di 6** | `[RUDZIS]` | alta |

### 2.3 SH-60F Ocean Hawk — solo per missioni 1990-91

- AQS-13F con cavo da 1575 ft, impulso più lungo, verricello più veloce, potenza maggiore; hover ~60 ft;
  6 tubi di boe a gravità; **niente radar e niente MAD**; 4 di equipaggio; Mk 46 `[WPAQS13]` `[SIKS70B]` (media).
- **[GIOCO]** Nel 1989 la portaerei usa l'**SH-3H**. L'SH-60F vale solo con data ≥ 1990: modello di dip dell'SH-3H,
  trasduttore fino a ~1450 ft (STIMA), calata e recupero −30% (STIMA), 6 boe.

### 2.4 P-3C Orion (Update II, II.5, III)

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Update II (1977), 44 aerei | IRDS AN/AAS-36, riferimento boe AN/ARS-3, Harpoon | `[FASP3]` | alta |
| Update III (1985-86), 50 nuovi | ricevitore **AN/ARR-78** (99 canali), processore **AN/UYS-1 (Proteus)**, ESM **ALR-66B**; **CHEX** raddoppia i canali di boe elaborati | `[FASP3]` | alta |
| Radar / MAD | **APS-115** (muso più coda) su tutti nel 1989; **ASQ-81** nel «pungiglione» di coda | `[FASP3]` | alta |
| Canali seguiti | fino a 32 boe passive (STIMA: Update II 16 DIFAR, Update III con CHEX 32 DIFAR) | `[HARP3]` | bassa |
| Boe | **48 tubi esterni** caricati a terra più **36 in cabina** (3 tubi pressurizzati e 1 scivolo); FAS **87** nel carico max | `[FASP3]` `[HARP3]` | alta / bassa |
| Armi | 4-8 Mk 46 (vano più piloni); 2-4 Harpoon (Update II+); bombe di profondità **Mk 54** (bomba, non il siluro del 2004) e **Mk 57** (B57 nucleare); mine Mk 55, Mk 56, Mk 60 CAPTOR | `[FASP3]` `[TWZS3]` | alta / media |
| Missione | **~9 h**, estesa a **~13 h** spegnendo un secondo motore; il n. 1 si spegne di routine | `[AGC13H]` | media |
| Tempo in zona | **TOS ≈ 8,1 − d/165 h**: 200 nm → 6,9 h; 500 nm → 5,1 h; 845 nm → 3 h | `[CALC]` su `[FASP3]` | media |
| Quote | ricerca acustica anche a **~20 000 ft** (portata VHF delle boe); pattugliamento di riferimento **1500 ft**; passaggi MAD a **200-300 ft** | `[DMNP3]` `[FASP3]` | media |
| Turni | contro i Delta nel GIUK, fine anni '70: **6 sortite al giorno con 4 h in zona** per coprire 24 h (insieme agli SSN); il rilevo arriva sul contatto prima che il precedente se ne vada | `[HOLLER]` `[AGC13H]` | alta / media |
| Squadrone | **9 aerei** (VP); equipaggio **11**: TACCO, NAV/COMM, 3 piloti, 2 tecnici di volo, **SS1-SS2 acustici**, **SS3 non acustico** (radar, ESM, MAD, IRDS), armiere, tecnico | `[FASP3]` | alta |
| Stazione continua | STIMA: **8-9 aerei** (uno squadrone) per tenere 1 P-3 in zona a 300-450 nm per settimane; 4-5 per un picco breve | `[CALC]` | media |

**Dotazione tipica per missione (STIMA, media):**
- *su cue SOSUS:* 40-50 SSQ-53B, 8-12 SSQ-41B, 4-6 SSQ-77A VLAD (acque profonde con traffico), 8-10 SSQ-62B,
  2-3 SSQ-36, 2-4 SSQ-57; 4 Mk 46;
- *barriera:* più LOFAR e VLAD, meno DICASS;
- *guerra:* 4-8 Mk 46 o mine, 2-4 Harpoon.

### 2.5 S-3A / S-3B Viking — zone media ed esterna della portaerei

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| S-3A | radar **APS-116** (300 giri/min, 150 nm), FLIR OR-89, ESM ALR-47, MAD ASQ-81 su asta retrattile, riferimento boe ARS-2 | `[WPS3]` `[TWZS3]` | media |
| S-3B (dal 1988) | **APS-137 ISAR**, ESM ALR-76, ricevitore ARR-78, processore AN/UYS-1, Harpoon | `[FASS3]` | media |
| Boe | **60 scivoli** (59 tattici); 31 frequenze; 16 boe visualizzate insieme | `[TWZS3]` | media |
| Armi | **4 Mk 46**; mine Mk 55/56/60; B57. Mk 46 **vietati contro bersagli di superficie** | `[TWZS3]` | media |
| Prestazioni | transito ~345 kt a 36 000 ft; ~**5 h in zona** senza rifornimento; raggio 460-530 nm | `[WPS3]` `[USNIS3]` | media |
| Equipaggio | pilota, COTAC, TACCO, SENSO | `[FASS3]` | alta |
| Per portaerei | 1 squadrone VS di ~8-10 S-3 più 1 squadrone HS di 6 SH-3H | `[HARPCVW]` | media |
| Prassi (testimonianza di un SENSO) | sortite di ~2 h (un ciclo del ponte) o ~4 h con rifornimento; radar in EMCON («uno o due giri», poi silenzio; il contatto che sparisce, il «sinker», ha la priorità); **campo di 12 boe**, la prima DIFAR a 90 ft, poi alternate **90/200 ft** sopra e sotto lo strato; BT omesso «perché fa rumore»; un secondo Viking 2-3 h dopo riposa il campo; Link 11; l'E-2C smista i compiti; Charlie II seguito ~4 h nel Tirreno | `[TWZS3]` | media |

---

## 3. Mezzi della marina sovietica (AV-MF, fit 1989)

### 3.1 Ka-25PL «Hormone-A» (sistema «Baikal»)

- Raggio 200 km, **~2 h**; crociera ~195 km/h `[RW-Ka25]` `[AW-Ka25]` (media).
- Sonar a immersione **VGS-2 «Oka»**, cavo 125 m:
  - portata ≤ 6 km `[RW-Ka25]` (media);
  - CIA: **attivo ≥ 5,5 km, passivo fino a 2,3 km**, precisione ~100 yd e ~1° `[CIA72]` (media);
  - giudicato inutilizzabile di notte e con maltempo `[AI79]` (bassa-media).
- Carichi alternativi: 36 RGB-NM/NM-1, *oppure* l'arma (AT-1M, poi APR-2; 4 PLAB-250-120 o 8 PLAB-50; la nucleare
  8F59 sul Ka-25PLYu). Inoltre 3 boe radar-faro RLB e 4 marcatori OMAB `[RW-Ka25]` (alta).
- **Coppie ricerca-attacco** (PLS, PLYu): l'elicottero di ricerca marca il punto con una RLB, quello d'attacco vi
  arriva col radar `[RW-Ka25]` (media).
- Ciclo di dip STIMA 13-15 min; velocità di ricerca 35-45 km/h; il Ka-27 «del 30-40% migliore» `[ART-Ka27]`.
- Nel 1985-91 resta su Kresta II, Kara, Moskva/Leningrad e in parte su Kiev e Minsk.

### 3.2 Ka-27PL «Helix-A» (sistema «Osminog», dal 1981)

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Sonar | **VGS-3 «Ros'-V»**: formazione elettronica del fascio, trasmissione omni, ricezione a 5 fasci circolari o settoriali, **5 frequenze** per non disturbarsi nel gruppo (in pratica **una sola autorizzata**); corpo immerso fino a **150 m** | `[ART-Ka27]` | media-alta |
| Tempi di un giro | passivo ≤ 30 s; attivo circolare 7,5-30 s; settoriale 45-110 s | `[ART-Ka27]` | media-alta |
| Modi | ascolto passivo (ŠP), eco (EP), ricerca attiva a passi (AP: il fascio salta di 120° a ogni impulso), inseguimento | `[ART-Ka27]` | media-alta |
| Portate (Mar Nero 1977) | SSK pr.613 a 6-8 kn e 100 m, mare 2: **8 km in passivo, 7 km in eco** | `[ART-Ka27]` | alta |
| Frequenza | **sconosciuta**; STIMA 7-10 kHz | — | bassa |
| Uso dell'attivo | «**si usa raramente**», perché smaschera l'elicottero | `[ART-Ka27]` | alta |
| Difetto noto | il cavo a 16 conduttori non schermati faceva da antenna: oltre 70 m l'operatore sentiva musica e voci | `[ART-Ka27]` | alta |
| MAD | APM-73V rimorchiato a 85 m, ~400 m di portata, «senza successo» al Nord | `[ART-Ka27]` | media-alta |
| Radar | Initsiativa-2KM: RCS 250 m² ≥ 25 km, RCS 2 m² ≥ 5 km (quota 100-500 m) | `[RW-Ka27]` | media |
| Boe | 36 RGB-NM/NM-1; ricevitore «Pakhra» A-100 a 36-40 km da 100 m, 90-115 km da 1000 m | `[ART-Ka27]` | media-alta |
| Carichi | ricerca: 36 boe; attacco: 1 tra AT-1M, VTT-1, **UMGT-1**, **APR-2**, oppure 8 PLAB-250-120 | `[ART-Ka27]` | alta |
| Tempo in zona | **calcolato 2 h 48 min a 100 km, reale 1 h 40 min**; sul Kiev nel 1987 volo di 2 h 50 min con **1 h 30 min di ricerca** e 18 boe | `[ART-Ka27]` | alta |
| Gruppi | il sistema ne gestisce 6, ma l'orientamento reciproco funziona **solo fra 3**; trasferimenti con errore di 250-300 m; il Doppler non funziona con mare < 2 | `[ART-Ka27]` | alta |
| **Notte** | **sonar a immersione solo di giorno; di notte solo boe** | `[ART-Ka27]` | alta |
| Inseguimento | Mediterraneo 1987: **11 h** su due sottomarini stranieri col VGS-3 in eco; sopra i 15 kn inseguimento sulla scia | `[ART-Ka27]` | alta |
| Navi | Udaloy 2, Kirov 3, Slava e Sovremennyj 1, Kiev e Minsk ~16 (Ka-25 e Ka-27) | `[RW-Ka27]` | media |

### 3.3 Be-12 / Be-12N «Mail» — anfibio della zona vicina

- Crociera 450 km/h; **raggio tattico 500 km con 3 h in zona**; fino a **80 boe** `[ART-Be12]` (alta).
- **Ricerca col MAD a 70-100 m e 300-320 km/h**, a mano, per 45 min, poi 15 min a 200-300 m. Fino al **30% delle
  scoperte (1968-82) viene dal MAD**, con classificazione quasi nulla `[ART-Be12]` (alta).
- Posa delle boe da **500 m** dopo un controllo radar da 2000 m. 60-70 boe a scacchiera coprono **5000 km² con
  p = 0,5-0,6**; il solo MAD su 2500 km² dà **p = 0,01-0,02** `[ART-Be12]` (alta).
- Be-12N «Nartsiss-12» (1976): RGB-2, APM-73S, data link con navi ed elicotteri; classificazione con 3 boe omni a
  triangolo `[ART-Be12N]` (alta).
- Ritmo nel Mediterraneo (1968): 4 sortite a settimana da 3-4 h, **sempre in coppia**, 400 boe al mese. Nel 1989,
  29 scoperte su tutte le flotte `[ART-Be12]` (alta).

### 3.4 Il-38 «May» (sistema «Berkut-38»)

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Requisito | raggio **2200 km con ≥ 3 h in zona** a 500-2000 m; velocità minima 330-350 km/h; crociera 456 km/h | `[ART-Il38]` | alta |
| Missioni reali 1969-81 | 4095 sortite in 24 570 h (**~6 h di media**), **172 scoperte** di sottomarini stranieri | `[ART-Il38]` | alta |
| MAD | **100 m di giorno, 200 m di notte**, a mano senza autopilota; APM-60 o APM-73S (~400 m) | `[ART-Il38]` `[ART-Be12N]` | alta |
| Radar «Berkut» (Wet Eye) | stabilizzato; giro circolare o settori di 60°/150°; modo «microplan» 20 × 20 km; giudicato «arcaico»; CIA: **25 nm contro alberi esposti** in condizioni ideali | `[ART-Il38]` `[CIA72]` | alta / media |
| Boe | **24 canali** radio (P-3C: 99) | `[ART-Il38]` | alta |
| Carico tipico | **144 RGB-1 + 10 RGB-2 + 10 bombe + 2 siluri** (AT-2 o APR-2) | `[ART-Il38]` | alta |
| Potenziamento «Izumrud» | ricevitore a 68 canali e boa a banda larga RGB-16 (prove concluse nel 1984), solo su una parte degli Il-38; STIMA: metà flotta nel 1989 | `[ART-Il38]` | media / bassa |
| Rendimento | il 33° Centro valuta la sua idoneità alla ricerca **8-10 volte peggiore** di quella del P-3C; fino al 30% di RGB-1 guaste a terra; MTBF del sistema ≤ 6 h | `[ART-Il38]` `[RU-IL38]` | media |
| Basi | 24° reggimento a Severomorsk-1 (Nord), 77° a Nikolaevka (Pacifico); distaccamenti in Yemen, Etiopia, Libia, Siria | `[RW-Il38]` | alta |

### 3.5 Tu-142M/MK «Bear-F mod 3» (sistema «Korshun-K»)

| Voce | Dato | Fonte | Conf. |
|---|---|---|---|
| Velocità | crociera **M 0,68 ≈ 735 km/h** da 7750 m; pattugliamento 450 km/h | `[ART-Tu142]` | media |
| Autonomia | consumo ~6000 kg/h, 91 t di carburante: STIMA **14-15 h** | `[ART-Tu142]` | alta / STIMA |
| Distanze | zone fino a **4200 km** dal 1981 | `[ART-Tu142]` | alta |
| Quote | autopilota non sotto **500 m**: boe e ricerca sopra i 500 m, corse MAD a mano | `[ART-Tu142]` | alta |
| Carico «ricerca e attacco» | **66 RGB-75, 44 RGB-15, 10 RGB-25, 10 RGB-55A, 3 siluri**, oppure fino a 240 esplosive MGAB | `[ART-Tu142]` | alta |
| Armi | AT-1/1M/2/2M, APR-1, bombe di profondità tra cui la nucleare **5F48 «Skalp»** | `[ART-Tu142]` | alta |
| MAD | MMS-106 «Ladoga» in cima alla deriva, sensibile alla carica statica; segnale integrato su 10 s; per una corsa vanno spenti 4 generatori, l'autopilota, la **luce lampeggiante**, la torretta di coda. STIMA 250-400 m | `[ART-Tu142]` | alta |
| Zona tattica | 1200 × 1200 km; il quadro passa fra gli aerei col data link BSS | `[ART-Tu142]` | alta |
| Rendimento 1983 | **3 sortite per scoperta**, 4 RGB-75 per ora d'inseguimento | `[ART-Tu142]` | alta |
| Verdetto del 1987 | esercitazione del 1985: **falsi allarmi fino al 50%**. Decisione VPK n. 574-87 del 9-12-1987: **RGB-75 e RGB-25 inefficienti con mare 3-5**; RGB-15 con esplosive e RGB-55A **non rilevano nel Barents**; flotta da rimodernare entro il 1989; voli da Cuba sospesi | `[ART-Tu142]` | alta |
| Basi | **Kipelovo** (35ª divisione, 49 aerei, Atlantico nord e Mar di Norvegia); Kamennyj Ruchej (24 aerei, Pacifico); 4 a Cam Ranh | `[RW-Tu142]` | alta |
| Inseguimenti noti | Barents 1974: 4 Tu-142, contatto per 2 h 55 min. Mar delle Filippine 1977: 5 Tu-142 contro un SSN USA, 4 h 05 min | `[EN-Tu142]` (Friedman) | media |

---

## 4. Sonoboe, sorgenti e marcatori: i dati che servono al gioco

### 4.1 Boe USA nel 1989

| Boa | Funzione | Dati per il gioco | Fonte | Conf. |
|---|---|---|---|---|
| SSQ-41B | passiva omni (LOFAR) | 10 Hz-10/20 kHz; 31 canali a 162-173 MHz, 1 W; vita 1/3/8 h; solo «contatto nel cerchio» | `[FASSSQ41]` `[HOLLER]` | media-alta |
| SSQ-53B | passiva direzionale (DIFAR) | in produzione dal 1969; quote 90/1000 ft (STIMA per la B; la D ha 4 quote); vita 1/3/8 h; **σ del rilevamento 5-9°** misurato in mare (nominale ±10°) | `[HOLLER]` `[AAD-DIFAR]` | alta |
| SSQ-77A VLAD | DIFAR a schiera verticale | dal 1981; **+10-15 dB** contro il rumore di traffico orizzontale; 500/1000 ft; vita 1/4/8 h | `[HOLLER]` `[FASSSQ77]` | alta |
| SSQ-57A | passiva tarata | misura del rumore ambiente; 90/400 ft | `[HOLLER]` | alta |
| SSQ-47B | attiva, solo distanza | **ping CW ogni 10 s** a 13-19 kHz, 60/800 ft, ~30 min | `[HOLLER]` | alta |
| SSQ-50 CASS | attiva a comando, solo distanza | 60 ft, poi 1500 ft; 4 frequenze 6,5-9,5 kHz; ultimo ordine 1982 | `[HOLLER]` | alta |
| SSQ-62B DICASS | attiva direzionale a comando | **6,5 / 7,5 / 8,5 / 9,5 kHz**, CW o FM; 90 ft, poi 400 o 1500 ft (il cambio va solo verso il basso); **100 ping-secondi**; distanza, rilevamento, Doppler; «fatta per dare i criteri d'attacco **anche con una sola boa**» | `[FASSSQ62]` `[U62G]` | alta |
| SSQ-36 | batitermografo | profilo fino a 1000-2625 ft: dà la SVP all'aereo | `[FASSSQ36]` | media |

**Fisica comune** `[U53D]` `[U53G]` (alta, schede moderne; le boe del 1989 avevano limiti simili):
- lancio da 40 a 30 000 ft e fino a 370 kt;
- discesa col paracadute **~30 m/s**: da 1000 ft circa 10 s, da 10 000 ft circa 100 s;
- **stabilizzazione dell'idrofono in 100 s a 30 m, 125 s a 60 m, 160 s a 120 m, 240 s a 300 m**;
- autoaffondamento a fine vita (8 h al massimo);
- VHF da 1 W, con portata pratica verso l'aereo di 20-60 nm (STIMA; orizzonte radio 41 nm a 1000 ft, 125 nm a
  10 000 ft);
- errore di posa 100-300 yd dalla bassa quota, 1100-2100 yd da 30 000 ft `[DTIC-106692]` (media);
- deriva STIMA 0,5-2 kn, cioè +0,1-0,3 nm d'errore di posizione all'ora (senza GPS nel 1989).

### 4.2 Boe sovietiche nel 1989

| Boa | Piattaforma | Dati | Fonte | Conf. |
|---|---|---|---|---|
| RGB-NM «Chinara», RGB-NM-1 «Zheton» | Be-12, Ka-25, Ka-27 | passive omni **con soglia** (trasmettono solo se il segnale la supera; c'è anche un modo marcatore continuo); 18 canali a 49,2-53,4 MHz; idrofono a 20 m (Zheton: 20/40/100 m scelti a terra); discesa ~30 m/s, operative 1-2 min dopo l'ammaraggio; attesa ≤ 6 h; banda **7-10 kHz**, ascolto **a orecchio**; portata **0,2-5 km** (CIA: ~2,7 km contro un bersaglio rumoroso); **inutili oltre mare 3**; ignorano picchi sotto 10-15 s; 28% guaste nel 1966, poi affidabilità 0,7-0,8 | `[ART-Be12]` `[CIA72]` | alta |
| RGB-1 | Il-38 (Tu-142 base) | passiva omni, schiera di 5 idrofoni a 35 (70) m; discesa 34 m/s; poi in attesa con autoavvio; transponder radar per la localizzazione; prove 1965: SSK a 6-8 kn, mare 1-2, **1,7-4 km** | `[ART-Il38]` `[RW-Il38]` | alta / media |
| RGB-2 | Il-38, Be-12N | passiva **direzionale**, antenna a 13 elementi che ruota a 8 giri/min, a 20 m; 40-45 min; **1,5-2 km** contro SSK silenziosi; errore 5-7° | `[ART-Il38]` `[ART-Be12N]` | alta |
| RGB-3 | Il-38 | attivo-passiva, 5 min, 2 km; **non usata in pratica** | `[ART-Il38]` | alta |
| RGB-75 | Tu-142M | passiva **infrasonora 2-60 Hz**, continua fino a 5 h, set di 24; portata reale **≤ 1 km a bassa velocità, 15-20 km a 12 kn**; inefficiente con mare 3-5 | `[ART-Tu142]` | alta |
| RGB-15 | Tu-142M | passiva 2-5000 Hz, 2 h; 0,5-1 km da sola, **5-10 km come ricevitore dell'eco esplosiva** | `[ART-Tu142]` | alta |
| RGB-25 | Tu-142M | passiva direzionale, idrofono fino a 150 m, set di 10 | `[ART-Tu142]` | alta |
| RGB-55A | Tu-142M | **attiva** omni comandata, 4 frequenze, fino a 1 h; ~5 km, errore di distanza 75-100 m, Doppler | `[ART-Tu142]` | alta |
| MGAB (OZ, LZ, SZ) | Tu-142M | sorgenti esplosive da 100-800 g che detonano a **25, 150 o 400 m**; la spirale SZ dà un **treno di colpi a 4 Hz**; fino a 240 per aereo | `[ART-Tu142]` | alta |
| RLB «Poplavok-1A» | Ka-25 | boa **radar-faro** (non acustica): marca il punto per l'elicottero d'attacco; sgancio ≤ 200 m, ~20 min | `[RW-Ka25]` | alta |

### 4.3 Marcatori e segnalatori

| Oggetto | Dati | Fonte | Conf. |
|---|---|---|---|
| Mk 25 | fiamma gialla e fumo bianco per **10-20 min**; non va lanciato dall'hover | `[AUS-MK25]` | alta |
| Mk 58 | due candele per **40-60 min**, visibile a ≥ 4,5-5 km di giorno e di notte | `[AUS-MK58]` | alta |
| Mk 84 **SUS** | segnalatore acustico **non esplosivo** aereo→sottomarino: toni a **~3 kHz**, 4-5 codici, parte entro 3 s e dura 45-120 s; usato anche per **simulare il lancio di un'arma** in esercitazione | `[PRC68SUS]` | media |
| OMAB-25-12D / -8N | marcatori sovietici: fluoresceina di giorno, fiamma di acetilene di notte | `[RW-Ka25]` `[ART-Il38]` | alta |

---

## 5. Dottrina e missioni

### 5.1 Barriera GIUK e Keflavik

- La NAS **Keflavik** ospita a rotazione uno squadrone VP dell'Atlantico, con base madre Brunswick (Maine). Per
  esempio il VP-11 dal 10-11-1988, con il tempo peggiore da oltre un decennio `[WPVP11]` (media).
- Dalla fine degli anni '70 i Delta passano il GIUK pedinati da **SSN e P-3**, con **6 sortite al giorno da 4 h** in
  zona `[HOLLER]` (alta).
- Caso del 1978 (Polmar): il SOSUS norvegese segnala un SSBN a Capo Nord, il *Batfish* lo pedina per 51 h e 350 nm,
  poi lo perde; un P-3 da Reykjavik riprende un contatto intermittente il giorno dopo `[HARPSOSUS]` (bassa-media).
- Distanze da Keflavik `[CALC]`: varco Groenlandia-Islanda 200-300 nm, varco Islanda-Fær Øer 250-400 nm, Mar di
  Norvegia centrale 600-750 nm. Quindi **6-7 h in zona nei varchi, ~4 h nel Mar di Norvegia**.

### 5.2 Il datum dal SOSUS

- **La catena.** Le NAVFAC elaborano; i centri di valutazione fondono i dati, anche dalla radiogoniometria HF; ne
  esce una **area di probabilità (SPA)** in cui si mandano le forze a riacquisire il contatto. Il ritardo è
  «inevitabile» e l'area cresce col tempo `[HOWARD11]` (alta).
- **Precisione.** Circa **50 nm di raggio** nelle zone migliori `[HARPSOSUS]` (bassa-media). Nei casi didattici
  della tesi SALT l'ellisse è di **60 × 30 nm** `[NPS-SALT90]` (media).
- **Arrivo dell'aereo**, STIMA: allerta 1-2 h, transito a 330 kt (300 nm in circa 1 h), elaborazione 0,5-1 h, quindi
  **2,5-4 h dopo il fix**.
- **Cerchio d'incertezza** (*furthest-on circle*): `r(t) = r0 + u·t`. Con un Victor III a 8-10 kn per 3 h il raggio
  è di 75-80 nm, cioè ~18 000-20 000 nm², che non si copre a tappeto. Si posano **barriere sulla rotta probabile**,
  verso un varco o un bastione `[CALC]`.

| u (kn) | ritardo 10 min | 30 min | 60 min |
|---|---|---|---|
| 5 | 1,8 nm / 11 nm² | 3,5 / 38 | 6 / 113 |
| 10 | 2,7 / 22 | 6 / 113 | 11 / 380 |
| 20 | 4,3 / 59 | 11 / 380 | 21 / 1385 |

(r0 = 1 nm `[CALC]` da `[NPS-FD00]`.)

### 5.3 Scorta al gruppo portaerei USA

- **Ruoli.** L'S-3 è la difesa principale nelle **zone media ed esterna**, mentre SH-3 e LAMPS coprono la **zona
  interna** (Proceedings 1987) `[USNI87Z]` (media). Il LAMPS III deve bonificare la zona fino a **~30 nm davanti** al
  gruppo `[SIKS70B]` (media). La zona esterna è citata a ~90 nm `[ARENDS]` (media).
- **Come la vedevano i sovietici (1985)** `[NSLZONE]` (media):
  - zona vicina fino a 37 km, con le scorte a 7-9 km e gli elicotteri davanti sulla rotta a 18-28 km;
  - zona media fino a 140 km;
  - zona lontana oltre 460 km, con gli SSN *Los Angeles* in «protezione attiva profonda».

| Zona [GIOCO] | Distanza dalla portaerei | Mezzi |
|---|---|---|
| Interna | 0-15/20 nm | 2 SH-3H col sonar a immersione sull'asse (10-15 nm davanti) e sui fianchi; SQS-53 delle scorte vicine |
| Media | 20-60/90 nm | fregate con SQR-18/19 sui settori di minaccia, ciascuna col suo LAMPS; S-3 su campi di boe |
| Esterna | 90-250 nm | S-3 su cue; P-3 da terra; SSN in supporto diretto davanti al gruppo |

Ciclo del ponte STIMA 1 h 30. S-3 in sortite di 1-2 cicli; SH-3H in doppio ciclo (3 h), con 2 elicotteri in zona
interna e rotazione su 6 macchine.

### 5.4 Coordinazione

- **Link 11** fra S-3, E-2, navi e P-3; ogni boa posata va sul link `[TWZS3]` (media). Il LAMPS passa i dati alla
  sua nave via Hawk Link, e la nave li ritrasmette in Link 11 (STIMA).
- «**Pouncer**»: il termine non risulta nelle fonti aperte. Il concetto sì: la nave col TACTAS rileva in zona di
  convergenza e manda l'elicottero armato a «balzare» sul contatto `[NAVSRC]` (media).
- **SSN in supporto diretto.** Vale la gestione dello spazio acqueo, con zone vietate all'attacco aereo attorno al
  SSN amico (principio noto, numeri non trovati). **[GIOCO]** Il 688 in supporto diretto ha una «moving haven» di
  10-20 nm in cui gli aerei amici non attaccano; fuori rischia il fuoco amico. Si collega alle zone R2 della radio
  (dossier 10).
- **Lato sovietico** `[ART-Be12]` (alta):
  - le navi «coprono col loro rumore» le boe e fanno perdere il contatto;
  - Okean-70: i Be-12 rilevano su una barriera e **in 12 min passano il contatto ai Ka-25**;
  - su 74 scoperte del Be-12: 26 inseguimenti interrotti per ordine, 18 contatti persi, 18 interruzioni per meteo,
    carburante o boe finite, **12 contatti passati con successo** al gruppo navale (KPUG).

### 5.5 Criteri d'attacco e regole d'ingaggio

- **USA.** Classificazione **POSSUB → PROBSUB → CERTSUB** (prassi NATO nota, n.v.); «flaming datum» se il
  sottomarino ha lanciato.
  - Criterio d'attacco tipico (STIMA): fix entro **500-1000 yd** (DICASS o incroci DIFAR stretti) più una conferma
    indipendente (**MAD** o una seconda attiva), in guerra e fuori dalle zone amiche. La DICASS dà i criteri anche
    da sola `[FASSSQ62]` (alta).
  - In pace niente attacco: si pedina, si classifica e si registra; al massimo un SUS in esercitazione.
- **URSS, regolamento del 1983** `[CIA-REG86]` (alta):
  - all'inizio delle ostilità si attacca subito con una salva da distruzione sicura;
  - **se non si distinguono più contatti, si colpiscono tutti** (§347);
  - se l'elicottero rileva oltre la portata delle armi della nave, attacca per primo e la nave segue con missili ASW
    o siluri sulla sua designazione (§358);
  - con l'ordine, i nucleari si usano per primi (§359).
- **Le tre situazioni del Tu-142M** `[ART-Tu142]` (alta):
  1. **pace:** sole boe, niente armi, inseguimento «discreto» il più a lungo possibile per studiare la firma;
  2. **minaccia:** transito basso e ricerca degli SSBN; contatti abbastanza frequenti da colpire all'ordine;
  3. **guerra:** si attacca ogni sottomarino senza accertarne la nazionalità.

### 5.6 Dottrina sovietica: zone e bastioni

- **Forme d'azione** `[CIA-REG86]` (alta): «in area», «su linea» (barriera), «su direttrice», «**a chiamata**» (si
  manda una forza dove un'altra ha un possibile contatto). L'aviazione «spazza simultaneamente» le aree assegnate per
  chiarire la situazione su vaste aree. Il comandante del gruppo navale prende il controllo di aerei ed elicotteri
  quando passano una linea stabilita.
- **Operazione contro gli SSBN** (Saakjan, *Military Thought* 1975) `[CIA-MT75]` (alta):
  - ricerca massiccia con SSN, SSK, aerei e gruppi con elicotteri, più sensori fissi e campi minati;
  - navi in mezzo fra le aree degli SSN e degli aerei, perché l'SSBN che le evita **acceleri** verso chi cerca in
    silenzio;
  - gruppi «a morsa» da direzioni diverse; barriere negli stretti.
- **Zone e raggi.** La zona vicina è di Be-12, Mi-14 e Ka-27; quella lontana di Il-38 e Tu-142. Raggi reali: Be-12
  ≤ 500 km, Il-38 2200-2500 km, Tu-142M fino a 4200 km e all'Atlantico centrale da Cuba `[ART-*]` (alta).
- **Bastioni.** Il termine non compare nelle fonti sovietiche aperte, che parlano di difesa delle zone di pattuglia
  delle proprie SSBN. Nel **Barents** operano Il-38 da Severomorsk, Be-12 da Safonovo e Ka-27; lì le boe attive del
  Korshun non funzionavano (VPK 1987). Nel **Mar di Norvegia** Il-38 e Tu-142M lavorano contro gli SSN NATO in
  avvicinamento. Nell'**Okhotsk** ci sono Il-38, Be-12 e Tu-142 `[ART-*]` (alta).

---

## 6. Procedure passo per passo

### 6.1 Schemi di boe USA

Spaziatura (STIMA, media; logica della portata al 50%): **S ≈ 1,5-2 × R50**, linee sfalsate per i campi.

| Schema | Geometria | Quando | Esempio (STIMA) |
|---|---|---|---|
| **Barriera lineare** | n boe in linea perpendicolare alla rotta presunta, L = (n−1)·S | varchi, bastioni, cue vecchio | Victor I/II a 10 kn (R50 4-6 nm): S = 8 nm, 6 boe per 40 nm |
| **Barriera stretta** | S ≈ R50 | per classificare chi passa | Kilo (R50 1-2 nm): S = 1,5 nm |
| **Chevron (V)** | due bracci a 30-45°, vertice verso il sottomarino | rotta nota, velocità incerta | 3 + 3 boe, bracci di 10-15 nm |
| **Cerchio / campo** | anello di 6-8 DIFAR attorno al datum, r ≈ 0,7·(r0 + u·t), più una boa centrale | datum fresco senza rotta | datum di 20 min, 10 kn: r ≈ 3-4 nm, 8 boe |
| **Cuneo** | settore di 60-90° aperto verso la fuga stimata | dopo un rilevamento DIFAR singolo | 5-7 boe su archi a 1/2 e 1 portata |
| **Trappola** | boe **davanti** alla posizione stimata | bersaglio più veloce del ritmo di posa | 1-2 portate davanti alla rotta |
| **Box / rombo DICASS** | 3-4 DICASS ai vertici attorno al fix DIFAR, lato 1-2 kyd | localizzazione finale | ping a comando, una boa per volta |
| **Cloverleaf** (storico) | boa centrale e 4 a 2 nm, posate volando a quadrifoglio in ~13 min | datum puntiforme | `[HOLLER]` (alta, seconda guerra mondiale) |

### 6.2 Dalla scoperta all'attacco: P-3C

[GIOCO] con dati e STIME:
1. **Cue** (SOSUS, SURTASS, altro aereo). Transito a 25 000 ft e 330 kt; all'arrivo BT e SSQ-57 per il profilo e il
   rumore ambiente.
2. **Rilevazione.** Barriera o campo DIFAR/LOFAR posato a 1500-5000 ft e 200-250 kt. Ogni boa è attiva in 2-4 min
   (§4.1). Ascolto per 30-90 min.
3. **Localizzazione.** Incroci di 2-3 DIFAR (σ 5-9°): area d'incertezza di 1-3 nm, poi boe di rinforzo più strette.
   Esempio: due boe a 3 nm che si incrociano a 60° con σ = 7° danno un'ellisse di ~0,45 × 0,35 nm `[CALC]`.
4. **Tracciamento.** Due o tre DIFAR davanti alla traccia ogni 20-30 min, mentre il sottomarino si muove di 3-5 nm.
5. **Attacco.**
   - Discesa a **300-1000 ft**, che espone all'ESM e alla vista `[HARPHAAWC]` (media).
   - DICASS per distanza e rilevamento.
   - Passaggio **MAD** sulla rotta stimata, con un fumogeno al «MAD mark».
   - Sgancio del Mk 46 300-500 yd davanti al sottomarino (STIMA) e marcatore Mk 25/58.
6. **Se manca.** Il Mk 46 cerca per 6-8 min; il P-3 torna in ascolto; un secondo siluro dopo 5-10 min, solo con un
   fix nuovo (STIMA).

**Tempi totali (STIMA)** `[NPS-*]` `[COTE]`: dalla prima scoperta al lancio **15-40 min**. Una rottura di contatto
oltre 10 min fa ricominciare la catena. Negli anni '60 l'anello debole era la **localizzazione**; il DIFAR la
risolse `[COTE]` (alta).

### 6.3 MAD

**Fisica e modello.** Il modello OPTEVFOR della US Navy, nella tesi NPS di Schluckebier del 1984 `[NPS-MAD84]` (alta
per il modello, media per i numeri), è `R = (c·M / (ORF·N))^(1/3)`:
- la portata scala con la **radice cubica del dislocamento**;
- caso base: sottomarino da 4000 t, separazione verticale 200 m, rumore σ 0,1 γ: **376 m obliqui** per P = 0,5;
- con **500 m di separazione** la probabilità è «solo lieve» anche sulla verticale;
- la curva stocastica è una normale con P = 0,5 a R e σ = 0,2·R.

La portata pratica dell'ASQ-81 è ~500 m obliqui `[FNWS9]` (media). Cote: la portata del MAD era «spesso inferiore» al
volume di ricerca del Mk 46, e il MAD serviva a dare il via al lancio, non a cercare `[COTE]` (alta).

**[GIOCO] Portata obliqua al 50%** (OPTEVFOR, ORF 3, σ 0,1 γ) `[NPS-MAD84]` + STIMA:

| Battello | Dislocamento in immersione | R50 obliqua |
|---|---|---|
| Kilo, Foxtrot | 2-3,2 kt | 300-350 m |
| **688** | 6,9 kt | **≈ 450 m** |
| Victor III | 7 kt | ≈ 455 m |
| Akula | 13 kt | ≈ 555 m |
| Typhoon | 25 kt | ≈ 690 m |
| **Alfa** (titanio) | — | **≈ 0,6-0,7 × R** (STIMA: restano macchinari e reattore) |

- **Larghezza laterale:** `x_max = √(R² − Δz²)`, con Δz = quota dell'aereo + profondità del battello. Un P-3 a 300 ft
  contro un 688 a 400 ft dà 400 m; contro un 688 a 1000 ft dà 210 m. **Andare profondi dimezza la sweep width.**
- **Rumore magnetico:** con mare agitato e alle alte latitudini σ 0,2-0,5 γ, cioè portata ×0,6-0,3 (STIMA).
- **Sovietici:**
  - APM-73: ~400 m; Il-38 a 100 m di giorno e 200 m di notte; Be-12 a 70-100 m;
  - Tu-142M (MMS-106) 250-400 m (STIMA);
  - CIA 1972: 460-550 m per il nuovo MAD.
  - Il MAD è usato anche per la **ricerca iniziale**, non solo per l'ultimo tratto `[ART-Be12]` (alta).

**Schemi di passaggio** (USA; [GIOCO] con termini documentati):
- **sul fix:** rotta sull'incrocio dei rilevamenti; al contatto «MAD! MAD! MAD!» e un fumogeno;
- **cloverleaf:** dopo il primo contatto, virate a quadrifoglio con rotte diverse di ~90°, finché 3-4 contatti danno
  direzione e velocità (STIMA: 2-3 min per petalo a 200 kt);
- **cerchio di trappola** di raggio u × tempo trascorso attorno all'ultimo contatto (termine documentato dal 1945
  `[ASWCONVOY]`);
- **trappola sulla rotta:** linee trasversali davanti alla posizione stimata, distanziate di 2 × x_max.

### 6.4 Sonar a immersione: SH-3H

Dati `[RUDZIS]` `[MILAQS13B]` (alta), salvo STIMA:
1. Transito a 150-500 ft, 100-114 kt.
2. Dall'accoppiatore (500 ft, 70 KIAS) all'**hover a 40 ft**: 60-90 s (STIMA).
3. Calata del trasduttore sopra o sotto lo strato: 30-90 s (STIMA); il verricello rallenta a 35 ft in risalita.
4. Ascolto passivo 1-3 min, poi **3-6 ping** sulla scala da 5-8 kyd (un ping ogni 6-10 s, STIMA dal tempo d'andata e
   ritorno).
5. Recupero: 60-90 s (STIMA).
6. Ripartenza a 150 ft e 70 KIAS; **salto di 5-10 nm** in 3-6 min.

Ciclo totale: **4-6 min in inseguimento, ~20 min in ricerca** (dip di 15 min più il salto).

Portate (STIMA, media): attivo in alta potenza **3-5 kyd** contro un SSN medio, fino a 8-12 kyd con buona
propagazione sotto lo strato; passivo debole (banda stretta a 10 kHz), solo contro la cavitazione vicina.

Modello NPS (2016-18) `[NPS-ASW16]` `[NPS-DIP18]` (alta per il modello):
- portata 3 nm e 5 min per dip;
- il **ritardo d'arrivo** al datum domina, e il numero di dip necessari cresce linearmente con lui;
- raddoppiare la velocità del sottomarino da 8 a 16 kn **più che quadruplica** i dip;
- il pattern migliore è una **spirale di dip disgiunti**.

### 6.5 Sonar a immersione: Ka-27PL

Dati `[ART-Ka27]` `[TI]` (media-alta), salvo STIMA:
1. Arrivo automatico sul punto assegnato dal sistema o dal capogruppo, con errore di 250-300 m.
2. **Hover a ~30 m**; l'automatismo tiene quota e posizione sulla lunghezza e sull'angolo del cavo.
3. Calata del corpo fino a **150 m**, scegliendo la quota sull'idrologia.
4. **Ascolto passivo** (un giro ≤ 30 s, ripetuto) per **3-10 min**; l'attivo solo con un contatto o un sospetto.
5. Recupero e trasferimento al punto successivo a **4-6 km**.
6. **Ciclo completo 10-12 min**; ricerca a 50-60 km/h; elicotteri in linea a **3-3,5 km**.
7. **Sul contatto:** due misure d'eco danno posizione, rotta e velocità, e quindi il punto successivo. Con 3 o più
   elicotteri i punti li assegna il capogruppo.
8. **Attacco:** modo «Ataka», di solito da un altro elicottero già armato; «attacca chi è in posizione migliore»
   `[AW-Ka25]` (media).

Per il Ka-25 il ciclo è più lento di ~30% (13-15 min, STIMA). **Di notte niente dip**, solo boe.

**Differenze dagli USA:**
- il passivo prima di tutto;
- mai di notte;
- gruppi di 4-6 elicotteri dal Kiev, guidati da un capogruppo in volo;
- l'SH-60B, invece, è un camion di boe al servizio della nave.

### 6.6 Procedure sovietiche degli aerei

**Il-38 «Berkut»: dalla barriera all'attacco** `[ART-Il38]` (alta)
1. Controllo radar della superficie.
2. **Barriera lineare di RGB-1**, con una boa marcatore all'inizio.
3. Sorveglianza della barriera, anche automatica.
4. Quando una boa reagisce, il calcolatore stima posizione, rotta e velocità e **posa da solo una barriera di
   precisione di RGB-2**.
5. **Sottomarino visto al radar che s'immerge:** barriera circolare di RGB-1 **a spirale logaritmica**, con i
   parametri dalla distanza e dal rapporto delle velocità.
6. Serie di RGB-2 centrata sulla RGB-1 che reagisce.
7. Negli anni '70 si usava anche il campo a scacchiera, con lati paralleli a non meno di 20 km.

Differenza dagli USA: niente LOFAR a lunga distanza. Il contatto si prende «a soglia» a 1-4 km e lo si **rincorre con
barriere**.

**Be-12: inseguimento a barriere** `[ART-Be12]` (alta)
1. Boa marcatore sul punto di scoperta, con attorno una barriera circolare.
2. La boa della barriera che reagisce dà rotta e velocità.
3. **Barriere d'intercettazione davanti alla rotta**, su un settore di 90-180°, a raggio minimo di 3000 m.
4. **In media un contatto ogni 26 min** contro un sottomarino a 6-10 kn, cioè una barriera ogni 6-10 km: con la
   barriera a 6 km e un settore di 180° servono ≥ 10 boe a 2 km.
5. Un contatto MAD fa tornare l'aereo sul punto.
6. Siluro sulla seconda boa che reagisce, o meglio sulla prima.
7. Precisione del lancio: col radar da 2000 m l'errore è 120 m (a volte 300-350); sui dati delle boe omni supera i
   2000 m, perciò si lanciava direttamente sulla boa che reagiva.

**Tu-142M «Korshun»: campo, eco esplosiva** `[ART-Tu142]` (alta)
1. **Campo:**
   - set di RGB-75 a 10-15 km (15-20 in servizio; 30-40 nei Sargassi), poi RGB-15, da rimpiazzare dopo 2 h;
   - compiti automatici a lati paralleli e a «scatolina», un quadrato a spirale.
2. **Reazione di una RGB-75:** in pratica 9-12 RGB-1 in barriera circolare.
3. **Inseguimento:** anello chiuso («okhvat»), poi arco o barriera sul settore di rotte possibili.
4. **Eco esplosiva:**
   - con fondale > 1000 m l'esplosiva cade sopra la boa al secondo passaggio;
   - con fondale < 1000 m detona a 2-3 km dalla boa, e la linea di posizione è un'**ellisse** con fuochi nella boa e
     nell'esplosiva;
   - servono 2-3 colpi nello stesso punto, a **3-4 min** l'uno dall'altro;
   - per Artem'ev le esplosive «tolgono alla ricerca ogni segretezza».
5. **Attacco** «su bersaglio univoco», o «su bersaglio a due soluzioni» in una sola passata.

**Quote e velocità di posa** `[ART-*]` (alta/media):

| Velivolo | Quota di posa | Velocità | Spaziatura |
|---|---|---|---|
| Be-12 | 500 m | ~350-400 km/h (STIMA) | barriera a 2 km |
| Il-38 | 500-2000 m | 350-400 km/h | campo con lati ≥ 20 km |
| Tu-142M | ≥ 500 m | ~450 km/h | campo a 10-40 km |

### 6.7 L'attacco

| Arma | Lancio | Fonte | Conf. |
|---|---|---|---|
| **Mk 46 Mod 5** (USA) | P-3: discesa a una quota di sgancio di **300-1000 ft**, normalmente sotto i 500. Elicottero (dato canadese): **aviolancio a 150 ft e 100 kt**, oppure dall'**hover a 40 ft** col sonar in acqua; l'SH-3 può lanciare in «hover launch» sul proprio contatto. Paracadute Mk 28 (ala fissa) o Mk 31 (elicottero). Acquisizione **1600 yd**, ~45 kn, ricerca a serpentina o circolare, **6-8 min** di corsa, oltre 1200 ft | `[HARPHAAWC]` `[JPROC]` `[USNI81]` `[VP4MK46]` `[FASMK46]` | media |
| **AT-1/AT-1M** (URSS) | quota **400-2000 m** fino a 600 km/h, fondale ≥ 60 m; paracadute stabilizzatore poi principale a 500 m; **cerchio a sinistra di 60-70 m a 12°/s**; acquisizione ~500 m; **autodistruzione a 9 min**; **Pk 0,15-0,18** alle prove di stato | `[ART-Be12]` `[RW-Ka25]` | alta |
| **AT-2/AT-2M** | 534 mm, 1030 kg; autoguida a 600-1000 m; paracadute multiplo | `[ART-Il38]` | alta |
| **UMGT-1 «Orlan»** (1981) | 400 mm, **41 kn × 8 km**, fino a 550 m; impatto in acqua a **60-65 m/s**; scende a 40-50 m e gira a sinistra a 7°/s | `[ART-Be12]` `[RW-UMGT1]` | alta / media |
| **APR-2 «Yastreb-M»** (1976) | razzo-siluro: plana in spirale a −17° fino a 150 m in passivo, poi motore e attivo; 43-60 kn; bersagli a 15-600 m; **Pk 0,7-0,85** con errore di designazione di 300-500 m; fondale ≥ 80 m | `[RW-APR2]` | media |
| **APR-1 «Kondor»** (1971) | Pk con 2 missili 0,3-0,5; Il-38 e Tu-142 | `[RW-APR1]` | media |
| **PLAB-50, PLAB-250-120** | bombe di profondità «a bassissima efficacia»; Ka-27 8 × 250 | `[ART-Be12]` | alta |
| **Nucleari** | B57 (USA, P-3, S-3, SH-3); 5F48 «Skalp» (Be-12, Il-38, Tu-142); 8F59 sul Ka-25PLYu, raggio letale 1,7-3 km | `[WPS3]` `[RW-Ka25]` | media |

**Dagli elicotteri sovietici**, STIMA: lancio in volo livellato a 100-300 m e 100-150 km/h dopo il recupero del sonar.
Il limite di 400 m dell'AT-1 vale per gli aerei.

**Efficacia per il gioco** `[COTE]` + STIMA (media):
- volume letale del Mk 46: un cilindro di «diverse migliaia di yarde» per ~1500 ft;
- **P_uccisione** di un Mk 46: **0,2-0,4** contro un SSN allertato con esche, **0,5-0,7** contro uno non allertato,
  **≤ 0,1** contro un battello fermo sul fondo in acque basse (Falkland).

**STIMA della taratura A6.1 (25/10/2026)**, ognuna con la sua fonte:

| STIMA | Valore | Fonte |
|---|---|---|
| Affidabilità del Mk 46 (testa e spoletta) | 0,8 | proprietà dell'arma; tiene la Pk fra 0,5 e 0,7 contro l'SSN non allertato (sopra, `[COTE]`) |
| Affidabilità dell'AT-1 | 0,3 | Pk 0,15-0,18 delle prove di stato `[ART-Be12]`, misurata sul banco del Barents |
| Errore di sgancio | l'errore di posa dell'equipaggio (§7.4, Rayleigh), la metà dall'hover a 40 ft | aviolancio a 150 ft e 100 kt o dall'hover (sopra, `[HARPHAAWC]`) |
| Quota di ricerca impostata | la quota vera più un errore con σ 60 m | l'aereo non misura la quota: batitermografo e ping senza quota |
| Fondale minimo per l'aviolancio | AT-1 60 m, APR-2 80 m (sopra); Mk 46 40 m | Mk 46: STIMA sul Mod 5 per acque basse; in acqua più bassa il siluro tocca il fondo nel tuffo |
| Bersaglio quasi fermo entro 10 m dal fondo | la testa attiva lo vede a 1/3 della portata e lo scarta come fondo 7 volte su 10 | Pk ≤ 0,1 sul fondo (sopra) |
| Noisemaker nel fascio della testa attiva, entro 1000 m | verifica riavviata, aggancio perso con 0,25/s | effetto «medio» del dossier 05 §5.3 `[TWZ]` |
| Esche dell'IA | 4 per battello, una ogni 45 s, 10 min di vita | come i Mk 2 ADC (dossier 05 §5.1) |
| Esche semoventi dell'IA (MG-74) | i nucleari sovietici col siluro in coda: 2 per battello, una ogni 120 s, sulla rotta che lasciano; 10 kn, fino a 250 m, 20 min, eco 15 dB, rumore del lanciatore, virata alternata di 40° ogni 240 s | dossier 05 §5.2 e §7 (2-4 al posto dei siluri, 8-12 kn, 15-30 min: STIMA) |
| Ritmo di posa del Be-12 nella ricerca d'area | una boa ogni 120 s al più, a mano dal vano; le boe sul contatto subito | la coppia (§3.3) nel campo di boe del §9: scoperta 0,60 e Pk dell'AT-1 0,17 su 200 semi, sul banco del Barents (A6.5p) |
| Carico di boe del Be-12 | 32 RGB-NM per aereo con l'AT-1, 40 in sola ricerca | il campo di 60-70 boe del §9 lo posa la coppia (§3.3): con 60 per aereo la coppia ne posava 113 (taratura del 27/10/2026) |
| MAD del Be-12 | fattore 0,7 sulla portata (prima 0,9) | il solo MAD vale 0,01-0,02 su 2500 km² (§9, fonte alta): a 0,9 due aerei a 85 m spazzavano un quinto dell'area anche senza boe e la coppia arrivava a 0,74; a 0,5 la conferma per l'attacco spariva (lanci 0,05). A 0,7: coppia 0,63, AT-1 0,22 a lancio, aereo solo 0,35, su 100 semi (A6.5p e A6.5, 27/10/2026) |

Esito in acqua profonda (`a6_1p`, 200 semi, lo stesso binario): contro il Victor che evade, Pk del Mk 46 **0,33** con
l'MG-74 e 0,53 senza; affondato 0,10 contro 0,18. Con la spoletta misurata dallo scafo (a contatto, 1 m:
`spoletta_scafo_m` di siluri.toml) la Pk resta 0,35 e affondato 0,08. L'esca semovente si muove, quindi la verifica Doppler della testa non
la smaschera come fa con l'ADC Mk 2 ferma; resta solo il rifiuto dell'eco puntiforme. Rientra così nello 0,2-0,4 contro
l'SSN allertato con esche. La MOSS Mk 70 c'è nei dati e nell'API della missione (`lancia_esca_mobile`) come what-if del
giocatore: sul 688 del 1989 non c'era (dossier 05 §5.1).

### 6.8 Durata in zona e cambio dei turni

| Mezzo | TOS(d) [GIOCO] | Cambio |
|---|---|---|
| P-3C | 8,1 − d/165 h (d in nm) | il rilevo arriva prima della partenza; 6 sortite al giorno per una stazione continua |
| S-3A/B | ≈ 5 − d/170 h (STIMA) | un secondo Viking 2-3 h dopo riposa il campo |
| SH-60B | 4 − d/50 h | 2 elicotteri per nave: ~1 in volo per 8-10 h al giorno |
| SH-3H | 3,0 − d/57 h (×0,5 in giornata calda) | doppio ciclo del ponte, 2 in zona su 6 |
| Ka-27PL | 1,5 h a 100 km, 1 h a 200 km (reale) | gruppo di 4-6 dal Kiev; 1-2 dall'Udaloy |
| Ka-25PL | ~1 h a 50 km (STIMA) | uno per nave |
| Be-12 | 3 h a 500 km, 4,5 h a 200 km (STIMA) | sempre in coppia |
| Il-38 | 7 h a 1000 km, 3 h a 2200 km (STIMA dal requisito e dalla media di 6 h) | coppie d'Il-38 su contatti lunghi (oltre 2 giorni in un caso nel Barents) |
| Tu-142M | 8 h a 2000 km, 2 h a 4000 km (STIMA) | il quadro passa via data link fra gli aerei; in pace pochi voli dopo il 1987 |

Oltre al carburante, la permanenza finisce quando finiscono **boe** o **armi**.

---

## 7. Macchina a stati dell'IA per l'ASW aereo [GIOCO]

Una sola macchina per tutti i mezzi; dottrina, sensori e tempi vengono dai parametri della classe (§2-3, §6) e
della parte (USA o URSS).

### 7.1 Stati

| Stato | Cosa fa | Esce verso |
|---|---|---|
| **T — Transito** | rotta alla zona o al datum, alla quota di transito; radar secondo l'EMCON (dossier 11 §6.2) | R, D, X |
| **R — Ricerca d'area** | barriera o campo di boe (aerei); dip di ricerca in linea (Ka-27, SH-3H); corse MAD di ricerca (Be-12) | K, X |
| **D — Investigazione del datum** | il datum arriva da SOSUS, nave, radar, ESM, siluro lanciato o colpo; scelta e posa dello schema (§7.3) | K, P |
| **K — Contatto** | conferma e classificazione: falso allarme o contatto reale | L, R/D |
| **L — Localizzazione** | fix: incroci DIFAR, RGB-2, eco del sonar a immersione, DICASS, MAD | A, I, P |
| **I — Inseguimento** | DIFAR o barriere davanti alla traccia; dip ogni 4-6 min (USA) o 10-12 min (URSS); passaggio del contatto alle navi | L, A, P, C |
| **A — Attacco** | criteri soddisfatti e regole d'ingaggio di guerra: avvicinamento, conferma MAD o DICASS, sgancio, marcatore | V |
| **V — Verifica** | attesa della corsa del siluro (6-9 min), ascolto; nuovo fix, poi un altro attacco o di nuovo L | A, L, P |
| **P — Contatto perso** | ricerca sul cerchio d'incertezza (cerchio, trappola davanti, spirale di dip) | L, R, X |
| **C — Cambio turno** | passa il quadro al rilevo (voce o data link) prima di andarsene | X |
| **X — Rientro** | carburante, boe o armi finite; o fine missione | — |

In **pace** e in esercitazione lo stato A è sostituito da un **attacco simulato**: un SUS Mk 84 per gli USA, un
rapporto al posto comando per i sovietici. Ne segue un messaggio radio e, secondo il file radio della missione,
un'escalation (dossier 10).

### 7.2 Eventi e probabilità di transizione

Tutti i valori sono STIMA, riferiti a un 688 a 5 kn sotto lo strato con mare 3, salvo dove indicato. I moltiplicatori
dell'equipaggio sono al §7.4.

| Da | Evento | Verso | P USA | P URSS | Note |
|---|---|---|---|---|---|
| T, R | cue SOSUS o di nave | D | 1 | 1 | ritardo d'arrivo come al §5.2 |
| R | boa o sonar oltre la soglia | K | per tick dalla rivelazione | idem | il motore acustico decide la rivelazione |
| K | è un falso allarme (biologici, mercantile, rumore) | R | 0,2 | **0,35-0,5** | Korshun 1985: fino al 50% di falsi allarmi |
| K | conferma (seconda boa, DIFAR, eco) | L | 0,7 in 10 min | 0,5 in 10 min | Il-38 «8-10 volte peggio» |
| L | fix entro 1000 yd | A (guerra), I (pace) | 0,6-0,8 in 10 min con 3-4 DIFAR | 0,4-0,6 (RGB-2 su barriera) | σ DIFAR 7° |
| L | conferma MAD su passaggio sulla verticale | A | 0,6 a 400 ft, 0,2-0,4 a 1000 ft | ×0,8 | §6.3 |
| L, I | contatto perso oltre 10 min | P | — | — | la catena riparte |
| I | contatto passato alle navi entro 100 km | I (ridotto) | 0,5 | **12 su 74** in un caso storico, quindi 0,16 | `[ART-Be12]` |
| P | riacquisizione entro 30 min | L | ~0,4 | ~0,3 | cala col ritardo, §5.2 |
| D | elicottero col sonar a immersione su un datum di 10 min | K, sull'intera ricerca | **0,14** | 0,1 | `[NPS-FD00]` (alta per il modello) |
| R | aereo senza cue su 10 000 nm² | K | < 0,05/h | < 0,01/h | |
| R | P-3 con 16 DIFAR su 3600 nm² e un cue | K | 0,3-0,5 in 2 h | — | formula SALT con W = 2 nm, V = 5 kn |
| V | sottomarino colpito | — | P_uccisione al §6.7 | AT-1M 0,15-0,3; APR-2 0,7 | |
| ogni stato | carburante sotto la riserva | C o X | 1 | 1 | |

**Velocità del battello.** Sopra i 15 kn la scoperta passiva vale ×3-5, in cavitazione ×10. Un battello fermo contro
boe passive viene scoperto meno di uno in moto (formula SALT: è il bersaglio che si muove attraverso il campo)
`[NPS-SALT90]`.

### 7.3 Scelta della manovra

**Scelta dello schema su un datum (USA):**

| Ritardo t e informazioni | Schema | P |
|---|---|---|
| t < 15 min, rotta ignota | cerchio di 6-8 DIFAR | 0,6 |
| | cuneo verso la fuga più probabile | 0,4 |
| t < 15 min, rotta nota | chevron | 0,6 |
| | trappola davanti | 0,4 |
| t 15-60 min | cerchio largo | 0,3 |
| | trappola davanti alla rotta probabile | 0,5 |
| | barriera | 0,2 |
| t > 1 h (cue SOSUS) | barriera sulla rotta verso il varco o la base | 0,7 |
| | campo distribuito | 0,3 |
| Contatto DIFAR singolo | cuneo | 0,5 |
| | due DIFAR per l'incrocio | 0,5 |
| Fix pronto | box DICASS | 0,7 (con DICASS a bordo) |
| | passaggio MAD diretto | 0,3 |

**Scelta della manovra (URSS):**

| Situazione | Manovra | P |
|---|---|---|
| Arrivo in zona (Il-38, Be-12) | barriera lineare di RGB-1 trasversale alla rotta presunta | 0,7 |
| | campo a scacchiera | 0,3 |
| Arrivo in zona (Tu-142M) | campo di RGB-75 a 15-20 km | 1 |
| Sottomarino visto immergersi | barriera circolare a spirale | 1 |
| Boa che reagisce | barriera circolare di 9-12 boe | 0,6 |
| | serie di RGB-2 per la rotta | 0,4 |
| Rotta stimata | barriere d'intercettazione ogni 6-10 km davanti (una reazione ogni ~26 min) | 1 |
| Battello silenzioso (Tu-142M) | eco esplosiva (MGAB e RGB-15), con 2-3 colpi a 3-4 min | 0,6 |
| | RGB-55A attiva | 0,4 |
| | nel Barents probabilità di riuscita ×0,2 | — |

**Elicotteri:**

| Situazione | USA (SH-3H) | URSS (Ka-27) |
|---|---|---|
| Ricerca | dip attivo di 15 min, salti di 10 nm | dip **passivo** di 3-10 min, salti di 4-6 km, in linea a 3-3,5 km |
| Contatto | ciclo di 4-6 min, salti di 5-7 nm davanti | due misure d'eco, poi il capogruppo assegna i punti |
| Perso | salto lungo davanti alla rotta (0,5) o spirale di dip (0,5) | spirale di dip (0,6) o boe (0,4) |
| Notte | dip normale | **niente dip**: boe soltanto |
| Mare ≥ 5, vento ≥ 35 kn | dip impossibile, solo boe (dossier 02 §7.7) | idem; le boe Baku sono già inutili oltre mare 3 |
| 40 °C | dip impossibile (SH-3H) | non documentato |
| Attacco | Mk 46 dall'hover (0,4) o a 150 ft e 100 kt (0,6) | da un altro elicottero già armato (0,7) o dallo stesso dopo il recupero (0,3) |

### 7.4 Equipaggio e errori

| Equipaggio | Errore di posa | Falsi allarmi | Ritardo di reazione | Perdita di contatto |
|---|---|---|---|---|
| Esperto (P-3 Update III, S-3B) | 100 yd | ×0,7 | ×0,7 | ×0,7 |
| Medio | 200 yd | ×1 | ×1 | ×1 |
| Scarso (Il-38 Berkut, Ka-25) | 300-500 yd | ×1,5 | ×1,5 | ×1,5 |

Gli errori realistici sono sempre attivi:
- **falsi contatti** su biologici e mercantili, legati al paesaggio sonoro (S5c) e al dossier 09;
- **boe guaste:** 5% USA (STIMA), 20-30% RGB-1 e 20-30% Baku (storico, alta);
- **boe che finiscono**, carburante;
- **ordini che interrompono l'inseguimento** in pace: 26 casi su 74 per il Be-12.

---

## 8. Che cosa percepisce il battello

### 8.1 Periscopio

- **Orizzonte geometrico** fra periscopio (2-3 m) e aereo `[CALC]`: `d (nm) ≈ 1,17·(√h_ft + √8)`. Un P-3 a 300 ft
  sta entro ~23 nm, a 1000 ft entro 40 nm; un elicottero a 50 ft entro 11 nm.
- **Portate d'avvistamento** (giorno, buona visibilità, operatore che guarda nel settore giusto; STIMA):

| Bersaglio | Ingrandimento 6× | 1,5× |
|---|---|---|
| P-3 a 300 ft | 8-12 nm | 4-6 nm |
| Elicottero in hover | 4-7 nm | 2-4 nm |

  - Foschia: ×0,5. Sole alle spalle dell'aereo: ×0,5.
  - Un P-3 grigio sotto nubi basse è difficile oltre 5 nm.
- **Sagome:**
  - P-3 e Il-38: quadrimotori ad ala bassa con il «pungiglione» del MAD;
  - S-3: bireattore compatto ad ala alta con l'asta del MAD;
  - Tu-142: grande, ala a freccia, eliche controrotanti;
  - Be-12: idrovolante ad ala di gabbiano;
  - SH-3: scafo a barca con galleggianti, 5 pale;
  - SH-60: fusoliera bassa, 4 pale, stabilizzatore orizzontale;
  - Ka-25 e Ka-27: rotori coassiali tozzi, niente rotore di coda.
- **Di notte:**
  - a luci accese (anticollisione, navigazione) l'aereo si vede a 10-20 nm (STIMA);
  - in guerra o in esercitazione realistica gli aerei volano oscurati: allora resta solo il rumore;
  - il Tu-142M **spegne il lampeggiante per le corse MAD** `[ART-Tu142]` (alta);
  - già nel 1966 le luci degli elicotteri ASW tradivano la loro posizione, e la Marina studiò luci visibili solo
    dall'alto `[NBS66]` (alta);
  - il proiettore anti-nave dei P-3A/B manca sul P-3C, che ha la TV a bassa luminosità: un fascio di proiettore è
    rarissimo e annuncia un attacco contro un battello in superficie.
- **Indizi visivi:**
  - l'anello di spruzzi del flusso del rotore sotto l'elicottero in hover;
  - un **Mk 25 o un Mk 58** in vista vuol dire che il nemico ha già un datum sulla tua posizione stimata:
    allontanarsi di 2-3 nm entro 15-20 min;
  - i galleggianti delle boe con l'antenna di 1-1,5 m, visibili a 0,5-2 nm con mare calmo e quasi invisibili con
    mare 3.

### 8.2 ESM

I dettagli sono nel dossier 11 §4-6.

| Radar | Che cosa si sente | Fonte | Conf. |
|---|---|---|---|
| APS-115 (P-3C) | banda X, due antenne (muso e coda) per 360°; un «colpo» del lobo ogni 5-10 s (STIMA: parente dell'APS-80 a ~6 giri/min); con tutte e due attive, due sorgenti sfasate | `[MILPER-APS115]` | media / bassa |
| APS-116 / APS-137 (S-3A/B) | **modo periscopio a 300 giri/min e 2000 impulsi/s**: un ronzio quasi continuo modulato a 5 Hz, la firma del cacciatore di periscopi | dossier 11 `[APL97]` | alta |
| APS-124 (SH-60B) | banda X, fino a 120 giri/min | dossier 11 | media |
| Initsiativa-2K/2KM (Ka-25/27) | centimetrico, 55 kW sul Ka-25; in volo acceso, in hover spento o poco usato | `[RW-Ka25]` | media |
| Berkut (Il-38), Initsiativa-2B (Be-12) | acceso prima della posa per «studiare la superficie», sui transponder delle boe e per mirare: segnale **a intervalli ma frequente** | `[ART-*]` | alta |
| Korshun (Tu-142M) | **salti di frequenza** impulso per impulso, quindi identificazione difficile; il Tu-142M ha l'ELINT e sente a sua volta il radar del battello | `[ART-Tu142]` | alta |

- **«Radar flooding»** `[COTE]` (alta): illuminare un'area con continuità perché l'allarme radar del sottomarino
  scatti a ogni alzata d'albero e lo tenga giù. **[GIOCO]** Un aereo che «inonda» vuole inchiodarti, non
  necessariamente trovarti.
- Nel 1962 i Foxtrot notarono un «livello elevato d'attività dei radar dell'aviazione antisom, a brevi intervalli»
  `[CIMSEC-ARKH]` (alta): è la percezione tipica.
- Un ESM silenzioso non vuol dire sicurezza: un P-3 in caccia acustica pura vola coperto, senza radar né radio
  `[SI-SUBHUNT]` (media).

### 8.3 Sonar

- **Finestra di Snell.** Il suono passa dall'aria all'acqua solo entro **~13° dalla verticale** (arcsin 343/1500).
  L'impronta in superficie ha raggio ≈ 0,23 × la quota. **[GIOCO]** Portata orizzontale udibile da un battello a
  profondità d: `x ≈ 0,18·h + 1,2·d`. Un P-3 a 91 m contro un 688 a 150 m dà ~200 m, cioè **~3 s** di ascolto
  `[AT-FERG19]` `[URICK72]` `[FMARS22]`. In una registrazione con un idrofono a 90 m un turboelica «si sente per
  pochi secondi» `[FMARS22]` (alta). Il problema era studiato davvero: Ferguson e Speechley (1989), *Acoustic
  detection and localization of an ASW aircraft by a submarine*.
- **Toni:**
  - P-3: **68 Hz** (4 pale × 17 Hz), Doppler ±5 Hz, armoniche fino a ~1 kHz `[AT-FERG19]` (alta);
  - Il-38 e Be-12: ~70 Hz; Tu-142: ~49 Hz e 98 Hz; S-3: niente tonale d'elica (`[CALC]`, media);
  - SH-60 e SH-3: ~17 Hz per il rotore principale, 80 Hz per la coda dell'SH-60; Ka-25: 11,9 Hz; Ka-27: ~13-14 Hz,
    con battimenti caratteristici dei rotori coassiali (STIMA sui giri).
- **Livelli** (STIMA):
  - P-3 a 300 ft sulla verticale contro un battello a 50-150 m: 110-125 dB re 1 µPa in banda 50-150 Hz, contro un
    rumore ambiente di ~80 dB;
  - il riferimento misurato sono i jet militari a Whidbey, 134 dB con un idrofono a 30 m `[PNNL-WHID]`.
- **[GIOCO] Regole d'ascolto:**
  - aereo: evento «aereo in sorvolo» solo quando passa nella finestra; P 0,8 in profondità, 0,95 alla quota
    periscopica, dove la superficie mossa allunga la finestra a 10-30 s;
  - elicottero in hover: «rumore di superficie localizzato» entro 0,5-1 nm (rotore più spruzzi).
- **Tonfi:**
  - i sonar sovietici sentivano **l'impatto delle boe in acqua (30 m/s) fino a 15 cavi, 2,8 km** `[ART-Be12]`
    (alta); è l'unico dato storico;
  - **[GIOCO]** boa A-size 155-165 dB impulsivo in 0,5-10 kHz, udibile a 1-3 nm (3-6 nm in profondo senza strato in
    mezzo);
  - Mk 46 170-180 dB, poi dopo 5-10 s il motore e la testa a 25-30 kHz: è l'evento «siluro in acqua»;
  - UMGT-1 a 60-65 m/s, motore subito; AT-1 a 45-55 m/s, poi il cerchio a 12°/s con i ping che arrivano dall'alto;
  - una **sequenza di tonfi a intervalli regolari** indica un aereo che posa una barriera.
- **Ping:**
  - AQS-13 a ~10 kHz, ≥ 211 dB, 3,5 o 35 ms; la sequenza è tonfo del trasduttore → 1-3 min di silenzio → ping ogni
    6-10 s → silenzio (recupero) → un altro tonfo 5-10 nm più in là: il comandante può ricostruire il ciclo e
    **fuggire durante il salto**;
  - DICASS: 6,5-9,5 kHz, a comando;
  - SSQ-47B: 13-19 kHz **ogni 10 s**, inconfondibile;
  - Ka-27: l'attivo è raro, quindi sentirlo vuol dire che c'è un contatto o un sospetto;
  - RGB-55A: 4 frequenze, fino a 1 h;
  - esplosive MGAB: colpi singoli a 25, 150 o 400 m, o un **treno a 4 Hz**, spesso 2-3 nello stesso punto a 3-4 min
    di distanza;
  - SUS Mk 84: toni a ~3 kHz per 45-120 s, cioè un attacco simulato o un codice d'ordine.
- **Asimmetria d'intercettazione** `[CALC]` (alta per il principio):

| Sonar | Intercettazione dal battello | Scoperta dell'eco |
|---|---|---|
| DICASS (sorgente 200 dB) | ~20 km | ~1,6-2 km |
| AQS-13 (sorgente 215 dB) | ~27 km | ~3-4 km |

  Il rapporto è **7-12×**: il battello sente il ping molto prima di essere scoperto, sa che è un elicottero (cadenza,
  frequenza fissa) e ne prende il rilevamento.

### 8.4 Il modello dell'Enhanced: riga di pala e riverberazione (A2, 25/10/2026)

Due limiti dell'acustica di A2 chiusi nel motore (`nucleo/acustica.cpp`), con le stime motivate.

**Riga di pala** `[Ross76]` `[U83]` (dossier 02 §2.2-2.3):
- `f = pale x giri / 60`, con i giri per nodo delle firme (02 §2.2, TPK inventato coerente).
- Livello: a 10 kn **15 dB sopra il continuo** dello spettro a bassa frequenza (Ross: 10-20 dB; STIMA a metà), poi
  **60 log V** (righe dipolari non cavitanti, Ross); +10 dB con la cavitazione, -10 posato. Le stesse righe vanno nello
  spettro LOFAR del giocatore e nella scoperta: il LOFAR di `valuta` prende la tonale dei macchinari e le righe d'asse,
  di pala e di doppia pala che cadono nella gamma del sensore. I sensori di bordo non hanno una gamma propria (le
  bande cominciano a 50-150 Hz) e non le prendono; le boe sì (SSQ-41B, 53B, 77A da 10 Hz, RGB-15 e RGB-75 da 2 Hz).
- RGB-75: LOFAR stretto (bin 0,05 Hz in 10 min, STIMA del trattamento del Korshun). Portate in acqua profonda (mare 1,
  boa e bersaglio a 150 m):

| Bersaglio | 4,5 kn | 8 kn | 12 kn | 20 kn |
|---|---|---|---|---|
| 688 | 0,4 km | 0,9 km | 3,0 km | ~30 km |
| Sturgeon | 0,7 km | 1,3 km | 3,9 km | ~30 km |
| Victor III | 1,3 km | 2,1 km | 5,7 km | ~30 km |
| Alfa | 2,3 km | 5,5 km | 11 km | ~40 km |

  «≤ 1 km a bassa velocità» `[ART-Tu142]` torna. I «15-20 km a 12 kn» tornano solo contro battelli rumorosi o
  sopra i 14-15 kn: col rumore ambiente di Wenz a 8-12 Hz (75-80 dB/Hz, dossier 02 §4) una riga di pala sentita a
  15-20 km a 12 kn dovrebbe valere ~145-150 dB, più di tutta la banda 5-200 Hz di un 688. **[STIMA; la fonte non dice
  contro che bersaglio]**

**Riverberazione** degli attivi `[U83]` cap. 8:
- volume: `SV = -80 dB` (c'era già);
- superficie: Chapman e Harris, `Ss = 3,3 b log(θ/30) - 42,4 log b + 2,6` con `b = 158 (v f^1/3)^-0,58` (v in kn, f in
  Hz), al più -20 dB `[CH62]`;
- fondo: Lambert, `μ = -27 dB` (02 §1.3);
- area dell'anello alla distanza obliqua dell'eco, `R φ c τ / 2`, con φ il fascio orizzontale (tutto il giro per le
  omni) e τ la durata, o la risoluzione 1/B per l'FM con compressione (DICASS: ~10 ms, STIMA);
- lobo verticale gaussiano largo quanto il fascio, -3 dB a metà fascio e al più -30 dB per verso: i lobi laterali
  delle schiere (STIMA).

Risultati (AQS-13B a 60 m, 688 a 5 kn; asimmetria col WLR-9 del bersaglio):

| Ambiente | Eco | Ping sentito | Rapporto |
|---|---|---|---|
| Mare di Norvegia, febbraio (strato a 250 m), 688 a 300 m, mare 3 | **5,4 kyd** | 54 km | 10,9 |
| idem, mare 5 | 2,7 kyd | 51 km | — |
| idem, 688 a 150 m (nello strato) | 10,6 kyd | 59 km | 6,1 |
| isoterma profonda, mare 3 | 9,2 kyd | 62 km | 7,4 |

  Il tipico (3-5 kyd con lo strato in mezzo) è al bordo alto; la buona propagazione dà 9-11 kyd (8-12 di §6.4). La
  DICASS dà 4,6-4,9 kyd col rapporto 10-12 (§8.3: 1,6-2 km e ~20 km). Le navi in acqua profonda restano come prima
  (SQS-53 ~9,5 km contro un Victor III); su 150 m di fondale la riverberazione di fondo dimezza la portata
  (4,6 km), come atteso in acqua bassa.

---

## 9. Efficacia storica: i numeri per tarare

| Fatto | Valore | Fonte | Conf. |
|---|---|---|---|
| Elicottero su un «flaming datum» di 10 min, contro un sottomarino che dosa la velocità | **P ≈ 0,14** | `[NPS-FD00]` | alta (modello) |
| Il-38 contro P-3C | idoneità alla ricerca **8-10 volte peggiore** | `[ART-Il38]` `[RU-IL38]` | media |
| Tu-142M, 1983 | 3 sortite per scoperta | `[ART-Tu142]` | alta |
| Boe del Korshun, 1985 | **50% di falsi allarmi** | `[ART-Tu142]` | alta |
| Be-12: campo di 60-70 boe | p = 0,5-0,6 su 5000 km² | `[ART-Be12]` | alta |
| Be-12: solo MAD | p = 0,01-0,02 su 2500 km² | `[ART-Be12]` | alta |
| AT-1, prove di stato | Pk 0,15-0,18 | `[ART-Be12]` | alta |
| Falkland, 1 maggio 1982 | il San Luis, fermo sul fondo a ~70 m, resiste a **20 h** di caccia di fregate e Sea King; molti attacchi su falsi contatti; ~50 Mk 46 spesi in tutta la campagna senza colpi | `[WP-SANLUIS]` `[WOTR]` | media / bassa |
| Santa Fe, 25 aprile 1982 | trovato **al radar in superficie** da un Wessex e danneggiato dalle bombe di profondità: l'elicottero uccide il battello in superficie | `[WP-SANTAFE]` | alta |
| Cuba 1962 | **3 Foxtrot su 4 costretti a emergere** per batteria scarica dopo una caccia prolungata; interni a +50-65 °C | `[CIMSEC-ARKH]` `[NSA-399]` | alta |
| P-3 contro un Echo II, 1979 | pedinamento con boe passive e attive, MAD e attacchi simulati; il battello vede l'attacco al periscopio, fa un «Crazy Ivan» e alla fine emerge | `[SI-SUBHUNT]` | media |
| Victor III K-324, 1983 | sorpreso in superficie da un P-3C con l'elica impigliata in un sonar trainato | `[SI-SUBHUNT]` | media |
| Anni '60 (Cote) | l'anello debole era la localizzazione; il MAD dava il via al lancio | `[COTE]` | alta |

**Morale per il gioco.** L'ASW aereo vince quando ha un **datum fresco**, un battello **rumoroso o in superficie** o
un diesel con la **batteria scarica**. Contro un SSN silenzioso, profondo e lento, che non espone l'albero, vince di
rado. I sovietici sono molto più deboli degli americani.

**Il fondale conta (27/10/2026, taratura del runtime).** Questi numeri vengono da oceani profondi. Sulla sabbia e sulla
roccia della piattaforma e della scarpata, sotto l'angolo critico il fondo riflette quasi tutto alle basse frequenze
(0,5 dB a rimbalzo a 250 Hz; perdita al fondo del motore dalla sera del 27/10), e le boe sentono più lontano per la via
del fondo. Per questo la scena «SSN silenzioso, come la storia» (A6.3) sta nella piana abissale di Porcupine, sul fango,
e dà 0,02; la stessa scena sulla scarpata delle Faroe (A6.3s) dà 0,39 ed è una fascia fisica, non storica. L'ASW non si
ritocca per inseguire i numeri storici fuori dalle acque profonde.

**STIMA della taratura A6.2 (25/10/2026): Il-38 contro P-3C.** Le due fonti del §9 tirano in direzioni diverse:
- il campo di 60-70 boe del Be-12 con p = 0,5-0,6 su 5000 km² (fonte alta) vale per le boe RGB e per il campo; da solo
  porterebbe l'Il-38 dove il Be-12 (0,55 sul cue SOSUS, contro 0,6 del P-3C);
- l'«8-10 volte peggio» dell'Il-38 (fonte media) viene dall'elaborazione del «Berkut» e dalla procedura di conferma,
  non dal carico di boe.

Scelta: la fonte alta tara il campo di boe, uguale per Be-12 e Il-38; la fonte media tara il divario fra le
piattaforme, con due moltiplicatori del solo Il-38 in `asw_aereo.toml` (`mezzi.il38`): **conferma × 0,08** e **falsi
allarmi delle boe × 3**. Risultato con l'evasione dell'IA, 80 semi: Il-38 **0,14** contro 0,60 del P-3C (circa 6 volte
in tasso di scoperta), Be-12 0,63 su 200 semi.

---

## 10. Proposte per l'Enhanced e per la difficoltà

Tutto vale solo con la regola di simulazione; il livello classico resta com'è: l'SH-60B che pattuglia, va sul datum e
fa comparire la boa marcatrice (docs/originale/comportamenti_ia.md §3.C).

### 10.1 Modulo e dati

- **Modulo puro nuovo**, `nucleo/asw_aereo.{hpp,cpp}`:
  - la **sonoboa** come entità: tipo, quota dell'idrofono, istanti di posa, d'impatto e di stabilizzazione, vita,
    deriva, guasto, soglia (sovietiche), banda;
  - la **posa degli schemi** del §6.1 e del §6.6 come funzioni pure: datum, ritardo, velocità stimata e portata
    danno i punti;
  - il **MAD** con la formula OPTEVFOR (§6.3);
  - il **ciclo del sonar a immersione** (§6.4-6.5) e la **macchina a stati** del §7.
  - La rivelazione resta del motore acustico (nucleo/acustica): boe e sonar a immersione diventano **ricevitori**
    con la loro quota e banda, sulla stessa equazione del sonar e con le stesse sorgenti.
- **Dati** in `data/motore/asw_aereo.toml`, generato da uno script della pipeline: boe (§4.1-4.2), carichi per
  missione (§2-3), TOS(d), quote e velocità (§6), probabilità e moltiplicatori (§7).
- **Classi nuove** in `classi.toml` e nel kit: s3a/s3b, sh3h, sh60f (solo 1990+), ka27, be12 (p3c, il38, tu142,
  sh60b e ka25 hanno già il modello).
- **Emettitori** da aggiungere a `data/motore/emettitori.toml`:
  - APS-116, APS-137 (S-3B, modo periscopio);
  - Korshun, con un flag di agilità di frequenza che rende difficile l'identificazione all'ESM;
  - Initsiativa-2B (Be-12) e Initsiativa-2KM (Ka-27);
  - il nome del radar del Ka-25 corretto in «Short Horn».

### 10.2 Che cosa si aggiunge alla missione

- Una **sorgente aerea** nella scena acustica, con la regola della finestra di Snell (§8.3): oggi gli aerei sono
  esclusi dalle sorgenti.
- **Transitori:** tonfi delle boe, del trasduttore e dei siluri; ping AQS-13, VGS-3, DICASS, SSQ-47; esplosive; SUS.
  Vanno sul bus degli eventi acustici, come gli eventi della missione (evento_acustico).
- **Registro M1:**
  - `boa_vicina` c'è già, si aggiungono:
    - `aereo_in_sorvolo`;
    - `ping_sonar_immersione` (oltre a `ping_subito`);
    - `marcatore_avvistato`;
    - `attacco_simulato` (il SUS in pace);
  - i conti per la carriera vanno nelle esposizioni.
- **Messaggi per il giocatore:** «tonfo in superficie, rilevamento X», «aereo in sorvolo», «sonar a immersione,
  rilevamento X», «fumogeno al periscopio». Servono codici nuovi nella fascia del runtime o in una fascia da
  concordare.
- **Effetti e kit**, come dal requisito 13:
  - hover a 12 m (SH-3H) o 30 m (Ka-27), con l'anello di spruzzi;
  - cavo e trasduttore che scendono e risalgono;
  - boe espulse con tonfo, galleggiante e colorante;
  - MAD rimorchiato;
  - siluro col paracadute;
  - Mk 25 e Mk 58;
  - luci di notte, spente in caccia.
- **Radio:**
  - in pace l'attacco simulato o il rapporto diventano un messaggio radio e, secondo il file radio, un cambio di ROE
    o un'escalation (dossier 10);
  - la «moving haven» del 688 in supporto diretto è una zona R2.

### 10.3 Difficoltà

| Livello | ASW aereo |
|---|---|
| **Facile** | scoperta ×0,5; equipaggi «scarsi»; tonfi e ping sempre udibili e segnalati; niente operazioni notturne per tutti; avviso «aereo ASW in zona» all'arrivo |
| **Normale** | parametri del dossier con equipaggi «medi»; gli avvisi sonar e ESM come al §8 |
| **Realistico** | equipaggi secondo il mezzo (USA esperti, sovietici scarsi o medi); boe guaste e deriva; finestra di Snell stretta; aerei oscurati in guerra; il comandante deve ricostruire il ciclo del sonar a immersione dagli indizi |
| **Opzione «minaccia percepita»** | i sovietici con l'efficacia che la NATO temeva (×5 sulla scoperta rispetto alla verità storica), per chi vuole un Il-38 pericoloso come un P-3 |

Valgono inoltre:
- teatro e stagione: giornata calda per l'SH-3H (Mediterraneo d'estate, Golfo), mare ≥ 4 per le boe sovietiche,
  Barents per le boe attive del Korshun;
- per l'Alfa (lato sovietico) il nemico è l'ASW USA, cioè P-3, S-3 e SH-3H.

### 10.4 Ordine di lavoro proposto

1. **Ricerca:**
   - ritentare le fonti DTIC e CIA della §12;
   - frequenze di VGS-2 e VGS-3;
   - livelli di sorgente misurati.
2. **Modulo e dati:** `nucleo/asw_aereo` e `asw_aereo.toml`, con le prove: posa degli schemi, MAD, ciclo di dip,
   transizioni.
3. **Missione:**
   - sorgente aerea e transitori;
   - IA degli aerei con la macchina a stati;
   - eventi del registro e messaggi.
4. **Kit ed effetti:** hover, cavo, boe, paracadute, marcatori, luci.
5. **Missioni:** le 22 originali conservano il loro SH-60B; le missioni rapide e l'editor possono chiedere P-3,
   S-3, Il-38, Tu-142, Ka-27 e Be-12, con la data e il teatro che scelgono i mezzi giusti.

---

## 11. Fonti

### 11.1 USA

- `[MILAQS13B]` MIL-D-81873(AS), *Detecting-Ranging Set, Sonar AN/AQS-13B*, 17-12-1975 — https://everyspec.com/MIL-SPECS/MIL-SPECS-MIL-D/MIL-D-81873_24742/
- `[RUDZIS]` J. D. Rudzis, *Improving the U.S. Navy SH-3 Sea King helicopter to counter the submarine threat in the 1990's*, tesi, Univ. of Tennessee, 1989 (prove NATC 1987) — https://trace.tennessee.edu/utk_gradthes/13063
- `[HOLLER]` R. A. Holler, «The Evolution of the Sonobuoy from World War II to the Cold War», *U.S. Navy Journal of Underwater Acoustics*, 2014 — https://www.navairdevcen.org/PDF/THE%20EVOLUTION%20OF%20THE%20SONOBUOY.pdf
- `[GAOALFS]` GAO/NSIAD-91-208, *Airborne Low Frequency Sonar*, 1991 — https://www.gao.gov/assets/nsiad-91-208.pdf
- `[NHHCSH60B]` Naval History and Heritage Command, SH-60B — https://www.history.navy.mil/content/history/nhhc/research/histories/naval-aviation-history/naval-aircraft/current-aircraft-inventory/sh-60b-seahawk.html
- `[FASSH2]` FAS, SH-2 Seasprite — https://man.fas.org/dod-101/sys/ac/sh-2.htm · `[GSSH2]` GlobalSecurity, SH-2 Seasprite — https://www.globalsecurity.org/military/systems/aircraft/sh-2.htm · `[ACSH2F]` Armed Conflicts, Kaman SH-2F Seasprite — https://www.armedconflicts.com/-t76521
- `[SIKS70B]` Sikorsky Archives, S-70B — https://sikorskyarchives.com/home/sikorsky-product-history/helicopter-innovation-era/sikorsky-s-70b/
- `[FASSH60]` https://man.fas.org/dod-101/sys/ac/sh-60.htm · `[FASP3]` https://man.fas.org/dod-101/sys/ac/p-3.htm · `[FASS3]` https://man.fas.org/dod-101/sys/ac/s-3.htm · `[FASMK46]` https://man.fas.org/dod-101/sys/ship/weaps/mk-46.htm
- `[FASSSQ41]` https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-41.htm · `[FASSSQ62]` http://www.fas.org/man/dod-101/sys/ship/weaps/an-ssq-62.htm · `[FASSSQ77]` https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-77.htm · `[FASSSQ36]` https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-36.htm
- `[FNWS9]` *Fundamentals of Naval Weapons Systems*, cap. 9 — https://man.fas.org/dod-101/navy/docs/fun/part09.htm
- `[WPAQS13]` https://en.wikipedia.org/wiki/AN/AQS-13 · `[WPSH60]` https://en.wikipedia.org/wiki/Sikorsky_SH-60_Seahawk · `[WPS3]` https://en.wikipedia.org/wiki/Lockheed_S-3_Viking · `[WPSH3]` https://en.wikipedia.org/wiki/Sikorsky_SH-3_Sea_King · `[WPVP11]` https://en.wikipedia.org/wiki/VP-11
- `[SBIRMAD]` Navy SBIR N15A-T009 — https://navysbir.com/n15_a/N15A-T009.htm
- `[TWZS3]` «Confessions of a Submarine Hunting S-3 Viking Crewman», The War Zone — https://www.twz.com/30525/confessions-of-a-submarine-hunting-s-3-viking-crewman-during-the-twilight-of-the-cold-war
- `[AGC13H]` The Aviation Geek Club, pattuglia di 13 h del P-3 — https://theaviationgeekclub.com/p-3-nfo-tells-the-story-of-when-his-orion-had-to-flew-a-13-hours-anti-submarine-patrol-and-shut-down-2-engines-because-the-p-3-due-to-relieve-his-aircraft-had-been-delayed/
- `[DMNP3]` Defense Media Network, «The P-3 Orion Faces Its Future» — https://www.defensemedianetwork.com/stories/the-p-3-orion-faces-its-future/
- `[HARP3]` `[HARPSOSUS]` `[HARPCVW]` `[HARPHAAWC]` HarpGamer (amatoriale, con bibliografia; HARPSOSUS trascrive Polmar) — https://harpgamer.com/harpforum/topic/3605-tactics-101-anti-submarine-warfare-part-3/ · https://harpgamer.com/harpforum/topic/4118-a-little-history-of-sosus-and-a-hint-of-what-it-is-capable-of/ · https://harpgamer.com/harpforum/topic/3614-air-wing-compositions-from-1943-to-1998/ · https://harpgamer.com/harpforum/topic/287-boeing-eyes-high-flying-torpedo/
- `[HOWARD11]` «Fixed Sonar Systems», *The Submarine Review*, 2011 — https://nps.edu/documents/103449515/0/HOWARDAPR2011.pdf/1219db41-a727-4940-ab0b-b953c54f6e01
- `[NSLZONE]` Naval Submarine League — https://archive.navalsubleague.org/?p=1475
- `[ARENDS]` S. R. Arends, *NWC Review* 32/3, 1979 — https://digital-commons.usnwc.edu/nwc-review/vol32/iss3/7
- `[USNI87Z]` `[USNI81]` `[USNIS3]` Proceedings 1987, 1981, 1979 (solo estratti) — https://www.usni.org/magazines/proceedings/1987/october/winning-battle-group-asw · https://www.usni.org/magazines/proceedings/1981/june/professional-notes · https://www.usni.org/magazines/proceedings/1979/july/pictorial-viking-sea
- `[JPROC]` CH-124 Sea King, aviolancio del Mk 46 — https://jproc.ca/rrp/rrp3/seaking_other.html
- `[VP4MK46]` https://www.vp4association.com/aircraft-information-2/32-2/mk-46-torpedo/
- `[AUS-MK25]` https://uxo.defence.gov.au/sites/default/files/2023-02/MarineMarkerMk25Rev01.pdf · `[AUS-MK58]` https://uxo.defence.gov.au/sites/default/files/2023-02/MarineMarkerMk58Rev02.pdf
- `[PRC68SUS]` https://prc68.com/I/SUSMk%2084.html
- `[DESIGSYS]` https://designation-systems.net/usmilav/jetds/an-arq2ary.html · `[NAVSRC]` http://www.navsource.org/archives/01/57s3.htm
- `[NBS66]` NBS Report 9437, 1966, luci degli elicotteri ASW — https://nvlpubs.nist.gov/nistpubs/Legacy/RPT/nbsreport9437.pdf
- `[ASWCONVOY]` *U.S. Fleet Anti-Submarine Instructions*, parte III — https://www.ibiblio.org/hyperwar/USN/ref/ASW-Convoy/ASW-Convoy-3.html
- `[MILPER-APS115]` https://www.militaryperiscope.com/weapons/electronics/airborne-radars/anaps-115/overview/

### 11.2 URSS

- `[ART-Il38]` A. Artem'ev, «"Il'jušin" contro la minaccia subacquea», *Aviacija i Kosmonavtika* — http://www.airwar.ru/enc/sea/il38.html
- `[ART-Tu142]` A. Artem'ev, «Il velivolo ASW a lungo raggio Tu-142» — http://www.airwar.ru/enc/sea/tu142m.html
- `[ART-Be12]` A. Artem'ev, «Be-12: l'aereo dei tre elementi» — http://www.airwar.ru/enc/sea/be12.html · `[ART-Be12N]` http://www.airwar.ru/enc/sea/be12n.html
- `[ART-Ka27]` A. Artem'ev, «L'elicottero imbarcato Ka-27», *Aviacija* — http://www.airwar.ru/enc/sh/ka27.html · `[AW-Ka25]` http://www.airwar.ru/enc/sh/ka25.html
- `[RW-Ka25]` https://ru.wikipedia.org/wiki/Ка-25 · `[RW-Ka27]` https://ru.wikipedia.org/wiki/Ка-27 · `[RW-Il38]` https://ru.wikipedia.org/wiki/Ил-38 · `[RW-Tu142]` https://ru.wikipedia.org/wiki/Ту-142 · `[RW-APR1]` https://ru.wikipedia.org/wiki/АПР-1 · `[RW-APR2]` https://ru.wikipedia.org/wiki/АПР-2 · `[RW-UMGT1]` https://ru.wikipedia.org/wiki/УМГТ-1
- `[TI]` TechInsider, prova del Ka-27PL — https://www.techinsider.ru/diy/14361-okhota-na-krupnuyu-rybu/
- `[CIA72]` CIA, *Soviet Antisubmarine Warfare: Current Capabilities and Priorities*, 1972 — https://archive.org/details/cia-readingroom-document-0005512850
- `[CIA-REG86]` CIA, *Combat Regulations of the Soviet Navy* (traduzione del regolamento 1983), 1986 — https://www.cia.gov/readingroom/print/2062270
- `[CIA-MT75]` CIA, *Military Thought (USSR): Some Aspects of an Operation to Destroy Enemy Missile Submarines* (Saakjan 1975) — https://www.cia.gov/readingroom/print/2062271
- `[EN-Tu142]` https://en.wikipedia.org/wiki/Tupolev_Tu-142 (Friedman 2006) · `[AI79]` *Air International* 1979 (bassa-media)

### 11.3 Il battello, modelli e casi

- `[NPS-MAD84]` Schluckebier, *A comparison of three MAD models*, NPS 1984 — https://calhoun.nps.edu/handle/10945/19199
- `[NPS-ASW16]` Ben Yoash, *Anti-submarine warfare search models*, NPS 2016 — https://calhoun.nps.edu/handle/10945/50460
- `[NPS-DIP18]` «Where to Dip?», *Military Operations Research* 23.2, 2018 — https://calhoun.nps.edu/handle/10945/61138
- `[NPS-FD00]` Soto, *The Flaming Datum problem with varying speed*, NPS 2000 — https://calhoun.nps.edu/handle/10945/26551
- `[NPS-SALT90]` Tisdale, *Initial detection models in SALT*, NPS 1990 — https://calhoun.nps.edu/handle/10945/34948
- `[COTE]` O. R. Cote Jr., *The Third Battle*, Newport Paper 16, 2003 — https://web.archive.org/web/2004/http://www.nwc.navy.mil/press/npapers/np16/NewportPaper16.pdf
- `[AAD-DIFAR]` *Accuracy and precision of DIFAR localisation systems* (IWC SC/65b/SH08) — https://www.marinemammals.gov.au/site/assets/files/1080/sc-65b-sh08.pdf
- `[AT-FERG19]` Ferguson et al., Acoustics Today 2019 — https://acousticstoday.org/wp-content/uploads/2019/03/International-Student-Challenge-Problem-in-Acoustic-Signal-Processing-2019-Brian-G.-Ferguson-1.pdf · `[URICK72]` Urick, *JASA* 52 (1972)
- `[FMARS22]` *Frontiers in Marine Science* 2022 — https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2022.996493/pdf
- `[PNNL-WHID]` https://tethys.pnnl.gov/publications/above-below-military-aircraft-noise-air-under-water-whidbey-island-washington
- `[U53D]` https://umaritime.com/wp-content/uploads/2026/03/53D-CANADA.pdf · `[U53G]` https://umaritime.com/wp-content/uploads/2024/02/SSQ-53G-DIFAR-Sonobuoy-Feb-2024.pdf · `[U62G]` https://umaritime.com/wp-content/uploads/2024/02/SSQ-62G-DICASS-SONOBUOY-Feb-2024.pdf
- `[DTIC-106692]` https://apps.dtic.mil/sti/pdfs/ADA106692.pdf (solo l'estratto)
- `[WP-SANLUIS]` https://en.wikipedia.org/wiki/ARA_San_Luis_(S-32) · `[WOTR]` https://warontherocks.com/2025/12/shallow-waters-big-lessons-what-the-falklands-war-reveals-about-submarine-warfare/ · `[WP-SANTAFE]` https://en.wikipedia.org/wiki/ARA_Santa_Fe_(S-21)
- `[CIMSEC-ARKH]` https://cimsec.org/vice-admiral-vasili-arkhipov-on-soviet-submarine-operations-during-the-cuban-missile-crisis/ · `[NSA-399]` https://nsarchive2.gwu.edu/NSAEBB/NSAEBB399/
- `[CH62]` R. P. Chapman, J. H. Harris, «Surface backscattering strengths measured with explosive sound sources», *JASA*
  34 (1962) 1592-1597 · `[Ross76]` D. Ross, *Mechanics of Underwater Noise*, Pergamon 1976 · `[U83]` R. J. Urick,
  *Principles of Underwater Sound*, 3a ed., McGraw-Hill 1983 (come nel dossier 02)
- `[SI-SUBHUNT]` https://airandspace.si.edu/stories/editorial/sub-hunter · `[RU-IL38]` https://ru.wikipedia.org/wiki/Ил-38

---

## 12. Lacune

1. **Livelli di sorgente misurati** di DICASS, SSQ-47 e VGS-3. Per l'AQS-13 c'è solo il minimo di specifica, ≥ 211 dB.
2. **Frequenze** di VGS-2, VGS-3, RGB-3 e RGB-55A.
3. **Rumore subacqueo misurato** di P-3 ed elicotteri, e del tonfo di boe e siluri: tutto STIMA, salvo i 2,8 km di
   Artem'ev.
4. **Manuali NATOPS e tattici:** quote e velocità ufficiali di sgancio del Mk 46 e spaziature ufficiali degli schemi
   di boe; il termine «pouncer».
5. **Area d'incertezza del SOSUS** (solo i 50 nm di Polmar, di seconda mano), tempi d'allerta dei P-3, portata del
   link ARQ-44.
6. **Tassi di successo nelle esercitazioni** degli anni '80 e studi CNA.
7. **Episodi concreti** di Il-38 o Tu-142 contro battelli NATO.
8. **Portata radar sovietica contro il periscopio:** c'è solo la CIA del 1972 in condizioni ideali; manca anche la
   banda esatta.
9. **Parametri dell'APS-115** (PRF, giri): ci sono solo quelli dell'APS-80.
10. **Altre lacune minori:** regole d'illuminazione notturna USN del 1989; mine aeree sovietiche; potenza di 5F48 e
    8F59; stato reale nel 1989 di Izumrud e dei Tu-142M dopo la decisione del 1987.
11. **Da ritentare quando i siti tornano in linea:**
    - Cote completo (DTIC ADA421957);
    - DTIC ADA140727, ADA281747, ADA106692, ADA257589, AD0912444/46;
    - CIA «Trends in Soviet ASW capabilities»;
    - Proceedings 1987 «Winning Battle Group ASW» e «Keeping the Orion a Hunter»;
    - GAO C-MASAD-81-4 (LAMPS III).

    Ritentato il 25/10/2026 (pezzo A7):
    - DTIC risponde ancora 403 (anche ai PDF diretti);
    - la sala di lettura della CIA è in manutenzione. Il rapporto «Trends in Soviet Antisubmarine Warfare
      Capabilities» però c'è: OSI-SR/SC/65-13 dell'11/10/1965, 8 pagine, CIA-RDP78T05439A000500370079-4,
      https://www.cia.gov/readingroom/node/332883. Dai risultati di ricerca: ASW sovietica in mare aperto «virtualmente
      nulla» contro i nucleari, il problema principale è la scoperta iniziale. Nessun dato tecnico leggibile;
    - di GAO C-MASAD-81-4 si ha solo la scheda: «The Light Airborne Multipurpose System, LAMPS MK III, progress
      evident but some problems and questions remain», 23/2/1981, https://www.gao.gov/products/c-masad-81-4;
    - Proceedings 1987: non cercato oltre, è dietro abbonamento;
    - SSQ-47B, sei frequenze fra 13 e 19 kHz e VHF da 1/4 W (FAS,
      https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-47.htm). Livelli di sorgente di SSQ-47B e DICASS: nessuno
      pubblicato;
    - RGB-55: attivo 10 min e passivo 50 min, a comando, collegamento fino a 30 km (forum armedconflicts.com,
      confidenza bassa). Frequenze di RGB-55A, VGS-2 e VGS-3: nessuna fonte aperta trovata.
    Nessun valore di asw_aereo.toml o acustica.toml cambia.

**Rimandi interni:**
- 01 §5-6 (mezzi e armi);
- 02 §7.6-7.7 (boe, MAD, EMCON, periscopio);
- 03 (zone);
- 09 (sonar e paesaggio sonoro);
- 10 (radio, ROE e zone R2);
- 11 (radar, EMCON, ESM, scoperta dell'albero);
- 14 (carico armi);
- requisito 13 di `docs/00_ROADMAP.md`;
- docs/originale/comportamenti_ia.md §3.C.
