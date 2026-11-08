# Registrazioni subacquee reali utilizzabili come campioni nel remake gratuito di «688 Attack Sub»

Ricerca su fonti aperte, 25 ottobre 2026. Remake non commerciale (open source o freeware).

## Legenda e metodo

- **[V]** = verificato leggendo la pagina ufficiale (link riportato) o l'intestazione/listato reale del file o del bucket.
- **[S]** = affermazione presa da una fonte secondaria (articolo, motore di ricerca, scheda di terzi). Il link è indicato, ma la pagina ufficiale non l'ha confermata.
- **[D]** = deduzione mia, da non considerare un fatto accertato.
- Le pagine che non si sono aperte, o si sono aperte solo in parte, sono elencate nell'ultima sezione.
- Non sono un avvocato. Questo è un dossier tecnico, non un parere legale. Nei casi dubbi la via più sicura è una mail al detentore dei diritti, con risposta conservata.

### Come leggere la «licenza» nei casi americani

Per le opere di dipendenti federali USA il 17 U.S.C. § 105 esclude la protezione del diritto d'autore: <https://www.law.cornell.edu/uscode/text/17/105> [V]. Vale solo per ciò che è prodotto dal governo federale (NOAA, NPS, Navy). Non vale per i dati di partner non federali (università, aziende come JASCO, istituti stranieri) anche se ospitati nell'archivio NOAA [D].

---

## 1. NOAA: PMEL Acoustics, Ocean Noise Reference Station Network (NRS), NCEI passive acoustic archive, SanctSound

### 1a. PMEL Acoustics: suoni di esempio (ghiaccio, terremoti, balene accelerate, navi)

- **Contenuto [V].** Pagina «Sounds (examples)»: <https://www.pmel.noaa.gov/acoustics/sounds.html>. Categorie: sismico, balene, antropico, criogenico/ghiaccio. Le registrazioni sono a bassa frequenza e accelerate (3x, 10x, 16x) per renderle udibili. Esempi: tremore armonico a 16x, blue whale NE Pacifico a 10x, airgun a 10x, crollo di iceberg a 3x. Pagina criogenica: <https://pmel.noaa.gov/acoustics/specs_cryogenic.html>.
- **Multimedia [V].** Pagina <https://www.pmel.noaa.gov/acoustics/multimedia.html>. Include «Humpback whale with ship in background» (Stellwagen e American Samoa), «Ship traffic Challenger Deep» a 10x, damselfish, blue whale AB.
- **Formato [V].** Ho letto l'intestazione WAV con una richiesta HTTP a 44 byte:
  - `HB-ship-SBNMS.wav`: PCM 16 bit, mono, 44.100 Hz, 834 KB.
  - `bloop.wav`, `tremorx16.wav`, `nepblue24s10x.wav`, `A53JD33Calving_3x.wav`: PCM 16 bit, mono, 8.000 Hz (cioè già accelerati).
- **Durata.** Clip brevi, da pochi secondi a circa 2 minuti [D, dalle dimensioni dei file: 0,2 MB a 4,5 MB a 8–44 kHz].
- **Licenza / termini [V].** Nessuna licenza formale. Dice solo, alla pagina multimedia:
  > «Unless otherwise noted (copyrighted material for example), information presented on the PMEL Acoustics Program webpage is considered public information and may be distributed freely. If you elect to use materials from this page, please cite NOAA PMEL Acoustics Program as the source, and include the appropriate URL of the page(s) from which the materials were taken.»
  
  Fonte: <https://www.pmel.noaa.gov/acoustics/multimedia.html>. In più c'è il § 105 [V] (sopra).
- **Uso nel gioco gratuito.** Sì [D, sulla base del testo citato]. Distribuzione libera consentita. Attribuzione: «NOAA PMEL Acoustics Program» più l'URL della pagina d'origine. Nessun obbligo di non-commercialità. Restano esclusi i materiali «copyrighted» segnalati (alcuni video, collaborazioni con artisti elencate nella stessa pagina).
- **Caveat.** Non si può usare il logo NOAA né far intendere un patrocinio. Questo deriva da pagine NOAA sulla photo library, trovate solo via ricerca [S]: <https://www.noaa.gov/noaa-collections/photo-library>. La pagina non si apre con curl (405).
- **Copie di terzi.** Su Internet Archive esistono ~43 elementi «noaa-pmel-acoustics-…», con marchio PDM e file wav/flac/mp3 (ad es. <https://archive.org/details/noaa-pmel-acoustics-hb-ship-sbnms>) [V]. Sono caricate da terzi («Hessel Digital Library», settembre 2026). Come origine citare sempre PMEL.

### 1b. NOAA/NPS Ocean Noise Reference Station Network (NRS)

- **Contenuto [V].** <https://www.pmel.noaa.gov/acoustics/noaanps-ocean-noise-reference-station-network/>. 13 stazioni (NRS01–NRS13) in acque USA, dal 2014. Banda dichiarata 10–2.000 Hz. Il testo dice «Over 63 cumulative years of audio data are publicly available» presso NCEI.
- **Formato [V].** Il bucket Google Cloud `noaa-passive-bioacoustic/nrs/audio/…` contiene file FLAC da circa 90 MB ciascuno (<https://storage.googleapis.com/storage/v1/b/noaa-passive-bioacoustic/o?prefix=nrs/audio/01/nrs_01_2014-2015/audio/>). L'intestazione FLAC di `NRS01_20141014_234015.flac` indica: 5.000 Hz, 16 bit, mono, 72.000.000 campioni = 4,0 ore. Il README del progetto: <https://storage.googleapis.com/noaa-passive-bioacoustic/nrs/README.pdf> [V].
- **Qualità per il gioco [D].** Banda utile fino a 2,5 kHz: sono suoni a bassissima frequenza (navi lontane, balene baleniere, terremoti), ma sono lunghissimi. Per un uso come campione servono estrazioni e velocizzazioni.
- **Licenza [V].** La scheda NCEI (<https://doi.org/10.7289/V5M32T0D> → data.noaa.gov metaview) riporta solo esclusione di garanzie («Use liability…»), nessuna licenza né vincolo commerciale. Citazione richiesta: «NOAA OAR Pacific Marine Environmental Laboratory, National Marine Fisheries Service, NOS Office of National Marine Sanctuaries, and DOI NPS … 2014. NOAA Ocean Noise Reference Station Network Raw Passive Acoustic Data. NCEI. https://doi.org/10.7289/V5M32T0D» (README NRS) [V]. I dati sono federali → § 105 [D].
- **Uso nel gioco gratuito.** Sì [D]. Attribuzione: citazione del DOI.

### 1c. NCEI Passive Acoustic Data Archive e SanctSound

- **Pagina ufficiale [V].** <https://www.ncei.noaa.gov/products/passive-acoustic-data>. Dice «free, globally-accessible, raw audio files and data products». Accesso: bucket GCP `noaa-passive-bioacoustic` (listato pubblico via API), Passive Acoustic Data Map, richiesta a pad.info@noaa.gov. Formati: wav/aif/mp3/flac.
- **Contenuto [V].** Il listato del bucket mostra 30 cartelle di progetto: `nrs`, `sanctsound`, `afsc`, `nefsc`, `sefsc`, `pifsc`, `swfsc`, `jasco`, `cornell`, `mbari`, `dclde`, `adeon`, `soundcoop` ecc. (<https://storage.googleapis.com/storage/v1/b/noaa-passive-bioacoustic/o?delimiter=/>). Volume complessivo non indicato dalla pagina.
- **Clip pronte (SanctSound sound clips) [V].** <https://storage.googleapis.com/storage/v1/b/noaa-passive-bioacoustic/o?prefix=sanctsound/products/sound_clips/mb01/sanctsound_mb01_04_sound_clips/>. Esempio: `SanctSound_MB01_04_bluewhaleAcall_20200124T031012Z.wav`, 5,8 MB, PCM 16 bit mono 48.000 Hz (intestazione letta), più la versione `_8xSpeed.wav`. La pagina NCEI cita i clip: <https://www.ncei.noaa.gov/products/passive-acoustic-data>. Il progetto: 30 siti, 7 santuari nazionali e 1 monumento, 2018–2021 (scheda <https://doi.org/10.25921/saca-sp25>) [V].
- **Licenza [V].** Nessuna licenza esplicita nella pagina NCEI. Le schede dei dataset (SanctSound, collezione) hanno solo «Use liability» e «Distribution liability» (esclusione di garanzie) più «Cite as». «In most cases, electronic downloads of the data are free» (campo Fees). La citazione di tutto l'archivio: «NOAA NCEI. 2017. Passive Acoustic Data Collection. https://doi.org/10.25921/PF0H-SQ72». Citazione SanctSound: «NOAA Office of National Marine Sanctuaries and U.S Navy. 2020. SanctSound Raw Passive Acoustic Data. NCEI. https://doi.org/10.25921/saca-sp25».
- **Attenzione, incoerenze.** Il catalogo data.gov di un dataset NCEI affine (USWTR, registrazioni HARP presso poligoni Navy, 2000–2010) dichiara `license: CC0 1.0` ma anche `accessLevel: non-public` e `rights: otherRestrictions`: <https://catalog.data.gov/dataset/audio-files-recorded-at-us-navy-undersea-warfare-training-ranges-uswtr-sites> [V]. Un'altra ricerca riporta «No license information was provided» [S]. Non basarsi sul solo campo del catalogo.
- **Uso nel gioco gratuito.** Sì per i dataset prodotti da enti federali (NOAA, NPS, Navy) [D, § 105]. Per i dati di partner non federali (`jasco/`, `cornell/`, `dclde/`, università) la licenza non è verificata: leggere il README e la scheda NCEI del singolo dataset [D]. Attribuzione: usare la citazione con DOI. Nessun obbligo di registrazione per il bucket (accesso anonimo) [V].
- **Altre pagine.** Portale SanctSound: <https://sanctsound.ioos.us/> [V]: offre «Sound Library» e prodotti di livello sonoro, senza dichiarazione di licenza.

### 1d. Disclaimer NOAA generale

La pagina <https://www.noaa.gov/disclaimer> riguarda solo i link esterni e non dice nulla sul diritto d'autore [V]. <https://www.noaa.gov/protecting-your-privacy> dichiara pubblico dominio solo per i dati NOAA presentati con Google Maps/Earth [V]. Nessuna delle due serve come licenza per l'audio.

---

## 2. Watkins Marine Mammal Sound Database (WHOI / New Bedford Whaling Museum)

- **Stato del sito [V].** Il sito originale (`whoicf2.whoi.edu` e `cis.whoi.edu/science/B/whalesounds/`) risponde con una pagina «Site Under Maintenance» (25/10/2026). Ho letto le pagine dalle copie di Wayback Machine (luglio–dicembre 2025):
  - Home: <http://web.archive.org/web/20251126124101/https://whoicf2.whoi.edu/science/B/whalesounds/index.cfm>
  - About (con le condizioni): <http://web.archive.org/web/20250713040020/https://whoicf2.whoi.edu/science/B/whalesounds/about.cfm>
  - Esempio di elenco clip: <http://web.archive.org/web/20250713053801/https://whoicf2.whoi.edu/science/B/whalesounds/bestOf.cfm?code=BD15A>
  
  Non ho potuto verificare che le condizioni attuali siano rimaste identiche. I file audio non sono nelle copie Wayback.
- **Contenuto [V].** «approximately 2000 unique recordings of more than 60 species» (la pagina About); sezione «Best of»: 1.694 clip da 32 specie; «All cuts» circa 15.000 clip; «Master tapes» quasi 1.600 nastri interi (zip con metadati). Registrazioni dagli anni '40 ai 2000. Il comunicato WHOI parla di circa 1.800 master tapes e più di 10.000 clip: <https://www.whoi.edu/press-room/news-release/historic-marine-mammal-sound-archive-now-available-online/> [V] (numeri non coincidenti tra le due fonti).
- **Formato [V].** Link diretti a `.wav` (pagina bestOf). Frequenza di campionamento e bit **non verificati**: variano per clip e la pagina About ammette «some errors may remain in sample rates, cut sizes, or other metadata».
- **Licenza / termini [V]** (pagina About, sezione «Crediting information»):
  > «Sound files on this website are free to download for personal or academic (not commercial) use. Reposting this work on third party websites is not permitted. Sound files and associated metadata should be credited as follows: "Watkins Marine Mammal Sound Database, Woods Hole Oceanographic Institution and the New Bedford Whaling Museum."»
  
  C'è un modulo «New Bedford Whaling Museum Permission-to-Publish form» e un contatto del museo (Robert Rocha, rrocha@whalingmuseum.org). Il comunicato WHOI del 2014–2015 riporta la versione precedente: «Commercial use of the recordings is prohibited» e credito «Watkins Marine Mammal Sound Database, Woods Hole Oceanographic Institution» [V].
  
  La scheda d'archivio WHOI della collezione fisica dice: «Permission to publish material from the collection must be authorized by the Institution Archivist.» <https://archives.mblwhoilibrary.org/repositories/2/resources/168> [V].
- **Uso nel gioco gratuito.** **Condizionato, probabilmente no senza permesso scritto** [D]. Un gioco freeware non è «commerciale», ma includere i WAV in un pacchetto scaricabile equivale a ridistribuire («reposting… on third party websites is not permitted»). Serve il modulo «Permission to Publish» del museo. Attribuzione obbligatoria con la dicitura esatta. In alternativa: far scaricare i file all'utente dal sito originale invece di includerli nel pacchetto [D].
- **Rilevanza per «688».** Solo fauna marina (sperma, orche, foche, delfini, ecc.): utile per il biologico/rumore di fondo, non per navi o sottomarini.

---

## 3. DOSITS (Discovery of Sound in the Sea, University of Rhode Island)

- **Contenuto [V].** Audio Gallery: <https://dosits.org/galleries/audio-gallery/> (letto via WebFetch; curl riceve 403). Categorie: balene misticeti (~11), odontoceti (~22), pinnipedi (~14), sirenii, invertebrati (7), pesci (~31), altri suoni naturali (9), suoni antropici (~20). Totale stimato 115–120 registrazioni (stima del lettore automatico, non un conteggio ufficiale). Pagina dei suoni antropici <https://dosits.org/galleries/audio-gallery/anthropogenic-sounds/> [V]: airgun, ATOC, dragaggio, esplosivi, rompighiaccio, sonar attivo a media frequenza, ecoscandaglio multifascio, tomografia acustica, motore fuoribordo, battipalo, nave, sonar, SURTASS LFA, siluro, ecc. **Non c'è una voce «sottomarino».**
- **Formato / qualità.** Non dichiarato nelle pagine che ho letto (pagine del siluro e della nave: solo descrizione testuale, nessun link di download visibile) [V].
- **Licenza [V].** Pagina <https://dosits.org/dosits-disclaimer/>:
  - «Copyright © 2002-2023, University of Rhode Island Graduate School of Oceanography. All Rights Reserved.» (il piè di pagina dice 2002–2025, URI e Inner Space Center).
  - Materiale di terzi: i permessi sono stati dati al sito; «Please contact the original copyright holder for permission to use.»
  - «Some media on DOSITS was released under a Creative Commons license … mostly a non-commercial, attribution CC license, international v3 or v4 … the CC license apply only to the sounds themselves, not the spectrograms in the videos created by DOSITS.» **La pagina non elenca quali suoni.**
  - Contatto per permessi: askDOSITS@gmail.com.
- **Uso nel gioco gratuito.** **No per default; sì solo caso per caso** [D]. Va controllata la licenza del singolo suono (CC BY-NC è compatibile con un freeware, con attribuzione). Per gli altri, permesso scritto da askDOSITS@gmail.com. Le pagine dei suoni che ho letto (Torpedo, Ship) riportano solo «All rights reserved» e nessuna licenza CC [V].

---

## 4. ShipsEar (Università di Vigo)

- **Pagina ufficiale [V].** <https://underwaternoise.atlanttic.uvigo.es/> (la vecchia URL `atlanttic.uvigo.es/underwaternoise/` rimanda qui); elenco: <https://underwaternoise.atlanttic.uvigo.es/indexDB.php>.
- **Contenuto.** Registrazioni di navi e imbarcazioni in acque basse (coste atlantiche della Spagna, porti di Vigo, 2012–2014). La pagina ufficiale non riporta un totale. L'abstract dell'articolo dice «90 records representing sounds from 11 vessel types» [V]: repository Investigo <https://hdl.handle.net/11093/10832>, DOI 10.1016/J.APACOUST.2016.06.008. Durate da 15 s a 10 min, circa 3 ore in totale [S] (articolo arXiv: <https://arxiv.org/pdf/2411.02787>). Nell'elenco ufficiale (prima pagina) ho contato 20 voci con 48 minuti totali: non ho letto le altre pagine.
- **Formato [V].** Ho letto l'intestazione del primo WAV nel file `samples.zip` (richiesta parziale di 70 KB su 84 MB): PCM **24 bit, mono, 52.734 Hz**. Il valore 52.734 Hz è confermato anche da fonti secondarie [S].
- **Licenza [V].** Nessuna licenza esplicita. Il piè di pagina dice «©Uvigo-Sonitum 2016». Il file `samples.zip` (84 MB, campioni di vari tipi di nave) è scaricabile liberamente. Per l'accesso completo: «To get full access to the database please send a request to the email contact» (dsantos@gts.uvigo.es).
- **Uso nel gioco gratuito.** **Non chiaro, serve permesso scritto** [D]. Nessun permesso di ridistribuzione dichiarato; il copyright resta a Uvigo-Sonitum. Il dataset è pensato per la ricerca. Dire all'autore che il gioco è gratuito e chiedere una dicitura esplicita. Attribuzione: citare Santos-Domínguez, Torres-Guijarro, Cardenal-López, Pena-Gimenez, Applied Acoustics 113 (2016) 64–69.
- **Rilevanza per «688».** Alta come rumore di navi di superficie (motoscafi, traghetti, rimorchiatori, navi da carico, dragaggio) vicine all'idrofono.

---

## 5. DeepShip (Northwestern Polytechnical University)

- **Pagina ufficiale [V].** Repository GitHub dell'autore: <https://github.com/irfankamboh/DeepShip>. README: «47 hours and 04 minutes of real world underwater recordings of 265 different ships belong to four classes» (petroliere, rimorchiatori, traghetti passeggeri, navi da carico); registrate dal 2 maggio 2016 al 4 ottobre 2018 «in strait of Georgia delta node». Su GitHub c'è solo una parte (67 file WAV, circa 730 MB nelle cartelle Cargo 13, Tanker 29, Passengership 21, Tug 4); il resto va chiesto per mail a mirfan@mail.nwpu.edu.cn [V].
- **Formato [V].** Intestazione di `Cargo/15.wav`: WAV float a 32 bit, mono, 32.000 Hz (conforme a 32 kHz riportato in letteratura [S]: <https://arxiv.org/pdf/2411.02758>).
- **Origine [S, D].** «Strait of Georgia delta node» è un sito di Ocean Networks Canada. Fonti secondarie (articoli che citano il dataset) lo descrivono come derivato da dati ONC registrati con idrofoni icListen: <https://arxiv.org/pdf/2505.14285>, <https://arxiv.org/pdf/2401.02099>. Non sono riuscito a leggere l'articolo originale (Expert Systems with Applications 183, 2021, DOI 10.1016/j.eswa.2021.115270, pagina Elsevier non accessibile). L'origine ONC è quindi una deduzione ragionevole, non verificata sulla fonte primaria.
- **Licenza [V].** Nessuna: l'API GitHub riporta `license: null`, il README non contiene condizioni d'uso, non c'è file LICENSE.
- **Uso nel gioco gratuito.** **Non chiaro; meglio andare alla fonte** [D]. Un repository senza licenza lascia tutti i diritti all'autore. Se i dati sono dati ONC di proprietà ONC, la fonte primaria è CC BY 4.0 (sezione 7): conviene scaricare l'audio equivalente direttamente da ONC con attribuzione corretta, oppure chiedere conferma scritta a mirfan@mail.nwpu.edu.cn.
- **Rilevanza per «688».** Navi mercantili reali con rumore di fondo e cetacei, una banda utile fino a 16 kHz.

---

## 6. MBARI: idrofono MARS (Pacific Sound), Soundscape Listening Room

- **Archivio dati [V].** Registro AWS: <https://registry.opendata.aws/pacific-sound/>. Registrazioni quasi continue dal luglio 2015 al largo della California centrale (Monterey Bay), da idrofono sul cavo MARS (900 m di profondità [S/D, non letto]). Bucket pubblici S3 senza account: file originali a **256 kHz** (annuali, `pacific-sound-256khz-2015` … `-2025`), copie decimate a **16 kHz** e **2 kHz**.
- **Formato [V].** Dai listati S3 e dalle intestazioni: 16 kHz = WAV PCM **24 bit** mono, un file al giorno di 4,15 GB (`MARS-20200101T000000Z-16kHz.wav`); 2 kHz = un file al giorno da 518 MB (24 bit per deduzione dalla dimensione); 256 kHz = file di 10 minuti da 460,8 MB (`MARS_20200101_000851.wav`). Documentazione e notebook: <https://docs.mbari.org/pacific-sound/>.
- **Licenza [V].** Registro AWS: «License: CC-BY 4.0». Pagina MBARI <https://www.mbari.org/data/passive-acoustic-data/>: «License is CC by 4.0». Citazione indicata: «Pacific Ocean Sound Recordings was accessed on DATE from https://registry.opendata.aws/pacific-sound.» (la pagina di documentazione, con licenza AGPL, riguarda il codice dei notebook e non i dati).
- **Soundscape Listening Room [V].** <https://www.mbari.org/project/soundscape-listening-room/>: flusso in diretta (ora offline per manutenzione) e libreria di clip (balene, delfini, otarie, barche, pioggia, vento, terremoti); la pagina non indica licenza né formato dei singoli clip. Presumibilmente rientrano nello stesso archivio CC BY, ma non è scritto [D].
- **Uso nel gioco gratuito.** **Sì, con attribuzione** (CC BY 4.0 permette anche uso commerciale e modifiche). Attribuzione: nome dell'autore/ente (MBARI), titolo del dataset, link alla licenza <https://creativecommons.org/licenses/by/4.0/>, indicazione delle modifiche. Nessuna registrazione.
- **Note.** Il materiale è molto grande: servono estrazioni. Contiene navi mercantili e balene (blu, fin, megattere) con nitidezza elevata.

---

## 7. Ocean Networks Canada (ONC)

- **Politica dati [V].** <https://www.oceannetworks.ca/data/data-policy/>:
  > «All data solely-owned by ONC are available free of charge … These data are provided under a Creative Commons CC-BY 4.0 License, open and free for anyone to use with attribution.» 
  
  Ma: «Data owned or co-owned by a data partner may be subject to alternative licensing such as a Creative Commons CC-BY-NC license, or a more restrictive license … In some cases … assumed to have "All Rights" Reserved.» E: «Mixed re-use of data hosted at ONC with varying open data licenses will inherit the most restrictive data license applied to the component datasets.»
- **Citazione richiesta [V].** <https://www.oceannetworks.ca/data/how-to-cite-onc/>: autore del dataset, titolo, editore, anno, DOI, identificatore persistente della query. Linee guida: <https://wiki.oceannetworks.ca/spaces/DP/pages/78414590/Data+Citations>.
- **Contenuto.** Idrofoni a lungo termine in Columbia Britannica (Strait of Georgia, Salish Sea, Barkley Sound, Clayoquot, ecc.), dati di navigazione (AIS) abbinabili.
- **Formato.** **Non verificato su pagine ufficiali.** Fonti secondarie: idrofoni icListen HF a 64 kHz per i WAV (Saturna Island/SIMRES) [S]: <https://www.orcasound.net/2018/03/29/shortcuts-for-listening-to-the-canadian-pacific/>. Pacchetto comunitario `tehom`: mp3, wav, png, mat [S]: <https://tehom.readthedocs.io/en/stable/tehom.html>.
- **Registrazione [S].** L'API Oceans 3.0 richiede un token ottenuto registrandosi: <https://pypi.org/project/onc> (risultati di ricerca, non verificato sulla pagina ONC). Il registro re3data indica «restricted … registration» per parte dei dati: <https://www.re3data.org/repository/r3d100010154> [V].
- **Caveat sul partner.** Alcuni idrofoni di ONC sono di partner (ad es. SIMRES, Saturna Island): il sito Orcasound dice che l'uso dei dati richiede il permesso scritto del partner [S].
- **Uso nel gioco gratuito.** **Sì per i dati di proprietà esclusiva ONC (CC BY 4.0), con attribuzione e citazione DOI; da verificare caso per caso per i dati dei partner.** Il flusso consigliato: per ogni file scaricato, annotare la licenza riportata dal portale Oceans 3.0 [D].
- **Rilevanza per «688».** Alta: traffico navale reale (navi mercantili, traghetti, rimorchiatori) con AIS per l'etichettatura.

---

## 8. Registrazioni militari, didattiche o d'archivio: sottomarini, siluri, sonar attivo

### Risposta breve su sottomarini nucleari della Guerra Fredda

**Non ho trovato nessuna registrazione audio di un sottomarino nucleare (americano o sovietico) della Guerra Fredda che sia di pubblico dominio o rilasciata con licenza chiara.** Ho cercato su Internet Archive (via API), Wikimedia Commons (via API), NOAA/NCEI, catalogo del National Archives (non ha prodotto elementi) e su motori di ricerca. Le firme acustiche di sottomarini sono rimaste classificate: le ricerche trovano solo quelle documentali (ad es. analisi della Navy) e controversie (registrazioni svedesi del 1980, attribuite poi a un piccolo natante) [S]: <https://www.popsci.com/environment/the-cold-wars-accidental-whale-observatory/>. Il materiale SOSUS rilasciato negli anni '90 è oceanografico (balene, terremoti) [S].

### 8a. Australian War Memorial: «HMAS Oxley, Oberon class submarine – operational sounds» (trovata, interessante)

- **Scheda [V].** <https://www.awm.gov.au/collection/C1240481>, numero S04697. Sottomarino diesel-elettrico Oberon-class, registrazione di Defence Public Relations, c. 1980. Durata 1 h 16 min. Originale su DAT: **48 kHz, 16 bit, stereo**. Elenco: atmosfera in sala ufficiali, plancia che sale a quota periscopica, allarme Claxton, sala operazioni (suoni sonar), «underwater sound of cavitating propeller», lancio siluro in sala siluri, bersaglio cacciatorpediniere, ecc.
- **File scaricabile [V].** Il lettore della pagina usa `https://s3-ap-southeast-2.amazonaws.com/awm-media/collection/S04697/audio/3849992.MP3`, 17,26 MB (richiesta HEAD). Su 76 minuti sono circa 30 kb/s [D]: qualità bassa, non l'originale DAT.
- **Licenza [V].** «Item copyright: © Australian War Memorial. This item is licensed under CC BY-NC» con link <http://creativecommons.org/licenses/by-nc/3.0/au/>. Ma nelle «Copying Provisions» la stessa scheda dice: «Copyright restrictions apply. Only personal, non-commercial, research and study use permitted. Permission of copyright holder required for any commercial use and/or reproduction.» La pagina sul copyright dell'AWM descrive CC BY-NC 3.0 AU per testi e immagini e richiede attribuzione all'AWM: <https://www.awm.gov.au/about/organisation/corporate/copyright> [V]. Per l'audio non spiega come attribuire.
- **Uso nel gioco gratuito.** **Probabile sì, ma scrivere all'AWM prima di includere i file nel pacchetto** [D]. CC BY-NC 3.0 AU consente la ridistribuzione non commerciale con attribuzione, però la frase «personal, non-commercial, research and study use» e «any … reproduction» rende dubbia la ridistribuzione. Attribuzione: «Australian War Memorial, S04697 (CC BY-NC 3.0 AU)». Il gioco deve restare davvero non commerciale (niente donazioni legate al download né pubblicità) [D].
- **Rilevanza per «688».** Alta come atmosfera di sottomarino (ambienti, allarmi, torpedo room, sonar), anche se è diesel-elettrico e di classe diversa da un 688.
- **Altre schede AWM** dello stesso tipo (elicottero, pattugliatore Fremantle, Neptune): <https://www.awm.gov.au/collection/C1240487>, <https://www.awm.gov.au/collection/C1240502> [S, appaiono nei risultati di ricerca]. Non ho trovato altre schede di sottomarini.

### 8b. Navy / WWII / archivi

- **US Navy NCEI USWTR.** Registrazioni HARP/ARP 2000–2010 presso poligoni sottomarini, nel bucket NOAA (vedi 1c) [V]. Sono registrazioni ambientali, non di sottomarini.
- **WHOI Archives.** Dischi del USS Maloy (sonar a scansione, 1950) e le «Audograph Recordings» MC-109 (trasmissione del suono sott'acqua, «submarine noise», 1949–50) [S]: <https://archives.mblwhoilibrary.org/repositories/2/resources/220>. Sono materiale fisico; ritenuto non digitalizzato e con permesso dell'archivista necessario («Permission to publish … must be authorized by the Institution Archivist») [V per la regola, S per i contenuti].
- **«Sounds of the Sea» e album LP.** Smithsonian Folkways «Sounds of the Sea Vol. 1: Underwater Sounds of Natural Origin» (registrato dal Naval Research Laboratory) e «Sounds of Sea Animals» (FX 6125, Kellogg) [S]: <https://folkways.si.edu/sounds-of-the-sea-vol-1-underwater-sounds-of-biological-origin/album/smithsonian>, <https://fieldnoise.substack.com/p/seventy-years-of-listening-underwater>. Sono biologici, ancora sotto copyright Folkways. Su Internet Archive una copia di «Sounds of Sea Animals Vol. 2 Florida» (1955) senza licenza dichiarata [V]: <https://archive.org/details/lp_sounds-of-sea-animals-vol-2-florida_winthrop-niles-kellogg>. Non usabili senza permesso.
- **Filmati didattici della Navy.** Non ho trovato su Internet Archive un filmato di formazione sonaristi della Seconda guerra mondiale in pubblico dominio con audio di bersagli (la ricerca nel campo cinema+Prelinger ha dato 0 risultati). Una ricerca sul catalogo NARA non ha restituito risultati specifici [S]: <https://www.archives.gov/research/military/navy/special-media>. Non posso né confermare né escludere che esistano nel catalogo NARA.
- **Internet Archive, suoni «naval warfare».** Esistono 78 giri di effetti (ad es. «NAVAL WARFARE EFFECTS – Submarine Depth Charges», «Submarine-Destroyer Battle») senza licenza dichiarata [V]: <https://archive.org/details/78_naval-warfare-effects-submarine-depth-charges_gbia3000791a>. Sono effetti sonori commerciali d'epoca: stato del copyright non chiaro, **non usabili senza ulteriori verifiche**.
- **Wikimedia Commons.** Ricerche audio su sonar, sottomarino, siluro, idrofono: nessun risultato rilevante (solo pronunce del termine «torpedo» e clip di balene da fonti NPS in pubblico dominio) [V via API].
- **Altre fonti di sottomarini.** Altri sottomarini cui ho trovato solo notizie di stampa (Ivy Bells, ecc.) non sono registrazioni acustiche utilizzabili [S].

### 8c. Freesound

- **Termini [V].** <https://freesound.org/help/faq/>: tre licenze, CC0, CC BY 4.0, CC BY-NC 4.0 (più la vecchia Sampling+). «Some sounds you cannot use commercially, and many of the sounds you need to credit the author.» Formula suggerita: `"sound1" by user1 ( http://freesound.org/s/soundID/ ) licensed under CCBYNC 4.0`.
- **CC0 (ricerca filtrata) [V].** «submarine sonar» 53 risultati: soprattutto effetti sintetici (pack «Vintage Submarine Effects», «Synthesized Pings»), 24 a 44,1 kHz e 26 a 48 kHz: <https://freesound.org/search/?q=submarine+sonar&f=license%3A%22Creative+Commons+0%22>. Altre ricerche CC0: «hydrophone» 295 (soprattutto lavandini e laghetti, qualche fiume e barche), «underwater boat» 22, «sonar ping» 33, «torpedo» 26, «whale» 80, «icebreaker» 9. Esempi con pagina letta: «Submarine internal» (blaukreuz, 4 min, 48 kHz/24 bit, 5.1, CC0) <https://freesound.org/people/blaukreuz/sounds/465472/>, «Sea underwater … boat motor sometimes» (felix.blume, CC0) <https://freesound.org/people/felix.blume/sounds/705053/>.
- **Uso nel gioco gratuito.** **Sì per i file CC0** (nessun obbligo; consigliato crediti). Per CC BY serve l'attribuzione; per CC BY-NC va bene per un freeware non commerciale con attribuzione. Va controllata la licenza del singolo file al momento del download e registrata nel manifest di crediti del gioco [D].
- **Rilevanza.** Per la qualità «reale» la maggior parte dei CC0 sono sintesi o registrazioni amatoriali; per ping sonar e allarmi sono comunque pratici.

---

## 9. Altre fonti utili

### 9a. Orcasound (AWS Open Data)

- **Licenza [V].** <https://registry.opendata.aws/orcasound/>: **CC BY-NC-SA 4.0** (<https://creativecommons.org/licenses/by-nc-sa/4.0/>).
- **Contenuto [V].** Audio in diretta e archiviato (dal 2018) da idrofoni nelle acque di Washington e Columbia Britannica, per le orche residenti del Sud. Bucket `archive-orcasound-net` = FLAC lossless; `streaming-orcasound-net` = HLS; `acoustic-sandbox` = dati etichettati. Accesso anonimo.
- **Uso nel gioco gratuito.** **Sì, con condizioni:** non commerciale (compatibile con un freeware), attribuzione, e **ShareAlike**: le opere derivate (ad es. un campione modificato incorporato in un pacchetto) vanno distribuite con la stessa licenza [D]. Per un gioco con codice open source la clausola SA ricade solo sui campioni modificati. Meglio tenere i file Orcasound in una cartella a parte con la sua licenza.

### 9b. Macaulay Library (Cornell)

- **Termini [V].** <https://support.ebird.org/support/solutions/articles/48001064551> (pagina di aiuto): «media are not available for personal download unless they belong to you»; ascolto libero; incorporamento solo non commerciale; «Users may request access … for research and some educational uses for free. For all commercial uses (and some educational uses), a license agreement is required.» Si chiede di seguire le linee guida di attribuzione. Il campo di ricerca è sul sito Macaulay.
- **Uso nel gioco gratuito.** **No senza richiesta e accordo con Cornell** [D]. Pagina «terms-of-use» sul dominio macaulaylibrary.org: 404 [V]. Un gioco freeware non rientra con certezza nei «some educational uses».

### 9c. British Library (Sound Archive, wildlife)

- **Stato.** **Non verificato su pagine ufficiali.** Fonti secondarie dicono che registrazioni di fauna selvatica furono rese disponibili con licenze Creative Commons (CC BY su SoundCloud) [S]: <https://sonicfield.org/?p=2169>. Non ho trovato nelle fonti raccolte una sezione subacquea/marina. **Non lo consiglio** come fonte per questo uso senza una verifica diretta.

### 9d. OOI (Ocean Observatories Initiative, NSF)

- **Termini [V].** <https://oceanobservatories.org/how-to-use-acknowledge-and-cite-data/>: «All OOI data are freely available … The only requirement for use of OOI data is to acknowledge OOI as its source, provide citations where appropriate, and include acknowledgment of the US National Science Foundation's support.» L'assistenza OOI ha confermato che per gli idrofoni non c'è una licenza specifica, basta citare [V]: <https://discourse.oceanobservatories.org/t/license-type-for-mseed-hydrophone-recordings/753> (aprile 2024).
- **Formato.** I dati idrofonici sono in miniSEED, non verificato altro.
- **Uso nel gioco gratuito.** **Sì con attribuzione/riconoscimento OOI e NSF** [D].

### 9e. JASCO

- Nel bucket NOAA c'è la cartella `jasco/audio/jasco-esrf/` [V]. Non ho trovato una licenza per questi dati né una pagina JASCO con termini d'uso. **Non verificata.** JASCO è un'azienda privata; il § 105 non si applica [D]. Non usare senza leggere il README/scheda NCEI dello specifico dataset.

### 9f. Altri dataset di navi derivati da ONC

- VTUAD (IEEE DataPort, DOI 10.21227/msg0-ag12) e Oceanship (arXiv 2401.02099, 121 ore da ONC): licenza non trovata, VTUAD richiede abbonamento IEEE DataPort [S]. Non consigliati; meglio andare alla fonte ONC.

---

## Pagine che non si sono aperte o non sono state lette per intero

- `whoicf2.whoi.edu` / `cis.whoi.edu` (Watkins live): «Under Maintenance» (WebFetch: certificato rifiutato). Lette solo le copie Wayback; i WAV non sono nelle copie. Il repository WHOAS `darchive.mblwhoilibrary.org/handle/1912/5877` mostra un CAPTCHA.
- `dosits.org`: curl dà 403; lette le pagine con WebFetch. Le pagine di «Sonar» e «SURTASS LFA» non sono state aperte; la pagina MFA ha dato 404 con l'URL che ho provato.
- `noaa.gov/digital-library` e `photolib.noaa.gov`: HTTP 405 anche da WebFetch. La regola «le immagini della photo library sono pubblico dominio» è quindi [S]. Non riguarda l'audio.
- `oceannetworks.ca/data-licence/` e analoghe: 404; la politica dati vera è `/data/data-policy/`.
- Articolo DeepShip su Elsevier: reindirizzamento non seguibile; origine ONC solo da fonti secondarie.
- `macaulaylibrary.org/terms-of-use/`: 404. `ncei.noaa.gov/about/privacy-policy`, `/disclaimer`: 404.
- British Library: non aperta.
- Nelle copie Wayback di Watkins non ho verificato la frequenza di campionamento.

---

## Cosa usare per il gioco: consigli pratici

1. **Subito utilizzabili con attribuzione e poca burocrazia:** MBARI/Pacific Sound (CC BY 4.0), ONC dati propri (CC BY 4.0), NOAA PMEL (citare la fonte), NCEI/NRS/SanctSound per dataset federali (citare DOI), OOI (citare OOI e NSF), Freesound CC0.
2. **Con permesso scritto:** Watkins (modulo del museo), AWM Oxley (CC BY-NC, ma ambiguità sulla riproduzione), ShipsEar e DeepShip (nessuna licenza), DOSITS (per suoni non CC), Macaulay.
3. **Con vincolo di licenza:** Orcasound (CC BY-NC-SA).
4. **Non trovati:** registrazioni di pubblico dominio di sottomarini nucleari della Guerra Fredda. Per i suoni di sottomarino conviene combinare: rumore di navi reali (MBARI, ONC, NOAA, ShipsEar), idrofoni di fauna (Watkins, NOAA), atmosfera di bordo e sonar da AWM Oxley (se il permesso arriva), effetti sintetici CC0 per ping e allarmi.

---

## Tabella riassuntiva

| Archivio | Contenuto | Licenza | Uso nel gioco gratuito | Attribuzione |
|---|---|---|---|---|
| NOAA PMEL Acoustics (sounds/multimedia) | Clip brevi: ghiaccio, terremoti, balene, navi, airgun (8–44,1 kHz, 16 bit mono; molti accelerati) | Nessuna licenza formale: «public information … may be distributed freely»; § 105 | Sì [D], salvo materiali marcati «copyrighted» | «NOAA PMEL Acoustics Program» + URL della pagina; niente logo/patrocinio |
| NOAA NRS (via NCEI) | 13 stazioni dal 2014, >63 anni di audio, FLAC 5 kHz/16 bit/mono, file da 4 h | Nessuna licenza esplicita; federale (§ 105) | Sì [D] | Citazione DOI 10.7289/V5M32T0D |
| NOAA NCEI Passive Acoustic Archive / SanctSound | Bucket con 30 progetti; SanctSound 30 siti 2018–2021; clip WAV 48 kHz/16 bit, anche a 8x | Nessuna licenza esplicita (solo garanzie); catalogo data.gov incoerente (CC0 vs non-public) | Sì per dataset federali [D]; da verificare per partner (JASCO, Cornell, ecc.) | Citazione DOI archivio (10.25921/PF0H-SQ72) o del progetto |
| Watkins Marine Mammal Sound Database | ~2.000 registrazioni, >60 specie; 1.694 clip «best of», ~15.000 clip, ~1.600 nastri; WAV, sampling non verificato | Termini propri: scarico gratis per uso personale/accademico, non commerciale; vietato ripubblicare su siti terzi | Condizionato: probabilmente no senza il modulo «Permission to publish» del museo | Obbligatoria: «Watkins Marine Mammal Sound Database, Woods Hole Oceanographic Institution and the New Bedford Whaling Museum» |
| DOSITS (URI) | ~115–120 suoni, fauna e antropici (nave, siluro, sonar, airgun); nessun sottomarino | © All rights reserved; alcuni suoni CC (per lo più BY-NC v3/v4), non elencati | Solo caso per caso: CC BY-NC sì; altri richiedono permesso (askDOSITS@gmail.com) | Secondo la licenza del singolo suono |
| ShipsEar (Vigo) | 90 registrazioni, 11 tipi di nave, ~3 h; WAV 24 bit mono 52.734 Hz | Nessuna licenza; «©Uvigo-Sonitum 2016»; accesso completo su richiesta | Non chiaro: chiedere permesso scritto | Citare Santos-Domínguez et al. 2016 |
| DeepShip (NWPU) | 265 navi, 4 classi, 47 h 04 min; WAV float 32 bit mono 32 kHz; probabile origine ONC [S] | Nessuna (GitHub senza licenza) | Non chiaro: usare la fonte ONC (CC BY 4.0) o chiedere all'autore | Dipende; per ONC: citazione ONC |
| MBARI MARS / Pacific Sound | Idrofono dal 2015, 256 kHz, 16 kHz (24 bit), 2 kHz; AWS S3 anonimo | CC BY 4.0 | Sì (anche commerciale) | Sì: ente, titolo, licenza, modifiche; «accessed on DATE from https://registry.opendata.aws/pacific-sound» |
| Ocean Networks Canada | Idrofoni in BC (navi, fauna); WAV/MP3 (formato non verificato) | CC BY 4.0 per i dati di proprietà ONC; partner con CC BY-NC o più restrittive | Sì per dati ONC; verificare per i partner | Citazione completa con DOI e identificatore di query; registrazione al portale (API con token) [S] |
| AWM «HMAS Oxley, operational sounds» (S04697) | Sottomarino Oberon c. 1980, 1 h 16 min; MP3 pubblico ~30 kb/s (originale DAT 48 kHz/16 bit stereo) | CC BY-NC 3.0 AU, ma «only personal, non-commercial, research and study use permitted» | Probabile sì, ma chiedere conferma all'AWM | «Australian War Memorial, S04697, CC BY-NC» |
| Sottomarini nucleari Guerra Fredda | Nessuna registrazione di pubblico dominio trovata | n/d | n/d | n/d |
| Internet Archive (PMEL mirror; 78 giri «naval warfare effects») | Mirror di file NOAA (PDM); effetti d'epoca senza licenza | PDM per mirror NOAA; ignota per gli altri | Mirror NOAA: sì (citare PMEL); effetti d'epoca: no senza verifica | Citare PMEL |
| Freesound | Effetti e registrazioni di utenti; 53 CC0 per «submarine sonar» | CC0, CC BY 4.0 o CC BY-NC 4.0 per file | CC0 sì; BY/BY-NC con attribuzione (NC: solo non commerciale) | CC0 nessuna; altrimenti autore + link + licenza |
| Orcasound | Idrofoni per orche, FLAC, dal 2018 | CC BY-NC-SA 4.0 | Sì (non commerciale) con ShareAlike sui derivati | Sì |
| OOI (NSF) | Idrofoni cablati; miniSEED | Nessuna licenza specifica, solo riconoscimento | Sì [D] | OOI e NSF; citazioni dei dati |
| Macaulay Library (Cornell) | Fauna selvatica, include audio di cetacei | Licenza su richiesta; niente scarico personale | No senza accordo con Cornell | Linee guida di Macaulay |
| British Library | Registrazioni naturali, nessuna sezione subacquea trovata | CC BY (riportata da fonti secondarie) | Non verificata | n/d |
| JASCO (nel bucket NOAA) | Dati ESRF | Non trovata | Non verificata | n/d |
