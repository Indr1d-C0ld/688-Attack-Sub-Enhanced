# R4 — Elenco curato dei campioni per la libreria (da far approvare prima di scaricare)

> Pezzo R4 del dossier `docs/ricerca/18_audio_reale.md` (§8). Licenze e archivi da §1, uso dei campioni da §2
> (tessitura sbiancata e ricolorata dal modello: il livello per banda, le righe e il Doppler restano del modello).
> Voci del catalogo: `tools/pipeline/sorgenti_libreria.py` (paesaggio) e `tools/pipeline/sorgenti_asw.py` (transitori).
> Compilato il 25/10/2026. **Nessun file audio è stato scaricato.** Ho usato solo: pagine web, listati JSON/XML dei
> bucket pubblici (Google Cloud `noaa-passive-bioacoustic`, S3 `pacific-sound-*`), i JSON di metadati dei clip
> SanctSound e richieste HEAD. Per frequenza, bit e canali ho letto **solo l'intestazione** del file con una richiesta
> Range di 64-512 byte (nessun campione salvato): lo segnalo perché tecnicamente è una lettura parziale.

Vincoli applicati: solo CC0, pubblico dominio (opere federali USA, 17 U.S.C. §105) e CC BY 4.0; esclusi CC BY-NC,
ShareAlike, Watkins, DOSITS, ShipsEar, AWM, Orcasound. Freesound: originali scaricabili solo con un account, quindi
ogni suo file è marcato **«richiede account dell'utente»**.

Legenda della colonna «Licenza»: **PD-§105** = opera federale USA ([17 U.S.C. §105](https://www.law.cornell.edu/uscode/text/17/105));
**PD-§105?** = federale per pubblicazione, ma autore materiale non federale o non dichiarato (vedi «Note sulle licenze»).
Le dimensioni sono `Content-Length` letti con HEAD. Tutti i server proposti rispondono `Accept-Ranges: bytes`; per
PMEL, il bucket NOAA (GCS) e MBARI (S3) ho verificato anche la risposta `206 Partial Content`.

Abbreviazioni delle basi URL:

- `PMEL` = `https://www.pmel.noaa.gov/acoustics/`
- `SS` = `https://storage.googleapis.com/noaa-passive-bioacoustic/sanctsound/products/sound_clips/`
- `NPSA` = `https://www.nps.gov/nps-audiovideo/audiovideo/`
- `RUS` = `https://www.pmel.noaa.gov/rusalca/sites/default/files/atoms/audio/`

Attribuzioni (le stringhe da mettere nei crediti del gioco; la colonna «Attr.» rimanda qui):

- **A-PMEL**: «NOAA PMEL Acoustics Program, <URL della pagina da cui è preso il file>» (richiesto da
  <https://www.pmel.noaa.gov/acoustics/multimedia.html>: «please cite NOAA PMEL Acoustics Program as the source, and
  include the appropriate URL»).
- **A-SS**: «NOAA Office of National Marine Sanctuaries and U.S. Navy. 2021. SanctSound Raw Passive Acoustic Data.
  NOAA National Centers for Environmental Information. https://doi.org/10.25921/saca-sp25 [data di accesso]» (è la
  citazione scritta nel campo `CITATION` del JSON di metadati di ogni pacchetto di clip).
- **A-NPS**: «National Park Service, Glacier Bay National Park & Preserve — <autore indicato sulla pagina, es.
  C. Gabriele / NPS> — https://www.nps.gov/glba/learn/nature/soundclips.htm» (la pagina NPS chiede «a citation or
  acknowledgement of the National Park Service as the source»: <https://www.nps.gov/aboutus/disclaimer.htm>).
- **A-NMFS**: «NOAA Fisheries, Northeast Fisheries Science Center, Passive Acoustics Group —
  https://www.fisheries.noaa.gov/new-england-mid-atlantic/science-data/fish-and-invertebrate-acoustics» (la pagina
  delle politiche chiede «appropriate acknowledgement»: <https://www.fisheries.noaa.gov/website-policies-and-disclaimers>).
- **A-RUS**: «NOAA PMEL, RUSALCA (Russian-American Long-term Census of the Arctic) —
  https://www.pmel.noaa.gov/rusalca/sounds-arctic-ocean/».
- **A-MBARI**: «Pacific Ocean Sound Recordings, Monterey Bay Aquarium Research Institute (MBARI), CC BY 4.0
  (https://creativecommons.org/licenses/by/4.0/). Accessed on <DATA> from https://registry.opendata.aws/pacific-sound.
  Modificato: estratto di 55-60 s, sbiancato e ricolorato dal motore del gioco.»
- **A-NRS**: «NOAA OAR Pacific Marine Environmental Laboratory, National Marine Fisheries Service, NOS Office of
  National Marine Sanctuaries, and DOI National Park Service. 2014. NOAA Ocean Noise Reference Station Network Raw
  Passive Acoustic Data. NCEI. https://doi.org/10.7289/V5M32T0D».
- **A-FS**: CC0 non chiede nulla; credito di cortesia «"<titolo>" by <utente> (https://freesound.org/s/<id>/), CC0».

---

## 1. Biologici

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 1 | `balenottera_comune` | `SS` `sb03/sanctsound_sb03_05_sound_clips/data/SanctSound_SB03_05_finwhale_20190923T074930Z.wav` (Stellwagen Bank, Atlantico, settembre 2019) | PD-§105 (NOAA ONMS e U.S. Navy; scienziati NEFSC e WHOI/NOAA) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 5.760.046 | 60,0 s, tempo reale | intero |
| 2 | `megattera` | `NPSA` `cde82ded-68b8-4c67-b6c3-9ab190908357.mp3` — «humpback whale feeding call», Glacier Bay, luglio 2000 (è la chiamata d'alimentazione estiva che il catalogo modella) | PD-§105 ([pagina](https://home.nps.gov/glba/learn/nature/soundclips.htm): «recordings … made by the National Park Service») | A-NPS | MP3 44,1 kHz, 128 kb/s, mono | 703.236 | ~44 s | intero |
| 3 | `capodoglio` | `SS` `hi03/sanctsound_hi03_02_sound_clips/data/SanctSound_HI03_02_spermwhale2_20191216T115715Z.wav` (Oahu sud-orientale) | PD-§105? (autori del pacchetto: Lynker LLC e University of Hawaii) | A-SS | WAV PCM 44,1 kHz, **32 bit**, mono | 1.054.776 | 6,0 s | intero (da ripetere in loop col ritmo del modello) |
| 4 | `orca` | `NPSA` `e60488c2-30fb-414b-bc9f-ee16f886a897.mp3` — «Resident Killer Whale Vocalizations», Glacier Bay | PD-§105 (credito «NPS / C. Gabriele») | A-NPS | MP3 44,1 kHz, 64 kb/s, mono | 681.481 | ~85 s | 60 s col maggior numero di chiamate: si sceglie dopo il download con l'inviluppo in 1-6 kHz |
| 5 | `delfini` | `SS` `ci04/sanctsound_ci04_05_sound_clips/data/SanctSound_CI04_05_dolphins_20200921T232749Z.wav` (Channel Islands) | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 5.760.280 | 60,0 s | intero |
| 6 | `foca_barbata` | `RUS` `BeringStfeb2010_beardedseal1.mp3` (stretto di Bering, febbraio 2010) | PD-§105? (pagina NOAA PMEL senza credito; vedi note) | A-RUS | MP3 44,1 kHz, 64 kb/s, mono | 451.396 | ~56 s | intero |
| 7 | `merluzzo` | `SS` `sb01/sanctsound_sb01_01_sound_clips/data/SanctSound_SB01_01_atlanticcod_20181113T024154Z.wav` (grugniti di riproduzione, Stellwagen, novembre) | PD-§105 (NEFSC, WHOI/NOAA) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 480.200 | 5,0 s | intero |
| 8 | `eglefino` | `https://www.fisheries.noaa.gov/s3/2023-04/Meae-knock-NOAA-PAGroup-01-haddock-clip.mp3` (colpi di eglefino) | PD-§105 (NOAA NEFSC, «not subject to copyright»: [politiche](https://www.fisheries.noaa.gov/website-policies-and-disclaimers)) | A-NMFS | MP3 8 kHz, 24 kb/s, mono (banda utile < 4 kHz, sufficiente: l'eglefino sta sotto 600 Hz) | 91.197 | ~30 s | intero |
| 9 | `coro_pesci` | `SS` `gr01/sanctsound_gr01_02_sound_clips/data/SanctSound_GR01_02_fishchorus_20190902T004059Z.wav` (Gray's Reef, Georgia, notte di settembre) | PD-§105 (NEFSC, WHOI/NOAA, GRNMS) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 480.228 | 5,0 s | intero (loop granulare) |
| 10 | `gamberi` | `SS` `ci01/sanctsound_ci01_03_sound_clips/data/SanctSound_CI01_03_snappingshrimp_20190906T230827Z.wav` | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 5.760.236 | 60,0 s | intero |

## 2. Geofisici e ghiaccio

I file PMEL sono già accelerati per renderli udibili: il fattore va scritto nei metadati della voce, perché la
pipeline lo deve togliere (o usare per l'ascolto accelerato, §2.2 punto 8 del dossier). Banda reale = 4 kHz / fattore.

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 11 | `t_phase_tettonico` | `PMEL` `pioneer/sounds/pio_eq_244_1159.wav` («Earthquake», [pagina](https://www.pmel.noaa.gov/acoustics/sounds_seismic.html)) | PD-§105 | A-PMEL | WAV PCM 8 kHz, 16 bit, mono; **×10** | 291.244 | 18,2 s (≈ 182 s reali) | intero |
| 12 | `t_phase_vulcanico` | `PMEL` `sounds/whistle16x.wav` («Eruption», stessa pagina; la pagina scrive «speed 10x», il nome del file 16x: da verificare sullo spettrogramma) | PD-§105 | A-PMEL | WAV PCM 8 kHz, 16 bit, mono; ×10 o ×16 | 827.430 | 51,7 s | intero |
| 13 | `distacco_iceberg` | `PMEL` `sounds/A53JD33Calving_3x.wav` (iceberg A53, [pagina](https://www.pmel.noaa.gov/acoustics/sounds_cryogenic.html)) | PD-§105 | A-PMEL | WAV PCM 8 kHz, 16 bit, mono; **×3** | 4.515.910 | 282 s (≈ 14 min reali) | 60 s (del file) attorno al colpo più forte: non è indicato, si trova dopo il download col massimo dell'inviluppo RMS (file piccolo, conviene scaricarlo intero) |
| 14 | `tremore_iceberg` | `PMEL` `sounds/HarmonicTremor2006_215_09_20UsedOnBloopWebsite.wav` (40 Hz e armoniche) | PD-§105 | A-PMEL | WAV PCM **999 Hz** (intestazione), 16 bit, mono; ×3 | 277.492 | 138,9 s | intero |
| 15 | `incaglio_iceberg` | `PMEL` `sounds/noise97139.wav` («Iceberg Grounding») | PD-§105 | A-PMEL | WAV PCM 8 kHz, 16 bit, mono; **×16** | 289.628 | 18,1 s (≈ 290 s reali) | intero |
| 16 | `fessurazione_ghiaccio` (e tessitura «ghiaccio» d'ambiente) | `RUS` `BeringStfeb2010_icenoise.mp3` («Sea Ice Noise», Bering, febbraio 2010) | PD-§105? (come #6) | A-RUS | MP3 44,1 kHz, 64 kb/s, mono | 223.399 | ~28 s | intero |
| 17 | `fusione_ghiaccio` | `https://www.nps.gov/nps-audiovideo/legacy/mp3/akr/avElement/glba-seltzer.mp3` («Bergy Seltzer», [galleria](https://home.nps.gov/glba/learn/nature/glaciers-soundscape-gallery.htm)) | PD-§105 (credito «H. Lentfer / NPS») | A-NPS | MP3 44,1 kHz, 256 kb/s, **stereo** | 503.505 | ~16 s | intero; la pagina non dice se è idrofono o microfono in aria: da verificare all'ascolto |

## 3. Tessiture d'ambiente (pioggia, vento e onde)

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 18 | pioggia sul mare (rumore ambiente, M5) | `SS` `mb01/sanctsound_mb01_05_sound_clips/data/SanctSound_MB01_05_rain_20200816T093204Z_28dBgain.wav` (Monterey Canyon, 2020) | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono (guadagno digitale +28 dB già applicato: controllare il clipping) | 5.760.204 | 60,0 s | intero |
| 19 | vento e onde (rumore ambiente di Wenz) | `SS` `ci02/sanctsound_ci02_03_sound_clips/data/SanctSound_CI02_03_wind_20191026T155434Z.wav` (ovest di San Miguel, 2019) | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 2.880.140 | 30,0 s | intero (la versione `_28dBgain_2xSpeed` non serve) |

## 4. Navi mercantili civili (cavitazione e battito reali)

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 20 | mercantile (nave grande) | `SS` `ci05/sanctsound_ci05_03_sound_clips/data/SanctSound_CI05_03_largeship_20190925T135956Z.wav` — «a large commercial ship» a est di Anacapa (descrizione della [Sound Library](https://sanctsound.ioos.us/sounds.html)) | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 960.224 | 10,0 s | intero |
| 21 | mercantile (traffico della corsia) | `SS` `oc02/sanctsound_oc02_01_sound_clips/data/SanctSound_OC02_01_ship_20190419T233040Z.wav` — rumore di nave a bassa frequenza vicino a Neah Bay, accesso allo stretto di Juan de Fuca | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 5.760.232 | 60,0 s | intero |
| 22 | mercantile (lento, in avvicinamento) | `NPSA` `7dd6d0fe-831f-4a11-9837-1b4f629d429b.mp3` — «Freighter … approaching the Bartlett Cove dock» | PD-§105 (credito «NPS / C. Gabriele») | A-NPS | MP3 44,1 kHz, 64 kb/s, mono | 297.794 | ~37 s | intero |

## 5. Pesca, diporto e industria

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 23 | `peschereccio_piccolo` | `NPSA` `cb678cb6-5627-4544-9fbb-c8e3884f0827.mp3` — «Small Diesel Engine» (barca con piccolo diesel che passa) | PD-§105 (credito «NPS / C. Gabriele») | A-NPS | MP3 44,1 kHz, 64 kb/s, mono | 250.356 | ~31 s | intero |
| 24 | `fuoribordo` | `NPSA` `39f7562b-1d7a-4fe4-a969-6be98e7d4886.mp3` — «Outboard Engine (60hp) at 20 Knots» | PD-§105 (credito «NPS / C. Gabriele») | A-NPS | MP3 44,1 kHz, 64 kb/s, mono | 78.157 | ~10 s | intero |
| 25 | `airgun` | `PMEL` `env-noise/sounds/airguns_00_03_05_marCE-Airguns-10x.wav` ([pagina](https://www.pmel.noaa.gov/acoustics/sounds_enviro.html); identico per dimensione a `airguns_00_03_05_marCE.wav` di `sounds.html`: ne basta uno) | PD-§105 | A-PMEL | WAV PCM 8 kHz, 16 bit, mono; **×10** | 828.364 | 51,8 s (≈ 8,6 min reali) | intero |
| 26 | `ecoscandaglio_profondo` / `profilatore` (timbro dell'impulso) | `SS` `ci05/sanctsound_ci05_03_sound_clips/data/SanctSound_CI05_03_echosounder_20191003T132915Z_36dBgain.wav` | PD-§105 (Naval Postgraduate School) | A-SS | WAV PCM 48 kHz, 16 bit, mono (+36 dB già applicati) | 960.332 | 10,0 s | intero |
| 27 | `esplosione` (e attacco di `esplosione_mgab`) | `SS` `fk03/sanctsound_fk03_01_sound_clips/data/SanctSound_FK03_01_explosion_20190131T220530Z.wav` (Florida Keys, sorgente ignota) | PD-§105 (NEFSC, WHOI/NOAA) | A-SS | WAV PCM 48 kHz, 16 bit, mono | 480.230 | 5,0 s | intero |

## 6. Transitori ASW (timbro di rotore e turboelica) — **richiedono account dell'utente su Freesound**

| # | Voce | File | Licenza | Attr. | Formato | Byte | Durata | Taglio |
|---|---|---|---|---|---|---|---|---|
| 28 | `aereo_p3c` (anche `aereo_il38`) | Freesound 464893, «Lockheed Electra turboprop flyover» di HootOwl — <https://freesound.org/people/HootOwl/sounds/464893/> (L-188: stessi Allison T56 del P-3) | CC0 ([pagina](https://freesound.org/people/HootOwl/sounds/464893/)) — **richiede account dell'utente** | A-FS | MP3 44,1 kHz, stereo | ~1,2 MB (dalla pagina) | 32,6 s | intero |
| 29 | `elicottero_sh3` | Freesound 333505, «Sea-king Helicopter.WAV» di Sandermotions — <https://freesound.org/people/Sandermotions/sounds/333505/> | CC0 — **richiede account dell'utente** | A-FS | WAV 48 kHz, 16 bit, stereo | ~15,2 MB | 83,1 s | 60 s attorno al passaggio più vicino (massimo dell'inviluppo) |
| 30 | `elicottero_sh60` | Freesound 156678, «blackhawk.wav» di John Sipos — <https://freesound.org/people/John%20Sipos/sounds/156678/> | CC0 — **richiede account dell'utente** | A-FS | WAV 44,1 kHz, 16 bit, stereo | ~7,4 MB | 43,9 s | intero |

Per Freesound le dimensioni sono quelle scritte sulla pagina: senza account non c'è un URL diretto dell'originale su cui
fare la HEAD. Le anteprime (MP3/OGG compressi) si scaricano anche senza account, ma non sono gli originali.

---

## 7. Riserve (varianti e alternative, non conteggiate nel totale)

| Voce | File | Licenza | Formato | Byte | Durata e taglio |
|---|---|---|---|---|---|
| `megattera` (variante atlantica, lossless) | `SS` `sb03/sanctsound_sb03_05_sound_clips/data/SanctSound_SB03_05_humpbackwhale_20190909T213108Z.wav` | PD-§105, A-SS | WAV 48 kHz/16/mono | 2.880.228 | 30 s, intero |
| `balenottera_azzurra` | `SS` `mb01/sanctsound_mb01_04_sound_clips/data/SanctSound_MB01_04_bluewhaleAcall_20200124T031012Z.wav` | PD-§105, A-SS | WAV 48 kHz/16/mono | 5.760.140 | 60 s, intero |
| `balenottera_minore` | `PMEL` `whales/sounds/whalewav/atlmin_512_64_0-50_10x.wav` | PD-§105, A-PMEL | WAV 8 kHz/16/mono, ×10 | 147.950 | 9,2 s, intero |
| `capodoglio` (16 bit) | `SS` `hi03/sanctsound_hi03_02_sound_clips/data/SanctSound_HI03_02_spermwhale_20191216T115715Z.wav` | PD-§105? | WAV 44,1 kHz/16/mono | 459.454 | 5,2 s, intero |
| `orca` (CC BY, lossless) | MBARI `https://pacific-sound-16khz.s3.amazonaws.com/2018/04/MARS-20180413T000000Z-16kHz.wav`, **Range `bytes=1505040332-1507680331`** (08:42:35Z + 55 s; esempio della [guida MBARI](https://docs.mbari.org/pacific-sound/notebooks/listen/PacificSoundListen/)) | CC BY 4.0, A-MBARI | WAV PCM 16 kHz, **24 bit**, mono; dati da byte 332 | 2.640.000 (file intero 4.147.200.332) | 55 s; all'estratto va rimesso davanti un'intestazione WAV |
| `delfini` (CC BY) | MBARI `.../2015/07/MARS-20150731T000000Z-16kHz.wav`, **Range `bytes=4009488332-4012128331`** (23:12:11Z + 55 s) | CC BY 4.0, A-MBARI | come sopra | 2.640.000 | 55 s |
| `megattera` (canto, CC BY) | MBARI `.../2015/12/MARS-20151207T000000Z-16kHz.wav`, **Range `bytes=1204560332-1207200331`** (06:58:15Z + 55 s) | CC BY 4.0, A-MBARI | come sopra | 2.640.000 | 55 s |
| `orca` (variante) | `SS` `oc03/sanctsound_oc03_02_sound_clips/data/SanctSound_OC03_02_killerwhale_20191128T091339Z.wav` | PD-§105, A-SS | WAV 48 kHz/16/mono | 641.934 | 6,7 s |
| `foca_barbata` (federale certo) e ghiaccio primaverile | NRS01 (Artico d'Alaska, 72,44 N, NOAA/AFSC): `https://storage.googleapis.com/noaa-passive-bioacoustic/nrs/audio/01/nrs_01_2014-2015/audio/NRS01_20150415_212724.flac` | PD-§105, A-NRS | FLAC 5 kHz/16/mono (banda < 2,5 kHz: il trillo si sente solo in parte) | 55.898.311 | 4 h; vedi «Tagli dai file lunghi» |
| `fessurazione_ghiaccio` (inverno) | NRS01 `.../NRS01_20150120_003625.flac` | PD-§105, A-NRS | FLAC 5 kHz/16/mono | 63.450.429 | 4 h; idem |
| mercantile (terza nave SanctSound) | `SS` `oc03/sanctsound_oc03_02_sound_clips/data/SanctSound_OC03_02_ship_20200228T080107Z.wav` | PD-§105, A-SS | WAV 48 kHz/16/mono | 5.760.236 | 60 s |
| nave passeggeri | `SS` `mb02/sanctsound_mb02_03_sound_clips/data/SanctSound_MB02_03_cruiseship_20190927T002033Z.wav`; oppure NPS «Cruise Ship» `NPSA` `1cc5583b-fe72-4cb2-b64c-563bd87ad91b.mp3` (≈10 nodi; MP3 128 kb/s stereo) | PD-§105 | WAV 48/16/mono; MP3 | 5.722.002; 1.928.462 | 59,6 s; ~120 s (taglio: 60 s attorno al massimo) |
| traghetto | NPS «State Ferry» `NPSA` `9ff56b54-6739-4440-a28e-26a251bc9f78.mp3` | PD-§105, A-NPS | MP3 44,1/64 kb/s/mono | 250.356 | ~31 s |
| mercantile MBARI (CC BY) | da cercare: vedi «Tagli dai file lunghi» | CC BY 4.0, A-MBARI | WAV 16 kHz/24/mono | ~2.880.000 per 60 s | — |
| pioggia, neve, vento (varianti) | NPS «Heavy Rain» `NPSA` `30679527-fd7a-4cad-8c7d-cb1bcc07604d.mp3`; «Snowfall» `2b68b3a9-7c0d-4452-bbe5-cc99b5d4fb61.mp3` (utile per Barents e Kunashir); «Moderate Wind» `2f574049-3325-446c-a8ac-2e5b31e9e32f.mp3` | PD-§105, A-NPS | MP3 44,1/64 kb/s/mono | 250.356 ciascuno | ~31 s |
| tempesta | `SS` `gr03/sanctsound_gr03_02_sound_clips/data/SanctSound_GR03_02_hurricane_20190904T221437Z.wav` | PD-§105, A-SS | WAV 48/16/mono | 480.226 | 5 s |
| `fuoribordo` (10 nodi) e `smallboat` | NPS `NPSA` `705da20f-b986-4e49-ae58-39f9f286e9ad.mp3`; `SS` `ci01/sanctsound_ci01_03_sound_clips/data/SanctSound_CI01_03_smallboat_20190825T191712Z.wav` | PD-§105 | MP3; WAV 48/16/mono | 81.083; 482.520 | ~10 s; 5 s |
| `esplosione_mgab` / `treno_mgab` (cariche piccole) | `SS` `mb01/sanctsound_mb01_05_sound_clips/data/SanctSound_MB01_05_sealbomb_20200619T064138Z_33dBgain.wav` | PD-§105, A-SS | WAV 48/16/mono | 768.272 | 8 s |
| ping di sonar attivo (riferimento per i `ping_*`) | `SS` `mb03/sanctsound_mb03_02_sound_clips/data/SanctSound_MB03_02_MFAsonar_20191006T175235Z_38dBgain.wav` (HARP) | PD-§105, A-SS | WAV **200 kHz**/16/mono | 4.076.018 | 10,2 s |
| `merluzzo` (variante) e `coro_pesci` (variante) | NOAA Fisheries `https://www.fisheries.noaa.gov/s3/2023-04/Gamo-grunt-NOAA-PAGroup-03-amplified-atlantic-cod-clip.mp3`; `SS` `fk02/sanctsound_fk02_01_sound_clips/data/SanctSound_FK02_01_fishchorus_20190416T004332Z.wav` | PD-§105 | MP3 8 kHz/24 kb/s; WAV 48/16/mono | 40.941; 480.228 | ~14 s; 5 s |
| `risacca` | Freesound 570901, «Sines Praia da Costa Norte_IndustrialPort_UnderwaterSounds» di Sonotomia — <https://freesound.org/people/Sonotomia/sounds/570901/> | CC0 — **richiede account dell'utente** | WAV 48 kHz/24/stereo | ~33,2 MB | 2:01; descrizione da leggere: potrebbe essere più porto che risacca |
| ghiaccio (acqua dolce) | Freesound 564351, «20180301_Frozen hydrophones.wav» di cabled_mess — <https://freesound.org/people/cabled_mess/sounds/564351/> | CC0 — **richiede account dell'utente** | WAV 96 kHz/24/stereo | ~23,6 MB | 43 s; lago, non mare |

## 8. Lacune (nessun campione con licenza ammessa: resta la sintesi del modello)

- **`aringa`** (treni a larga banda notturni): nessuna registrazione federale, CC0 o CC BY trovata.
- **`battitura_pali`**: le registrazioni pubbliche trovate sono nei rapporti dei DOT statali (opere statali, non
  federali) o su DOSITS (escluso). L'impulso di 30-50 ms è semplice da sintetizzare.
- **`piattaforma`**: nessuna registrazione libera trovata.
- **`peschereccio_strascico`**: nessuna registrazione dichiarata come strascico; si può derivare dal #23 aggiungendo
  la componente dei verricelli col modello.
- **`risacca`**: solo il CC0 incerto di Sonotomia (riserve). In alternativa il #19 e la pioggia filtrati.
- **Tonfi (`tonfo_*`) e ping (`ping_*`)**: restano sintesi del modello, come previsto da §2.3; il MFA sonar e
  l'ecoscandaglio servono solo da riferimento d'attacco.
- `vulcano_sottomarino` (rombo continuo a 125 Hz): solo il #12, che è un'eruzione impulsiva.

## 9. Tagli dai file lunghi

- **Server:** PMEL (`Accept-Ranges: bytes`, 206 verificato), bucket NOAA su Google Cloud (206 verificato), S3 di MBARI
  (`Accept-Ranges: bytes`, lettura parziale riuscita), NPS e NOAA Fisheries (`Accept-Ranges: bytes`). I tagli con
  Range funzionano ovunque; fra i file principali (1-30) **nessuno richiede Range**: sono tutti brevi e si scaricano
  interi.
- **MBARI 16 kHz** (file giornaliero da 4,15 GB, inizio alle 00:00:00Z): intestazione di 332 byte, poi 48.000 byte al
  secondo (16.000 campioni × 3 byte). Byte d'inizio = `332 + secondi_dalla_mezzanotte × 48000`; 60 s = 2.880.000 byte.
  I valori della tabella 7 sono calcolati così. All'estratto si antepone un'intestazione WAV nuova (PCM 24 bit, 16 kHz,
  mono).
- **Mercantile MBARI da trovare:** le corsie passano a circa 20 km dall'idrofono (guida MBARI sul rumore navale). Per
  trovare un passaggio senza scaricare il giorno intero: si legge con Range **un'ora** del file a 2 kHz
  (`https://pacific-sound-2khz.s3.amazonaws.com/AAAA/MM/MARS-AAAAMMGGT000000Z-2kHz.wav`, 6.000 byte/s, 21,6 MB l'ora),
  si calcola il livello nel terzo d'ottava a 63 Hz minuto per minuto (il metodo della guida) e si prende il massimo;
  poi si estraggono 60 s da quel punto dal file a 16 kHz (2,88 MB). Conviene un giorno del 2019 (prima del calo del
  traffico del 2020 per il COVID).
- **NRS (FLAC da 4 h, 55-75 MB):** i file hanno una **SEEKTABLE** di 1.440 punti (uno ogni 10 s) nei metadati, letta
  nell'intestazione. Quindi si può tagliare senza scaricare tutto: si leggono i primi ~26 KB (STREAMINFO e SEEKTABLE),
  si prende l'offset del punto voluto e si scarica con Range fino al punto di 60 s dopo (circa 230-300 KB), poi si
  ricompone un FLAC con STREAMINFO e i frame. Dove stia l'evento non lo so: per la foca barbata ad aprile i trilli sono
  quasi continui nel paesaggio primaverile artico [D, da letteratura citata nel dossier 09], quindi basta provare 4-6
  estratti di 60 s a ore diverse (≈ 1,5 MB in tutto) e scegliere sullo spettrogramma; per le fessurazioni idem a
  gennaio. Se è più semplice, si scarica un file intero (56 o 63 MB).
- **PMEL `A53JD33Calving_3x.wav`** e i file NPS/Freesound più lunghi di 60 s: si scaricano interi (sono piccoli) e si
  taglia dopo, sul massimo dell'inviluppo.

---

## Totale da scaricare

- **File principali senza account (1-27):** 27 file, **46.407.717 byte ≈ 46,4 MB**.
- **Freesound (28-30), richiedono account dell'utente:** 3 file, ≈ 23,8 MB (dimensioni dalle pagine).
- **Totale proposto: 30 file, ≈ 70,2 MB.** Ben sotto i 300 MB.
- Riserve, se si volessero tutte: ≈ 236 MB in più, di cui 119 MB sono i due FLAC NRS interi (con i tagli Range
  scendono a pochi MB) e 57 MB i due WAV Freesound delle riserve.

Per sorgente (principali): SanctSound 12 file (≈ 36,1 MB), PMEL 6 (≈ 7,0 MB), NPS 6 (≈ 2,5 MB), RUSALCA 2 (≈ 0,7 MB),
NOAA Fisheries 1 (0,09 MB), Freesound 3 (≈ 23,8 MB).

## Note sulle licenze

1. **Opere federali (17 U.S.C. §105).** PMEL Acoustics: «public information and may be distributed freely… please
   cite NOAA PMEL Acoustics Program» ([multimedia](https://www.pmel.noaa.gov/acoustics/multimedia.html)), salvo il
   materiale segnato come protetto (nessuno dei file scelti lo è). NPS: «generally considered in the public domain»
   per il materiale creato dall'NPS ([disclaimer](https://www.nps.gov/aboutus/disclaimer.htm)); la pagina di Glacier
   Bay dice che le registrazioni sono fatte dall'NPS. NOAA Fisheries: ciò che è creato dal governo «is not subject to
   copyright in the United States» ([politiche](https://www.fisheries.noaa.gov/website-policies-and-disclaimers));
   il file dell'eglefino è del Passive Acoustics Group del NEFSC (nel nome: `NOAA-PAGroup`). Il clip del tamburo nero
   sulla stessa pagina è di «Heyman» (non federale) e l'ho escluso.
2. **SanctSound.** La scheda NCEI non ha una licenza, solo l'esclusione di garanzie e la citazione (dossier §1). I
   pacchetti scelti sono di scienziati della **Naval Postgraduate School** (Marina USA), del **NEFSC** e di
   **WHOI/NOAA**: §105 [D, alta]. Fa eccezione il capodoglio (#3, HI03_02): gli autori del pacchetto sono **Lynker LLC**
   (appaltatore) e **University of Hawaii**, quindi il §105 non è certo; NOAA lo pubblica senza restrizioni, ma
   conviene una mail a NCEI (`pad.info@noaa.gov`) o tenere pronta l'alternativa (nessun capodoglio federale certo
   trovato; il modello sa già fare i clic).
3. **RUSALCA (#6, #16).** La pagina è su noaa.gov ma non dà né autore né credito; le registrazioni dei mooring
   RUSALCA dello stretto di Bering sono di solito dei ricercatori dell'Università di Washington (APL-UW) [D, non
   verificato]. Se l'autore non è federale, il §105 non si applica. Alternativa sicura: gli estratti NRS01
   (NOAA/AFSC, riserve), con banda ridotta a 2,5 kHz.
4. **NPS «Bergy Seltzer» (#17)** è accreditato «H. Lentfer / NPS»: probabile collaboratore del progetto «Voices of
   Glacier Bay»; la pagina lo presenta come materiale NPS. Rischio basso, ma da annotare.
5. **MBARI (riserve): CC BY 4.0**, uso commerciale e modifiche ammessi; obbligo di attribuzione e di indicare le
   modifiche ([registro AWS](https://registry.opendata.aws/pacific-sound/), [pagina MBARI](https://www.mbari.org/data/passive-acoustic-data/)).
   I notebook della documentazione sono GPL/AGPL, ma riguardano il codice, non i dati.
6. **Freesound (#28-30):** CC0 verificato sulle pagine. Serve **l'account dell'utente** per gli originali. CC0 non
   garantisce che l'autore avesse i diritti: prima di scaricare va letta la descrizione di ciascuna pagina (non l'ho
   verificata), per escludere materiale preso da film o da altre librerie.
7. **Niente loghi né patrocinio:** nei crediti si citano NOAA, NPS, MBARI come fonte, senza logo e senza far intendere
   un'approvazione.
8. **Qualità:** gli MP3 NPS (64 kb/s), NOAA Fisheries (8 kHz, 24 kb/s) e RUSALCA sono compressi. Per una tessitura che
   viene sbiancata e ricolorata va bene, ma gli artefatti MP3 sopra ~11-16 kHz vanno tagliati in analisi. I file PMEL
   accelerati hanno banda reale di poche centinaia di Hz: giusto per balene grandi, sismica e ghiaccio. Alcuni clip
   SanctSound hanno un guadagno digitale già applicato (`_28dBgain`, `_36dBgain`): controllare il clipping.
9. **Esclusi come da vincoli:** Watkins, DOSITS (aveva l'eglefino e la battitura di pali), ShipsEar, AWM, Orcasound,
   ogni CC BY-NC e ShareAlike. Esclusi anche i clip SanctSound di partner non federali dove esisteva un'alternativa.

## Verifica delle licenze «PD-§105?» (25/10/2026)

I tre file sono esclusi dal manifesto (campo `escluso` in `tools/pipeline/campioni.py`): il pubblico dominio non si
dimostra, e per quelle voci resta la sintesi del modello.
- **#3 capodoglio:** i metadati del pacchetto
  (<https://storage.googleapis.com/noaa-passive-bioacoustic/sanctsound/products/sound_clips/hi03/sanctsound_hi03_02_sound_clips/metadata/sanctsound_hi03_02_sound_clips-metadata.json>)
  danno come autori Eden Zang (Lynker, LLC) e Anke Kügler (University of Hawaii at Manoa), con NOAA/ONMS e U.S. Navy
  come finanziatori; la scheda NCEI
  (<https://data.noaa.gov/metaview/page?xml=NOAA/NESDIS/NGDC/MGG/passive_acoustic//iso/xml/NOAA-Navy-SanctSound_Raw_Data.xml&view=getDataView>)
  non dichiara licenza. Nel bucket non c'è un altro clip di capodoglio.
- **#6 foca barbata e #16 fessurazione del ghiaccio:** la pagina <https://www.pmel.noaa.gov/rusalca/sounds-arctic-ocean/>
  non dice né autore né licenza; gli ormeggi dello Stretto di Bering sono del gruppo di Kate Stafford (APL-UW, non
  federale). Un'alternativa possibile sono i dati dello stesso ormeggio all'Arctic Data Center (per esempio
  doi:10.18739/A2WH3J, autrice K. M. Stafford): la licenza va letta sulla scheda prima di proporne lo scarico.

## Fonti

- Dossier del progetto: `docs/ricerca/18_audio_reale.md` §1-2; `docs/ricerca/18_note/archivi.md`.
- NOAA PMEL Acoustics: <https://www.pmel.noaa.gov/acoustics/sounds.html>, <https://www.pmel.noaa.gov/acoustics/sounds_seismic.html>,
  <https://www.pmel.noaa.gov/acoustics/sounds_whales.html>, <https://www.pmel.noaa.gov/acoustics/sounds_enviro.html>,
  <https://www.pmel.noaa.gov/acoustics/sounds_cryogenic.html>, <https://www.pmel.noaa.gov/acoustics/multimedia.html> (crediti).
- RUSALCA, «Sounds of the Arctic Ocean»: <https://www.pmel.noaa.gov/rusalca/sounds-arctic-ocean/>.
- NOAA/NPS NRS: <https://www.pmel.noaa.gov/acoustics/noaanps-ocean-noise-reference-station-network/> (stazioni e
  partner), listato `https://storage.googleapis.com/storage/v1/b/noaa-passive-bioacoustic/o?prefix=nrs/audio/01/`,
  README <https://storage.googleapis.com/noaa-passive-bioacoustic/nrs/README.pdf>, DOI <https://doi.org/10.7289/V5M32T0D>.
- SanctSound: listato `https://storage.googleapis.com/storage/v1/b/noaa-passive-bioacoustic/o?prefix=sanctsound/products/sound_clips/`
  (288 WAV, 993 MB in tutto, 67 JSON di metadati letti), Sound Library <https://sanctsound.ioos.us/sounds.html>,
  DOI <https://doi.org/10.25921/saca-sp25>, NCEI <https://www.ncei.noaa.gov/products/passive-acoustic-data>.
- NPS Glacier Bay: <https://home.nps.gov/glba/learn/nature/soundclips.htm>,
  <https://home.nps.gov/glba/learn/nature/glaciers-soundscape-gallery.htm>, <https://www.nps.gov/aboutus/disclaimer.htm>.
- NOAA Fisheries NEFSC: <https://www.fisheries.noaa.gov/new-england-mid-atlantic/science-data/fish-and-invertebrate-acoustics>,
  <https://www.fisheries.noaa.gov/website-policies-and-disclaimers>.
- MBARI Pacific Sound: <https://registry.opendata.aws/pacific-sound/>, <https://www.mbari.org/data/passive-acoustic-data/>,
  <https://docs.mbari.org/pacific-sound/notebooks/listen/PacificSoundListen/> (tempi d'esempio),
  <https://docs.mbari.org/pacific-sound/notebooks/shippingnoise/PacificSoundShippingNoiseAnalysis/> (metodo a 63 Hz),
  listati `https://pacific-sound-16khz.s3.amazonaws.com/?list-type=2&delimiter=/` e `https://pacific-sound-2khz.s3.amazonaws.com/?list-type=2&delimiter=/`.
- Freesound: <https://freesound.org/people/HootOwl/sounds/464893/>, <https://freesound.org/people/Sandermotions/sounds/333505/>,
  <https://freesound.org/people/John%20Sipos/sounds/156678/>, <https://freesound.org/people/Sonotomia/sounds/570901/>,
  <https://freesound.org/people/cabled_mess/sounds/564351/>, FAQ licenze <https://freesound.org/help/faq/>.
- 17 U.S.C. §105: <https://www.law.cornell.edu/uscode/text/17/105>.
