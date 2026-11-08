# 21e — Audio del maltempo (pezzo M5): fulmini in acqua, tuono, scafo, scariche radio

Ricerca su fonti aperte per sintetizzare i suoni del maltempo nel remake di *688 Attack Sub*.
Complementa `b_sensori.md` §1.10, §4.1 e §7, che **non** sono ripetuti qui (Arnold 1984, Hill 1985, DOSITS, NWS 10 miglia, finestra di Snell, ITU-R P.372 in generale). Il rumore di pioggia e vento in acqua è già nel modello e non è trattato.

**Convenzioni.**
- Affidabilità di ogni dato: **alta** (articolo con revisione, norma, rapporto tecnico ufficiale), **media** (rassegna, tesi, atti di convegno, sintesi secondaria di un lavoro primario), **bassa** (divulgazione, forum, blog).
- **n.v.** = non verificato o non trovato in fonti aperte.
- **[CALC]** = calcolato qui da formule fisiche standard; **STIMA** = proposta motivata, da tarare a orecchio.
- Livelli in acqua in dB re 1 µPa; in aria in dB re 20 µPa (dB SPL). Per passare dall'aria all'acqua a parità di pressione si aggiungono 26 dB.
- Sigle delle fonti tra parentesi quadre, elenco in §6.

---

## 1. Fulmine che colpisce il mare: suono subacqueo

### 1.1 Numeri trovati

| Grandezza | Valore | Fonte | Affid. |
|---|---|---|---|
| Livello di sorgente | **≈ 260 dB re 1 µPa @ 1 m** (260,5 dB re 1 µPa²m² nel calcolo TNO) | [HIL85] citato da [DOL09], [DOSITS-FAQ], [NAVY-LFA], [INPEX09] | media (numero primario dietro paywall; tutte le sintesi rimandano a Hill) |
| Durata del picco alla sorgente | **≈ 30 µs** ("durata tipica di un colpo") | [DOL09] | media |
| Energia acustica per colpo | **≈ 30 kJ** (da 260,5 dB × 30 µs) | [DOL09]; [CALC] 27 kJ | media; incertezza "di diversi ordini di grandezza" ammessa dagli autori |
| Livello di energia equivalente @ 1 m | **≈ 215 dB re 1 µPa²·s·m²** | [CALC] da 260,5 dB + 10·log(30 µs) | [CALC] |
| Rendimento elettrico → acustico in acqua | ≈ 0,006 % (con 500 MJ per scarica) | [DOL09] | bassa-media (speculativo) |
| Energia totale stimata da registrazione | compatibile con l'energia elettrostatica di una scarica nube-suolo tipica | [ABA84] | alta (solo qualitativo nell'abstract) |
| Distanza delle celle nella registrazione storica | **30-40 km**, idrofono calato dalla R/V *Tommy Munro*, Golfo del Messico | [ABA84], [DOSITS-AUD] | alta |
| Spettro ricevuto | energia soprattutto **10-1000 Hz**, picchi **100-300 Hz** (spettrogramma DFO, Columbia Britannica) | [INPEX09] | media |
| Spettro del tuono ricevuto in acqua, temporale a **5-10 km** | picco **50-250 Hz**, fino a **15 dB sopra il fondo**; energia rilevabile da **10 Hz a 1 kHz** | Dubrovsky e Kosterin 1993 via NRC 2003, citati da [INPEX09] | media (fonte primaria n.v.) |
| Carattere | "impulso breve improvviso", banda larga, sorgente in superficie, imprevedibile | [INPEX09] tab. 5-1 | media |
| Rilevazione a grande distanza | colpi fortissimi (−335 / −347 kA) a 69 e 103 km da una stazione sismica costiera **non** rilevati in modo convincente; colpi a < 150 km da Diego Garcia "appena rilevabili"; un colpo da −528 kA non rilevato a migliaia di km | [MH370] | bassa (blog, analisi amatoriale ma documentata) |
| Livello "270 dB" | circola in rete senza riferimento | [MH370] | bassa: **non usare** |
| Frequenza dei fulmini sul mare | ~2 colpi/km²/anno sulla costa, rari in mare aperto | già in `b_sensori` §1.10 | — |

**Colpi multipli (il «~57 ms»).** La stessa scarica nube-suolo negativa ha di solito più colpi di ritorno:

| Grandezza | Valore | Fonte | Affid. |
|---|---|---|---|
| Colpi per scarica | media **3-5**; 3,8 in Brasile (233 scariche); 3,9 Arizona + San Paolo | [CIGRE13], [SABA06], [SARA10] | alta |
| Scariche con un solo colpo | ~**20 %** (17 % in Florida) | [SABA06], [RUT94] | alta |
| Intervallo fra colpi | media geometrica **≈ 60 ms** (61 ms su 608 intervalli; 61,5 ms) | [CIGRE13], [SABA06], [SARA10] | alta |
| Corrente di picco del primo colpo | tipica ~30 kA | [MH370] (cita Wikipedia) | bassa; ordine di grandezza corretto |

Il «57 ms» richiesto rientra in questa media: va trattato come **media di una distribuzione larga**, non come costante. La dispersione è **n.v.** nelle fonti aperte lette; si propone una lognormale (§5.1).

### 1.2 Fulmine nube-nube e tuono attraverso la superficie

- **Ogni scarica dentro la nube** (circa ¾ del totale [WIKIB]) non tocca l'acqua: il solo canale verso il mare è il **tuono in aria che attraversa la superficie**.
- **Finestra di Snell** [CALC]: sin θc = 343/1500, quindi **θc = 13,2°**. Una sorgente a quota *h* entra in acqua "in propagazione" solo se il sottomarino sta entro **0,235·h** in orizzontale. Per un canale a 4-5 km di quota il raggio è **≈ 1-1,2 km**. Il tuono di una scarica nella nube si sente in acqua soltanto se la scarica è **quasi sulla verticale**.
- **Fuori dalla finestra** l'onda in acqua è evanescente e decade con la profondità. Attenuazione a **18 m** (quota periscopica) [CALC]:

| f | incidenza 30° | 45° | 80° |
|---|---|---|---|
| 5 Hz | −6 dB | −10 dB | −14 dB |
| 10 Hz | −13 dB | −19 dB | −27 dB |
| 20 Hz | −25 dB | −38 dB | −55 dB |
| 50 Hz | −64 dB | −96 dB | −137 dB |
| 100 Hz | −127 dB | oltre −190 dB | oltre −270 dB |

  A quota periscopica il tuono lontano passa solo come **residuo infrasonico (< 10-20 Hz)**, sotto la banda utile del sonar. Le irregolarità della superficie ne fanno passare un po' di più (fenomeno noto per gli aerei), ma la quantità è **n.v.**
- **Dentro la finestra** [CALC]: all'interfaccia aria-acqua la pressione quasi raddoppia (+6 dB). Il livello appena sotto la superficie vale quindi **L_aria(dB SPL) + 26 + 6 ≈ L_aria + 32 dB re 1 µPa**. Un tuono da 110 dB di picco sulla verticale dà ~142 dB re 1 µPa di picco subito sotto: ben udibile.
- Registrazione di conferma: un idrofono sotto la superficie durante un temporale estivo **sulla verticale** registra i "toni bassi del tuono" mescolati alla pioggia [NASA00] (media; nessun livello).
- **Lavori recenti 2000-2025** specifici su fulmine e tuono in acqua (reti OBS, idrofoni CTBTO, DAS): **non trovati** in fonti aperte. La letteratura recente su questi temi riguarda l'infrasuono **in aria** (§2). Le stazioni idroacustiche IMS non risultano usate per i fulmini; l'unico tentativo trovato è amatoriale [MH370].

---

## 2. Tuono in aria

### 2.1 Pressione e livello in funzione della distanza

| Distanza | Pressione / livello | Fonte | Affid. |
|---|---|---|---|
| Pochi metri dal canale | **165-180 dB SPL**, a volte oltre 200 | [WIKI-T] | bassa-media |
| 70 m (fulmine innescato) | spettro fino a migliaia di Hz | Depasse 1994 via [JOH13] | media |
| **1 km**, scariche nube-suolo | picco **mediana 13 Pa ≈ 116 dB** (banda 0,01-500 Hz) | [JOH13], 191 eventi, New Mexico | media-alta (tesi con dati) |
| **1 km**, scariche nella nube | picco **mediana 4 Pa ≈ 106 dB**, quasi sempre < 10 Pa (114 dB) | [JOH13] | media-alta |
| 1 km, intervallo complessivo | da **0,1 Pa a > 100 Pa** (74-134 dB); massimo del catalogo 140 Pa | [JOH13] | media-alta |
| ~1 km (2-3 s dopo il lampo) | impulso **> 20 Pa ≈ 120 dB** | [RUSZ21] | alta |
| ~7-14 km (20-40 s dopo il lampo) | oscillazioni di **qualche Pa (≈ 100-106 dB di picco)**, filtrate sopra 2 Hz | [RUSZ21] | alta |
| Temporale vicino (infrasuono) | "diversi Pa"; 20 Pa da picco a picco in una campagna del 2005 | [FB10] | media (abstract) |
| 0,3-20 km | caratteristiche del tuono **fortemente dipendenti dalla distanza** (27 scariche, 0,1-180 Hz) | [LAC19] | media (abstract) |
| Decadimento oltre 10 km (infrasuono 0,5-5 Hz) | ampiezza ∝ **d^−0,717** entro 100 km; prima stima: **−20 dB ogni 50 km** | [FAR21], [FAR12] | alta / media |
| Udibilità | raramente oltre **~25 km** (rifrazione verso l'alto con gradiente normale; il vento la riduce) | [FLE49] | alta |
| Udibilità (NWS) | **~10 miglia (16 km)** con gradiente termico normale; con **inversione** il tuono arriva più forte | [NWS-T] | alta |
| Infrasuono rilevabile | entro 50-75 km in propagazione diretta; zona d'ombra 70-200 km; oltre 200 km con poco rumore | [FAR12], [FB10] | media |
| Energia acustica totale | da decine a migliaia di kJ per scarica | [JOH13] | media |

**Tabella per il gioco** [CALC/STIMA]. La pressione di picco segue 1/r partendo dalla mediana di [JOH13] a 1 km; il livello A-pesato si stima togliendo 15-25 dB, perché l'energia sta sotto i 200 Hz (correzione A: −30 dB a 50 Hz, −19 a 100 Hz, −11 a 200 Hz).

| Distanza | Picco nube-suolo | Picco nella nube | dB(A) percepito, STIMA | Carattere |
|---|---|---|---|---|
| 100 m | ~130-136 dB (limite non lineare) | ~120 dB | 110-120 dB(A) | schianto secco, "strappo" |
| 1 km | **116 dB** | 106 dB | 95-100 dB(A) | colpo + rimbombo |
| 5 km | ~102 dB | ~92 dB | 75-85 dB(A) | rimbombo, poco "clap" |
| 10 km | ~96 dB (meno l'ombra di rifrazione) | ~86 dB | 60-70 dB(A) | brontolio basso |
| 15 km | ~90 dB | ~80 dB | 50-60 dB(A) | brontolio al limite, coperto da vento e mare |
| > 20-25 km | solo infrasuono | — | inudibile | — |

### 2.2 Spettro

| Dato | Valore | Fonte | Affid. |
|---|---|---|---|
| Spettro medio di 23 registrazioni | massimo largo **vicino a 200 Hz**, nessuna componente infrasonica dominante | [FEW67] | alta |
| Few 1969 | massimi a **52 e 96 Hz**; fronte d'onda **cilindrico** | [FEW69] via [RG16] | alta |
| Modello di Few | contenuto dominante **10-150 Hz** | [ASS08] (rassegna) | media |
| Holmes et al. 1971, 40 tuoni (New Mexico 1967-68) | picchi **da < 4 a 125 Hz**; energia e picco più bassi per le scariche nella nube; alcune componenti sotto 1 Hz | [HOL71] via [JOH13] e [NRC86] | media-alta |
| Energia | quasi tutta **sotto 500 Hz** | [JOH13] (sintesi di Few e Holmes) | media-alta |
| Sri Lanka, 90 scariche | frequenza più comune **175-200 Hz**, media **227 Hz**; oscillazioni dei "clap" **< 300 Hz** | [BOD08] | media |
| Fondamentale per tipo (ICLP 2018) | nube-suolo 88-160 Hz; nella nube 82-123 Hz | [ABE18] | media |
| Fondamentale del tuono udibile | **< 250 Hz** | [ABE16] | media |
| Infrasuono | dominante **1-5 Hz** (picco a 3,9 Hz nel modello); 0,5-2,2 Hz in [JOH13] | [ASS08], [JOH13] | alta / media |
| Assorbimento | forte **sopra 500 Hz**, trascurabile **sotto 100 Hz** in condizioni tipiche | [OUY] | media-bassa |
| Distanza e timbro | da vicino componenti acute (il "clap"); da lontano si perdono gli acuti e il clap sparisce | [SOS24], [NWS-T] | bassa-media |

Formula di Few [WIKIB, FEW69]: f_max = 0,63·c/R₀, con raggio di rilassamento R₀ = √(E/(π·p₀)). Con E = 50 kJ/m viene R₀ = 0,4 m e f_max ≈ 540 Hz [CALC]. I picchi misurati a 50-200 Hz implicano energie più alte o R₀ di 1-4 m. Il modello teorico tende a dare picchi più alti del tuono reale [NRC86].

### 2.3 Durata, struttura e ritardo

| Dato | Valore | Fonte | Affid. |
|---|---|---|---|
| Durata del tuono (90 scariche) | **2,2-33,7 s**; più probabile **10-15 s**; media 14 s nella nube, 12 s nube-suolo | [BOD08] | media |
| "Clap" per tuono | di solito 1-2; nella nube 1-5, nube-suolo 1-3; **intervallo medio fra clap 2,0 s** | [BOD08] | media |
| Lunghezza dei canali | 2-12 km | [BOD08] | media |
| Clap | **0,2-2 s**, più acuto; rimbombo oltre 30 s | [WIKI-T] (Vavrek) | bassa-media |
| Durata del segnale | spesso decine di secondi; energia più concentrata nel tempo per le scariche nube-suolo | [JOH13] | media |
| Direttività | ~80 % dell'energia entro ±30° dal piano perpendicolare al tratto principale del canale | [WIKIB] (Depasse) | media |
| Velocità del suono | 343 m/s a 20 °C (≈ 331,3 + 0,606·T °C); **≈ 3 s/km**, 5 s/miglio | [WIKI-T], [NWS] | alta |

**Durata geometrica del rimbombo** [CALC]. Per un canale verticale alto H = 5 km visto a distanza D: Δt = (√(D² + H²) − D)/c.

| D | 100 m | 1 km | 3 km | 5 km | 10 km | 15 km |
|---|---|---|---|---|---|---|
| Δt (solo canale verticale) | 14 s | 12 s | 8 s | 6 s | 3,4 s | 2,4 s |

I rami orizzontali nella nube (5-15 km) aggiungono da 5 a 20 s. La geometria spiega perché il tuono vicino è lungo e "pieno". Quello lontano è più corto e soprattutto più basso e debole: la sua coda si perde nel rumore di fondo.

---

## 3. Dentro un sottomarino o una nave d'acciaio

### 3.1 Che cosa dicono le fonti

| Tema | Dato | Fonte | Affid. |
|---|---|---|---|
| Norme IMO sul rumore a bordo | il Codice **esclude esplicitamente il rumore di vento, onde e ghiaccio**; le misure si fanno con **vento ≤ forza 4 e onde ≤ 1 m** | [IMO337] §1.3.10, §3.5.2 | alta |
| Limiti IMO (rumore di macchina) | plancia **65 dB(A)**; alette e posti di vedetta **70**; sala radio **60**; cabine **60** (1.600-10.000 GT) o **55** (≥ 10.000 GT); mense 65/60; sala controllo macchine 75 | [IMO337] §4.2 | alta |
| Livelli misurati a bordo | media ~69 dB(A) su molti locali; cabine fino a 57,5 dB(A) (studio 2023); macchine oltre 90 dB(A) nel 79 % delle navi tedesche (1985) | [SHIPN] | media |
| Rumore delle onde in plancia o in alloggio con mare grosso | **n.v.** (nessuna misura aperta trovata; le norme lo escludono) | — | — |
| Pioggia forte su lamiera (tetti metallici) | 50-65 dB all'interno senza isolamento (−10/−20 dB con isolamento); fino a **70 dB(A)** in aule scolastiche; intensità ∝ v⁶·d³ (velocità e diametro della goccia) | [ROOF] | bassa-media |
| Pioggia "forte" normalizzata | 40 mm/h, gocce da 5 mm, 4-7 m/s (ISO 140-18 / 10140-5) | [ROOF] | media |
| Vento all'orecchio (galleria del vento) | **85 dB a 15 mph (6,7 m/s) → 120 dB a 60 mph (26,8 m/s)**, orecchio sottovento più rumoroso | [SEI17] | alta |
| Vento all'orecchio (motociclisti, casco) | 90-109 dB(A) a 22-32 m/s; **+15,5 dB per raddoppio della velocità** | [HUR96] | media-alta |
| Sottomarino in superficie | i battelli nucleari "rollano come maiali" in superficie (scafo di forma sbagliata); a quota periscopica si sta male soprattutto con mare al traverso o in poppa | [PPRUNE] | bassa (forum, ex sommergibilisti dichiarati) |
| Snorkel con mare di prua (diesel) | la valvola dell'aria si chiude quando la testa va sott'acqua e si riapre con un **colpo**; orecchie che si stappano, difficile parlare | [PPRUNE] | bassa |
| Profondità per non sentire le onde | testimonianze discordi: "sotto i 100 piedi è trascurabile" contro "rollato forte a 300 piedi in una grande tempesta"; regola: mezza lunghezza d'onda | [WAVEQ] | bassa |
| Acqua dalla torretta | USS *Saury*: un'onda in poppa manda molta acqua giù dal portello della torretta e fa ruotare la prua di ~140° in mezzo minuto | [SAURY] | media (sito del museo USS Nautilus) |
| Registrazioni d'archivio | AWM, HMAS *Oxley* (Oberon), ~1980, 1 h 16 min: ambienti interni, emersione, quota periscopica, ma **nessuna traccia di maltempo** | [AWM] | alta (solo inventario) |
| Testimonianze dirette sui *suoni* del maltempo a bordo di un SSN | **n.v.**; memorie possibili da verificare: Penley, *Deep Venture* (tempesta artica), Conley, *Down to the Sea in Submarines* | [MEM] | — |

### 3.2 Fisica utile [CALC]

**Isolamento dello scafo ai suoni aerei**, legge di massa: TL ≈ 20·log(m·f) − 47 dB, con m in kg/m². Lo spessore reale dello scafo resistente dei 688 è **n.v.**; si usa la STIMA 50 mm di acciaio (m = 392 kg/m²).

| Lamiera | 50 Hz | 100 Hz | 300 Hz | 1 kHz |
|---|---|---|---|---|
| 25 mm (casing, vela, nave) | 33 dB | 39 dB | 48 dB | 59 dB |
| 50 mm (scafo resistente, STIMA) | 39 dB | 45 dB | 54 dB | 65 dB |
| 75 mm | 42 dB | 48 dB | 58 dB | 68 dB |

Conseguenze:
- Il tuono a 1 km (116 dB di picco, energia sotto 200 Hz) arriva **dentro lo scafo a ~70 dB di picco**, un tonfo basso. A 5 km scende a ~55-60 dB, sotto il fondo della ventilazione: **inudibile**.
- La pioggia su lamiera è dominata da frequenze di 1-4 kHz, attenuate di 60-70 dB: **inudibile dentro lo scafo resistente**. Si sente solo dal portello della plancia aperto, o in plancia.
- Gli urti delle onde sono **rumore per via strutturale**, non aereo: per questi l'isolamento dalla massa non vale. Il livello è **n.v.**

**Moto ondoso e profondità**: l'ampiezza del moto orbitale decade come e^(−2πz/λ). Onde lunghe 100 m (periodo ~8 s): a 18 m resta il **32 %**, a 50 m il **4 %**, a 100 m lo **0,2 %**. Mare lungo da tempesta, λ = 300 m (periodo ~14 s): a 100 m resta il **12 %**. Per questo alcuni sommergibilisti rollano ancora a 100 m nelle grandi tempeste.

---

## 4. Scariche atmosferiche nel ricevitore HF/VLF

### 4.1 Livello del rumore (Fa) — ITU-R P.372-18

| Dato | Valore | Fonte | Affid. |
|---|---|---|---|
| Fa atmosferico **massimo** (superato lo 0,5 % delle ore), letto dalla Fig. 2 | ~172 dB a 10 kHz; ~100 dB a 1 MHz; ~75 dB a 3 MHz; ~57 dB a 5 MHz; ~30 dB a 10 MHz; → 0 verso 20 MHz | [P372] Fig. 2 (lettura ±5 dB) | alta (lettura grafica media) |
| Fa atmosferico **minimo** (superato il 99,5 % delle ore) | ~150 dB a 10 kHz; ~75 dB a 100 kHz; ~0 dB a 0,8 MHz; risale a ~27 dB a 7-8 MHz | [P372] Fig. 2 | idem |
| Rumore galattico | Fam = 52 − 23·log f(MHz) (±2 dB): 36 dB a 5 MHz, 29 dB a 10 MHz | [P372] eq. 15 | alta |
| Antenne direttive | ±5 dB secondo la direzione di puntamento | [P372] | alta |
| Campo di rumore | En = Fa + 20·log f(MHz) + B − 95,5 dB(µV/m), monopolo corto verticale; B = 10·log(banda in Hz) | [NTIA85] eq. 8 | alta |
| Esempio [CALC] | 5 MHz, SSB 3 kHz (B = 34,8), Fa = 60 → En ≈ +13 dBµV/m (4,6 µV/m); il fondo galattico dà −11 dBµV/m: **il temporale sta ~24 dB sopra il fondo** | — | [CALC] |
| Temporale locale | +10 dB circa sul rumore quieto | già in `b_sensori` §4.1 | media |

### 4.2 Statistica d'ampiezza (APD e Vd)

- **Vd** = 20·log(Erms/Emedia) dell'inviluppo. Vale **1,05 dB per il rumore gaussiano**; più è alto, più il rumore è impulsivo [NTIA85] (alta). Ld è legato a Vd: Ld ≈ 1,697·Vd + 0,7265 [NTIA85].
- **Conversione di banda** [NTIA85] eq. 55: Vd(B) = Vd₂₀₀ + (0,4679 + 0,2111·Vd₂₀₀)·log(B/200 Hz). È valida a MF e HF; P.372 avverte di usarla con cautela a LF e VLF.
- **Valori mediani CCIR 322** [CALC], calcolati con i coefficienti della Tab. 41 di [NTIA85] e x = log f(MHz). Ogni cella dà il valore a 200 Hz / a 3 kHz (SSB):

| Stagione, fascia oraria | 20 kHz | 100 kHz | 1 MHz | 3 MHz | 5 MHz | 10 MHz | 20 MHz |
|---|---|---|---|---|---|---|---|
| Inverno 00-04 | 6,6 / 8,8 | 8,1 / 10,6 | 6,8 / 9,0 | 6,0 / 8,0 | 5,5 / 7,4 | 4,5 / 6,2 | 3,1 / 4,4 |
| Inverno 12-16 | 12,3 / 15,9 | 8,8 / 11,5 | 3,9 / 5,4 | 4,4 / 6,0 | 4,8 / 6,5 | 4,8 / 6,5 | 3,2 / 4,6 |
| Estate 00-04 | 9,7 / 12,7 | 8,7 / 11,5 | 7,2 / 9,5 | 6,2 / 8,3 | 5,5 / 7,4 | 4,3 / 5,9 | 2,6 / 3,8 |
| Estate 12-16 | 10,0 / 13,1 | 10,3 / 13,4 | 8,2 / 10,8 | 6,9 / 9,1 | 6,2 / 8,3 | 5,1 / 7,0 | 3,8 / 5,3 |
| Estate 20-24 | 8,4 / 11,1 | 7,2 / 9,6 | 5,1 / 7,0 | 4,3 / 5,9 | 3,9 / 5,4 | 3,2 / 4,5 | 2,2 / 3,3 |

  Lettura: in HF a banda SSB **Vd ≈ 5-9 dB**, cioè rumore chiaramente "a crepitii". In VLF **Vd ≈ 11-16 dB**. Di notte (molte scariche lontane sovrapposte via ionosfera) il rumore è meno impulsivo che di giorno.

### 4.3 Struttura nel tempo (che cosa si sente)

| Dato | Valore | Fonte | Affid. |
|---|---|---|---|
| Unità naturale del rumore | il **burst** prodotto da un'intera scarica | [AIY77] | media-alta |
| Durata del burst (MF/HF, tropici) | mediana **~500 ms**, ~200 ms al picco d'attività della nube; lognormale | [AIY77] | media-alta |
| Microstruttura | fondo continuo debole più **impulsi distinti di ~1 ms**; ogni 100 ms di burst, mediana di **9** impulsi entro 6 dB dal picco, **13** entro 12 dB, **20** entro 18 dB; ampiezze lognormali, tempi casuali | [AIY77] | media-alta |
| Burst al minuto (tropici) | **5-40**, più probabili **10-20**, mediana **20**; lognormale | [AIY77] | media-alta |
| Scariche K nella nube | ricorrenza **2-20 ms**, durata **1-3 ms** | [AIY77] | media |
| Una nube temporalesca tropicale (mediane) | vita **3 h**; picco **8 lampi/min** per **10 min**; **700** lampi in tutto; 5 celle da 35 min | [AIY77] | media |
| Soglia percettiva | l'orecchio coglie un impulso di ~1 ms; oltre ~200 ms si percepisce un rumore continuo | [AIY77] | media |
| Durata HF (3 MHz) del colpo di ritorno | negativo **~140 µs** (60-600 µs); positivo **5-25 ms** | [UTEM] | media |
| Lampi nel mondo | **~45 al secondo** (OTD); ~100 colpi al secondo | [FAR12] (cita Christian 2003), [MCG20] | alta / bassa |
| Rilevazione | sferics VLF ricevibili fino a ~5.000 km, ~10.000 km di notte verso ovest | [WIKI-S] | media |

**Carattere udibile.**
- In VLF audio (0,3-11 kHz): "schiocchi, crepitii e pop", come **pancetta che frigge** o **pioggia sul tetto**. La sera estiva diventa un "ruggito" continuo; il mattino è quieto [MCG20].
- Le scariche entro ~50 miglia sono "**estremamente forti**" in cuffia [MCG20].
- I Q-burst (fulmini positivi potenti) danno un "pop" enorme [MCG20].
- In AM/HF i radioamatori parlano di **"static crashes"** (dato descrittivo comune, bassa).

### 4.4 Sferic, tweek e whistler (VLF)

| Fenomeno | Numeri | Fonte | Affid. |
|---|---|---|---|
| Sferic | salita in **pochi µs**, poi oscillazione smorzata; massimo spettrale **~5 kHz**; energia prevalente sotto 100 kHz | [WIKI-S] | media |
| Tweek | solo **di notte**, da sorgenti a **≥ 2.000 km**; taglio della guida d'onda a **1,6-1,9 kHz** (nominale 1,7 kHz); discesa da ~10 kHz a 1,6 kHz in **10-50 ms**; armoniche a multipli del taglio; quota di riflessione 75 km al tramonto e 88-91 km a mezzanotte | [MCG20], [SUK96], [OHYA], [WIKI-S] | media |
| Tweek, suono | "gocciolio" o "ping" musicale; a volte 2-3 echi a intervalli uguali | [MCG20] | bassa-media |
| Taglio [CALC] | f_c ≈ c/(2h): h = 88 km → **1,70 kHz**; armoniche a 3,4 e 5,1 kHz | — | [CALC] |
| Whistler | nota discendente **da ~6 kHz a ~0,5 kHz** (a volte da oltre 10 kHz); a 1 salto **0,3-1 s**, a 2 salti **1-4 s**; il 2 salti arriva 1-2 s dopo lo sferic che lo genera; picco 3-5 kHz; banda 1-30 kHz | [MCG20], [WIKI-W] | media |

---

## 5. Proposta per la sintesi

Principio: **un solo generatore di eventi "fulmine"** alimenta vista, sonar, tuono e radio. Ogni lampo ha posizione, tipo (nube-suolo o nella nube) e numero di colpi. Ne derivano:
- luce: subito;
- crash radio: subito (velocità della luce);
- impulso sonar: dopo D/1500 s (in acqua);
- tuono: dopo D/343 s (in aria).

A 10 km il sonar sente il colpo dopo **6,7 s** e il tuono arriva dopo **29 s**: una sequenza coerente e "da manuale".

### 5.1 Generatore di eventi (STIMA, parametri da `b_sensori` §7 e §4.3)

| Parametro | Valore proposto | Base |
|---|---|---|
| Lampi per cella attiva | 1-8 al minuto (picco 8 per 10 min); Poisson | [AIY77] |
| Quota nube-suolo / nella nube | 25 % / 75 % | [WIKIB] |
| Colpi per scarica nube-suolo | 1 + Poisson(2,8): media 3,8, 20 % singoli | [SABA06] |
| Intervallo fra colpi | lognormale, mediana **60 ms**, σ_ln = 0,7 (≈ 20-180 ms al 90 %); STIMA per la dispersione | [CIGRE13] |
| Ampiezza dei colpi successivi | −6 dB ± 3 dB rispetto al primo (STIMA) | corrente dei colpi successivi più bassa (ordine di grandezza noto, n.v. qui) |

### 5.2 Fulmine in acqua (sonar, BTR, cuffia del sonarista)

- **Forma di ogni colpo**: rumore filtrato passa-banda **40-400 Hz** con picco a **150 Hz** (STIMA dentro 100-300 Hz [INPEX09]). Attacco **1-2 ms**, decadimento esponenziale τ = **40 ms**. Coda di multipercorso (superficie e fondo) di **100-300 ms** a −10/−20 dB.
- **Sotto i 3 km** si aggiunge un "crack" acuto di 5-10 ms con energia fino a **2-5 kHz** (STIMA: i 30 µs di sorgente sono a banda larghissima, ma l'alta frequenza si perde presto).
- **Filtro passa-basso secondo la distanza** (STIMA): 2 km → 1 kHz; 10 km → 500 Hz; 30 km → 300 Hz.
- **Livello come eccesso sul rumore ambiente nella banda 50-300 Hz** (STIMA tarata su Dubrovsky e Kosterin, Arnold e MH370):

| Distanza | Eccesso sul fondo | Nota |
|---|---|---|
| 1 km | +35 dB | colpo violento, fa saltare l'AGC del BTR |
| 5 km | +20 dB | |
| 10 km | +15 dB | [INPEX09]: "fino a 15 dB" a 5-10 km |
| 30-40 km | +5/+8 dB | udibile per l'esperto [ABA84] |
| > 60 km | 0 | non rilevabile [MH370] |

- In alternativa, per un modello fisico: livello di energia di sorgente **215 dB re 1 µPa²·s·m²**, perdita sferica 20·log r fino a 1 km e poi 10·log r, più **15-25 dB di perdita "da sorgente in superficie"** (dipolo con la superficie). Questa perdita è una STIMA che riconcilia i 260 dB con i +15 dB osservati.
- **Rilevamento**: il colpo arriva dalla direzione vera, ma dura troppo poco per una traccia. Al BTR compare una riga verticale di 1-2 pixel su tutti i rilevamenti vicini (banda larga). Fa da **falso allarme** "transitorio".
- **Tuono nella nube sulla verticale** (entro 0,235·h, cioè ~1 km): onda in acqua = tuono in aria + 32 dB re 1 µPa con passa-basso a 300 Hz. In tutti gli altri casi mandarlo a zero a quota periscopica, tranne un rombo sotto i 20 Hz (non riproducibile su casse normali: di fatto silenzio).

### 5.3 Tuono in aria (solo in superficie: plancia aperta, oppure "attraverso lo scafo")

- **Ritardo** = D / c, con c = 331,3 + 0,606·T(°C).
- **Struttura**, solo per D < 3 km: "clap" iniziale di **0,2-2 s**, fatto di 5-20 micro-impulsi (onde N da 3-20 ms, distanziati in modo casuale di 10-100 ms), banda **50 Hz-3 kHz**. Poi il rimbombo.
- **Rimbombo**: rumore rosa filtrato. Picco spettrale **150-200 Hz** a 1 km, che scende a **60-100 Hz** a 10 km. Modulazione d'ampiezza con 1-4 "clap" secondari, intervallo medio **2 s** [BOD08].
- **Durata**: Δt geometrico (tab. §2.3) **più 5-15 s** per i rami nella nube; il totale va limitato a **2-35 s** [BOD08].
- **Livelli e passa-basso** (STIMA):

| D | Picco | Passa-basso | Clap? |
|---|---|---|---|
| 0,3 km | 125 dB | 4 kHz | sì, violento |
| 1 km | 116 dB | 2 kHz | sì |
| 3 km | 106 dB | 800 Hz | debole |
| 5 km | 102 dB | 500 Hz | no |
| 10 km | 94 dB | 250 Hz | no |
| 15 km | 88 dB | 150 Hz | no |
| > 20 km | — | — | (taglio di rifrazione [FLE49]) |

  Per le scariche nella nube: −10 dB e una durata più lunga. Un'**inversione termica** (notte, inverno) dà +6 dB e sposta il taglio a 30 km [NWS-T].
- **Mix nel gioco**: i numeri sono SPL "fisici". Va mappato in loudness relativa con la dinamica compressa: 116 dB → 0 dBFS, 90 dB → −20 dBFS.

### 5.4 Dentro lo scafo e in plancia

| Situazione | Suoni | Parametri (STIMA salvo dove indicato) |
|---|---|---|
| **Profondità > ~λ/2** (≥ 60-100 m con mare normale, ≥ 150 m con mare lungo da tempesta) | **niente maltempo**; resta solo il fondo di bordo | fondo di bordo 55-65 dB(A) (STIMA; plancia IMO 65 come riferimento [IMO337]) |
| **Quota periscopica, mare 4-6** | scricchiolii strutturali lenti al periodo d'onda (6-12 s); tonfi sordi 20-80 Hz quando un'onda investe vela e alberi; sciabordio; oggetti che scivolano; voci di manovra per tenere la quota | tonfi 60-75 dB(A) di picco, attacco 20-50 ms, decadimento 0,3-0,8 s; rollio udibile come cigolio periodico (sinusoide lenta sul pitch di un rumore filtrato 200-600 Hz) |
| **Snorkel o ventilazione con mare di prua** | colpo della valvola a testa di snorkel; sbalzo di pressione (orecchie) | colpo metallico 80-90 dB(A), ogni 5-20 s con mare grosso [PPRUNE] |
| **In superficie, dentro lo scafo** | colpi di mare (slamming) sulla prua e sulla vela: tonfi strutturali profondi; acqua che corre sul ponte (fruscio a bassa frequenza); tuono vicino come tonfo ovattato (§3.2: −45 dB e passa-basso a 200 Hz); **pioggia inudibile** | slamming 75-85 dB(A), 20-60 Hz più una "campana" metallica a 150-300 Hz con decadimento 1 s, al periodo d'incontro (4-10 s), probabilità crescente con lo stato del mare; tuono = §5.3 − 45 dB (passa-basso 200 Hz) |
| **In plancia (vela, portello aperto)** | vento, spruzzi, pioggia sull'incerata, onde che rompono contro la vela, tuono pieno | vento all'orecchio: **L ≈ 85 + 17·log₂(v / 6,7 m/s) dB** (85 dB a 6,7 m/s, 120 dB a 27 m/s) [SEI17]; pioggia: rumore bianco filtrato 1-8 kHz, 65-80 dB(A) secondo l'intensità; spruzzi: burst di rumore 0,3-1 s a 80-90 dB(A) |
| **Vicino al portello (camera di manovra)** | pioggia e vento "dal tubo" del portello | plancia −20/−25 dB, con risonanza del tronco (passa-banda 100-300 Hz) |

### 5.5 Sala radio (HF in SSB/AM, VLF)

Modello a tre strati sommati sul segnale demodulato:

1. **Fondo gaussiano**, al livello di Fa (galattico o di macchina). Fruscio bianco limitato al filtro (SSB 300-2.700 Hz; AM 200-4.000 Hz).
2. **Crepitio lontano**, cioè scariche globali via ionosfera:
   - Poisson con **20-60 impulsi/s di notte** e **3-10/s di giorno** in HF (STIMA; in VLF fino a ~100/s [MCG20]);
   - ogni impulso è la risposta impulsiva del filtro di banda, durata ≈ 1/B ≈ **0,4 ms** per SSB, con leggera risonanza ai bordi;
   - ampiezze lognormali, σ = 6 dB, **mediana 3-6 dB sotto l'rms del fondo**;
   - si taglia fino a ottenere **Vd ≈ 6-8 dB** (tab. §4.2).
3. **Crash dei temporali regionali (< 500 km) e locali**:
   - un **burst per lampo**, generato dal §5.1;
   - durata lognormale, **mediana 0,5 s** (0,2 s al picco d'attività);
   - dentro il burst **~100-200 impulsi/s di ~1 ms** con ampiezze lognormali (σ ≈ 6 dB), così che circa il 10 % stia entro 6 dB dal picco [AIY77];
   - picco del burst rispetto al fondo: **+30/+40 dB entro 50 km**, **+15/+25 dB a 50-300 km**, **+5/+10 dB oltre**;
   - con un temporale locale il livello di fondo del canale sale di **+10 dB** e si ottengono 10-20 crash al minuto [AIY77].

Demodulazione:
- **AGC**: attacco 2-5 ms, rilascio **0,3-1 s** (STIMA). Dopo ogni crash forte il segnale utile "sparisce" e risale: è l'effetto più riconoscibile.
- **Noise blanker** opzionale: azzera per 1-2 ms gli impulsi oltre +12 dB sull'rms, riduce il crepitio lontano e lascia i crash.

VLF (ricezione in immersione con filo o boa, se il gioco ha un "monitor audio"):
- pop a banda larga 0,3-11 kHz con picco ~5 kHz;
- di notte, frazione di **tweek** dal 20 al 80 %: "ping" che scivola da 8-10 kHz a **1,7 kHz** in 20-40 ms, con armoniche a 3,4 e 5,1 kHz a −10 dB e coda risonante di 30-60 ms;
- **whistler** rari (alle medie latitudini 0-10 all'ora; STIMA): glissando discendente 6 → 0,5 kHz in 0,5-2 s, dispersione ∝ 1/√f.

Nota: i messaggi VLF reali sono dati MSK, non voce. Il monitor audio è una licenza scenica.

---

## 6. Fonti

| Sigla | Riferimento | URL | Affid. |
|---|---|---|---|
| [ABA84] | Arnold R.T., Bass H.E., Atchley A.A. (1984), *Underwater sound from lightning strikes to water in the Gulf of Mexico*, JASA 76(1):320-322, doi 10.1121/1.391065 | https://pure.psu.edu/en/publications/underwater-sound-from-lightning-strikes-to-water-in-the-gulf-of-m/ | alta (abstract) |
| [HIL85] | Hill R.D. (1985), *Investigation of lightning strikes to water surfaces*, JASA 78(6):2096-2099, doi 10.1121/1.392668 | https://pubs.aip.org/asa/jasa/article/78/6/2096/793357/Investigation-of-lightning-strikes-to-water | alta (abstract; numeri via [DOL09]) |
| [DOL09] | Dol H., Ainslie M., de Jong C., Blacquière G., van Riet M. (TNO, 2009), *Natural and anthropogenic sources of sound in the North Sea*, UAM 2009, Nafplion | https://publications.tno.nl/publication/100596/a7fGdm/dol-2009-natural.pdf | media |
| [INPEX09] | URS Australia per INPEX (27/7/2009), *Review of literature on sound in the ocean and on the effects of noise on marine fauna*, App. 15, §5.2.3 | https://www.inpex.com.au/media/ywfp1jpx/draft-eis-technical-appendices-appendix-15-review-of-literature-on-sound-in-the-ocean-and-on-the-effects-of-noise-on-marine-fauna.pdf | media |
| [DOSITS-FAQ] | DOSITS, FAQ (URI), fulmine 260 dB re 1 µPa @ 1 m | https://dosits.org/faq | media (divulgazione scientifica) |
| [DOSITS-AUD] | DOSITS, *Lightning* (audio di Bass, Arnold, Atchley, NCPA), Knowlton 2018 | https://dosits.org/galleries/audio-gallery/other-natural-sounds/lightning/ ; https://dosits.org/?p=17659 | alta (provenienza dell'audio) |
| [NAVY-LFA] | US Navy, SURTASS LFA, *Terminology* (260 dB, cita Hill 1985) | https://www.nepa.navy.mil/surtass-lfa/terminology | media |
| [MH370] | 370location.org (2017), *Ocean lightning strikes compared with acoustic event detections* | https://370location.org/2017/12/ocean-lightning-strikes-compared-with-acoustic-event-detections/ | bassa |
| [NASA00] | Nystuen J.A., Simmon R. (2000), *Listening to Raindrops*, NASA Earth Observatory | https://science.nasa.gov/earth/earth-observatory/listening-to-raindrops/ | media |
| [CIGRE13] | Rakov V.A. (convenor) et al. (2013), CIGRE TB 549 *Lightning parameters for engineering applications* | https://iee.usp.br/sites/default/files/biblioteca/producao/2013/Livros/Rakov.pdf | alta |
| [SABA06] | Saba M.M.F. et al. (2006), video ad alta velocità, Brasile (233 scariche, 3,8 colpi, 61 ms), GRL | citato in [CIGRE13] | alta |
| [SARA10] | Saraiva A.C.V. et al. (2010), Arizona e San Paolo, JGR | https://experts.arizona.edu/en/publications/a-comparative-study-of-negative-cloud-to-ground-lightning-charact/ | alta |
| [RUT94] | Rakov V.A., Uman M.A., Thottappillil R. (1994), JGR, 76 scariche in Florida | citato in [CIGRE13] | alta |
| [WIKIB] | Wikibooks, *Engineering Acoustics/Thunder acoustics* (cita Rakov e Uman 2003, Depasse 1994, Few 1969) | https://en.wikibooks.org/wiki/Engineering_Acoustics/Thunder_acoustics | bassa-media |
| [WIKI-T] | Wikipedia, *Thunder* | https://en.wikipedia.org/wiki/Thunder | bassa-media |
| [FEW67] | Few A.A. et al. (1967), *A dominant 200-hertz peak in the acoustic spectrum of thunder*, JGR 72(24):6149 | https://agupubs.onlinelibrary.wiley.com/doi/10.1029/JZ072i024p06149 | alta |
| [FEW69] | Few A.A. (1969), *Power spectrum of thunder*, JGR 74:6926 | via [RG16], [JOH13] | alta (via sintesi) |
| [RG16] | Abeywardhana et al., *Frequency Analysis of Thunder Features* (ICLP 2016) e scheda ResearchGate | https://www.researchgate.net/publication/309982045_Frequency_Analysis_of_Thunder_Features ; https://archive.cmb.ac.lk/items/2d490745-be2f-49f6-8521-ae3dcefa1628 | media |
| [ABE16] | come [RG16] (fondamentale < 250 Hz) | idem | media |
| [ABE18] | Abegunawardana S. et al. (2018), *Audible Frequency Analysis of Ground Flashes*, ICLP 34 | https://archive.cmb.ac.lk/items/a5cd5ec8-cab8-4195-9aca-57e842fa8241 | media |
| [BOD08] | Bodhika J.A.P. (2008), tesi PhD, Univ. Colombo, *Analysis of thunder generated by lightning cloud and ground flashes* | https://archive.cmb.ac.lk/items/22e6cd73-3db8-4e61-b403-8cfa1417a90b | media |
| [HOL71] | Holmes C.R., Brook M., Krehbiel P., McCrory R. (1971), *On the power spectrum and mechanism of thunder*, JGR 76:2106-2115 | via [JOH13] | media-alta |
| [JOH13] | Johnson R.L. (2013), tesi MS, New Mexico Tech, *Characteristics of thunder and relationships to lightning sources in the Magdalena Mountains* | https://www.nmt.edu/academics/ees/theses/2013t_johnson_rl.pdf | media-alta |
| [NRC86] | National Research Council (1986), *The Earth's Electrical Environment*, cap. sul tuono (Few) | https://nap.nationalacademies.org/read/898/chapter/30 (testo non caricato; usato solo per citazioni secondarie) | alta (non letto per intero) |
| [RUSZ21] | Rusz J., Chum J., Baše J. (2021), *Locating thunder source using a large-aperture micro-barometer array*, Front. Earth Sci. 9:614820 | https://www.frontiersin.org/journals/earth-science/articles/10.3389/feart.2021.614820/pdf | alta |
| [ASS08] | Assink J.D., Evers L.G., Holleman I., Paulssen H. (2008), *Characterization of infrasound from lightning*, GRL, doi 10.1029/2008GL034193 | https://www.knmi.nl/research/publications/characterization-of-infrasound-from-lightning | alta (abstract) |
| [FB10] | Farges T., Blanc E. (2010), *Characteristics of infrasound from lightning and sprites near thunderstorm areas*, JGR 115, A00E31 (e abstract EGU 2010) | https://meetingorganizer.copernicus.org/EGU2010/EGU2010-8273.pdf | media |
| [FAR12] | Farges T., Millet C., Matoza R.S. (2012), *Infrasound from lightning measured in Ivory Coast*, TEA-IS Summer School | https://matoza.faculty.geol.ucsb.edu/files/farges_2012.pdf | media |
| [FAR21] | Farges T. et al. (2021), *Infrasound thunder detections across 15 years over Ivory Coast*, Atmosphere 12(9):1188 | https://www.mdpi.com/2073-4433/12/9/1188 | alta (abstract) |
| [LAC19] | Lacroix A., Farges T., Marchiano R., Coulouvrat F. (2018-2019), ASA 176 e EGU 2019 | https://acoustics.org/1apab1-on-the-origin-of-thunder-reconstruction-of-lightning-flashes-statistical-analysis-and-modeling-arthur-lacroix/ ; https://meetingorganizer.copernicus.org/EGU2019/EGU2019-18393.pdf | media |
| [FLE49] | Fleagle R.G. (1949), *The audibility of thunder*, JASA 21:411 (e Physics Today 1949) | https://physicstoday.aip.org/news/hearing-thunder | alta (via sintesi) |
| [NWS-T] | NWS, *The Sound of Thunder* | https://www.weather.gov/source/zhu/ZHU_Training_Page/lightning_stuff/lightning2/thunder.html | alta |
| [NWS] | NWS, *Lightning science: thunder* (già in `b_sensori`) | https://www.weather.gov/safety/lightning-science-thunder | alta |
| [SOS24] | Sound of Silence Acoustics (2024), *Boom! The science behind thunder's roar* | https://sosacoustics.co.uk/2024/12/06/boom-the-science-behind-thunders-roar/ | bassa |
| [IMO337] | IMO, Risoluzione MSC.337(91), *Code on Noise Levels on Board Ships* (2012) | https://wwwcdn.imo.org/localresources/en/KnowledgeCentre/IndexofIMOResolutions/Documents/MSC%20-%20Maritime%20Safety/337(91).pdf | alta |
| [ROOF] | Euracoustics FA2025 (pioggia ISO 10140-5); Fraunhofer IBP (rumore di pioggia); soundreduction.co.uk; fielders.com.au | https://dael.euracoustics.org/confs/fa2025/data/articles/000725.pdf ; https://www.ibp.fraunhofer.de/en/building-physics-testing/rain-noise-roof-constructions.html ; https://www.soundreduction.co.uk/about-us/news/the-problem-of-noise-created-by-rainfall-on-profiled-metal-roofing-systems/ | media (metodo) / bassa (dB interni) |
| [SEI17] | Seidman M.D. et al. (2017), *Evaluation of noise exposure secondary to wind noise in cyclists*, Otolaryngol. Head Neck Surg. | https://journals.sagepub.com/doi/10.1177/0194599817715250 | alta (abstract) |
| [HUR96] | Lower M.C., Hurst D.W., Thomas A. (1996), *Noise levels and noise reduction under motorcycle helmets*, Inter-Noise 96 | https://www.ioa.org.uk/system/files/proceedings/mc_lower_dw_hurst_a_thomas_noise_levels_and_noise_reduction_under_motorcycle_helmets.pdf | media-alta |
| [PPRUNE] | PPRuNe, *Do submarines feel rough seas?* (ex sommergibilisti) | https://www.pprune.org/military-aviation/452526-do-submarines-feel-rough-seas.html | bassa |
| [WAVEQ] | forum vari (boatdesign.net, Q&A) sulla profondità d'onda | https://www.boatdesign.net/goto/post?id=533159 | bassa |
| [SAURY] | USS Nautilus Museum, *USS Saury (SS-189): the perils of pooping* | https://ussnautilus.org/uss-saury-ss-189-the-perils-of-pooping/ | media |
| [AWM] | Australian War Memorial, S04697, *HMAS Oxley, Oberon class submarine – operational sounds* (~1980) | https://www.awm.gov.au/collection/C1240481 | alta (inventario) |
| [MEM] | Penley G., *Deep Venture*; Conley D., *Down to the Sea in Submarines* (USNI) | https://openroadmedia.com/ebook/deep-venture/9781455616565 ; https://www.usni.org/product/11998 | n.v. (non letti) |
| [P372] | ITU-R P.372-18 (09/2026), *Radio noise* | https://www.itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.372-18-202609-I!!PDF-E.pdf | alta |
| [NTIA85] | Spaulding A.D., Washburn J.S. (1985), *Atmospheric radio noise: worldwide levels and other characteristics*, NTIA Report 85-173 | https://its.ntia.gov/publications/download/85-173.pdf | alta |
| [AIY77] | Aiya S.V.C. (1977), *The interference environment of radio noise from lightning*, J. Indian Inst. Sci. 59(3):113 | https://journal.iisc.ac.in/index.php/iisc/article/view/3472 | media-alta (rassegna con dati di Bangalore) |
| [MCG20] | McGreevy S.P. (2020), *WR-3 Receiver and Natural ELF-VLF Radio Listening Guide* | https://www.archive.org/download/AuroralChorusIiiTheMusicOfTheMagnetosphere-Disc2Of2/WR-3-VLF-Receiver-Listening-Guide%20_Natural_ELF-VLF_Radio_Listening_Guide-May2020version-SPMcGreevy.pdf | bassa-media (osservatore esperto, non revisionato) |
| [WIKI-S] | Wikipedia, *Radio atmospheric signal* | https://en.wikipedia.org/wiki/Radio_atmospheric_signal | media |
| [WIKI-W] | Wikipedia, *Whistler (radio)* | https://en.wikipedia.org/wiki/Whistler_(radio) | media |
| [SUK96] | Sukhorukov A.I. (1996), *ELF-VLF atmospheric waveforms under night-time ionospheric conditions*, Ann. Geophys. 14:33 | https://angeo.copernicus.org/articles/14/33/1996/ | alta (abstract) |
| [OHYA] | Ohya H. et al., abstract SGEPSS sui tweek (10 → 1,6 kHz in 10-50 ms; quote di riflessione) | https://sgepss.org/OLD/sookai/106/pdf/ohya@cute.te.chiba-u.ac.jp-27726.html.jis.pdf | media |
| [OUY] | Ouyang et al., studio cinese sull'attenuazione del tuono (> 500 Hz forte, < 100 Hz trascurabile), Acta Physica Sinica | https://wulixb.iphy.ac.cn/article/app/id/16461/reference | media-bassa (letto solo in sintesi) |
| [SHIPN] | Rumore a bordo: MDPI *Buildings* 13(2):363 (2023, cabine 57,5 dB(A)); rassegna statistica su misure navali (media ~69 dB(A)); Inter-Noise 1985, 428 navi tedesche | https://www.mdpi.com/2075-5309/13/2/363 ; https://trid.trb.org/View/428661 | media |
| [UTEM] | Studio sulla radiazione HF (3 MHz) dei colpi di ritorno, J. Adv. Manuf. Technol. (UTeM) | https://jamt.utem.edu.my/jamt/article/download/5710/3864 | media (letto solo in sintesi) |

---

## 7. Punti n.v. e rischi

1. **Livello di sorgente del fulmine in acqua.** Tutte le fonti aperte risalgono a Hill 1985 (260 dB, 30 µs, ~30 kJ). Il calcolo non è stato verificato sull'originale e TNO stesso lo giudica incerto di "diversi ordini di grandezza". Non è stato trovato **nessun lavoro idroacustico 2000-2025** su fulmini in acqua.
2. **Spettro e durata dell'impulso ricevuto a distanza**: solo descrizioni secondarie (10-1000 Hz, picchi 100-300 Hz; 50-250 Hz a 5-10 km). La durata dell'impulso ricevuto è **n.v.** (STIMA 50-300 ms con multipercorso).
3. **Dispersione degli intervalli fra colpi**: la media geometrica (~60 ms) è solida; la deviazione standard è **n.v.** (STIMA lognormale, σ_ln 0,7).
4. **dB SPL del tuono a distanze fisse**: nessuna tabella misurata aperta. I valori di §2.1 sono costruiti dalle mediane a 1 km [JOH13] e da [RUSZ21], con 1/r e correzione A stimata.
5. **Rumore del maltempo dentro scafi d'acciaio**: le norme IMO lo escludono. Livelli in dB di slamming, pioggia sul ponte e vento nella vela: **n.v.** Le testimonianze trovate sono di forum (bassa).
6. **Spessore dello scafo resistente dei 688**: **n.v.** (STIMA 50 mm per la legge di massa).
7. **Rate di impulsi HF in funzione della distanza**: dati solo per i tropici (Bangalore [AIY77]). Il rate notte/giorno del crepitio lontano è una STIMA.
8. **Fa da P.372**: letto dal grafico (±5 dB). Per mappe stagionali e fasce di 4 ore servono le Fig. 13-36 (disponibili nel PDF scaricato, non trascritte qui).

---

## 8. Che cosa ne ha preso il motore (25/10/2026)

Pezzo M5. Codice in `engine/nucleo/{include/nucleo,src}/audio_meteo.*`, `ascolto.*` e `bordo_audio.*`; voce
`fulmine_mare` in `tools/pipeline/sorgenti_asw.py`; sintesi in `engine/README.md`, «Audio del maltempo».
Differenze dalla proposta del §5:

- **Fulmine in acqua.**
  - Il livello non viene dai 260 dB di [HIL85]: 195 dB rms per 50 ms, tarati sul modello di propagazione del motore
    (sorgente a 1 m, col dipolo della superficie), così da avere +15 dB sul fondo a 10 km come in [INPEX09].
  - Ne risultano ~+23 dB a 1 km, contro i +35 della tabella del §5.2, e niente oltre ~35 km, dove il modello isotermo
    entra in zona d'ombra; [ABA84] li sente a 30-40 km.
  - Il ritardo in acqua (D / 1500) non c'è, come per gli altri transitori della scena.
- **A bordo.**
  - I livelli dei colpi di mare (75-85 dB in superficie) sono applicati al picco lineare, non in dB(A).
  - Un fulmine a 200 m arriva nel locale siluri a ~52 dB a 150 Hz, appena sopra il fondo della barca silenziosa:
    il tonfo netto c'è solo entro ~100 m.
- **Radio.**
  - Il crepitio lontano è stato tarato sul Vd (`tools/re/prove/taratura/scariche.cpp`): mediana degli impulsi 24 dB
    sopra il fondo del ricevitore in HF di notte (Vd ~6 dB) e 15 di giorno.
  - In VLF 36 dB di notte, che danno un Vd ~7 dB: sotto gli 11-16 delle tabelle (§4.2), con 100 impulsi al secondo.
  - I crash sono fatti di impulsi di rumore da 0,5-1,5 ms; l'attacco dell'AGC è di 1 ms (3 nella proposta), perché
    con 3 ms gli impulsi brevi non lo muovevano.
