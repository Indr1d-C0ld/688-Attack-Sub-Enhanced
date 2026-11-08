# 01 — Piattaforme, armi e sensori (dossier tecnico per il remake di *688 Attack Sub*)

> Ambientazione di riferimento: **1989** (missioni tra fine anni '80 e un conflitto ipotetico NATO–Patto di Varsavia).
> Scopo: fornire numeri citabili per fisica, acustica e IA. Ogni dato ha **fonte** e **confidenza** (alta / media / bassa).
> Compilato: 22 ottobre 2026.

---

## 0. Convenzioni e avvertenze

**Unità.** kn = nodi; nmi = miglia nautiche (1852 m); kyd = 1000 yard (914 m); t = tonnellate metriche salvo diversa indicazione; profondità in m (piedi tra parentesi quando la fonte è in piedi). Livelli acustici in **dB re 1 µPa @ 1 m** (SL = source level).

**Confidenza.**
- **alta**: dato ufficiale / declassificato, oppure concordanza di ≥2 fonti indipendenti autorevoli.
- **media**: fonte secondaria seria (Polmar, Friedman, Jane's citato, FAS, navweaps, Wikipedia RU/EN con fonti), oppure una sola fonte buona.
- **bassa**: stima, fonte singola debole, valore ricavato dal compilatore, o dato noto dalla letteratura ma **non riverificato online in questa sessione** (segnalato come *n.v.*).

**Sigle delle fonti** (dettagli completi nella §10 Bibliografia): es. `[FAS688]`, `[NWRU]`, `[MIA1]`. Le citazioni da libri (Polmar & Moore `[PM04]`, Polmar `[PGSN]`, Friedman `[FR94]`, `[FRWNWS]`, Jane's `[JFS]`) senza numero di pagina sono **dati ricordati dalla letteratura e non riverificati sul testo**: marcati *n.v.* e mai sopra “media”.

**Colonna “1989”.** `sì` = operativo nel 1989; `marginale` = pochissimi esemplari / IOC appena raggiunta / in ritiro; `no` = non in servizio (anacronismo).

**Dati ufficiali USA.** La US Navy pubblica volutamente valori “floor” (“25+ kn”, “oltre 800 ft”, “Mk48 > 28 kn, > 5 mi”). Per il gioco si usano le **stime della letteratura aperta**, segnalando sempre la differenza.

---

## 1. Coerenza storica 1989 e anacronismi del database originale

| Elemento nel DB del gioco originale | Stato reale nel 1989 | Verdetto | Raccomandazione per il remake |
|---|---|---|---|
| **688 Flight I** (SSN-688…718) | 31 battelli tutti in servizio; niente VLS; Tomahawk/Harpoon da tubo | sì | Piattaforma giocatore di default. |
| 688 Flight II con VLS (SSN-719+) | Providence (SSN-719) in servizio dal 1985; ~8 battelli nel 1989 `[WP688]` | sì | Variante selezionabile (12 VLS Tomahawk). |
| 688I (SSN-751+), BSY-1 | San Juan (SSN-751) ago. 1988, Pasadena feb. 1989 | **marginale** | Variante “sperimentale/late-1989” opzionale. BSY-1 solo su questa. |
| **Mk48 ADCAP** | Produzione 1985, IOC **1988** `[WPMk48]` `[NWUS]` | **marginale** | Dotazione mista: es. 4–6 ADCAP + resto Mk48 Mod 4; o opzione di scenario. |
| **Mk50 Barracuda** | Sviluppo; produzione dal ~1991–92, flotta ~1993–94 `[WPMk50]` | **no** | Togliere dal 1989; tenerlo solo come opzione “prototipo/scenario 1992+”. |
| **Spearfish** | Produzione piena 1988, servizio **1992–94** `[WPSpear]` `[NWBR]` | **no** | Trafalgar 1989 = **Tigerfish Mod 1/2**. Spearfish solo come opzione scenario futuro. |
| **Sea Lance (UUM-125)** | Programma **cancellato nel 1990**, mai operativo `[WPSUBROC]` | **no** | Eliminare o “arma ipotetica” in modalità what-if. |
| **SUBROC (UUM-44)** | In ritiro 1989–90; testate W55 smantellate 1990 `[WPSUBROC]` | **marginale** | Ammesso su Sturgeon/688 Flight I fino a fine 1989 (arma nucleare: in un conflitto convenzionale non verrebbe usata — opzione “regole d'ingaggio nucleari”). |
| Tomahawk TASM / TLAM-C / TLAM-N | IOC 1983 / 1986 / 1984 | sì | OK. |
| UGM-84 Sub-Harpoon | IOC USN 1981, RN ~1982 | sì | OK (sulle 688 rimosso solo negli anni '90). |
| TB-16 | In servizio dalla metà anni '70 | sì | OK. |
| TB-23 (thin line) | Introduzione fine anni '80 | **marginale** | Ammesso su alcuni 688; TB-16 come standard. |
| **Forrest Sherman DD** | Tutte radiate **1982–83** (Edson, nave scuola, radiata dic. 1988) | **no** | Sostituire con **Spruance DD-963** o **Knox FF-1052** (equivalenti 1989); se si mantiene il nome, ammetterla solo in scenari NRF/riserva. |
| **Koni (Pr.1159)** | Costruita per l'export; nella VMF **1 sola unità** (“Delfin”, addestramento equipaggi esteri) | **marginale** | Usarla come nave di marine alleate (DDR, Bulgaria, Cuba, Libia, Algeria, Jugoslavia) — coerente. |
| Alfa (Pr.705/705K) | 6 battelli; quasi tutti fermi/in riserva; radiati in massa nel 1990 `[DS705]` | **marginale** | Tenere (iconica), ma rara negli scenari. |
| Ka-25 Hormone-A | In servizio ma in sostituzione da Ka-27PL (dal 1982) | sì (declinante) | Tenere; aggiungere Ka-27PL Helix-A come alternativa realistica. |
| Typhoon, Oscar I/II, Akula I, Victor III, Charlie II, Kilo, Foxtrot | Tutti in servizio nel 1989 | sì | Vedi varianti esatte §2. |
| Kirov / Frunze / Kalinin | Kirov 1980, Frunze 1984, Kalinin 1988 — armamenti diversi | sì | Specificare l'unità (vedi §4.3). |
| Iowa BB | 4 unità attive (Iowa: esplosione torre II, 19 apr. 1989) | sì | OK, configurazione 1980s con Tomahawk/Harpoon/CIWS. |
| USET-80, TEST-71M, 65-76, SS-N-15/16, SS-N-21 | Tutti in servizio | sì | OK. SS-N-21 è arma strategica di attacco al suolo: irrilevante contro navi. |
| SET-65, 53-65K | In servizio (anche se superati) | sì | OK. |

---

## 2. Sottomarini

### 2.1 USA — SSN-688 *Los Angeles*, Flight I (fit anni '80)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 6.082 t emersione / 6.927 t immersione | `[FAS688]` `[WP688]` | alta |
| Dimensioni | 109,7 × 10,1 × 9,4 m (360 × 33 × 31 ft) | `[FAS688]` `[WP688]` | alta |
| Propulsione | 1 reattore S6G (~150–165 MWt), 2 turbine, 1 asse, ~30.000–35.000 shp; motore ausiliario retrattile 325 hp | `[FAS688]` `[WP688]` | alta |
| Velocità max | Ufficiale “20+/25+ kn”; reale **30–32 kn** (FAS), stime fino a 33+ | `[FAS688]` `[WP688]` | media |
| Velocità “silenziosa” | ~5–8 kn (ultraquieto ≤ 5 kn); vedi §3 | stima `[MIA1]` | bassa |
| Prof. collaudo (test) | **950 ft (290 m)** | `[FAS688]` | media |
| Prof. di collasso | **1.475 ft (450 m)** (Jane's cita 450 m come “test”) | `[FAS688]` `[WP688]` | media |
| Equipaggio | 13 ufficiali + 116 sottufficiali/marinai (~129) | `[FAS688]` | alta |
| Tubi | 4 × 533 mm, a centro nave, angolati | `[FAS688]` | alta |
| Carico armi | 22–26 armi da tubo (4 nei tubi + ricariche); Wikipedia indica 24 (testo) o 37 (infobox, dubbio) | `[WP688]` `[PM04]` *n.v.* | media |
| Armi 1989 | Mk48 Mod 4 (+ ADCAP marginale), UGM-84A Harpoon, Tomahawk TASM/TLAM da tubo, SUBROC (ritiro), mine Mk67 SLMM / Mk60 CAPTOR | `[FAS688]` `[WPSturgeon]` | alta |
| Direzione del tiro | Mk 117 (digitale; sostituisce Mk 113) | `[FR94]` *n.v.* | media |
| Sonar | AN/BQQ-5 (sfera BQS-13 con >1.000 idrofoni + array conforme di scafo), AN/BQS-15 (attivo HF corto raggio/ghiaccio), TB-16 (e TB-23 a fine '80) | `[FAS688]` `[WP688]` | alta |
| Radar / periscopi | AN/BPS-15; periscopio d'attacco Type 2, di ricerca Type 18 | `[FAS688]` | alta |
| ESM / intercettazione | AN/WLR-8(V); AN/WLR-9 intercettatore acustico | `[FAS688]` | alta |
| Timoni orizzontali | Sulla vela (Flight I/II): **non** adatti a emersione nel ghiaccio | `[WP688]` | alta |
| Tempo di ricarica tubo | Non pubblicato. Stima di gioco: **2–4 min/tubo** (caricatore idraulico, 1 squadra di siluristi) | stima | bassa |
| Raggio di virata | Non pubblicato. Stima: diametro tattico ≈ 4–6 lunghezze (~450–650 m) a 15–20 kn con timone pieno | stima | bassa |
| Velocità di variazione quota | Non pubblicata. Modello fisico: dz/dt ≈ v·sin(θ); assetto operativo tipico ≤ 15–20°, emergenza ~30° | stima | bassa |

**Note di gioco.** Nel 1989 un 688 Flight I imbarcava tipicamente un mix tipo ~14–18 Mk48 + 4 Harpoon + 4–8 Tomahawk (in base alla missione); le capsule Tomahawk/Harpoon consumano posti-siluro. Il 688I (SSN-751+) ha timoni prodieri retrattili, 12 VLS e BSY-1 — marginale nel 1989.

### 2.2 USA — SSN-637 *Sturgeon*

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 3.698 t / 4.714 t (3.640 / 4.640 long ton) | `[WPSturgeon]` | alta |
| Lunghezza | 89,1 m (scafo corto); **92,1 m** scafo lungo (da SSN-678 *Archerfish* in poi, 9 battelli) | `[WPSturgeon]` | alta |
| Larghezza / pescaggio | 9,65 m / 7,44 m | `[WPSturgeon]` | alta |
| Propulsione | S5W, 2 turbine, 15.000 shp, 1 asse | `[WPSturgeon]` | alta |
| Velocità | 15 kn emersione / **26 kn** immersione | `[WPSturgeon]` | media |
| Prof. collaudo | **1.320 ft (400 m)** (prof. di collasso stimata ~ 1,5× → ~600 m) | `[WPSturgeon]`; collasso: stima | media / bassa |
| Equipaggio | 107 | `[WPSturgeon]` | alta |
| Tubi / armi | 4 × 533 mm a centro nave; 4 + fino a 21 ricariche (~23 armi) | `[WPSturgeon]` | alta |
| Armi 1989 | Mk48 (+ADCAP), 4 Harpoon tipici, fino a 8 Tomahawk, 2–4 SUBROC (in ritiro), mine Mk67/Mk60 | `[WPSturgeon]` | alta |
| Sonar | BQQ-2 originale → **BQQ-5** con array rimorchiato (backfit anni '80); **BQS-8** da ghiaccio verso l'alto e BQS-12/13 di prua, per gruppi di scafi; il BQS-14 solo sulla Lipscomb (SSN 685), il BQS-14A sulla Parche (docs/ricerca/34) | `[WPSturgeon]` `[FASBQQ5]` `[NPSturgeon]` | media |
| Silenziamento | Retrofit (harmonic power conditioners, Special Hull Treatment su alcuni) | `[WPSturgeon]` | media |
| Timoni | Sulla vela, ruotabili a 90° per emersione nel ghiaccio | `[PM04]` *n.v.* | media |

### 2.3 Regno Unito — *Trafalgar*

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| In servizio 1989 | Trafalgar (1983), Turbulent (1984), Tireless (1985), Torbay (1987), Trenchant (1989); Talent e Triumph dopo | `[JFS]` *n.v.* | media |
| Dislocamento | 4.500–4.800 t / 5.200–5.300 t | `[WPTraf]` | media |
| Dimensioni | 85,4 × 9,8 × 9,5 m | `[WPTraf]` | alta |
| Propulsione | PWR1, 2 turbine GEC, 1 asse; **pump-jet** su tutti tranne HMS *Trafalgar* (elica 7 pale) | `[WPTraf]` | alta |
| Velocità | “oltre 30 kn” (32 kn comunemente) | `[WPTraf]` | media |
| Prof. collaudo | Classificata. Wikipedia: “600 m” (dubbio, probabilmente collasso o errato). Stima prudente: **test ~300 m, collasso ~500 m** | `[WPTraf]` / stima | bassa |
| Equipaggio | 97 (anni '80: 12 uff. + 85) / 130 (dato moderno) | `[WPTraf]` `[JFS]` *n.v.* | media |
| Tubi / armi | 5 × 533 mm; fino a 25–30 armi | `[WPTraf]` | alta |
| Armi 1989 | **Tigerfish Mk24 Mod 1/2**, Mk 8 (antinave), **UGM-84 Sub-Harpoon**, mine. *Niente Spearfish, niente Tomahawk (1998+).* | `[NWBR]` `[WPTiger]` | alta |
| Sonar | Type 2020 (prua), 2007 (fianco), 2024/2026 rimorchiato, 2019 PARIS (intercettazione), 2077 sotto-ghiaccio *n.v.* | `[FRWNWS]` *n.v.* | media |

### 2.4 URSS — Pr.705/705K *Lira* (“Alfa”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 2.300 t / 3.180 t (alt. 3.610) | `[DS705]` | media |
| Dimensioni | 81,4 × 10,0 × 7,6 m | `[DS705]` | media |
| Propulsione | Reattore a metallo liquido (Pb-Bi) OK-550 (705) / BM-40A (705K), 1 turbina ~40.000 shp, 1 asse | `[DS705]` `[WPAlfa]` | media |
| Velocità | **41 kn** in servizio (41,2 kn); picchi di prova ~43–44 | `[WPAlfa]` | alta |
| Profondità | **lavoro 320–350 m, limite 400 m** (fonti russe). Stime occidentali anni '80: 600–900 m (2.000–3.000 ft) — **errate** | `[DS705]` `[GS705]` | alta (russa) |
| Equipaggio | 31–32 (solo ufficiali/michman, altamente automatizzato) | `[DS705]` | alta |
| Tubi / armi | 6 × 533 mm; ~18–20 armi (SET-65, 53-65K, TEST-71, RPK-2 Vyuga, mine) | `[DS705]` `[PGSN]` *n.v.* | media |
| Sonar | GAK “Okean” *n.v.* | `[PGSN]` *n.v.* | bassa |
| Caratteristiche | Accelerazione e virata eccezionali (relatori occidentali: inversione di rotta in decine di secondi); **molto rumoroso** ad alta velocità | `[GS705]` `[PM04]` *n.v.* | media |
| 1989 | Quasi tutti in riserva/ fermi; 5 radiati nel 1990 | `[DS705]` | media |

**Nota storica:** fu proprio la (sopravvalutata) profondità/velocità dell'Alfa a spingere l'accelerazione del programma ADCAP `[WPAlfa]`.

### 2.5 URSS — Pr.671RTM *Shchuka* (“Victor III”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | ~4.950–6.990 t emersione / **7.250 t** immersione | `[WPVictor]` | media |
| Dimensioni | 106–107,1 × 10,6 × 7 m | `[WPVictor]` | media |
| Propulsione | 1 VM-4P (2 × 75 MWt), turbine 31.000 shp, 1 asse con eliche tandem a 4 pale + 2 eliche ausiliarie a bassa velocità | `[WPVictor]` | media |
| Velocità | **30–31 kn** immersione; 11,6 kn emersione | `[RW671]` | media |
| Profondità | **lavoro 400 m, limite 600 m** (RU); Polmar 400 m test *n.v.* | `[RW671]` | media |
| Equipaggio | ~96–100 | `[WPVictor]` | media |
| Tubi | 4 × 533 mm + 2 × 650 mm | `[WPVictor]` | alta |
| Carico | 24 armi o 36 mine | `[WPVictor]` | media |
| Armi 1989 | 53-65K, SET-65, TEST-71M, USET-80; 65-76 (650 mm); RPK-2 Vyuga (SS-N-15), RPK-6/7 Vodopad/Veter (SS-N-16); 671RTMK con S-10 Granat dal 1988 | `[NWRU]` `[WPRPK6]` `[WPRK55]` | media |
| Sonar | **MGK-503 Skat-KS** + array rimorchiato nel “pod” sul timone verticale | `[RWSkat]` `[WPVictor]` | alta |
| Rumore | Primo SSN sovietico “quasi” al livello occidentale: in ambienti USN considerato pari allo *Sturgeon* | `[TYL]` nationalinterest | media |

### 2.6 URSS — Pr.971 *Shchuka-B* (“Akula I”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| In servizio 1989 | K-284 (dic. 1984), K-263 (1987), K-322, K-480 (1988), ~1 altro nel 1989 | `[WPAkula]` `[RN971]` | media |
| Dislocamento | 8.140 t / 12.770 t (dati russi); stime occidentali anni '80 più basse (~7.500 / 9.100) | `[WPAkula]` `[PM04]` *n.v.* | media |
| Dimensioni | 110,3 × 13,6 × 9,7 m | `[WPAkula]` | media |
| Propulsione | OK-650B (190 MWt), turbina ~43.000–50.000 shp, 1 asse, elica a 7 pale + 2 propulsori ausiliari | `[WPAkula]` | media |
| Velocità | **33 kn** (alcune schede 30–35) | `[RN971]` | media |
| Profondità | **lavoro 480–520 m, collaudo/limite 600 m** | `[RN971]` | alta |
| Equipaggio | 73 | `[WPAkula]` | alta |
| Tubi | 4 × 533 mm + 4 × 650 mm | `[WPAkula]` | alta |
| Carico | ~40 armi (28 da 533 + 12 da 650) *n.v.* | `[PM04]` *n.v.* | media |
| Armi 1989 | USET-80, TEST-71M, 65-76; Vyuga/Vodopad/Veter; S-10 Granat (SS-N-21) | `[WPRK55]` `[WPRPK6]` | media |
| Sonar | **MGK-540 Skat-3** (prua + 2 antenne laterali estese + rimorchiato) — il DB del gioco dice “MGK-500 Skat” | `[RWSkat]` | alta |
| Rumore | Shock per la USN: più silenzioso di ogni SSN sovietico precedente; i 971 *migliorati* (anni '90) giudicati più silenziosi del 688I `[USNI95]` | `[USNI95]` `[MIA1]` | media |

### 2.7 URSS — Pr.949/949A *Granit/Antey* (“Oscar I/II”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| In servizio 1989 | Oscar I: K-525 (1980), K-206 (1983); Oscar II: dal 1986 (K-148), ~3 nel 1989 | `[WPOscar]` | media |
| Lunghezza | ~143 m (949) / ~154 m (949A); larghezza 18,2 m | `[WPOscar]` | alta |
| Dislocamento | 949A: 12.500–14.700 t emersione / 16.500–24.000 t immersione (le fonti divergono molto per definizione di “immersione”); **gioco: 14.700 / 19.400** | `[WPOscar]` | media |
| Propulsione | 2 × OK-650 (190 MWt), 2 turbine, ~98.000 shp, 2 assi | `[WPOscar]` | alta |
| Velocità | 15 kn / **32 kn** | `[WPOscar]` | media |
| Profondità | collaudo **600 m** (lavoro ~520 m) | `[WPOscar]` | media |
| Equipaggio | 94–107 | `[WPOscar]` | media |
| Tubi | 4 × 533 + 2 × 650 mm; 28 armi o 32 mine | `[WPOscar]` | alta |
| Missili | **24 P-700 Granit (SS-N-19)** in contenitori tra i due scafi, lancio in immersione | `[WPOscar]` `[WPGranit]` | alta |
| Sonar | MGK-540 Skat-3 | `[RWSkat]` | alta |

### 2.8 URSS — Pr.670M *Chayka* (“Charlie II”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 4.300 t / 5.100–5.500 t | `[WPCharlie]` | media |
| Dimensioni | 103–104,5 × 10 m | `[WPCharlie]` | media |
| Propulsione | 1 reattore, 1 turbina ~15.000 shp, 1 asse | `[WPCharlie]` | alta |
| Velocità | 20 kn emersione / **24 kn** immersione | `[WPCharlie]` | media |
| Profondità | ~300 m collaudo (stima letteratura) *n.v.* | `[PM04]` *n.v.* | bassa |
| Equipaggio | 98 | `[WPCharlie]` | media |
| Tubi | Wikipedia EN: 6 × 533 mm (12 armi); fonti russe: **4 × 533 + 2 × 400 mm** *n.v.* | `[WPCharlie]` / *n.v.* | media |
| Missili | **8 P-120 Malakhit (SS-N-9 Siren)**, lancio in immersione, ~110–120 km | `[WPCharlie]`; portata `[FRWNWS]` *n.v.* | media |
| Rumore | 2ª generazione, rumoroso | `[TYL]` | media |

### 2.9 URSS — Pr.941 *Akula* (“Typhoon”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| In servizio 1989 | 6 battelli (1981–1989) | `[WPTyphoon]` | alta |
| Dislocamento | 23.200 t / **48.000 t** | `[WPTyphoon]` | alta |
| Dimensioni | 175 × 23 × 12 m | `[WPTyphoon]` | alta |
| Propulsione | 2 × OK-650 (190 MWt), 2 × 50.000 shp, 2 assi, eliche a 7 pale intubate | `[WPTyphoon]` | alta |
| Velocità | 22 kn / **25–27 kn** (testo WP: 28) | `[WPTyphoon]` | media |
| Profondità | collaudo **400 m** (limite ~500 m stima) | `[WPTyphoon]` | media |
| Equipaggio | 160 | `[WPTyphoon]` | alta |
| Tubi | **6 × 533 mm** (fonti RU/WP); alcune fonti occidentali: 2 × 650 + 4 × 533 → usare 6 × 533 | `[WPTyphoon]` | media |
| Carico | 22 armi (Type 53, RPK-2 Vyuga, RPK-6) | `[WPTyphoon]` | media |
| SLBM | 20 R-39 (SS-N-20) | `[WPTyphoon]` | alta |
| Sonar | MGK-503 Skat-KS | `[RWSkat]` | alta |
| Rumore | **SL stimato 125 dB (5–200 Hz) e 105 dB/Hz a 1 kHz a 4–8 kn** | `[MIA1]` Tab. A2 | media |

### 2.10 URSS — Pr.877 *Paltus* (“Kilo”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 2.325 t / 3.075 t | `[WPKilo]` | alta |
| Dimensioni | 72,6 × 9,9 × 6,2 m | `[WPKilo]` | alta |
| Propulsione | Diesel-elettrica: 2 × 1.000 kW generatori, motore 5.500–6.800 shp, elica 6 pale | `[WPKilo]` | alta |
| Velocità | 17 kn emersione (10 snorkel), **20 kn** immersione | `[WPKilo]` | alta |
| Autonomia batteria | **400 nmi a 3 kn**; 12,7 nmi a 21 kn (≈ 36 min a tutta) | `[WPKilo]` | alta |
| Autonomia snorkel | 6.000–7.500 nmi a 7 kn; 45 giorni | `[WPKilo]` | alta |
| Profondità | lavoro **240 m**, limite **300 m** | `[WPKilo]` | alta |
| Equipaggio | 52 | `[WPKilo]` | alta |
| Tubi / armi | 6 × 533 mm (prua); 18 siluri o 24 mine | `[WPKilo]` | alta |
| Sonar | **MGK-400 Rubikon** (“Shark Gill”) + MG-519 Arfa (mine) | `[WPKilo]` | alta |
| Rumore | Classe “quiet” di Miasnikov (≈120 dB 5–200 Hz, 100 dB/Hz @1 kHz); Typhoon/Delta IV 8–13 dB sopra il Kilo coevo → **Kilo in batteria ≈ 112–117 dB** | `[MIA1]` | media |

### 2.11 URSS — Pr.641 (“Foxtrot”)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Dislocamento | 1.983 t / 2.515 t | `[WPFoxtrot]` | alta |
| Dimensioni | 89,9 × 7,4 × 5,9 m | `[WPFoxtrot]` | alta |
| Propulsione | 3 diesel 2D42M × 2.000 hp; 3 motori elettrici (2 × 1.350 + 1 × 2.700 hp); 3 assi | `[WPFoxtrot]` | alta |
| Velocità | 16 kn emersione / **15 kn** immersione / 9 kn snorkel | `[WPFoxtrot]` | alta |
| Autonomia batteria | 380 nmi a 2 kn; immersione 3–5 giorni (testo WP: fino a 10) | `[WPFoxtrot]` | media |
| Profondità | collaudo 246–296 m (Polmar ~280 m) | `[WPFoxtrot]` | media |
| Equipaggio | 78 | `[WPFoxtrot]` | alta |
| Tubi | **10 × 533 mm (6 prua + 4 poppa)**; 22 siluri (o 32 mine) | `[WPFoxtrot]` | alta |
| Armi 1989 | 53-65K, SET-65, SAET-60M, SET-53M; 400 mm a poppa su alcune *n.v.* | `[NWRU]` | media |
| Sonar | MG-200 Arktika-M (attivo) + MG-10 Feniks (passivo) *n.v.* | *n.v.* | bassa |

---

## 3. Modello acustico (rumore irradiato) — STIME

> **Avvertenza fondamentale.** I livelli assoluti di rumore irradiato dei sottomarini **non sono pubblicati** in letteratura aperta (lo dice esplicitamente GlobalSecurity, citando Lurton 2002). I valori seguenti sono **stime di modellazione** costruite su: (1) tabelle di Miasnikov 1998 `[MIA1]` `[MIA2]` (l'unica fonte aperta con numeri per classe); (2) trend di Tyler (JHU APL, 1992) `[TYL]`: rumore sovietico **–30 dB tra 1975 e 1988**; (3) graduatorie qualitative USN/ONI riportate da USNI 1995 `[USNI95]` e Polmar `[PM04]`. **Confidenza complessiva: bassa–media.** Usarle come calibrazione relativa coerente, non come “verità”.

### 3.1 Dati di base ricavati dalla letteratura aperta

**Miasnikov, Appendice 1 `[MIA1]`:**
- Classi modello (SL in banda 5–200 Hz / spettro a 1 kHz per Hz): *rumoroso* 140 / 120; *silenzioso* 120 / 100; *molto silenzioso* 100 / 80 dB.
- Picco del continuo a 50–100 Hz; sopra 200 Hz cala di **6 dB/ottava** (= 20 dB/decade) → `SL(f) = SL(1 kHz) – 20·log10(f/1000)` (200 Hz–10 kHz) `[MIA2]`.
- Velocità: ultraquieto ≈ 4 kn; max “low-noise” ≈ 8 kn (+5–10 dB); transito 15 kn: **+15–20 dB** rispetto al quieto; in regime di transito “SL ∝ v⁶” (≈ +18 dB per raddoppio di velocità).
- Righe discrete: linee di rete **50 Hz (URSS) / 60 Hz (USA)** e armoniche (fino alla 5ª registrata); linee d'elica 0,1–10 Hz; le righe discrete superano lo spettro continuo a 1 kHz di ~20 dB.
- Invecchiamento: +5 dB medi nella vita operativa.
- Riferimento russo vs americano: differenza di 27 dB tra le convenzioni (attenzione nel mescolare fonti RU/US).

**Miasnikov, Tabella A2 (SSBN russi, 5–200 Hz / 1 kHz):** Hotel 140–145/120–125 (4 kn); Yankee 135–140; Delta I/II 130–135; Delta III 125–130; **Typhoon 125/105 (4–8 kn)**; Delta IV 120/100; **Akula (971) 110/90** (confronto).

**Tyler 1992 `[TYL]`:** Victor III (1978) primo battello sovietico a basso rumore; calo di ~30 dB (fattore 1000) dal 1975 al 1988.

### 3.2 Tabella di gioco proposta (dB re 1 µPa @1 m, banda larga 5–200 Hz; valore a 1 kHz/Hz ≈ –20 dB)

| Classe | SL @ velocità silenziosa (≈4–5 kn) | @ 8 kn | @ 15 kn | @ max | Note / tonali | Conf. |
|---|---|---|---|---|---|---|
| Foxtrot (batteria) | 120–125 | 128 | (max 15) 138 | 140 @15 kn | snorkel: **+20–25 dB** (diesel, 140–145) | bassa |
| Kilo (batteria) | **112–117** (Miasnikov) | 120 | 132 | 138 @20 kn | snorkel +20–25 dB | media |
| Charlie II | 130–135 | 138 | 148 | 155 @24 kn | 2ª gen.; 50 Hz forti | bassa |
| Alfa | 125–130 | 135 | 145 | **≥ 160 @ 40 kn** | pompe Pb-Bi; rumorosissimo veloce | bassa |
| Victor III | 120–125 | 127 | 140 | 150 @ 30 kn | pari a Sturgeon (USN) | bassa-media |
| Typhoon | **125** (4–8 kn) | 125 | 140 | 148 @ 25 kn | `[MIA1]` | media |
| Oscar I/II | 118–123 | 125 | 138 | 150 @ 32 kn | 2 assi, grande scafo | bassa |
| Akula I | **110–115** | 117 | 130 | 145 @ 33 kn | `[MIA1]` 110 | media |
| Sturgeon | 115–120 | 122 | 135 | 145 @ 26 kn | 60 Hz | bassa |
| 688 Flight I | **110–115** | 116 | 128 | 143 @ 32 kn | 60 Hz; più silenzioso di Akula I nel 1989 | bassa |
| 688I | 105–110 | 112 | 125 | 140 | marginale 1989 | bassa |
| Trafalgar (pump-jet) | 108–113 | 114 | 126 | 140 @ 32 kn | pump-jet: niente linea di pala netta | bassa |

**Navi di superficie e mercantili (banda larga, 15–25 kn)**, da Ross 1976 `[ROSS]`, Urick 1983 `[URICK]`, McKenna et al. 2012 `[MCK]` *(n.v. per i valori specifici)*: fregate/caccia 150–170 dB (molto variabile con cavitazione dell'elica; Prairie-Masker USN riduce –10 dB); portaerei/BB 170–180; **mercantili 175–185, grandi petroliere 180–190 dB**; navi con propulsione a turbina a gas hanno righe di riduttore marcate. Conf.: media.

### 3.3 Modello di velocità/profondità raccomandato

```
SL(v,d) = SL_q                                  se v ≤ v_q
        + 20·log10(v/v_q)                       se v_q < v ≤ v_cav(d)   (macchinario-dominato; ≈ +6 dB per raddoppio)
        + 60·log10(v/v_cav(d))  (in aggiunta)    se v > v_cav(d)         (cavitazione/flusso; Miasnikov "∝ v⁶")
v_cav(d) ≈ v_cav0 · sqrt((10 + d)/10)           d in metri (la pressione idrostatica ritarda la cavitazione)
```
- Valori suggeriti di `v_cav0` (a quota periscopica): 1ª–2ª gen. sovietica 5–6 kn; Victor III/Akula/688 7–9 kn; Trafalgar pump-jet 9–10 kn. **Stime** (bassa).
- **Tonali**: linea di pala `f_BR = n_pale × rpm/60` (688: 7 pale; Akula: 7; Kilo: 6; Typhoon: 2 × 7; Foxtrot: 3 assi × 6); linee di rete 50/60 Hz e armoniche; righe di pompe del primario (reattori sovietici di 2ª gen. senza circolazione naturale) e riduttori; linee di risonanza indotte dal flusso (FIR) — un Victor III “sotto i 6 kn era invisibile agli array tranne due righe FIR” (aneddoto, bassa).
- **Transitori** (+10–30 dB per pochi secondi): apertura portelli tubi, lancio, variazioni brusche di elica, scarichi. Il WLR-9 dei 688 rileva i transitori e i ping.

---

## 4. Navi di superficie

### 4.1 USA

| Classe | Disl. (pieno carico) | Dimensioni | Velocità | Prop. | Equip. | Armi principali (1989) | Sonar / Radar | Elicotteri | 1989 | Fonte / Conf. |
|---|---|---|---|---|---|---|---|---|---|---|
| **Forrest Sherman DD-931** | ~4.050–4.200 t | 127,4 × 13,8 m | 32,5 kn | 2 turbine vapore, 70.000 shp | ~290–330 | 2–3 × 5"/54 Mk42; ASROC (unità modernizzate ASW); 2 × Mk32 triple (Mk46) | SQS-23 (+SQS-35 VDS su alcune); SPS-40 | — | **no** (radiate 1982–83) | `[PGSN]`/`[JFS]` *n.v.* — media |
| **Kidd DDG-993** | ~9.570 t | 171,6 × 16,8 m | 33 kn | 4 × LM2500, 80.000 shp | ~340 | 2 × Mk26 (SM-2MR + ASROC), 8 Harpoon, 2 × 5"/54 Mk45, 2 Phalanx, 2 × Mk32 | **SQS-53A**; SPS-48E, SPS-49 | 1 SH-2F (LAMPS I) | sì | *n.v.* — media |
| **Oliver H. Perry FFG-7** | ~4.100 t | 135,6 m (138,1 m long hull) × 13,7 m | 29 kn | 2 × LM2500, 1 asse, 41.000 shp | ~200 | Mk13 (36 SM-1MR + 4 Harpoon), 76/62 Mk75, Phalanx, 2 × Mk32 (Mk46) | **SQS-56** (+ **SQR-19 TACTAS** su unità LAMPS III); SPS-49 | 2 SH-60B (unità LAMPS III, FFG-36+) o SH-2F | sì | *n.v.* — media |
| **Nimitz CVN** | ~91.000–100.000 t | 332,8 × 40,8 m (ponte 76,8 m) | 30+ kn | 2 × A4W, 4 assi, 260.000 shp | ~3.200 + 2.480 gruppo aereo | 3 Sea Sparrow Mk29, 3–4 Phalanx | nessun sonar | ~85–90 velivoli (CVW incl. S-3A Viking, SH-3H) | sì | *n.v.* — media |
| **Iowa BB** (riattivate 1982–88) | ~57.500 t | 270,4 × 33 m | 33 kn (progetto) | 4 turbine, 212.000 shp | ~1.500–1.800 | 9 × 16"/50, 12 × 5"/38, **32 Tomahawk** (8 ABL), **16 Harpoon**, 4 Phalanx | nessun sonar; SPS-49 | piazzola (RPV Pioneer) | sì | *n.v.* — media |
| **Sacramento AOE-1** | ~53.600 t | 241 × 33 m | 26 kn | turbine vapore, 100.000 shp | ~600 | Sea Sparrow, 2 Phalanx | — | 2 UH-46 | sì | *n.v.* — media |

**Sonar di superficie USA (1989):**
- **AN/SQS-53 (A/B/C)**: evoluzione del SQS-26CX; modalità attive *direct path*, *bottom bounce* e **zona di convergenza (CZ)**; passiva con rilevamento vero `[FASSQS53]` (alta). Frequenza ~3,5 kHz *n.v.* (media). Portate tipiche: direct path ~10–20 km; CZ 1ª a ~55 km (≈30 nmi) in acque profonde *(fisica del canale, media)*.
- **AN/SQS-56**: sonar di scafo a media frequenza delle FFG-7 (~5,6/7,5/8,4 kHz *n.v.*, bassa); portata direct path ~5–10 km.
- **AN/SQR-19 TACTAS**: array rimorchiato passivo tattico, ~1 miglio a poppavia, 360°, parte dell'SQQ-89 `[FASSQR19]` (alta); introdotto ~1985 *n.v.* (media). Nel gioco: ottima rilevazione a bassa frequenza fino a CZ; ambiguità destra/sinistra.
- **SQS-23** (Forrest Sherman), ~5 kHz *n.v.* (bassa).

### 4.2 URSS — combattenti

| Classe | Disl. | Dimensioni | Velocità | Prop. | Equip. | Armi | Sonar | Elicotteri | 1989 | Fonte |
|---|---|---|---|---|---|---|---|---|---|---|
| **Kara (Pr.1134B)** | 8.200 / 9.700 t | 173,2 × 18,6 × 6,7 m | 34 kn | COGAG 120.000 hp, 2 assi | 380 | 2 × 4 **Metel (SS-N-14)** 8 missili, ~50 km; 2 × 2 Shtorm (SA-N-3, 80); 2 × 2 Osa-M (SA-N-4, 40); 2 × 2 AK-726 76 mm; 4 AK-630; **2 RBU-6000 + 2 RBU-1000**; **2 × 5 tubi 533 mm** (6 × 53-65K + 4 × SET-65) | **MG-332 Titan-2** (scafo) + **MG-325 Vega** VDS (fino a 100 m) | 1 Ka-25PL | sì | `[WPKara]` alta |
| **Kirov (Pr.1144)** | ~24.300 t pieno carico *n.v.* | ~252 × 28,5 × 9,1 m | 32 kn *n.v.* | CONAS: 2 × KN-3 + caldaie, 120.000–140.000 shp, 2 assi | ~710–740 *n.v.* | **20 P-700 Granit**; 12 × S-300F (96); 2 × Osa-M (40); Kirov: 2 × 100 mm, SS-N-14 (14 missili *n.v.*), 1 RBU-6000 + 2 RBU-1000; Frunze/Kalinin: 1 × 2 AK-130, Udav-1, Vodopad-NK; **10 × 533 mm** | **Polinom** (“Horse Jaw”) + VDS (“Horse Tail”) | **3** Ka-25/Ka-27 | sì | `[WPKirov]` media |
| **Krivak II (Pr.1135M)** | 3.300 / 3.575 t | 123,5 × 14,1 × 4,6 m | 32 kn | COGAG, 2 assi | ~200 | 1 × 4 **Metel/Rastrub (SS-N-14)**; 2 Osa-M (40); **2 × 1 AK-100 100 mm**; 2 RBU-6000; **2 × 4 tubi 533 mm**; mine | MG-332 Titan-2 (Bull Nose) + MG-325 Vega VDS (Mare Tail) — WP EN indica Zvezda/Bronza (probabile confusione con versioni successive) | — | sì | `[WPKrivak]` media |
| **Slava (Pr.1164)** | 9.380 / 11.490 t | 186,4 × 20,8 × 8,4 m | 32 kn | COGOG 130.000 shp | ~485 | **16 P-500 Bazalt (SS-N-12)** (Vulkan in seguito); 64 S-300F; 40 Osa-M; 1 × 2 AK-130; 6 AK-630; 2 RBU-6000; **2 × 5 tubi 533 mm** | MG-332 Titan-2T (scafo) + VDS (WP: “Platina”, Horse Tail) | 1 Ka-25/Ka-27 | sì (Slava 1982, Ustinov 1986, Chervona Ukraina 1989) | `[WPSlava]` media |
| **Koni (Pr.1159)** | 1.140 / 1.900 t | 95 × 12,8 × 5 m | 27 kn | CODAG, 3 assi | 110 | 1 Osa-M (20); 2 × 2 AK-726 76 mm; 2 × 2 AK-230; **2 RBU-6000**; scarico bombe di profondità; 20 mine | **MG-322 Herkules** (scafo) | — | marginale (export) | `[WPKoni]` media |
| **Nanuchka I (Pr.1234)** | 560 / 660 t | 59,3 × 12,6 × 2,7 m | 32–35 kn | 3 diesel, 30.000 hp | ~60 | **6 P-120 Malakhit (SS-N-9)** ~110–120 km; Osa-M (20); 2 × 57 mm AK-257 | **nessuno** | — | sì | `[WPNanu]` media |

### 4.3 Kirov — differenze per unità (importante per il 1989)
- **Kirov** (1980): SS-N-14 Metel, 2 × 100 mm AK-100, RBU-6000 + 2 RBU-1000, 8 AK-630.
- **Frunze** (1984): AK-130 binato al posto dei 100 mm; Metel sostituito da Vodopad-NK (SS-N-16) dai tubi 533; Udav-1 al posto della RBU-6000; Kinzhal (SA-N-9) previsto ma non operativo.
- **Kalinin** (1988): come Frunze + Kashtan (CADS-N-1) al posto degli AK-630.
Fonte: `[WPKirov]` (parziale) + `[PGSN]` 5ª ed. *n.v.* — media.

### 4.4 Mercantili generici
Mercantile cargo 10.000–20.000 tsl, 14–18 kn; petroliera 50.000–250.000 tpl, 13–16 kn. Nessun armamento/sensore ASW. Rumore 175–190 dB (vedi §3.2). Valori generici, confidenza media.

---

## 5. Aerei ed elicotteri ASW (sezione approfondita)

### 5.1 Ka-25PL *Hormone-A* (in gioco)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Equipaggio | 2 (pilota + navigatore-operatore sonar/boe); Western: 3–4 con operatori | `[RWKa25]` | media |
| Velocità | max 220 km/h (119 kn); crociera 185 km/h (100 kn) | `[RWKa25]` | alta |
| Raggio / autonomia | raggio di combattimento **200 km**; autonomia **~2 h** con carico ASW; percorso pratico 450 km | `[RWKa25]` | media |
| Pesi | vuoto 4.765 kg; decollo normale 6.970 kg; max 7.140 kg; carico ASW normale 650 kg (max 1.100–1.300) | `[RWKa25]` | media |
| Sonar a immersione | **VGS-2 “Oka”**, cavo **125 m** (verricello LPG-6), portata **≤ 6 km** in ogni modo | `[RWKa25]` | media |
| Boe | **RGB-N “Iva”**, RGB-NM “Chinara”, RGB-NM1 “Zheton”; fino a **36** in contenitore; sgancio ≤ 200 m quota; ~20 min di trasmissione; ricezione 0,3–25 km | `[RWKa25]` | media |
| MAD | **APM-60 “Orsha”** (alternativo al VGS-2), poi APM-73 | `[RWKa25]` | media |
| Armi | Siluro **AT-1/AT-1M** (450 mm, 550–560 kg, testata 70 kg, 27 kn × 5 km, acquisizione 500 m, quota 20–200 m); APR-2 (versioni successive); bombe PLAB-250-120 (fino a 4) o PLAB-50 (fino a 8); bomba di profondità nucleare | `[RWKa25]` `[NWRU]` | media |
| Radar | Initsiativa (“Short Horn”) *n.v.* | *n.v.* | bassa |

### 5.2 Ka-27PL *Helix-A* (alternativa d'epoca raccomandata; dal 1982)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Equipaggio | 3 | `[RWKa27]` | alta |
| Velocità | max 291 km/h; crociera economica 118 km/h (per la ricerca) | `[RWKa27]` | media |
| Raggio | 200 km con **~1,4 h** in zona | `[RWKa27]` | media |
| Sistema | **“Osminog”**: idroacustica + radar Initsiativa-2KM + navigazione/tattica + calcolatore (150.000 op/s) | `[RWKa27]` | media |
| Sonar a immersione | **VGS-3** — RU wiki: “Ros'-V”; fonti occidentali: “Ugra” (discrepanza nominale); 7 modi (passivo, attivo, inseguimento auto/semiauto…); portata vs Pr.613 a 6–8 kn, 100 m, mare 2: **~8 km** | `[RWKa27]` | media |
| Boe | **36** RGB-N / RGB-NM-1 | `[RWKa27]` | media |
| MAD | APM-73V | `[RWKa27]` | media |
| Armi | 1 APR-2 “Yastreb-M” o 1 UMGT-1 / AT-1M; o 8 PLAB-250-120; marker OMAB | `[RWKa27]` | media |
| Prestazioni ricerca | bersagli fino a 500 m di profondità e 75 km/h (40 kn); ≤ 200 km dalla nave; mare forza 5 | `[RWKa27]` | media |

### 5.3 SH-60B *Seahawk* LAMPS III (in gioco)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Servizio | primo volo produzione 1983, IOC 1984, primo impiego 1985 | `[WPSH60]` | alta |
| Equipaggio | 3: pilota, ufficiale tattico (ATO/copilota), operatore sensori (SO) | `[FASSH60]` | alta |
| Velocità | max 146 kn (FAS dash 160 kn; tabella 126 kn al livello del mare — incoerente) | `[WPSH60]` `[FASSH60]` | media |
| Autonomia | **~4 h** di missione; percorso 450 nmi | `[FASSH60]` | media |
| Tempo in zona | non pubblicato da FAS; requisito LAMPS III comunemente citato: **~1–3 h a 50 nmi** dalla nave a seconda del carico *n.v.* | *n.v.* | bassa |
| Sonar a immersione | **Nessuno** (lo avrà l'SH-60F con AQS-13F) | `[WPAQS13]` | alta |
| Boe | lanciatore pneumatico **25 tubi** (lato sinistro): SSQ-41B, SSQ-53B DIFAR, SSQ-62 DICASS, SSQ-36 BT | `[WPSH60]` `[FASSSQ53]` | alta |
| Elaborazione | i segnali delle boe vengono **rilanciati via data link** (ARQ-44 sull'elicottero ↔ SRQ-4 sulla nave) ed elaborati a bordo nave (SQQ-28 nell'SQQ-89) — l'elicottero è un “sensore remoto” della nave | `[FASSH60]` (“secure high-speed data link”) + *n.v.* per sigle | media |
| MAD | AN/ASQ-81(V)2 rimorchiato | `[WPSH60]` (MAD rimorchiato) *n.v.* sigla | media |
| Radar / ESM | AN/APS-124; AN/ALQ-142 | `[WPSH60]` | alta |
| Armi 1989 | **2 × Mk46 Mod 5**; (Mk50 solo dagli anni '90; Penguin dal 1993 → anacronistico nel 1989) | `[FASSH60]` `[WPSH60]` | alta |
| MTOW | 9.926 kg | `[WPSH60]` | alta |

### 5.4 SH-3H *Sea King* (alternativa d'epoca: elicottero ASW di zona interna delle portaerei)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Equipaggio | 4 (2 piloti + 2 operatori) | `[WPSH3]` | alta |
| Velocità | max 144 kn; crociera 118 kn (dati SH-3D) | `[WPSH3]` | media |
| Autonomia | requisito ~4 h; percorso 542 nmi | `[WPSH3]` | media |
| Sonar a immersione | **AN/AQS-13B** (dal tardo '70 su SH-3H; 13E con FFT inizio anni '80). Attivo, fascia alta delle medie frequenze, rudimentale passivo, batitermografo. Frequenze “9,25/10/10,75 kHz” e cavo 450 ft **non confermati** | `[WPAQS13]` | media (qualitativo) / bassa (numeri) |
| Boe / MAD | ricevitori ARR-75; **ASQ-81** MAD | `[WPSH3]` | media |
| Armi | 2 × Mk46; carico max 380 kg; B57 nucleare | `[WPSH3]` | media |

**Ciclo di immersione (dip cycle) — modello di gioco** *(stime, bassa)*: avvicinamento → hover a 15–20 m → calata del trasduttore (≈ 3–5 m/s) a quota scelta sopra/sotto lo strato → ascolto passivo 1–2 min + 3–6 ping → recupero (~1 min) → trasferimento 2–5 nmi → nuovo dip. **Ciclo totale 6–12 min**. Portata attiva tipica 3–8 km (Ka-25 ≤ 6 km `[RWKa25]`; Ka-27 ~8 km `[RWKa27]`). La calata può essere sentita dal sottomarino (transitorio + ping).

### 5.5 P-3C *Orion* (Update II/III)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Velocità | max 411 kn; crociera 328–330 kn; pattugliamento ~200 kn bassa quota *n.v.* | `[FASP3]` | alta |
| Autonomia | 10–13 h (12 h 20' su 4 motori; 17 h su 2) | `[FASP3]` | alta |
| Raggio | **845 nmi con 3 h in zona** a 457 m; 1.345 nmi senza tempo in zona | `[FASP3]` | alta |
| Boe | **fino a 84–87** (48 tubi esterni + scorta interna; FAS: 87 nel carico max) | `[FASP3]` | alta |
| Elaborazione | Update II (1977): Harpoon, IRDS, riferimento boe; Update III (1985–86): processore **AN/UYS-1 (SASP)**, raddoppio canali boe | `[FASP3]` | alta |
| MAD | AN/ASQ-81 in coda *n.v.* sigla | `[FASP3]` | media |
| Radar / ESM | APS-115 (Update I/II) *n.v.*; ALR-66 (Update III) | `[FASP3]` | media |
| Armi | **8 Mk46** in vano + piloni; **Harpoon AGM-84** (Update II); bombe di profondità Mk54/Mk57 (Mk57 = B57 nucleare) / Mk101; mine | `[FASP3]` | alta |
| Equipaggio | 11 (min. 5) | `[FASP3]` | alta |

**Tattica tipica:** barriera/linea di **SSQ-41/53** con spaziatura 2–8 nmi in funzione della portata stimata (dipende dal rumore del bersaglio, §3); localizzazione con incroci DIFAR; conferma con **DICASS** attiva; passaggio **MAD** (portata pratica 300–700 m in obliquo, dipende dal momento magnetico e dalla profondità del bersaglio — **stima, bassa**); attacco con Mk46 sul punto MAD; marker Mk25 (fumogeno/fiamma) per marcare il *datum*.

### 5.6 Il-38 *May* e Tu-142M/MK *Bear-F*

| Parametro | Il-38 | Tu-142M/MK | Fonte | Conf. |
|---|---|---|---|---|
| Sistema | **Berkut-38** (radar panoramico + ricevitore boe + calcolatore Plamya-264) | **Korshun** (calcolatore Argon-15; elaborazione Kaira → Zarechye) | `[RWIl38]` `[RWTu142]` | media |
| Boe | **144 RGB-1** (passiva omni) + **10 RGB-2** (passiva direzionale) + **3 RGB-3** (attivo-passiva) | RGB-1 (22/cassetta, 14 kg), RGB-2 (10/cassetta, 40 kg); sorgenti esplosive (eco) | `[RWIl38]` `[RWTu142]` | media |
| MAD | APM-60 o APM-73 | MMS-106 “Ladoga” (in deriva; disturbi elettrostatici) | idem | media |
| Armi | 2 siluri AT-1/AT-2 o APR-1/APR-2; PLAB-250-120, PLAB-50; mine; carico max 8,4 t | siluri, bombe, boe, mine in 2 vani | idem | media |
| Prestazioni | max 650 km/h; raggio 2.200 km; percorso 9.500 km; equip. 7 | raggio ~2× Il-38 (obiettivo di progetto); autonomia ~15–17 h *n.v.* | idem | media / bassa |

### 5.7 Tu-22M2/M3 *Backfire* (minaccia antinave, non ASW)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Varianti 1989 | Tu-22M2 e Tu-22M3 (dal 1983) | `[WPTu22M]` | alta |
| Velocità | M2: Mach 1,65; M3: ~Mach 1,88 (2.000 km/h a 9.140 m) | `[WPTu22M]` | media |
| Raggio | 2.500 km con 10 t; raggio di combattimento CIA 3.360–3.960 km / DIA 4.000–5.000 km (stime della guerra fredda) | `[WPTu22M]` | media |
| Armi | **1–3 Kh-22 (AS-4 Kitchen)**: ~400–600 km, Mach 3+, testata ~1.000 kg o nucleare *n.v.* | `[WPTu22M]` / *n.v.* | media / bassa |
| Radar / difesa | PNA “Down Beat”; 1 GSh-23 in coda; equip. 4 | `[WPTu22M]` | alta |

### 5.8 Boe acustiche (sonobuoy)

| Boa | Tipo | Parametri chiave | Fonte | Conf. |
|---|---|---|---|---|
| AN/SSQ-41B | passiva omni (LOFAR) | prof. 60 o 300 ft (DTIC; versioni successive 60/1000 ft), vita 1/3/8 h | `[DTIC41]` | media |
| AN/SSQ-47B | attiva solo distanza | DTIC la descrive come variante dell'SSQ-41 per misure di riverbero → **ruolo operativo incerto**; molte fonti la danno come boa attiva a range | `[DTIC41]` | bassa |
| **AN/SSQ-53B DIFAR** | passiva direzionale | idrofono direzionale 10–2.400 Hz + omni; prof. 100/400/1.000 ft; vita 1/3/8 h; 99 canali; rilevamento magnetico (2 boe → fix) | `[FASSSQ53]` | alta |
| **AN/SSQ-62B DICASS** | attiva comandata | prof. iniziale 90 ft, su comando 400 o 1.500 ft; **4 frequenze 6,5/7,5/8,5/9,5 kHz**; distanza + rilevamento + Doppler | `[FASSSQ62]` `[DICASS]` | alta |
| AN/SSQ-36 | batitermografo | profilo di temperatura (strato) | `[FASSH60]` (generico) | media |
| RGB-N / RGB-NM / RGB-NM-1 | passive (elicotteri) | ~20 min di trasmissione; ricezione 0,3–25 km | `[RWKa25]` | media |
| RGB-1 / RGB-2 / RGB-3 | passiva omni / passiva direz. / attivo-passiva (aerei) | pesi 14 / 40 kg | `[RWTu142]` `[RWIl38]` | media |

### 5.9 Coordinamento nave–elicottero (per l'IA)

- **USA (LAMPS III):** la nave (SQS-53/56 + **SQR-19**) rileva a lunga distanza/CZ → l'SH-60B vola sul rilevamento, posa un **pattern di 4–8 DIFAR** attorno al *datum* → i dati vanno in data link alla nave (SQQ-28) che calcola la soluzione → **DICASS** per la distanza → passaggio **MAD** → attacco **Mk46** (l'ordine di lancio può venire dalla nave). Raggio operativo tipico 30–50 nmi dalla nave (portata del data link a vista radio). *Stima strutturale: media; numeri: bassa.*
- **URSS:** Ka-25/27 operano in **coppia** dal Kara/Kirov/Slava (o da gruppi di caccia), con prevalenza di **sonar a immersione attivo** in sequenze di dip lungo il rilevamento fornito dalla nave; boe RGB-N per barriere rapide; attacco con AT-1/APR-2 o bombe PLAB; la nave attacca a distanza con Metel/Vodopad-NK o, vicino, con RBU-6000. *(media/bassa)*

---

## 6. Armi

### 6.1 Siluri pesanti (533 / 650 mm)

| Siluro | Paese / IOC | Velocità – portata | Prof. max | Testata | Guida | 1989 | Fonte | Conf. |
|---|---|---|---|---|---|---|---|---|
| **Mk48 Mod 1/3/4** | USA 1972 (Mod 4 prod. dal 1980) | ufficiale “>28 kn, >5 mi”; stime: **55 kn / ~38 km (24 mi)**, **40 kn / ~50 km (31 mi)** (Almanac of Seapower 1987, Jane's) | ufficiale >1.200 ft; stime **≈ 760–800 m** | 293 kg HE, spoletta di prossimità | filo + attivo/passivo; Mod 4 “fire-and-forget” | sì | `[WPMk48]` `[NWUS]` `[FASMk48]` | media |
| **Mk48 ADCAP (Mod 5)** | USA, prod. 1985, **IOC 1988** | stime **55–65 kn**; ~27–38 km a 55–65 kn; ~50 km a 40 kn | stime ≥ 800 m | ~295 kg PBXN-103 | filo + attivo/passivo digitale | **marginale** | `[WPMk48]` `[NWUS]` `[HIS]` | media |
| Mk 37 / NT37 | USA 1957/1967 | 26 kn / 9,1 km; 17 kn / 21,5 km (NT37: 36 kn / 13,7 km) | — | 150 kg | filo (Mod 1/2), attivo/passivo, acquisizione ~900 m | sì (alleati) | `[NWUS]` | alta |
| **Tigerfish Mk24 Mod 1/2** | UK 1980/1987 | **35 kn / 13–21 km**; **24 kn / ~27–39 km** | 351 m (Mod 0), **442 m (Mod 1/2)** | 134–340 kg Torpex | filo → passivo (attivo opzionale) | sì | `[WPTiger]` `[NWBR]` | media |
| **Spearfish** | UK, prod. 1988, **servizio 1992–94** | “60+ kn / ~21 km” (versione 1981); “80 kn” riportati; gittata max ~56 km (30 nmi) a bassa velocità | n.d. (velocità calante in profondità) | 300 kg PBX alluminizzato | filo + attivo/passivo | **no** | `[WPSpear]` `[NWBR]` | media |
| SET-65 | URSS 1965 | **40 kn / 15–16 km** | **20–400 m** | 205–240 kg | acustica attiva (acquisiz. ~800 m) | sì | `[RWSET65]` `[NWRU]` | media |
| **53-65K** | URSS 1969 | **45 kn / 19 km** | n.d. | 300 kg | **segui-scia** (antinave) | sì | `[WPType53]` `[NWRU]` | alta |
| 53-65M | URSS 1969 | 44 kn / 22 km | n.d. | 300 kg | segui-scia | sì | `[NWRU]` | alta |
| SAET-60M | URSS 1969 | 40 kn / 15 km | n.d. | 300 kg | passiva antinave | sì | `[NWRU]` | media |
| **TEST-71 / 71M** | URSS 1971 / ~1977 | **40 kn / 15 km** (o 35 kn attacco / 24 kn × 15 km per RU wiki); 71MKE: 35 kn / 25 km | **400 m** | 205 kg | **filo (KTU-71) + attivo/passivo “Sapfir”, acquisizione ≤ 800 m** | sì | `[RWTEST71]` `[NWRU]` | media |
| **USET-80** | URSS 1980 | **45 kn / 18 km** (43 kn) ; navweaps 40–50 kn / 20 km | **1.000 m** (RU) / >400 m (navweaps) | 200–300 kg | attivo/passivo + **segui-scia** (bolle) | sì | `[RWUSET80]` `[NWRU]` | media |
| **65-76 “Kit”** (650 mm) | URSS 1976 | **50 kn / 50 km**; 30–35 kn / 100 km | **480 m** (lancio fino a 480 m) | 450–557 kg (≈765 kg TNT eq.) o nucleare | **segui-scia** (antinave) | sì | `[RW6576]` `[NWRU]` | media |
| 65-73 (650 mm) | URSS 1973 | 50 kn / 50 km | — | **nucleare 20 kt**, non guidato | — | sì (nucleare) | `[NWRU]` `[WPType65]` | media |

**Note di gioco su siluri:**
- **Filo:** lunghezza non pubblicata; modellare rottura del filo per velocità relativa/manovra del lanciatore e per tempo. Il Tigerfish Mod 0 perdeva il filo spesso al lancio (solo 40% di funzionamento) `[WPTiger]`.
- **Segui-scia (53-65K, 65-76, USET-80):** inutile contro sottomarini immersi; immune a esche acustiche; si sconfigge manovrando la scia (“wake”) e cambiando rotta tardi.
- **Frequenze dei seeker:** non pubbliche. Ordine di grandezza in letteratura: siluri pesanti ~20–40 kHz, leggeri ~25–35 kHz (Mk46 ~30 kHz) — **bassa**.

### 6.2 Siluri leggeri / ASW (324–450 mm)

| Siluro | IOC | Velocità – portata | Prof. | Testata | Guida | 1989 | Fonte | Conf. |
|---|---|---|---|---|---|---|---|---|
| **Mk46 Mod 5** | USA 1979/1984 | ufficiale >28 kn / 8.000 yd; reale **45 kn / ~11 km** (11.400–12.000 yd) | ufficiale >365 m; stima **450 m** (1.500 ft) | 44 kg PBXN-103 | attivo o passivo/attivo; ricerca a serpentina o cerchio | sì | `[FASMk46]` `[NWUS]` | alta |
| **Mk50 Barracuda** | USA, prod. ~1991–92 | >40 kn / ~15 km (16.000 yd) | >580 m (1.900 ft) | 45 kg carica cava | attivo/passivo, propulsione SCEPS a ciclo chiuso | **no** | `[WPMk50]` `[NWUS]` | media |
| Stingray | UK 1986 | 45 kn / 8 km | ~800 m (dichiarato “pari all'Alfa”) | 45 kg carica cava | attivo | sì | `[NWBR]` | media |
| SET-40 (MGT-2) | URSS 1962 (400 mm) | 29 kn / 8 km | n.d. | 80 kg | attivo/passivo, acquisiz. 600–800 m | sì (obsoleto) | `[NWRU]` | media |
| SET-72 | URSS 1972 (400 mm) | 40+ kn / 8 km | n.d. | 60–100 kg (stima) | attivo/passivo | sì | `[NWRU]` | media |
| AT-1 / AT-1M | URSS 1962 (450 mm, aerei/elicotteri) | **27 kn / 5 km** | 20–200 m | 70 kg | attivo/passivo, acquisiz. 500–1.000 m | sì | `[RWKa25]` `[NWRU]` | media |
| AT-2 / AT-2U | URSS 1965/1973 (533 mm, aerei; carico di Metel) | 40 kn / 7 km | n.d. | 80–150 kg | attivo/passivo, acquisiz. ~1.000 m | sì | `[NWRU]` | media |
| UMGT-1 “Orlan” | URSS ~1981 (400 mm; carico di Vodopad/Veter e Ka-27) | 41 kn / 8 km | 500 m | 60 kg | attivo/passivo, acquisiz. ~1.500 m | sì | `[NWRU]` | media |
| APR-2 “Yastreb” | URSS anni '80 (350 mm, razzo-siluro) | 62 kn / 1,5–2 km (fase subacquea) | 15–600 m | 80–100 kg | attivo/passivo, acquisiz. ~1.500 m; Pk dichiarata 70–85% | sì | `[RWKa25]` `[NWRU]` | media |

### 6.3 Missili antisommergibile (stand-off)

| Arma | IOC | Portata | Carico | Lancio | 1989 | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| **UUM-44 SUBROC** | USA 1965 | ~55 km (≈30 nmi) | W55 nucleare (resa classificata; stime 1–5 kt fino a 250 kt) | tubo 533, balistico inerziale | **marginale** (ritiro 1989–90) | `[WPSUBROC]` | alta |
| UUM-125 Sea Lance | — | (progetto ~100+ nmi) | Mk50 o nucleare | tubo 533 | **no** (cancellato 1990) | `[WPSUBROC]` | alta |
| ASROC RUR-5 | USA 1961 | ~1–9 km *n.v.* | Mk46 (o W44 nucl., ritirata) | Mk16/Mk26 navi | sì | *n.v.* | media |
| **RPK-2 Vyuga (SS-N-15)** | URSS 1969 | ~10–40 km (circa metà del Veter) | nucleare di profondità | tubo 533 | sì | `[WPRPK6]` | media |
| **RPK-6 Vodopad (SS-N-16)** | URSS 1981 | ~50 km *n.v.* | UMGT-1 o nucleare | tubo 533 (sub) / Vodopad-NK (navi) | sì | `[WPRPK6]` | media |
| **RPK-7 Veter (SS-N-16)** | URSS 1984 | **100 km** | UMGT-1 / nucleare | tubo 650 | sì | `[WPRPK6]` | media |
| **Metel / Rastrub (SS-N-14)** | URSS 1973 | **~50 km** | siluro AT-2U | navi (Kara, Krivak, Kirov) | sì | `[WPKara]` | media |

### 6.4 Missili antinave e d'attacco

| Missile | IOC | Portata | Velocità | Testata | Guida | Piattaforme | Fonte | Conf. |
|---|---|---|---|---|---|---|---|---|
| **UGM-84A Harpoon** | 1981 | ~110–120 km (60–70 nmi) | Mach 0,85 | 221 kg | inerziale + radar attivo | 688, Sturgeon, Trafalgar (capsula da tubo) | *n.v.* `[FRWNWS]` | media |
| **BGM/UGM-109B TASM** | 1983 | **~460 km (250 nmi)** | ~Mach 0,7 | 454 kg | inerziale + radar attivo | 688, Sturgeon, Iowa | *n.v.* | media |
| **TLAM-C (109C)** | 1986 | ~1.250 km | ~Mach 0,7 | 454 kg | TERCOM + DSMAC | 688, Iowa | *n.v.* | media |
| TLAM-N (109A) | 1984 | ~2.500 km | ~Mach 0,7 | W80 nucleare | TERCOM | 688 | *n.v.* | media |
| **P-700 Granit (SS-N-19)** | 1983 | **~550 km** (stime 400–625) | Mach 1,6 basso / 2,5 alto | **750 kg** HE o nucleare 500 kt | inerziale + radar attivo + anti-radiazione; salve coordinate (dubbie) | Oscar, Kirov | `[WPGranit]` | media |
| P-500 Bazalt (SS-N-12) | 1975 | ~550 km | Mach 2,5 | ~1.000 kg | inerziale + radar attivo | Slava | *n.v.* | media |
| **P-120 Malakhit (SS-N-9)** | 1972 | **~110–120 km** | Mach 0,9 | ~500 kg o nucleare | radar attivo/IR | Nanuchka, Charlie II | *n.v.* | media |
| **S-10 Granat (SS-N-21)** | 1984–87 | ~2.500–3.000 km | 720 km/h | nucleare 200 kt | inerziale + TERCOM; tubo 533 | Akula, Victor III (RTMK), Sierra | `[WPRK55]` | media (bassa la parte citazioni) |
| Kh-22 (AS-4) | anni '70 | ~400–600 km | Mach 3+ | ~1.000 kg o nucleare | radar attivo/passivo | Tu-22M | *n.v.* | bassa |

### 6.5 Armi ASW a corto raggio

| Arma | Dati | Fonte | Conf. |
|---|---|---|---|
| **RBU-6000 “Smerch-2”** (1961) | 12 canne 212 mm; razzo **RGB-60** 113,6 kg; portata **≤ 5.230 m** (min ~300 m); affondamento **11,6 m/s**; prof. bersaglio **≤ 450 m**; 2,4 salve/min; ricarica automatica dal basso; controllo tiro Burya | `[RWRBU6000]` | alta |
| **RBU-1000 “Smerch-3”** (1961) | 6 canne 300 mm; **RGB-10** 196 kg (HE ~80–100 kg *n.v.*); portata ~100–1.000 m *n.v.*; affondamento **11,8 m/s**; prof. ≤ 450 m; 2 colpi/min | `[RWRBU1000]` | alta / bassa (portata) |
| Bombe di profondità aeree | PLAB-250-120, PLAB-50 (URSS); Mk54 (USA, convenzionale); Mk57/B57 (nucleare) | `[RWKa25]` `[FASP3]` | media |
| Scarichi di poppa (Koni) | bombe di profondità convenzionali (nel gioco: ~12, raggio letale ~20–30 m — stima) | `[WPKoni]` / stima | bassa |

### 6.6 Contromisure acustiche (esche / noisemaker)

| Contromisura | Dati | 1989 | Fonte | Conf. |
|---|---|---|---|---|
| **ADC Mk2** (USA) | **Ø 3"**, espendibile; da lanciatore interno (ICL) dei sottomarini o da navi; **staziona verticalmente** a quota preselezionata con motore + eliche intubate e emette segnali per ingannare i siluri; batteria termica al litio. Dettagli **classificati** | sì (anni '80) *n.v.* data | `[ADC]` | media |
| **ADC Mk3** (USA) | stivata tra scafo resistente ed esterno; lancio con generatore di gas; dati di prestazione non pubblici | marginale *n.v.* | `[ADC]` | bassa |
| Mk70 MOSS (USA) | esca mobile simulatore di sottomarino lanciata da tubo (basata su corpo Mk37), anni '70–'80 | sì | *n.v.* `[FR94]` | bassa |
| **MG-34** (URSS) | dispositivo di disturbo idroacustico espendibile (noisemaker) — **dati pubblici non trovati** | sì | — | bassa |
| **MG-74 “Korund”** (URSS) | imitatore semovente di sottomarino — **dati pubblici non trovati** | sì | — | bassa |

**Modello di gioco suggerito:** noisemaker statico (Mk2/MG-34): rumore a larga banda 15–30 dB sopra il battello a velocità silenziosa per 5–15 min, efficace contro seeker passivi e come “falsa eco” per seeker attivi entro ~1–2 km; esca mobile (MOSS/MG-74): profilo di rumore del battello a 8–12 kn per 10–20 min. Nessun effetto su siluri segui-scia. *(stime, bassa)*

### 6.7 “Sonar target marker”
Nel DB del gioco originale è con ogni probabilità un **marcatore di contatto/datum** (UI). Equivalenti reali: **Mk25 Marine Location Marker** (fumogeno/fiamma, aerei USA) e **OMAB-25** (giorno/notte, aerei sovietici) `[RWKa27]`. Proposta: mantenerlo come funzione UI + marker fisico solo per gli aerei.

---

## 7. Sensori

### 7.1 Sottomarini USA/UK

| Sensore | Descrizione | Dati | Fonte | Conf. |
|---|---|---|---|---|
| **AN/BQQ-5** (IBM) | suite integrata: sfera prodiera BQS-13 (attivo/passivo LF), array conforme di scafo, gestione degli array rimorchiati | >1.000 idrofoni nella sfera; su SSN-637 e SSN-688 | `[FASBQQ5]` `[FAS688]` | alta |
| **AN/BQS-15** | attivo ad alta frequenza a corto raggio (ostacoli, ghiaccio, mine) | portata qualche km *stima* | `[FAS688]` | media |
| **AN/BSY-1** | sistema di combattimento integrato del 688I (sensori BQQ-5E) | solo SSN-751+ | `[WP688]` | alta (marginale 1989) |
| **TB-16** | array rimorchiato a filo grosso | **~73 m (240 ft), Ø 8,9 cm (3,5")**, cavo **730–790 m (2.400–2.600 ft)**, ~50 idrofoni; banda non pubblicata | `[FASTB16]` + forum che cita `[FR94]` | media |
| **TB-23** | array “thin-line” | **~290 m (~960 ft)**; banda non pubblicata (più bassa del TB-16) | forum che cita `[FR94]` | bassa |
| Mk 117 FCS | direzione del tiro digitale (Mk48, Harpoon, Tomahawk) | — | `[FR94]` *n.v.* | media |
| Periscopi | **Type 18** (ricerca: ingrandimenti 1,5×/6×/12×/24× *n.v.*, TV, ESM, sestante) — **Type 2** (attacco, testa piccola, 1,5×/6×) | quota periscopica ~18 m (60 ft) chiglia *stima* | `[FAS688]` | alta (esistenza) / bassa (numeri) |
| Radar | AN/BPS-15 (navigazione/superficie, banda X) | — | `[FAS688]` | alta |
| ESM | **AN/WLR-8(V)** (superet. 0,5–18 GHz *n.v.*); **AN/WLQ-4 “Sea Nymph”** (ESM avanzato su Sturgeon per missioni speciali) *n.v.*; **WLR-9** intercettatore acustico (ping/siluri) | — | `[FAS688]` + *n.v.* | media |
| Trafalgar | Type 2020, 2007, 2024/2026, 2019 PARIS, 2077 | — | *n.v.* | media |

### 7.2 Sottomarini sovietici

| Sensore | Piattaforme (1989) | Dati | Fonte | Conf. |
|---|---|---|---|---|
| **MGK-400 Rubikon** (“Shark Gill”) | Kilo; 2ª gen. aggiornate | base di confronto per Skat | `[WPKilo]` `[RWSkat]` | alta |
| **MGK-500 Skat** (Skat-B/Skat-M) | 3ª gen. | antenna prodiera cilindrica + conformi laterali (Skat-B) | `[RWSkat]` | media |
| **MGK-503 Skat-KS** | **Victor III, Typhoon**, Sierra (2 unità) | + array rimorchiato flessibile; **portata 2–3× del Rubikon** | `[RWSkat]` | alta (relativo) |
| **MGK-540 Skat-3** | **Akula, Oscar I/II**, Sierra II | digitale; prua + 2 antenne laterali estese + rimorchiato | `[RWSkat]` | alta |
| **Pelamida** | array rimorchiato (pod sulla deriva di Akula/Victor III) | dati non trovati | — | bassa |
| Okean | Alfa | dati non trovati | — | bassa |

**Nota di gioco:** il DB del gioco attribuisce “MGK-500 Skat” all'Akula: per il 1989 è più corretto **MGK-540 Skat-3**; Victor III/Typhoon → **MGK-503 Skat-KS**.

### 7.3 Sonar navali sovietici

| Sensore | Tipo | Piattaforme | Dati | Fonte | Conf. |
|---|---|---|---|---|---|
| **MG-332 Titan-2** (Bull Nose) / Titan-2T | scafo, LF/MF | Kara, Krivak, Slava | — | `[WPKara]` `[WPSlava]` | media |
| **MG-325 Vega** (Mare Tail) | VDS | Kara, Krivak | quota rimorchio **fino a 100 m** | `[WPKara]` | media |
| **Polinom** (MG-355, Horse Jaw) | scafo, LF, grande | Kirov, Udaloy | “1ª e 2ª zona lontana” (CZ) — nessun numero | `[WPKirov]` + www1.ru | media |
| **Platina** (MGK-335) | MF | WP la indica come VDS dello Slava (discordante) | — | `[WPSlava]` | bassa |
| **MG-322 Herkules** | scafo, MF | Koni | — | `[WPKoni]` | media |

Portate (stime per il gioco, bassa): Titan-2/Polinom direct path 8–15 km, Polinom in CZ ~30–60 km in acque profonde; Vega VDS sotto lo strato 5–12 km; Herkules 2–5 km.

### 7.4 Radar e ESM di superficie (sintesi)
USA: SPS-49 (ricerca aerea 2D, ~250 nmi), SPS-48E (3D), SPS-55/SPS-67 (superficie), SPG-60/SPQ-9 (tiro). URSS: MR-600 Voskhod (Top Sail), MR-800 Voskhod (Top Pair), MR-700/710 Fregat (Top Steer/Top Plate), MR-310 Angara (Head Net), Volna (Top Dome, SA-N-6). Fonti: `[WPKara]` `[WPSlava]`; resto *n.v.* — media. Per la simulazione conta soprattutto la capacità di rilevare un **periscopio/albero** esposto: tipicamente 5–15 km con radar di superficie in banda X a mare calmo (stima, bassa).

---

## 8. Tabelle riassuntive

### 8.1 Sottomarini (valori raccomandati per il gioco)

| Classe | 1989 | Disl. imm. (t) | Vmax (kn) | V silenziosa (kn) | Test / collasso (m) | Tubi | Armi | Sonar | SL @ silenziosa (dB, stima) |
|---|---|---|---|---|---|---|---|---|---|
| 688 Flight I | sì | 6.927 | 32 | 5 | 290 / 450 | 4 × 533 | ~22–26 | BQQ-5, TB-16 | 112 |
| 688I | marginale | 7.150 | 32 | 5 | 290 / 450 | 4 × 533 + 12 VLS | 26 + 12 | BSY-1 | 107 |
| Sturgeon | sì | 4.714 | 26 | 5 | 400 / 600 | 4 × 533 | 23 | BQQ-5 | 117 |
| Trafalgar | sì (5) | 5.300 | 32 | 6 | 300 / 500 (stima) | 5 × 533 | 25–30 | 2020, 2026 | 110 |
| Alfa | marginale | 3.180 | 41 | 6 | 350 / 400 | 6 × 533 | 18–20 | Okean | 128 |
| Victor III | sì | 7.250 | 30 | 5 | 400 / 600 | 4 × 533 + 2 × 650 | 24 | Skat-KS | 122 |
| Akula I | sì (~5) | 12.770 | 33 | 6 | 520 / 600 | 4 × 533 + 4 × 650 | 40 | Skat-3 | 112 |
| Oscar I/II | sì | 19.400 | 32 | 5 | 520 / 600 | 4 × 533 + 2 × 650 | 28 + 24 SS-N-19 | Skat-3 | 120 |
| Charlie II | sì | 5.500 | 24 | 4 | 300 / 400 (stima) | 4 × 533 + 2 × 400 | 12 + 8 SS-N-9 | — | 132 |
| Typhoon | sì | 48.000 | 25–27 | 6 | 400 / 500 | 6 × 533 | 22 + 20 SLBM | Skat-KS | 125 |
| Kilo | sì | 3.075 | 20 | 3 (batteria) | 240 / 300 | 6 × 533 | 18 | Rubikon | 114 |
| Foxtrot | sì | 2.515 | 15 | 2–3 (batteria) | 250 / 300 | 10 × 533 | 22 | Arktika-M | 122 |

### 8.2 Navi di superficie
| Classe | 1989 | Disl. pieno (t) | Vmax | Sonar | ASW | Elicotteri |
|---|---|---|---|---|---|---|
| Forrest Sherman | **no** | 4.100 | 32,5 | SQS-23 | ASROC, Mk32 | — |
| Kidd | sì | 9.570 | 33 | SQS-53A | ASROC, Mk32 | 1 |
| O.H. Perry | sì | 4.100 | 29 | SQS-56 (+SQR-19) | Mk32 | 2 SH-60B |
| Nimitz | sì | ~95.000 | 30+ | — | (gruppo aereo) | ~10 S-3/SH-3 |
| Iowa | sì | 57.500 | 33 | — | — | — |
| Sacramento | sì | 53.600 | 26 | — | — | 2 UH-46 |
| Kara | sì | 9.700 | 34 | Titan-2 + Vega | Metel, RBU-6000/1000, 10 tubi | 1 Ka-25 |
| Kirov | sì | ~24.300 | 32 | Polinom + VDS | Metel/Vodopad-NK, RBU, 10 tubi | 3 |
| Krivak II | sì | 3.575 | 32 | Titan-2 + Vega | Metel, RBU-6000, 8 tubi | — |
| Slava | sì | 11.490 | 32 | Titan-2T + VDS | RBU-6000, 10 tubi | 1 |
| Koni | marginale | 1.900 | 27 | Herkules | RBU-6000, bombe | — |
| Nanuchka | sì | 660 | 32 | — | — | — |

### 8.3 Aerei / elicotteri
| Velivolo | 1989 | V crociera | Autonomia / tempo in zona | Sensori ASW | Armi ASW |
|---|---|---|---|---|---|
| Ka-25PL | sì (declinante) | 100 kn | ~2 h; raggio 200 km | VGS-2 (≤6 km), 36 RGB-N, APM-60 | AT-1, PLAB |
| Ka-27PL | sì | 64 kn (ricerca) | 1,4 h a 200 km | VGS-3 (~8 km), 36 boe, APM-73 | APR-2/UMGT-1, PLAB |
| SH-60B | sì | ~120 kn | ~4 h | 25 boe, ASQ-81, APS-124, data link | 2 Mk46 |
| SH-3H | sì | 118 kn | ~4 h | AQS-13B/E, boe, ASQ-81 | 2 Mk46 |
| P-3C | sì | 330 kn (transito) | 3 h a 845 nmi | 84–87 boe, ASQ-81, IRDS | 8 Mk46, Harpoon, Mk54/57 |
| Il-38 | sì | ~400 kn | raggio 2.200 km | Berkut-38, 157 boe, APM-60/73 | 2 AT-1/AT-2, PLAB |
| Tu-142M/MK | sì | ~400 kn | ~15 h *n.v.* | Korshun, RGB-1/2, MMS-106 | siluri, bombe |
| Tu-22M2/M3 | sì | Mach 0,9 / dash 1,65–1,88 | raggio 2.000–4.000 km | PNA radar | 1–3 Kh-22 (antinave) |

### 8.4 Armi
| Arma | 1989 | Vel. (kn) | Portata | Prof. (m) | Guida |
|---|---|---|---|---|---|
| Mk48 Mod 4 | sì | 55 / 40 | 38 / 50 km | ~800 | filo + A/P |
| Mk48 ADCAP | marginale | 55–63 | 27–38 km (50 a 40 kn) | >800 | filo + A/P digitale |
| Mk46 Mod 5 | sì | 45 | 11 km | ~450 | A/P |
| Mk50 | **no** | 40+ | 15 km | 580+ | A/P |
| Tigerfish | sì | 35 / 24 | 13–21 / 27–39 km | 442 | filo + P (A opz.) |
| Spearfish | **no** | 60–80 | 21–56 km | — | filo + A/P |
| SET-65 | sì | 40 | 15 km | 400 | A |
| 53-65K | sì | 45 | 19 km | — | scia |
| TEST-71M | sì | 40 | 15 km | 400 | filo + A/P |
| USET-80 | sì | 45 | 18 km | 1.000 | A/P + scia |
| 65-76 | sì | 50 / 30–35 | 50 / 100 km | 480 | scia |
| SUBROC | marginale | — | 55 km | — | balistico (nucl.) |
| SS-N-15 / SS-N-16 | sì | — | ~40 / 100 km | — | inerziale |
| SS-N-19 | sì | M 1,6–2,5 | ~550 km | — | radar att. |
| Harpoon | sì | M 0,85 | ~120 km | — | radar att. |
| TASM | sì | M 0,7 | 460 km | — | radar att. |
| RBU-6000 | sì | 11,6 m/s affond. | 5,2 km | 450 | balistico |

---

## 9. Principali lacune di dati

1. **Rumore irradiato assoluto (dB) per classe e per velocità**: nessuna fonte aperta primaria per sottomarini USA/UK; per i sovietici solo Miasnikov (SSBN + Akula). La tabella §3.2 è una calibrazione relativa — **la lacuna più grande**. Le tabelle ONI (es. “Worldwide Submarine Challenges” 1996/97) mostrano solo curve relative senza scala assoluta.
2. **Frequenze e portate di acquisizione dei seeker** (Mk48, ADCAP, USET-80, TEST-71) e lunghezza dei fili: classificate.
3. **Profondità di collaudo** di Trafalgar, Charlie II, Typhoon (limite): solo stime.
4. **Manovrabilità** (raggio di virata, rateo di virata, velocità di variazione quota) per tutti i battelli: non pubblicata → da modellare fisicamente.
5. **Tempi di ricarica dei tubi**: non pubblicati.
6. **Sonar sovietici** (Skat, Polinom, Titan-2, Vega, Pelamida, Okean): solo dati qualitativi/relativi; nessuna portata numerica affidabile.
7. **Esche sovietiche MG-34 / MG-74** e ADC Mk3: praticamente nessun dato aperto.
8. **AQS-13** (frequenze, cavo) e **VGS-3** (nome “Ugra” vs “Ros'-V”): numeri non confermati.
9. **Dati da libri non riverificati** (*n.v.*): Polmar & Moore, Polmar *Guide to the Soviet Navy*, Friedman, Jane's — da controllare sul cartaceo con numeri di pagina (soprattutto navi USA §4.1, carichi d'armi, Kirov per unità, missili antinave).
10. **CIA CREST**: individuati NIE 11-15-82D e NIE 11-15-84/D (*Soviet Naval Strategy and Programs*), ma non è stato possibile estrarne il testo sui programmi di silenziamento — da consultare manualmente.

---

## 10. Bibliografia

### Fonti online consultate
- `[FAS688]` FAS, *SSN-688 Los Angeles-class*: https://man.fas.org/dod-101/sys/ship/ssn-688.htm
- `[WP688]` Wikipedia, *Los Angeles-class submarine*: https://en.wikipedia.org/wiki/Los_Angeles-class_submarine
- `[WPSturgeon]` https://en.wikipedia.org/wiki/Sturgeon-class_submarine
- `[NPSturgeon]` https://navypedia.org/ships/usa/us_ss_sturgeon.htm (i sonar per gruppi di scafi; docs/ricerca/34)
- `[WPTraf]` https://en.wikipedia.org/wiki/Trafalgar-class_submarine
- `[WPAlfa]` https://en.wikipedia.org/wiki/Alfa-class_submarine
- `[DS705]` DeepStorm.ru, Pr.705: http://deepstorm.ru/DeepStorm.files/45-92/nts/705/list.htm
- `[GS705]` GlobalSecurity, *Project 705 Lira – Western Response*: https://www.globalsecurity.org/military/world/russia/705-response.htm
- `[WPVictor]` https://en.wikipedia.org/wiki/Victor-class_submarine
- `[RW671]` RuWiki, *Подводные лодки проекта 671РТМ(К) «Щука»*: https://ru.ruwiki.ru/wiki/Подводные_лодки_проекта_671РТМ(К)_«Щука»
- `[WPAkula]` https://en.wikipedia.org/wiki/Akula-class_submarine
- `[RN971]` RusNavy.com, schede K-391, K-263, K-331, K-419: https://rusnavy.com/nowadays/strength/submarines/bratsk/ (e pagine analoghe)
- `[WPOscar]` https://en.wikipedia.org/wiki/Oscar-class_submarine
- `[WPCharlie]` https://en.wikipedia.org/wiki/Charlie-class_submarine
- `[WPTyphoon]` https://en.wikipedia.org/wiki/Typhoon-class_submarine
- `[WPKilo]` https://en.wikipedia.org/wiki/Kilo-class_submarine
- `[WPFoxtrot]` https://en.wikipedia.org/wiki/Foxtrot-class_submarine
- `[RWSkat]` Wikipedia RU, *Скат (гидроакустический комплекс)*: https://ru.wikipedia.org/wiki/Скат_(гидроакустический_комплекс)
- `[MIA1]` E.V. Miasnikov, *The Future of Russia's Strategic Nuclear Forces*, Appendix 1 (1998): https://spp.fas.org/eprint/snf03221.htm
- `[MIA2]` idem, Appendix 2: https://spp.fas.org/eprint/snf03222.htm
- `[TYL]` G.D. Tyler Jr., “The Emergence of Low-Frequency Active Acoustics as a Critical Antisubmarine Warfare Technology”, *JHU APL Technical Digest* 13(1), 1992: https://www.jhuapl.edu/techdigest/Content/techdigest/pdf/V13-N01/13-01-Tyler.pdf (citato tramite estratto di ricerca; PDF troppo grande per la lettura integrale)
- `[USNI95]` USNI *Proceedings*, ottobre 1995: https://www.usni.org/magazines/proceedings/1995/october/republic-navies-quest-quiet-submarine
- `[WPKara]` https://en.wikipedia.org/wiki/Kara-class_cruiser
- `[WPKrivak]` https://en.wikipedia.org/wiki/Krivak-class_frigate
- `[WPKirov]` https://en.wikipedia.org/wiki/Kirov-class_battlecruiser
- `[WPSlava]` https://en.wikipedia.org/wiki/Slava-class_cruiser
- `[WPKoni]` https://en.wikipedia.org/wiki/Koni-class_frigate
- `[WPNanu]` https://en.wikipedia.org/wiki/Nanuchka-class_corvette
- `[RWKa25]` https://ru.wikipedia.org/wiki/Ка-25
- `[RWKa27]` https://ru.wikipedia.org/wiki/Ка-27
- `[RWIl38]` https://ru.wikipedia.org/wiki/Ил-38
- `[RWTu142]` https://ru.wikipedia.org/wiki/Ту-142
- `[WPTu22M]` https://en.wikipedia.org/wiki/Tupolev_Tu-22M
- `[FASSH60]` https://man.fas.org/dod-101/sys/ac/sh-60.htm
- `[WPSH60]` https://en.wikipedia.org/wiki/Sikorsky_SH-60_Seahawk
- `[WPSH3]` https://en.wikipedia.org/wiki/Sikorsky_SH-3_Sea_King
- `[WPAQS13]` https://en.wikipedia.org/wiki/AN/AQS-13
- `[FASP3]` https://man.fas.org/dod-101/sys/ac/p-3.htm
- `[FASSSQ53]` https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-53.htm
- `[FASSSQ62]` https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-62.htm
- `[DICASS]` NPS Savage, DICASS AN/SSQ-62: https://www.web3d.org/x3d-resources/content/Savage/CommunicationsAndSensors/Sonobuoys/DICASS-an-ssq-62.pdf
- `[DTIC41]` DTIC AD0849291 (AN/SSQ-41/47): https://apps.dtic.mil/sti/pdfs/AD0849291.pdf
- `[FASMk48]` https://man.fas.org/dod-101/sys/ship/weaps/mk-48.htm
- `[WPMk48]` https://en.wikipedia.org/wiki/Mark_48_torpedo
- `[NUM]` Naval Undersea Museum, Mk 48: https://navalunderseamuseum.org/mk-48/
- `[HIS]` H.I. Sutton, Covert Shores, *Guide to heavyweight 533mm torpedoes*: https://www.hisutton.com/Guide-to-heavyweight-533mm-torpedoes.html
- `[NWUS]` NavWeaps, *USA Torpedoes since WWII*: https://navweaps.com/Weapons/WTUS_PostWWII.php
- `[NWBR]` NavWeaps, *British Torpedoes since WWII*: https://navweaps.com/Weapons/WTBR_PostWWII.php
- `[NWRU]` NavWeaps, *Russian/Soviet Torpedoes since WWII*: https://navweaps.com/Weapons/WTRussian_post-WWII.php
- `[FASMk46]` https://man.fas.org/dod-101/sys/ship/weaps/mk-46.htm
- `[WPMk50]` https://en.wikipedia.org/wiki/Mark_50_torpedo
- `[WPTiger]` https://en.wikipedia.org/wiki/Tigerfish_(torpedo)
- `[WPSpear]` https://en.wikipedia.org/wiki/Spearfish_torpedo
- `[WPType53]` https://en.wikipedia.org/wiki/Type_53_torpedo
- `[WPType65]` https://en.wikipedia.org/wiki/Type_65_torpedo
- `[RWTEST71]` https://ru.wikipedia.org/wiki/ТЭСТ-71
- `[RWUSET80]` https://ru.wikipedia.org/wiki/УСЭТ-80
- `[RWSET65]` https://ru.wikipedia.org/wiki/СЭТ-65
- `[RW6576]` https://ru.wikipedia.org/wiki/65-76
- `[WPGranit]` https://en.wikipedia.org/wiki/P-700_Granit
- `[WPRPK6]` https://en.wikipedia.org/wiki/RPK-6_Vodopad
- `[WPSUBROC]` https://en.wikipedia.org/wiki/UUM-44_SUBROC
- `[WPRK55]` https://en.wikipedia.org/wiki/RK-55
- `[RWRBU6000]` https://ru.wikipedia.org/wiki/РБУ-6000
- `[RWRBU1000]` https://ru.wikipedia.org/wiki/РБУ-1000
- `[ADC]` Navy SBIR N191-022 / N191-023 (ADC 3"): https://www.navysbir.com/n19_1/N191-022.htm , https://navysbir.com/n19_1/N191-023.htm ; Military & Aerospace Electronics: https://www.militaryaerospace.com/sensors/article/14213787/torpedo-decoys-acoustic-countermeasure-submarines
- `[FASBQQ5]` https://man.fas.org/dod-101/sys/ship/weaps/an-bqq-5.htm
- `[FASTB16]` https://man.fas.org/dod-101/sys/ship/weaps/tb-16.htm (+ thread Subcommittee forum che riporta la tabella di Friedman: https://forum.subcommittee.com/forum/general-forums/general/3103-la-towed-array)
- `[FASSQS53]` https://man.fas.org/dod-101/sys/ship/weaps/an-sqs-53.htm
- `[FASSQR19]` https://man.fas.org/dod-101/sys/ship/weaps/an-sqr-19.htm
- `[CIA-NIE]` CIA Reading Room, NIE 11-15-84/D *Soviet Naval Strategy and Programs Through the 1990s*: https://cia.gov/readingroom/document/cia-rdp87t00126r000600700004-3 ; NIE 11-15-82D: https://www.cia.gov/readingroom/document/0000268225 (testo non estratto)

### Libri (citati da memoria della letteratura, *n.v.* — da verificare con pagina)
- `[PM04]` N. Polmar, K.J. Moore, *Cold War Submarines: The Design and Construction of U.S. and Soviet Submarines*, Brassey's/Potomac, 2004.
- `[PGSN]` N. Polmar, *Guide to the Soviet Navy*, 4ª ed. (1986) e 5ª ed. (1991), Naval Institute Press.
- `[FR94]` N. Friedman, *U.S. Submarines Since 1945: An Illustrated Design History*, Naval Institute Press, 1994.
- `[FRWNWS]` N. Friedman, *The Naval Institute Guide to World Naval Weapon Systems*, ed. 1989 e 1991/92.
- `[JFS]` *Jane's Fighting Ships* 1988–89 / 1989–90.
- `[URICK]` R.J. Urick, *Principles of Underwater Sound*, 3ª ed., McGraw-Hill, 1983.
- `[ROSS]` D. Ross, *Mechanics of Underwater Noise*, Pergamon, 1976.
- `[LURTON]` X. Lurton, *An Introduction to Underwater Acoustics*, Springer, 2002.
- `[MCK]` M.F. McKenna et al., “Underwater radiated noise from modern commercial ships”, *JASA* 131(1), 2012.
