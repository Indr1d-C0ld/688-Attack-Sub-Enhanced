# Registrazioni biologiche e del ghiaccio: candidati con licenza ammessa (punto M)

> Punto M di `docs/23_sonar_blocco3.md` §6. Compilato il 28/10/2026.
> **Nessun file audio è stato scaricato.** Ho letto solo pagine descrittive e metadati: l'API di Wikimedia Commons
> (`extmetadata`, wikitesto, cronologia del caricamento), l'API di audioboom (`audio_clips`), le API di Zenodo e
> dell'Arctic Data Center (schede EML), il listato del bucket NOAA `noaa-passive-bioacoustic` e i JSON di metadati di
> SanctSound, l'indice CDX della Wayback Machine, le pagine di Freesound. Le dimensioni vengono da richieste HEAD;
> **non** ho letto intestazioni con Range (a differenza di R4): dove frequenza o durata non sono scritte sulla pagina,
> sono «non indicate».

Licenze ammesse: CC0, pubblico dominio (opere federali USA, 17 U.S.C. §105), CC BY 3.0/4.0. Escluse: NC, ND, SA,
«solo uso educativo» o personale, licenze non dichiarate. Il contesto viene da `18_note/campioni_r4.md` (voci già in
libreria, esclusioni di SanctSound HI03 e RUSALCA).

Legenda delle attribuzioni (le stringhe per `crediti.toml`; la colonna «Attr.» rimanda qui):

- **A-AWI**: «Registrazione: Alfred Wegener Institute, Helmholtz Centre for Polar and Marine Research (AWI), 2022,
  CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/), dal progetto Polar Sounds (AWI, HIFMB, Cities and
  Memory), <URL del post>. Modificato: sbiancato e ricolorato dal motore del gioco.»
- **A-SWFSC**: «NOAA Fisheries, Southwest Fisheries Science Center, Marine Mammal and Turtle Division, via Wikimedia
  Commons, <pagina del file>» (pubblico dominio: credito di cortesia).
- **A-AFSC**: «NOAA Fisheries, Alaska Fisheries Science Center, Marine Mammal Laboratory,
  https://apps-afsc.fisheries.noaa.gov/Audio/Alaska-Marine-Mammal-Acoustic-Studies/index.html».
- **A-SS**: come in R4: «NOAA Office of National Marine Sanctuaries and U.S. Navy. 2021. SanctSound Raw Passive
  Acoustic Data. NOAA NCEI. https://doi.org/10.25921/saca-sp25 [data di accesso]».
- **A-UW**: «Kathleen M. Stafford, University of Washington, "Passive acoustic data from A3 in the Bering Strait
  <n> of 17", NSF Arctic Data Center, doi:<DOI del pacchetto>, CC BY 4.0. Modificato: estratto, sbiancato e
  ricolorato.»
- **A-PMEL**: come in R4: «NOAA PMEL Acoustics Program, <URL della pagina>».

---

## 1. Tabella riassuntiva delle scelte

| Voce | Scelta consigliata | Licenza | Riserva | Licenza riserva |
|---|---|---|---|---|
| Capodoglio, click | SWFSC «Sperm Whale Normal Clicks» (Commons, 71 s) | PD (`{{PD-USGov-NOAA}}`) | AWI «The sperm whale» (PALAOA, 300 s) | CC BY 4.0 |
| Capodoglio, coda | SWFSC «Sperm Whale Rapid Clicks and Coda» (104 s) **da verificare all'ascolto** | PD (`{{PD-USGov-NOAA}}`) | nessuna registrazione; ritmi dai dati CC0 di Gero et al. per il modello | CC0 (dati, non audio) |
| Foca barbata, trilli | AWI «Bearded seal (Erignathus barbatus)» (120 s) | CC BY 4.0 | Stafford, Bering A3, aprile-giugno 2014 (WAV 16 kHz da 10 min, da cercare) | CC BY 4.0 |
| Ghiaccio marino, pressione e scricchiolii | AWI «Rubbing sea ice, recording 2» (120 s) | CC BY 4.0 | AFSC «Ice Noise» (`bs08-100116-042301-ice.mp3`, Bering) | PD presunto |
| Ghiaccio, fessurazione | AWI «Singing sea ice, recording 1» (62 s) e «Rubbing sea ice, recording 3» | CC BY 4.0 | PMEL «bloop» (icequake, ×16) | PD |
| Tricheco, colpi | AFSC «Walrus» (`walrus-knocks.mp3`) | PD presunto | Stafford, Bering A3 (inverno-primavera, da cercare) | CC BY 4.0 |
| Tricheco, campane | **nessuna registrazione ammessa trovata** | — | — | — |
| Balenottera minore, boing | SWFSC «Minke Whale (boing)» (`MinkWhaBoin.wav`, solo copia Wayback) | PD (NOAA SWFSC) | SanctSound PM05_01 `minkewhale` (Papahānaumokuākea) | PD §105? |
| Balenottera minore, bioduck | AWI «The Antarctic Minke Whale» (300 s) | CC BY 4.0 | Zenodo 6671165 (clip da marcatore, Antartide) | CC0 |
| Orca | (già in libreria: NPS Glacier Bay) + SanctSound MB02_04 `killerwhales` | PD §105 | AWI «The killer whale (Orcinus orca)» (70 s) | CC BY 4.0 |

Note generali:

- **I file AWI sono MP3 a 128 kb/s** (parametro `ab=128` nel rinvio del CDN; le dimensioni lette con HEAD sono
  coerenti). La frequenza di campionamento non è indicata. Vanno bene come tessitura; per l'analisi fine in alta
  frequenza meno.
- **I file AWI si scaricano senza account**: `https://audioboom.com/posts/<id>.mp3` rinvia al CDN.
- **Freesound richiede l'account** dell'utente per gli originali (come in R4); qui nessuna scelta principale ne dipende.

---

## 2. Capodoglio (*Physeter macrocephalus*)

### Candidati

**C1. «Sperm Whale Normal Clicks.ogg»** (Wikimedia Commons) — scelta consigliata per i click.
- Pagina: https://commons.wikimedia.org/wiki/File:Sperm_Whale_Normal_Clicks.ogg
- Licenza: pubblico dominio. Modello `{{PD-USGov-NOAA}}`; campi Commons «Author: National Oceanic and Atmospheric
  Administration», «Source: http://swfsc.noaa.gov/textblock.aspx?Division=PRD&ParentMenuId=148&id=5776».
- Origine verificata: la pagina «Cetacean Sounds» dello SWFSC (copia Wayback del 15/1/2014,
  http://web.archive.org/web/20140115081059/http://swfsc.noaa.gov:80/textblock.aspx?Division=PRD&ParentMenuId=148&id=5776)
  elenca «Sperm Whale (normal clicks)» → `/uploadedFiles/Divisions/PRD/Sounds/SpermWhaleNormalClicks.wav`, nella
  sezione Marine Mammal and Turtle Division. Nessuna dicitura di licenza sulla pagina: è un sito federale.
- Crediti: A-SWFSC. Caricato su Commons da Kurzon il 22/1/2013.
- Formato su Commons: Ogg Vorbis, 44,1 kHz, stereo, 160 kb/s nominali; 71,2 s; 1.484.800 byte.
- Originale WAV: 8.258.961 byte (indice CDX della Wayback, catture 2007-2011); frequenza non indicata.
- Utilità: click regolari d'ecolocalizzazione da una campagna navale con idrofono trainato [D: le campagne SWFSC usano
  array trainati; la pagina non lo dice]. Rumore di fondo e isolamento: non indicati, da ascoltare.

**C2. «Sperm Whale Rapid Clicks and Coda.ogg»** (Commons) — l'unico file intitolato «coda».
- Pagina: https://commons.wikimedia.org/wiki/File:Sperm_Whale_Rapid_Clicks_and_Coda.ogg
- Licenza: `{{PD-USGov-NOAA}}`, stessa fonte SWFSC («Sperm Whales (rapid clicks and coda)» →
  `SperWhaRapidandCoda.wav`). Crediti: A-SWFSC.
- Formato: Ogg Vorbis, 48 kHz, stereo, ~259 kb/s; 104,1 s; 4.408.011 byte.
- **Attenzione:** nell'indice della Wayback `SperWhaRapidandCoda.wav` e `SperWhaRapClik.wav` hanno lo **stesso
  digest** (`GAHYM3LRVW5LINO3ZOIXFDBJ3MHHNXJT`, 14.161.303 byte): sono lo stesso file. Anche su Commons
  «Sperm Whale Rapid Clicks.ogg» dura 104,1 s. Quindi il titolo «coda» non garantisce che ci sia una coda: va
  verificato sullo spettrogramma prima di usarlo per la voce «coda».

**C3. «Sperm Whale Buoy Recording.ogg»** (Commons) — riserva interessante per il gioco.
- Pagina: https://commons.wikimedia.org/wiki/File:Sperm_Whale_Buoy_Recording.ogg
- Licenza: `{{PD-USGov-Military}}`; descrizione «A recording of the clicks from a pair of sperm whales taken by a
  DIFAR buoy operated by the Navy». Fonte e autore: solo «US Navy».
- Formato: Ogg Vorbis, 44,1 kHz, mono, 96 kb/s; 62,6 s; 557.719 byte.
- Utilità: è una **sonoboa DIFAR**, cioè proprio il canale che il gioco simula. Debolezza: la provenienza non è
  verificabile (nessun URL d'origine); il caricatore (Kurzon) è lo stesso degli altri file. Usabile, ma con rischio
  di licenza più alto di C1.

**C4. AWI, «The sperm whale (Physeter macrocephalus)»** (Polar Sounds) — riserva.
- Pagina: https://audioboom.com/posts/8229553-the-sperm-whale-physeter-macrocephalus
- Licenza, dalla descrizione del post: «Recording credit: CC-BY 4.0 Alfred Wegener Institute, Helmholtz Center for
  Polar and Marine Research 2022.» Crediti: A-AWI.
- Contenuto (dal post): click regolari registrati a PALAOA (Antartide), «several overlapping regular click trains»,
  con deboli richiami di foca di Weddell sullo sfondo. Niente code.
- Formato: MP3 128 kb/s; 300,06 s; 4.822.645 byte. Frequenza non indicata.
- Utilità: lungo e con più animali, buono come paesaggio; il fondo ha altri biologici.

**C5. AFSC, «Sperm Whale»** (`sperm_whale.mp3`) — riserva minore.
- Pagina: https://apps-afsc.fisheries.noaa.gov/Audio/Alaska-Marine-Mammal-Acoustic-Studies/index.html
- File: `assets/sperm_whale.mp3`, 209.020 byte (HEAD). Durata e formato oltre a MP3 non indicati.
- Licenza: **nessuna dicitura per il singolo clip**; la pagina accredita solo le illustrazioni («Illustrations: Uko
  Gorter»). È un sito federale (NOAA AFSC): pubblico dominio presunto, non dimostrato.

**C6. «Sperm Whale Creak 2.ogg»** (Commons) — solo per il *creak*.
- https://commons.wikimedia.org/wiki/File:Sperm_Whale_Creak_2.ogg — `{{PD-USGov-NOAA}}`, fonte «NOAA website»
  (nessun URL). Ogg 44,1 kHz mono 96 kb/s, 11,1 s, 171.610 byte. Provenienza vaga: solo riserva.

**Dati per la coda (non audio).** Zenodo 4963528, Gero, Whitehead e Rendell, «Individual, unit, and vocal clan level
identity cues in sperm whale codas» (https://zenodo.org/records/4963528), licenza **CC0** (campo `license:
cc-zero`): un foglio Excel con gli intervalli fra i click delle code (21 tipi, Caraibi). Non è un suono, ma dà al
modello i ritmi veri delle code (per esempio «1+1+3» e «5R1»).

### Scelta

- **Click:** C1. **Riserva:** C4 (licenza CC BY certa e AWI) oppure C3 se si accetta la provenienza vaga.
- **Coda:** C2 solo se l'ascolto conferma una coda; altrimenti la coda resta al modello, coi ritmi CC0 di Gero et al.

---

## 3. Foca barbata (*Erignathus barbatus*)

### Candidati

**F1. AWI, «Bearded seal (Erignathus barbatus)»** — scelta consigliata.
- Pagina: https://audioboom.com/posts/8232592-bearded-seal-erignathus-barbatus
- Licenza: «Recording credit: CC-BY 4.0 Alfred Wegener Institute, Helmholtz Center for Polar and Marine Research
  2022.» Crediti: A-AWI.
- Contenuto (dal post): «typical downsweept trills underneath the Arctic sea ice», prodotti dai maschi in primavera.
  Sito preciso non indicato (Artico).
- Formato: MP3 128 kb/s; 120,0 s; 1.973.784 byte. Frequenza non indicata.
- Utilità: è esattamente il trillo lungo discendente cercato, sotto la banchisa, da idrofono scientifico.

**F2. Stafford, ormeggio A3 dello Stretto di Bering, primavera 2014** — riserva lossless.
- Scheda: «Passive acoustic data from A3 in the Bering Strait 14 of 17 (140423_090000_AU_BS03.wav to
  140603_230000_AU_BS03.wav)», doi:10.18739/A2H65S (https://arcticdata.io/catalog/view/doi:10.18739/A2H65S).
- Licenza, dal campo `intellectualRights` della scheda EML: «This work is licensed under the Creative Commons
  Attribution 4.0 International License.» Autrice: Kathleen M. Stafford, University of Washington. Crediti: A-UW.
- Formato (dall'abstract): «Data were recorded from 10 Hz - 8192 Hz (16 kHz sample rate) the first 10 minutes of each
  hour». Ogni file WAV è di 19.660.844 byte (indice Solr), coerente con 16.384 Hz, 16 bit, mono, 600 s.
- Esempi: `140423_110000_AU_BS03.wav`, `140426_160000_AU_BS03.wav`, `140429_080000_AU_BS03.wav`.
- **Quale file contenga i trilli non è indicato**: da aprile a giugno sono attesi [D, letteratura del dossier 09], ma
  va scaricato qualche file (≈ 20 MB l'uno) e cercato sullo spettrogramma. Banda fino a 8 kHz: il trillo ci sta
  quasi tutto.
- Nota: è lo stesso ormeggio dei clip RUSALCA esclusi in R4; i dati grezzi sull'Arctic Data Center hanno invece una
  licenza esplicita.

**F3. NRS01 (NOAA/AFSC, Artico d'Alaska)** — già in R4 come riserva: PD §105, FLAC a 5 kHz (banda < 2,5 kHz).

### Scelta

- **F1**, riserva **F2**. F3 resta l'alternativa federale, con banda stretta.

---

## 4. Ghiaccio marino: fessurazione, pressione, scricchiolii

### Candidati AWI (Polar Sounds, tutti CC BY 4.0, Oceano Australe, crediti A-AWI)

La licenza è la stessa riga su ogni post: «Recording credit: CC-BY 4.0 Alfred Wegener Institute, Helmholtz Center for
Polar and Marine Research 2022.» Tutti MP3 128 kb/s; frequenza non indicata.

| # | Titolo e pagina | Contenuto (dal post) | Durata | Byte |
|---|---|---|---|---|
| G1 | [Rubbing sea ice, recording 2](https://audioboom.com/posts/8230903-rubbing-sea-ice-recording-2) | «generated when ice floes rub against each other or bend» | 120,0 s | 1.958.840 |
| G2 | [Rubbing sea ice, recording 3](https://audioboom.com/posts/8230904-rubbing-sea-ice-recording-3) | come G1 | 120,0 s | 1.969.196 |
| G3 | [Rubbing sea ice, recording 1](https://audioboom.com/posts/8230218-rubbing-sea-ice-recording-1) | nessuna descrizione | 37,8 s | 629.513 |
| G4 | [Singing sea ice, recording 1](https://audioboom.com/posts/8231522-singing-sea-ice-recording-1) | nessuna descrizione | 61,7 s | 1.056.683 |
| G5 | [Singing sea ice, recording 2](https://audioboom.com/posts/8231521-singing-sea-ice-recording-2) | nessuna descrizione | 25,1 s | non letta |
| G6 | [Narwhal (and squeaky ice)](https://audioboom.com/posts/8231546-narwhal-and-squeaky-ice) | narvali e ghiaccio che cigola (Artico) | 119,1 s | 1.950.369 |
| G7 | [Collapsing shelf ice](https://audioboom.com/posts/8230217-collapsing-shelf-ice) | distacco dalla piattaforma, poi pinnipedi | 26,4 s | 471.648 |
| G8 | [Colliding icebergs, recording 2](https://audioboom.com/posts/8230907-colliding-icebergs-recording-2) | collisione di due iceberg (PALAOA) | 120,0 s | non letta |

- Utilità: G1-G2 sono la «pressione» e gli scricchiolii fra lastroni; G4-G5 il «canto» del ghiaccio. Sono antartici,
  ma il suono del pack è dello stesso tipo. G6 è artico, ma col narvalo sopra. G7 e G8 si sovrappongono alle voci già in
  libreria (`distacco_iceberg`, `incaglio_iceberg`).

### Altri candidati

**G9. AFSC, «Ice Noise»** (`bs08-100116-042301-ice.mp3`) — riserva artica.
- Pagina: https://apps-afsc.fisheries.noaa.gov/Audio/Alaska-Marine-Mammal-Acoustic-Studies/index.html (etichetta «Ice
  Noise» sull'infografica). File `assets/bs08-100116-042301-ice.mp3`, 1.107.439 byte. Il nome fa pensare a un
  ormeggio del Mare di Bering, il 16/1/2010; la pagina non lo dice.
- Licenza: nessuna dicitura per il clip; sito federale NOAA: PD presunto (come C5). Durata e frequenza non indicate.

**G10. PMEL, «Original icequake (bloop) sound»** (`sounds/bloop.wav`) — riserva.
- Pagina: https://www.pmel.noaa.gov/acoustics/sounds_cryogenic.html. Testo: «consistent with icequakes generated by
  large icebergs as they crack and fracture… Recorded signal sped up 16 times.» 162K WAV (dalla pagina).
- Licenza: PD; PMEL chiede di citare la fonte (A-PMEL, come in R4). È ghiaccio d'iceberg, non banchisa; molto
  basso in frequenza (fattore ×16 da togliere).

**G11. Stafford, Bering A3, inverno** — riserva lossless.
- Esempio di pacchetto: doi:10.18739/A2WH3J, «11 of 17 (131219_120000_AU_BS03.wav to 140130_020000_AU_BS03.wav)»;
  licenza CC BY 4.0 nella scheda EML (stessa frase di F2). WAV 16 kHz da 10 min. Dove sia il ghiaccio rumoroso non è
  indicato: da cercare.

**G12. Voermans, «Acoustic recording under landfast sea ice near glacier»** (Zenodo 7750700) — riserva.
- Pagina: https://zenodo.org/records/7750700. Licenza: campo `license: cc-by-4.0`. Autore: Joey Voermans
  (University of Melbourne).
- Contenuto: idrofono sotto il ghiaccio fisso di Tempelfjorden (Svalbard), febbraio 2022, evento dal fronte del
  ghiacciaio «starting at about 0:14». File: `Hydrophone_recording_sample_20220222_013130.wav` (2.932.844 byte) e
  l'originale `.WAV` (57.600.488 byte). Durata e frequenza non indicate.
- Utilità: «sotto la banchisa» vero, ma il suono principale è del ghiacciaio.

**G13. Thalamus_Lab, «Iceberg fragment - Svalbard»** (Freesound 231525) — riserva per `fusione_ghiaccio`.
- https://freesound.org/people/Thalamus_Lab/sounds/231525/ — licenza «Attribution 3.0» (CC BY 3.0) sulla pagina.
  Autore Daniel Blinkhorn (Xperimental Sound Lab). Idrofono, Svalbard 2011. WAV 44,1 kHz, 24 bit, stereo; 60,0 s;
  15,1 MB. **Richiede account Freesound.**

### Scelta

- **Pressione e scricchiolii:** G1, riserva G2 (o G9 per un timbro artico).
- **Fessurazione:** G4 con G2; riserva G10.

---

## 5. Da verificare: tricheco, balenottera minore, orca

### Tricheco (*Odobenus rosmarus*)

**T1. AFSC, «Walrus»** (`walrus-knocks.mp3`) — scelta consigliata per i colpi.
- Pagina AFSC come C5; file `assets/walrus-knocks.mp3`, 1.789.172 byte. Il nome dice «knocks» (colpi). Durata e
  frequenza non indicate. Licenza: PD presunto (nessuna dicitura per il clip).

**T2. Stafford, Bering A3** — riserva: CC BY 4.0 (vedi F2 e G11); trichechi attesi nello Stretto in inverno e
primavera [D], file da cercare.

- **Campane (bells): nessuna registrazione ammessa trovata.** L'unica con le campane è nell'archivio WHOI (1992),
  escluso (vedi §6).
- Freesound 103115 (jeo, «svalbard_phippsoya_20060829.wav», CC BY 4.0) è un gruppo di trichechi **in aria** sulla
  spiaggia (Sennheiser 416): licenza ammessa, ma inutile per un idrofono.

### Balenottera minore (*Balaenoptera acutorostrata* e *B. bonaerensis*)

**M1. SWFSC, «Minke Whale (boing)»** (`MinkWhaBoin.wav`) — scelta per il boing.
- Elencato sulla pagina «Cetacean Sounds» dello SWFSC (copia Wayback citata in C1). Il boing fu attribuito alla
  minore proprio dallo SWFSC (Rankin e Barlow 2005).
- **Esiste solo nella Wayback Machine** (il sito SWFSC non c'è più): cattura del 16/10/2011,
  `https://web.archive.org/web/20111016181104/http://swfsc.noaa.gov/uploadedFiles/Divisions/PRD/Sounds/MinkWhaBoin.wav`,
  `audio/wav`, 4.591.293 byte (indice CDX). Durata e frequenza non indicate.
- Licenza: PD (opera di un centro NOAA); come per C1, nessuna dicitura sulla pagina. Crediti: A-SWFSC (senza Commons).

**M2. SanctSound PM05_01, `SanctSound_PM05_01_minkewhale_20200226T113612Z.wav`** — riserva.
- Sound Library (https://sanctsound.ioos.us/sounds.html): «North Pacific minke whales are known to produce a unique
  sound called the "boing." This minke whale call was recorded at Gardner Pinnacles in Papahānaumokuākea monument.»
- 397.012 byte. Licenza: **PD §105?** Il JSON del pacchetto dà come scienziati Marc Lammers (NOAA ONMS), Eden Zang
  (Lynker, appaltatore) e Anke Kügler (Università delle Hawaii): stesso dubbio del capodoglio HI03 escluso in R4.
- Il clip HI03_02 `minkewhale` (Oahu) ha solo autori non federali: escluso.

**M3. SanctSound SB03_09, `SanctSound_SB03_09_minkewhale_20200502T012119Z.wav`** — minore atlantica (treni di
impulsi: «repeated drum beat sounds… east of Boston in Stellwagen Bank»).
- 3.936.228 byte; scienziato Timothy Rowell; PD §105 [D: Rowell lavora per la NOAA, il JSON non dà l'ente].
  Crediti A-SS. Alternativa alla riserva PMEL `atlmin` di R4.
- Simile, CC BY 4.0: Commons «PAM Minke Beara 22.wav» (Suilann, opera propria, Irlanda 2022), WAV PCM 96 kHz mono,
  835.244 byte (https://commons.wikimedia.org/wiki/File:PAM_Minke_Beara_22.wav).

**M4. AWI, «The Antarctic Minke Whale (Balaenoptera bonaerensis)»** — scelta per il bioduck.
- https://audioboom.com/posts/8227325-the-antarctic-minke-whale-balaenoptera-bonaerensis — CC BY 4.0 (riga AWI).
- Contenuto: «a very unique sound called "bio-duck"… repetitive sequences of pulsed sounds between 50 – 300 Hz with
  harmonics up to 1500 Hz». MP3 128 kb/s; 300,0 s; 4.810.918 byte. Crediti A-AWI.

**M5. Zenodo 6671165, «Antarctic minke whale acoustic data»** — riserva per il bioduck.
- https://zenodo.org/records/6671165 — `license: cc-zero`; autore Ari Friedlaender (UC Santa Cruz).
- Quattro WAV brevi da marcatori su animale (75-143 KB l'uno, per esempio `190226-56_31_230614.wav`). L'abstract
  parla di downsweep e tre richiami nuovi, **non** del bioduck: utile solo come timbro della specie. Rumore di flusso
  probabile (marcatore), da ascoltare.

### Orca (*Orcinus orca*)

La libreria ha già `orca` (NPS Glacier Bay, PD). I file Commons «Killer whale residents broadband.ogg» (85,1 s),
«Killer whale.ogg» e «Killer whale simple.ogg» vengono dalla stessa fonte NPS: probabili doppioni.

**O1. SanctSound MB02_04, `SanctSound_MB02_04_killerwhales_20200414T081659Z.wav`** — seconda variante consigliata.
- Sound Library: «These broadband social calls from killer whales were recorded in Monterey Canyon in Monterey Bay
  sanctuary in 2020» (la descrizione è per Monterey; il clip MB01_05 è dello stesso santuario).
- 718.434 byte. Scienziati John Joseph e Tetyana Margolina (Naval Postgraduate School, federale): PD §105, come i
  clip accettati in R4. Crediti A-SS. Durata e frequenza non indicate.
- Alternative dello stesso tipo: MB01_05 `killerwhale_20200423T164427Z.wav` (435.638 byte); OC03_02 (già fra le
  riserve di R4).

**O2. AWI, «The killer whale (Orcinus orca)»** — riserva.
- https://audioboom.com/posts/8229494-the-killer-whale-orcinus-orca — CC BY 4.0 (riga AWI). Orche antartiche,
  «mainly pulsed calls»; MP3 128 kb/s; 69,9 s; 1.196.113 byte.
- Simili: «Killer whale and Ross seals, recording 1» (120 s) e «recording 2» (60 s), con le foche di Ross.

**Altre ammesse, meno pratiche:**
- SWFSC `KillerWhales.wav` (Wayback, 4.982.773 byte, PD NOAA), come M1.
- Zenodo 4996664, Wellard et al., Bremer Canyon (Australia occidentale): `license: cc-zero`, uno zip di 67.526.832
  byte (contenuto non elencato).
- Zenodo 7657352, Rychen et al., Norvegia settentrionale 2023: `cc-by-4.0`, ma file HDF5 da 0,5-10 GB.
- Freesound 797762 (raiden04), resa binaurale da quei dati: «Attribution 4.0», WAV 48 kHz, 32 bit, stereo, 414,5 s,
  151,8 MB; richiede account. Crediti a raiden04 e a Rychen et al.
- AFSC `killer-whale-1.mp3` (472.942 byte), PD presunto.

### Scelta

- **Tricheco:** T1 per i colpi, riserva T2. Campane senza registrazione: resta il modello.
- **Minore:** boing M1 (riserva M2); bioduck M4 (riserva M5); per l'Atlantico M3.
- **Orca:** tenere NPS; aggiungere O1; riserva O2.

---

## 6. Fonti escluse e motivo

- **Commons «Svalbard Sample for Wiki.ogg»** (foca barbata, Max Eastley, 2007): segnato `{{PD-self}}`, ma caricato
  da «DjDetski», non dall'autore, senza permesso VRTS. Il pubblico dominio non è dimostrato.
- **Commons «Cachalot coda exchange.ogg»** (Jim Nollman): CC BY-SA 3.0.
- **Commons «Sperm Whale Ordinary Clicks.ogg»**: `{{self|cc-zero}}` «own work» di Kurzon, che ha caricato anche
  tutti i file NOAA: che sia davvero una registrazione sua è dubbio. Escluso per prudenza.
- **Freesound MBARI_MARS** (404318 capodoglio, 458854 orche) e **smithereens 749954** (capodoglio, Terranova):
  CC BY-NC 4.0. Nota: l'archivio MBARI Pacific Sound su AWS è CC BY 4.0 (R4); il capodoglio dell'11/10/2016 vi si
  potrebbe ritagliare, ma l'ora non è indicata.
- **SanctSound HI03_02** (capodoglio, minore di Oahu): autori Lynker e Università delle Hawaii (verifica di R4).
- **RUSALCA** (foca barbata e ghiaccio, R4): pagina senza autore né licenza.
- **Watkins Marine Mammal Sound Database (WHOI)**: «free… for personal or academic (not commercial) use»; vietato
  ripubblicare (pagina «About», https://cis.whoi.edu/science/B/whalesounds/about.cfm).
- **Archivio WHOI** (registrazioni Ray e Watkins, tricheco del 1992 con campane): media © WHOI, licenza solo non
  commerciale su richiesta.
- **Macaulay Library**, ML126129 (foca barbata, Point Barrow): «© Donald K. Ljungblad», nessuna licenza libera.
- **DOSITS**: «Copyright © 2002-2025 University of Rhode Island and Inner Space Center. All rights reserved.»; i pochi
  clip CC sono quasi tutti NC (disclaimer https://dosits.org/dosits-disclaimer/).
- **USAP-DC 601416** (McMurdo, crepe del ghiaccio, P. Cziko): CC BY-NC-SA 4.0; inoltre 5,3 TB.
- **Zenodo 16319258** («The underwater song of Erignathus»): CC BY-NC 3.0.
- **North Slope Borough** (MP3 di foca barbata sulla pagina «ice seals»): ente locale, licenza non dichiarata.
- **Orcasound**: di norma CC BY-NC-SA (già escluso in R4).
- **Freesound, ghiaccio d'acqua dolce** (cabled_mess 564351 e 563978, felix.blume 380924, notsawry 841320, rucisko
  342448, dheming 268023): licenze ammesse (CC0 o CC BY 4.0), ma laghi, fiumi o ghiaccio calpestato, non banchisa.
  Da tenere solo come ultima risorsa.
- **NOAA Fisheries, schede specie** (capodoglio, foca barbata, minore, orca) e blog «Diving deep: sperm whale clicks»:
  nessun file audio.
- **Ocean Explorer** (`oceanexplorer.noaa.gov/gallery/sound/`): pagina non più raggiungibile (404); i file noti
  (balenottere, airgun, bloop) sono quelli PMEL già in R4.
- **USGS e BOEM**: nessuna registrazione di ghiaccio o di queste specie trovata.

## 7. Note sulle licenze

1. **AWI (Polar Sounds).** La riga CC BY 4.0 sta su ogni singolo post, ma l'ha scritta il partner Cities and Memory.
   Sul sito AWI (comunicato e idw-online) non ho trovato la licenza ripetuta. I dati PALAOA dell'AWI su PANGAEA sono
   CC BY 3.0/4.0 (per esempio doi:10.1594/PANGAEA.967167), coerente con la riga. Rischio basso; chi vuole la certezza
   può scrivere a Ilse van Opzeeland (AWI, gruppo di acustica oceanica), indicata come referente nel comunicato.
   L'attribuzione deve indicare le modifiche.
2. **SWFSC (C1, C2, M1, orche).** Opere di un centro NOAA: §105. La pagina d'origine non dice chi registrò; i file
   Commons portano il modello `{{PD-USGov-NOAA}}`. M1 e `KillerWhales.wav` si prendono dalla Wayback: è la copia
   d'archivio di un file federale.
3. **AFSC (C5, G9, T1).** Sito federale, nessun credito per singolo clip: PD presunto. Alcuni clip della stessa
   pagina potrebbero venire da collaboratori (per esempio `samana_largevesselnoise.mp3`), quindi rischio medio. Una
   mail all'AFSC (Marine Mammal Laboratory) toglierebbe il dubbio.
4. **Arctic Data Center (F2, G11, T2).** CC BY 4.0 esplicita nella scheda EML di ogni pacchetto, autrice non
   federale: va bene, la licenza basta.
5. **SanctSound.** Come in R4: nessuna licenza sulla scheda NCEI; §105 secondo gli autori del pacchetto (Naval
   Postgraduate School e NOAA sì; Lynker e Università delle Hawaii no).

## 8. Fonti consultate

- Piano: `docs/23_sonar_blocco3.md` §6; R4: `docs/ricerca/18_note/campioni_r4.md`; crediti: `docs/CREDITI.md`.
- Wikimedia Commons API, categorie «Audio files of Physeter macrocephalus», «Audio files of Orcinus orca»,
  «Hydrophones»; ricerche per specie (nessun file utile per tricheco e minore antartica; per la foca barbata solo il file escluso al §6).
- SWFSC «Cetacean Sounds» (copia Wayback 2014) e indice CDX `web.archive.org/cdx/search/cdx`.
- AFSC: https://www.fisheries.noaa.gov/resource/outreach-materials/acoustic-studies-sound-board-marine-mammals-alaska
  e la pagina della tavola sonora.
- Polar Sounds: API `api.audioboom.com/channels/2138625/audio_clips` (164 post che citano Polar Sounds, AWI o PALAOA; 54 hanno la riga CC BY dell'AWI);
  https://citiesandmemory.com/polar-sounds/; https://idw-online.de/en/news808781.
- Arctic Data Center: API Solr e oggetti EML (`arcticdata.io/metacat/d1/mn/v2/`).
- Zenodo API: record 7657352, 7750700, 4996664, 6671165, 4087133, 4963528, 4989942, 7230045.
- SanctSound: listato del bucket `noaa-passive-bioacoustic`, JSON di metadati PM05_01, SB03_09, MB01_05, MB02_04,
  HI03_02; Sound Library https://sanctsound.ioos.us/sounds.html.
- PMEL: https://www.pmel.noaa.gov/acoustics/sounds_cryogenic.html, `sounds_whales.html`.
- Freesound: pagine dei singoli suoni citati; USAP-DC 601416; Macaulay ML126129; DOSITS.

## 9. Esito (28/10/2026)

L'utente ha dato il via alle nove scelte del §1 (le riserve solo se una scelta non si scaricava: non e' servito).
- **Scaricate** in `tools/dl/29_biologici/<voce>/`, una cartella nuova per voce, con le licenze ricontrollate sulle
  pagine il giorno stesso (invariate). Otto file hanno la dimensione attesa; `MinkWhaBoin.wav` e' di 6.633.372 byte
  (l'indice CDX dava la lunghezza del record compresso), con l'impronta SHA-1 uguale a quella della Wayback.
- **Coda del capodoglio, scartata.**
  - Su Commons «Rapid Clicks» e «Rapid Clicks and Coda» sono file diversi, ma i WAV d'origine dello SWFSC sono lo stesso.
  - Sul segnale (`tools/re/prove/m_coda/clic.py`): 342 click in raffiche irregolari, con piu' animali ed echi. Su 35
    gruppi separati da pause, solo quattro sono regolari, e uno solo ha pause nette: 4 click a 47,9 s, ICI 0,165 s,
    1,4 s di silenzio prima e 3 s dopo. Non basta: la coda resta al modello.
- **Bioduck:** scaricato ma non collegato, perche' il gioco non ha un teatro antartico.
- **Collegate nella libreria:** sette (`campioni.toml`: capodoglio, foca_barbata, fessurazione_ghiaccio e _2,
  tricheco, balenottera_minore_pacifica, orca_2), coi crediti in `docs/CREDITI.md` e `data/motore/crediti.toml`.
