# E9. Grafica

*Autore: kit. Revisione del 29/10/2026 (gruppo 4: museo, statistiche globali, albo, editor). Stato: rivisto
(curatore: kit).*

## 1. A che serve

Spiega come è fatta l'immagine dell'Enhanced: la tela VGA a 256 colori, la palette divisa in fasce, gli sprite delle
navi generati da modelli 3D con le foto d'epoca, le navi danneggiate disegnate nell'assetto che la fisica calcola, il
mare, le esplosioni, le consolle e i caratteri delle stazioni, la carta, la rifinitura estetica dei menu e delle
cascate del sonar. La regola è una sola: l'aspetto del 1989, con quello che si vede che viene dalla simulazione.

## 2. Il modello

### 2.1 La tela e la palette

- **Tela**: 640×480 a 256 colori indicizzati, pixel quadrati (lo stile del modo VESA 101h), per tutte le schermate.
  Sullo schermo si ingrandisce a multipli interi, al pixel più vicino (`piattaforma/src/finestra.cpp`): è
  l'uscita predefinita, e il classico resta così. Dal 31/10/2026 (decisione del 22/10/2026) ci sono anche
  (`Finestra::imposta_uscita`):
  - **sharp bilinear**: la tela si ingrandisce al pixel più vicino al multiplo intero che copre la finestra (fino a
    8), poi si porta linearmente alla finestra intera in 4:3. I pixel restano nitidi, senza bande nere e senza i
    pixel di larghezza diversa del ridimensionamento al più vicino;
  - **CRT facoltativo**, con l'una o l'altra uscita: per ogni riga della tela tre sottorighe, l'ultima scurita al
    66% (la riga di scansione), e per ogni colonna tre sottocolonne appena tinte di rosso, verde e blu (la
    maschera, 89%). Si moltiplica sopra l'immagine. Niente curvatura.

  Si cambia con **F10** (comando `uscita_schermo`), nel menu e in partita: nitida → sharp bilinear → sharp bilinear
  col CRT → nitida, con un avviso di due secondi in alto a sinistra. Lo gestisce la finestra
  (`Finestra::eventi`), e la scelta si salva in `~/.config/melchior688/preferenze.toml`: `[schermo]`,
  `uscita = "nitida"` o `"sharp_bilinear"`, `crt = true`. Da riga di comando `--uscita nitida|sharp`, `--crt` e
  `--senza-crt` valgono per quella partita.
- **Palette per scena**, con gli effetti di palette dell'originale (luce del giorno e della notte, luce d'emergenza).
  La luce della vista agisce solo sulle fasce della vista (`INDICI_VISTA` in `grafica/periscopio.hpp`: 16-79, 96-143,
  144-158, 196-198, 199-247); il resto è interfaccia e non cambia (capitolo E8). Il tema delle stazioni si applica dopo
  la luce, così di notte lampade e fosfori restano accesi.

| Indici | Uso |
|---|---|
| 0-15 | colori EGA dell'interfaccia |
| 16-47 | tema delle stazioni: smalto, accenti, lampade, targhette, fosforo; nelle schermate a sé anche ritratti, cerimonie, decorazioni, cartoline |
| 48-79 | rilievo della terra della carta (8 tinte × 4 luci) |
| 80-95 | interfaccia dell'originale |
| 96-143 | mare della vista; ridefinita dalla carta, dallo spaccato e dalla scala lunga del sonar (96-223) quando coprono la vista |
| 144-158 | grigi degli sprite (anche foschia, nubi, pioggia, neve); 159 trasparente |
| 160-174 | rampa monocroma dell'oculare: termico, intensificatore, televisione; nel debriefing, che non ha la vista, 160-191 sono i 32 colori della minicartolina |
| 175-198 | fuoco, fumo acceso, fango, fuliggine; alcuni anche nelle sagome delle armi |
| 199-247 | cielo della vista; ridefinita da carta (199-231), cascate del sonar e del Museo (200-231 la scala corta, fino a 223 la lunga), spaccato |
| 248-253 | liberi; 254 verde scuro delle sagome; 255 bianco |

Chi aggiunge una rampa ne prende una libera e la registra in `grafica/palette.hpp`.

### 2.2 Gli asset dell'originale

Le immagini a 320×200 dell'originale (centrali, interfaccia, titolo, schermate finali, ritratti) si portano a 640×480
senza colori nuovi: raddoppio (Scale2x/EPX per i disegni, nitido per le foto a tutto campo, dove Scale2x fa artefatti
«a vermicello» sulla grana) e stiramento verticale 5:6 al pixel più vicino, per riavere l'aspetto 4:3 del monitor
(`grafica/riscala`, `porta_a_640x480`). Nessun upscaler a rete neurale: inventa dettagli fuori stile e fuori palette.

### 2.3 Il kit delle navi

**Le navi intatte vicine dal modello** (Enhanced coi periscopi veri, dal 31/10/2026). Da 0,5 pixel per metro in su
lo sprite di una nave intatta non viene dagli sprite precalcolati ogni 5°: si genera dal modello a passi di 1°
(`Periscopio::sprite_intatto`), alla scala della vista a quarti d'ottava, con 2 × 2 campioni per pixel invece di
3 × 3. Costa circa un quarto, e a vista non si distingue. Nel gioco in finestra lo genera un filo di lavoro, uno alla
volta per tutta la vista: finché non è pronto resta lo sprite di prima, o quello precalcolato. Nelle istantanee e
nelle prove si genera subito, così restano ripetibili (`usa_filo_di_lavoro`). I tempi misurati in Release, per uno
sprite a 3,75 pixel per metro con 3 × 3 campioni: Nimitz 243 ms, Kirov 126, Perry 49, petroliera costiera 33,
peschereccio costiero 7; a 1 pixel per metro Nimitz 20 e Perry 4,5 (misura in `test/prove_lamiera.cpp`, da lanciare
con `--no-skip`).

Il motore ha 71 classi (`data/motore/classi.toml`: navi da guerra, civili, sottomarini in superficie, aerei,
elicotteri, missili), ognuna col suo insieme di sprite (71), e 55 modelli per il motore (navi e sottomarini). Dal
30/10/2026 anche le classi civili del traffico hanno sprite, modello e scafo loro: modello proprio per rompighiaccio,
traghetto, rimorchiatore e portarinfuse, e dal 31/10/2026 per la nave oceanografica (tipo Johan Hjort), il BMRT,
il peschereccio costiero, il cianciolo e la petroliera costiera (tipo Pegasus, da una foto vista a ~38° a proravia
del traverso: fotocamera e quote stimate insieme). Prima le civili senza foto erano la classe più vicina riscalata
alle misure vere (`tools/navi/riscala_classe.py`, scelte in `tools/pipeline/civili.py`). Sempre dal 30/10/2026 le missioni S16-S18
hanno le classi vere al posto dei sostituti, ognuna col modello proprio:
- la fregata norvegese Oslo e la motocannoniera Hauk;
- la portaerei Kitty Hawk, variante della Nimitz con l'isola e l'albero dove li mette la sua foto;
- la corvetta svedese Stockholm, il sottomarino Näcken (calibrato su un disegno di profilo) e l'elicottero Hkp 4.

Nel gioco il nome della Näcken è «Nacken»: i font hanno solo le accentate italiane. Nelle foto delle navi civili nomi, numeri e
marchi veri si coprono (`cancella` nel file della classe). Ogni classe del kit ha un **modello 3D** assemblato da parti parametriche in scala esatta
(`tools/navi/kit.py`: scafo, tughe, alberi a traliccio, antenne radar, lanciatori, cannoni, radome, fumaioli,
imbarcazioni), descritto in un file YAML per classe (`tools/navi/classi/`).
Le **foto d'epoca** a licenza libera, con la fotocamera calibrata (foro stenopeico, punti di riferimento, affinamento
sulla sagoma), si proiettano sul modello. La maschera nave/sfondo di una foto si ricava dai toni (cielo, mare), si
traccia a mano (`tools/navi/maschera_manuale.py`) o, quando lo sfondo è confuso (rimorchiatori dietro, scafo bagnato
chiaro come il mare), è la sagoma del modello calibrato sui soli punti, erosa di qualche pixel
(`tools/navi/contorno_modello.py --maschera`): allora la coincidenza della sagoma non misura il modello. Gli scarti dei
punti di calibrazione correggono le quote del modello (per esempio la tuga poppiera della Ticonderoga, alta 20 m, e la
vela dell'Ohio, a 10 m) e, se la foto è molto di scorcio, anche le posizioni lungo lo scafo (la vela del Type 206 l'ha
fissata così, d'accordo fra due foto). Le foto si scaricano con `tools/navi/scarica_foto.py`, che accetta solo pubblico
dominio, CC0, CC BY e CC BY-SA, controllando la licenza foto per foto, e scrive i crediti in `foto/crediti.json`. Quei
crediti finiscono nei crediti del gioco (`tools/pipeline/crediti.py`). I punti si leggono sul ritaglio con la griglia
nelle coordinate dell'originale (`tools/navi/griglia_foto.py`). Il risultato:

- **sprite** a 16 grigi della palette originale, ogni 5° di angolo sulla prua e da tutti e due i lati (con le
  elevazioni per gli aerei), con le ancore (galleggiamento, centro);
- **modelli per il motore** (formato MDL2): triangoli, parti con nome (scafo, vela, alberi, timoni, eliche…),
  tessitura cotta dalle foto, opera viva compresa.

Coordinate del kit: x verso prua con l'origine a metà lunghezza, y a sinistra, z in su col galleggiamento a zero.

### 2.4 Le navi danneggiate

Una nave intatta si disegna con lo sprite precalcolato. Una nave danneggiata (capitolo E2) si disegna al volo dal suo
modello, con un rasterizzatore software che segue la stessa strada degli sprite precalcolati: camera da 4 km,
sovracampionamento 3×3, 16 grigi con la retinatura di Bayer in fase con gli sprite (`grafica/modello`,
`genera_sprite`). A nave intatta lo sprite generato coincide con quello precalcolato: sulle 26 classi controllate dal
kit lo scarto medio è di 0,05-0,31 livelli di grigio (`docs/06_kit_navi.md`); la prova sulla Perry chiede sagome
uguali al 99,5%, scarto medio sotto 0,35 livelli e il 96% dei pixel entro un livello.

La **posa** viene dalla fisica: sbandamento, assetto, immersione, e i tronconi spezzati a una x data; la parte sotto il
mare non si vede. Lo sprite si rigenera solo quando cambiano vista, posa, scala o segni (`Periscopio::sprite_generato`,
con le soglie: mezzo grado d'angolo, 5% di scala). Sopra: falle nere, fuliggine sui
locali anneriti, alberi che cadono, la nave sollevata e inarcata dalla bolla.

### 2.5 La vista del periscopio

- **Campo**: quello del modello del periscopio (capitolo C9), per esempio 32° a 1,5× e 8° a 6×; il cerchio è largo 300
  pixel (`oculare::RAGGIO` = 150), lo schermo della televisione 400×300.
- **Orizzonte** geometrico con la rifrazione standard (raggio terrestre efficace 7/6,
  `RAGGIO_TERRA_RIFRAZIONE` in `nucleo/geometria.hpp`): a 1,5 m d'occhio è a circa 4,7 km.
- **Navi in scala** per distanza e ingrandimento; il galleggiamento scende sotto l'orizzonte per la quota dell'occhio e
  la curvatura: a 17 km si vedono solo gli alberi.
- **Mare**: non più piatto. Quattro onde lunghe attorno al picco di Pierson-Moskowitz e quattro corte del vento, ferme
  nel mondo e in moto alla loro velocità di fase; il chiaro e lo scuro dai riflessi del cielo; la schiuma delle creste
  secondo Monahan (`W = 3,84·10⁻⁶ U^3,41`, da forza 3); le strisce di schiuma lungo il vento da forza 7 e la
  spruzzaglia da forza 8 (**STIMA**, dossier 21 §9); la foschia verso l'orizzonte (Koschmieder). Il periodo di picco è
  quello della simulazione (`nucleo/onde.hpp`, `periodo_picco_s`); il mare vicino copre col suo profilo navi e
  particelle dietro le creste (`oculare::mare`, `ProfiloMare`).
- **Il mare sulla finestra**: l'onda della simulazione (`nucleo/onde.hpp`) nel punto della testa sale sul vetro, lascia
  il velo e le gocce (piccole lenti che capovolgono la scena), gli spruzzi da mare 4-5 (Hs oltre 1,25 m), la pioggia.
- **La testa colpita** (artiglieria, capitolo E4): l'immagine che si spegne nel rumore, il vetro incrinato con l'acqua
  che sale, l'acqua delle colonne vicine sul vetro (`oculare::guasto`).
- **La telecamera verticale** dell'Alfa: guarda in su, verso la superficie mossa o il ghiaccio, con la grana e le righe
  della televisione.
- **Reticolo** per scuola: croce americana, griglia sovietica col centro vuoto, pali britannici; vignettatura retinata.
- **Stadimetro**: l'immagine fantasma sdoppiata, a scacchiera.
- **Filtri e modi**: polaroid, rosso, verde, giallo, affumicato; bassa luminosità verde, intensificatore sovietico
  verde giallino, televisione in bianco e nero con le righe, termico.
- **Meteo**: nubi in prospettiva, cortine di pioggia, nebbia, fumo di mare, pioggia e neve, lampi e fulmini.
- **Moto delle navi** (`grafica/moto_navi`): rollio e beccheggio per taglia e mare, tarati sul DD-963 (4,56° di rollio
  RMS con mare 5), al periodo d'incontro; la prua che si pianta e solleva lo spruzzo da mare 5.

### 2.6 Gli effetti

`grafica/effetti.hpp`: particelle nel mondo 3D, ordinate per distanza insieme alle navi. Esplosioni subacquee e
nell'aria (capitolo E1), fiamme e fumo, vapore, spruzzi, schiuma, resti; elicotteri in hover con l'anello di spruzzi,
il cavo del sonar e il MAD; boe col paracadute e il colorante; siluri col paracadute; marcatori; capsule e booster dei
missili; piume dei periscopi; emersioni violente; colonne dei colpi d'artiglieria e illuminanti. Il seme varia ogni
effetto: due esplosioni uguali non si somigliano mai del tutto.

### 2.7 Le stazioni

Dal dossier 19 sulle consolle vere del 688 e dell'Alfa (`grafica/sale`, `grafica/consolle`, `grafica/tema_stazioni`):

- **temi di colore** per battello negli indici 16-47 (smalto, accenti, lampade, targhette, fosfori); la luce della sala
  (bianca, rossa, blu) tinge le superfici, mentre lampade e fosfori restano accesi;
- **strumenti**: pannelli, targhette, quadranti, barre, lampade, finestrelle d'allarme, pulsanti illuminati e coperti,
  manopole, levette, tamburi, sette segmenti, schermi sotto la cappa, cloche;
- **sale**: le centrali del 688 (pedana dei periscopi) e dell'Alfa (ferro di cavallo), i pannelli di governo, armi,
  radio ed ESM, Ritm, Okean, Sozh, la sala sonar in luce blu, la cornice del periscopio;
- **caratteri** disegnati da zero (`tools/pipeline/font_stazioni.py`, formato FNT2): «Fosforo 688» 5×7 e 7×9 con
  l'alone, «Incisione 688» alla Gorton, «Stampino URSS» nelle larghezze del GOST, cifre a sette segmenti. Il font
  dell'originale (FNT1) resta per le scritte del 1989.

### 2.8 La carta, lo spaccato, le cerimonie

- **Carta VGA** (`grafica/carta_vga`): capitolo C12. Il fondale a fasce retinate con l'ombreggiatura, la terra in
  tinte di quota (48-79), le isobate coi numeri. Sopra, negli stili VGA e nautico, la mobilia (cornice coi minuti, rosa,
  scala, cartiglio, scandagli) e i toponimi dei teatri senza sovrapposizioni; nell'Enhanced gli strati meteo (barbe del
  vento, stato del mare, celle temporalesche, ghiaccio, nebbia: `grafica/strati_meteo`), spenti nel classico.
- **Spaccato del battello** (`grafica/spaccato`): il profilo vero dal modello 3D, i compartimenti da
  `data/motore/scafi/<classe>.toml`, gli interni e lo stato dei danni (capitolo B3), in 15 rampe sulle fasce della
  vista. Gli interni sono disegnati a mano per il 688 e l'Alfa; le altre classi hanno un allestimento generico.
  Una cassa zavorra squarciata (`CassaBattello::squarciata`) ha lo squarcio nella lamiera in cima alla cassa, sulla
  prima colonna che passa davvero dalla cassa; in esaurimento l'aria ne esce in bolle. Le didascalie che non entrano
  perdono le parole in coda; se resterebbe il solo numero del compartimento, l'ultima parola si accorcia col punto
  («IV REA.» sull'Alfa).
- **Cerimonie e onori** (`grafica/cerimonia`, `ritratto`, `decorazioni`, `giornale`, `cornici`, `medaglioni`): il
  comandante in uniforme, le medaglie e i nastrini, l'equipaggio schierato, i giornali, i medaglioni a rilievo dei
  riconoscimenti. I medaglioni sono 68: ai 41 si aggiungono i 14 di `docs/26` (29/10/2026) e i 13 del secondo giro (31/10/2026).
Questi ultimi sono la vela nella polynya, il tetto di ghiaccio con le chiglie, la nave coi fanali e quella con la luce
sbarrata, il salvagente (sobrio, per il soccorso), il battello nel cono di poppa dell'altro, i due battelli affiancati
con la quota, l'occhio con la mina, la rotta attorno alla mina, l'elicottero sopra il battello, le navi con le schiere,
le tre sagome del catalogo e il libro aperto con la stella. Fra questi il libretto
  aperto, l'immagine sdoppiata dello stadimetro, l'angolo sulla prora, l'occhio, il missile oltre l'orizzonte, la
  scatola col neutrale, l'allarme siluro, le mine, la tempesta, le colonne d'acqua dei colpi, il battello sul fondo,
  le rette del punto nave, la busta sbarrata e la riga nuova dello spettro. La tavola è
  `docs/ricerca/25_bozze/medaglioni/medaglioni_68.png` (`anteprima_cerimonia <dati> medaglioni`).
- **Minicartoline** del debriefing: la fotografia del mondo all'evento, o le sole pose se non c'è, ridisegnata in un
  riquadro con i suoi colori negli indici 160-191 (16-47 sono della lamiera della consolle); per ciò che non si vede,
  un'icona; per un affondamento, una scena composta col modello della classe (capitolo F1 §2.5).
- **Sagome delle armi** (`grafica/sagome_armi`): ogni arma del carico in scala, nella Sala siluri, nel tubo della
  stazione Armi, nello spaccato. Con `sagoma_da_id` le hanno anche i siluri che non sono nel carico (Tigerfish,
  Spearfish, Mk 46, AT-1M, AT-2M, UMGT-1, APR-2) e le classi delle missioni del 1989 senza sprite: mk48, type53, type40 (il SET-40),
  sea_lance e il contatto biologico, disegnato come una balenottera comune di 21 m. Misure e fonti stanno
  nell'intestazione del codice. La forma e i colori dei siluri sovietici e britannici sono in gran parte STIMA:
  le fonti aperte non li descrivono e foto libere ce ne sono poche.
- **Simboli delle viste dall'alto** (`grafica/simboli_carta`): un modulo solo per tutte le carte dell'Enhanced
  (stazione, briefing, debriefing, replay, vista dal vivo, editor). La parte la dicono il colore NTDS e la forma APP-6:
  - amico: azzurro, cerchio;
  - ostile: rosa, rombo;
  - ignoto: giallo, ottagono;
  - neutrale: verde, quadrato.

  Il dominio lo dice il telaio: la metà inferiore per i subacquei, la superiore per gli aerei, le pale per gli
  elicotteri, la base per la terra. Ci sono anche:
  - il battello del giocatore, il quadratino verde, con la scia e l'ellisse al 95% nel suo verde;
  - le cose in acqua: siluro, missile, esca, mina, boa e biologico;
  - la X della distrutta, le emissioni (ping, radar, radio) e la croce della soluzione TMA;
  - la legenda coi simboli veri.

  Ogni simbolo e ogni scritta ha di serie un alone nero di un pixel, così risalta sul mare scuro, sul mare chiaro e
  sulla terra (il classico tiene il contorno a croce dell'originale, nero e rosso scuro per le X). Le lettere dei
  contatti della stazione tengono i colori del 1989, che dicono lo stato della classificazione e non la parte (volume
  1, §5.6). Sulla carta chiara la palette (`applica_palette_carta`) dà ai posti NTDS le varianti scure di
  MIL-STD-2525C. Lo usano la carta (`app/carta.cpp`: lettere e X, segni del lucido, cadute, battello,
  intelligence, ellisse di navigazione), quindi anche il briefing e il debriefing che la disegnano, e il replay, la
  vista dal vivo e l'editor (`app/replay.cpp`, `app/editor_missione.cpp`), la stazione Missili e la pagina
  d'intercettazione del sonar (il `Pennello` con l'alone), con `dominio_da_categoria`,
  `oggetto_da_classe` e `indice_parte_eta` per passare dai dati al simbolo. Anche la croce TMA della stazione
  (`interfaccia.cpp`, carta restyle) e la carta delle tracce del debriefing (`debriefing.cpp`): unità, scie
  attenuate, armi orientate lungo la traccia, boe e battello in un `Pennello` solo, quindi con l'alone anche nel
  classico, perché la schermata è nuova.
- **Immagini del Museo**: sprite, modelli, sagome, oculari, cascate della firma, carte d'epoca, spaccati, foto «alla
  1989» (pochi livelli di grigio, retinatura) da originali a licenza libera.

### 2.9 La rifinitura estetica (roadmap 33)

L'utente ha scelto la combinazione del dossier 25 §8.6 il 28/10/2026:
- **A «Lamiera di bordo»** per menu e regolazioni;
- **B** per i documenti (esito, cerimonie, fascicolo, bacheca);
- **C «Il quadro di comando»** per gli strumenti.

La passata vale per i menu, le schermate del remake e le stazioni dell'Enhanced: le schermate di gioco del classico
restano come l'originale. Le bozze, con le tavole prima e dopo, sono in `docs/ricerca/25_bozze/` (gruppo 1, cascate e
variante A, approvato il 28/10/2026). I pezzi sono del kit, il cablaggio nell'app è della principale. Al 29/10/2026:
- **cablati:** la scelta della missione e le regolazioni della sandbox nella variante A, col piede comune
  (`app/scelta_missione.cpp`, `app/regolazioni_sandbox.cpp`); le scale delle cascate e il fosforo nella stazione
  sonar, nel DEMON, nella storia lunga e nella pagina d'intercettazione (capitolo C5 §2.8); il replay nella variante
  C (`app/replay.cpp`: la consolle «SALA ANALISI - REPLAY», la carta grande, la legenda e la lettura nei riquadri, la
  fascia del tempo, il piede comune), ma non la vista dal vivo della sandbox, che tiene la disposizione di prima; le
  statistiche della sandbox nella variante C (`app/statistiche.cpp`: la consolle «SALA ANALISI - STATISTICHE DELLA
  SANDBOX», due schermi nella pagina 1, uno largo nelle altre, le linguette e il piede cliccabili); le serie nella
  variante C (`app/schermata_serie.cpp`: «SALA ANALISI - SERIE DELLA SANDBOX», le linguette, la tabella della pagina
  2 che scorre con la sintesi della pagina 1 fissa in fondo, la NATO in ciano chiaro e il Patto in rosso chiaro,
  indici 11 e 12); il briefing e il debriefing nella variante B (§2.9.4); il gruppo 4 (§2.9.5): museo, statistiche
  globali, albo ed editor;
- **pronti ma non cablati:** la vista dal vivo della sandbox; i ritocchi della bozza del sonar (§2.9.1);
- **ancora da fare:** gli altri documenti in B (cerimonie, fascicolo, bacheca, esito), col piede di cerimonia
  dell'albo; le stazioni dell'Enhanced.

#### 2.9.1 Le cascate del sonar

`grafica/scala_cascata`, namespace `cascate`:
- **le tinte** della stazione: fosforo P1, grigio, viridis, turbo, caldo;
- **la scala lunga** di 128 indici (96-223, `SCALA_LUNGA`) quando la stazione copre la vista, invece dei 32 della scala
  corta (200-231, `SCALA_CORTA`), che restano per il Museo e i riquadri. L'indice 0 della scala resta nero: sotto il
  pavimento il fondo è nero;
- **il dithering** ordinato Bayer 4×4 fra due gradini, fermo, che rende la frazione a 1/16;
- **il fosforo** del modo 1989 (`cascate::Fosforo`): un'intensità per pixel che decade, traccia senza scalini (Xiaolin
  Wu), bagliore dai vicini più accesi (35%), grana ferma, una riga su due all'85%. La persistenza ha tre gradi: breve
  0,80, normale 0,90, lunga 0,95 a fotogramma (`cascate::coefficiente`). Nella stazione la sceglie il campo PERS e
  vale per l'A-scan; la stazione aggiunge un velo di grana al 7% sotto il pavimento (`Effetti::fondo`), che di
  serie nel modulo è spento.

La mappa dB → frazione è lineare fra il pavimento e il tetto della stazione (`cascate::frazione`). Sulle frange
d'interferenza vere (C2 §2.7) i 32 livelli fanno curve di livello; i 128 col dithering le rendono lisce.

La stazione ha le parti ai posti del dossier 09 §4.3: BTR a sinistra; A-scan, LOFAR e DEMON a destra; tracker e
stato in basso.

**Cablata il 29/10/2026, la stazione Sonar in stile cascate SDR.** Le tre bozze sono diventate (scelta dell'utente):
A, il quadro SDR, nel moderno; B, la consolle BQQ-5 a due CRT, nel 1989; C, l'analizzatore, come pagina sul tasto 4
nei due modi. Il dettaglio è in C5 §2.8; le bozze e le istantanee del gioco (`gioco_*.png`) in
`docs/ricerca/25_bozze/sonar_sdr/` (§9 di `docs/ricerca/25_bozze/indice.md`; ricerca in
`docs/ricerca/30_sonar_grafica_sdr.md`). Le regole comuni:
- il pavimento a +2 dB, così il rumore cade sul nero;
- 16 livelli di fosforo nel 1989, viridis a 128 col dithering nel moderno;
- i tracker come segnalibri sopra la scala;
- il fascio puntato e i ciechi disegnati senza trasparenza (bordi a puntini e tratteggio);
- il band plan delle righe attese colorate per tipo;
- la media solo sull'A-scan.

Le bozze le disegna `strumenti/bozze_sonar_sdr`. Nel gioco non sono cablati i segni della carta sui tracker
(ottagono giallo, rombo rosa): i segnalibri hanno il colore del passo.

**In bozza (29/10/2026), l'Okean dell'Alfa con le stesse regole** (bozza D, §9.1 dell'indice delle bozze):
- Enisej come cascata polare, col tempo nel raggio e il pavimento a +2 dB;
- il cieco di poppa a tratteggio;
- la scala L/P sul bordo;
- il fascio a parentesi e i tracker sul bordo;
- lo schermo rettangolare sotto la cappa nera, con SPEKTR e DEMOD nello stampino;
- nel 1989 il fosforo verde giallino.

#### 2.9.2 La variante A

`grafica/lamiera`:
- lamiera con la grana, pannello incassato a viti, vetro nero, targa rivettata, testo inciso e secondario
  (`secondaria`: grigio chiaro con l'ombra nera, leggibile sul grigio medio);
- tasti a spia nei quattro stati (normale, evidenziato, premuto, disabilitato), più il predefinito col filo d'oro
  (la lampada ambra del tema); le spie delle caselle; i tasti della tastiera;
- il piede uguale dappertutto (`piede`): la riga d'aiuto e al più quattro tasti, alto 50 pixel (`ALTEZZA_PIEDE`).

Usa solo i colori fissi 0-15 e il tema delle stazioni (16-47), quindi la schermata tiene la carta (96-143, 199-231) e
il rilievo (48-79). L'ordine della palette è: il giorno, la carta se serve, il tema per ultimo.

#### 2.9.3 La variante C

`grafica/quadro`, per replay, statistiche, serie e sandbox:
- la consolle con la targa del posto («SALA ANALISI»);
- lo schermo d'analisi senza il cappuccio dei CRT delle stazioni, con la testata;
- le linguette delle pagine, le righe di valori e le intestazioni sul fosforo (indici 40-47 del tema);
- il riquadro sopra una carta (la legenda e la lettura del replay) e la linea del tempo come fascia sottile
  (`fascia_tempo`, scelta dell'utente del 28/10/2026 per il replay a carta grande), con le tacche a colori e ad altezze
  (`TaccaTempo`, capitolo F5 §2.1).

Il piede è quello di A.

#### 2.9.4 La variante B: briefing e debriefing

`grafica/lamiera`, i documenti del gruppo 3 (bozze in `docs/ricerca/25_bozze/documenti/`, scelte dell'utente del
28/10/2026). Usano solo gli indici 0-47:
- la testata classificata: il timbro della classifica a sinistra, il gruppo data-ora a destra (`testata_classificata`);
- il modulo continuo della telescrivente, bianco a fasce grigie ogni tre righe, coi fori di trascinamento (`modulo_continuo`), e il
  foglio bianco (`foglio`);
- il timbro a inchiostro, alto 15 pixel (`timbro`), le voci a puntini coi valori a penna (`voce_modulo`), le caselle
  (`casella`);
- le spie dei livelli (`spia_voce`) e i campi a vetro con l'etichetta incisa (`campo_vetro`); l'a capo a parole
  (`a_capo`).

**Il briefing** (`app/briefing.cpp`): a sinistra, nel vetro, la carta del teatro; sopra, gli ordini sul modulo
continuo, a pagine (frecce su e giù, «(SEGUE) n/m» o «(FINE)» in fondo), col timbro della classifica del messaggio
della serie 0; Tab li chiude. A destra le spie dei livelli 1-8: quelli che la missione non ha (oggi basi e radar
costieri, SOSUS, eventi speciali, e le zone, l'intelligence o le portate ASW quando mancano) sono spenti e scritti
scuri. Sotto, la scheda a campi a vetro: battello, teatro e radio; regole d'ingaggio; meteo e previsione a 12 ore;
codice della partita. Il campo che non entra si tace. Il piede ha tre tasti (Esc rifiuta, F7 sala siluri, Invio in
mare), anche col clic.

**Il debriefing** (`app/debriefing.cpp`): la carta nel vetro a sinistra e il foglio bianco a destra, con l'evento a
macchina, la minicartolina incollata e l'elenco, o il rapporto di missione col timbro dell'esito (capitolo F1 §2.5 e
§2.6). Il piede ha quattro tasti, gli altri comandi stanno nella riga d'aiuto.

#### 2.9.5 Il gruppo 4: museo, statistiche globali, albo ed editor

Bozze in `docs/ricerca/25_bozze/` (§5), scelte dell'utente del 29/10/2026. I pezzi nuovi:
- `lamiera::elenco_vetro` (A): l'elenco nel vetro, la voce scelta sulla lampada ambra accesa con la scritta nera, la
  tacca dello scorrimento a destra, lunga quanto la parte visibile;
- `lamiera::in_colori_vista` (A): rimappa i pixel del tema (16-47) di un rettangolo sul colore più vicino fra gli EGA
  0-15 e i grigi degli sprite 144-158. Le lampade vanno solo sugli EGA colorati, così restano rosse, ambra e verdi. La
  distanza pesa il verde 4, il rosso 3, il blu 2;
- `quadro::stato_vuoto` (C): un riquadro tratteggiato al centro dello schermo, col titolo acceso e le righe che dicono
  come si riempie;
- `posto_libero` e `piede_cerimonia` (B, `grafica/cornici`): il posto tratteggiato e numerato, e il piede di legno coi
  tasti come targhette d'ottone. Ha la geometria del piede comune (la riga d'aiuto, al più quattro tasti, il predefinito
  col filo d'oro); un tasto spento è il vano nel legno. Usano solo le rampe di cerimonia (inchiostro 40-47, bianco 48-52,
  oro 53-60, legno 207-214).

Le schermate:
- **Statistiche globali** (`app/statistiche.cpp`, variante C): la consolle «SALA ANALISI - STATISTICHE GLOBALI», due
  schermi. A sinistra i gruppi a righe alterne velate; a destra la carta col calore (232-247), la scala e le armi.
  Senza missioni i valori sono trattini spenti e la carta ha lo stato vuoto.
- **Albo** (`app/albo.cpp`, variante B): ritratti, scrivania e carta usano gli indici 16-47 e le rampe di cerimonia,
  quindi niente lamiera né piede comune: il piede è `piede_cerimonia`. I cartellini sono alti 182 pixel (erano 196),
  col timbro «IN CARICA» sulla fototessera; i posti liberi sono `posto_libero`.
- **Schermata dei comandi** (F11; `grafica/schermata_comandi`, dati e tasti in `app/schermata_comandi`): lamiera, una
  linguetta per scheda, il foglio coi consigli a penna rossa e i tasti come cappucci, a pagine. Il disegno non guarda la
  partita; prove in `test/prove_schermata_comandi.cpp`.
- **Museo** (`app/museo.cpp`, A coi documenti di B): sprite, modelli e foto della voce usano la palette della vista,
  che comprende 16-79. Si disegnano la consolle e la cornice, poi `in_colori_vista` le porta fuori dal tema, poi
  l'immagine; la palette della schermata non applica il tema. Le basi hanno una carta loro (`data/motore/basi/<base>.fdl`,
  `tools/pipeline/carte_basi.py`): 168 per 96 km attorno al punto, da ETOPO 2022 a 15", o a 60" dove i dati su disco sono
  solo quelli; niente errori d'epoca, e la simulazione non le legge. Lo schedario è `elenco_vetro`; la scheda tecnica è un
  `foglio` con le `voce_modulo` (fino a 8, etichette tagliate); il testo è un `foglio` a pagine.
- **Editor** (`app/editor_missione.cpp`, variante C): la consolle «SALA PIANI - EDITOR DI MISSIONE»; la carta nello
  schermo grande, `{11, 44, 382, 375}`; le schede nello schermo di destra, con le linguette su due righe, i campi per
  gruppi e la verifica con la spia (`spia_voce`) in fondo; il piede comune.

**Il piede della cerimonia** (scelta dell'utente del 29/10/2026, cablato). Fascicolo, bacheca ed esito hanno il piede
di legno e ottone dell'albo, `piede_cerimonia` a y = 428, al posto della riga d'aiuto e dei pulsanti grigi. I clic
sui tasti passano da `tasto_cerimonia(x, y, y_piede, n)`, nuovo in `grafica/cornici`. Quello che sta sopra finisce a
424.
- **Il fascicolo:** la carta `{32, 8, 576, 416}`. I tasti: ESC INDIETRO, P REPLAY (spento senza un replay),
  SINISTRA PRIMA, DESTRA DOPO (il predefinito); il numero della pagina nella riga d'aiuto.
- **La bacheca:** la scheda del medaglione sale di 14 px. I tasti: ESC INDIETRO, G COMANDANTE o GIOCATORE (spento ma
  visibile senza un comandante), TAB SCHEDA.
- **L'esito:**
  - i tasti: ESC DEBRIEFING (ESC INDIETRO dalla seconda pagina) e INVIO CONTINUA o FINE;
  - le pagine finiscono a 424: il rapporto, il giornale, i riconoscimenti a passo di 62 px, la motivazione;
  - la pagina e il suo nome stanno nella riga d'aiuto.

Le tavole sono in `docs/ricerca/25_bozze/cerimonia/` (§7 di `indice.md`).

**La carta F6 grande e pulita** (scelta dell'utente del 29/10/2026, la B, cablata in `app/carta.cpp`):
- **il retino** delle zone non rilevate a 12 px (`OpzioniCarta::passo_tratteggio`; era 6);
- **lo strato meteo:** una barba grigia (RETICOLO) ogni 192 px, senza la cifra del mare. Il vento e il mare che il
  battello misura stanno una volta sola nella legenda in alto a destra, coi colori del cartiglio; i toponimi la
  evitano;
- **la fascia in alto non c'è più** (`riserva_alto` 0): la carta prende anche quelle righe.
  - L'ingrandimento e la proiezione stanno nel pannello di sotto.
  - L'edizione e il tratteggio stanno nel cartiglio (`MobiliaCarta::zone_non_rilevate`: «HATCHED: UNSURVEYED»,
    «SHTRIKH.: NE OBSLEDOVANO»).
  - L'errore di posizione e il punto nave in corso, con la navigazione realistica, stanno sul margine bianco in basso,
    a destra.

Le tavole sono in `docs/ricerca/25_bozze/carta/` (§8 di `indice.md`); le disegna `strumenti/bozze_carta`.

#### 2.9.5b Il gruppo 5: radio, manovra, armi e pannello della carta

Bozze in `docs/ricerca/25_bozze/stazioni/` (§6 di `indice.md`), approvate dall'utente il 29/10/2026; si cabla prima il
688, l'Alfa dopo con le sue tavole. Le stazioni hanno due metà: sopra la vista (640×360), sotto il pannello delle
consolle (360-480). Ogni bozza ne ridisegna una:
- **Radio** (F2, sopra, A coi documenti di B): la fascia di stato con le antenne e le ROE come spie (rosse se
  trattenute, verdi se libere), la prossima finestra in un vetro con la barra del tempo, la sessione; il messaggio
  scelto sulla telescrivente (`modulo_continuo`) col timbro della classifica e «(SEGUE) 1/3»; a destra in onda e in
  programma nei vetri, i ricevuti in `elenco_vetro`.
- **Manovra** (F4, sopra, col periscopio giù, C): il ripetitore di manovra al posto della vista sott'acqua. A sinistra
  il profilo di quota, a destra l'ordinato contro l'attuale e l'acqua sotto la chiglia.
- **Armi** (F7, sopra, col periscopio giù, C): a sinistra le armi in acqua (corsa, filo, modo, stato, bersaglio) e
  l'arma scelta; a destra i tubi, il carico e la ricarica.
- **Carta** (F6, il pannello di sotto, A): pilota, proiezione e scie come spie; punti di rotta e scala nei vetri; il
  bersaglio nel suo vetro; i comandi come tasti della tastiera disegnati (`tasto_tastiera`), su due file.

Le tavole dell'Alfa (approvate) hanno le stesse disposizioni col tema dell'Alfa. Le scritte sulla lamiera sono scure
e piatte, come le targhette delle sue consolle: `lamiera::Scritte` (`chiare` o `scure`, `scritte_per(battello)`) è
l'ultimo argomento facoltativo di `principale`, `secondaria`, `piede`, `testata_classificata` e `campo_vetro`. Col
valore predefinito il 688 resta com'era.

Col periscopio su torna la vista. Il pezzo nuovo è `quadro::profilo_quota` (C, `ProfiloQuota`). I campioni della
quota e del fondo misurato vanno dal più vecchio al più recente a passo regolare; il fondo vale 0 dove non è
misurato. Disegna:
- la scala delle quote a sinistra, a multipli di 50 m sopra il punto più profondo;
- i minuti sotto;
- il fondo a retino solo dove è misurato;
- lo strato a tratti in ciano;
- la quota periscopica a tratti in verde;
- la traccia del battello.

Le misure per il cablaggio sono in `bozze_strumenti.cpp` (`radio`, `manovra`, `armi`, `carta_pannello`).

#### 2.9.6 Il libretto di riconoscimento

`grafica/libretto` (docs/25, scelte dell'utente del 29/10/2026), un documento della variante B sopra la vista del
Periscopio (capitolo C9 §2.16):
- **`sagoma`:** la classe dal modello 3D con `genera_sprite`, poi l'inchiostro: nero pieno, il retino di Bayer dove
  la luce batte (il grigio più chiaro dello sprite), il contorno sempre nero. Con `solo_vela` si tiene quello che sta
  sopra un terzo dell'altezza (almeno 3 m), ritagliato, con l'ancora sul taglio: la vela dei sottomarini poggia sulla
  linea;
- **`pagina`:** 210×340. La testata con la sezione e «n/37»; il nome (`originale.fnt`) e il sottotitolo; il profilo e
  le tre viste piccole sui galleggiamenti tratteggiati; dislocamento e lunghezza; l'albero nel riquadro nero con «CON
  IDENTIFICA VA AL TELEMETRO»; segni, armi, sensori, i radar in blu; il rimando al museo e la classifica in rosso. Il
  testo è in `fosforo688_5x7.fnt`, che ha trattino, virgola e punto e virgola (`incisione688_5` no). Restituisce
  falso se il testo non entra. Con un filtro attivo la testata diventa la striscia del filtro;
- **`striscia`:** la fascia nera del filtro, con la carta sopra: «FILTRO: PATTO / FREGATA / LANCIATORI» e a destra un
  testo dato («12 TAVOLE», «1/2»). «FILTRO:» resta fuori se non entra tutto; il resto si tronca coi puntini;
- **`scheda_filtro`:** la scheda del filtro aperta, sulla stessa carta e nello stesso rettangolo della pagina. La
  testata col titolo in `originale.fnt` e quante tavole passano (rosso se nessuna); la parte e il tipo fra le frecce,
  grigi su TUTTE; «COSA VEDO» con una casella per etichetta, piena a penna (blu 1) se segnata, e a destra quante tavole
  resterebbero segnandola, in grigio se nessuna. La riga del cursore è in negativo. Se le etichette non entrano
  l'elenco scorre col cursore: «ALTRE 5 SOPRA» al posto della spiegazione, «ALTRE 5 SOTTO» in fondo. Il piede con i
  tasti, in tre righe al più;
- **i colori:** solo gli EGA 0-15, così la pagina non tocca la palette della vista.

La tavola «luci» (`pagina_luci`, C9 §2.17) ha otto riquadri neri con gli schemi COLREG dalle configurazioni di
`luci.toml`: lo scafo e gli alberi in grigio, i fanali come punti di 3 pixel (bianco 15, rosso 12, verde 10, ambra 14,
col nucleo bianco), le luci di coperta come punti singoli grigi.

Le tavole grandi in stile ONI, una per classe, le fa `strumenti/libretto_tavole`, in `docs/ricerca/25_bozze/libretto/`.
Lo stesso strumento fa le bozze del filtro: `pagina_filtro.png` (le fregate del Patto coi lanciatori),
`pagina_filtro_scorre.png` (l'elenco che scorre) e `pagina_filtrata.png` (la pagina con la striscia).


#### 2.9.7 I caratteri che i font non hanno

I font del 1989 e quelli delle stazioni (`originale`, `fosforo688`, `incisione688`, `stampino_urss`) hanno l'ASCII,
le accentate italiane, il grado, il «per» e il paragrafo. Le altre lettere si scrivono con la loro riserva
(`Font::caratteri`, in `grafica/src/font.cpp`), che vale per ogni font:
- le lettere europee nella traslitterazione latina: Æ AE, Ø O, ß ss, Š S, Ł L…;
- la tipografia delle decorazioni e delle motivazioni (29/10/2026):
  - le virgolette basse e alte diventano virgolette dritte, gli apici diventano apostrofi;
  - gli ordinali ª e º diventano A e O, per esempio «DI 1A CLASSE»;
  - le lineette diventano trattini, i puntini di sospensione tre punti;
- i segni delle schede: ≥ e ≤ diventano «>=» e «<=», ± «+-», gli esponenti la cifra, € «EUR», il punto alto un punto.

Una lettera senza glifo e senza riserva esce «?».

Il titolo delle intestazioni di cerimonia (`intestazione`, `grafica/cornici`) è in scala 2. Se fra i due emblemi non ci
sta, come i nomi lunghi delle medaglie sovietiche, passa in scala 1, e solo come ultima risorsa si tronca.

## 3. Fonti e confidenza

| Elemento | Fonte | Confidenza |
|---|---|---|
| Dimensioni e forme delle navi | dossier 01, foto d'epoca, disegni pubblici | media-alta per classe (nel rapporto di ogni classe) |
| Consolle, colori, caratteri | dossier 19 (foto delle postazioni vere) | media |
| Orizzonte e rifrazione | ottica atmosferica standard | alta |
| Onde e schiuma | Pierson-Moskowitz, Monahan | alta (le formule) |
| Moto delle navi col mare | dossier 21 §7.2, tarato sul DD-963 | media |
| Strisce di schiuma e spruzzaglia per forza del vento | dossier 21 §9 | **STIMA** |
| Persistenza, bagliore e grana del fosforo | dossier 25 §9, bozze del gruppo 1 | **STIMA**, scelta a vista con l'utente |

## 4. Classico ed Enhanced

Il classico ha la grafica dell'originale portata a 640×480; l'Enhanced ha consolle, oculari, mare mosso, meteo, navi
danneggiate, effetti. Le stesse navi intatte hanno gli stessi sprite del kit in tutti e due. La rifinitura estetica
(§2.9) tocca i menu e le schermate del remake, che sono comuni, e le stazioni dell'Enhanced; le schermate di gioco del
classico non cambiano.

## 5. Come si vede in gioco

Ovunque: volume 1, tutti i capitoli. Le anteprime senza finestra sono negli strumenti di `engine/strumenti`
(`anteprima_stazioni`, `anteprima_periscopio`, `anteprima_meteo`, `anteprima_spaccato`, `anteprima_cerimonia`,
`anteprima_albo_onore`, `anteprima_sagome`, `anteprima_simboli`, `anteprima_carta`, `anteprima_enciclopedia`,
`anteprima_serie`), con le tavole in `extracted/` e in `tools/anteprime/`.
Le bozze della rifinitura estetica le fanno `bozze_estetica`, `bozze_sonar` (anche con le righe vere di
`tools/re/prove/cascate_esempio`) e `bozze_strumenti`, con `tools/estetica/bozze.py`, `bozze_strumenti.py` e
`prima_dopo.py`.
`anteprima_simboli <dati> <uscita.bmp> vga confronto` mette accanto la carta senza e con l'alone.

## 6. Taratura e prove

Le prove stanno in `engine/test/`:
- `prove_grafica.cpp`: «modello: sprite generato contro sprite precalcolato» (§2.4), «geometria del periscopio»;
- `prove_stazioni_grafica.cpp`: «font: l'originale (FNT1) si disegna come i suoi bit, con a acuta e paragrafo»,
  «temi delle stazioni: solo gli indici 16-47, luce della sala sulle superfici», «pannelli di governo e armi: nel
  rettangolo, aree coi comandi della mappa dei tasti»;
- `prove_vista_periscopio.cpp`: «palette: fosfori per modo e filtri solo sugli indici della vista», «mare mosso:
  schiuma di Monahan, solo sotto l'orizzonte e nel cerchio, piu' moto col mare grosso»;
- `prove_effetti.cpp`, `prove_effetti_asw.cpp`, `prove_artiglieria_grafica.cpp` (anche «oculare: la testa colpita
  (spenta, incrinata) e niente se intatta»), `prove_meteo_grafica.cpp`;
- `prove_carta_vga.cpp` (anche «carta VGA: toponimi dei teatri, a scala, senza sovrapposizioni ne' fuori dall'area»),
  `prove_carta_enhanced.cpp`, `prove_simboli_carta.cpp`, `prove_sagome_armi.cpp`, `prove_cerimonia.cpp`,
  `prove_museo_grafica.cpp`; la navigazione del Museo è a parte, in `prove_museo_navigazione.cpp` (bersaglio
  `prove_museo`): eventi veri di tastiera e mouse passano da `Finestra::eventi` fino al cambio di scheda. Lo stesso per le
  frecce delle schermate di contorno in `prove_frecce_schermate.cpp` (bersaglio `prove_frecce`): una pressione è un passo,
  breve o lunga che sia;
- la rifinitura estetica: `prove_scala_cascata.cpp` («scala delle cascate: il dithering Bayer rende la media della
  frazione», «scala delle cascate: il fosforo decade, traccia senza buchi e resta nel suo rettangolo»),
  `prove_lamiera.cpp` (anche «l'elenco nel vetro scorre, e nei colori della vista non resta niente del tema», «il piede
  di cerimonia e il posto libero usano solo le rampe di cerimonia»), `prove_quadro.cpp` (anche «lo stato vuoto sta nel
  suo rettangolo, col titolo acceso», «il profilo di quota sta nella sua area; la traccia, lo strato e la quota
  periscopica alle loro quote»), `prove_lamiera.cpp` (anche «le scritte scure sul beige dell'Alfa non usano il chiaro
  dello smalto; le chiare restano quelle di prima»).

## 7. Limiti noti e in arrivo

- Le navi intatte lontane (sotto 0,5 pixel per metro) e quelle del classico usano gli sprite precalcolati ogni 5°:
  fra un angolo e l'altro l'aspetto scatta, ma da lontano si vede poco. Le vicine dell'Enhanced vanno a passi di 1°
  (§2.3).
- Le appendici sotto il mare dei sottomarini in superficie si vedono solo nelle emersioni violente e negli affondamenti.
- Lo spaccato ha gli interni disegnati a mano solo per il 688 e l'Alfa; le altre classi hanno un allestimento
  generico.
- Il fosforo e la scala lunga valgono solo nelle stazioni del sonar: il Museo e i riquadri tengono la scala corta.
- **Le varianti per schermata** (verificate con le istantanee del 03/11/2026):
  - variante A, la lamiera: scelta della missione, regolazioni della sandbox, Museo; briefing e debriefing coi documenti
    di B (gruppo 3);
  - variante B, legno e carta: albo, fascicolo, bacheca, albo d'onore ed esito, col piede di cerimonia (gruppo 4);
  - variante C, la consolle «SALA ANALISI»: replay, statistiche, serie e confronto della sandbox, e dal 03/11/2026 la
    vista dal vivo (gruppo 6: `quadro::consolle_vivo` e `quadro::riga_registro`).
- **In arrivo**: i ritocchi della bozza del sonar (§2.9) e le stazioni dell'Enhanced, una bozza alla volta.

## 8. Riferimenti

- Codice: `grafica/` (palette, tela, riscala, font, sprite, modello, periscopio, oculare, cielo, moto_navi, effetti,
  carta_vga, strati_meteo, sale, consolle, tema_stazioni, spaccato, cerimonia, ritratto, decorazioni, giornale, cornici,
  medaglioni, sagome_armi, simboli_carta, cartolina, cascata_firma, scala_cascata, lamiera, quadro, grafici_serie);
  `nucleo/onde.hpp`, `nucleo/geometria.hpp`; nell'app `carta.cpp`, `interfaccia.cpp`, `sonar_enhanced.cpp`,
  `scelta_missione.cpp`, `regolazioni_sandbox.cpp`.
- Strumenti: `tools/navi/` (kit e modelli), `tools/pipeline/` (font, palette, esportazione).
- Progetto: `docs/06_kit_navi.md`, `docs/10_danni_affondamento_esplosioni.md` §5, `docs/00_ROADMAP.md` (decisioni
  tecniche sulla grafica).
- Ricerca: dossier 19 (consolle), 20 (periscopi), 21 (meteo e mare), 25 (estetica dei simulatori) con le bozze in
  `docs/ricerca/25_bozze/indice.md`.
