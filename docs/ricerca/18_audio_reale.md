# 18 — Audio reale: registrazioni, variazioni, ascolto d'epoca e suono di bordo (dossier per il remake di *688 Attack Sub*)

> Requisito 27 di `docs/00_ROADMAP.md` (comparto sonoro dal modello acustico), con le richieste dell'utente del
> 25/10/2026: partire da registrazioni vere dove esistono, con le variazioni dello stesso oggetto secondo i fattori
> reali; e far sentire a bordo, senza cuffie, ciò che arriva attraverso lo scafo.
> Solo ricerca: la proposta a pezzi è in fondo (§8). Niente codice finché l'utente non sceglie.
> Compilato: 25 ottobre 2026, da quattro ricerche parallele (archivi e licenze, fattori di variazione, ascolto storico,
> suono di bordo) più le verifiche della sessione sull'acustica (motore audio, mescolamento con il modello).
> Le quattro ricerche integrali, con tutti i link e le etichette [V]/[S]/[D], sono in `docs/ricerca/18_note/`
> (`archivi.md`, `variazioni.md`, `ascolto_storico.md`, `bordo.md`): questo dossier ne è la sintesi.

---

## 0. Convenzioni

- **Confidenza:** alta (fonte primaria letta), media (fonte seria ma secondaria, o dato misurato su un altro tipo di
  nave), bassa (letteratura grigia, forum, riassunti).
- **[V]** verificato sulla fonte; **[S]** fonte secondaria; **[D]** deduzione; **STIMA** numero ricavato da una legge
  o da un calcolo, da trattare come parametro di gioco.
- Non è un parere legale: per le licenze dubbie la strada sicura è una mail al detentore dei diritti, con la risposta
  conservata.
- Il gioco è gratuito. «Non commerciale» nelle licenze CC BY-NC lo copre oggi, ma vincola il futuro (donazioni,
  ripubblicazioni): la proposta (§8) preferisce CC0, pubblico dominio e CC BY.

---

## 1. Archivi di registrazioni subacquee e licenze

Tutte le licenze sono state lette sulle pagine ufficiali il 25/10/2026, salvo dove indicato.

| Archivio | Contenuto e qualità | Licenza | Uso nel gioco gratuito | Attribuzione |
|---|---|---|---|---|
| **NOAA PMEL Acoustics** (`pmel.noaa.gov/acoustics/sounds.html`, `.../multimedia.html`) | clip brevi di ghiaccio, terremoti, balene, navi, airgun; WAV mono 16 bit a 8 kHz (già accelerati ×3, ×10, ×16) o 44,1 kHz | «public information and may be distributed freely… please cite NOAA PMEL Acoustics Program» [V]; opere federali, 17 U.S.C. §105 [V] | **sì** (escluso il materiale segnato «copyrighted»; niente logo NOAA) | «NOAA PMEL Acoustics Program» + URL |
| **NOAA/NPS NRS** (13 stazioni dal 2014, via NCEI) | FLAC 5 kHz 16 bit mono, file da 4 h, banda 10-2000 Hz | nessuna licenza esplicita, solo esclusione di garanzie; federale (§105) | **sì** [D] | DOI 10.7289/V5M32T0D |
| **NOAA NCEI / SanctSound** (bucket `noaa-passive-bioacoustic`, accesso anonimo) | 30 progetti; clip SanctSound WAV 48 kHz 16 bit, anche ×8 | idem; il catalogo data.gov è incoerente (CC0 e «non-public» insieme) [V] | **sì per i dataset federali**; da verificare uno per uno quelli di partner (JASCO, Cornell, università) | DOI dell'archivio 10.25921/PF0H-SQ72 o del progetto |
| **MBARI Pacific Sound** (idrofono MARS a 900 m, Monterey, dal 2015; AWS Open Data) | WAV 24 bit a 256 kHz, 16 kHz e 2 kHz; navi mercantili, balenottere azzurre e comuni, megattere | **CC BY 4.0** [V: registro AWS e pagina MBARI] | **sì** (anche commerciale, con modifiche) | MBARI, titolo, licenza, modifiche; «accessed on DATE from registry.opendata.aws/pacific-sound» |
| **Ocean Networks Canada** (idrofoni in Columbia Britannica, con AIS) | traffico navale reale; formato non verificato sulle pagine ONC (icListen 64 kHz secondo fonti secondarie) | **CC BY 4.0** per i dati di sola proprietà ONC; i partner possono avere CC BY-NC o diritti riservati [V: data-policy] | **sì per i dati ONC**, caso per caso per i partner | citazione con DOI e identificativo della query |
| **OOI** (NSF) | idrofoni cablati, miniSEED | nessuna licenza specifica, solo il riconoscimento [V] | sì [D] | OOI e NSF |
| **Freesound** | effetti e registrazioni d'utente | per file: CC0, CC BY 4.0, CC BY-NC 4.0 [V: FAQ] | **CC0 sì**, CC BY con credito; da controllare file per file | per CC BY: autore, titolo, link, licenza |
| **Orcasound** | idrofoni per le orche, FLAC dal 2018 | CC BY-NC-SA 4.0 [V] | sì ma non commerciale e con ShareAlike sui derivati: da evitare o isolare | sì |
| **Watkins Marine Mammal Sound Database** (WHOI e New Bedford Whaling Museum) | circa 2000 registrazioni di oltre 60 specie, dagli anni '40; WAV | «free to download for personal or academic (not commercial) use. Reposting… on third party websites is not permitted» [V, copia Wayback: il sito è in manutenzione] | **no senza** il modulo «Permission to Publish» del museo | dicitura esatta obbligatoria |
| **DOSITS** (URI) | circa 115 suoni, fauna e antropici (nave, siluro, sonar, airgun); nessun sottomarino | tutti i diritti riservati; alcuni suoni CC (per lo più BY-NC) non elencati [V] | solo caso per caso, o col permesso (askDOSITS@gmail.com) | secondo il suono |
| **ShipsEar** (Università di Vigo) | 90 registrazioni di 11 tipi di nave, circa 3 h; WAV 24 bit mono a 52.734 Hz | nessuna licenza, «©Uvigo-Sonitum» [V] | serve il permesso scritto | Santos-Domínguez et al., *Applied Acoustics* 113 (2016) |
| **DeepShip** (NWPU) | 265 navi in 4 classi, 47 h, WAV float a 32 kHz | repository senza licenza [V]; origine probabile ONC [S] | meglio la fonte ONC, o il permesso dell'autore | — |
| **Australian War Memorial S04697**, «HMAS Oxley (Oberon) – operational sounds», c. 1980 | 1 h 16 min: centrale, sonar, elica in cavitazione, lancio siluro, allarmi; l'MP3 pubblico è a circa 30 kb/s (originale DAT 48 kHz) | CC BY-NC 3.0 AU, ma la scheda dice «only personal, non-commercial, research and study use… permission required for… reproduction» [V] | **probabile, da chiedere all'AWM** prima di includerlo | «Australian War Memorial, S04697» |
| Macaulay Library (Cornell), BBC Sound Effects (RemArc), British Library, JASCO | — | Macaulay: accordo su richiesta; BBC RemArc: solo uso personale, «condividere» vietato, revocabile [V, versione 2016]; British Library e JASCO non verificati | **no** | — |

**Sottomarini nucleari della Guerra Fredda: nessuna registrazione di pubblico dominio trovata** (Internet Archive,
Wikimedia Commons, NOAA, NARA). Le firme restano classificate; il materiale SOSUS rilasciato è oceanografico. L'unico
materiale militare reale trovato è l'Oberon dell'AWM. I dischi a 78 giri di effetti («Naval warfare effects –
submarine depth charges») su Internet Archive non hanno uno stato di copyright chiaro: da non usare.

**Interni di sommergibile:** il pacchetto Freesound CC0 «Vintage Submarine Effects» (craigsmith, 12 file: sala
macchine, centrale, sala siluri prodiera, esplosioni distanti, 48 kHz mono) è utilizzabile, ma secondo l'uploader sono
effetti da studio hollywoodiani degli anni '30-'40, non registrazioni documentate a bordo. Nessuna registrazione
interna di un battello militare reale con licenza libera, salvo l'AWM.

**Mezzi aerei** (per tessiture da filtrare, §6): su Freesound molti CC0 (Chinook, UH-1, Sea King, Black Hawk, Bell 407,
Lockheed L-188 Electra — il parente civile del P-3 con gli stessi Allison T56 —, C-130, F-15, F-18), alcuni CC BY 4.0
(turboelica a Ekaterinburg); su Wikimedia Commons un Chinook CC0 e due elicotteri di pubblico dominio (pdsounds).
Elenco con i link nel §6.4.

---

## 2. Come mescolare campioni e modello

Il criterio dell'utente: **cascata, LOFAR e ascolto devono restare coerenti.** Ciò che si sente deve stare sullo
schermo e viceversa, alla stessa distanza, con lo stesso Doppler e la stessa perdita. Il motore ha già tutti i pezzi
dal lato del modello: firme e libreria (`nucleo/acustica.hpp`), righe ricevute e spettro del fascio
(`nucleo/analisi_acustica.hpp`), la sintesi d'ascolto (`nucleo/ascolto.hpp`, S5d). Proposta [D, scelta di progetto]:

### 2.1 Chi decide che cosa

| Componente del suono | Chi lo decide | Perché |
|---|---|---|
| **Livello per banda** (l'inviluppo spettrale ricevuto) | il modello: sorgente, perdita, fascio, rumore | è quello che misura la cascata e decide la scoperta |
| **Righe** (rete, pompe, turbine, pale) e **modulazioni** (DEMON, battito d'elica) | il modello | sono le righe del LOFAR e il conteggio delle pale; cambiano con giri, regime e danni (§3) |
| **Doppler, ritardo, perdita con la distanza, mascheramento** | il modello | stesso calcolo per tutte le presentazioni |
| **Tessitura** (la struttura fine nel tempo e nel timbro: canto di balena, clic dei gamberi, crepitio del ghiaccio, pioggia, onde, la cavitazione «vera» di un mercantile) | il campione registrato | il modello non sa produrre questa grana in modo credibile |

### 2.2 Il procedimento (per ogni campione della libreria)

1. **Analisi a monte**, nella pipeline e non a run time:
   - l'inviluppo medio in terzi d'ottava;
   - le righe stabili (con l'algoritmo di `righe_stabili` dell'ACINT);
   - la modulazione d'inviluppo (DEMON);
   - il ritmo degli eventi.

   I risultati diventano i metadati della voce in `sorgenti_libreria.py`: componenti, righe, ritmo. Così cascata e LOFAR
   vedono ciò che il campione contiene.
2. **Sbiancamento:** il campione viene diviso per il suo inviluppo medio (in STFT, a terzi d'ottava). Resta la tessitura
   a spettro piatto.
3. **Ricolorazione a run time:** si moltiplica per l'inviluppo che il modello calcola per quella sorgente in quel fascio:
   livello, perdita con l'assorbimento in frequenza, risposta del fascio, filtro del pannello. Il guadagno per banda
   si aggiorna ogni 0,25-0,5 s, con dissolvenza.
4. **Le righe** non vengono dal campione: le sinusoidi del modello si sommano dopo.
   - Per navi civili e biologici le righe proprie della registrazione restano nella tessitura, ma sono le stesse che
     l'analisi del punto 1 ha messo nella libreria.
   - Per le unità militari le righe del campione vanno attenuate, con un notch sulle stabili, e sostituite da quelle
     del modello.
5. **Doppler:** ricampionamento a rapporto variabile `1 + v_r / c`, aggiornato a ogni blocco, con un interpolatore a
   finestra. Lo stesso fattore sposta le sinusoidi del modello.
6. **Propagazione:** perdita per banda dal motore, cioè passa basso con la distanza per l'assorbimento. A scelta un
   pettine d'interferenza dello specchio di Lloyd per le sorgenti vicine alla superficie [D]. Il ritardo si ignora per
   la cuffia, mentre per il suono di bordo conta l'ordine degli eventi (§6.5).
7. **Durata e ripetizione:** inizio, segmenti e dissolvenze vengono dal seme dell'istanza (come `attivita` del paesaggio
   sonoro), così la stessa partita suona uguale e due balene non cantano all'unisono.
8. **Ascolto accelerato** (×4, ×16) per i suoni sotto 30 Hz, come NOAA PMEL [V]: il campione si legge più in fretta e
   il modello moltiplica le frequenze, come in `sintetizza`.

### 2.3 Per categoria

| Categoria | Campione | Modello | Fonti di campioni con licenza libera |
|---|---|---|---|
| Biologici (balene, delfini, gamberi, pesci) | sì: la tessitura è tutto | livello, ritmo, distanza | NOAA PMEL e SanctSound, MBARI (CC BY), Freesound CC0; Watkins solo con permesso |
| Ambiente (pioggia, onde, ghiaccio, terremoti) | sì | livello col vento e la pioggia (Wenz), distanza | NOAA PMEL (criogenici), MBARI |
| Navi civili e traffico | sì, per la cavitazione e il «battito» reale | righe (da analisi), Doppler, perdita, aspetto | MBARI, ONC (CC BY), NOAA NRS; ShipsEar e DeepShip con permesso |
| Navi militari e sottomarini | **no** (non ne esistono di liberi; l'Oberon dell'AWM solo come riferimento d'ascolto o con permesso) | tutto il modello | — |
| Siluri, ping, lancio, esplosioni | modello (sintesi) o effetti CC0 per il solo timbro d'attacco | livelli, tempi, bolla | Freesound CC0 (ping sintetici), il pacchetto CC0 craigsmith (esplosioni distanti) |
| Aerei ed elicotteri (in superficie e al sonar) | sì, per il timbro di rotore e turboelica | finestra di Snell, quota, Doppler (dossier 17 §8.3) | Freesound CC0, Wikimedia CC0/PD |

---

## 3. Fattori di variazione non ancora coperti dal motore

Il motore copre oggi velocità (tabella per classe), cavitazione con la profondità, righe dei macchinari a tabella,
righe di pala (Ross, 60 log V), posato e snorkel. Per i fattori nuovi, quanto cambia e con quali fonti:

| Fattore | Che cosa cambia | Quanto | Fonte | Conf. |
|---|---|---|---|---|
| **Giri di una macchina rotante** (pompa, ventilatore) | riga di rotazione e multipli; frequenza ∝ giri | potenza sonora da squilibrio ∝ n⁴: per una pompa a due velocità di rapporto r, 40 log r (r = 2: +12 dB; r = 1,5: +7 dB) | [Ross76] cap. 10.1 | alta la legge, STIMA l'applicazione |
| **Pompe del reattore** | presenza e livello delle righe di pompa | il reattore a circolazione naturale può fare a meno delle pompe fino a circa 10 nodi [S]; pompe «necessarie solo ad alta velocità» sull'S5G; **nessuna conferma aperta** di pompe a due velocità sull'S6G del 688 | [GS-S5G] [TWZ-Narwhal] | media/bassa |
| **Rete elettrica** | posizione e larghezza delle righe 50/60 Hz e armoniche; ronzio a 2f | tolleranza ±5% per i sommergibili, modulazione 0,5% (±0,3 Hz a 60 Hz), transitori ±4% per 2 s; larghezza di riga 0,1-1 Hz | [MIL1399] (ed. 2008; il 1989 non verificato), [Mia98] | alta / media |
| **Ausiliari accesi e spenti** | righe che compaiono e spariscono | somma incoerente: una sorgente 12 dB sotto il totale non muove la banda larga ma si vede nel LOFAR; nave di ricerca +10 dB con +44 kW di carico | [Ross76] App. B, [Mit02] | alta / media |
| **Transitori** (piani, portelli, cambi di profondità) | impulsi brevi e forti | **nessun dato aperto**; le tesi NPS li modellano come processi di Poisson | [Xen] [Kra88] | bassa |
| **Regime del battello** | banda larga | ~4 → ~8 kn: +5-10 dB; poi SL ∝ V⁶ (+18 dB per raddoppio) | [Mia98] | media |
| **Aspetto** | banda larga per angolo | nave di 40 m: prua −12 dB e poppa −9 dB dal traverso, modello cos^1,95(θ − 97°) + 0,08; mercantili: poppa 5-10 dB sopra la prua, in parte effetto di Lloyd; toni sotto 30 Hz quasi omnidirezionali; **nessuna misura aperta di sottomarini** | [Tre08] [McK12] [Gas17] [AV00] | alta (navi), STIMA (sottomarini) |
| **Virate** | eccesso sul rettilineo | +4-10 dB all'inizio; ~1,65 dB per °/s a bassa frequenza, fino a +18-20 dB | [Tre08] | alta (nave), media (trasposta) |
| **Danni all'elica** | cavitazione precoce e più forte | «danni o sporco aumentano molto la cavitazione»; +10% di resistenza ≈ >10 dB; all'insorgenza +8-15 dB [S]; pale non identiche: bande laterali a ±1× i giri; elica «cantante» 100-1000 Hz | [Spo88] [Ross76] cap. 8.6, 9.5 | media |
| **Macchine squilibrate o con cuscinetti rovinati** | riga di rotazione, armoniche, bande laterali | soglie di monitoraggio: +6-8 dB significativo, +20 dB guasto serio | [Shin91] | alta (vibrazioni), media (irradiato) |
| **Usura nella vita** | livello generale | +5 dB in media | [Mia98] | media |
| **Rivestimento anecoico staccato** | forza di bersaglio e flusso | 5-7% perso su tre Virginia (2010); nessun dB sul rumore irradiato | [BDN10] | bassa |
| **Variazione individuale** | livelli e righe fra unità | stessa nave misurata più volte: σ 2,4 dB (giorni), 3,3 dB (anni); navi diverse: σ 5-6,8 dB; per le gemelle solo «possono essere molto diverse»; il «ritratto acustico» delle righe discrete è unico per ogni sottomarino e a volte identifica lo scafo | [Spo88] [SH91] [Mia98] [How11] | alta (navi), media (sottomarini) |
| **Pescaggio, mare, gas disciolto** | cavitazione | +1,5 m di pescaggio <2 dB; mare grosso «aumenta molto» la cavitazione; fino a 10 dB fra inverno ed estate | [Zob23] [Spo88] [Ross76] cap. 8.2 | alta/media |

**Proposta di parametri (STIMA, per l'Enhanced; il classico non cambia):**
- **Regime dei macchinari** per classe: stati (pompe spente, lente, veloci; turbogeneratore; ausiliari) con le righe
  che compaiono e spariscono e con la legge di Ross 40 log r. La soglia di circolazione naturale va decisa: per il 688
  non ci sono dati aperti.
- **Aspetto:** la forma di Trevorrow per la banda larga, con la poppa meno attenuata di una nave e le righe sotto
  100 Hz quasi omnidirezionali (±3 dB).
- **Danni:** modificatori discreti, cioè cavitazione anticipata con +8-15 dB, +6-20 dB sulle righe della macchina colpita
  e bande laterali d'asse.
- **Seme per unità:**
  - banda larga ±3 dB (σ di Sponagle);
  - righe di rete identiche in tutta la classe;
  - righe legate ai giri spostate di ±1-3% (STIMA: nessuna fonte con percentuali);
  - un'elica «cantante» con probabilità bassa.

  Così due Victor III non suonano identici, e la libreria dell'ACINT (S5d) può riconoscere lo scafo già incontrato.
- **Virate:** eccesso col rateo di virata.
- **Transitori:** processi di Poisson con cause da elenco, perché non ci sono tassi pubblici.

---

## 4. Come si ascoltava davvero (fine anni '70 - 1989)

- **La classificazione era in due tempi.** Prima un'impressione all'ascolto, poi 30-45 s di analisi su motore, pompe e
  pale [TWZ20]. Nell'esperimento ONR di Mackie (1974) l'operatore classificava quattro volte: dopo 10 s d'ascolto,
  dopo un'analisi libera fino a 5 min, dopo 8 indizi standard più il conteggio delle pale, e dopo aver visto i valori
  giusti. L'accuratezza salì dal 39 al 44, al 49 e al 57%. Audio e video insieme battevano il solo audio [Mac74].
  (alta)
- **Nel 1983-88 la presentazione era «primarily visual»** [Kin83]; i display a colori erano annunciati come imminenti
  nel 1988 [Lur88]. L'orecchio restava il canale d'allerta e di disambiguazione: banda larga, transitori, biologici,
  sonar attivi e siluri, con la «fusione mentale» di lofargramma e cuffia come mestiere da insegnare [Bar01]. Il peso
  relativo nel 1989 non è misurato da nessuna fonte. [D] (media)
- **Vocabolario dell'orecchio** [NAV76] [Mac74]:
  - gamberi: un ronzio, poi «grasso che sfrigola», poi «sterpi che bruciano»;
  - balene: colpi, gemiti, «un cancello arrugginito»;
  - capodoglio: un martellare;
  - siluro: prima l'aria, poi un rumore d'elica con battiti troppo rapidi da contare, simile a un jet ma meno acuto;
  - mercantili: più «laboring» e meno «galloping» delle navi da guerra;
  - il corso aurale della Royal Navy elenca whine, futter, shaft rub, diesel roar, trawl noise e cavitazione, con il
    conteggio di giri e pale [RN].
- **Registratori e libreria.**
  - La carta: BQQ-3 a stilo termico, ancora su 7 battelli su 12 nel 1981 [Kin81], e poi i LOFARgram elettrostatici.
  - Il nastro magnetico, anche «multiple tape recorders running as backup» su un SSBN nel 1986-91 [Che24].
  - La libreria delle firme sovietiche esisteva ed era fatta a terra e dai sommergibili [Cot00].
  - Il confronto lo facevano gli specialisti ACINT, il supervisore e i sonaristi ONI imbarcati [SSEP].
  - La classificazione automatica delle minacce fu **tolta** dal BSY-1 [GAO92]. Per il 1989 la classificazione è quindi
    lavoro umano. (alta)
- **Console e sala sonar.**
  - BQQ-5: da una console a tre (OJ-274, poi OJ-431), calcolatore UYK-7; il BQQ-5D arriva nel 1988; il BQQ-5E (colore,
    TB-29) è degli anni '90 e non va usato per il 1989 [Fore] [GSec].
  - La guardia: 6 ore su 12, tre sonaristi a rotazione oraria più il supervisore, in piedi dietro di loro.
  - La sala: luce rossa o blu tenue (da meno di 0,01 a 0,28 foot-candle sulle console), CRT a circa 60 cm [Kin81]
    (alta).
- **Salvataggi, annotazioni e marcatori sulle console del 1989: nessuna fonte aperta.** Gli strumenti S5b-S5d del
  gioco (slot, annotazioni, verniero) restano una scelta moderna «alla SDR», da segnare come tale. [D]
- **Sovietici:**
  - Skat (dal 1968, TsNII Morfizpribor) con quattro calcolatori Karat, uno per la classificazione;
  - due monitor NPO Mars a «frame» per sottosistema;
  - un magnetofono ad anello di 5 s che registra l'ultimo segnale sonar intercettato, da riascoltare per classificarlo
    [Kaz19] (alta sul testo).
  - Nulla di verificato sull'uso operativo né sui sonaristi sovietici; le varianti MGK-500, 503 e 540 non concordano
    fra le fonti.

---

## 5. Motore audio per il gioco

### 5.1 Lo stato vero dell'uscita (verificato il 25/10/2026)

Il messaggio della build «ALSA non trovato» **non impedisce l'uscita audio**. La libreria SDL3 3.2.10 di
`tools/deps/sysroot` (README del motore, «Compilazione») è compilata con i driver caricati a run time:
- `pipewire`, `pulseaudio`, `alsa`, `sndio`, `disk`, `dummy`;
- il sistema ha `libasound.so.2`, `libpipewire-0.3.so.0` e `libpulse.so.0`.

Un programma di prova (`SDL_Init(SDL_INIT_AUDIO)`, nessun suono riprodotto) apre il driver `pulseaudio` e vede due
uscite: l'audio interno analogico e l'HDMI. Serve `LD_LIBRARY_PATH` o l'rpath verso il sysroot per `libsndio.so.7`,
come per la grafica. [V]

### 5.2 Opzioni

| Opzione | Pro | Contro |
|---|---|---|
| **SDL3 audio** (`SDL_OpenAudioDeviceStream`, `SDL_PutAudioStreamData`) | già nella build, nessuna dipendenza nuova, nessun sudo; flussi con conversione di formato e ricampionamento | API un po' più verbosa di miniaudio per il missaggio |
| **miniaudio** (un solo header, pubblico dominio o MIT-0; ultima versione trovata 0.11.25, dalla 0.12 diventa .c più .h) [miniaudio] | mixer, nodi d'effetto e decoder WAV/FLAC/MP3 integrati | una dipendenza in più per qualcosa che SDL3 fa già; la versione 0.11.25 è letta da un pacchetto e non dalla pagina ufficiale |
| ALSA o PipeWire diretti | — | non servono: li usa già SDL |

**Proposta:**
- SDL3 per l'uscita, con un mixer nostro in `nucleo` (puro e deterministico, a blocchi) e un adattatore in `app`. La
  generazione dei campioni resta separata dalla riproduzione, così il replay e le prove headless restano identici.
- I decoder FLAC e WAV per la libreria si possono prendere da `dr_flac` e `dr_wav` (stesso autore di miniaudio, pubblico
  dominio [S]), oppure si convertono i file a monte nella pipeline.

### 5.3 Resa d'epoca accanto alla qualità piena

- **L'originale** suonava campioni PCM a 4 bit senza segno (due per byte) a 2-6,5 kHz:
  - esplosione 5000 e 4000 Hz, ping 5500 e 6200 Hz, siluro 6500 Hz, eliche ricampionate per classe nell'ANALYZE;
  - sull'altoparlante del PC, sul Tandy (SN76496) o sul Covox, con un ciclo d'attesa tarato sulla CPU.

  Documentato in `docs/01_analisi_tecnica_originale.md` §2.6 e `docs/originale/suoni.md`. La scheda del gioco elenca
  AdLib, Covox, altoparlante, Sound Blaster e Tandy [MobyG], ma il Sound Blaster era usato per la musica FM. [V]
- **Sound Blaster** [WP-SB] [DOSDays]:
  - SB 1.0: 8 bit mono fino a circa 23 kHz;
  - SB 2.0: fino a 44 kHz in riproduzione;
  - SB Pro: 8 bit stereo fino a 22 kHz.

  La prima scheda non aveva un filtro anti-aliasing, da cui il timbro «metallico»; la SB Pro aggiunse due filtri
  selezionabili. Il valore di quel filtro non è verificato. (media)
- **Tre rese (proposta):**
  - **«originale»**: 4 bit, 5-6 kHz, nessun filtro, con il ronzio dell'altoparlante come opzione;
  - **«Sound Blaster»**: 8 bit senza segno a 11 o 22 kHz, l'aliasing della SB 1.0 o il filtro della Pro;
  - **«piena»**: virgola mobile a 48 kHz, per l'ascolto analitico in cuffia.

  Le prime due sono un post-processo (quantizzazione, decimazione senza filtro e un passa basso fisso) sullo stesso
  flusso.

---

## 6. Il suono a bordo, senza cuffie

### 6.1 Che cosa si sente davvero

Le testimonianze verificate concordano su quattro cose udibili a orecchio, attraverso lo scafo, con il battello in
silenzio [Tam09] [Seal13] [Gun07] [Har71]:
1. **Le eliche di una nave sulla verticale.** Un rombo che cresce «come una locomotiva a vapore direttamente sopra»; a
   volte l'unico modo di seguire la scorta con il battello sul fondo. (USS Tambor, 1944, alta)
2. **Le cariche di profondità:**
   - lo splash e il tonfo delle cariche che scendono, il «clic» dell'innesco, il colpo come «un immenso martello» (vetri
     dei manometri rotti, sughero che vola);
   - le cariche lontane, come «un debole rimbombo» per tutto lo scafo;
   - due cariche inesplose sentite cadere e rimbalzare sul fianco. (alta)
3. **I ping delle scorte vicine**, descritti come ghiaia, sassolini, ticchettio d'orologio o martellate sullo scafo,
   quasi mai come un «ping» musicale: i sonar degli anni '40 lavoravano a 24-30 kHz, sopra l'udito [Mui21]. Nel
   Gunnel, 1942, «il pinging… risuonava forte contro lo scafo». (media-alta; i forum sono bassi)
4. **Una nave che affonda:** crepitii, caldaie che scoppiano, paratie che cedono. (alta)

**Nessuna testimonianza di prima mano** per balene, gamberi, sonoboe, elicotteri, siluri in homing o un altro
sottomarino vicino a orecchio: si sentono in cuffia [NAV76] [WHOI-shrimp]. In sala macchine con i motori in marcia non
si sente nulla di esterno: i macchinisti del Gunnel non si accorsero delle bombe [Gun07]. Un dato solido sul proprio
sonar attivo: i ping misurati a bordo della Tigrone sono 93-108 dB SPL nei locali prodieri, «in tutta la parte
prodiera della nave» [Har71].

### 6.2 La funzione di trasferimento acqua → scafo → interno (STIMA)

Lastra d'acciaio di 4 cm (HY-80) a sola massa, incidenza normale, da acqua ad aria [Lei12 per l'interfaccia, −61,5 dB
fra i riferimenti a parità d'intensità]:

**livello interno [dB re 20 µPa] ≈ livello esterno [dB re 1 µPa] − K(f)**

| f | 50-250 Hz | 500 Hz | 1 kHz | 2 kHz | 4 kHz | 8 kHz |
|---|---|---|---|---|---|---|
| K | 91-92 dB | 93 | 96 | 101 | 107 | 113 |

- È un **passa basso**: piatto fino a circa 500 Hz, poi circa 5-6 dB per ottava (legge di massa). Lo scafo di 3 cm dà
  circa 3-4 dB in meno in alto, quello di 6 cm 2-3 dB in più. Incertezza ±10-15 dB.
- **Correzioni** [D]:
  - riverbero del locale: da 0 a −9 dB su K;
  - risonanze di ordinate, anelli e coincidenza, con picchi fino a 10-20 dB fra 3 e 8 kHz;
  - sotto la frequenza d'anello (circa 160-200 Hz) lo scafo trasmette meglio del modello, quindi rimbombi e tonfi
    passano di più;
  - per le esplosioni conta l'accelerazione dello scafo intero, non l'acustica.
- **Controllo incrociato:** un trasduttore da 220-230 dB a pochi metri dà 103-113 dB dentro, contro i 93-108 misurati
  sulla Tigrone. Torna nell'ordine di grandezza. (media)
- **Profondità:** quasi nessun effetto sul passaggio dello scafo (meno di 1-2 dB, STIMA). Cambiano la propagazione e il
  rumore interno.

### 6.3 Il fondo di ogni stazione

| Stazione | Fondo interno | Particolarità |
|---|---|---|
| Centrale | 60-65 dBA da progetto; molto meno in assetto silenzioso [Cla92] | le ventole dai condotti |
| Locale siluri prodiero | come la centrale a macchine ferme | il più esposto ai ping propri (93-108 dB) e, per vicinanza al trasduttore, a quelli esterni [Har71] |
| Sala macchine | 87 dBA misurati su un battello RN, obiettivo 85; i diesel oltre 90 [Cla92] [Har71] | maschera tutto l'esterno |
| Alloggi e mensa | sotto 70 dB (criterio NAVSHIPS 1970) [Har71] | — |

Con la soglia d'udibilità (ISO 226, da memoria: 20 Hz circa 78 dB, 50 Hz 44, 1 kHz 2, 4 kHz −5) ne viene che a 20-50 Hz
anche 50-60 dB dentro restano sotto soglia, mentre a 4 kHz bastano pochi dB sopra un fondo basso. È per questo che si
sentono i ping e le cariche, mentre la balenottera a 20 Hz non si sente.

### 6.4 Aerei ed elicotteri: risposta onesta

- **A orecchio no**, né a quota periscopica né in profondità.
  - Il suono entra in acqua solo entro 13° dalla verticale (finestra di Snell) [Fer19] e la ricezione diretta «dura un
    paio di secondi» [AT19]: un turboelica a 151 m su un idrofono a 20 m si sente per circa 2,5 s.
  - Un EA-18G a 120-190 m dà 134 ± 3 dB re 1 µPa a 33 m di profondità [Kue20]; con K di 91-96 dB restano circa 40 dB
    dentro, sotto il fondo della centrale.
  - Ray theory di Young per un aereo a 150 m: a 18 m di profondità +2,4 dB sul livello in aria, a 100 m −5,8 dB
    [Ell00].
- **Al sonar sì, debolmente, per pochi secondi sulla verticale:** è quello che fa già `sorgente_aerea` (A2, dossier 17
  §8.3).
  - Le schiere rimorchiate li sentono anche da lontano per riflessione sul fondo, con preavviso di molti minuti
    [FS09].
  - Un elicottero in hover sul sonar passivo «suona come un cacciatorpediniere» lento [Ste60].
- **In superficie** (o con l'albero fuori) si sentono come in aria.
- Un rimbombo «attutito» dentro lo scafo sarebbe una scelta artistica, non realismo: se lo si vuole, va tenuto sotto
  40 dB, solo a macchine ferme e per pochi secondi.
- **Campioni per il timbro** (al sonar e in superficie, filtrati con la finestra e la funzione dello scafo):
  - Freesound CC0:
    - Chinook (surrey_film 84472);
    - UH-1 (Rmutt 156906);
    - Sea King (Sandermotions 333505, SirWilliam77 443427);
    - Black Hawk (John Sipos 156678; un loop di qubodup 854493, derivato e da verificare);
    - Bell 407 (bzamecnik 348991);
    - **Lockheed L-188 Electra**, turboelica T56 come il P-3 (HootOwl 464893);
    - C-130 (Sandermotions 330816);
    - F-15 e F-18.
  - Freesound CC BY 4.0: un turboelica (newlocknew 610271), un elicottero lontano (Speziax 645330).
  - Wikimedia: Chinook CC0, due elicotteri di pubblico dominio (pdsounds).
  - BBC RemArc no.

### 6.5 Proposta: l'«ambiente sonoro di bordo» per stazione

- Le **stesse sorgenti della scena acustica** con un percorso diverso. Al posto del sensore c'è lo scafo: livello
  esterno al punto del battello (sorgente, perdita, Doppler, ma omnidirezionale e senza guadagno d'antenna), poi K(f)
  per banda, poi il riverbero del locale.
- **Il fondo della stazione** (centrale, siluri, macchine, alloggi) cambia con il regime: assetto silenzioso,
  macchine in moto, ventilazione. Il mascheramento viene dalla stessa somma per banda contro la soglia d'udibilità.
- **Gli eventi:**
  - le eliche sulla verticale come evento a tre fasi (rombo crescente, splash e clic, botto);
  - i ping esterni come ticchettio o ghiaia sullo scafo: il transitorio d'inizio impulso e la risposta dello scafo,
    non il tono [D];
  - i ping propri forti nei locali prodieri;
  - le cariche e le esplosioni con l'accelerazione dello scafo (scossone e oggetti che vibrano, accanto all'acustica);
  - i siluri vicini e la propria uscita d'aria al lancio;
  - i crepitii di un relitto che affonda.
- **La cuffia del sonarista** resta il canale analitico. Quando il giocatore è alla stazione sonar si ascolta la cuffia,
  nelle altre stazioni l'ambiente di bordo; la cuffia può essere mescolata piano anche altrove, come opzione.
- **Gli aerei a bordo non si sentono**, salvo che in superficie.

---

## 7. Lacune

1. Nessuna registrazione libera di sottomarini nucleari o di navi militari della Guerra Fredda: per le unità militari
   tutto viene dal modello.
2. Pompe a due velocità e soglia di circolazione naturale del 688 (S6G): nessuna fonte aperta.
3. Direttività misurata di sottomarini, transitori, tassi di commutazione degli ausiliari, effetto in dB di allagamenti
   e danni allo scafo: nessuna fonte aperta.
4. Percentuali di differenza in Hz fra le righe di unità gemelle: nessuna fonte.
5. Livelli interni misurati per stazione su un sottomarino nucleare: solo criteri di progetto e misure RN.
6. Funzioni di salvataggio, annotazione e marcatori delle console del 1989: nessuna fonte.
7. Frequenze e livelli di AQS-13/18, delle boe attive e dei sonar dei siluri: non trovati (vedi anche dossier 17 §12).
8. Testi non letti: *Blind Man's Bluff*, *Hunter Killers*, Clancy *Submarine*, Urick 1983, Polmar 1981 su Proceedings.
   Da leggere su copia cartacea.
9. Watkins (sito in manutenzione) e i termini attuali della BBC: letti solo da copie d'archivio.
10. Coefficiente della formula di Cole per l'onda d'urto e soglia ISO 226: citati a memoria, da verificare.

---

## 8. Proposta a pezzi (da far scegliere all'utente)

Tutto vale **solo con la regola di simulazione**; il classico tiene i 6 campioni a 4 bit dell'originale.

| Pezzo | Contenuto | Dipende da | Note |
|---|---|---|---|
| **R1 Uscita audio** | SDL3 audio streams, mixer a blocchi in `nucleo` (deterministico) con i canali cuffia, bordo e interfaccia, volumi; le tre rese (originale, Sound Blaster, piena); nell'`app` l'adattatore | — | nessuna dipendenza nuova: SDL3 c'è già e apre l'uscita (§5.1) |
| **R2 Cuffia in tempo reale** | la sintesi di `nucleo/ascolto` a blocchi continui: fasi delle righe continue, sovrapposizione e somma del rumore, parametri levigati; il fascio puntato e i filtri del pannello sonar | R1 | la stessa fisica di cascata e LOFAR |
| **R3 Ambiente di bordo** | il percorso attraverso lo scafo K(f) per stazione, il fondo per regime, gli eventi del §6.5 | R1, R2 | i parametri di K(f) e del fondo in un file dati, con le fonti di questo dossier |
| **R4 Libreria di campioni** | strumento della pipeline: scarico da un elenco curato (solo CC0, pubblico dominio e CC BY), manifesto delle licenze e file `CREDITS`, analisi a monte (inviluppo, righe, ritmo, DEMON) nei metadati di `sorgenti_libreria.py`, conversione in un formato unico | — | da decidere: se i file si distribuiscono col gioco o si scaricano al primo avvio |
| **R5 Mescolamento** | tessitura sbiancata e ricolorata dal modello, Doppler per ricampionamento, perdita per banda, notch delle righe per le unità militari (§2.2) | R2, R4 | biologici, ambiente e navi civili prima |
| **R6 Variazioni nel modello acustico** | regime dei macchinari, aspetto, virate, danni, seme per unità (§3); anche cascata, LOFAR, scoperta e ACINT le vedono | — | cambia l'Enhanced: va confrontato su SCENE1A e SCENE4A come le altre modifiche acustiche |
| **R7 Transitori e armi** | lancio, portelli, piani, siluri (motore, eliche e pompe a getto, sibilo, testa attiva), esplosioni con le pulsazioni della bolla, scafi che si spezzano, allagamenti: sorgenti del modello come i transitori di A2 | R1 | per le esplosioni, i dati del dossier 10 sulla bolla |
| **R8 Aerei** | tessiture CC0 di rotori e turboelica dietro la finestra di Snell, solo al sonar e in superficie | R2, R4, R5 | `sorgente_aerea` esiste già |

**Scelte dell'utente (25/10/2026):**
1. licenze: solo CC0, pubblico dominio e CC BY;
2. i campioni si distribuiscono dentro il gioco, con i crediti raccolti da `tools/pipeline/crediti.py` (kit);
3. ordine: R1 uscita audio, R2 cuffia del sonar in tempo reale, R3 ambiente di bordo per stazione;
4. aerei: a bordo niente, solo al sonar.

**Stato (25/10/2026):** R1, R2 e R3 fatti in `nucleo` e `piattaforma` (engine/README.md, «Audio: mixer, cuffia del
sonar e bordo»); l'aggancio a main.cpp e alle stazioni e' della sessione principale. Nel bordo le stime di questo
dossier sono parametri del codice: K(f) e il riverbero di 4 dB, il fondo delle stazioni (la barca silenziosa a 38-40
dBA), il ticchettio dei ping 10 dB sotto il livello interno, il ping proprio 120-138 dB sotto la sorgente secondo la
stazione.

**R6 (25/10/2026):** fatto in `nucleo` («Variazioni nel modello acustico» in engine/README.md), coi parametri della
proposta del §3. Due scelte non scritte sopra:
- la pompa veloce sta 40 log r sopra la lenta, cioè +12 dB per r = 2: le firme davano +1 dB;
- il confronto per rapporti dell'ACINT accetta le righe dei giri entro il 3,5%.

Lo accende la missione (`UnitaAcustica::variazioni`); le differenze sulle missioni di prova si guardano prima di
chiudere.

**R7 (25/10/2026):** fatto in `nucleo` e nel catalogo («Transitori delle armi e degli scafi» in engine/README.md), coi
numeri di `18_note/transitori_armi_r7.md`. Le sequenze le fa partire la missione dai suoi eventi.

**R4 (25/10/2026):** 27 campioni federali scaricati col permesso dell'utente (`tools/pipeline/campioni.py`, elenco in
`18_note/campioni_r4.md`). I tre di licenza da verificare (capodoglio, foca barbata, fessurazione del ghiaccio) sono
esclusi dopo la verifica: il pubblico dominio non si dimostra (motivi e pagine in `18_note/campioni_r4.md`). L'analisi a monte è in
`tools/pipeline/campioni_analisi.py`, che scrive `data/suoni/campioni/` e `data/motore/campioni.toml`.

**R5 (25/10/2026):** fatto in `nucleo`, col procedimento del §2.2 («Libreria di campioni e mescolamento» in
engine/README.md). Differenze dalla proposta:
- lo sbiancamento e il taglio delle righe si fanno a monte, sull'intero campione (FFT unica), e non a run time;
- il gioco distribuisce solo la tessitura sbiancata;
- per le navi la tessitura si ricampiona anche per far tornare il suo battito con quello del modello;
- l'attività nel tempo delle sorgenti della libreria segue l'inviluppo del campione (`collega_campioni`): non era nella
  proposta, ma serve alla coerenza nel tempo.

Resta fuori il pettine dello specchio di Lloyd. La pioggia e il vento del rumore ambiente suonano con le loro
tessiture dal 25/10/2026.

**R8 (25/10/2026):** fatto.
- L'utente ha scelto tutti i 14 file di `18_note/campioni_r8.md` e il decodificatore PyAV.
- Allo spettrogramma 4 tratti erano inutilizzabili (voci, musica, saturazione) e sono esclusi nel manifesto: restano 10
  registrazioni.
- Alla verifica delle licenze anche il Ka-27PS di mil.ru (U13) è escluso: ne restano 9.
- L'S-3, il Ka-27, il Ka-25 e l'hover con gli spruzzi restano col modello.

**Le domande che erano poste all'utente:**
1. **La politica delle licenze:** solo CC0, pubblico dominio e CC BY (consigliata), oppure anche CC BY-NC e i permessi
   scritti (Watkins, AWM, ShipsEar).
2. **Distribuire i campioni col gioco o farli scaricare** dalla pipeline al primo avvio, che evita di ridistribuire i
   file grezzi.
3. **L'ordine dei pezzi.** Consigliato R1 → R2 → R3 per avere subito cuffia e bordo dal modello, poi R6 e R7; R4, R5 e R8
   quando la libreria è pronta.
4. **Il rimbombo degli aerei a bordo:** realismo (niente) o atmosfera (sotto 40 dB, solo a macchine ferme).

---

## 9. Fonti

**Archivi e licenze:**
- NOAA PMEL Acoustics, pagine sounds e multimedia: https://www.pmel.noaa.gov/acoustics/multimedia.html
- NRS (README): https://storage.googleapis.com/noaa-passive-bioacoustic/nrs/README.pdf
- NCEI: https://www.ncei.noaa.gov/products/passive-acoustic-data
- 17 U.S.C. §105: https://www.law.cornell.edu/uscode/text/17/105
- MBARI: https://registry.opendata.aws/pacific-sound/ e https://www.mbari.org/data/passive-acoustic-data/
- ONC: https://www.oceannetworks.ca/data/data-policy/
- OOI: https://oceanobservatories.org/how-to-use-acknowledge-and-cite-data/
- Freesound: https://freesound.org/help/faq/
- Orcasound: https://registry.opendata.aws/orcasound/
- Watkins (copia Wayback): http://web.archive.org/web/20250713040020/https://whoicf2.whoi.edu/science/B/whalesounds/about.cfm
- DOSITS: https://dosits.org/dosits-disclaimer/
- ShipsEar: https://underwaternoise.atlanttic.uvigo.es/
- DeepShip: https://github.com/irfankamboh/DeepShip
- AWM S04697: https://www.awm.gov.au/collection/C1240481
- BBC RemArc 2016: https://github.com/bbcarchdev/Remarc/blob/master/doc/2016.09.27_RemArc_Content_licence_Terms_of_Use_final.pdf
- craigsmith, «Vintage Submarine Effects»: https://freesound.org/people/craigsmith/packs/24916/

**Fattori di variazione:**
- [Ross76] D. Ross, *Mechanics of Underwater Noise*, Pergamon 1976 (DTIC ADA075101: https://archive.org/details/DTIC_ADA075101).
- [Spo88] N. Sponagle, *Variability of Ship Noise Measurements*, DREA TM 88/210 (https://archive.org/details/DTIC_ADA201248).
- [Shin91] Shin, Robinson, Rossano, NPS-ME-91-002 (https://archive.org/details/DTIC_ADA242189).
- [Mia98] E. Miasnikov, appendice 1 in *The Future of Russia's Strategic Nuclear Forces*, 1998 (https://spp.fas.org/eprint/snf03221.htm).
- [MIL1399] MIL-STD-1399 sez. 300B (2008).
- [Mit02] R. B. Mitson, ICES CM 2002/J:10.
- [Tre08] Trevorrow, Vasiliev, Vagle, *JASA* 124(2) 767-778 (2008).
- [AV00] Arveson, Vendittis, *JASA* 107(1) 118-129 (2000).
- [McK12] McKenna et al., *JASA* 131(1) 92-103 (2012).
- [Gas17] Gassmann, Wiggins, Hildebrand, *JASA* 142(3) 1563-1574 (2017).
- [SH91] Scrimger, Heitmeyer, *JASA* 89(2) 691-699 (1991).
- [Zob23] ZoBell et al., *PLOS ONE* 18 e0282677 (2023).
- [How11] J. Howard, *The Submarine Review*, aprile 2011.
- [GS-S5G] GlobalSecurity, S5G.
- [TWZ-Narwhal] The War Zone, 2020.
- [Xen] Xenthos, *Radiated Noise Ranging for Submarines*.
- [Kra88] Kratovil, tesi NPS, 1988.
- [BDN10] Bangor Daily News, 3/10/2010.

**Ascolto storico:**
- [Mac74] R. R. Mackie, ONR 1974 (https://archive.org/details/DTIC_ADA002307).
- [NAV76] NAVEDTRA 10130-C, *Introduction to Sonar*, 1976 (https://archive.org/details/DTIC_ADA091009).
- [Kin81] Kinney et al., NSMRL 955 (https://archive.org/details/DTIC_ADA100767).
- [Kin83] Kinney et al., NSMRL 994 (https://archive.org/details/DTIC_ADA125059).
- [GAO92] GAO/NSIAD-92-50 (https://archive.org/details/DTIC_ADA248051).
- [Cot00] O. R. Cote, *The Third Battle* (https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm).
- [Fore] Forecast International, BQQ-5(V).
- [GSec] GlobalSecurity, AN/BQQ-5.
- [Lur88] Luria, DiVita, Neri, Proceedings, febbraio 1988.
- [Bar01] Barron, *Submarine Review* 2001.
- [TWZ20] A. Amick, The War Zone, 2020.
- [Che24] D. Chetlain, The War Horse, 2024.
- [SSEP] FAS SSEP.
- [RN] Royal Navy, corsi di analisi aurale.
- [Kaz19] Kazakov, *Nauka, tekhnika i obrazovanie*, 2019 (https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody).

**Bordo e aerei:**
- [Tam09] Schultz, Shell, *Naval History* 23(6), 2009 (USS Tambor).
- [Seal13] R. Smith, Warfare History Network, 2013 (USS Seal).
- [Gun07] NSL, *USS Gunnel First War Patrol*, 2007.
- [Har71] J. D. Harris, NSMC Report 682, 1971 (DTIC ADA349945).
- [Cla92] S. J. Clampton, INCE «Fan Noise», 1992 (https://www.aivc.org/sites/default/files/airbase_10228.pdf).
- [Lei12] T. G. Leighton, *JASA* 131(3) 2539-2542 (2012).
- [Mui21] T. G. Muir, *Acoustics Today* 17(1), 2021.
- [Fer19] B. G. Ferguson, *Acoustics Today* 15(1), 2019.
- [AT19] *Acoustics Today*, primavera 2019, p. 14.
- [FS09] Ferguson, Speechley, *IEEE JOE* 34 75-82 (2009).
- [Ell00] Eller, Cavanagh, AFRL-HE-WP-TR-2000-0156 (DTIC ADA395453).
- [Kue20] Kuehne et al., *J. Mar. Sci. Eng.* 8, 923 (2020).
- [Ste60] G. P. Steele, Proceedings, novembre 1960.
- [WHOI-shrimp] WHOI Oceanus, gamberi pistola.

**Audio del gioco:**
- [miniaudio] https://github.com/mackron/miniaudio.
- [WP-SB] Wikipedia, *Sound Blaster*.
- [DOSDays] https://dosdays.co.uk/topics/sb_sbpro.php.
- [MobyG] scheda tecnica del gioco (https://pixelatedarcade.com/games/688-attack-sub/techspecs).
- `docs/01_analisi_tecnica_originale.md` §2.6 e `docs/originale/suoni.md`.
