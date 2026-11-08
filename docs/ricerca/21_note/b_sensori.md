# 21b — Meteo dinamico e sensori: come vento, mare, pioggia, nebbia, temporali, neve e ghiaccio cambiano la scoperta

Nota di ricerca per il dossier 21 (meteo realistico) del remake di *688 Attack Sub* (1985-1990).

**Convenzioni.**
- Confidenza: **alta** = manuale ufficiale o articolo con revisione; **media** = fonte secondaria seria; **bassa** = forum, gioco o lettura incerta.
- **[GIOCO]** = proposta per il motore. **STIMA** = numero inventato o ricavato per il gioco. **[CALC]** = calcolato qui da una formula citata. **n.v.** = non trovato.
- SS = stato del mare (scala Douglas/WMO). La corrispondenza SS ↔ vento ↔ onda è già nel dossier `02` §4.2: qui non si ripete.

**Rimandi, per non duplicare:**
- `02` §4: Wenz, Coates, Knudsen, traffico, pioggia, biologici, ghiaccio.
- `02` §8: tabella qualitativa meteo → effetti. Questa nota la porta a numeri.
- `11` §4-6: orizzonte radar, ESM a senso unico, R50 dei radar contro il periscopio con SS2/SS4.
- `17` §6.3: MAD, con portata ∝ N^(-1/3).
- `20`: periscopi e finestre riscaldate.

---

## 0. Sintesi per chi implementa

1. **Il vento è il parametro principale.** Da solo alza il rumore ambiente sopra ~300 Hz, rimescola lo strato superficiale, fa crescere il clutter di mare, copre il pennacchio del periscopio, rende difficile la quota periscopica e scuote i sensori degli aerei.
2. **Il cattivo tempo aiuta il sottomarino** contro radar, occhio e sonar attivo a media frequenza. **Aiuta poco contro il LOFAR a bassa frequenza** (`02` §4.7). Lo strato rimescolato più profondo può anche **allungare** le portate nel condotto superficiale a bassa frequenza.
3. **La pioggia** pesa su quattro sensori:
   - acustica: 1-30 kHz;
   - radar in banda X: attenuazione ed eco di pioggia;
   - visibilità;
   - poco o niente sul VLF.
4. **La nebbia** annulla l'occhio e il periscopio ottico. Il termico LWIR resiste solo in nebbia leggera e foschia. Il radar resta indifferente.
5. **Il temporale** aggiunge tre effetti:
   - celle visibili al radar fino a ~20 nm, con zone d'ombra dietro;
   - scariche (sferics) in HF e VLF: +10 dB di rumore locale in HF;
   - impulsi acustici dei fulmini in mare, udibili a 30-40 km.

---

## 1. Acustica subacquea

### 1.1 Vento e stato del mare → rumore ambiente

- **Knudsen** (fit in `02` §4.2): `NL(f, SS) = 94,5 − (50/3)·log f + 30·log(SS+1)`, in dB re 1 µPa²/Hz, con f in Hz, valida circa 0,5-25 kHz. Fonte: [KNU-fit] https://arxiv.org/pdf/0712.1833 (media). Il termine di stato del mare `30·log(n+1)` è ripreso dal riferimento [8] dell'articolo.
- **Pendenza:** circa −5 dB/ottava (f^(-5/3)). Il rumore cala più in fretta sopra 15 kHz per l'assorbimento. Fonte: [KNU-fit] (media).
- **Componente del vento nelle curve di Wenz** (`02` §4.1): massimo largo tra 100 Hz e 1 kHz, presente da ~100 Hz a oltre 10 kHz. Fonte: [W62], via tethys e DTIC: https://apps.dtic.mil/sti/pdfs/ADA067515.pdf (alta). Correlazione forte col vento tra 500 Hz e 10 kHz: https://www.mdpi.com/2077-1312/11/12/2226 (media).
- **Saturazione:** con vento oltre ~10-15 m/s lo strato di bolle attenua il rumore di superficie sopra ~10 kHz. Il rumore smette di crescere e può calare (`02` §4.2). Fonte: [NYS-heavy] https://staff.washington.edu/jan4/JANystuenWebsite/Publications/j_pubs/nmc93.html (alta).

**[GIOCO] Correzione di NL per stato del mare, rispetto a SS2.** I valori sono [CALC] dalla Knudsen, uguali a tutte le frequenze tra 0,5 e 25 kHz. Sotto 300 Hz domina il traffico: usare Coates (`02` §4.1).

| SS | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|---|
| ΔNL (dB), formula `30·log((SS+1)/3)` | −14,3 | −5,3 | 0 | +3,7 | +6,7 | +9,0 | +11,0 |
| ΔNL (dB), valori classici di Urick a 1 kHz (44,5/50/55/61,5/64,5/66,5/68,5) | −10,5 | −5 | 0 | +6,5 | +9,5 | +11,5 | +13,5 |
| Limite sopra 10 kHz (bolle) | — | — | — | — | — | max +9 | max +9 (STIMA) |

- Confidenza: alta per Urick, media per il fit.
- La riga di Urick è in `02` §4.2.
- Per il gioco basta una delle due righe. Quella di Urick è più "ripida" tra SS2 e SS3.

### 1.2 Pioggia

**Fatti:**
- **Pioggerella e gocce piccole** (0,8-1,1 mm): fanno un picco caratteristico vicino a **15 kHz**, nella banda 13-25 kHz, per la bolla di "tipo I". Fonti:
  - [NYS-anat] https://staff.washington.edu/jan4/JANystuenWebsite/Publications/j_pubs/mnjso92.html (alta);
  - [NASA-rain] https://earthobservatory.nasa.gov/features/Rain/rain_2.php (media).
- **Pioggia forte:** il picco a 15 kHz **sparisce**. Dominano le gocce sopra 2,2 mm, con bolle a **2-10 kHz** secondo il diametro. Fonti:
  - [NYS-heavy] (alta);
  - [NPS-rain], misure fino a 300 mm/h: https://calhoun.nps.edu/entities/publication/e028e5bd-e633-4b10-b7cd-fc754637e4ed (media).
- **Pioggia con vento oltre 10 m/s:** lo strato di bolle attenua il suono di superficie sopra 10 kHz. Il picco della pioggia si "smussa". Fonte: [NYS-heavy] (alta).
- **Livelli:** la pioggia forte alza lo spettro di 10-30 dB in 1-10 kHz, fino a circa 65-80 dB a 5 kHz con 50-100 mm/h (`02` §4.4, media).

**[GIOCO] Rumore di pioggia per classe.**
- Livelli assoluti in dB re 1 µPa²/Hz, da sommare in potenza al rumore del vento.
- Si usa la formula di `02` §4.4: `N_rain = 50 + 10·log R + 10·log(f/5 kHz)` sotto 5 kHz.
- STIMA, bassa.

| Classe | R (mm/h) | 1 kHz | 5 kHz | 15 kHz | Nota |
|---|---|---|---|---|---|
| Pioggerella | < 1 | trascurabile | ~45 | **~55 (picco)** | firma "da pioggerella" riconoscibile al BTR alto |
| Leggera | 1-2,5 | ~47 | ~53 | ~52 | |
| Moderata | 2,5-7,6 | ~52 | ~58 | ~55 | |
| Forte | 7,6-50 | ~57-60 | ~63-67 | ~60 | picco a 15 kHz assente |
| Violenta / temporale | > 50 | ~63 | ~70-80 | ~65 | con vento > 10 m/s togliere 3-6 dB sopra 10 kHz |

- Classi di intensità: soglie 2,5 e 7,6 mm/h dell'AMS: https://glossary.ametsoc.org/wiki/Rain (media).
- **Confronto:** SS2 a 5 kHz vale 47 dB [CALC]. Una pioggia forte (+15-20 dB) "spegne" quindi il sonar attivo a media frequenza e la larga banda alta, come uno stato del mare 7-8.

### 1.3 Onde frangenti e strato di bolle

- **Effetto di trasmissione a 3 kHz** (modello di bolle Hall-Novarini, sorgente e ricevitore a 7 m): perdita in più di **2,6 dB a 13 m/s** e **8,1 dB a 16 m/s**. Fonte: [APS20] https://wulixb.iphy.ac.cn/article/doi/10.7498/aps.69.20191208 (alta). Va però applicato solo al cammino vicino alla superficie: sonar di scafo, boe superficiali, sottomarino a quota periscopica.
- **Alta frequenza (70-150 kHz):** modelli per vento fino a 30 kn (Bruno e Novarini 1983). Fonte: https://sonicfield.org/library/high-frequency-sound-attenuation-caused-by-the-wind-generated-bubble-layer-in-th (media; testo non letto).
- **A 2-4 kHz** la perdita dei pennacchi di bolle è dovuta soprattutto alla **diffusione**, non all'assorbimento. Fonte: https://mdpi-res.com/d_attachment/jmse/jmse-09-01441/article_deploy/jmse-09-01441.pdf (alta).

**[GIOCO] Perdita aggiunta "bolle"** sul cammino che passa nei primi ~10 m (STIMA, interpolata da [APS20]):

| Vento | < 10 m/s | 13 m/s | 16 m/s | 20 m/s | 25 m/s |
|---|---|---|---|---|---|
| 1-5 kHz | 0 dB | +2,5 | +8 | +12 | +15 |
| > 10 kHz | 0 | +4 | +10 | +15 | +20 |

### 1.4 Ghiaccio

- **Al margine del ghiaccio (MIZ)** il rumore è circa **10 dB sopra** quello sotto il pack fisso o in acqua libera. Lo genera l'onda sui flottanti: flessione e rottura dei lastroni entro pochi km dal bordo. Il dato originale è di Diachok e Winokur (1974).
  - Ripreso in: https://dce2.au.dk/Pub/arcticenvironment/reports/ArcticReport4.pdf (media).
  - Meccanismo in Makris e Dyer: https://acoustics.mit.edu/faculty/makris/Environmental%20correlates%20of%20Arctic%20ice-edge%20noise.pdf (alta).
- **Sotto il pack stabile:** silenzio di 5-10 dB sotto SS0. Rumore impulsivo di fessurazione termica nelle notti fredde (`02` §4.6, media).
- **[GIOCO]** Il vento sul pack non genera rumore "Knudsen": sotto copertura ≥ 9/10 il termine di vento si ignora. Nella fascia di 0-10 km dal bordo si aggiunge **+10 dB** (+15 con SS ≥ 5 in acqua libera; STIMA), con transitori.

### 1.5 Tempeste, strato rimescolato e condotto superficiale

**Fatti:**
- **Spessore di Ekman** (Stewart, eq. 9.16): `D_E = 7,6·U10 / √sin|φ|` (m, con U10 in m/s). Valori tipici **45-300 m**. Fonte: [STEW] https://earthweb.ess.washington.edu/booker/ESS514/stewart/stewart_ocean_book.pdf (alta). È una scala, non la profondità dello strato.
- **Profondità dello strato rimescolato misurata nel Nord Atlantico (53° N):** `h ≈ 0,44·u*/f`, dove u* è la velocità d'attrito lato acqua e f il parametro di Coriolis. Con stratificazione: `h ≈ 1,9·u*/√(N·f)`. Fonte: [LOZ05] https://data-ww3.ifremer.fr/BIB/Lozovatsky_etal_JGR2005.pdf (alta).
- **Uragani:**
  - Gilbert 1988: lo strato si approfondisce di **30-35 m** a destra della traiettoria. Fonte: https://earthref.org/ERR/48739 (media; attribuzione dall'estratto).
  - Eloise 1975: da **33 a 52 m**.
  - Igor 2010, Grand Banks: da **18 a 60 m**. Fonte: https://meetingorganizer.copernicus.org/EGU2012/EGU2012-9908.pdf (media).
  - Meccanismo: taglio della corrente, turbolenza, inglobamento del termoclino. Fonte: https://hurricanescience.org/science/science/hurricaneandocean/index.html (media).

**[CALC] Profondità di equilibrio dello strato col vento** (Lozovatsky, 53° N, Cd ≈ 1,2·10⁻³):

| U10 | 5 m/s | 10 m/s | 15 m/s | 20 m/s | 25 m/s |
|---|---|---|---|---|---|
| h ≈ 0,44·u*/f | ~23 m | ~45 m | ~75 m | ~110 m | ~150 m |
| D_E di Stewart a 45° (scala) | 45 m | 90 m | 136 m | 181 m | 226 m |

**Effetti acustici** (`02` §3; fisica alta, numeri del gioco STIMA):
- Strato più profondo → condotto superficiale più spesso → **frequenza di taglio più bassa**. Più bande restano intrappolate e un sottomarino sotto lo strato diventa più difficile da sentire per una nave in superficie.
- Il mare grosso però **aumenta la fuga dal condotto** (perdita di superficie, §1.6) e il rumore. Risultato netto: con la tempesta il condotto c'è ma "perde".
- **Inverno ad alte latitudini:** strato profondo stagionale (`02` §3, tabella regioni: MLD 200-400 m nell'Atlantico N d'inverno).

**[GIOCO] Regola di rimescolamento** (STIMA):
- `MLD_target = max(MLD_clima, 0,44·u*/f)`.
- Approfondimento con costante di tempo di ~12 h; risalita (riscaldamento diurno, vento calato) in 2-5 giorni.
- Una burrasca di 24 h a 20 m/s porta lo strato estivo da 20-30 m a ~100 m. Il "pomeriggio" estivo (`02` §3.5) scompare finché soffia il vento.

### 1.6 Perdita alla superficie e diffusione

- **Schulkin (1969):** `α_s = 1,64·√(f·h)` dB per ciclo di raggio limite. f è in kHz; h è l'altezza media cresta-cavo in piedi. Valida 1,5-25 kHz; perdita attribuita alle bolle dei frangenti. Confidenza media: formula letta in un estratto di ricerca, primaria non letta.
- **Marsh-Schulkin** (1961-62): perdita di superficie come funzione del prodotto frequenza × altezza d'onda, ricavata da dati di condotto. Revisione NUWC 1994: rifatta con il modello di assorbimento di Mellen, la perdita è **minore per tutti i prodotti F·H**. Fonte: https://auditory.org/asamtgs/asa94mit/3pUW/3pUW22.html (alta). Rassegna "Colossus revisited": https://apps.dtic.mil/sti/pdfs/ADA164588.pdf (alta, non letta).
- **Riflessione coerente** (Rayleigh/Kirchhoff, come in Beckmann-Spizzichino): `R_coh = exp(−2·(k·σ_h·sinθ)²)`. σ_h è lo scarto quadratico dell'elevazione (≈ Hs/4), θ l'angolo radente. Vale per acustica e radar (multipath). Fisica standard, alta. Riferimento sulla diffusione da mare ruvido: https://www.ioa.org.uk/system/files/proceedings/mk_donnelly_rn_hewitt_rough_sea_surface_scattering_with_a_wide_angle_parabolic_equation_model_and_comp.pdf (media).

**[CALC] Schulkin**, dB per ciclo; h ≈ Hs medio di classe, approssimazione STIMA:

| SS (h ft) | 2 (1) | 3 (3) | 4 (6) | 5 (10,7) | 6 (16,4) |
|---|---|---|---|---|---|
| 1 kHz | 1,6 | 2,8 | 4,0 | 5,4 | 6,6 |
| 3,5 kHz (SQS-26/53) | 3,1 | 5,3 | 7,5 | 10,0 | 12,4 |
| 10 kHz | 5,2 | 9,0 | 12,7 | 17,0 | 21,0 |

- [GIOCO] Usare questi valori come `α_L` per salto nel condotto superficiale (`02` §3, che dava 0,5-5 dB "bassa"), **dimezzati**, per tener conto della revisione 1994 (STIMA).

### 1.7 Sonar delle navi di superficie

- **Rumore di flusso:** con cupole di chiglia sopra **18 kn** il sonar è "seriamente ostacolato"; **con mare alto a quelle velocità è praticamente inservibile**. Fonte: brevetto USA 4333169, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4333169 (media).
- Per sonar più piccoli (cacciamine) il flusso domina già sopra **8-10 kn**: https://apps.dtic.mil/sti/pdfs/AD0920130.pdf (media).
- **Bolle trascinate sotto la chiglia** (*bubble sweep-down*, peggiorate dal beccheggio): passano sul trasduttore e degradano trasmissione e ricezione (*quenching*). Non esiste rimedio completo; si interviene sulla forma della prua. Fonte: https://content.iospress.com/articles/international-shipbuilding-progress/isp139 (alta).
- **Altre fonti di rumore proprio:** macchine, eliche, cavitazione, bolle sulla cupola. Fonte: https://apps.dtic.mil/sti/pdfs/AD0632009.pdf (media). Prove della USS Radford con SS1-4 fino a 31 kn, con deformazione della cupola legata ai moti: https://apps.dtic.mil/sti/pdfs/ADA364794.pdf (media).
- **Schiere rimorchiate:** lontane dal rumore della nave, ma **difficili da filare e recuperare con mare alto**. Fonte: https://patents.google.com/patent/US8737172B2 (media). Il moto della nave entra come errore e rumore. Fonte: brevetto USA 5528555, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5528555 (media).
- Degrado quantitativo della schiera con lo stato del mare: **n.v.**

**[GIOCO] Rumore proprio aggiunto** (STIMA, bassa per i numeri):

| SS | 0-3 | 4 | 5 | 6 | ≥ 7 |
|---|---|---|---|---|---|
| Sonar di scafo (SQS-26/53, MG-332…), ΔNL proprio | 0 | +3 dB | +6 dB, vel. max ricerca 15 kn | +10 dB, 20% dei ping persi (*quenching*) | +15 dB, 40% persi, vel. ≤ 10 kn |
| VDS | 0 | 0 | +2 | +4 | recupero obbligato |
| Schiera rimorchiata (SQR-18/19) | 0 | 0 | +1 | +3 (rumore di moto < 50 Hz) | +6; filare o recuperare vietato |
| Sonar del sottomarino a quota periscopica | 0 | +2 | +4 (flusso e moto) | +8 | — |
| Sottomarino sotto 60 m | 0 | 0 | 0 | 0 | 0 |

### 1.8 Boe acustiche

- **Il moto ondoso fa rumore proprio a bassa frequenza** attraverso la sospensione dell'idrofono. Con mare vivo sviluppato e onde **oltre 4 m** sono stati misurati circa **180-280 mPa nella banda 2-20 Hz**. Fonte: https://link.springer.com/article/10.1007/BF00163480 (alta).
- Le boe "scricchiolano e gorgogliano" a intervalli di alcuni secondi, per il moto indotto dalle onde. Fonte: https://apps.dtic.mil/sti/pdfs/AD0392736.pdf (media).
- **Limiti nominali:** una boa moderna (SSQ-955 HIDAR, 2024) è data per **funzionamento fino a SS5 e sopravvivenza fino a SS7**. Fonte: https://umaritime.com/wp-content/uploads/2024/02/SSQ-955-Passive-Search-Sonobuoy-and-variants-HIDAR-Feb-2024.pdf (media; moderna, solo indicativa per il 1989).
- Limiti di SSQ-41/47/53/62 contro lo stato del mare: **n.v.**

**[GIOCO] Boe** (STIMA):
- Al rumore ambiente Knudsen si somma un rumore proprio sotto 100 Hz: **0 dB fino a SS3, +3 a SS4, +6 a SS5, +10 a SS6**.
- Probabilità di guasto all'ammaraggio o del cavo: **5% fino a SS4, 15% a SS5, 30% a SS6**.
- Gli idrofoni profondi (tarati a 90-300 m) risentono meno del moto: dimezzare la penalità.

### 1.9 Sonar a immersione (elicotteri)

- Il corpo del sonar deve stare **quasi verticale e fermo**. Anche **mezzo nodo** d'acqua che scorre fa abbastanza rumore da coprire l'eco di un sottomarino moderno a qualche migliaio di yard. Fonte: storia del Wessex HAS.31B, https://www.faaaa.asn.au/heritage/heritage-wessex/designing-wessex-31b-auto-hover-system/ (media).
- In hover il punto di sospensione si sposta per tenere l'angolo del cavo, secondo il vento. Fonte: https://apps.dtic.mil/sti/pdfs/ADA112502.pdf (media).
- **Limiti di un SOP d'addestramento** (flashcard): niente immersioni con onde oltre **~10 ft**, vento oltre **~17-21 kn** o corrente oltre ~4 kn. Fonte: https://brainscape.com/flashcards/asw-14176474/packs/21629195 (bassa; valori d'addestramento, non di guerra).
- Giornata calda e peso (SH-3H): `17` §2 (alta).
- **[GIOCO]** Immersione normale fino a SS4. A SS5 rumore +6 dB, pericolo per la cupola e 50% di possibilità di rinuncia. Da SS6 o con vento > 35-45 kn niente immersione (`02` §8). STIMA.

### 1.10 Fulmini in mare (suono subacqueo)

- **Arnold, Bass e Atchley (1984):** suoni registrati nel Golfo del Messico e attribuiti a **fulmini caduti in acqua**, con celle temporalesche a **30-40 km**. L'energia acustica stimata è compatibile con l'energia elettrostatica di una scarica tipica. *JASA* 76(1), 320-322. Fonte: https://pure.psu.edu/en/publications/underwater-sound-from-lightning-strikes-to-water-in-the-gulf-of-m/ (alta).
- **Hill (1985):** analizza il contatto della scarica con la superficie e propone un meccanismo di "blow-off" (espulsione) per l'impulso acustico. *JASA* 78(6), 2096-2099. Fonte: https://pubs.aip.org/asa/jasa/article/78/6/2096/793357/Investigation-of-lightning-strikes-to-water (alta per l'abstract).
- Livello di sorgente, spettro e banda: **n.v.** (dietro paywall). Il valore "260 dB" che circola non è stato confermato.
- **Frequenza dei fulmini:** circa **2 colpi per km² all'anno** sull'oceano costiero; **rari in mare aperto**; quasi assenti in Artico e Antartico. Fonte: DOSITS, https://dosits.org/galleries/audio-gallery/other-natural-sounds/lightning (alta).
- **Terra contro mare:** circa 10:1 secondo i dati satellitari LIS/OTD (Christian et al. 2003). Fonte: https://doi.org/10.1029/2002JD002347 (alta; rapporto citato a memoria del lavoro, da verificare).
- **Tuono dall'aria:** entra in acqua solo entro ~13° dalla verticale (finestra di Snell, `17` §8). In pratica un sottomarino non "sente" il tuono lontano, solo i colpi in acqua.

**[GIOCO]** (STIMA):
- Durante un temporale in mare, impulsi a larga banda (sorgente 200-220 dB re 1 µPa @1 m, banda 0,1-10 kHz, durata 50-200 ms) con frequenza di 1-5 al minuto entro 20 km.
- Al BTR sono transitori senza rilevamento stabile: **falsi allarmi** "esplosione/lancio" per l'operatore inesperto.
- Udibili a 30-40 km (Arnold 1984). Nessun dato sul fatto che coprano un siluro.

---

## 2. Radar

### 2.1 Clutter di mare

- **Tabelle di Nathanson** (1969, aggiornate nel 1990 e 2004): riferimento empirico "standard". Polarizzazione HH e VV, radenza 0,1-60°, stati del mare fino a 6. Fonti:
  - https://www.mathworks.com/help/radar/ref/seareflectivity.html (alta);
  - slide secondarie su Scribd (bassa).
- **Modello NRL** (Gregers-Hansen e Mittal 2012), fit delle tabelle di Nathanson, 0,1-60°, 0,5-35 GHz, SS 0-6, errore ~2,2-2,6 dB:
  ```
  σ0(dB) = c1 + c2·log10(sin ψ) + (27,5 + c3·ψ°)·log10(f/1 GHz) / (1 + 0,95·ψ°)
           + c4·(1+SS)^(1/(2 + 0,085·ψ° + 0,033·SS)) + c5·ψ°²
  HH: c1 −73,00  c2 20,78  c3 7,351   c4 25,65  c5 0,00540
  VV: c1 −50,79  c2 25,93  c3 0,7093  c4 21,58  c5 0,00211
  ```
  Fonti: tesi di B. Stade, Oklahoma 2025, tab. 3.3, https://shareok.org/bitstreams/139ca4d5-555c-41c5-8bfc-e0aa4b498c85/download (alta per la trascrizione); validità e errore in MathWorks (alta).
- **GIT** (Horst, Dyer e Tuley 1978): usa vento, altezza d'onda e direzione (sopra/sottovento); 1-100 GHz, 0,1-10°. Formula completa non riletta qui (n.v. in questa sessione).
- Il clutter di mare è **non gaussiano e "a picchi"**. I picchi dei frangenti **durano molti secondi** e si muovono a ~9 kn come un bersaglio: sono il falso periscopio tipico. Fonte: [APL97] https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V18-N01/18-01-Ousbourn.pdf (alta).

**[CALC] σ0 col modello NRL, banda X (9,4 GHz), in dB m²/m²:**

| ψ, pol. | SS0 | SS1 | SS2 | SS3 | SS4 | SS5 | SS6 |
|---|---|---|---|---|---|---|---|
| 0,3°, HH | −72,3 | −62,0 | −54,5 | −48,6 | −43,8 | −39,8 | −36,4 |
| 1°, HH | −66,5 | −56,6 | −49,4 | −43,7 | −39,1 | −35,3 | −32,0 |
| 3°, HH | −61,4 | −52,3 | −45,9 | −40,8 | −36,7 | −33,3 | −30,3 |
| 1°, VV | −60,7 | −52,4 | −46,3 | −41,6 | −37,7 | −34,4 | −31,7 |

- Da SS2 a SS5 il clutter sale di circa **14 dB**.
- Antenna di nave a 25 m su bersaglio a 10 km: ψ ≈ 0,14°. P-3 a 1000 ft su bersaglio a 5 nm: ψ ≈ 1,9°.

### 2.2 Bersagli piccoli: periscopio, snorkel, alberi

**[CALC] Segnale/clutter** (S/C) di un albero da **0,5 m²** contro il clutter medio NRL in HH. Cella = R·θ·ΔR. Ricorda: i picchi reali sono peggio della media.

| Caso | SS0 | SS1 | SS2 | SS3 | SS4 | SS5 | SS6 |
|---|---|---|---|---|---|---|---|
| Nave, antenna 25 m, 10 km, fascio 1,5°, ΔR 150 m | +28 | +18 | +10 | +4 | −1 | −5 | −8 |
| Aereo a 1000 ft, 5 nm, fascio 2,4°, ΔR 150 m (APS-115) | +13 | +4 | −3 | −9 | −13 | −17 | −20 |
| Aereo a 1000 ft, 5 nm, ΔR 0,3 m (APS-116/137, modo periscopio) | +40 | +31 | +24 | +18 | +14 | +10 | +7 |

- Lettura: un radar a impulso lungo perde il periscopio già con SS2-3. La **risoluzione di 1 ft** dell'APS-116/137 riguadagna ~27 dB, ma paga coi **picchi di clutter**.
- **ARPDD:** con mare forza 3, circa **100 rilevazioni automatiche di clutter al secondo**, ridotte a **~1 falso allarme al giorno** solo con molti stadi di discriminazione. Fonte: [APL97] (alta).
- **Curva teorica di sensibilità RCS** per SS1, 2, 3, 5 da 1000 ft: la soglia sale molto con lo stato del mare, finché il limite diventa il rumore. Fonte: [APL97], fig. 3 (alta per la curva, valori non letti).
- **Lo snorkel nella Seconda guerra mondiale** riduceva la portata radar e visiva degli aerei di un fattore **2-3**; molti contatti cadevano "nella zona del ritorno di mare" e venivano persi. Fonte: OEG Report 51, cap. 7, https://www.ibiblio.org/hyperwar/USN/rep/ASW-51/ASW-7.html (alta). Larghezza di scoperta radar e visiva sullo snorkel **1/10-1/100** di quella su un battello in superficie. Fonte: OEG 51, cap. 13, https://www.ibiblio.org/hyperwar/USN/rep/ASW-51/ASW-13.html (alta).
- **RCS del periscopio:** `20` §5 e `11` §5.2 (0,1-1,5 m², STIMA).

**[GIOCO] Moltiplicatore di R50 radar contro periscopio e albero** (riferimento SS2 = 1, coerente con `11` §6.1). STIMA guidata dalla tabella S/C.

| SS | 0 | 1 | 2 | 3 | 4 | 5 | 6+ |
|---|---|---|---|---|---|---|---|
| Radar a impulso normale (navi, APS-115, Wet Eye, navigazione) | 1,3 | 1,15 | 1,0 | 0,6 | 0,35 | 0 | 0 |
| Radar ad alta risoluzione (APS-116/137 modo periscopio) | 1,2 | 1,1 | 1,0 | 0,8 | 0,6 | 0,4 | 0,2 |
| Snorkel con scarico | 1,2 | 1,1 | 1,0 | 0,8 | 0,6 | 0,3 | 0,15 |
| Falsi "periscopi" per ora, radar allertato | 0 | 0 | 1 | 4 | 6 | 10 | 10 |

- In HH. Con VV il clutter è più alto di 1-5 dB alle basse radenze: moltiplicatore ×0,85 da SS2 in su. STIMA.

### 2.3 Pioggia e temporali al radar

**Attenuazione** (ITU-R P.838-3): `γ = k·R^α` dB/km. A 10 GHz in H: k = 0,01217, α = 1,2571. In V: k = 0,01129, α = 1,2156. Fonte: https://itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.838-3-200503-I!!PDF-S.pdf (alta).

[CALC], 10 GHz, polarizzazione H, sola andata (raddoppiare per il radar):

| R (mm/h) | 1 | 2 | 4 | 10 | 25 | 50 | 100 |
|---|---|---|---|---|---|---|---|
| dB/km | 0,012 | 0,029 | 0,07 | 0,22 | 0,70 | 1,66 | 3,98 |

- Dentro 10 km di pioggia a 50 mm/h il radar X perde **33 dB** andata e ritorno: dietro un nucleo temporalesco **non vede**.
- A 3 GHz la perdita è di uno o due ordini di grandezza più bassa. Fonte: https://www.porttechnology.org/technical-papers/effects_of_rain_on_performance_of_maritime_radars/ (media, solo estratto). La pioggia è molto meno grave in banda L. Fonte: corso MIT LL, https://www.ll.mit.edu/media/6971 (alta).
- **Banda UHF/VHF (SATCOM, 240-400 MHz):** attenuazione da pioggia trascurabile (< 0,01 dB/km anche a 50 mm/h; [CALC] dall'andamento di P.838). Bassa la confidenza sul numero preciso, alta sul fatto che sia trascurabile.

**Eco di pioggia** (*rain clutter*):
- Relazione Z-R di Marshall-Palmer: `Z = 200·R^1,6` (mm⁶/m³). Fonte: MIT LL (alta).
- Riflettività di volume `η = π⁵·|K|²·Z/λ⁴`. Fonte: fisica standard (alta).
- RCS della cella di pioggia [CALC] con fascio a pennello 1,5° × 1,5° e ΔR 150 m, cella piena:

| R (mm/h) | dBZ | a 5 km | a 10 km | a 20 km |
|---|---|---|---|---|
| 1 | 23 | 0,1 m² | 0,4 | 1,8 |
| 4 | 33 | 1,0 | 4 | 16 |
| 10 | 39 | 4 | 18 | 71 |
| 25 | 45 | 19 | 77 | 306 |
| 50 | 50 | 58 | 232 | 930 |

- Un periscopio da 0,5 m² dentro una pioggia moderata a 10 km è **sotto l'eco della pioggia**.
- Il Doppler e la polarizzazione circolare (comune sui radar di navigazione) tolgono 10-20 dB di eco di pioggia (STIMA, media sul principio).

**Celle temporalesche:**
- Radar X di bordo (nave da ricerca SPURS-2): celle convettive isolate visibili **fino a ~20 nm**.
- Con pioggia stratiforme tutt'intorno l'eco si ferma a **4-8 nm** per attenuazione.
- Vicino alla nave, echi deboli possono venire da pioviggine, spruzzi o onde.
- Fonte: https://downloads.psl.noaa.gov/psd3/cruises/SPURS2_2016_2017/Xband/radar_manuals_and_background/SPURS2_2017_Xband_radar_readme_EThompson.pdf (media).
- **Zone d'ombra radiali** dietro i nuclei forti in banda X. Fonte: CSU-CHILL, https://www.chill.colostate.edu/w/DPWX/Strong_attenuation_and_differential_attenuation_at_X-Band:_14_July_2015 (media).

### 2.4 Propagazione anomala: condotti

**Condotto d'evaporazione:**
- Altezza media mondiale **~13 m**. Nel Mare d'Irlanda la media è 6,9 m e il condotto esiste più del 94% del tempo. Fonte: https://researchrepository.ul.ie/server/api/core/bitstreams/21909f48-f1b5-4eeb-a8b2-e79d7834a101/content (media).
- **Medio Atlantico** (1978): 2-25 m, media 15 m. Dopo un fronte è passato da 3 a 20 m in ~4 h. Fonte: https://openpolar.no/Record/ftdtic:ADA091851/Details (media).
- **Mediterraneo e Caraibi orientali** hanno condotti mediamente **più alti** dell'Atlantico NE e del Pacifico N, che sono freddi. Fonte: corso NPS, https://www.oc.nps.edu/NWDC_EM_Course/course_materials/module4_4.html (media; pagina non raggiungibile oggi, testo dall'estratto).
- **Mediterraneo orientale** (ERA 1990-2015): massimi **d'estate** tra le 12 e le 15 UTC, i più alti nell'**Egeo**. Fonte: https://calhoun.nps.edu/entities/publication/5a0fe2ea-da88-4f59-9ab2-602a5a1a1a73 (media).
- **Effetti:** in banda X il condotto porta il segnale **oltre l'orizzonte** e crea **buchi di copertura in quota**. Sposta l'ultimo nullo d'interferenza di vari km. Studi della US Navy riportano portate su bersagli bassi **più che raddoppiate** per alcuni radar. Frequenza ottimale indicata: **10-20 GHz**. Fonte: https://apps.dtic.mil/sti/pdfs/ADA278493.pdf (media; dall'estratto, OCR incerto sulla banda).

**Condotti di superficie** (*surface-based*):
- Mappa mondiale (Cahoon e Thayer 1965): massimi **oltre il 75%** nel "Golfo d'Arabia" (Golfo Persico). **Sotto l'1%** su gran parte degli oceani temperati e delle zone artiche e polari d'estate. Comuni ai tropici. Fonte: https://its.ntia.gov/publications/details?pub=2958 (alta).
- **Egeo:** condotti di superficie d'estate circa **il doppio** che d'inverno, per l'umidità. Fonte: https://calhoun.nps.edu/handle/10945/6857 (media).
- Percentuali per l'Atlantico N e il Mediterraneo occidentale: **n.v.**

**[GIOCO] Fattore sull'orizzonte radar ed ESM** (si applica all'orizzonte di `11` §0.2; STIMA):

| Regione / stagione | Probabilità di condotto "forte" in una missione | Banda X/Ku | Banda S/C | VHF/UHF |
|---|---|---|---|---|
| Norvegese, GIUK, Atlantico NE, inverno | 5% | ×1,1 | ×1,0 | ×1,0 |
| Atlantico NE, estate | 15% | ×1,3 | ×1,1 | ×1,0 |
| Mediterraneo occidentale, inverno | 15% | ×1,3 | ×1,1 | ×1,0 |
| Mediterraneo, Egeo, estate (pomeriggio) | 40% | ×1,5-2 | ×1,2 | ×1,05 |
| Golfo Persico, Mar Arabico | 60-75% | ×2-3 | ×1,5 | ×1,1 |
| Artico, sotto il pack | ~0% | ×1,0 | ×1,0 | ×1,0 |

- Con condotto forte l'ESM del sottomarino sente radar oltre l'orizzonte (`11` §4.3: fattore fino a 2).
- Col vento oltre 15 m/s il condotto d'evaporazione sale ma lo strato si rimescola. Il clutter e il multipath del mare grosso annullano il guadagno sui bersagli piccoli: niente bonus contro periscopi se SS ≥ 4 (STIMA).

---

## 3. Elettro-ottica e vista

### 3.1 Visibilità meteorologica

- **Legge di Koschmieder:** `V = ln(1/ε)/β`. Con soglia di contrasto ε = 0,02 si ha `V ≈ 3,912/β` (β in km⁻¹ → V in km). Vale per atmosfera omogenea, bersaglio nero e sfondo di cielo. Fonte: https://codingace.net/physics/visibility_koschmieder.html (media). Ipotesi e varianti: brevetto USA 7016045, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/7016045 (media).
- La portata ottica meteorologica (MOR) del WMO usa ε = 0,05, cioè `V ≈ 3,0/β`. Fonte WMO non riletta: alta come standard noto, n.v. come URL.
- **Classi ICAO di nebbia** (portata visiva): Cat I **1220 m**, Cat II **610 m**, Cat IIIa **305 m**, Cat IIIc **92 m**. Fonte: FLIR, https://www.flir.com/discover/rd-science/can-thermal-imaging-see-through-fog-and-rain/ (media).

**Pioggia → visibilità** [CALC]:
- Si usa l'attenuazione ottica in pioggia di ITU-R P.1814: `γ = 1,076·R^0,67` dB/km. Fonti:
  - riportata da https://arxiv.org/pdf/1506.04836 (media);
  - raccomandazione: https://itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.1814-0-200708-I!!PDF-E.pdf.
- Poi si applica Koschmieder.

| R (mm/h) | 1 | 2,5 | 5 | 12,5 | 25 | 50 | 100 |
|---|---|---|---|---|---|---|---|
| Visibilità (km, ε = 0,02) | 16 | 8,6 | 5,4 | 2,9 | 1,8 | 1,15 | 0,7 |

**Neve:**
- Per uno stesso tasso di neve (equivalente in acqua) la visibilità varia di un fattore **3-10** secondo i cristalli (brinati, aggregati, bagnati). Fonte: Rasmussen et al. 1999, https://impacts.ucar.edu/en/publications/the-estimation-of-snowfall-rate-using-visibility/ (alta).
- Classi USA: **forte ≤ 1/4 mi (400 m)**, **moderata 5/16-5/8 mi (0,5-1 km)**, **leggera > 5/8 mi**. Fonte: idem.
- **Di notte**, a parità di estinzione, la visibilità di luci può essere **oltre il doppio** di quella diurna. Fonte: idem.
- Neve all'ottico, P.1814: esponente 1,38 (secca) e 0,72 (bagnata). Coefficienti "a" incerti nell'estratto. Fonte: https://arxiv.org/pdf/1506.04836 (media).
- **Spruzzi:** da Beaufort 9 (≈ 22 m/s) «gli spruzzi possono ridurre la visibilità». Da Beaufort 10-12 la riducono sempre di più. Fonte: [STEW] tab. 4.1 (alta).

**[GIOCO] Classi di visibilità del motore** (soglie dal codice internazionale di visibilità riportato in https://arxiv.org/pdf/1506.04836, media; moltiplicatori STIMA, vedi §3.4):

| Classe | V (km) | Esempio meteo | Occhio / periscopio | Intensificatore | MWIR 3-5 µm | LWIR 8-12 µm |
|---|---|---|---|---|---|---|
| Eccezionale | > 40 | aria artica, dopo un fronte freddo | 1,0 (limite: orizzonte) | 1,0 | 1,0 | 1,0 |
| Chiara | 20-40 | | 1,0 | 1,0 | 1,0 | 1,0 |
| Foschia leggera | 10-20 | | 0,8 | 0,8 | 0,9 | 0,95 |
| Foschia | 4-10 | pioggia moderata | 0,5 | 0,5 | 0,7 | 0,85 |
| Foschia densa, foschia umida | 2-4 | pioggia forte | 0,3 | 0,3 | 0,45 | 0,6 |
| Nebbia leggera | 1-2 | rovescio violento, neve moderata | 0,15 | 0,12 | 0,25 | 0,45 |
| Nebbia moderata | 0,5-1 | neve forte | 0,07 | 0,05 | 0,1 | 0,3 |
| Nebbia fitta | 0,2-0,5 | | 0,03 | 0,02 | 0,03 | 0,05 |
| Nebbia densa | < 0,2 | | 0,01 | 0,01 | 0,01 | 0,01 |

- Moltiplicatore sulla portata in condizioni chiare, sempre limitata a V × 1,2 per l'occhio.
- Le colonne IR descrivono la **nebbia di radiazione** costiera. Con **nebbia d'avvezione marina** (gocce grandi) e con **pioggia**, l'IR va trattato **come l'occhio** (§3.4).

### 3.2 Avvistamento umano in mare

**Dati della Seconda guerra mondiale** (OEG Report 51, cap. 13; alta), https://www.ibiblio.org/hyperwar/USN/rep/ASW-51/ASW-13.html:

- Larghezza di scoperta visiva da aereo su sottomarino in superficie in moto (miglia):

| Quota | Visibilità 3 mi | Visibilità 15 mi |
|---|---|---|
| 500 ft | 3,0 | 8 |
| 1000 ft | 3,5 | 10 |
| 2000 ft | 3,5 | 11 |
| 10 000 ft | — | 14 |

- Avvistamenti ogni 100 sortite, Biscaglia, maggio 1943:

| Quota di pattuglia | Visibilità 0-4 mi | 5-12 mi | > 12 mi |
|---|---|---|---|
| 0-2000 ft | 6,2 | 14 | 22 |
| Oltre 2000 ft | 7,9 | 26 | 37 |

  Il rapporto con visibilità scarsa è ~0,3, con visibilità media ~0,65.
- **Dal sottomarino:** la portata visiva dalla plancia e al periscopio era «spesso limitata dalla scarsa visibilità», ma poteva arrivare a **12 miglia**. Fonte: Hezlet, https://www.rnsubmusfriends.org.uk/hezlet/volume1/chapter33.htm (media).

**Creste bianche e pennacchio del periscopio:**
- La **piuma** del periscopio è **difficile da distinguere dalle creste bianche**. Si riconosce perché **resta visibile più a lungo**. Fonte: US Navy 1917, https://ncisahistory.org/wp-content/uploads/2019/11/No.-9-Antisubmarine-Warfare-Notes-for-the-use-of-Naval-Armed-Guards-May-1917.pdf (alta, storica).
- **Comparsa delle creste:** Beaufort 3 (≈ 4,9 m/s) «creste che iniziano a rompersi; creste bianche sparse»; Beaufort 4 «numerose»; Beaufort 6 «ovunque». Fonte: [STEW] tab. 4.1 (alta).
- **Copertura di creste bianche** (Monahan e O'Muircheartaigh 1980): `W = 3,84·10⁻⁶·U10^3,41`, che vale 1% a 10 m/s. Fonte: https://researchportal.hw.ac.uk/en/publications/marine-condensation-nucleus-generation-inferred-from-whitecap-sim/ (alta).

[CALC] copertura W:

| U10 (m/s) | 4 | 5 | 7 | 10 | 13 | 15 | 20 | 25 |
|---|---|---|---|---|---|---|---|---|
| W (%) | 0,04 | 0,09 | 0,3 | 1,0 | 2,4 | 3,9 | 10,5 | 22 |

- Il numero di "falsi pennacchi" in vista cresce con W: a 13 m/s c'è una cresta ogni pochi secondi nel campo dell'osservatore.

**Fattori meteo del soccorso in mare (IAMSAR Vol. II)** (bassa-media: valori noti del manuale, tabella non riletta qui; estratto IAMSAR 2019 cap. 5: https://cranston.independent-inquiry.uk/wp-content/uploads/cranston-evidence/INQ010511_Extract_from_IAMSAR__International_Aeronautical_and_Maritime_Search_and_Rescue__Manual__Chapter_5_of_Volume_II__2019_Edition.pdf):

| Vento / onda | Persona in acqua, natanti < 10 m | Altri oggetti |
|---|---|---|
| 0-15 kn o 0-3 ft | 1,0 | 1,0 |
| 15-25 kn o 3-5 ft | 0,5 | 0,9 |
| > 25 kn o > 5 ft | 0,25 | 0,9 |

- Che le prestazioni calino col peggiorare delle condizioni è confermato dal cap. 5 IAMSAR e da https://trid.trb.org/View/152199. Quest'ultimo è uno studio USCG: visibilità, vento, onda lunga, nuvole e sole influenzano la larghezza di scoperta, che **degrada più in fretta** di quanto preveda il manuale (alta).
- Un periscopio è un oggetto "piccolo": colonna sinistra.

**Riflesso del sole** (*sun glint*): nasconde ogni bersaglio basso nel settore del sole e fa scintillare il mare ondulato. Numeri **n.v.**

**[GIOCO] Moltiplicatore della portata visiva sul periscopio e sulla sua piuma** (vedetta, aereo o elicottero; STIMA ancorata a IAMSAR):

| SS (vento) | 0-1 (< 6 kn) | 2 (7-10) | 3 (11-16) | 4 (17-21) | 5 (22-27) | 6+ (> 28) |
|---|---|---|---|---|---|---|
| Periscopio e piuma | 1,3 (mare d'olio: la piuma si vede a miglia) | 1,0 | 0,8 | 0,5 | 0,3 | 0,2 |
| Snorkel con scarico | 1,2 | 1,0 | 0,9 | 0,7 | 0,5 | 0,3 |
| Nave o sottomarino in superficie | 1,0 | 1,0 | 1,0 | 0,9 | 0,9 | 0,8 (Beaufort ≥ 9: spruzzi) |
| Scia di siluro | 1,0 | 0,9 | 0,6 | 0,3 | 0,1 | 0 |

- Questo fattore si moltiplica per quello della classe di visibilità (§3.1) e per la luce (§3.3).

### 3.3 Notte e luna

Illuminamento al suolo (Wikipedia, media; Fraunhofer, alta), https://en.wikipedia.org/wiki/Orders_of_magnitude_(illuminance) e https://publica.fraunhofer.de/handle/publica/270323:

| Condizione | Lux |
|---|---|
| Luna piena, cielo sereno | ~0,25 (altre fonti 0,1) |
| Notte senza luna, stelle e luminescenza del cielo | ~0,002 |
| Notte coperta senza luna | ~0,0001; misurati fino a 0,0005 (dipende dall'inquinamento luminoso); minimi sotto la pioggia |

- Soglie degli intensificatori di II generazione (anni '80): **n.v.**

**[GIOCO]** (STIMA):

| Luce | Occhio nudo | Intensificatore |
|---|---|---|
| Luna piena e serena | ×0,3 | ×0,7 |
| Mezza luna | ×0,15 | ×0,5 |
| Stelle | ×0,05 (solo sagome di navi e scie fosforescenti) | ×0,3 |
| Coperto senza luna | ×0,02 | ×0,1 |

- Il termico non dipende dalla luce.
- La luna bassa dietro il bersaglio raddoppia la portata sulla sagoma (STIMA).

### 3.4 Infrarosso termico e intensificatori in nebbia, foschia e pioggia

Fonte: FLIR, modelli MODTRAN e TTIM; Δt 10 °C, soglia 0,15 K; media.
- **Nebbia Cat II (610 m):** LWIR ≈ **4 volte** la portata dell'occhio e 4 volte quella dell'MWIR. Nella nebbia di radiazione Cat II **solo il LWIR** batte il visibile.
- **Nebbia Cat IIIa-IIIc (< 300 m):** **nessuna differenza** tra IR e occhio.
- **Pioggia:** LWIR e MWIR **si equivalgono**. Calo forte delle prestazioni nei primi 100-500 m.
- **Aerosol marini** (particelle più grandi): danno **le portate IR più basse** in ogni clima. Le nebbie dense hanno gocce più grandi.
- **Umidità:** l'estate umida attenua più dell'inverno. Il LWIR rende meglio d'inverno, l'MWIR nel caldo tropicale.
- Fonte: https://www.flir.com/discover/rd-science/can-thermal-imaging-see-through-fog-and-rain/

Altre fonti:
- Nella nebbia Cat I la trasmissione visibile è molto più bassa di entrambe le finestre IR: https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/7605369 (media).
- Il LWIR è preferito per la nebbia; l'MWIR passa o no secondo la dimensione delle gocce: https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/10948571 (bassa-media).

**Uso 1985-90:**
- Termici sul periscopio: CH84 della Trafalgar (`20` §3).
- FLIR sugli aerei: OR-89 dell'S-3, IRDS del P-3 (`17`).
- [GIOCO] Si usano le colonne della tabella §3.1, con due regole:
  - nebbia "marina d'avvezione" → IR = occhio;
  - pioggia → IR = occhio × 1,2.

### 3.5 Sale, spruzzi e gocce sulle ottiche

- **Gocce sulla finestra del periscopio:** il biofilm che cresce in immersione fa **aderire le gocce** alla finestra di testa e degrada la vista. La pulizia richiede di emergere o di attendere il porto. Fonte: brevetto (anti-incrostazione, campo elettrico), https://patents.justia.com/patent/20080164145 (media).
- **Appannamento interno:** si combatte col vuoto o col gas secco nel tubo (US NRL 1943). Fonte: https://apps.dtic.mil/sti/pdfs/ADD020402.pdf (alta, storica).
- **Finestra riscaldata:** CK34 Barr & Stroud (`20` §3, Jane's, bassa-media).
- Quantità di sale depositato e perdita di contrasto: **n.v.**

---

## 4. Radio ed ESM

### 4.1 HF e scariche atmosferiche

- **ITU-R P.372** dà il rumore radio di fondo da 0,1 Hz a 100 GHz, fulmini compresi. Mappe del fattore di rumore atmosferico Fa per **stagione e blocchi di 4 ore**. Fonte: https://www.itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.372-18-202609-I!!PDF-E.pdf (alta).
- Ai **bassi HF** domina il rumore atmosferico, generato soprattutto dai temporali **tropicali**. Fonte: https://eejournal.ktu.lt/index.php/elt/article/view/454/620 (media).
- **Un temporale locale alza il rumore di ~10 dB** rispetto a un periodo quieto. Con antenne direttive l'Fa varia di ±5 dB secondo la direzione. Fonte: estratto di ricerca su P.372 e misure HF (media).
- Il rumore HF da fulmini è **impulsivo e non gaussiano**: crepitii.

### 4.2 VLF ed ELF in immersione

- **Profondità pellicolare dell'acqua di mare** [CALC], `δ = 1/√(π·f·µ0·σ)` con σ = 4 S/m (tipica 3-6 S/m):

| f | δ | Attenuazione |
|---|---|---|
| 20 kHz | 1,8 m | 4,9 dB/m |
| 24 kHz | 1,6 m | 5,4 dB/m |
| 76 Hz | 29 m | 0,30 dB/m |

  Conducibilità: https://siliconchip.com.au/Issue/2023/March/Underwater+Communication (media).
- **Profondità di ricezione:** VLF **~10-40 m** secondo le fonti, ELF **~100 m**. Fonti: https://en.wikipedia.org/wiki/Communication_with_submarines (media); siliconchip (media).
- **Il meteo non conta.** In immersione la ricezione VLF ed ELF non dipende da onde, pioggia o nebbia. Il limite è il **rumore atmosferico dei temporali su una vasta area geografica**, più forte dei segnali. Fonte: https://apps.dtic.mil/sti/pdfs/AD0646629.pdf (alta). Lo stesso rapporto colloca il massimo di novembre sul Sud America, a 15° S e 60° O.
- Il rumore da fulmini disturba il VLF **tutto l'anno**. Il trattamento numerico lo sopprime. Fonte: https://navysbir.com/06_3/4.htm (media).
- Le scariche (*sferics*) si propagano a scala globale e sono il "fondo" naturale del VLF. Fonte: https://www.radio-workshop.co.uk/g4nsj-vlf-elf-reception-very-low-frequencies/ (bassa-media).
- Il moto ondoso conta solo per l'**antenna a boa o a filo** vicina alla superficie: oscillazioni di profondità di qualche metro valgono 5-15 dB in VLF ([CALC] da δ). Il giocatore lo sente come "segnale che va e viene" con mare grosso.

### 4.3 Satelliti e UHF

- Attenuazione da pioggia trascurabile sotto 1 GHz (§2.3).
- Con mare grosso l'antenna esce e rientra dall'acqua, quindi il collegamento si interrompe (STIMA, fisica ovvia).

### 4.4 ESM e fulmini

- Effetto dei fulmini sui ricevitori ESM a microonde (falsi impulsi): **n.v.**
- Condotti → `§2.4`. Pioggia sul cammino → attenuazione a senso unico (metà dei dB del radar), quindi l'ESM resta in vantaggio anche in pioggia.

---

## 5. MAD: rumore magnetico, turbolenza e onde

- **Fonti di rumore nella banda MAD (0,04-0,4/0,5 Hz):** geologia, campo geomagnetico, **piattaforma**, gradiente, **onde**. Per un aereo il limite non è il sensore ma il rumore della piattaforma e dell'ambiente. Fonte: rapporto POLATOMIC 2000 (confronto con l'ASQ-81), https://apps.dtic.mil/sti/html/tr/ADA390966/index.html (alta).
- **Rumore di manovra:** il campo proprio dell'aereo cambia con l'assetto, finisce nella banda del sottomarino e richiede bobine di compensazione. L'ASQ-81 è citato come magnetometro di riferimento. Fonte: brevetto USA 5182514, https://patents.google.com/patent/US5182514 (media).
- **Prova di compensazione** (*figure of merit*, FOM; ambito geofisico civile): somma dei picco-picco di 12 manovre (rollio ±10°, beccheggio ±5°, imbardata ±5° su 4 prue). Valori accettati da **1,2 a 2,4 nT**; per lavori di precisione < 1 nT. Prima della compensazione, ~9 nT picco-picco nell'esempio. Fonte: https://sbgf.org.br/mysbgf/eventos/expanded_abstracts/14th_CISBGf/Magnetic%20Compensation%20of%20Survey%20Aircraft;%20a%20poor%20man's%20approach%20and%20some%20re-imagination.pdf (media).
- **Turbolenza:** in aria agitata l'aereo fa continue piccole manovre involontarie, quindi più rumore di manovra. Studi quantitativi MAD contro turbolenza: **n.v.**
- **Onde:** l'acqua di mare che si muove nel campo terrestre induce campi magnetici, una fonte di rumore per il MAD (tesi NPS 1991, https://openpolar.no/Record/ftdtic:ADA246589/Details, alta).
  - Con onda lunga di qualche metro, un magnetometro in superficie ha misurato **~5 nT cresta-cavo**. Fonte: ASEG 2003, https://www.earthdoc.org/content/journals/10.1071/ASEG2003ab098 (media).
  - Banda del rumore d'onda 0,4-0,8 Hz (Mar Cinese Meridionale). Fonte: https://jeit.ac.cn/en/article/doi/10.11999/JEIT180026 (media).
  - Il campo decade con la quota; a 100-300 m è molto minore (STIMA).
- **Il Tu-142M spegne il lampeggiante** e altri generatori per le corse MAD (`17`): il rumore di piattaforma conta davvero.

**[GIOCO]** Con `R ∝ N^(-1/3)` (`17` §6.3), rumore base σ 0,1 γ (1 γ = 1 nT); STIMA:

| Condizione | σ | Portata | Falsi contatti MAD |
|---|---|---|---|
| Aria calma, SS ≤ 3 | 0,1 γ | ×1,0 | ~0 |
| Turbolenza moderata o SS 4-5 | 0,2 γ | ×0,79 | 1 ogni 2 h |
| Turbolenza forte, SS ≥ 6, onda lunga | 0,4-0,5 γ | ×0,63-0,58 | 1 ogni 30 min |
| Elicottero col MAD rimorchiato e vento > 35 kn | — | niente corse MAD | |

- **Nota di coerenza:** `17` §6.3 dà ×0,6-0,3 per σ 0,2-0,5 γ. Con la legge alla radice cubica i fattori sono ×0,79-0,58. I valori di `17` reggono solo se si alza anche la soglia per i falsi allarmi. **Da riconciliare** nel dossier 21.

---

## 6. Il periscopio visto dal sottomarino

### 6.1 Onde sulla testa del periscopio

**[CALC] Percentuale di tempo** in cui una finestra a quota h sopra il livello medio è sommersa: superficie gaussiana, σ = Hs/4, `P = Q(4h/Hs)`.

| Hs (SS) \ h | 0,5 m | 1 m | 1,5 m | 2 m | 3 m |
|---|---|---|---|---|---|
| 0,5 m (SS2) | 0% | 0% | 0% | 0% | 0% |
| 1,25 m (SS3-4) | 5,5% | 0,1% | 0% | 0% | 0% |
| 2,5 m (SS4-5) | 21% | 5,5% | 0,8% | 0,1% | 0% |
| 4 m (SS5-6) | 31% | 16% | 6,7% | 2,3% | 0,1% |
| 6 m (SS6-7) | 37% | 25% | 16% | 9% | 2,3% |

- In più vanno contati:
  - il **sollevamento e la variazione di quota** del battello (±0,3-1 m);
  - le **creste interposte** che coprono la linea di vista verso bersagli bassi;
  - gli spruzzi.
- Con SS5 un periscopio esposto 1-1,5 m resta "sotto" il 7-16% del tempo, con lampi di acqua verde sull'oculare. La vista si fa a scatti fra le creste.
- **Alzare il periscopio** per vedere meglio aumenta la sezione radar e la piuma (`11`, `20`).
- **Prassi** (forum, bassa): con mare agitato l'ufficiale di guardia fa **salire un po' il battello** perché la testa passi meno tempo dentro le onde. Fonte: https://forum.rc-sub.com/forum/general-topics/1399-what-is-periscope-depth.

### 6.2 Tenere la quota periscopica con mare grosso

- **Precisione richiesta:** la squadra di governo della quota può dover tenere un battello di centinaia di piedi **entro 1 ft** dalla quota ordinata ed **entro 0,5°** d'assetto. A quota periscopica le forze d'onda sono **«estremamente alte»**, i segnali di quota e assetto molto rumorosi, la dinamica lenta. Fonte: Ware et al. 1981, NASA/JPL, https://ntrs.nasa.gov/citations/19820005816 (alta).
- **Risucchio (*suction*):** le forze di secondo ordine tirano lo scafo **verso la superficie**. Crescono **in modo esponenziale** avvicinandosi alla superficie e sono spesso il **fattore limitante** dell'inviluppo operativo a quota periscopica. Fonti:
  - brevetto USA 6376831, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6376831 (media);
  - tesi NPS, https://calhoun.nps.edu/entities/publication/425b8d9c-01de-48bb-98b2-8007591cfd16 (alta).
- **Rischio di affioramento** (*broaching*), studio CFD sul modello BB2: a bassa quota la forza verticale punta **verso l'alto** e aumenta il rischio. Piccoli cambi di quota o di velocità cambiano molto la forza. Fonte: https://people.eng.unimelb.edu.au/imarusic/proceedings/21/Contribution_766_final.pdf (alta).
- **Simulazione:** risucchio di secondo ordine più forze di primo ordine rendono la quota difficile da tenere. **Più alta l'onda e più bassa la velocità, più facile "galleggiare" in su.** Rimedi: timoni e zavorra. Fonte: https://html.rhhz.net/jckxjsgw/html/67406.htm (media, solo abstract).
- **Prove su modello in mare al traverso** (Crossland): citate in https://www.mdpi.com/2077-1312/13/2/329 (media).
- **Prassi storica:** con mare e onda lunga eccessivi era **impossibile governare il battello a quota periscopica, soprattutto con mare in poppa**. Si scendeva in profondità finché il tempo calava; se il maltempo superava l'autonomia della batteria, si emergeva. Fonte: rapporto di guerra (IMarEST), https://library.imarest.org/record/7881/files/v02b4p13a.pdf (media; testo dall'estratto, pagina non raggiungibile).
- **Moto in profondità:** testimonianze di sommergibilisti (bassa): «sotto i 150 ft era liscio»; con un uragano il battello rollava ancora a 500 ft. Fonte: https://redlib.hbubli.cc/r/submarines/comments/1kuct18/how_deep_to_avoid_storms.

**[GIOCO] Quota periscopica con mare grosso** (STIMA, bassa per i numeri):

| SS | Errore di quota (σ) | Tempo di testa sommersa con 1,5 m esposti | Probabilità di affioramento ogni 10 min a ≤ 5 kn | Note |
|---|---|---|---|---|
| ≤ 3 | 0,3 m | ~0% | 0 | |
| 4 | 0,5 m | 1-3% | 1% | |
| 5 | 1,0 m | 7-15% | 5% (2% a ≥ 6 kn; ×2 con mare in poppa) | vista a scatti; il giocatore deve tenere ≥ 6 kn |
| 6 | 1,5 m | 15-25% | 15% | il radar nemico vede la vela; l'IA del 688/Alfa scende sotto 30 m salvo ordine |
| ≥ 7 | > 2 m | > 25% | 30% | quota periscopica inutilizzabile; antenne a filo/boa; si va sotto 60 m |

- **Rollio e beccheggio sotto la superficie:** si attenuano con la profondità come `exp(−k·z)`, con k = 2π/λ dell'onda. Un'onda lunga 100 m dà un fattore ~0,15 a 30 m e ~0,02 a 60 m [CALC].

---

## 7. Temporali e fulmini: quadro trasversale

| Sensore | Effetto | Fonte | Conf. |
|---|---|---|---|
| Sonar passivo | Impulsi dei fulmini in acqua, udibili a 30-40 km; poi pioggia violenta (§1.2) e vento di raffica | Arnold 1984; Hill 1985 | alta (esistenza); livelli n.v. |
| Udito in superficie | Tuono udibile entro **~10 miglia**; contare i secondi tra lampo e tuono e dividere per 5 dà la distanza in miglia | NWS, https://www.weather.gov/safety/lightning-science-thunder | alta |
| Radar X | Celle visibili fino a ~20 nm; ombra dietro i nuclei; eco di pioggia > 0,5 m² oltre 4 mm/h a 10 km | §2.3 | media / [CALC] |
| HF | +10 dB di rumore con temporale locale, crepitii | §4.1 | media |
| VLF ed ELF | Rumore di fondo dei temporali mondiali; i temporali vicini fanno "click" più forti | §4.2 | alta |
| ESM | Effetto dei fulmini **n.v.**; la pioggia attenua i radar nemici a senso unico | §4.4 | — |
| Vista notturna | **Il lampo illumina il mare e permette di vedere una sagoma?** Nessuna prova documentata | — | n.v. |
| MAD | Turbolenza delle celle: rumore di manovra; gli aerei aggirano le celle | §5 | bassa |

**[GIOCO] Proposte sui temporali** (STIMA):
- **Lampo notturno:** ogni lampo entro 15 km dà per 0,2 s una "luce da luna piena" (§3.3). Si fa un tiro di avvistamento al periscopio o alla vedetta con portata ×0,3 e solo su **sagome di navi** (non periscopi). È una meccanica scenica, non documentata: segnalarla come licenza.
- **Sala radio:** durante il temporale la ricezione HF ha −10 dB di margine: messaggi corrotti e più ripetizioni. Il VLF in immersione resta ricevibile; in mappa si vedono "fulmini" a 1-5 al minuto entro 20 km.
- **Aerei ASW:** evitano le celle (scostamento di 10-20 nm); niente corse MAD dentro le celle; boe posate ai margini.
- **Sonar:** transitori dei fulmini come falsi allarmi; la pioggia violenta copre il sonar attivo a media frequenza.

---

## 8. Tabelle riassuntive per il motore (STIMA salvo dove indicato)

### 8.1 Per stato del mare (riferimento SS2)

| SS | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7+ |
|---|---|---|---|---|---|---|---|---|
| ΔNL 0,5-25 kHz (dB) [CALC, Knudsen] | −14 | −5 | 0 | +4 | +7 | +9 | +11 | +12 (limite bolle sopra 10 kHz) |
| Perdita "bolle" vicino alla superficie, 1-5 kHz (dB) | 0 | 0 | 0 | 0 | 0 | +2 | +8 | +12 |
| α_L condotto, 3,5 kHz (dB/salto, ½ Schulkin) | 0,5 | 1 | 1,5 | 2,7 | 3,8 | 5 | 6,2 | 8 |
| R50 radar contro periscopio, impulso normale | 1,3 | 1,15 | 1 | 0,6 | 0,35 | 0 | 0 | 0 |
| R50 radar contro periscopio, APS-116/137 | 1,2 | 1,1 | 1 | 0,8 | 0,6 | 0,4 | 0,2 | 0,1 |
| Vista su periscopio e piuma | 1,3 | 1,3 | 1 | 0,8 | 0,5 | 0,3 | 0,2 | 0,1 |
| Vista su navi | 1 | 1 | 1 | 1 | 0,9 | 0,9 | 0,8 | 0,6 |
| Sonar di scafo, rumore proprio (dB) | 0 | 0 | 0 | 0 | +3 | +6 | +10 | +15 |
| Boe, rumore proprio sotto 100 Hz (dB) | 0 | 0 | 0 | 0 | +3 | +6 | +10 | guasti |
| Sonar a immersione | ok | ok | ok | ok | ok | +6 dB, 50% rinuncia | no | no |
| MAD, portata | 1 | 1 | 1 | 1 | 0,8 | 0,8 | 0,6 | 0,6 |
| Sottomarino a quota periscopica: testa sommersa | 0 | 0 | 0 | 0 | 2% | 10% | 20% | > 25% |

### 8.2 Per pioggia

| Classe | R (mm/h) | NL a 5 kHz (dB, assoluto) | Radar X, 2 vie (dB/km) [CALC] | Eco di pioggia a 10 km [CALC] | Visibilità (km) [CALC] | IR |
|---|---|---|---|---|---|---|
| Pioggerella | < 1 | ~45 (+ picco a 15 kHz) | < 0,02 | < 0,4 m² | > 16 | = occhio |
| Leggera | 1-2,5 | ~53 | 0,02-0,06 | 0,4-2 m² | 8-16 | = occhio × 1,2 |
| Moderata | 2,5-7,6 | ~58 | 0,06-0,25 | 2-10 m² | 4-8 | idem |
| Forte | 7,6-50 | 63-67 | 0,25-3,3 | 10-230 m² | 1-4 | idem |
| Violenta / temporale | > 50 | 70-80 | > 3,3 | > 230 m² | < 1 | idem |

### 8.3 Per visibilità

Vedi la tabella del §3.1 (moltiplicatori per occhio, intensificatore, MWIR, LWIR). Si moltiplica per §3.2 (mare) e §3.3 (luce).

---

## 9. Fonti principali (con etichette usate sopra)

- **[KNU-fit]** Kurahashi e Gratta, rumore oceanico e neutrini, arXiv 0712.1833 — https://arxiv.org/pdf/0712.1833 — media.
- **[W62]** Wenz 1962 — vedi `02` §11; DTIC ADA067515 — https://apps.dtic.mil/sti/pdfs/ADA067515.pdf — alta.
- **[NYS-anat]** Medwin, Nystuen et al. 1992 — https://staff.washington.edu/jan4/JANystuenWebsite/Publications/j_pubs/mnjso92.html — alta.
- **[NYS-heavy]** Nystuen, Medwin et al. 1993 — https://staff.washington.edu/jan4/JANystuenWebsite/Publications/j_pubs/nmc93.html — alta.
- **[NASA-rain]** NASA Earth Observatory — https://earthobservatory.nasa.gov/features/Rain/rain_2.php — media.
- **[APS20]** Acta Physica Sinica 69 (2020) 024303 — https://wulixb.iphy.ac.cn/article/doi/10.7498/aps.69.20191208 — alta.
- **[STEW]** R. H. Stewart, *Introduction to Physical Oceanography* (2008), tab. 4.1 ed eq. 9.16 — https://earthweb.ess.washington.edu/booker/ESS514/stewart/stewart_ocean_book.pdf — alta.
- **[LOZ05]** Lozovatsky et al., *JGR* 2005 — https://data-ww3.ifremer.fr/BIB/Lozovatsky_etal_JGR2005.pdf — alta.
- **[APL97]** Ousborne et al., ARPDD, *JHU APL Tech. Digest* 18(1) 1997 — https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V18-N01/18-01-Ousbourn.pdf — alta.
- **[NRL12]** Gregers-Hansen e Mittal 2012, coefficienti in B. Stade, tesi 2025 — https://shareok.org/bitstreams/139ca4d5-555c-41c5-8bfc-e0aa4b498c85/download — alta. Validità: https://www.mathworks.com/help/radar/ref/seareflectivity.html — alta.
- **[P838]** ITU-R P.838-3 — https://itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.838-3-200503-I!!PDF-S.pdf — alta.
- **[P1814]** ITU-R P.1814 — https://itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.1814-0-200708-I!!PDF-E.pdf; coefficienti da https://arxiv.org/pdf/1506.04836 — media.
- **[P372]** ITU-R P.372-18 — https://www.itu.int/dms_pubrec/itu-r/rec/p/R-REC-P.372-18-202609-I!!PDF-E.pdf — alta.
- **[OEG51]** Sternhell e Thorndike, *Antisubmarine Warfare in World War II*, 1946 — https://www.ibiblio.org/hyperwar/USN/rep/ASW-51/ — alta.
- **[FLIR]** FLIR, termico in nebbia e pioggia — https://www.flir.com/discover/rd-science/can-thermal-imaging-see-through-fog-and-rain/ — media.
- **[RAS99]** Rasmussen et al. 1999 — https://impacts.ucar.edu/en/publications/the-estimation-of-snowfall-rate-using-visibility/ — alta.
- **[CT65]** Cahoon e Thayer, NBS Report 8864 — https://its.ntia.gov/publications/details?pub=2958 — alta.
- **[SPURS]** Radar X della nave da ricerca SPURS-2 — https://downloads.psl.noaa.gov/psd3/cruises/SPURS2_2016_2017/Xband/radar_manuals_and_background/SPURS2_2017_Xband_radar_readme_EThompson.pdf — media.
- **[ABA84]** Arnold, Bass e Atchley 1984 — https://pure.psu.edu/en/publications/underwater-sound-from-lightning-strikes-to-water-in-the-gulf-of-m/ — alta.
- **[HILL85]** Hill 1985 — https://pubs.aip.org/asa/jasa/article/78/6/2096/793357/Investigation-of-lightning-strikes-to-water — alta (abstract).
- **[DOSITS-L]** DOSITS, suono dei fulmini — https://dosits.org/galleries/audio-gallery/other-natural-sounds/lightning — alta.
- **[NWS-T]** NWS, tuono — https://www.weather.gov/safety/lightning-science-thunder — alta.
- **[WARE81]** Ware et al. 1981, squadra di governo della quota — https://ntrs.nasa.gov/citations/19820005816 — alta.
- **[VLF66]** Ricevitori VLF e rumore dei temporali — https://apps.dtic.mil/sti/pdfs/AD0646629.pdf — alta.
- **[MADPOL]** Rapporto POLATOMIC 2000 sui rumori MAD — https://apps.dtic.mil/sti/html/tr/ADA390966/index.html — alta.
- Le altre fonti sono citate in linea.

---

## 10. Lacune (n.v.) e rischi

- **Fulmini sott'acqua:** livello di sorgente, spettro e durata (Arnold 1984 e Hill 1985 dietro paywall).
- **Lampi** come aiuto all'avvistamento notturno: nessuna prova.
- **Tabelle di Nathanson originali** non lette: si usa il fit NRL. GIT non trascritto.
- Percentuali di **condotti di superficie** per l'Atlantico N e il Mediterraneo occidentale. Altezza media del condotto d'evaporazione nel Mediterraneo (solo "più alto dell'Atlantico NE").
- **IAMSAR:** fattori meteo non riletti sulla tabella; da verificare.
- **Sonobuoy anni '80** (SSQ-41/47/53/62): limiti di stato del mare.
- **Sonar a immersione AQS-13:** limiti operativi di guerra.
- **Periscopio:** degrado del sonar di scafo e della schiera in numeri; quantità di sale sulla finestra; riscaldamento sui periscopi USA Type 2/18; soglie degli intensificatori anni '80.
- **MAD:** rumore contro turbolenza in numeri; incoerenza da riconciliare con `17` §6.3 (§5).
- **Formula di Schulkin:** letta da un estratto; la revisione 1994 dice che le vecchie perdite sono sovrastimate.
