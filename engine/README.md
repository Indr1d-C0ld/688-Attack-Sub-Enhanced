# Motore di 688 Attack Sub — Enhanced (Fase 1)

C++20 + SDL3, CMake (decisioni in `docs/00_ROADMAP.md`). Stato al 23/10/2026: fondamenta e una scena di prova
al periscopio con le 15 classi navali costruite col kit (`docs/06_kit_navi.md`), luce del giorno calcolata dal Sole,
moto interpolato e schermate originali portate a 640×480 con Scale2x.

## Struttura

| Parte | Cartella | Contenuto | Dipende da |
|---|---|---|---|
| nucleo | `nucleo/` | geometria nautica, orologio a passo fisso, generatore casuale deterministico, mondo e cinematica con stato interpolato, posizione del Sole, database delle classi (TOML), fondale dei teatri e rotte per acque navigabili, fisica dei danni (idrostatica per stazioni, allagamento, trave, incendi), fenomeni visibili di esplosioni e incendi, ordini del giocatore con coda e storico per il replay | toml++ |
| grafica | `grafica/` | tela 640×480 a 256 colori indicizzati, palette (miscela, luce ambiente), sprite delle navi (anche per elevazione), modelli 3D e sprite generati al volo per le navi danneggiate, effetti a particelle (fuoco, fumo, esplosioni, acqua attorno alla nave), font originale, vista al periscopio, Scale2x e stiramento 5:6, istantanee BMP | nucleo |
| missioni | `missioni/` | missioni in Lua 5.4 in un ambiente isolato e runtime del livello classico (semantica della VM del 1989), colpi delle armi, danni, relitti e reazioni nelle missioni | nucleo, Lua 5.4.7 |
| piattaforma | `piattaforma/` | finestra SDL3: presentazione a multipli interi senza filtri, tastiera | grafica, SDL3 |
| app | `app/` | eseguibile `melchior688` e scena di prova | tutto |
| prove | `test/` | test doctest (nucleo, grafica e dati esportati) | nucleo, grafica |

Il nucleo non conosce né la grafica né SDL. La simulazione avanza a passo fisso (1/30 s), quindi è
deterministica: `Mondo::impronta()` permette di verificarlo. Grafica e nucleo si compilano e si provano
anche senza SDL (`-DM688_FINESTRA=OFF`).

## Compilazione (senza sudo)

Niente è installato nel sistema:
- **CMake 3.31.6 e Ninja** sono nel venv del progetto: `tools/venv/bin/pip install cmake==3.31.6 ninja==1.11.1.4`;
- **SDL3 3.2.10** viene dai pacchetti Debian estratti in `tools/deps/sysroot`:
  `apt-get download libsdl3-0=3.2.10+ds-1 libsdl3-dev=3.2.10+ds-1 libsndio7.0` in `tools/deps/deb`, poi
  `dpkg -x <pacchetto> tools/deps/sysroot`. Le altre dipendenze di SDL3 (X11, Wayland, PipeWire, PulseAudio)
  sono già presenti nel desktop;
- **doctest 2.4.11 e toml++ 3.4.0** (header singoli, licenza MIT) e **Lua 5.4.7** (sorgenti C, licenza MIT) sono in `terzi/`.

```bash
engine/costruisci.sh
```

Lo script configura, compila e lancia le prove; `--pulito` riparte da zero. L'eseguibile ha un RPATH verso
`tools/deps/sysroot` (anche per `libsndio`, dipendenza di libSDL3).

## Dati

I dati vengono da `tools/pipeline/esporta_motore.py` e finiscono in `data/motore/`:

| File | Formato |
|---|---|
| `palette/giorno.pal`, `notte.pal` | 768 byte RGB a 8 bit (DAY.PAL e NIGHT.PAL dell'originale) |
| `font/originale.fnt` | `FNT1`, u16 glifi, u8 righe (15); per glifo u32 codepoint, u8 larghezza, righe (bit 7 = colonna 0). Font proporzionale di GAME.EXE, più à è é ì ò ù (e maiuscole), °, × |
| `navi/<classe>.spr` | `SPR2`, u16 sprite, f32 pixel per metro; per sprite u16 AOB, u8 lato (0 dritta), i8 elevazione (oggetti in volo), u16 larghezza, u16 altezza, f32 ancora x e y (centro nave al galleggiamento), u32 byte compressi, indici (159 = trasparente) compressi con DEFLATE (formato in `tools/pipeline/esporta_motore.py`) |
| `classi.toml` | per classe: nome, sprite, tipo nell'originale, dimensioni, velocità massima, pescaggio (navi e sottomarini), fonte, affidabilità |
| `modelli/<classe>.mdl` | `MDL2`: modello 3D del kit con l'opera viva (nei sottomarini anche timone inferiore e piani poppieri) e tessitura cotta dalle foto, per le navi danneggiate; triangoli e tessiture compressi con DEFLATE senza perdite (formato in `tools/navi/esporta_modello.py`) |
| `scafi/<classe>.toml` | sezioni dello scafo (60 stazioni, semilarghezze ogni 0,25 m) e compartimentazione per la fisica dei danni (`tools/navi/compartimenti.py`) |
| `teatri/<teatro>.fdl` | `FDL2`, u16 larghezza, u16 altezza, f32 lato della cella (m), f32 scala, f32 offset, u32 byte compressi, poi i valori u16 per cella (riga 0 = nord: quota = valore × scala + offset) compressi senza perdite: scarto dalla predizione a piano (sinistra + sopra − diagonale), byte bassi poi alti, DEFLATE (formato in `tools/pipeline/esporta_motore.py`). Le heightmap di `data/teatri/`: 7 teatri, celle da 200 m, 11,8 MB in tutto (32 in chiaro). Dopo i dati, il blocco `GEO1` (f64 latitudine e longitudine del centro della proiezione). Accanto, `<teatro>.suono.toml`: profili del suono |
| `teatri/<teatro>_carta_<nato\|urss>.fdl` | carta d'epoca della parte (`tools/pipeline/carta_epoca.py`): stesso formato e stessa griglia, più i blocchi `RIL1` (campagne di rilievo e campagna per cella) e `ANN1` (annotazioni) (formato in `carta_epoca.scrivi_fdl`); vedi «Carte d'epoca e georeferenziazione» |
| `immagini/<NOME>.img`, `.pal` | `IMG1`, u16 larghezza, u16 altezza, indici; con la palette della schermata (TITLE, MAIN) |

```bash
tools/venv/bin/python -I tools/pipeline/esporta_motore.py .
```

L'ancora degli sprite la scrive `tools/navi/costruisci.py` in `extracted/navi/<classe>/sprite/ancore.json`.

## Uso

```bash
engine/build/melchior688
```

Comandi:

| Tasto | Azione |
|---|---|
| frecce sinistra e destra | ruotano il periscopio (con Maiuscolo, più veloce) |
| frecce su e giù, `I` | ingrandimento 1,5× o 6× (alla carta `I` e `O` sono lo zoom) |
| PagSu e PagGiù | compressione del tempo, fino a ×64 |
| tastierino 4 e 6, 5 | timone uno scatto a sinistra o a dritta, al centro (in missione) |
| tastierino 8 e 2, 0 | barra di immersione uno scatto su o giù, a zero: tiene la quota (in missione) |
| `+` e `-` | telegrafo avanti o indietro di una posizione (in missione) |
| `.` | pilota automatico (in missione, con i punti di rotta) |
| Ctrl+P, Ctrl+1, Ctrl+E | quota periscopica, 150 piedi, emersione d'emergenza (in missione) |
| `U`, `N` | periscopio e antenna su o giù, solo a quota periscopica (in missione) |
| `H` | stadimetro: distanza del bersaglio di superficie nel periscopio alzato |
| `F2`, `F3`, `F4`, `F6`, `F7`, `F8`, `F9` | stazione Radio, Stato, Manovra, Navigazione (carta al posto della vista), Armi, Sonar, Centrale (in missione) |
| `1`–`4` alla Radio | periscopio, antenna, messaggio precedente e successivo dell'archivio; con `--regola simulazione`: antenna, filo galleggiante, archivio, e `5`–`8` ordini di missione, tipo di trasmissione, trasmetti, satellite |
| `9`, `[` `]` alla Radio con `--regola simulazione` | lancia una boa SLOT, ritardo della sua trasmissione (5-30 minuti) |
| `P` | pausa (nel classico ferma il tempo anche la Centrale, come il CONN dell'originale) |
| mouse | pulsanti dei pannelli; durante TARGET un clic sul segno lo sceglie; durante SET un clic sulla carta mette il punto |
| `1`–`9` | pulsanti della stazione: i tubi alle Armi; filtro, impulso attivo, schiera, ANALIZZA al Sonar |
| `R`, `A`, `M` | tubi 1, 5 e 7: il primo tasto seleziona, il secondo arma o libera (in missione) |
| `L` | lancio dal tubo selezionato (in missione) |
| `G` | TARGET: ferma il tempo; una lettera o Tab (anche le X) sceglie il contatto, Invio conferma, Esc toglie il bersaglio |
| `S`, `C`, `D` | siluro filoguidato successivo; ricerca; DETONATE (in missione) |
| `W`, `K` | carta: punto di rotta col cursore (frecce, Invio; tempo fermo), cancella l'ultimo |
| `J`, `[` e `]`, `B` | carta: proiezione a N minuti, minuti, sfondo senza fondale |
| `1`, `2`, `3` alla carta | ALL TRACKS, TARGET TRACK (scie di tutti o del bersaglio), PLOT PROJ |
| tastierino, `+` e `-` con `--regola simulazione` | Enhanced: barra a scatti di 5° fino a 35°, campane da indietro tutta a massima, quota ordinata a passi di 15 m |
| Tab, `[` `]` alle Armi con `--regola simulazione` | impostazioni del siluro del tubo selezionato (campo e valore) |
| C, D, Z X, 8 9, Y con `--regola simulazione` | siluro scelto via filo: testa attiva, spegnimento, rotta di 10°, quota di 15 m (alle Armi), taglio del filo |
| Ctrl+F con `--regola simulazione` | posarsi sul fondo: discesa lenta e macchine ferme vicino al fondo, o il rifiuto con il motivo |
| `1`, `4`–`9`, Z X al Sonar con `--regola simulazione` | filtro passa banda o passa alto, ANALIZZA la firma, sensore successivo, fascio del LOFAR (bersaglio, poi i tracker), integrazione 1/2/4/8 righe, AUTO (modo moderno), cursore (lettura, delta, armonico), fascio fisso di 5° a sinistra o a dritta |
| Tab, `[` `]`, frecce, I O, T K al Sonar con `--regola simulazione` | campo e valore della presentazione (pavimento, tetto, palette, scala dei tempi), cursore, zoom del LOFAR, nuovo tracker sul fascio, togli il tracker seguito; `--sonar 1989` o `moderno` sceglie il modo |
| B, W J, S Y C, Invio al Sonar con `--regola simulazione` | libreria (Tab scheda, frecce voce, Invio sovrappone la voce al LOFAR, Esc chiude); annotazione al cursore e via l'ultima; salva lo spettro in uno slot, slot successivo, confronto (affiancato, sovrapposto, differenza); Invio toglie la voce sovrapposta |
| `7`, `9`, Q alla carta con `--regola simulazione` | modo segno (Tab affiliazione, `[` `]` tipo fra subacqueo, superficie, aereo, nota, croce e zona, Invio mette e apre la scritta, K toglie, Maiuscolo+su/giù il raggio della zona); pannello tempo-rilevamento; regolo di Ekelund (frecce rotta e distanza, `[` `]` velocità, Invio al MATE) |
| `4`, `5`, `6`, `8`, E, F, Maiusc+K, B alla carta con `--regola simulazione` | righello (frecce, Tab, clic, F aggancia, Invio fissa, K toglie), linee di rilevamento, anelli, CPA/TCPA, evento con l'ora, pulisci il lucido, stile della carta (`--stile-carta vga|nautica|foglio`); `--carta epoca|vera` sceglie la carta d'epoca della propria parte (predefinita) o il fondale vero |
| Tab al quadro danni (F3) con `--regola simulazione` | compartimento da ingrandire nello spaccato, a giro, poi di nuovo il più colpito |
| clic sui tubi, Ctrl+S alle Armi con `--regola simulazione` | ricarica, annulla, arma o disarma il tubo; silenzio a bordo (ricariche lente e silenziose) |
| `--menu` | avvio dalle schermate: scelta della missione (frecce o cifre, sinistra e destra 688 o Alfa, Tab stazioni 1989 o moderne, `[` `]` carta d'epoca o vera, Invio), briefing, missione |
| `--briefing` con `--regola simulazione` | sala briefing prima della missione: `1`–`8` livelli, `9` tutto o niente, Tab ordini, frecce e I O sulla carta, Invio in mare, Esc rifiuta |
| `T` alla carta | MATE della TMA (con `--regola simulazione`): frecce per rotta e distanza, Maiuscolo con su e giù la velocità |
| Invio, Esc | con la schiera fuori, 3/4 e oltre chiedono di tagliarla: Invio la taglia, Esc annulla |
| `V` | "verità": classe, distanza e AOB dei contatti (diagnosi) |
| `F12` | istantanea BMP |
| `Esc` | esce |

I tasti si possono rimappare: `engine/build/melchior688 --scrivi-tasti tasti.toml` scrive il modello con tutti i
comandi; modificato, si usa con `--tasti tasti.toml` o copiandolo in `~/.config/melchior688/tasti.toml`.

Opzioni della scena: `--ora 7.4` (UTC, ore decimali), `--data 1989-10-15`, `--lat 61.5 --lon -7` (teatro;
predefinito a sud delle Faroe). Schermata originale ingrandita: `--immagine TITLE` (Scale2x) o con `--nitido`.

Senza finestra, per verifiche e documentazione:

```bash
engine/build/melchior688 --istantanea vista.bmp --secondi 60 --rilevamento 20 --alto --verita
```

## Modello della vista al periscopio

- **Campo visivo** (stima di gioco, periscopio tipo 18): 32° a 1,5× e 8° a 6×.
- **Lente** a 1,5 m sull'acqua a quota periscopica.
- **Orizzonte geometrico** con rifrazione standard (raggio terrestre efficace 7/6): a 1,5 m d'occhio è a
  circa 4,7 km.
- **Navi in scala**: nella scala corretta per distanza e ingrandimento.
  - Lo sprite è scelto per AOB (passo di 5°) e lato.
  - Il galleggiamento cala sotto l'orizzontale per la quota dell'occhio e per la curvatura terrestre.
  - Oltre l'orizzonte la parte bassa della nave è nascosta dal mare: a 17 km si vedono solo gli alberi.
- **Luce**: elevazione del Sole (algoritmo NOAA semplificato) per teatro, data e ora; dal crepuscolo nautico
  (−12°) al pieno giorno (+10°) gli indici della vista (cielo, mare, grigi degli sprite) si scuriscono e virano
  al blu, mentre quelli dell'interfaccia restano invariati. La palette notturna dell'originale (NIGHT.PAL: luce
  rossa della centrale) resta per gli interni.
- **Moto**: il disegno usa lo stato interpolato tra gli ultimi due passi (`Mondo::interpolato`).
- **Navi danneggiate** (`Nave::posa.danneggiata`): lo sprite si genera al volo dal modello 3D della classe
  (`grafica/modello.hpp`, `docs/10_danni_affondamento_esplosioni.md` §5.1).
  - Rasterizzatore software con la stessa pipeline degli sprite precalcolati: camera da 4 km, sovracampionamento
    3×3, foto cotte nella tessitura (una per faccia), ombreggiatura di ripiego, 16 grigi con retinatura di Bayer
    in fase con gli sprite.
  - A nave intatta lo sprite generato coincide con quello precalcolato: stessa sagoma e stesse ancore, scarto
    medio di circa 0,2 livelli di grigio (prova `modello: sprite generato contro sprite precalcolato`).
  - Caricamento (`Modello::carica`): decompressore DEFLATE proprio (`nucleo/inflate.hpp`, niente zlib,
    lo usano anche gli sprite `SPR2`), poi la quota
    delle foto e il valore dei texel visti ricostruito dalla predizione MED; in memoria la tessitura resta intera
    (2 byte per texel). Il formato `MDL1` non si legge piu': si riesporta con `tools/navi/esporta_modelli.sh`.
  - Posa: sbandamento, assetto, immersione e spostamento per la nave intera o per due tronconi (spezzata a una
    x data). La parte sotto il livello del mare non si vede; l'ancora è il centro nave sul livello del mare.
  - La risoluzione segue la distanza (al massimo 3,75 px/m, come gli sprite). Lo sprite si rigenera solo quando
    cambiano vista, posa o scala.
  - Prova: `--colpo siluro_chiglia|siluro_fianco|siluro_prua|siluro_poppa|due_siluri|missile [--bersaglio perry]
    [--seme N]` colpisce una nave della scena di prova; la posa viene dalla fisica dei danni (sotto).
- **Effetti** (`grafica/effetti.hpp`): particelle nel mondo, ordinate per distanza insieme alle navi (sotto,
  «Effetti visivi»).
- **Font**: l'originale allinea i caratteri in alto. Il motore porta per default le maiuscole piccole e le cifre
  sulla linea di base delle maiuscole grandi; `Font::allinea_base(false)` torna al comportamento originale.

### Oculare dell'Enhanced (docs/14_periscopi.md, P3)

`VistaPeriscopio::oculare` (`grafica/oculare.hpp`, `ParametriOculare`) accende la vista dei periscopi dell'Enhanced.
Spento (il predefinito) la vista resta quella del classico. `parametri_oculare(DatiPeriscopio, StatoPeriscopio)`
(`nucleo/periscopi.hpp`, P1) li ricava dal modello e dal posto; filtro (`filtro_da_nome`) e stadimetro li imposta poi
l'interfaccia. Acceso:
- **Testa sott'acqua** (alzata in corso): acqua torbida, più chiara verso la superficie, con le bolle.
- **Il mare sulla finestra** (`oculare::acqua_sulla_finestra`, col posto noto: `altezza_testa_m`). L'onda nel punto
  della testa viene da `nucleo/onde.hpp` (cinque componenti attorno al picco di Pierson-Moskowitz, varianza (Hs/4)²),
  che anche il modello del posto può usare.
  - Il fianco dell'onda sale dal basso, col profilo mosso, la schiuma e le strisce, finché la finestra è sommersa.
  - Quando l'acqua scende resta un velo che scola; le gocce (piccole lenti che capovolgono la scena) scivolano e
    asciugano in qualche secondo.
  - Col mare da 4-5 e la cresta vicina sotto la finestra ci sono gli spruzzi; `pioggia` (0-1) fa le sue gocce.
  - L'altezza dell'occhio segue l'onda: le creste vicine coprono di più le navi lontane.
- **Mare mosso** (`oculare::mare`): non più il gradiente piatto.
  - La superficie sotto l'orizzonte è fatta di quattro onde lunghe attorno al picco di Pierson-Moskowitz, sparse di
    ±35° attorno a `direzione_mare_gradi`, e di quattro onde corte del vento. Sono ferme nel mondo e corrono con la
    loro velocità di fase.
  - Chiaro e scuro vengono dalle facce verso l'occhio, che a pelo d'acqua riflettono il cielo. Le onde più corte di
    qualche pixel si spengono con la distanza, e verso l'orizzonte c'è la foschia (Koschmieder).
  - La schiuma delle creste che frangono copre la frazione di Monahan W = 3,84·10⁻⁶ U^3,41. `vento_kn` negativo dà il
    vento del mare formato per l'altezza delle onde.
  - Scene `mare_2`, `mare_5`, `mare_7` dell'anteprima.
- **Campo:** il cerchio è largo 300 pixel e il campo vero del modello ne copre il diametro (32°, 8°, 4° del Type 18 a
  12×); la scala dei rilevamenti resta sopra il cerchio.
- **Reticolo** per scuola, senza numeri dentro il cerchio: croce americana a tutto campo con le tacche (grandi e
  piccole dai passi di `periscopi.toml`), griglia sovietica con le tacche da un solo lato e il vuoto al centro, pali
  britannici. Vignettatura retinata verso il bordo.
- **Elevazione** della linea di mira: cielo e mare seguono l'elevazione di ogni riga; gli aerei si vedono col loro
  sprite per elevazione. `Periscopio::riga_elevazione` dà la riga di un'elevazione apparente.
- **Tremolio** (0-1, dal modello): la linea di mira oscilla fino a 0,5° (STIMA).
- **Stadimetro:** l'immagine fantasma, spostata in su di `sdoppiamento_gradi` (a sinistra con la lente ruotata),
  a scacchiera sull'immagine vera.
- **Filtri** (polaroid, rosso, verde, giallo, affumicato) e **modi monocromi** nella rampa 160-174: bassa luminosità
  verde, intensificatore sovietico verde giallino, TV in bianco e nero 4:3 con le righe e il reticolo chiaro,
  termico. I modi notte amplificano la luce ambiente e aggiungono grana. La palette si prepara con
  `oculare::palette` dopo `illuminata`; la vista vuole `Periscopio::usa_palette(giorno)`.
- **Telecamera verticale** della vela dell'Alfa (`oculare::telecamera_verticale`): caustiche della superficie o pack
  coi canali, con il guadagno automatico e la grana.

Immagini di controllo: `engine/build/anteprima_periscopio data/motore <cosa> uscita.bmp`, con l'elenco delle scene
in testa a `strumenti/anteprima_periscopio.cpp`; sono in `extracted/navi/_verifica/periscopio/`.

### Meteo nella vista (docs/15_meteo_e_mare.md, M3)

`VistaPeriscopio::meteo` (`grafica/cielo.hpp`, `MeteoVista`) accende il meteo nella vista, con o senza l'oculare.
Spento (il predefinito) la vista resta quella di oggi. `applica_meteo(v, StatoMeteo, lampi, occhio)` lo riempie dal
meteo della missione (`nucleo/meteo.hpp`, M1), insieme a onde, visibilità, mare e vento dell'oculare.
- **Nubi** (`cielo::nubi`): su un piano alla quota del plafond, in prospettiva, portate dal vento.
  - La copertura viene da una soglia sul rumore frattale.
  - La base è scura dove la nube è spessa e sotto i cumulonembi; verso l'orizzonte le nubi si confondono nella foschia.
  - Sotto i temporali ci sono le cortine di pioggia a 8-25 km.
- **Foschia e nebbia** (`cielo::foschia`): per distanza (Koschmieder) verso il grigio della foschia, sul mare e sul cielo
  basso. La nebbia d'avvezione e quella di radiazione hanno uno strato basso.
- **Fumo di mare** (`cielo::fumo_di_mare`): una fascia bassa sull'orizzonte, con colonne che salgono. Va dopo le navi,
  perché ne nasconde gli scafi.
- **Pioggia e neve** (`cielo::precipitazione`): strisce che pendono col vento di traverso e fiocchi che derivano; con
  l'oculare la pioggia bagna anche la finestra.
- **Lampi e fulmini** (`cielo::fulmini`, `cielo::lampo`):
  - 3-4 colpi a 57 ms;
  - il canale con i rami solo per le scariche nube-mare entro 15 km, altrimenti il bagliore dietro le nubi;
  - la luce del lampo per `Palette::schiarita`, piena di notte e al 25% di giorno.
- **Moto delle navi** (`grafica/moto_navi.hpp`):
  - rollio e beccheggio RMS per taglia e stato del mare, dalla tabella del dossier 21 §7.2 calibrata sul DD-963;
  - il rollio al periodo proprio, il beccheggio e il sussulto al periodo d'incontro;
  - lo sprite gira attorno al galleggiamento dell'angolo apparente;
  - la prua che si pianta solleva lo spruzzo da mare 5.
- **Elicotteri in hover col vento:** muso al vento e un po' basso, oscillazioni; negli effetti il cavo del sonar pende
  sottovento di 5-15° e l'anello di spruzzi si sposta (`AereoAsw::vento_m_s`, `vento_da_gradi`).

Immagini di controllo: `engine/build/anteprima_meteo data/motore <teatro> <mese> <giorno|notte|temporale|nebbia>
uscita.bmp [classico]`, col clima del teatro e del mese da `meteo.toml`; sono in `extracted/navi/_verifica/meteo/`.

## Missioni

`melchior688 --missione SCENE1A [--seme N]` gioca una missione originale convertita (`data/missioni/`,
`docs/09_schema_missioni.md`). Il file Lua gira in un ambiente isolato (solo `m688`, `math` senza `random`,
`string`, `table`; niente `io`, `os`, `load`, `require`, `dofile`). Il runtime (`missioni/`) riproduce la VM:
- buffer del modello corrente (`salva_modello`, `seleziona_*`, `nuova_unita`, `lancia`);
- contatti con qualità e durata della traccia e scelta del bersaglio;
- sensori a ogni passo (9 al secondo, anche nell'app dal 26/10/2026), timer, variabili e flag di scenario;
- messaggi di bordo (testi italiani in `data/motore/testi/`) ed esito di fine missione (codici ≥ 100), con il
  rapporto del briefing.

Fedele alla VM verificata il 23/10/2026 (`docs/originale/sensori_e_navigazione.md`), compresi i difetti
dell'originale (annotati nel codice):
- **scoperta**: limite di 20 nm per asse in quarti di unità; scoperta automatica (armi del proprio lanciatore,
  bersagli in superficie entro 9 nm da navi e aerei, navi e aerei al periscopio); radar/ESM contro periscopio
  (4,5 nm) e antenna (+2,25 nm); ascolto passivo solo nel settore di prua (±30°, ±90° con la schiera) con portata
  T = G + S·(Nb + 40 − v_oss) + E, termoclini (G/2, rumore/4), qualità e durata della traccia come nel record
  contatto;
- **eventi acustici** (`impulso_sonar` 33 s, `lancio_missile` 100 s, `rumore_transitorio` 11 s): chi li emette si
  rivela entro 20 nm, il ping illumina il settore di prua (±72°);
- **navigazione**: soglia d'arrivo `+50` in quarti di unità per asse (0 = mai, 120 sulle navi = un asse);
  l'inseguimento accosta sul rilevamento finché la traccia non è solida;
- **moto classico**: 1 kn = 0,048065 unità di gioco al secondo (`posa.moto_classico_m_s_per_nodo`, circa 3,46 volte
  il vero in scala di gioco), così timer e distanze delle missioni tornano; `Missione(..., moto_classico=false)` per
  le velocità reali.

- **cadenza e bersaglio** (§3.1 e §8): 9 passi al secondo. A ogni passo c'è un solo osservatore, a rotazione
  sulla lista dalla più recente alla più vecchia, che valuta tante unità quante ne sono state create nella
  missione, cioè tutte le altre (qualcuna due volte quando ne sono uscite dal gioco). Un'unità osserva una volta
  ogni `tipo` giri (`TYPE`, quasi sempre 1); le non combattenti (flag 40h) e le unità dell'IA che affondano non
  osservano mai. Lo script «al bersaglio» (`+10`, `gestori.bersaglio`) scatta una volta quando il conto alla rovescia
  di un contatto arriva a zero, contando solo le valutazioni con segnale > 500 (1 per i contatti nuovi, n con `ingaggia(n, …)` sul contatto agganciato); il
  contatto si perde alla prima valutazione senza scoperta e, se era agganciato o inseguito, gira lo script `+56`
  (bersaglio perso); posizione stimata copiata dal vero solo con traccia solida e segnale > 500. I contatti deboli
  lampeggiano: l'interfaccia mostra solo quelli con `Contatto::visibile()`.

- **colpi** (§9.8, ricavato dal disassemblato il 24/10/2026):
  - un'arma colpisce qualunque unita' entro 5 unita' per asse e 100 piedi di quota, compresi amici e neutrali;
    sono esclusi altre armi, aerei, falsi bersagli e il collegato, finche' c'e' il filo;
  - danno `+3D`, dimezzato in 7 passi su 18; l'arma esce subito dal gioco;
  - messaggi 7, 26, 28, 53, 67, conto delle uccisioni (variabili 10..19), `+5E` per `unita_colpita`;
  - `attacca` (cariche): probabilita' (18 − b)/18, danno fisso 30; `attacco_casuale` (missili): colpisce l'unita'
    corrente;
  - l'opcode 37h (`togli_unita`) toglie l'unita' corrente: fine corsa, esca esaurita;
  - `seleziona_collegata` seleziona il bersaglio;
  - nell'inseguimento uguaglia la quota chi non e' nave o aereo: un siluro sale alla quota di una nave;
- **distruzione**: conto di 18 visite (30 per il giocatore) a un'unita' per passo, nell'ordine della lista dei
  sensori e dalla testa a ogni nascita o uscita. Intanto l'unita' prosegue con
  gli ultimi ordini, resta rilevabile e non osserva piu'. Poi esce dal gioco; il giocatore in superficie resta un
  relitto, altrimenti messaggio 25 ed esito 2.

- **comandi del giocatore** (fase C1 di `docs/11_comandi_e_stazioni.md`):
  - `Missione::comando` fa da dispatcher dell'originale: gestore della missione con i parametri del §7;
  - `Missione::applica` traduce gli ordini del nucleo (`nucleo/ordini.hpp`): manovra, quote, punti di rotta e
    pilota automatico, sensori, tubi, lancio, guida a filo;
  - `--ordini FILE` ripete gli ordini salvati, `--registra FILE` li salva;
- **lancio** (`LAUNCH`, 2011:18B7): prepara il modello con il contatto del bersaglio a traccia solida, mentre
  l'unita' la crea `nuova_unita` (prima nascevano due siluri).
- **controlli dei giocatori e accostate** (§5 e §9.6):
  - strati termici, limite di velocita' sopra i 100 ft, periscopio e antenna che si abbassano sotto i 20 ft;
  - ogni unita' che naviga rallenta a un quarto di avanti tutta per accostare oltre 3°;
  - `quota` con rateo 0 tiene la quota, come `DEPTH_TO`.
- **angoli e virate** (24/10/2026, `docs/originale/numeri_eseguibile.md` §3.3–§4):
  - l'originale usa angoli matematici (0 = est, senso antiorario); il convertitore scrive le missioni in gradi
    bussola, con virate e ratei positivi a dritta. Il runtime resta tutto in bussola;
  - un rateo di virata non nullo si ferma quando la prua passa per la rotta ordinata (2011:09FC–0A20), salvo dopo
    `rotta(nil)` (HEAD_TO −1, rotta libera: il timone del giocatore);
  - `rotta(N)` lascia il rateo com'è, `virata_relativa` accosta nel verso più breve al rateo standard, e
    `accosta_a` (TURN_TO) gira **sempre a sinistra** al rateo standard, come l'originale.

- **stazione Armi** (fase C3, `nucleo/armi_giocatore.hpp` e `missioni/stazioni_missione.hpp`): tubi, scorte,
  armamento in due tempi, ricarica automatica, controlli del lancio e della guida e limite delle armi in acqua
  come il programma dell'originale (non gli script); lo stato dipende solo dagli ordini e dai messaggi della
  missione, quindi il replay torna.

- **stazione Sonar, guasti, carta e barra del bersaglio** (fase C4, `nucleo/sonar_giocatore.hpp`,
  `nucleo/guasti_giocatore.hpp`, `nucleo/carta_giocatore.hpp`, `missioni/contatti_giocatore.hpp`,
  `app/interfaccia.hpp`, `app/carta.hpp`): schiera, filtro, impulso e ANALIZZA; i sei sistemi che si guastano con i
  colpi; i contatti del giocatore (non la verita') sulla carta, al periscopio e nella barra; TARGET e SET che fermano
  il tempo come l'originale. Il collegamento alla missione e' `missioni/stazioni_missione.hpp`.

Ancora da fare: il mouse.

### Danni nelle missioni

`missioni/danni_missione.hpp` (fase D5, `docs/10_danni_affondamento_esplosioni.md` §7) da' a ogni nave di
superficie colpita in missione la sua fisica dei danni. La testata dipende dall'arma della missione:
- **Mk 48:** 544 kg sotto la chiglia, spoletta d'influenza;
- **533 mm sovietico:** 300 kg a contatto sul fianco;
- **carica:** 135 kg sotto lo scafo;
- **missile:** dal lanciatore: Granit 750 kg, Bazalt 1000, Malakhit 500, Harpoon 221, altri 500.

Il punto d'impatto viene dal seme. Nel mondo da disegnare l'unita' degli script lascia il posto alla sua
immagine (id 100000 + unita'), con la posa della fisica, fiamme, fumo e alberi che cadono. Le opzioni sono
`--missione SCENE9C --regola classica|simulazione`.

**Regola classica** (predefinita, livello classico): gli script restano fedeli.
- Quando la missione da' una nave per distrutta, la nave diventa un **relitto** fermo dove e' stata colpita.
- Se la fisica non la condanna gia', l'equipaggio la abbandona e si aggiungono falle sul fondo di tutti i
  locali (1 cm^2 per tonnellata): affonda in 30–100 minuti, nel modo che la fisica sceglie. La petroliera carica
  resta a galla come relitto.
- Se la fisica la affonda mentre per gli script e' ancora viva, l'unita' degli script prosegue invisibile.

**Regola di simulazione** (Enhanced): l'integrita' dell'originale scende ma non distrugge. Gli impatti si
controllano su ogni coppia a ogni passo (sotto, «Impatti dell'originale»).
- La missione da' la nave per distrutta quando la fisica la condanna o la affonda, oppure quando l'integrita'
  e' esaurita e la nave e' **menomata**: senza energia, spezzata, incendi estesi, acqua oltre il 5% del
  dislocamento, sbandamento oltre 8°. Cosi' gli script non la bersagliano all'infinito.
- **Reazioni** (ordini del motore che prevalgono sugli script):
  - senza energia, ferma;
  - con sbandamento o acqua a bordo, 5 o 12 nodi;
  - ritirata di 20 minuti lontano da chi ha attaccato;
  - quando si abbandona la nave, le navi amiche illese entro 20 nm vanno sul punto a 15 nodi.

**Messaggi del motore** (codici 200+, in `data/motore/testi/messaggi_bordo.tsv`, entro 20 nm dal giocatore):
- rumori di rottura;
- esplosione secondaria;
- bersaglio fermo;
- sbanda e imbarca acqua;
- affondato;
- richiesta di soccorso intercettata;
- unita' dirette sul punto dell'attacco.

Il 67 dell'originale (fuoco amico) ora ha il suo testo.

**Esiti sulle 22 scene** (giocatore passivo, 60 minuti, 5 semi, sul fondale reale): i siluri dell'IA colpiscono.
Il rilievo si ripete con `engine/build/rilievo_missioni [secondi] [semi] [scene]` (alcuni minuti); i numeri
sono della base di riferimento (sotto): cadenza dei sensori, visite e impatti dell'originale, `LAUNCH` corretto.
- SCENE8A: i sottomarini sovietici affondano Iowa, Sacramento, Kidd e Perry del gruppo navale (una o due per
  partita), come racconta il briefing; la scorta affonda l'Alfa in 3 partite su 5.
- SCENE6A: i missili distruggono le 5 navi da carico del convoglio. SCENE9C: ne distruggono 5 in un seme, 1 negli
  altri, piu' le 2 Perry.
- SCENE4A: le bombe di profondita' distruggono il giocatore in 1 partita su 5.
- SCENE9B: un Oscar affondato in 4 partite su 5.
- SCENE7B: il giocatore e' distrutto in tutte le partite (esito 2), e con lui il Typhoon da scortare.
- SCENE1B (TORPEX '89): il 688 avversario affonda da 1 a 3 Spruance bersaglio in 4 partite su 5.

**Cadenza della scoperta come l'originale** (24/10/2026; `docs/originale/sensori_e_navigazione.md` §3.1). Prima
ogni unita' valutava un bersaglio a ogni passo; ora c'e' un osservatore per passo che le valuta tutte, e le non
combattenti (flag 40h: mercantili, biologici, bersagli delle esercitazioni) non osservano mai. Le unita' che
osservano valutano i loro bersagli da 1,3 a 4 volte piu' spesso, secondo quante non combattenti ci sono nella
scena. Confronto sulle stesse 110 partite:
- **prima scoperta del giocatore:** quasi invariata, perche' la decidono la geometria e la scoperta automatica:
  uguale entro 2 s in 68 partite su 74, 224 s prima in una (SCENE7A), 174 s dopo in due (SCENE8B). Nelle
  esercitazioni SCENE1B e SCENE1C i cacciatorpediniere bersaglio, non combattenti, non scoprono piu' il
  giocatore;
- **primo colpo:** prima in 20 partite su 38, mai dopo (da 7 s a 11 minuti), perche' tracce solide e conti di
  `ingaggia` arrivano prima;
- **esiti:** 5 partite su 110 cambiano:
  - SCENE7B, 3 semi: affondano il Typhoon da scortare (esito 6) invece del giocatore (esito 2);
  - SCENE4A, seme 2: il giocatore sopravvive e le petroliere passano (esito 7 invece di 2);
  - SCENE9A, seme 3: nessuna nave del convoglio sopravvive (esito 5 invece di 6);
- **colpi:** armi a segno da 216 a 245; attacchi riusciti da 209 a 184 (tentati da 725 a 777); unita' distrutte
  da 112 a 105. Prima in SCENE8A affondavano anche Iowa e Nimitz, in SCENE9C 4 mercantili su 5; ora in
  SCENE9B e SCENE8B cade qualche unita' in piu'.

**Rotazione delle visite come l'originale** (24/10/2026; §3.1 e §9.2 del documento). Il conto di chi affonda
(`02F3`) ora scorre la lista nello stesso ordine dei sensori e riparte dalla testa quando nasce un'unita' (ogni
lancio) o ne esce una; prima andava per id crescente. Le unita' che affondano escono quindi in momenti diversi,
e intanto restano rilevabili e colpibili. Sulle stesse 110 partite cambiano 12 partite e 3 esiti:
- SCENE8A, semi 1 e 3: cadono Iowa, Kidd e Sacramento invece del solo Kidd. Lo script d'arrivo (`L_01bf`)
  conta le navi perse del gruppo: con tre da' l'esito 3 («gruppo navale attaccato») invece del 4 (al massimo
  due), e lo da' a 62 minuti, all'arrivo della Nimitz, invece che a 51. L'esito 4 e'
  dello script, non quello che il runtime assegna quando cade il giocatore 1;
- SCENE9A, seme 3: torna l'esito 6 (convoglio salvo) che la sola cadenza aveva cambiato in 5;
- armi a segno da 245 a 254, attacchi riusciti da 184 a 194, unita' distrutte da 105 a 114.

Rispetto al runtime di prima di entrambe le correzioni cambiano 6 esiti su 110: SCENE7B (3 semi), SCENE4A (1),
SCENE8A (2).

Poi la fase C1 dei comandi ha corretto `LAUNCH`, che creava due siluri a ogni lancio
(`docs/11_comandi_e_stazioni.md`): 12 esiti su 110 cambiano, armi a segno da 254 a 177.

**Impatti dell'originale** (24/10/2026; §9.8 del documento). L'originale controlla la collisione solo nella coppia
osservatore/bersaglio dei sensori, quindi poche coppie per passo, e dopo un colpo chiude il passo. Il livello
classico fa cosi' (`Missione::impatti_dell_originale`, predefinito). La regola di simulazione (`DanniMissione` con
`--regola simulazione`) controlla invece ogni coppia a ogni passo: un'arma che passa vicino colpisce, chiunque
osservi. Il rilievo con `--ogni-coppia` usa questa seconda regola. Confronto sulle stesse 110 partite (ogni coppia
→ originale):
- armi a segno 177 e 178, unita' distrutte 105 e 108: in totale la stima della fase D5 regge;
- cambiano pero' 20 partite (tempi dei colpi, attacchi tentati) e 2 esiti: SCENE8A, semi 1 e 3, esito 4 a 51
  minuti invece di 6 a 19.
- `verifica_fondale` e `rilievo_missioni` usano la regola del livello classico.

Fra due armi i sensori non valutano niente (`0C25`), ne' colpo ne' scoperta: nessuna differenza nelle 110 partite.

**Contatti verso chi esce dal gioco** (24/10/2026; §10.7 del documento). Prima restavano per sempre, con il bersaglio,
l'aggancio di `ingaggia` e l'inseguimento: l'IA restava agganciata a un'unita' affondata e lo script «bersaglio perso»
(`+56`) non partiva mai. Ora, come l'originale, una valutazione che incontra un contatto orfano prima di quello
del bersaglio lo libera come un contatto perso e finisce li'. I contatti hanno l'ordine di creazione e, per i
giocatori, la lettera piu' bassa libera da «A»; il pool si ferma a 299. Misura sulla stessa fotografia del codice,
con e senza la correzione: cambiano 4 partite su 110 e 2 esiti (SCENE8A, semi 4 e 5: la scorta affonda l'Alfa,
esito 6 invece di 4); armi a segno invariate, attacchi tentati da 547 a 629, perche' l'IA torna a cercare bersagli.

**Contatti deboli e aggiornamento della traccia** (24/10/2026; §3.4 del documento). Ricavato da `2011:10A5-1229`:
- una valutazione con segnale <= 500 fa solo il **lampeggio** (`Contatto::lampeggio`, `+12`) e finisce li': niente
  posizione stimata e niente scelta del bersaglio. Prima il contatto debole faceva scattare lo script «al bersaglio»;
- la qualita' cresce di differenza/4 + 1 **col segno** della differenza (prima sempre + 1, anche con segnale in calo);
- il contatto nuovo passa subito dal primo aggiornamento; la scoperta automatica porta a 1400 la durata solo di un
  contatto che c'e' gia';
- `Contatto::visibile()` e' il test dell'interfaccia (`990C`): traccia solida oppure lampeggio acceso.

Misura sulla stessa fotografia del codice, con e senza: cambiano 49 partite su 110 e 6 esiti (SCENE4A seme 1: 7
invece di 2; SCENE7B semi 1 e 3: 2 invece di 6; SCENE8A semi 4 e 5: 4 invece di 6; SCENE9A seme 5: nessun esito
entro l'ora invece di 2). Armi a segno da 183 a 148, unita' distrutte da 98 a 88; la scoperta del giocatore passivo
arriva piu' tardi (mediana 134 s invece di 93). Separando le correzioni: la fine della valutazione sotto 500 conta
per 35 partite e 5 esiti, il segno della qualita' per 28 partite e 1 esito, il primo aggiornamento per 8 partite e
nessun esito, la durata della scoperta automatica per nessuna. Con tutte e quattro spente si torna a 2 partite
diverse da prima (arrotondamenti). `verifica_fondale`: nessun urto, 5 esiti diversi col fondale come prima.

**Livello ESM** (`Missione::livello_esm()`, `DS:476C`; docs/originale/numeri_eseguibile.md §3.5): l'ultima
intercettazione radar/ESM di una nave o di un aereo sul giocatore con periscopio o antenna alzati, 85 - (|dx| +
|dy|)/32 in quarti; resta senza decadimento, torna a 0 con periscopio e antenna giu' e a ogni uscita di un'unita'
dal gioco (`003C`). Non cambia il comportamento dell'IA.

**Scia della carta** (`Missione::scia()`, `2011:12EE`; docs/originale/numeri_eseguibile.md §6.1): anello di 150 punti.
Il giocatore locale registra se stesso ogni 30 suoi turni di osservazione e, nello stesso turno, la posizione vera
dei contatti che aggiorna con traccia solida. Non cambia la simulazione: rilievo identico alla base.

**Soglia dell'intercettazione** (24/10/2026; `2011:0EDC`, `1003-1016`). Il segnale di 550 garantito dal radar/ESM vale
solo quando la scoperta passiva e' possibile (sonar e rumore non nulli, entro 20 nm) ma manca l'ascolto (G ed E
nulli) o la portata non basta; dentro la portata vale il segnale passivo anche se minore. Il bonus alla qualita'
e' un quarto intero del livello (`DS:476A`): sotto livello 4 non c'e' bonus ne' contatto garantito. Prima il
runtime alzava a 550 ogni segnale con l'intercettazione. Misura sulla stessa fotografia: 2 partite su 110, 2 esiti
(SCENE9A semi 1 e 3: nessun esito entro l'ora invece di 6); armi a segno invariate (142).

**Base di riferimento:** `engine/strumenti/riferimento/rilievo_missioni.tsv` (24/10/2026, 17:00: cadenza, visite e impatti
dell'originale, `LAUNCH` corretto, contatti orfani, contatti deboli e aggiornamento della traccia, angoli in
bussola, soglia dell'intercettazione, fasi C2-C4 fatte fin qui: 142 armi a segno, 79 unita' distrutte). Si confronta con
`python3 engine/strumenti/confronta_rilievo.py engine/strumenti/riferimento/rilievo_missioni.tsv NUOVO`. Va rifatta
quando il comportamento delle missioni cambia di proposito, dicendo qui che cosa e' cambiato.

**Angoli in bussola (24/10/2026):** rispetto alla base delle 16:30, la correzione degli angoli e della virata cambia
100 partite su 110 e 22 esiti; le armi a segno passano da 148 a 142, le unita' distrutte da 88 a 83.
- Tutte le rotte iniziali e ordinate erano ruotate (90° − r e verso invertito): ora in TORPEX i cacciatorpediniere
  vanno a nord-ovest come nel briefing (316–324°).
- Esiti, sempre con il giocatore passivo:
  - SCENE2C: 5 «evasione riuscita» → 7 «evasione misera»;
  - SCENE2B: 4 → 5 e 7, cioè l'Alfa perde comunque il 688, con un altro testo;
  - SCENE7B: 2 → 6 e 7 in tre semi;
  - SCENE8A: 4 → nessun esito entro l'ora in due semi;
  - SCENE9A: nessun esito → 8 in due semi;
  - SCENE9B: nessun esito → 6 in tutti i semi.
- Il convertitore colloca ora le navi in acqua di almeno pescaggio + 3 m, con lo stesso margine di ±400 m attorno.
  Prima una Spruance di SCENE1B e SCENE1C partiva sul ciglio di una secca e la toccava dopo 7 s con la rotta
  corretta.
- La base e' stata rifatta dopo la soglia dell'intercettazione (sopra).

`verifica_fondale` (24/10/2026, dopo la correzione degli angoli): nessun urto, nessuna unita' sulla terra. 11 partite
su 110 cambiano esito con il fondale, perche' rotte diverse portano a scontri diversi.

**Limiti:**
- le unita' dell'IA non lanciano armi diverse da quelle degli script;
- i sottomarini colpiti hanno solo l'esplosione, senza fisica (fase D6);
- l'abbandono nave non ha ancora zattere che si staccano dalla nave a galla, solo quelle dopo l'affondamento.

### Fondale e incagli

L'eseguibile carica il fondale del teatro della missione (`data/motore/teatri/<teatro>.fdl`) e i pescaggi
delle classi (`Missione::imposta_fondale`). Le posizioni delle missioni sono già in metri sulla proiezione del
teatro, quindi il fondale si legge direttamente (interpolazione bilineare fra i centri delle celle da 200 m;
fuori dalla griglia non c'è fondo). La **quota è quella della chiglia**; in superficie conta il pescaggio.

Fisica, uguale per tutti (navi e sottomarini; aerei esclusi):
- **orizzontale:** un passo che porterebbe la chiglia in un fondo più alto (e più alto di quello in cui si
  trova) non avviene: l'unità si incaglia, la velocità va a zero. Da una secca si può sempre uscire verso acqua
  più profonda; l'incaglio finisce a 50 m dal punto in cui è avvenuto;
- **verticale:** la chiglia si appoggia sul fondo (`sul_fondo`);
- **danni:** al primo contatto, integrità del modello × (v / 20 m/s)², con v la velocità vera (non quella del
  moto classico) o il rateo di discesa. Sotto 1 m/s nessun danno: 30 nodi contro la costa tolgono il 59%, una
  discesa a 5,5 m/s l'8%. L'originale toglie sempre 40 punti;
- **giocatore:** messaggio 30 «toccato il fondo, danni allo scafo» se lo scafo regge, altrimenti messaggio 9 ed
  **esito 1** («si è incagliato ed è andato perduto»), come l'originale, dove nessuno script lo imposta
  (`docs/originale/sensori_e_navigazione.md` §9);
- **armi** sott'acqua: non scendono sotto il fondo; sulla costa il siluro è perso (messaggio 38 se è del
  giocatore).

Il pilota dell'IA (`Missione::pilota`), assente nell'originale, dove le navi ignorano il fondale e passano sopra la
terra:
- **rotte:** verso una meta, se la via diretta incontra acqua bassa, rotta A* sulle celle in acque collegate
  (`nucleo/rotte.hpp`), tagliata a vista. Le acque collegate si calcolano una volta per soglia: acqua minima =
  pescaggio + 0,5 m; tra minima e comoda (pescaggio + 3 m per le navi, + 10 m per i sottomarini) il costo è
  quadruplo. Se la meta è in un'acqua non collegata (un fiordo chiuso da una soglia troppo bassa) l'unità va nel
  punto raggiungibile più vicino e si ferma;
- **aggiramento a vista** delle secche senza meta (rotta o timone dallo script), con scarti di 15°;
- **velocità di manovra:** 5 nodi mentre aggira una secca o accosta di oltre 45° su una rotta pianificata (come
  il messaggio 47 dell'originale, «Riduco la velocità per accostare»);
- **franco:** i sottomarini tengono la chiglia 25 piedi (7,6 m) sopra il fondo, qui e lungo la prua, come
  l'originale per le unità sotto il fondo; quando il fondo lo impone risalgono al rateo massimo del modello, e
  rallentano se non bastasse;
- **freno di emergenza:** macchine ferme se entro una volta e mezza lo spazio di frenata (più 200 m), dritto o
  lungo l'arco della virata, la chiglia incontrerebbe il fondo;
- **collocazione:** le unità che partono, o che gli script creano o spostano, in acqua meno che comoda o in una
  pozza chiusa (acque collegate sotto i 20 km²) vanno nell'acqua comoda più vicina (`Missione::spostamenti()`).

Verifica (23/10/2026): 22 scene × 5 semi × 60 minuti (110 partite), con e senza fondale:
- nessun urto dell'IA, nessun tocco e nessuna unità sulla terra;
- esiti identici;
- 16 unità collocate all'avvio o alla creazione;
- nel Baltico (SCENE0A) Alfa, Victor e Akula restano fra 13 e 39 m in 21–47 m d'acqua, invece di scendere a
  235–274 m.

La verifica si ripete con `engine/build/verifica_fondale [secondi] [semi]` (alcuni minuti).

Costi:
- un'ora di gioco costa 0,6–2,5 s di calcolo, contro 0,1 s senza fondale, quindi circa 45 ms per secondo reale
  a ×64;
- l'avvio della missione costa 0,2–0,4 s.

Prove:
- tutte le 22 scene girano 15 minuti senza errori né funzioni mancanti, anche sul fondale reale, dove l'IA non
  urta mai il fondo;
- TORPEX 30 minuti; determinismo con il seme; ambiente isolato;
- fondale sintetico: franco e aggiramento dell'IA, incaglio del giocatore con danni, discesa sul fondo, scafo
  che cede con esito 1.


### Quadro del fondale: ecoscandaglio, BQS-15 sul fondo, carta + misure (docs/24)

`docs/24_quadro_fondale.md` (approvato il 28/10/2026); dossier `docs/ricerca/06_fondali_e_navigazione.md` §1 e §7; prove
`ecoscandaglio:`, `quadro del fondale:` e `bqs15 sul fondo:`. Tre moduli puri del nucleo, solo per l'Enhanced; l'aggancio
alla missione e' del runtime, le viste della principale.
- **`nucleo/ecoscandaglio`:** `sonda(...)` da' un `Sondaggio`.
  - Un cono di 30 gradi: l'eco viene dal punto piu' vicino, quindi i rilievi si arrotondano e si anticipano. Un
    pinnacolo a 300 m di lato porta il fondo letto da 2000 a 1430 m.
  - Errore: la scala dell'apparato (1%) e un rumore.
  - Falsi fondi al 2%: di giorno lo strato diffondente, sopra i 20 kn la scia (10%).
  - Potenza piena fino a 6000 m o bassa, -20 dB, fino a 2000 m.
  - `emissione()` per l'intercettazione (`[emissioni.fathometer]`); `ProgrammaEcoscandaglio` per il ping a comando o
    periodico.
- **`nucleo/ritorni_fondo`:** `spazza_fondo(...)` da' il settore di prua del BQS-15: +-60 gradi ogni 7,5, ventaglio da
  -30 a +45.
  - Su ogni raggio l'eco piu' vicino e, se e' piu' in basso, anche la parete davanti alla chiglia (ostacolo).
  - Portate: pareti 3,2 km, fondo radente 1,5 km; -30% a 15 kn, -60% oltre i 20.
  - I raggi senza eco liberano il bordo basso del ventaglio.
  - Sovietici: MG-70 (Victor, Akula) e MG-24 (Alfa) con le portate ridotte. `bqs15_fondo_di_classe()` da' quello della
    classe.
- **`nucleo/quadro_fondale`:** la carta della parte piu' le misure, sulla griglia della carta.
  - La sigma della carta viene da spaziatura del rilievo e pendenza, o dalla zona non rilevata.
  - Le misure si fondono con un Kalman per cella. Nell'impronta vale il vincolo «almeno»; le misure invecchiano di 20
    m/h verso la carta.
  - `punto_nave()` distribuisce la correzione sulle misure dall'ultimo punto nave e rifa' il mosaico.
  - Letture: `leggi`, `celle_misurate`, `profilo` di prua, `sondaggi`, `ritorni_recenti`, `versione`, salvataggio.
  - `avvisi()` solo dalle misure: sotto la chiglia, fondo in salita, ostacolo a prua, velocita' oltre quella sicura.
  - Lo scenario del San Francisco: a 33 kn con un sondaggio ogni 15 minuti l'urto arriva prima del secondo
    sondaggio; con uno al minuto «fondo in salita» a 4,8 km dalla cima.
- **Dati:** `[ecoscandagli.fathometer]` e `[bqs15_fondo.*]` in `acustica.toml`, da `tools/pipeline/acustica.py`; quasi
  tutto STIMA, con la fonte.

### Carte d'epoca e georeferenziazione

**Centro della proiezione.** Ogni `.fdl` porta dopo la griglia il blocco `GEO1`, con il centro della proiezione
azimutale equidistante del teatro (lo stesso di `data/teatri/<teatro>/heightmap.json`). `Fondale::geografiche(Vec2)`
e `Fondale::proiettata(Geografiche)` passano dal piano a latitudine e longitudine WGS84 e ritorno, con i problemi
diretto e inverso di Vincenty dal centro: nella aeqd ellissoidica distanza e azimut geodetici dal centro sono quelli
del piano. Lo scarto da pyproj è sotto 1 cm a 230 km. Serve al reticolo della carta, anche con il «fondale vero».

**Carte d'epoca** (docs/08 §5): `data/motore/teatri/<teatro>_carta_<nato|urss>.fdl`, da
`tools/pipeline/carta_epoca.py` con i parametri di `tools/pipeline/carte_epoca.yaml` (tutti **stime**). È la carta
nautica di ciascuna parte, derivata dal fondale vero e degradata, con la stessa griglia: si carica con
`Fondale::carica(percorso_carta(dati, teatro, parte))`. In più ha:
- `campagna(p)`: la campagna di rilievo (anno, ente, spaziatura delle linee) o -1 dove la zona non è rilevata;
- `annotazioni()`: ED, PA, «profondità minore riportata», «acqua scolorita»;
- `edizione()`: l'anno dell'edizione della carta.

`Missione::imposta_fondale_carta(parte, carta)` tiene le due carte accanto al fondale vero.
`fondale_carta(parte)` restituisce la carta, o il fondale vero se non è impostata (partita con il «fondale vero»).
Simulazione, pilota dell'IA e incagli usano solo `fondale()`.

Come si genera (deterministico, seme da teatro e parte):
- campagne in blocchi di Voronoi di 20–50 nm per zona, con anno, ente, orientazione delle linee, spostamento e scarto
  della velocità del suono; secondo la copertura alcune restano non rilevate, e la fascia costiera avversaria
  (22 km) non si rileva mai;
- linee di rilievo con la spaziatura del teatro (fitta sulle soglie, rada nei bacini), campionate con un
  ecoscandaglio a cono, più sondaggi sparsi nelle zone non rilevate;
- ricostruzione TIN dalle posizioni sbagliate, con la costa vera a quota 0;
- generalizzazione con un passa-basso che tiene i bassi fondali;
- profondità in metri interi, terra vera a passi di 10 m;
- annotazioni per i rilievi veri spariti dalla carta.

Una cella è «non rilevata» se è lontana più di 1,5 volte la spaziatura da ogni sondaggio di linea.

Le anteprime vero | carta | differenza sono in `data/teatri/<teatro>/carta_<parte>_anteprima.png`. Le 14 carte
pesano 14,5 MB (il fondale vero dei 7 teatri ne pesa 11,8).

Esiti (24/10/2026), sul mare del teatro:

| Teatro | NATO: non rilevato / errore medio / annotazioni | URSS: non rilevato / errore medio / annotazioni |
|---|---|---|
| Baltico | 20% / 1,1 m / 0 | 0% / 1,1 m / 1 |
| Barents | 29% / 7,4 m / 15 | 0% / 4,2 m / 22 |
| Celtica | 0% / 77 m / 43 | 9% / 83 m / 25 |
| Faroe | 3% / 16 m / 43 | 13% / 16 m / 45 |
| Gibilterra | 0% / 16 m / 32 | 11% / 18 m / 52 |
| Kunashir | 30% / 67 m / 58 | 0% / 43 m / 60 |
| Otranto | 17% / 21 m / 53 | 16% / 25 m / 41 |

Nei teatri profondi (Celtica, Kunashir) l'errore medio viene soprattutto dallo scarto della velocità del suono sui
fondi oltre i 2000 m.

Prove in `test/prove_nucleo.cpp`:
- geografiche e proiettata contro pyproj in tre teatri;
- fondale vero e carte dei 7 teatri: stessa griglia, terra conservata, mare mai terra, maschera solo in mare;
- tabella invertita: Barents migliore per l'URSS, Faroe, Gibilterra e Celtica per la NATO, Kunashir e Baltico
  orientale per l'URSS;
- carta fatta a mano;
- `Missione::fondale_carta` con e senza carta, in `test/prove_danni_sottomarini.cpp`.

### Spaccato del quadro danni (F3 dell'Enhanced)

`grafica/spaccato.hpp`: lo spaccato del battello del giocatore con lo stato dei danni del kit e le squadre di
controllo danni (docs/10 §10.16). L'API è:
- `Spaccato(classe, Modello, DatiScafo)`;
- `imposta_palette(Palette&)`: rampe 96–143 e 199–247;
- `disegna` (tutto il battello), `disegna_dettaglio(x0, x1)` (un compartimento ingrandito), `disegna_didascalie`;
- `x_tela`, `compartimenti`, `estensione`;
- `StatoSpaccato::da(BattelloDanni, quota, assetto, tempo)`.

Anteprime 640×360 con `engine/build/anteprima_spaccato <dati> <classe> <integro|danneggiato|grave> <uscita.bmp>`.
Il costo è di 3–4 ms a fotogramma dopo la prima volta.

### Effetti dell'ASW aereo (A4 di docs/13)

`grafica/effetti.hpp` riceve lo stato dalla missione e non dipende dal controllore dell'IA.
- **A ogni passo:** `aerei_asw(vector<AereoAsw>)` con posizione, quota, rotta, velocità, hover, diametro del
  rotore, `cavo_m` del sonar a immersione, `mad_m` del MAD rimorchiato, apertura e luci; `boe_asw(vector<Sonoboa>,
  t_missione)`.
- **Una volta sola:** `marcatore_asw(Mk 25 | Mk 58 | OMAB giorno | OMAB notte, pos, durata)` e
  `arma_paracadute(pos, quota, rotta, velocità, sovietica)`, che restituisce i secondi fino al tonfo.

`avanza` produce gli effetti:
- l'anello di spruzzi sotto l'hover: forte per l'SH-3H a 12 m, più debole per il Ka-27 a 30 m;
- il cavo e il trasduttore, col tonfo quando entra in acqua e l'acqua che cola quando esce;
- il «bird» del MAD, che pende sotto l'elicottero fermo e resta indietro in volo;
- le luci: lampeggiante, rossa a sinistra, verde a dritta, bianca in coda, spente in caccia;
- per le boe: espulsione, discesa col paracadute, tonfo, colorante (boe USA), galleggiante con l'antenna fino a
  `t_fine`;
- per i siluri: il paracadute, il tonfo e il paracadute che resta sull'acqua;
- per i marcatori: le fiamme e il fumo, o la fluoresceina.

Le cose che durano stanno in `particelle()`, quelle rifatte a ogni passo in `orli()`; il periscopio le disegna già
entrambe. Le materie nuove sono `colorante`, `luce` (indici EGA, accese anche di notte) e `cavo` (filo verticale).

Nel periscopio un'unità con `Nave::quota < 0` è in volo a −quota metri. Viene disegnata all'altezza vera, con lo
sprite dell'elevazione più vicina (e00, e10 o e25); gli aerei del classico restano a quota 0. Le icone delle
minicartoline sono quattro: `Icona::aereo`, `boa`, `elicottero`, `marcatore`; i nomi del registro sono
`aereo_in_sorvolo`, `boa_vicina`, `ping_sonar_immersione`, `marcatore_avvistato`.

Anteprime con `anteprima_cerimonia <dati> <asw|asw2|asw_notte|icone> <uscita.bmp>`.

### Sagome delle armi e sezione del tubo

`grafica/sagome_armi.hpp` disegna le armi di `carico.toml` di fianco, in scala, con la prua a destra. Le fonti
dell'aspetto sono in docs/06_kit_navi.md, «Sagome delle armi e dei tubi». L'API è:
- `sagoma_da_scheda(SchedaArma, esercitazione)` e `lunghezza_disegnata`: le capsule sono più lunghe del
  missile;
- `disegna_sagoma_arma(Tela, SagomaArma, Rettangolo, px_m = 0, tavolozza)`: con `px_m = 0` usa la scala più
  grande che entra nel rettangolo;
- `disegna_tubo(Tela, Rettangolo, SagomaInTubo)`: il tubo con lo stato, cioè vuoto, ricarica (l'arma scorre
  dentro secondo `ricarica`), carico, armamento o armato. Mostra anche il tubo allagato, la porta esterna
  aperta e il segno della mina pronta alla posa. `geometria_tubo` dice dove sta l'arma;
- `traccia_sagoma_arma(..., PuntoSagoma)`: materiale e tono per pixel; lo usa lo spaccato, che riceve il carico
  delle rastrelliere in `StatoSpaccato::rastrelliera`;
- `TavolozzaSagome::fissa()` per il pannello Armi in missione (indici che la vista non tocca);
  `TavolozzaSagome::ricca(Palette&, 96)` per le schermate a tutto schermo.

Anteprime con `engine/build/anteprima_sagome <dati> <armi|esercitazione|tubi|pannello> <uscita.bmp>`. Per le
rastrelliere nello spaccato: `anteprima_spaccato <dati> <classe> integro <uscita.bmp> 0 minamento`.

### Grafica attorno alla missione (M4 della Fase 4a)

Ritratto, medaglie, cornici e minicartoline delle schermate di docs/12, in stile VGA a 256 colori come i simulatori
MicroProse. La grafica soltanto: le schermate e il flusso (M5) sono dell'app.

**Palette**:
- cerimonia e fascicolo coprono la vista: `cerimonia::imposta_palette(p, marina)` e `prepara_palette_ritratto`
  ridefiniscono 16–79, 96–143 e 199–247 (rampe di pelle, capelli, uniformi, metalli, smalti, carta, legno, velluto,
  nastrini);
- il debriefing tiene la palette della vista: cornici, timbri e icone nel modo `vista` usano solo 0–15, 80–95 e
  144–158.

**Decorazioni** (`grafica/decorazioni.hpp`, `TavolaDecorazioni::carica(regole, "decorazioni_grafica.toml")`):
- una sola fonte per id, nomi, precedenza e colori dei nastrini, cioè `carriera.toml` della M3 (`RegoleCarriera`);
- la parte grafica (forma, metallo, classe, a vite) è in `data/motore/decorazioni_grafica.toml`, scritto a mano, con
  gli stessi id (una prova controlla che coincidano);
- nastrini con i dispositivi (stelline d'oro e d'argento, «V», «E»);
- barretta dalle `VoceBarretta` della carriera, in ordine di precedenza, con la fila incompleta in alto e i distintivi
  saltati;
- medaglie in rilievo a distanze con segno: drappo USA, pentagono sovietico, ordini a vite;
- distintivi in grande e attestati incorniciati.

**Ritratto** (`grafica/ritratto.hpp`):
- testa e petto, con la testa in 3D a distanze con segno: lineamenti dal seme, invecchiamento, capelli, baffi,
  carnagione, occhi, berretto USN o VMF;
- luce da studio con ombre morbide e occlusione, ridotta alle rampe con la retinatura di Bayer e un filo scuro attorno;
- uniforme dipinta: Service Dress Blue o kaki con i gradi al colletto, tuzurka con le controspalline, dolphins,
  barretta, Command at Sea, rombo e «Comandante di sottomarino»;
- `disegna_insegna_grado` disegna in grande i galloni del polsino (USN) o la controspallina (VMF), per la promozione;
- costo: 0,36 s per 236 × 310 e 0,23 s per 176 × 232, grazie al campionamento adattivo sui bordi, ai volumi di
  contenimento dei lineamenti e alla simmetria. Si disegna una volta per schermata.

**Cornici** (`grafica/cornici.hpp`): console, oro, rossa, legno; pannelli, pulsanti, carta del fascicolo,
intestazione con gli emblemi; timbri ruotati e consumati (CLASSIFIED, CONFIDENTIAL, СЕКРЕТНО scritto CEKPETHO).

**Minicartoline** (`grafica/cartolina.hpp`):
- `dal_registro(soggetto, secondo, distanza, elevazione, azimut, tempo)` traduce il suggerimento di
  `EventoMissione::inquadratura` in un `PuntoDiVista`;
- `disegna_cartolina` (riquadro 128 × 96) ridisegna il `Mondo` all'istante dell'evento con il periscopio (6x a pelo
  d'acqua, 1,5x dall'alto), gli effetti e i modelli danneggiati;
- se ne prende il centro attorno al bersaglio e si riduce di 2 col colore più frequente;
- le icone d'epoca hanno i nomi del registro (`icona_da_nome`): cascata, sonar, radio, esm, radiogoniometro, danni,
  zona.

**Esiti** (fase 3, dossier 15 §7.8-7.9):
- `disegna_equipaggio` (`grafica/ritratto.hpp`): ritratto di gruppo per le decorazioni di reparto. Tre file sovrapposte
  davanti alla vela col numero, gli ufficiali in prima fila col comandante in mezzo; marinai USN col berretto bianco e
  il colletto alla marinara, VMF con la bustina e la maglia a righe; teste in 3D ognuna col suo volto.
  Cambia con la missione: i feriti con la benda o il braccio al collo, i caduti mancano e un nastro di lutto
  attraversa l'angolo.
- `disegna_giornale` (`grafica/giornale.hpp`): pagina d'epoca (USA: testata, titolo, foto a retino, testo in colonne,
  altri articoli) o prima pagina sovietica col comunicato TASS in riquadro. Colonne secondo la larghezza.
- `disegna_tabella`: elenco datato del fascicolo (affondamenti e contatti classificati) con le righe alternate.

**Anteprime**: `engine/build/anteprima_cerimonia <dati> <cosa> <uscita.bmp>`, con `cosa` fra medaglie,
medaglie_urss, nastrini, ritratti, ritratto_grande, teste, cerimonia_usn, cerimonia_vmf, promozione, fascicolo,
debriefing, equipaggio, giornale, fascicolo_elenco e tempo. Le immagini sono in `extracted/navi/_verifica/cerimonia/`.

Prove in `test/prove_cerimonia.cpp`:
- id uguali nei due file, righe da 36, ordine di precedenza;
- barretta con le file e i dispositivi;
- ogni medaglia nel suo riquadro;
- ritratto deterministico e sotto il tempo, con uniformi e insegne;
- modo vista delle cornici;
- minicartolina dal registro e icone.

## Dinamica del sottomarino dell'Enhanced (fase C6a)

`nucleo/dinamica_sottomarino.hpp`: il moto dei sottomarini con la regola di simulazione (docs/11 §3.1 e §10; numeri
dai dossier 01 §2 e §8.1 e 02 §2.3). Il livello classico non lo usa. Prove in `test/prove_dinamica_sottomarino.cpp`.
- **Avanzo:** spinta dell'elica dai giri (frazione dei massimi, da -0,5 a 1; la turbina li insegue in una ventina di
  secondi) contro la resistenza, tarata dalla potenza all'asse e dalla velocita' massima. A giri costanti la
  velocita' di regime e' giri x massima; il 688 va da fermo a 20 nodi in poco meno di due minuti.
- **Timone:** angolo di barra fino a 35° (5°/s) oppure «accostare per» una rotta con una barra massima. Il rateo di
  virata insegue velocita' / raggio (Nomoto del primo ordine): diametro tattico a barra tutta 5,1 lunghezze per il
  688 (561 m), 3,3 per l'Alfa, 6 per Oscar e Typhoon. In accostata con la barra tutta la velocita' cala a circa
  due terzi.
- **Quota e assetto:** quota ordinata (l'assetto la insegue entro l'angolo massimo, 15° di dottrina) oppure angolo
  di immersione. I piani hanno autorita' col quadrato della velocita': a battello fermo non governano.
  dz/dt = -v sin(assetto) + w, con w = `rateo_verticale_regime` del battello a velocita' 0 (lo stesso modello del
  kit, senza la sua portanza: qui la portanza e' l'assetto, che il timoniere alza quando il battello e' pesante).
  Con 100 t d'acqua il 688 a 15 nodi tiene la quota con circa 11° di prua in su; fermo affonda a 1,5 m/s.
- **Battello danneggiato:** peso netto e momento d'assetto da `BattelloDanni`; i piani compensano il momento fino a
  15° di raddrizzamento a 8 m/s. Timone e idroplani in avaria e la velocita' massima da `LimitiBattello`.
- **Cavitazione:** `cavita()` quando i giri, in nodi equivalenti, superano V_inc(z) = V_inc0 sqrt((10,33 + z)/10,33);
  V_inc0 per classe (688 5 nodi, Alfa 4, Trafalgar con il pump-jet 5,5).

**Nella missione:** `DanniMissione` con la regola di simulazione chiama `Missione::dinamica_sottomarini(classi,
cartella)`. Da li' i sottomarini con lo scafo della loro classe si muovono con la dinamica:
- il giocatore con `Missione::comandi_sottomarino(id, ComandiSottomarino)`;
- l'IA con un timoniere che insegue rotta, velocita' e quota ordinate dagli script e dal pilota del fondale (una
  virata con il rateo diventa barra in proporzione, fino alla rotta ordinata).

Lo stato si legge con `Missione::dinamica(id)`. Peso netto, momento e governi in avaria arrivano da
`OrdineMotore` (`peso_netto_N`, `momento_assetto_Nm`, `timone`, `idroplani`); finche' DanniSottomarini non li
riempie, la quota di un sottomarino allagato segue ancora il suo rateo. Rilievo classico identico alla base delle
17:00.

## Siluri dell'Enhanced (fase C6b)

`nucleo/siluro.hpp`: il modello dei siluri per la regola di simulazione. Il livello classico non lo usa. Il modulo
è puro: a ogni passo riceve le sorgenti attorno (unità, esche) e dà lo stato e gli eventi. Prove in
`test/prove_siluro.cpp`.

**Impostazioni prima del lancio** (`ImpostazioniSiluro`, dossier 02 §6.2):
- rotta di corsa;
- modo della testa: passivo, attivo o misto, cioè passivo in ricerca e attivo per l'aggancio;
- velocità bassa o alta;
- RTE (distanza di corsa prima dell'attivazione);
- quota di corsa e quota di ricerca;
- soffitto e pavimento;
- schema di ricerca: serpentina, cerchio o dritto;
- anti-circolare.

**Fasi:** corsa → ricerca (serpentina di ±30° con periodo di 60 s, oppure cerchio) → verifica (4 s) → inseguimento
con anticipo (punto futuro del bersaglio, al più 30 s) → riacquisizione a cerchio dopo 6 s senza contatto. Il siluro
finisce spento, esaurito (fine dell'autonomia alla velocità del momento) o esploso (spoletta di prossimità, armata
dopo 500 m o dopo l'RTE se è più corto). All'aggancio passa all'alta velocità.

**Testa** (STIMA DI GIOCO: le fonti aperte non danno numeri):
- portata passiva = portata della classe × 10^((livello − 130 dB)/20), ridotta a 0,6 in alta velocità per il
  rumore proprio;
- portata attiva = portata della classe × 10^((TS − 15 dB)/40);
- uno strato fra siluro e sorgente dimezza la portata;
- cono di ±40–50° attorno alla prua;
- la testa ignora cio' che sta fuori dalla fascia fra soffitto e pavimento e, con l'anti-circolare, il lanciatore;
- in passivo sceglie la sorgente più forte, in attivo l'eco più forte.

**Esche** (ADC Mk 2, dossier 05 §5.1 e §5.3): ferma alla quota impostata, 150 dB, eco puntuale (TS 10 dB),
10 minuti (le fonti danno 6–45). Nella verifica:
- il passivo la rifiuta con probabilità 0,1, quindi la seduzione è forte;
- l'attivo con 0,5 per il Doppler zero, più 0,2 per l'eco puntuale;
- il segui-scia non la vede.

L'operatore via filo può rifiutare una sorgente o assegnarne una.

**Anti-circolare:** dopo l'attivazione, se il siluro punta il lanciatore (entro ±45°) a meno di 3 km, si spegne.

**Filo** (STIMA):
- 30 km svolti dal siluro e 3 km dal battello per il Mk 48;
- si rompe se il lanciatore supera 12 kn o 2 °/s di virata per più di 5 s, se un rocchetto finisce o se il
  lanciatore sparisce.

Comandi: rotta, quota, velocità, modo, abilita, rifiuta, assegna, spegni (si ferma e affonda a 1 m/s senza esplodere),
taglia. Lungo il filo tornano i rilevamenti della testa: la distanza solo con l'attivo, più l'indicazione di
sorgente ferma (sospetta esca).

**Classi** (dossier 01 §6.1–6.2) in `data/motore/siluri.toml`, generato da `tools/pipeline/siluri.py` (che lo chiama
`esporta_motore.py`) e letto da `carica_siluri()`. Dove le fonti discordano si sceglie un valore, scritto nella nota della
classe:

| Classe | Alta / bassa | Portata alta / bassa | Quota max | Scelte |
|---|---|---|---|---|
| `mk48` Mk 48 Mod 4 | 55 / 40 kn | 38 / 50 km | 760 m | ufficiale «>28 kn, >5 mi», «>1.200 ft»: si usano le stime della letteratura (Almanac of Seapower 1987, Jane's); quota stimata 760–800 m, si prende 760 |
| `mk48_adcap` | 60 / 40 kn | 32 / 50 km | 800 m | 55–65 kn nelle fonti: 60; 27–38 km in alta: 32. Marginale nel 1989 (IOC 1988) |
| `tigerfish` Mk 24 | 35 / 24 kn | 17 / 33 km | 442 m | 13–21 km e 27–39 km nelle fonti: i valori medi |
| `spearfish` | 60 / 30 kn | 21 / 56 km | 600 m | in servizio dal 1992–94: solo scenari what-if; la bassa velocità e la quota sono STIME |
| `uset80` USET-80 | 45 / 35 kn | 18 / 24 km | 1.000 m | 43–50 kn e 18–20 km; quota 1.000 m (fonte russa) contro «>400 m» (navweaps): si prende 1.000. È il «Type 53» antisom dell'originale |
| `test71m` TEST-71M | 40 / 24 kn | 15 / 20 km | 400 m | «35 kn d'attacco, 24 kn × 15 km» (RU wiki) contro 40 kn × 15 km: si prende 40 |
| `type53_65k` 53-65K | 45 kn | 19 km | 100 m | segui-scia antinave, immune alle esche, inutile contro sottomarini immersi |
| `mk46` Mk 46 Mod 5 | 45 kn | 11 km | 450 m | leggero, alta confidenza |
| `at1` AT-1M | 27 kn | 5 km | 200 m | Ka-25, acquisizione 500 m |

**Nella missione** (`Missione::siluri_enhanced(cartella_dati)`, che legge `siluri.toml` alla prima chiamata, acceso da
`DanniMissione` con la regola di simulazione; senza il file i siluri restano classici e l'errore va in `errori()`; prove
`siluro nella missione:`). Il livello classico non cambia:
- i siluri creati da `nuova_unita` (armi con flag 20h, non missili né esche) passano al modello se la classe è
  nella tabella. Le classi delle missioni: `mk48` → Mk 48, `type53` → USET-80 (il Type 53 antisom); la Spearfish
  resta alle missioni che la hanno già;
- gli script classici dell'arma non girano più: fine corsa, taglio del filo a tempo (LINK 0), impulsi, reazioni.
  `togli_unita` e `collega(0)` su un siluro del modello sono ignorati;
- impostazioni: quelle di `imposta_siluro_giocatore` per il prossimo siluro del giocatore (le dà `StazioniMissione`
  dal tubo che parte); senza, e per l'IA, come LAUNCH: rotta sul bersaglio memorizzato, testa mista, alta
  velocità, RTE al 60% della distanza, ricerca alla quota del bersaglio, corsa alla quota del lanciatore;
- sorgenti per la testa: tutte le unità, tranne aerei e altre armi. Livello STIMATO dal rumore dell'originale:
  105 + 2,5 × rumore + 0,5 dB per nodo oltre i 5 (130 dB per il rumore 10). Eco: 15 dB per i sottomarini,
  25 dB per le navi. I falsi bersagli (flag 80h) sono esche per la testa: l'esca lanciata (che LAUNCH fa arma) a
  150 dB come l'ADC Mk 2, i biologici col loro rumore. L'esplosione su un falso bersaglio non colpisce;
- tempo: il modello avanza di `dt × m_s_per_nodo / NODO`, così corre con il moto classico del mondo;
- esplosione: `colpisci()` sulla sorgente della spoletta, con la regola dei colpi del motore. Il `Colpo` ha la geometria
  vera (`geometria_vera`, `distanza_m` dal centro della vittima, `dz_m` positiva sotto lo scafo) per la fisica dei danni.
  La spoletta scatta entro il suo raggio più 5 m dal centro di un sottomarino, 10 m da una nave. Su un'esca o a vuoto
  l'arma esce senza colpo. Spento ed esaurito escono dal gioco. Sulla costa il siluro è perso come nel classico;
- filo: `collegata` torna a -1 quando il filo si rompe o si taglia. I vincoli usano velocità e virata del
  lanciatore;
- `eventi_siluri()`: gli eventi del modello con il siluro e il lanciatore (tempo di missione), per i messaggi
  e gli avvisi dell'interfaccia.

## Acustica dell'Enhanced (Fase 2)

`nucleo/acustica.hpp` (dossier `docs/ricerca/02_acustica_oceanografia_dottrina.md` §1–4 e §10, dossier 01 §3; prove
`acustica:`). Il modulo è puro e il livello classico non lo usa: l'integrazione nel runtime è della sessione sul
runtime.

**Equazione del sonar.** Livelli spettrali (per Hz) alla frequenza della banda e soglia DT come S/N nella banda,
quindi `SE = SL − TL − (NL − DI) − DT` in passivo e `SE = SL − 2·TL + TS − max(NL_banda − DI, RL) − DT` in attivo.
`valuta(sorgente, ricevitore, ambiente)` prova le bande del sensore e tiene la migliore. Restituisce SE, i termini
dell'equazione, il percorso e due avvisi: sorgente nei baffles, sensore sordo per la velocità.
`probabilita_rilevazione(se, σ) = Φ(SE/σ)`. `Fluttuazione` è un processo AR(1) per coppia, perché i contatti non
lampeggino.
- DT passivo: `5·log(d/(W·t))` con W = 0,7·f (un'ottava), d = 14 (Pfa 10⁻⁴, Pd 0,5) e t del sensore. DT attivo:
  `10·log(d/2)` in banda 1/τ. Più l'abilità dell'operatore (±3 dB).

**Perdita di trasmissione** (livello A del §10.2, analitico, senza ray tracing). Somma incoerente dei percorsi:
- **diretto:** sferica e assorbimento (Ainslie-McColm); specchio di Lloyd con la coerenza di Eckart per il mare mosso;
  penalità d'attraversamento del layer da 5 a 20 dB, crescente con il gradiente sotto lo strato e ridotta sotto il
  taglio del condotto; zona d'ombra oltre la portata del raggio limite, fino a +25 dB;
- **condotto superficiale:** sorgente e ricevitore nello strato, sopra la frequenza di taglio; cilindrica oltre r₀ =
  salto/6, con la perdita per salto che cresce con lo stato del mare;
- **zone di convergenza:** guadagno gaussiano di 8 dB alla distanza calcolata dal profilo, 4 dB in meno per ogni zona
  successiva, solo con l'eccesso di profondità di almeno 200 m. Se fra il vertice e la profondità critica ci sono meno
  di 800 m il profilo è un semicanale (alte latitudini), non una CZ;
- **rimbalzo sul fondo:** raggi oltre 10°, perdita per riflessione secondo il fondo (roccia, sabbia, fango);
- **acqua bassa** (sotto i 200 m): Marsh-Schulkin al posto degli altri percorsi.

**Profili** (`data/motore/teatri/<teatro>.suono.toml`, generati da `tools/pipeline/suono.py`): 12 profili mensili
mediani WOA23 di ogni teatro fino a 1500 m, prolungati con la stagione WOA (fino a 5500 m) e poi con la pressione. Il
profilo ricava layer, gradienti, asse, profondità critica e distanza della prima CZ. Valori ottenuti:
- Mar di Norvegia (Faroe) d'inverno: CZ a 51 km; il dossier stima 59;
- Mediterraneo (Otranto) d'estate: CZ a 39,5 km con l'asse a 95 m; il dossier stima 41;
- Mediterraneo d'inverno, Barents, Baltico e Curili d'inverno: nessuna CZ.

**Rumore ambiente:** Coates/Stojanovic (turbolenza, traffico del teatro, vento limitato a 15 m/s, termico), più
pioggia, gamberi schioccatori e ghiaccio (pack quieto, margine rumoroso).

**Rumore irradiato** (`data/motore/acustica.toml`, generato da `tools/pipeline/acustica.py`):
- livello a banda larga 5–200 Hz per velocità dalla tabella del dossier 01 §3.2, interpolato in log v;
- spettro: −20 dB per Hz a 1 kHz e −20 dB/decade sopra 200 Hz;
- cavitazione: l'insorgenza cresce con la profondità (`v·sqrt((10,33 + z)/10,33)`); sopra, +10 dB a gradino sopra
  100 Hz e +40·log(v/v_cav);
- snorkel: +22,5 dB per Foxtrot e Kilo;
- forza di bersaglio per aspetto: 10 dB in meno sui battelli anecoici (Victor III, Alfa, Akula).
Il dossier 02 §2.6 dà numeri più alti in un'altra banda, 100 Hz–10 kHz: si è seguito l'01, che ha i punti per velocità.

**Sensori:**
- sfera BQQ-5 in passivo fino a 1 kHz e in attivo a 3,5 kHz;
- schiera TB-16 (73 m) da 50 Hz a 1 kHz, sorda oltre 17 kn;
- sonar sovietici di 2ª e 3ª generazione;
- SQS-53 e SQS-56, Titan-2.
Rumore proprio: macchinario o flusso, `60·log(V/V₀)`. Baffles: nel settore cieco a poppa il rumore proprio sale di
15 dB e i fasci perdono il guadagno d'antenna (STIMA). Tutti i valori di taratura sono STIMA DI GIOCO (dossier 02 §10.6).

**Ordine di grandezza** (Faroe d'inverno, schiera TB-16 a 5 kn e 150 m):
- Akula o 688 a 5 kn: circa 3 km;
- Victor III: circa 12 km;
- Akula a 15 kn: circa 26 km;
- Charlie II: circa 50 km;
- un mercantile: oltre 100 km.
Le portate sono al traverso: verso prua la schiera è cieca.

**Per il runtime:**
- `sorgente_acustica(catalogo, UnitaAcustica)` costruisce la `Sorgente` di un'unità, la stessa per i sensori e per le
  teste dei siluri. La firma si sceglie così:
  - la classe; `type53` diventa USET-80;
  - un'esca lanciata (arma con flag 80h) prende `adc_mk2`;
  - i biologici prendono `biologico`;
  - altrimenti la firma generica della categoria;
  - gli aerei restano senza firma.
- `sorgente_per_siluri(sorgente)` dà la `SorgenteAcustica` della testa, con il livello a banda larga più 18 dB. Così
  il 688 silenzioso sta a 130 dB, come nello stimatore della C6b.
- `CatalogoAcustico::sensori_di(classe)` dà i sensori di bordo, dalla tabella `[suite]`.
- `errore_rilevamento_deg(rilevazione, sensore)` è θ₃dB/√SNR, con almeno 0,2°; verso l'endfire della schiera il fascio
  si allarga. `Rilevazione::conico` segnala l'ambiguità della schiera.
- Schiera trainata: cieca (−30 dB) oltre 0,3 °/s di virata e nell'endfire verso il battello (±25°); dopo la virata si
  raddrizza in 2 minuti (−10 dB a calare). Il runtime passa `rateo_virata_deg_s` e `tempo_da_virata_s`.
- Posato sul fondo: −10 dB, e l'eco attiva si confonde con la riverberazione del fondo (Lambert); il ricevitore
  posato non ha rumore di flusso.
- Ping: una `Sorgente` con `ping_db` è l'impulso da intercettare, ascoltato con i sensori `wlr9` e
  `intercettazione_sov` (contro-scoperta).
- La cavitazione si prende dalla dinamica (`Sorgente::cavitazione`) quando c'è, altrimenti da `cavita(firma, v, quota)`.
  L'insorgenza in superficie della firma è la tabella della dinamica della C6a (dossier 02 §2.3).
- Teatro e mese: il file è `file_clima(dati, teatro)`; le missioni non hanno una data, quindi si usa
  `ClimaTeatro::mese_predefinito`: inverno nei mari del nord, agosto nel Mediterraneo.

**LOFAR** (dossier 02 §1.2, §2.7): i sensori con `lofar_bin_hz` ascoltano anche la tonale più forte della firma.
- Il livello assoluto della riga viene dal 02 §2.6. La frequenza è la quinta armonica della rete di bordo: 300 Hz per le
  unità occidentali, 250 Hz per le sovietiche (scelta di gioco).
- Soglia nel bin: `5·log(d/(b·T))`. Il rumore si integra nel bin, quindi `SE = L − TL − (NL + 10·log b − DI) − DT`.
- TB-16: b = 0,05 Hz, T = 120 s. Sfera BQQ-5: 0,1 Hz e 60 s. Sovietici di 3ª generazione: 0,2 Hz e 60 s.
- `Rilevazione::se_tonale_db` è l'indizio per la classificazione: con +3 dB le righe si leggono. `lofar` è vero se la
  scoperta viene dalla tonale.
- Effetto, con la TB-16 a 5 kn alle Faroe d'inverno: un 688 silenzioso passa da 3,4 a 6,1 km, un Akula da 3,4 a 4,1, un
  Charlie II da 52 a 70 km. La sfera ne guadagna poco.

**Effetto pomeriggio** (dossier 02 §3.5):
- `intensita_pomeriggio(ora, vento, nubi)`: sale dalle 10 alle 14, è piena fino alle 18 e torna a zero alle 22. È
  nulla con il vento da 11 kn in su o con le nubi oltre il 50%.
- `applica_pomeriggio(profilo, intensita)` mette nei primi 10 m un microtermoclino di 1,5 °C (per l'intensità 1). Il
  runtime tiene da parte il profilo modificato e lo rifà quando l'intensità cambia.
- Nel livello A il microstrato riconosciuto da `analizza()` toglie fino a 9 dB al condotto e al diretto, per chi sta
  entro 5 volte il suo spessore. Il tracciamento dei raggi lo vede dal profilo.

**Tracciamento dei raggi** (livello B del §10.2):
- `calcola_campo(zs, ambiente)` fa partire 101 raggi fra −30° e +30°, con passo d'arco di 100 m (RK2), fino a 100 km.
  La griglia ha passi di 500 m e 10 m e va fino al fondo o a 1000 m.
- Sui raggi: riflessione in superficie con la coerenza di Eckart e sul fondo con la perdita del tipo di fondo, a 1 kHz.
- L'intensità viene da fasci gaussiani larghi quanto il tubo fra i raggi vicini (quote svolte attraverso le
  riflessioni), più le immagini al contorno. Dove non arriva nessun raggio, la perdita è al più la sferica più 25 dB.
- `CampoPerdita::tl(f, r, zr)` aggiunge l'assorbimento alla frequenza.
- `CacheCampi` tiene un campo per sorgente e lo rifà oltre 3 nm di spostamento, 30 m di quota, o con un cambio
  d'ambiente: profilo, fondale del 10%, fondo, stato del mare.
- `valuta(s, r, amb, cache)` usa il campo nella griglia e il livello A fuori. Nell'attivo vale la reciprocità.
- Il calcolo resta nello stesso passo, senza thread, così le partite si ripetono.
- `CacheCampi::limite_per_passo` (predefinito 1) limita i calcoli fra due `nuovo_passo()`. Oltre il limite resta il
  campo vecchio, oppure il livello A se il campo non c'è ancora. Il limite dipende solo dallo stato della partita.
- Costo, su uno Xeon E5-2696 v2 a 2,5 GHz con -O2: 57–66 ms per campo in acqua profonda, circa 21 ms nel Barents.
  La variante con 51 raggi e passi da 200 m e 1 km costa 22 ms, con uno scarto medio di 3,2 dB.
- Caso peggiore: 10 sorgenti a 30 kn con la compressione 64×. Ognuna supera le 3 nm ogni 5,6 s veri (un siluro a
  55 kn ogni 3 s), quindi servono circa 2 campi al secondo: 110–130 ms di calcolo per secondo vero, a scatti di 60 ms.
  A 64× un fotogramma contiene una decina di passi, quindi il limite per passo non basta a spalmare i calcoli.
- `CacheCampiAsincrona` è la memoria da usare nel gioco:
  - ogni richiesta parte in un thread con una copia del profilo e dell'ambiente;
  - il campo nuovo entra in vigore a un istante fisso del gioco, 10 s dopo la richiesta (`ritardo_s`); se a
    quell'istante il thread non ha finito, il thread principale lo aspetta;
  - prima del primo campo di una sorgente vale il livello A;
  - il campo in uso dipende solo dal tempo di gioco, non dalla macchina né dalla compressione del tempo, quindi i
    replay restano identici;
  - le richieste non bloccano: con la compressione alta i calcoli si sovrappongono nei thread.
  Si usa con `valuta(s, r, amb, cache, tempo_s)`. È provata anche con ThreadSanitizer.
- Rispetto al livello A trova le cose che le formule perdono: nel Mediterraneo d'estate il canale sotto il termoclino
  (asse a 95 m) e l'ombra dopo 1 km per le sorgenti a 20 m. Resta un modello a raggi incoerente: niente specchio di
  Lloyd e niente taglio in frequenza del condotto.

### Acustica nella missione (regola di simulazione)

`DanniMissione` con la regola di simulazione chiama `Missione::acustica_enhanced(cartella)`: carica `acustica.toml` e il
clima del teatro (`teatri/<teatro>.suono.toml`; mese da `imposta_mese`, altrimenti quello predefinito del teatro).
Senza i dati resta la scoperta dell'originale. Prove in `test/prove_acustica_missione.cpp`. Anche le teste dei siluri
(C6b) ascoltano lo stesso livello delle unita' (`sorgente_per_siluri`). Rilievo classico identico alla base.
- **Raggi:** la perdita di trasmissione dei sensori viene dai campi dei raggi delle sorgenti (`CacheCampiAsincrona`
  del modulo, una nella Missione): calcolati in un thread e in vigore 10 s di gioco dopo la richiesta, quindi le
  partite si ripetono; prima del primo campo il livello A. Le unita' uscite si tolgono dalla cache.
- **Paesaggio sonoro (S5c):** le sorgenti della libreria (balene, pesci, ghiaccio, pescherecci...) nel disco di 100
  km attorno al giocatore, col mese del clima del teatro e senza ora; ripopolate con id e semi nuovi oltre 50 km di
  spostamento; mosse con la scala del moto della missione; nella `scena_acustica` della stazione e, ascoltate ogni 2
  s col livello A, in `contatti_paesaggio()` (id da 100000, niente posizione, classificazione fino a «classificato»,
  avviso 346 per i biologici). `usa_paesaggio(false)` lo spegne (prove con la scena pulita).
- **Cadenza invariata:** un osservatore per passo, TYPE, orfani e coppia degli impatti come nel classico (script e IA
  dipendono da quei tempi). Cambia solo come `valuta` decide la scoperta, per gli osservatori con sonar (la suite
  della classe; le navi senza suite col sonar del loro schieramento). Armi, aerei ed esche osservano come nel classico.
- **Segnale:** il migliore fra i sensori d'ascolto (la schiera solo filata, penalizzata in virata dal modulo) piu' la
  fluttuazione AR(1) della coppia (tau 5 minuti, sigma 4 dB). Armi proprie, radar e vista contano come +30 dB con la
  posizione; l'intercettazione ESM come +5 dB, solo rilevamento.
- **Contatto:** i campi del classico restano (script, IA e interfaccia li leggono). La qualita' integra l'eccesso di
  segnale nel tempo (0,55 per dB e per secondo: +10 dB stabili danno la traccia solida in circa due minuti e
  mezzo), scende con l'eccesso negativo e a zero il contatto si perde (script +56 come prima). Il lampeggio e'
  acceso con l'eccesso negativo. La durata (classificazione) cresce sopra +3 dB; un'esca non arriva alla classe.
- **Rilevamento e posizione:** `Contatto::rilevamento` e' il rilevamento misurato con l'errore del sensore (e
  `rilevamento_specchio` per l'ambiguita' della schiera); `Contatto::sensore` e `Contatto::eccesso_db` dicono chi ha
  dato l'ultima scoperta e con quanto margine. `Contatto::posizione_nota` dice se la posizione stimata
  vale: nel classico con la traccia solida, nell'Enhanced per il giocatore solo da eco, vista e radar (col solo
  passivo la X sulla carta), per l'IA anche dalla traccia solida con un errore che cala con la durata.
- **Ping** (`impulso_sonar_enhanced`, dal comando del giocatore e dallo script PING): eco su chi e' nel fascio, con
  distanza e rilevamento; poi l'intercettazione di chi ascolta (WLR-9 e simili), che ha un contatto sul pinger.
- **Avvisi al giocatore** in `Missione::messaggi_sonar()` (non in `messaggi()`, dove i codici da 100 sono esiti):
  310 nuovo contatto, 311 siluro in acqua, 312 ci stanno pingando, 313 stiamo cavitando, 314 schiera cieca in virata;
  per la stazione sonar (S3) 340 e 341 classificato sottomarino o nave (durata oltre 800), 346 biologico, 344 il
  contatto cavita, 345 transitorio, 347 nuova riga al LOFAR (tonale oltre +3 dB, una volta per contatto).
- **Ordini di grandezza** (Faroe, 688 a 150 m e 5 nodi, Victor III a 5 nodi): a 3 km contatto subito e traccia solida
  in meno di due minuti; a 8 km in circa sei; a 15 e 30 km solo apparizioni brevi. Il 688 a 5 nodi non lo sente
  nessuno; il suo ping si'.

## Dati della stazione sonar completa (Fase 3, S3)

`nucleo/analisi_acustica.hpp` (roadmap punti 19 e 24, dossier 02 §2.1–2.2 e §2.7; prove `analisi:`), sopra
l'acustica. Il modulo calcola un istante; le storie (righe della cascata e del LOFAR nel tempo) le tiene la
stazione della principale.

**Firme** (`acustica.toml`, generato da `tools/pipeline/acustica.py`). Per ogni classe:
- righe dei macchinari, con il livello rispetto alla tonale, il tipo e l'intervallo di velocità;
- elica: pale, assi, giri al minuto per nodo;
- nazionalità e tipo d'unità.
STIMA DI GIOCO con le regole delle fonti:
- rete a 60 Hz (USA e UK) o 50 Hz (URSS) con le armoniche, 400 Hz sull'Alfa;
- pompe del primario lente e veloci, turbogeneratori e riduttori;
- diesel dei battelli a snorkel (solo a snorkel) e dei mercantili;
- Trafalgar con il pump-jet, senza riga di pala;
- balenottere con gli impulsi a 20 Hz.
Le frequenze sono diverse per classe, così l'analizzatore le distingue.

**Funzioni:**
- `righe_emesse(sorgente)`: le righe di quell'istante. Comprende l'asse (giri/60 Hz) e la pala (pale × giri/60) con
  la seconda armonica: 15 dB sotto la banda larga, 10 dB in più con la cavitazione. Il battello posato toglie 10 dB
  ai macchinari.
- `spettro_fascio(sorgenti, ricevitore, ambiente, rilevamento, f_min, f_max, bin)`: S/N per bin del fascio puntato a
  quel rilevamento, cioè il LOFAR. Somma il continuo e le righe di ogni sorgente:
  - perdita di trasmissione interpolata in log f;
  - righe spostate per Doppler;
  - risposta del fascio: lobo gaussiano largo quanto il fascio del sensore, lobi laterali 15 dB sotto l'isotropo,
    lo specchio per la schiera.
  f_min e f_max fanno da filtri. Con la cavitazione il continuo copre le righe.
- Argomento finale `coerente` (default false) su `spettro_fascio`, `spettro_verniero`, `spettro_continuo` e
  `righe_ricevute`: le frange d'interferenza dei cammini diretti, da `nucleo/interferenza.hpp` (blocco 1 del sonar,
  `docs/20_sonar_blocco1.md` §4; prove `interferenza:`). Il metodo delle immagini, in una guida isoveloce col fondale,
  dà il rapporto fra campo coerente e campo in potenza; sommato alla perdita del livello A aggiunge le frange senza
  cambiare il livello medio. Pesa 1 dove il livello A ha come migliore il diretto, il fondo o l'acqua bassa, e 0 nei
  rifratti. La scoperta resta incoerente. Con false gli spettri restano uguali bit per bit.
- `cascata(sorgenti, ricevitore, ambiente, fasci, banda, caso)`: la riga della cascata a banda larga (BTR),
  (S+N)/N per fascio. Segna ciechi i fasci nei baffles e nell'endfire; la schiera ha la traccia doppia. La grana del
  rumore (1,5 dB) è ripetibile con il `Casuale` della partita.
- `demon(sorgente, ricevitore, ambiente)`: l'inviluppo del rumore di cavitazione in 2–8 kHz, con le righe d'asse e di
  pala forti quanto il S/N in alta frequenza; piatto senza cavitazione. `leggi_demon` ne ricava asse, pala e numero
  di pale, quindi anche la velocità (60 × asse / giri per nodo).
- `righe_osservate(spettro, soglia)`: i picchi che superano di 3 dB il fondo locale (la mediana attorno).
- `confronta_firma(righe, catalogo, pale, bin)`: le ipotesi ordinate {classe, nazionalità, tipo, confidenza, righe
  concordi}. Le righe concordano entro il bin o lo 0,6% (Doppler). Il punteggio è la somma delle righe concordi pesate
  per il livello, meno 0,4 per ogni riga che la classe non spiega, più le pale del DEMON. La confidenza è una softmax:
  con poche righe le ipotesi restano vicine.

**Esempi** (TB-16 alle Faroe d'inverno, bersaglio a 8 kn):
- un Charlie II mostra 8 righe a 3 km (confidenza 1,00) e ancora 4 a 20 km;
- un Victor III a 3 km mostra solo la riga dei 250 Hz: sovietico, ma la classe resta incerta (0,07);
- con le 4 pale lette dal DEMON l'ipotesi migliora;
- un mercantile si riconosce dai diesel (0,68).

**Messaggi** 340–347 (testi in `esporta_motore.py`, blocco della S3):
- 342 «classificazione pronta» e 343 «non identificata» li emette la stazione dopo ANALIZZA; classe e confidenza
  stanno nel pannello;
- 340 e 341 (classificato sottomarino o nave), 344 (cavitazione), 345 (transitorio), 346 (biologico) e 347 (nuova
  riga) sono eventi della simulazione.

**Costi** (Xeon E5-2696 v2 a 2,5 GHz, -O2):
- spettro LOFAR da 4000 bin con 10 sorgenti: 4 ms (continuo e rumore calcolati ogni 1 Hz e interpolati); con le
  frange 2,6 ms in più per sorgente;
- cascata da 72 fasci: 0,2 ms;
- DEMON, righe e confronto: meno di 0,1 ms.

### Navi civili e traffico (blocco 1 del sonar)

`docs/20_sonar_blocco1.md`; dossier `docs/ricerca/26_navi_civili_traffico.md`; prove `navi civili:` e `traffico:`.

**Navi civili** (`nucleo/navi_civili.hpp`). 13 tipi in `[civili.*]` di `acustica.toml` (`tools/pipeline/civili.py`):
- **merci:** mercantile, portacontainer (cargo), portarinfuse;
- **cisterne:** petroliera a turbina, petroliera costiera;
- **traghetti e servizi:** traghetto, rimorchiatore, rompighiaccio, nave oceanografica;
- **pesca:** peschereccio, BMRT, peschereccio costiero, cianciolo.

`genera_nave_civile(tipo, seme)` dà la nave intera, sempre la stessa per lo stesso seme:
- **livello:** il modello JOMOPANS-ECHO della classe, preso come livello di servizio, con 40 log V attorno; 6 dB in
  meno sotto l'innesco della cavitazione;
- **righe:** l'asse e la pala; le accensioni del diesel lento legate all'asse (`RigaFirma::multiplo_asse`); i diesel
  medi col riduttore, fissi col passo variabile (`FirmaRumore::giri_costanti`); i generatori con la rete a 50 o
  60 Hz; la turbina e il riduttore;
- **DEMON:** sopra l'innesco (le altre navi non ne hanno, come prima).

Il catalogo ha la firma tipica di ogni tipo fra le firme, per la libreria e per le missioni; mercantile, cargo,
petroliera e peschereccio restano quelle di sempre. In `classi.toml` ci sono le nove classi nuove, col modello della
più vicina, e `civile = true` (`ClasseNave::civile`). Una nave del traffico porta la sua firma con
`UnitaAcustica::firma`.

**Traffico** (`nucleo/traffico.hpp`): rotte, traghetti di linea e zone di pesca dei sette teatri, in
`data/motore/teatri/<teatro>.traffico.toml` (`tools/pipeline/traffico.py`):
- le corsie stanno sull'acqua del pescaggio dei loro tipi: i porti si agganciano all'acqua utile e i tratti che
  toccano terra si instradano con A* sul fondale;
- le zone di pesca sono punti d'acqua buona.

Il traffico è una funzione del tempo:
- **corsie:** sono nastri con arrivi di Poisson a fasce di 10 minuti, da semi fissi; i traghetti partono all'ora
  locale; le navi stanno a destra dell'asse nel doppio senso;
- **pescherecci:** rimorchiano da un punto a un vicino;
- **tetto:** `Traffico::avanza` tiene le 40 navi più vicine entro 100 km, e le uscite oltre 110 km o scavalcate;
- **eventi:** `fuggi` e `perduta`, per la missione.

Il runtime crea le unità, neutrali e solo nell'Enhanced (contratto in `docs/20` §3).

### La schiera trainata dei sottomarini (blocco 1 del sonar, D)

`nucleo/schiera.hpp` (`docs/20_sonar_blocco1.md` §7; dossier 04 §1.5 e §6.2; prove `schiera:`). I dati per sistema
sono in `[schiere.*]` di `acustica.toml`: `tb16`, `tb23`, `skat`, `sqr19`, letti in `CatalogoAcustico::schiere`.

- **`forma_schiera(dati, v, quota_battello, cavo_filato)`:**
  - il cavo all'angolo critico di Pode (`w cos φ = ½ ρ C_n d v² sin² φ`; la TB-16 a 13° a 5 kn, 165 m sotto il
    battello per 730 m di cavo, come la tabella del dossier);
  - la schiera che flette di `L min(1, vD/v)`;
  - dà la quota del centro (quella dell'acustica), il punto più basso e un profilo di 16 punti per il fondo;
  - oltre la quota massima la schiera è tenuta lì, con il segnale per i danni.
- **`CavoSchiera::avanza`:** la discesa va verso l'equilibrio col tempo del cavo (`(cavo + schiera)/v`, da 60 a
  600 s), rallentando come accelerando.
- **`contatto_fondo(forma, profondità)`:** la frazione di schiera sul fondo, entro 5 m dal fondale.
- **Nel `Ricevitore`:**
  - `sfregamento` e `fondo_sfregamento` aggiungono al rumore proprio lo sfregamento (`rumore_sfregamento_db`, STIMA:
    95 dB re 1 µPa²/Hz a 5 kn su sabbia, 30 log V, roccia +6 e fango −8);
  - con `cavo_filato_m > 0`, gli spettri hanno le righe della vibrazione del cavo (Strouhal, `0,2 v / d`: circa 55 Hz
    a 5 kn per la TB-16), righe proprie con sorgente −2 anche nella cuffia;
  - coi valori di default tutto resta come prima.

### Righe del proprio battello e intercettazione (blocco 2 del sonar, F ed E)

`docs/22_sonar_blocco2.md`; dossier `docs/ricerca/28_intercettazione_righe_proprie.md`; prove `righe proprie:` e
`intercettazione:`.

**F, le righe proprie.**
- **Negli spettri:** con `Ricevitore::proprio` (la Sorgente del battello stesso) gli spettri, le righe ricevute
  (sorgente −3) e la cuffia hanno le righe della propria firma, che cambiano coi giri e col regime delle pompe.
- **Vie d'ingresso:** sfera e sensori di scafo per via strutturale, in tutti i fasci, con `Sensore::isolamento_proprio_db`
  (sfera 30, sovietici 28 e 24; 6 dB in più nei baffles). Le schiere per l'acqua, entro 30° dal fuoco anteriore.
- **Catalogo e maschera:** `righe_proprie(ricevitore, ambiente, fascio, bin)` per la pagina del catalogo;
  `togli_righe(osservate, frequenze, tolleranza)` per la maschera, prima di `confronta_firma`.
- **Nei dati:** i sottomarini nucleari hanno 3 righe deboli legate all'asse (rotore della turbina e due stadi del
  riduttore, `multiplo_asse`), decise dall'utente il 28/10.
- **`Ricevitore::schiera_danneggiata`:** −6 dB di guadagno d'antenna e niente sopra 300 Hz.

**E, l'intercettazione** (`nucleo/intercettazione.hpp`).
- **Dati:** `carica_intercettazione(acustica.toml)` legge `[intercettatori.*]` (WLR-9 a 4 bande, «Skat» con la chiave `skat_ogs` per non confonderlo con la schiera, «Rubikon»,
  «Rubin», con le classi che li portano) e `[emissioni.*]` (i sonar attivi, il BQS-15, le boe, i sonar a immersione);
  le teste dei siluri vengono dai `[transitori.ping_testa_*]`.
- **Ascolto:** `RicevitoreIntercettazione::ascolta(emissioni, ricevitore, ambiente, t, dt)` sente gli impulsi a sola
  andata (livello A alla loro frequenza, contro il rumore in 1 kHz). Misura frequenza col Doppler, durata, livello,
  rilevamento con l'errore della banda, CW o FM se il ricevitore lo sa fare.
- **Tracce:** legano gli intercetti per emettitore e danno cadenza, deriva del rilevamento (retta sull'ultimo minuto),
  salita del livello, segno del Doppler e classe (scafo, immersione, boa, testa d'arma, HF).
- **Allarme:** SILURO IN ACQUA (una testa d'arma), SILURO IN ACQUISIZIONE (cadenza in calo del 15%, +3 dB in 10 s,
  rilevamento fermo, Doppler positivo). Una Mk 48 a 45 kn dà il primo a 3,9 km e il secondo a 1,2 km.
- **Falsi allarmi e ripetibilità:** i falsi allarmi dei ricevitori vecchi (il «Rubin» circa 2 all'ora); tutto
  ripetibile dal seme.

### DEMON a cascata, righe automatiche, storia lunga e CPA nel MATE (blocco 3 del sonar, G, H, I, L)

`docs/23_sonar_blocco3.md`; prove `analisi: il DEMON del fascio`, `righe automatiche:`, `storia delle cascate:` e `misure
sonar: la soluzione dal CPA`. Il classico non cambia, la scoperta (`valuta`) nemmeno: è tutto lavoro della stazione e del sonarista.

- **L, il CPA nel MATE** (`nucleo/misure_sonar`): `soluzione_da_cpa(tma, misura_cpa)` propone una `SoluzioneTma` dal CPA
  della curva a S. Al CPA il bersaglio sta lungo il rilevamento, alla distanza del CPA, con la velocità relativa
  perpendicolare; il moto proprio viene dalla TMA. Dà le sigma di distanza, rotta e velocità; niente se il CPA non è
  stato osservato. Su un bersaglio rettilineo: 4518 m contro 4528, rotta 89,8° contro 90.
- **G, il DEMON a cascata** (`nucleo/analisi_acustica`): `demon_fascio(sorgenti, ricevitore, ambiente, rilevamento,
  f_max, bin)` è l'inviluppo 2-8 kHz di tutte le sorgenti che cavitano nel fascio, pesate con la sua risposta (anche
  le navi civili e il proprio battello). `leggi_demon_fascio` ne ricava fino a 4 serie (asse, pala, pale), togliendo
  una serie alla volta. La storia la tiene la vista.
- **H, le righe automatiche (ALI/ALT)** (`nucleo/rilevatore_righe.hpp`): un `RilevatoreRighe` per fascio o tracker
  riceve le righe del LOFAR con la loro grana.
  - **Integratore:** cerca i picchi sulla media delle ultime 4 righe e conferma una riga in 3 medie su 5.
  - **Inseguitore:** la segue sulla riga nuova con la deriva prevista (Hz/min). Dà id stabile, S/N medio ed età.
  - **Sbaglia come dal vero (STIMA):** circa 4 righe false all'ora su 200 Hz di grana; due righe a meno di 2 bin
    diventano una; una riga assente per più di 60 s si perde e torna con un altro id; le proprie si marcano se la
    maschera è spenta.
  - Solo nel modo moderno.
- **I, la storia lunga** (`nucleo/storia_cascata.hpp`): una `StoriaCascata` per cascata (LOFAR, DEMON o BTR, coi
  rilevamenti come bin).
  - **Risoluzioni:** 1 s per 10 minuti, 10 s per 2 ore, 60 s per 12 ore, ognuna col massimo del suo passo; 8 bit a
    0,25 dB, circa 8 MB con 4000 bin.
  - **Lettura:** `finestra(da, a, livello)` a qualsiasi istante; `comparsa(hz)` dice da quando c'è la riga (o la
    traccia) di adesso, tollerando i buchi brevi delle frange.
  - **Salvataggio:** `salva` e `carica` su stream, binario versionato.

### Misure del sonarista, ACINT, operatore IA e ascolto (S5d)

Moduli puri (dossier 09 §6.4; prove `misure sonar:`, `acint:`, `sonarista:`, `ascolto:`). L'aggancio alla stazione
(`app/sonar_enhanced`) e alla missione e' di chi le tiene.
- **Verniero:** `spettro_verniero` (zoom con la riga sparsa sui bin, finestra di Hann) e `misura_riga` (picco
  interpolato, incertezza di Cramer-Rao dal tempo d'integrazione e dal S/N sul fondo locale). `righe_ricevute` da' le
  righe del fascio con la frequenza Doppler e il S/N.
- **Doppler e CPA:** `misura_cpa` sulla curva a S (frequenza a riposo, velocita' relativa, istante e distanza del CPA,
  con le sigma; non valida se il CPA non sta nei dati); `confronta_cpa` contro una soluzione della TMA.
- **FRAZ:** `distanza_fraz` (R = -c df/dt / (f0 (dB/dt)^2), con i limiti di validita' nel motivo) e `pendenza`.
- **Registratore e ACINT:** `Registratore` (storia per contatto, 6 ore a 5 s, poi la parte vecchia si dimezza),
  `righe_stabili`, `classifica_righe` (scala NONSUB/POSSUB/PROBSUB/CERTSUB), `classifica_contatto` e
  `rapporto_acint` con le righe non in libreria, in italiano e in inglese.
- **Operatore IA:** `Sonarista` (recluta, medio, esperto): soglia e ritardo del rapporto, analisi prima della
  classificazione, classi sbagliate e biologici presi per sottomarini, tracce perse e ritrovate, scambi negli
  incroci; tracce «Sierra» e rapporti al comandante. Deterministico col `Casuale&` del chiamante.
- **Ascolto:** `sintetizza` (rumore colorato dal fascio, righe, battito d'elica, passa banda, ottava, x4/x16) e
  `scrivi_wav`. Il gioco non ha ancora l'uscita audio.

### Audio: mixer, cuffia del sonar e bordo (requisito 27, R1-R3)

Dossier `docs/ricerca/18_audio_reale.md` (§5-6, §8); prove `mixer audio:`, `ascolto:`, `bordo:`. Tutto deterministico
in `nucleo`; il dispositivo e' a parte, e senza dispositivo il gioco tace.
- **Uscita** (`piattaforma/audio.hpp`, `UscitaAudio`): flusso mono float SDL3 sul dispositivo predefinito, col driver
  scelto a run time (pipewire, pulseaudio, alsa, sndio): la build non ha bisogno di ALSA. `apri` restituisce nullptr
  senza audio (anche con `SDL_AUDIO_DRIVER` inesistente); `accoda`, `da_accodare(secondi)`, `volume`, `pausa`,
  `svuota`. Per eseguire serve `LD_LIBRARY_PATH` verso il sysroot (libsndio, dipendenza indiretta di SDL3).
- **Mixer** (`nucleo/mixer_audio.hpp`): canali cuffia, bordo e interfaccia, ricampionati in modo continuo alla frequenza
  d'uscita, con i volumi; rese `piena`, `originale` (16 livelli tenuti a 5,5 kHz senza filtro, come i campioni a 4 bit
  dell'EXE) e `sound_blaster` (256 livelli tenuti a 22.050 o 11.025 Hz senza anti-aliasing, piu' un passa basso).
- **Cuffia** (`nucleo/ascolto.hpp`, `CuffiaSonar` sul `GeneratoreSpettrale`): il fascio puntato in tempo reale, rumore
  colorato da `spettro_continuo` (lo spettro senza righe) a fotogrammi sovrapposti, righe a fase continua col Doppler,
  battito d'elica del DEMON, filtro, ottava, accelerazione; la scena si rilegge ogni 0,25 s. L'uscita non dipende da
  come si spezzano i passi. Costo: 0,25 ms in media e 3 ms al massimo per passo con 8 sorgenti.
- **Bordo** (`nucleo/bordo_audio.hpp`, `AmbienteBordo`): le sorgenti della scena allo scafo (perdita interpolata,
  Doppler), poi K(f) del dossier (passa basso: 91 dB sotto 250 Hz, 107 a 4 kHz, meno il riverbero) e la soglia
  d'udibilita', piu' il fondo della stazione (centrale, sonar, siluri, macchine, alloggi) per regime (silenzioso,
  normale, avanti tutta) col ronzio a 60 Hz. Niente aerei. Eventi a campione esatto: i ping altrui come ticchettio
  sullo scafo, i tonfi vicini, il ping proprio (forte a prua), gli scoppi (Cole per l'urto, Rayleigh-Willis per la
  bolla) con lo `scossone()` per la vista. 0 dBFS rms = 100 dB SPL, saturazione morbida. Costo come la cuffia.
- **Il battello stesso** (R3b, `passo(..., dt, &proprio)` con la sorgente acustica del giocatore):
  - le macchine: le righe della firma come le emette ora (R6: pompe lente, veloci o spente, ausiliari, individuo,
    danni), la piu' forte a 84 dB in sala macchine e 20-40 dB sotto nelle altre stazioni, e il fischio della turbina
    che sale coi giri;
  - i colpi del battello (le voci «scafo» di R7 entro 50 m) forti dentro, per via aerea;
  - la ventilazione in centrale e negli alloggi, le ventole in sala sonar, gli alloggi ovattati;
  - `stazione()` e `regime()` per la diagnosi.
  Senza `proprio` tutto come prima.

### Variazioni nel modello acustico (R6)

Dossier `docs/ricerca/18_audio_reale.md` §3; prove `variazioni:`.
- **Interruttore:** `UnitaAcustica::variazioni`, che la missione accende di serie nell'Enhanced
  (`Missione::usa_variazioni_acustiche(false)`, `rilievo_missioni --senza-variazioni`, opzione di realismo). Spento,
  la `Sorgente` e' bit per bit quella della classe.
- **A chi tocca:** solo navi e sottomarini veri. `sorgente_acustica` le mette in `Sorgente::var`, e le vedono tutti:
  scoperta (`valuta`, compreso il LOFAR, che prende tutte le righe emesse), cascata, spettro, DEMON, cuffia, bordo.
- **L'individuo**, dal `seme` dell'unita':
  - banda larga sigma 2,4 dB (Sponagle);
  - righe di turbine, riduttori, asse e pala spostate dell'1-3%, pompe e ausiliari dello 0,5%, la rete identica in
    tutta la classe;
  - un ritratto di +-2,5 dB per riga;
  - un'elica «cantante» su venti (100-1000 Hz, in una finestra di velocita').
- **Il regime:**
  - pompe automatiche (per velocita', come prima), spente, lente o veloci; la veloce sta 40 log r sopra la lenta (Ross);
  - ausiliari accesi il 70% del tempo, a turni di 20 minuti.
- **Il momento:**
  - aspetto di Trevorrow (nave: prua -10 dB, poppa -7; sottomarino al 30%, STIMA; sotto 100 Hz al 30%);
  - virata +1,65 dB per grado/s, fino a 12 dB sui sottomarini e 18 sulle navi;
  - usura fino a +5 dB.
- **I danni:**
  - all'elica: cavitazione fino al 40% prima e 8-15 dB piu' forte, bande laterali a +-1 volta i giri;
  - alle macchine: +6-20 dB sulle loro righe.
- **ACINT:** il confronto per rapporti accetta le righe dei giri entro il 3,5% (pesate 0,7) e quelle sulla rete entro
  lo 0,6%; rete ed elica tengono preciso il fattore Doppler.

### Libreria di campioni e mescolamento (R4, R5)

Elenco e licenze in `docs/ricerca/18_note/campioni_r4.md`; prove `campioni:`.
- **R4, la pipeline:**
  - `tools/pipeline/campioni.py` scarica i 27 campioni federali (NOAA SanctSound, PMEL, NOAA Fisheries, NPS; 3 esclusi per licenza: capodoglio,
    foca barbata e fessurazione del ghiaccio, che ora hanno le registrazioni di M) in
    `tools/campioni/grezzi/`, con le impronte;
  - `tools/pipeline/campioni_analisi.py` misura l'inviluppo in terzi d'ottava, le righe stabili, il DEMON, il ritmo e
    l'inviluppo nel tempo in tre bande, in frequenze vere (i PMEL sono accelerati);
  - poi scrive la tessitura sbiancata, senza righe e solo nella banda valida, in `data/suoni/campioni/*.wav`, e i
    metadati in `data/motore/campioni.toml`;
  - i crediti vanno in `tools/campioni/crediti.json`, per `crediti.py` del kit.
- **R5, il nucleo** (`nucleo/campioni.hpp`): `carica_campioni`, `LibreriaCampioni::scegli` (dal seme dell'istanza),
  `voce_campione` (la voce della libreria, o «mercantile» per mercantili, cargo e petroliere).
- **In cuffia** (`CuffiaSonar::imposta_campioni`), una sorgente con un campione suona con la tessitura:
  - letta alla posizione del tempo di gioco (`posizione_campione_s`) con l'interpolazione cubica;
  - ricampionata col Doppler (le navi anche al battito d'elica del modello, al piu' x1,3);
  - ricolorata a fotogrammi con lo spettro della sola sorgente nel fascio, prima del limitatore;
  - fuori dalla banda del campione resta il rumore del modello, le righe restano quelle del modello, il battito del
    DEMON del modello non si somma;
  - i livelli per banda tornano entro 2 dB con quelli del modello.
- **R8, aerei ed elicotteri al sonar:** 9 registrazioni da Wikimedia Commons (pubblico dominio, CC0 e CC BY; elenco
  in `docs/ricerca/18_note/campioni_r8.md`): P-3 e C-130, An-12 per l'Il-38, An-24 per il Be-12, Tu-95, SH-60J e
  UH-60, Marine One e UH-3H per il Sea King.
  - Dai video si prende il tratto di 10 s dove il motore (20-400 Hz) domina su voce e musica, controllato con lo
    spettrogramma in `tools/campioni/spettri/`.
  - Cinque sono esclusi: A-10 e Tu-95 alla parata (voci), Ka-29/Ka-52K (musica), MH-60S (saturato) e il Ka-27PS di
    mil.ru (licenza non verificata). Per l'S-3, il Ka-27, il Ka-25 e l'hover resta il modello.
  - In cuffia gli aerei suonano con la tessitura dietro la finestra di Snell; le loro righe restano quelle del modello.
- **M, biologici e ghiaccio** (blocco 3 del sonar; elenco e licenze in `docs/ricerca/29_note/registrazioni_biologiche.md`,
  scelte dall'utente il 28/10/2026): 7 registrazioni nuove.
  - Click del capodoglio (SWFSC, pubblico dominio), trilli della foca barbata (AWI, CC BY 4.0), pressione e «canto» del
    ghiaccio (AWI, CC BY 4.0), colpi del tricheco (AFSC, pubblico dominio presunto), boing della minore del Pacifico
    (SWFSC dalla Wayback, pubblico dominio), orche di Monterey (SanctSound).
  - Due sorgenti nuove nella libreria, con banda e cadenza misurate sulle registrazioni e il livello STIMA:
    `tricheco` (Barents, d'inverno) e `balenottera_minore_pacifica` (Kunashir, d'inverno).
  - Esclusi: la «coda» del capodoglio (la stessa registrazione dei rapid clicks, senza una coda riconoscibile: resta
    il modello) e il bioduck della minore antartica (nessun teatro antartico).
  - I file scaricati restano in `tools/dl/29_biologici/<voce>/`, copiati nei grezzi. `leggi()` ora scala i WAV stereo
    a interi prima di mediarne i canali.
- **Lo sbiancamento** e' robusto: mediana in terzi d'ottava, taglio dei bin oltre 10 dB e un secondo passaggio, cosi'
  le armoniche di eliche e rotori non lasciano buchi di colore.
- **Vento e pioggia del rumore ambiente:** in cuffia anche loro suonano con le tessiture SanctSound (il vento di San
  Miguel, la pioggia di Monterey). La loro parte nel fascio (`componenti_rumore_ambiente`, le stesse formule di
  `rumore_ambiente`) esce dal generatore dentro la banda del campione e la porta la tessitura ricolorata; i livelli per
  banda tornano entro 2 dB.
- **Nel tempo** (`collega_campioni(catalogo, libreria)`): l'attivita' delle sorgenti della libreria che hanno un
  campione segue il suo inviluppo nel tratto che la cuffia legge, alla stessa media di lungo periodo. Cosi' cascata,
  LOFAR, scoperta e cuffia vedono la stessa balena cantare nello stesso momento. Senza le due chiamate tutto resta
  com'era.

### Transitori delle armi e degli scafi (R7)

Numeri e fonti in `docs/ricerca/18_note/transitori_armi_r7.md`; prove `transitori armi:`. Quasi tutti i livelli sono
STIMA: nessun transitorio di lancio misurato e pubblicato.
- **Voci nuove di `[transitori]`** (categorie `arma` e `scafo`):
  - lancio: allagamento ed equalizzazione del tubo, porta, colpo del pistone ad acqua (172 dB, con la risonanza del
    tubo a ~55 Hz) e il suo ritorno, avvio del siluro, tappi della capsula dell'Harpoon, booster del Tomahawk, portello
    VLS;
  - ping delle teste attive (25-35 kHz, 200-210 dB; l'ADCAP in FM);
  - urto e bolla delle esplosioni;
  - colpi del battello: portello, attrezzo, piani, pompa, TDU, soffiata d'emergenza, scricchiolii;
  - affondamento: cedimenti, rottura di una nave, allagamento con le sue righe, bolle, urto sul fondo.
- **Le sequenze** (`nucleo/transitori_armi.hpp`):
  - `transitori_lancio` (tubo o pozzo, con o senza preparazione);
  - `transitori_esplosione` (Cole per il livello, `guadagno_esplosione_db`; Willis per le tre bolle, T2 = 0,74 T1 e
    T3 = 0,63 T1, che sfiatano vicino alla superficie);
  - `transitori_implosione` (TNT equivalente 2,5 p V / 4,184 MJ, -12 dB sull'urto, 10-20 cedimenti in 200 s);
  - `transitori_affondamento`;
  - `transitorio_testa` e `intervallo_ping_testa_s` (1,2 x 2R/c, da 5,5 s a 0,16 s).
- **Le firme dei siluri** hanno le righe di pala e del motore alle velocita' di corsa (Mk 48 a 55 kn: 907,5 Hz), e
  quindi sono anche nella libreria dell'ACINT.
- **In cuffia** tutti i transitori impulsivi di meteo, armi e scafi sono schiocchi a campione esatto (`impulsivo`),
  col passa basso dell'assorbimento del modello.
- **A bordo** lanci, teste e colpi di scafo sono eventi. Le esplosioni della scena no: le porta `scoppio()`.

### Acustica dell'artiglieria (G5 di `docs/16_artiglieria.md`)

Dossier 22 §7, §9.7 e §10; prove `artiglieria:`; taratura in `tools/re/prove/taratura/artiglieria.cpp`.
- **Il contratto** (`nucleo/acustica_artiglieria.hpp`): `EventoArtiglieria` (partenza, scoppio sull'acqua o in aria,
  tonfo, rimbalzo, raffica, testa colpita, passaggio vicino, col calibro, la carica e il tiratore), che produce il
  modello di tiro del runtime.
- **Le sorgenti della scena:** `transitori_artiglieria(evento, id_base, seme)` usa le voci `art_*` di acustica.toml,
  con gli id 990000-999999.
  - Lo scoppio sull'acqua e' un crack a 0,5 m, tarato sul 5" a 1 km (177 dB di picco a 18 m). Lo specchio lo fa la
    perdita del modello: con mare 3 oltre i 3 km si sente piu' della formula a specchio liscio.
  - Il colpo di partenza: 180 dB a 40-150 Hz da 3 m, udibile entro 5-10 km.
  - La raffica dell'AK-630 come ronzio di pezzi da 0,5 s.
  - Tonfo e rimbalzo 40 dB sotto lo scoppio.
- **La formula di Cole con lo specchio:** `picco_scoppio_db` (177, 155 e 144 dB a 1, 3 e 5 km), per il bordo e le
  prove.
- **A bordo** (`AmbienteBordo::artiglieria(evento, pos, quota, t_aria)`) e la stazione nuova `StazioneBordo::plancia`
  (all'aperto: niente scafo, fondo di vento e mare; vale anche per il tuono):
  - immerso: lo schiocco sullo scafo entro 1-2 km, e l'urto metallico col gorgoglio quando e' colpita la testa;
  - in superficie: il boato della partenza e dello scoppio dopo R / c, lo schiocco a N e il sibilo del proietto che
    passa, gli spruzzi entro 100 m. Tutto pieno in plancia, 45 dB sotto e cupo dentro lo scafo.

### Acustica delle collisioni

Dossier `docs/ricerca/16_collisioni.md` §2.6 (il contratto dei danni e' `nucleo/collisioni.hpp`); prove `urto:`. Solo
Enhanced; tutti i livelli sono STIMA.
- **Le sorgenti della scena** (`nucleo/acustica_urto.hpp`): `transitori_urto(urto, esito, id_base, seme)`, con gli id
  940000-949999, al punto di contatto e alla sua quota (non sopra 0,5 m).
  - Il colpo, al livello di `EsitoUrto::livello_sorgente_db` (170 dB re 1 uPa a 1 m per 1 MJ, +10 per decade). Si
    usa `urto_colpo`, sordo a 10-3000 Hz con lo scafo che risuona, oppure `urto_lamiera`, squillante a 50-8000 Hz,
    se una delle parti e' una lama o un'appendice (vela, alberi, timoni, eliche, schiera, sovrastrutture).
  - Lo stridio (`urto_stridio`, 200-6000 Hz) per `durata_strisciamento_s`, a pezzi di 1 s (al piu' 20). Il livello e'
    `livello_stridio_db`: l'energia tangente sulla durata, 160 dB per 1 MJ in 1 s, fra 130 e 180.
  - Se c'entra un'elica, i colpi delle pale (`urto_elica`) per 3 s, 20 dB sotto il colpo.
  - Un urto a 10 nodi fra un 688 e un Victor III si sente alla sfera di un battello a 10 km. Dopo l'urto entrambi
    sanno: l'ascolto dell'IA (`ascolta_transitorio`) lo prende come ogni transitorio.
- **A bordo** (`AmbienteBordo::urto(esito, proprio, distanza)`):
  - con `proprio` (la parte di `EsitoUrto` del battello) e' il nostro scafo. Arriva per la struttura in ogni stazione e
    in plancia (115 dB a 1 MJ assorbito, fra 90 e 135): uno schianto col rimbombo a 38-124 Hz e, con una lama, la
    lamiera che squilla. Poi lo stridio che s'impunta (60 dB sotto il suo livello in acqua) e lo scossone da `scossa_g`
    (0,5 g = pieno);
  - senza `proprio` e' l'urto di altri, che arriva dall'acqua con K(f): un colpo sordo entro ~100 m e niente in
    plancia.
- **Resta al runtime** l'aggancio in `Missione`: le sorgenti nella scena per ogni `Urto` valutato e la chiamata del
  bordo. I rumori di danno permanenti sono `DanniUrtoUnita::rumore_db` del kit.

### Audio del maltempo (M5 di `docs/15_meteo_e_mare.md`)

Numeri e fonti in `docs/ricerca/21_note/e_audio_meteo.md` («21e»); prove `audio meteo:`; taratura in
`tools/re/prove/taratura/`. Vale solo con la regola di simulazione.
- **Vento e pioggia** sono gia' nel rumore ambiente (`rumore_ambiente`): la missione riempie `Ambiente.vento_m_s`,
  `pioggia_mm_h` e `ghiaccio` dal meteo nel punto, e cuffia e sonar li sentono senza altro.
- **Fulmini al sonar** (`nucleo/audio_meteo.hpp`, `transitori_lampo(lampo, id_base, seme)`): un `Transitorio`
  `fulmine_mare` per colpo di un lampo nube-mare, a ~60 ms lognormali e 6 dB sotto il primo. Colpo di 40-400 Hz a 195
  dB per 50 ms, tarato sulle osservazioni (+15 dB sul fondo a 10 km, ~+23 a 1 km, niente oltre 60 km), con la coda di
  multipercorso e lo schiocco acuto; il sonar lo sente come transitorio (falso allarme). In cuffia (`CuffiaSonar`)
  diventa uno schiocco a campione esatto al S/N istantaneo del fascio (`snr_impulso_db`), invece di passare dallo
  spettro; senza fulmini l'uscita della cuffia e' identica a prima.
- **A bordo** (`AmbienteBordo::meteo(stato)`, `tuono(distanza, nube_mare, t_aria)`):
  - i colpi di mare al periodo d'onda, da mare 4, con l'attenuazione del moto orbitale exp(-2 pi z / lambda): in
    superficie tonfo e campana dello scafo, a quota periscopica tonfi e scricchiolii, in profondita' niente;
  - l'acqua sul ponte in superficie;
  - il tuono, solo in superficie, dopo distanza / c, 45 dB sotto quello in aria e con passa basso a 200 Hz, con
    rimbombi e schianto sotto 3 km, niente oltre 20 km;
  - i fulmini caduti vicino come tonfi (netti entro ~100 m).
  La pioggia e il vento dentro lo scafo non si sentono.
- **Sala radio** (`ScaricheRadio`, bande HF SSB, HF AM e VLF):
  - fondo del ricevitore, alzato di 10 dB da una cella entro 50 km (`ambiente(celle, pos, notte)`);
  - crepitio lontano tarato sul Vd: ~6 dB in HF di notte, meno di giorno, ~7 in VLF, sotto gli 11-16 delle tabelle;
  - un crash per lampo (`lampo(l, pos)`): +35 dB entro 50 km, impulsi da 1 ms;
  - AGC con rilascio di 0,6 s, che dopo un crash si mangia il segnale (`passo(dt, &segnale)`), e soppressore opzionale;
  - in VLF di notte i tweek e qualche whistler.
  Gli effetti sui messaggi restano della sala radio.

## Sala radio dell'Enhanced (pezzo S4)

`nucleo/radio.hpp`, puro (dati e scelte di gioco dal dossier `docs/ricerca/08_comunicazioni_radio.md` §9, tutto STIMA;
prove in `test/prove_radio.cpp`). Il livello classico non lo usa.
- **Antenne:** albero o periscopio alzati a quota periscopica (tutte le bande; oltre 10 nodi al 50%); filo galleggiante
  OE-315 (solo VLF, due minuti per filarlo: fino a 120 m, pieno fino a 8 nodi e al 50% fino a 12, cieco due minuti
  dopo una virata, oltre 20 nodi per piu' di un minuto si spezza); ELF per chi ce l'ha (i sovietici: ZEVS), solo come
  chiamata dei FLASH.
- **Broadcast:** coda continua, prima i FLASH; ogni messaggio ha un numero di serie e torna in onda ogni 2 ore, 3 volte
  i FLASH e gli IMMEDIATE, 2 i PRIORITY, 1 i ROUTINE. Si riceve solo con la banda per tutto lo spazio (al 50%: una
  volta su due). Il buco nella serie si vede quando arriva un numero successivo (`persi()`). Il traffico di fondo
  (meteo, intelligence generica, prove) arriva in media ogni mezz'ora.
- **Satellite (SSIXS):** con l'albero e fino a 72 gradi di latitudine una richiesta scarica in 3 minuti tutto cio' che
  e' andato in onda, persi compresi; piu' a nord niente satellite.
- **Trasmettere:** solo con l'albero; UHF col satellite (circa 1,5 s), altrimenti HF (1-3 minuti); i sovietici in burst
  da 0,72 s. Radiogoniometria nemica: P = P_rete (1 - e^(-t/tau)), con un ESM nemico entro l'orizzonte solo il
  rilevamento e subito, altrimenti un fix con decine o centinaia di km d'errore dopo ore.
- **Formato e orari:** gruppo data-ora dall'ora e dalla data della missione (`ora_utc`, `data`), DA/A/OGG., classifica,
  gruppo d'autenticazione; finestre di comunicazione ogni 12 ore per gli USA, alle 00:00 e alle 12:00 di Mosca per
  l'URSS. Telescrivente: `testo_in_arrivo()` e `progresso()` della messa in onda.

**Nella missione:** `DanniMissione` con la regola di simulazione chiama `Missione::radio_enhanced()`, che crea la sala
del giocatore (lato dalla missione) con gli ordini di missione dal briefing (serie 0). Messaggi della missione,
facoltativi, in `data/missioni/radio/<ID>.lua` (docs/09 §5.1, formato provvisorio): ricevuto, il gestore cambia gli
ordini. Comandi:
`radio_filo(cala)`, `radio_trasmetti(tipo)`, `radio_ssixs()`. Un fix da' alle unita' avversarie un contatto sul giocatore
(sensore «radiogoniometria», con la posizione del fix) e fa scattare la loro scelta del bersaglio. Avvisi 315-325 in
`Missione::messaggi_radio()`. Rilievo classico identico alla base.

**Fase R1** (dossier `docs/ricerca/10_radio_ruolo_in_partita.md` §0 e §4.1-4.4; modulo e prove `radio R1:`):
- **orologio di missione:** ciclo di 15 minuti per tutte le missioni dell'Enhanced (`parametri_di_missione`), traffico di
  fondo ogni 5 minuti, finestre ogni 30 minuti dal quindicesimo con 5 minuti di tolleranza (STIMA: con il rapporto
  vero le finestre non arriverebbero quasi mai); il file radio puo' cambiarli;
- **contatti d'intelligence:** un messaggio con `contatto` porta un'area, compilata con la verita' quando va in onda
  (le unita' all'ora dell'osservazione, con l'errore di posizione, rotta e velocita'); in `SalaRadio::intelligence(t)`
  centro, `semiasse_rotta`, `semiasse_traverso`, orientamento ed eta', che crescono col tempo; tolti dopo tre cicli;
- **ROE:** stati libero, stretto e trattenuto per sottomarini, navi, mercantili, neutrali (dal file) e aerei; quelle
  trasmesse e quelle ricevute; ogni lancio del giocatore e' giudicato sul bersaglio designato (infrazione, infrazione
  per non aver copiato il broadcast, nota) con la categoria che il giocatore credeva; `debriefing()` con trasmessi,
  persi e lanci; avviso 333 sulle infrazioni;
- **rapporto di contatto:** il gestore di `rapporti.contatto` arriva dopo il ritardo (avviso 331); **boa SLOT**:
  `radio_slot(ritardo)` trasmette dal punto di lancio, il premio solo con un aereo amico entro la portata VHF, il fix
  nemico cade sulla boa (avviso 332);
- avvisi 326 (ordini in vigore cambiati) e 327 (contatto d'intelligence).

**Fase R2** (dossier 10 §4.3 e §4.5-4.8; prove `radio R2:`):
- **zone dello spazio d'acqua:** d'azione, libera ASW, protette fisse (cerchio o poligono) e mobili (rettangolo davanti,
  dietro e ai lati di un battello amico, orientato sulla sua rotta), trasmesse e ricevute come le ROE; i messaggi le
  aggiungono, spostano o tolgono. `Missione::zone_ricevute()` le da' alla carta con la forma di adesso. Un lancio su
  un bersaglio immerso in una zona protetta si giudica con la stessa regola delle ROE (`GiudizioRoe::zona`);
  l'ingresso del giocatore in una zona protetta che conosce da' l'avviso 328 e una voce del debriefing;
- **compiti con scadenza:** `scade_s` e `gestore_scaduto`; scaduto senza essere ricevuto: il gestore e l'avviso 329;
  ricevuto dopo la scadenza: non vale (`Ricevuto::tardi`, niente gestore);
- **guinzaglio sovietico:** `finestre` obbligatorie, con la ricevuta trasmessa nella finestra; sessione mancata:
  avviso 330; dopo `saltate_max` di fila il gestore (`SalaRadio::sessioni(t)` per la stazione);
- **inganno:** un messaggio non autentico non esegue il gestore e non cambia ROE e zone per il comando; il giocatore
  ci crede finche' non lo verifica (`Missione::radio_verifica(serie)`, avviso 339 se il gruppo non torna); il
  giudizio usa solo gli ordini autentici;
- **richiamo e rami:** il gestore del messaggio accoda l'esito (`m688.messaggio(u, 100 + k)`), come le pagine
  d'esito dell'originale.

## ESM ripensato ed EMCON dell'IA (punto 23)

`nucleo/elettronica.hpp` (dossier `docs/ricerca/11_emettitori_radar_esm.md` §6; dati `data/motore/emettitori.toml` da
`tools/pipeline/emettitori.py`; prove `elettronica:`). Il livello classico non lo usa.
- **Radar:** 26 radar (navi USA e sovietiche, mercantili, aerei ed elicotteri ASW, sottomarini, teste dei missili)
  con banda, periodo, altezza e R50 contro periscopio, albero, snorkel e nave col mare; per giro
  p = F_op F_vel / (1 + (R / R50)^4) entro l'orizzonte radar, cumulata sull'esposizione.
- **EMCON:** silenzio, navigazione, ricerca, intermittente (aerei, acceso e spento a caso), ingaggio; navi USA in
  navigazione, sovietiche in ricerca, mercantili col radar di navigazione; salgono con gli allarmi (albero visto,
  fix di radiogoniometria) e tornano dopo 20 minuti.
- **ESM del giocatore** (WLR-8; Bukhta per l'Alfa): un intercetto per passaggio del fascio entro l'orizzonte per il
  fattore del radar; rilevamento con l'errore della banda che cala coi passaggi, distanza stimata, ampiezza 1-5,
  identificazione al terzo passaggio (anche sbagliata; 0,3 per il Korshun con l'agilita' di frequenza), tendenza e
  rischio di contro-scoperta.

**Nella missione:** `DanniMissione` con la regola di simulazione chiama `Missione::elettronica_enhanced(cartella)`.
I radar accesi vedono l'albero (o il battello emerso) del giocatore con la probabilita' per giro: contatto con la
posizione e EMCON in ingaggio. Al posto dell'intercettazione dell'originale (4,5-6,75 nm, sempre), che nell'Enhanced
non conta piu'. L'ESM del giocatore: `intercettazioni()`, contatti col sensore «esm» (solo il rilevamento),
`livello_esm()` = 17 per l'ampiezza piu' alta (i 9 riquadri), `emcon(id)`, avvisi 334-338 in `messaggi_esm()`. A 30
km una Krivak in ricerca si sente e non vede l'albero; a 4 km lo vede. Rilievo classico identico alla base.

**Libreria consultabile** (S5b; dossier 09 §3.4–3.5; prove `libreria:`):
- `elenco_libreria(catalogo, scheda)`: le classi con righe e le sorgenti, ordinate per nome. Schede: navale,
  biologico, geofisico, meteo, umano (civile e industriale insieme).
- `voce_libreria(catalogo, id)`: la scheda della voce. Le righe hanno l'etichetta: «rete 60 Hz x5», con la base della
  rete di bordo della classe; «pompa 84.6 Hz (da 15 kn)»; «famiglia 40 Hz x3»; «sweep 40-15 Hz». Poi le bande, l'elica,
  il ritmo descritto, gli indizi, le confusioni, l'ascolto, la fonte e la confidenza.
- `confronta_rapporti(righe, catalogo, pale)`: per ogni voce, il fattore Doppler k che allinea più righe (ogni coppia
  osservata/libreria ne propone uno, fino a ±1,2%), poi affinato sulle concordi. Restituisce la velocità radiale
  (k − 1)·c, positiva se la sorgente si avvicina; le righe concordi, quelle mancanti (le più forti prima) e quelle in
  più. La confidenza è una softmax. Gli sweep non entrano.
- `famiglie_armoniche(righe, f_max, catalogo)`: fondamentale ai minimi quadrati, multipli presenti e copertura. Le
  candidate si ordinano per membri × copertura, così le sottoarmoniche non vincono. L'indizio è: rete 50/60/400 Hz, una
  famiglia della libreria, «elica (pala)» sotto i 30 Hz (con le pale se c'è la riga d'asse), oppure ignota.
- `differenza(a, b)`: a − b bin per bin, per gli slot; b si interpola se le griglie differiscono.

## ASW aereo dell'Enhanced (A0 e A1 di docs/13_asw_aereo.md)

`nucleo/asw_aereo.hpp` (dossier `docs/ricerca/17_asw_aereo.md`, citato per paragrafo; dati `data/motore/asw_aereo.toml`
da `tools/pipeline/asw_aereo.py`; prove `asw aereo:`). Modulo puro, solo con la regola di simulazione; ogni estrazione
usa il `Casuale&` della missione. Nella missione dal pezzo A3 (sotto).
- **Dati:** 17 boe (USA §4.1, URSS §4.2), 4 marcatori, 4 sonar a immersione, 11 mezzi con carichi alternativi e TOS(d)
  = base − d/k, equipaggi (§7.4), difficolta' (§10.3), probabilita' (§7.2) e scelta degli schemi (§7.3). Ogni voce ha
  `fonte`, `conf` e l'elenco `stime` dei campi STIMA da tarare in A6. Le classi s3a, s3b, sh3h, sh60f, ka27 e be12 sono
  in `classi.toml` con lo sprite segnaposto di una classe simile (`SEGNAPOSTO` in esporta_motore.py); i radar APS-116,
  APS-137, Korshun, Initsiativa-2B e 2KM in `emettitori.toml`.
- **Boe:** `crea_boa` (discesa a 30 m/s, stabilizzazione con la quota: 100 s a 30 m ... 240 s a 300 m, vita, guasto,
  deriva 0,5-2 kn nel realistico); id = lanciatore·1000 + progressivo.
- **Schemi:** `posa_schema` per barriera (S = 1,75 R50), barriera stretta, chevron, cerchio di 0,7·(r0 + u·t), cuneo,
  trappola, box DICASS, cloverleaf, scacchiera, campo, barriera circolare (≥ 3000 m), spirale logaritmica,
  intercettazione, incrocio, eco esplosiva.
- **MAD:** `portata_mad_m` (OPTEVFOR: 376 m per 4000 t, radice cubica del dislocamento e del rumore; 688 ≈ 450 m),
  `probabilita_mad` (normale, sigma 0,2·R50), `spazzata_mad_m`; `SensoreMad` estrae una volta per passaggio, al punto
  piu' vicino.
- **Dip:** `CicloDip` (avvicinamento, calata, ascolto, ping, recupero, salto) col cavo per il kit: SH-3H 4-6 min in
  inseguimento e ~20 min in ricerca, Ka-27 10-12 min.
- **Controllore:** `ControlloreAsw::passo(t, PercezioneAsw, Casuale&)` ogni 5 s, gli 11 stati del §7.1 (T R D K L I A V P
  C X) e `volo()` (meta, quota, velocita', hover, cavo, luci, radar). Le rivelazioni vengono dal motore acustico (A2);
  qui si aggiungono i falsi allarmi spontanei delle boe seguite (al piu' `canali_boe`) e la decisione su un falso
  contatto (`crede_falso`). In pace e in crisi niente armi: SUS (USA) o rapporto. Le azioni (posa_boa con la boa gia'
  creata, cala_sonar, ping, recupera_sonar, comanda_boa, lancia_arma, marcatore, sus, passa_contatto, rapporto) le
  esegue la missione.

### L'ASW aereo nella missione (A3)

`missioni/src/missione_asw.cpp` (prove `asw missione:`). `DanniMissione` con la regola di simulazione chiama
`Missione::asw_enhanced(cartella)`; `usa_asw(false)` prima del primo passo per farne a meno. Configurazione: chiave
`aereo_asw` della missione o `data/missioni/asw/<ID>.lua` (docs/09 §5.4).
- **Guida:** in transito e in ricerca, senza schemi da posare, l'aereo segue la rotta dello script; altrimenti
  `navigazione` prende rotta e velocita' vere dal controllore (nel pilota, senza toccare meta e arrivi degli script).
  `vai_al_bersaglio` di un aereo controllato diventa un cue nave_colpita. La quota vera e' in `quota_aerea(u)` e in
  `voli_asw()`; `UnitaClassica::quota` resta 0, cosi' le regole classiche non cambiano. `sincronizza` da' al Mondo la
  quota negativa (aereo in volo), anche nel classico (150 m gli elicotteri, 300 m gli aerei).
- **Ascolto:** ogni 5 s le boe seguite (`ascolta_boe`), il dip passivo e gli echi di dip, DICASS e SSQ-47B (`ascolta`)
  contro i bersagli e contro navi e biologici entro 30 km (falsi allarmi); il MAD a ogni passo col dislocamento della
  classe (`[dislocamenti]` di asw_aereo.toml). Il radar dell'elettronica da' l'albero visto.
- **Ponte verso gli script:** contro un sottomarino immerso il contatto dell'aereo viene dalla traccia del
  controllore (traccia solida col fix). Le originali tengono anche il sensore classico (`sensore_classico`), tarato
  sulla missione. La boa marcatrice della missione 7 diventa una SSQ-62B in `boe()`; in ricognizione il controllore
  marca il fix con una DICASS che pinga ogni minuto e passa le coordinate al giocatore (389).
- **Armi:** il siluro col paracadute (mk46, at1, at2, umgt1, apr2 di siluri.toml) parte col modello dell'Enhanced dal
  punto d'ingresso; le PLAB affondano e scoppiano con la regola dei colpi e la geometria vera. `lanci_aerei()`,
  `marcatori_asw()`.
- **Per il giocatore:** gli aerei come sorgenti (finestra di Snell e hover) e i transitori (tonfi, ping, esplosioni,
  SUS) nella scena acustica e nei sensori passivi; avvisi 380-389 in `messaggi_asw()` (col rilevamento), al piu' uno
  al minuto per codice. Registro M1: `aereo_in_sorvolo`, `ping_sonar_immersione`, `marcatore_avvistato`,
  `attacco_simulato`, e `boa_vicina` anche per le sonoboe; i conti nelle esposizioni.
- Regressione dell'Enhanced: `rilievo_missioni --enhanced [--senza-asw]`.

### Acustica dell'ASW aereo (A2)

`nucleo/acustica_asw.hpp` (dossier 17 §4 e §8.3; prove `acustica asw:`). Modulo puro sopra l'equazione del sonar:
nessun dado per la scoperta, il `Casuale&` serve solo agli errori di rilevamento e di distanza.
- **Sensori** in `acustica.toml` (da `tools/pipeline/acustica.py`, `BOE`): ssq41b, ssq53b, ssq77a, ssq57a, ssq47b,
  ssq62b, rgb_nm, rgb1, rgb2, rgb75, rgb15, rgb25, rgb55a; i sonar a immersione aqs13b, aqs13f, vgs3, vgs2, ciascuno
  con la voce `_passivo`. Campi nuovi di `Sensore`: `di_fisso_db` (guadagno dato: DIFAR 3, VLAD 15, dip 12, DICASS
  10), `sigma_rilevamento_deg` (DIFAR 7, RGB-2 e RGB-25 6, DICASS 5), `omni` (nessun rilevamento), `sigma_distanza_m`.
  Le americane hanno il LOFAR dell'aereo (bin 1 Hz, 60 s); le sovietiche si ascoltano a orecchio (1-2 s).
- **Ricevitori:** `ricevitore_boa` (posizione con la deriva, quota dell'idrofono), `ricevitore_dip` (cavo meno hover).
- **Ascolto:** `ascolta_boe` per le boe passive in ascolto e col mare entro `mare_max`, con la soglia delle sovietiche
  (`SogliaBoa`: trasmettono solo un segnale che dura `soglia_s`); `ascolta` per un ricevitore (passivo, o l'eco di un
  ping). Escono le `Rivelazione` del controllore (rilevamento con l'errore del sensore, distanza degli attivi, verita').
  `StatoAscoltoBoe::distanza_max_m` salta le coppie lontane. Costo: ~8 µs per coppia boa-sorgente.
- **Aerei** (`[transitori]` aereo_* ed elicottero_*): `sorgente_aerea` solo nella finestra di Snell
  (`finestra_snell_m`: 0,18 h + 1,2 d, piu' 250 m per stato del mare vicino alla superficie), appena sotto la
  superficie, col livello riferito a 300 ft e -20 log(h / 91); toni di elica e rotore del §8.3; `probabilita_sorvolo`
  0,95 a quota periscopica e 0,8 in profondita'. `sorgente_hover`: rotore e spruzzi, sentiti entro ~1 nm.
- **Transitori** (`Transitorio`, `sorgente_transitoria`): tonfo di boa (1-3 nm), di trasduttore e di siluro; ping di
  AQS-13B/F, VGS-2/3, DICASS, SSQ-47B (ogni 10 s per la vita della boa), RGB-55A; esplosive MGAB singole e in treno a
  4 Hz; SUS Mk 84 (toni a 3 kHz per 80 s). `vita_s` e il processo `singolo` della libreria ne fanno eventi con un
  istante 0; `ascolta_transitorio` da' il migliore dei ricevitori del battello col rilevamento.
- **Asimmetria (§8.3):** in acqua profonda il ping dell'AQS-13B si intercetta 7,4 volte piu' lontano della scoperta
  dell'eco, quello della DICASS 10,6 volte (la prova lo controlla fra 7 e 12).
- **Riga di pala** (`frequenza_pala_hz`, `livello_pala_db`): 15 dB sopra il continuo a 10 kn, 60 log V (Ross); la
  stessa nello spettro del giocatore e nel LOFAR di `valuta`, che la prende quando cade nella gamma del sensore
  (`gamma_hz` delle boe; i sensori di bordo non ce l'hanno). La RGB-75 sente un 688 a 0,4 km a 4,5 kn e a 3 km a 12 kn,
  un Alfa a 11 km (dossier 17 §8.4).
- **Riverberazione** degli attivi: volume, superficie (Chapman-Harris col vento) e fondo (Lambert), nell'anello alla
  distanza obliqua dell'eco, con il lobo verticale; `risoluzione_s` per l'FM. L'AQS-13B fa 5,4 kyd con lo strato in
  mezzo e 9-11 kyd in buona propagazione; in acqua bassa il fondo accorcia gli attivi delle navi.

## Periscopi dell'Enhanced (P1 e P2 di docs/14_periscopi.md)

`nucleo/periscopi.hpp` (dossier 20, citato per paragrafo; prove `periscopi:`). Modulo puro e deterministico: ogni
estrazione usa il `Casuale&` della missione. Dati da `data/motore/periscopi.toml` (P0, `tools/pipeline/periscopi.py`):
`carica_periscopi` da' i modelli (`DatiPeriscopio`) e le classi (`DatiClassePeriscopi`: periscopi, quota periscopica,
testa esposta, dottrina delle guardate, avvisatore, TV).
- **Posto del giocatore** (`PostoPeriscopi`): scelta del periscopio della classe (con l'altro su, prima scende),
  alzata e discesa coi secondi del modello, elevazione, ingrandimento e modo. La testa esce quando manca meno della
  sua altezza alla corsa di 6 m (STIMA): `altezza_testa_m` dalla quota della classe. Il flag `su` arriva a fine corsa
  e cade quando la testa torna sotto. `esposto_s` conta la guardata; `esposizioni()` e `finite()` la storia.
- **Onde** (nucleo/onde.hpp, la stessa funzione della vista): `finestra_coperta` quando `elevazione_onda_m` con
  l'Hs dello stato del mare (`altezza_significativa_m`, Douglas) supera la testa meno 0,12 m. In quell'istante non si
  vede e la testa non si vede; la guardata non si spezza. Alla vista vanno la stessa Hs e il tempo di missione.
- **Velocita' e mare:** `tremolio` (0 fino a v_libera, 0,5 a v_tremolio, 1 a v_inservibile, +0,08 per grado di mare
  oltre 2, meta' se stabilizzato), `piuma` (fattore × (v − 2) / 8), `probabilita_danno_s` (0,4% al secondo per nodo
  oltre v_danno, con l'albero alzato).
- **Visibilita'** per un osservatore (nave, aereo, periscopio): `portata_visiva_m` da testa e piuma (dimensione
  equivalente, 7 km per metro di giorno), ridotta dal mare (0,35 a mare 5) e dalla notte (all'occhio resta l'8%,
  all'intensificatore il 35%, al termico il 60%), allungata dal riflesso del sole, mai oltre l'orizzonte visivo e la
  visibilita'. `probabilita_vista` e' un tasso di guardate di 0,1 al secondo (nave) con 1 / (1 + (d/R)^4). Tutte STIME
  da tarare in P5.
- **Radar:** `sagoma_testa` da' la sezione efficace (sezione per 1 m per l'altezza al quadrato) a
  `elettronica::TestaEsposta`; la R50 del periscopio scala con la radice quarta rispetto a 0,5 m² a 1,5 m.
- **IA** (`PeriscopioIa`): guardate di `esposizione_s` ogni 3-12 minuti, a quota periscopica e sotto v_libera, mai in
  allarme. I sovietici alzano prima l'avvisatore (5 s): un radar sentito rimanda la guardata. Con la TV МТ-70 (Victor
  III, Charlie II) di giorno fino a 50 m guardano senza esporsi (portata 0,3). Di notte l'osservatore usa il miglior
  sensore dei periscopi della classe (Trafalgar: il termico del CH84).

### I periscopi nella missione (P2)

`missioni/src/missione_periscopi.cpp` (prove `periscopi missione:`). `DanniMissione` con la regola di simulazione
chiama `Missione::periscopi_enhanced(cartella)`. Senza i dati, o nel classico, resta il periscopio dell'originale.
- **Quote:** la quota periscopica della missione (20 ft, quella dell'originale) vale la quota periscopica della classe.
- **Ordini:** `periscopio` passa al posto, e il gestore dello script alza il flag a fine corsa e lo abbassa con la
  testa sotto. Se sotto i 20 ft l'originale abbassa il flag, anche l'ordine torna giu'. Poi `scegli_periscopio`,
  `elevazione_periscopio` e `foto_periscopio` (solo coi modelli che hanno la foto, e periscopio su).
  `vista_periscopio` cambia ingrandimento e modo, che non toccano la simulazione.
- **Elettronica:** coi periscopi il radar dell'IA vede la testa vera (`testa_giocatore`); l'ESM funziona con l'antenna
  o con una testa che ha l'ESM (il Type 18 si', il Type 2 no).
- **Vista**, ogni secondo, col flusso dei periscopi:
  - navi (vedette a 20 m), aerei (alla loro quota) e periscopi dell'IA in guardata vedono la testa del giocatore:
    contatto con la posizione, sensore «vista», EMCON in ingaggio e scelta del bersaglio come per il radar;
  - il giocatore col periscopio su vede le teste dell'IA (`teste_ia()`: guardate e periscopi alzati dagli script):
    contatto col rilevamento e sensore «vista».

  La luce viene dal sole della missione (data, ora UTC, posizione: `luce_periscopi()`). Nel Mondo le teste dell'IA
  sono `alberi_alzati`.
- **Danni:** oltre v_danno `guasti_richiesti()` chiede il sistema «periscopio»; `DanniSottomarini::avanza` lo applica
  con `BattelloDanni::guasto_sistema` (rapporto con la causa). Col sistema sotto 0,5 il periscopio non sale; riparato
  dalle squadre, torna.
- **Avvisi 400-407** (fascia 400-409) in `messaggi_periscopi()`: scelto, su, inservibile, danneggiato, non sale,
  esposti da 30 s, periscopio nemico in vista, foto.
- **Registro M1:**
  - `foto_periscopio`: l'unita' piu' vicina nel campo come soggetto, la camera dalla testa del giocatore, le pose di
    quelle in vista; valore = rilevamento, livello = ingrandimento;
  - `esposizione_lunga`: oltre 30 s;
  - nell'esito: `foto_periscopio`, `esposizioni_lunghe`, `esposizione_periscopio_s`.

## Meteo e mare dell'Enhanced (M1, M2 e la radio di M4 di docs/15_meteo_e_mare.md)

`nucleo/meteo.hpp` (dossier 21, contratto al §11; prove `meteo:`). Modulo puro e deterministico, dati da
`data/motore/meteo.toml` (M0, `tools/pipeline/meteo.py`); `carica_meteo` legge generale, teatri, mesi, stagioni,
regimi, eventi e punti ERA5. La scala unica: `classe_douglas(hs)`, `centro_douglas(ss)`, `beaufort(u)`.
- **Generatore orario** (`Meteo`), un `Casuale` per processo, interpolato al secondo:
  - regime sinottico semi-Markov (durate gamma), coi pesi delle transizioni tarati perche' le quote nel tempo siano
    quelle della stagione e un 10% di salti fuori dai `prossimi`;
  - vento: AR(1) a due stadi (veloce di 4 h, lento col τ del mese) piu' il regime, mappato per ranghi sulla
    distribuzione dell'anomalia e poi sulla Weibull del mese, con le code tarate su Bft 7 (ERA5) e Bft 8 (ERA5 per
    `correzione_burrasche`);
  - mare da vento che rilassa verso l'equilibrio (CEM col fetch, U^1,6 oltre 10 m/s) e decade col τ dell'onda; swell
    AR(1) lognormale; scala e swell tarati con una corsa breve perche' la Hs media sia quella del mese;
  - nubi, catena asciutto/bagnato e intensita' gamma (tarate sulle ore bagnate), neve sotto 1 °C; temperature e
    pressione col regime (la pressione scende prima del fronte); nebbia per tipo (tarata su UR ≥ 97% e fumo di mare);
    visibilita' di Koschmieder; ghiaccio del mese e degli inverni rigidi; spruzzo gelante di Overland;
  - eventi rari (Poisson orari nei loro mesi) che sovrascrivono il regime; celle temporalesche che vanno col vento.
- `in(pos)`: i punti ERA5 del riquadro, lo stretto vicino al centro del teatro, le celle (rovesci, raffiche, lampi).
  `lampi(t0, t1)`: per minuto di vita della cella, gli stessi comunque si chieda. `previsione(emessa, valida)`: lo
  stato senza eventi, spostato nel tempo e con gli errori del 1989 per orizzonte; niente polar low, getti, temporali.
  `fissa(s)`: editor e opzione «fisso».
- Taratura: cento «anni» di gennaio e di luglio per teatro contro ERA5 (vento ±10%, Bft 7 ±20%, Bft 8 ±30%, Hs ±20%,
  classi Douglas, ore bagnate, τ del vento, persistenza a 6 h). Un passo costa ~6 µs; la costruzione ~0,15 s.

### Il meteo nella missione (M2) e alla radio (M4)

`missioni/src/missione_meteo.cpp` (prove `meteo missione:`). `DanniMissione` con la regola di simulazione chiama
`Missione::meteo_enhanced(cartella)`.
- **Modi:** `usa_meteo(ModoMeteo::realistico)` prima del primo passo per il generatore del teatro; il predefinito e'
  `semplificato` (mare 3, sereno: l'Enhanced di prima). `rilievo_missioni --enhanced --meteo`.
- **Seme:** teatro, data e missione col seme della partita. Mese: quello della missione o della data. L'origine
  (0, 0) e' il centro della missione.
- **Ogni secondo:** `meteo_qui()` nel punto del giocatore e `stato_mare()`; `meteo_in(pos)` altrove.
- **Effetti:**
  - acustica: vento, pioggia e ghiaccio dell'`Ambiente`;
  - elettronica: lo stato del mare del giocatore per radar ed ESM, R50 del dossier 21 §6.1 anche sotto 2 e oltre 4
    (i radar per periscopi vedono ancora a mare 5-7), la pioggia in banda X;
  - periscopi: Hs vera, errore di quota del §7.3 (la testa vera, che non spezza la guardata), vista con i
    moltiplicatori del §6.1, la notte delle stelle e del coperto, il termico nella nebbia di radiazione;
  - ASW aereo: mare, vento e onde nel punto dell'aereo per il controllore e le boe; AQS-13 fino a mare 5 sotto i
    10 ft d'onda, sonar sovietici fino a 5, niente immersione coi fulmini entro 5 nm; MAD ×0,8 a mare 4-5 e ×0,6
    da 6 (`rumore_mad_col_mare`); SSQ-53B fino a mare 6; siluri aviolanciati guasti all'ingresso in acqua (3-25%);
  - navi: la velocita' sostenibile del §7.2 per lunghezza col mare di prua (`velocita_col_mare`);
  - sottomarino: la vela del giocatore affiora a quota periscopica col mare grosso (§7.3: `vela_affiorata()`,
    15 s da emerso per il radar, avviso 408).
- **Radio (M4):** alle ore dei bollettini (00, 06, 12, 18 UTC) il WEAX sul broadcast, con la previsione a 12 e 24
  ore; GALE o STORM WARNING (PRIORITY) quando la previsione vede la burrasca entro 24 ore; la previsione chiesta
  (`TipoTrasmissione::previsione`, col costo della trasmissione) torna al ciclo dopo. Dal lato sovietico «метеосводка»
  e «штормовое предупреждение» della «гидрометеослужба флота». Chi non ascolta li perde.

## Ricarica realistica dei tubi (Enhanced)

`nucleo/armi_giocatore.hpp` (`StazioneArmi`), dal dossier `docs/ricerca/14_carico_armi.md` §1.5 e §5.4; prove
`ricarica:`. Si attiva da sola quando la missione passa ai siluri dell'Enhanced (`StazioniMissione::dopo_passo`). Nel
classico la stazione resta quella dell'originale: rilievo identico.

**Stati del tubo:**
- vuoto → **in ricarica** (dalla rastrelliera) → **carico** (l'arma nel tubo, asciutto) → in armamento (i 20 s
  dell'originale: allagare, equalizzare, aprire) → armato;
- il clic sul tubo è lo stesso ordine `carica_tubo` di sempre:
  - vuoto: ricarica, o in coda se due sono già in corso;
  - in ricarica: annulla, e l'arma torna in rastrelliera;
  - carico: arma;
  - in armamento o armato: disarma, e l'arma resta nel tubo;
- i tubi delle esche hanno i loro lanciatori e restano come nel classico;
- alla partenza i tubi delle armi sono carichi, presi dalle scorte della `dotazione`;
- dopo un lancio la ricarica parte da sola senza annuncio, e a ricarica finita il tubo si arma.

**Tempi** (`parametri_ricarica(classe)`, STIMA del dossier 14):
- 688 300 s per tubo;
- Alfa 75 s (caricatore automatico);
- Sturgeon 420 s (prima del carrello idraulico del 1976);
- Trafalgar e Victor III 240 s;
- le altre 300 s;
- al più due ricariche insieme.

**Silenzio a bordo** (ordine `silenzio_a_bordo`; per ora riguarda solo le ricariche e i loro transitori, più avanti
potrà toccare il rumore proprio):
- la ricarica dura il doppio e non fa rumore. Il rumore viene dalla velocità del carrello e dai colpi della catena e
  della culatta, che un movimento lento evita;
- fermarla del tutto, come in *Cold Waters*, renderebbe impossibile un lungo avvicinamento silenzioso dopo una salva.

**Transitorio:** fuori dal silenzio, un colpo ogni 60 s di lavoro. È un evento acustico della missione di 2 s, che
l'acustica dell'Enhanced vede come +10 dB sulla sorgente del giocatore: cinque colpi per un tubo del 688.

**Per il pannello:** `stato(tubo)`, `secondi_ricarica(tubo)`, `in_coda(tubo)`, `ricariche_in_corso()` /
`ricariche_max()`, `silenzio()`, `scorte_tutte()`.

**Messaggi:**
- 348 «ricarica iniziata» (solo a mano), 349 «in coda», 350 «annullata»;
- 351 e 352: inizio e fine del silenzio;
- 353–359 «tubo n ricaricato»;
- 360 «disarmato».

## Paesaggio sonoro (Fase 3, S5c)

Sorgenti non navali nella sezione `[sorgenti.*]` di `acustica.toml`. La libreria la scrive
`tools/pipeline/sorgenti_libreria.py`, con le voci del dossier 09 §5.3–5.6, e la esporta `acustica.py`. Il modello è
in `nucleo/src/paesaggio_sonoro.cpp`; le prove sono `paesaggio:`.

**Schema** (dossier 09 §6.2, `SorgenteLibreria`):
- categoria e sottocategoria;
- teatri, mesi e fasce orarie (giorno, crepuscolo, notte);
- densità per 1000 nm²;
- quote e velocità;
- estensione per le sorgenti diffuse;
- livello rms (i picco-picco perdono 9 dB) ed eco per l'attivo;
- aumento in una fascia oraria (il coro al crepuscolo);
- indizi, confusioni possibili, ascolto, fonte e confidenza.

**Componenti:**
- tonale; famiglia armonica (fondamentale, numero, decadimento);
- banda con picco e pendenza per ottava, normalizzata in modo che l'integrale valga il livello;
- sweep (energia distribuita sulla banda percorsa);
- impulso.
Tonali e famiglie possono seguire il ritmo (i grugniti del merluzzo, gli episodi del tremore d'iceberg).

**Ritmo:** continuo, periodico (intervallo minimo e massimo), Poisson (eventi all'ora) o treno (periodico a serie di
`serie_min` minuti con pause), con gruppi di 1–n eventi a intervallo interno. Gli istanti si ricavano da (seme,
indice dell'evento) con una funzione di mescolamento, senza stato: lo stesso tempo dà sempre la stessa risposta.
`attivita()` è la frazione della finestra d'integrazione in cui la componente suona, e il livello si media su quella.

**Libreria iniziale** (31 voci):
- **biologici:** balenottera comune (sweep 40→15 Hz in treni a gruppi), azzurra, megattera, minore, capodoglio (clic
  5–25 kHz), orca, delfini, foca barbata, merluzzo (famiglia a 50 Hz), eglefino e coro di pesci (diffusi, notte e
  crepuscolo), aringa, gamberi;
- **geofisici:** T-phase tettonico e vulcanico, vulcano sottomarino, distacco, tremore armonico (famiglia a 40 Hz) e
  incaglio d'iceberg, fessurazione e fusione del ghiaccio;
- **meteo:** risacca;
- **attività umane:** cannoni ad aria, battitura di pali, ecoscandaglio multifascio e profilatore, pescherecci a
  strascico e piccoli, fuoribordo, piattaforma, esplosione.
Vento, pioggia, traffico lontano e ghiaccio restano nel rumore ambiente.

**API per il runtime:**
- `popola_sorgenti(catalogo, teatro, mese, ora, centro, raggio, seme)` dà istanze con id da 100000: conteggio di
  Poisson dalla densità, posizioni nel disco;
- `muovi_sorgente(istanza, dt)`: la rotta cambia ogni 10 minuti;
- `sorgente_da_istanza(catalogo, istanza, tempo, finestra, ora)` dà la `Sorgente` con `fonte` al posto di `firma`.
  `valuta`, `cascata`, `spettro_fascio`, `righe_emesse` e `livello_banda_larga` la gestiscono. Le sorgenti diffuse
  allargano il lobo quanto il settore che occupano. Per le teste dei siluri i biologici sono falsi bersagli.

## Registro degli eventi ed esito dettagliato (M1)

`missioni/registro_missione.{hpp,cpp}` e' la base del debriefing evento per evento, della carriera (M3) e delle
statistiche (docs/12; progetto nelle note delle sessioni, progetto-registro-m1.md).

**Come registra:**
- Osserva soltanto: niente generatore casuale e nessun cambio di stato, quindi e' attivo anche nel classico, e il
  rilievo resta identico al riferimento.
- Si riempie alla fine di ogni passo di 2 s, prima che le unita' uscite si tolgano.
- Ci sono ganci alla fonte per la radio (eventi della sala, zone, trasmissioni e fix) e per le regole d'ingaggio.
- I danni li porta `Missione::raccogli_danni(sub, navi)`, che l'app chiama dopo `DanniSottomarini::avanza` e
  `DanniMissione::avanza`: rapporti del giocatore, SCRAM, camera di lancio allagata, emersioni E1, quota di collasso.
- Con lo stesso seme, gli stessi comandi e gli stessi passi gli eventi sono identici, con gli stessi `id`.

**API in `Missione`:** `eventi_missione()`, `momenti_salienti(n, spazio_s)`, `esito_dettagliato()`,
`raccogli_danni`, `classifica_contatto(giocatore, contatto, classe)` (classificazione data dal giocatore alla
stazione), `registro()`.

**Ogni evento (`EventoMissione`) ha:**
- tipo, tempo, posizione e quota;
- giocatore, unita' principale, altra (bersaglio o lanciatore) e arma, con classe, categoria e lato (ostile,
  neutrale, amico) e la categoria ROE;
- il giudizio ROE dei lanci del giocatore;
- la distanza dal giocatore;
- per le classificazioni: classe creduta, se e' giusta, la prima scoperta;
- codice e valore secondo il tipo;
- testi in italiano e in inglese;
- interesse 0..100 (funzione pura `interesse`, STIMA da tarare);
- `noto`: se il giocatore lo sapeva allora, per la verita' sovrapponibile;
- le pose delle unita' coinvolte;
- l'`Inquadratura` della minicartolina: soggetto, secondo, distanza, elevazione e azimut, oppure un'icona d'epoca
  per radio, ESM, radiogoniometria, danni interni, zone e sonar.

**Tipi:**
- inizio e fine;
- scoperta e classificazione: categoria con la durata del contatto a 800, classe a 1400, oppure data dal
  giocatore;
- lanci del giocatore e dei nemici, colpi, affondamenti;
- emersioni d'emergenza, siluri evitati, esche sedotte;
- danni subiti e domati, SCRAM, camera di lancio persa;
- messaggi ricevuti e persi, compiti scaduti, sessioni mancate, ordini ricevuti, trasmissioni;
- infrazioni alle ROE e ingressi nelle zone;
- fix di radiogoniometria, contro-scoperte (un'unita' ostile ha la posizione del giocatore: la prima volta, o dopo
  10 minuti), ESM a rischio alto, ping propri e subiti, cavitazione, boe vicine;
- urti col fondo e fuoco amico.

**L'esito dettagliato** (`EsitoDettagliato`):
- codice e testo dell'originale, e la categoria (da `categorie_esiti`, altrimenti dagli obiettivi; fuoco amico
  se il giocatore ha affondato amici o neutrali);
- gli obiettivi con stato, fatti e richiesti (formato in docs/09 §5.2; le 22 originali in data/missioni/obiettivi/);
- **completezza:** conta gli obiettivi attivi, cioe' affondare, raggiungere, scoprire, variabili ed esiti, con i
  primari doppi; i vincoli violati (proteggere, non essere scoperti, ROE) tolgono fino a meta'; sopravvivere resta
  fuori;
- **riuscita:** gli obiettivi attivi primari;
- i conti per la carriera:
  - armi: lanci e lanci a segno, per arma e per bersaglio;
  - affondati per lato, neutrali e amici colpiti, fuoco senza ordine, zone vietate;
  - classificazioni giuste ed errate e anticipo medio;
  - esposizioni per tipo e tempo esposto;
  - radio: ordini ricevuti ed eseguiti, rapporti richiesti e inviati, serie copiate e perse, sessioni mancate,
    compiti scaduti;
  - danni: danno massimo, incaglio, danno nucleare, danni domati, battello perso ed equipaggio salvo;
  - pericolo: distanza minima da un'arma ostile, tempo in contatto con unita' ostili, margine dalla quota di
    collasso (senza il modello dei danni, STIMA: 1,5 volte la quota massima);
- feriti e morti valgono 0 con `stimati = true`, finche' il modello dei danni (D6) non ha l'equipaggio.

**Prove:** test/prove_registro.cpp: scoperte, contro-scoperta, esposizione e obiettivi in una scena di prova;
l'esito dell'originale con la sua categoria; un lancio in SCENE1A; determinismo su SCENE4A; danni subiti.

## Carico e Sala siluri (M2)

`nucleo/carico.hpp`, dati in `data/motore/carico.toml` (`tools/pipeline/carico.py`, dal dossier
`docs/ricerca/14_carico_armi.md` §1–2 e §5); prove `carico:`. Vale solo nell'Enhanced: il classico parte con
`dotazione` e `tubi` della missione e non lo usa.

**Catalogo** (`carica_carico`):
- `SchedaArma` per arma, per la scheda della sala siluri e per la grafica:
  - guida, velocità, portata, testata, peso, lunghezza e diametro (0 = ignoto);
  - anno di servizio e di ritiro, posti in rastrelliera, pozzi verticali;
  - «solo IA», «what-if», nucleare;
  - sagoma (`modello:`, `sprite:` o `icona:`), fonte e confidenza;
- la `famiglia` è l'arma della stazione e del gestore di lancio delle missioni (mk48, type53, harpoon, tomahawk,
  sea_lance, esca, mina, attacco_terra, esca_mobile); `siluro` è la voce di `siluri.toml` per il modello C6b;
- esche mobili da tubo (famiglia `esca_mobile`, dossier 05 §5): MG-74 Korund (URSS, 1974) e MOSS Mk 70 (USA, 1976).
  Per il carico sono siluri: un posto in rastrelliera e un tubo da siluri. Il loro modello è `[esche.<id>]` di
  `siluri.toml`; due nel carico antisom dell'Alfa, nessuna nei carichi del 688. Le missioni senza deposito (le
  originali) ne aggiungono due alla dotazione. La MG-74 dell'Alfa, che è storica, va in rastrelliera prima dei
  siluri. La MOSS del 688, che è what-if, resta al deposito: nel modo moderno la carica il giocatore, nel 1989 è
  negata, e il carico predefinito è lo stesso nei due modi. Le missioni con deposito le hanno solo se le dichiarano
  (`esca_mobile` o la variante);
- `LocaleSiluri` per classe e variante (`los_angeles`, `los_angeles_flight2`, `los_angeles_688i`, `alfa`): tubi,
  rastrelliera, pozzi, esche, limite in acqua, e le posizioni della stazione Armi (`tubo`, `esca`, `pozzi`, `""`);
- i carichi standard per tipo di missione (antisom, antinave, attacco_terra, minamento, scorta) e la variante
  predefinita di ogni famiglia per nazione (`type53` → SET-65, `tomahawk` → TASM).

**Sala siluri** (`SalaSiluri`, dai `LimitiMissione` della missione):
- il deposito accetta famiglie o varianti; senza deposito vale la `dotazione` dell'originale, che è sia il massimo
  sia il carico predefinito;
- una variante prende dal deposito della famiglia se la scheda lo consente (`da_famiglia`), con il tetto dei primi due
  anni di servizio (ADCAP: 6 nel 1989); TLAM-C, USET-80, TEST-71M e le mine solo se la missione le dichiara;
- `disponibili()` dà ogni arma della marina con quante ne restano e il motivo del divieto: «in servizio dal …»,
  «ritirata nel …», «negata dalla missione», «what-if», «solo IA», «nucleare», «non al deposito»;
- il modo della partita: `SalaSiluri(…, what_if_giocatore, moderno)`. Le what-if marcate `moderno` (la MOSS sul 688)
  le consente solo il modo moderno; nel modo 1989 restano «what-if» anche con l'opzione del giocatore, che vale per le
  altre (Sea Lance);
- posti condivisi: tubi + rastrelliera − ospiti; i pozzi a parte e solo per le armi da pozzo; le esche a parte, fino
  alla scorta del locale;
- `metti` e `togli` per posto, `carico_standard(tipo)` adattato al deposito (un'arma mancante lascia il posto alla
  predefinita della famiglia, poi al siluro principale), `carico_predefinito()` come l'originale (sul Flight I il
  missile del tubo 7 va nell'ultimo tubo da siluri, sul Flight II nei pozzi), `carico_iniziale()`;
- `avvisi()`: tubi vuoti, posti e pozzi liberi, esche mancanti, minamento senza mine, armi what-if;
- `scarto_dal_standard()` misura quanto il carico si scosta dallo standard del tipo, per il debriefing e la carriera.

**Stazione Armi** (`StazioneArmi::imposta_carico`, prima del primo passo):
- le posizioni partono con le armi scelte, «carico» da armare, e la ricarica realistica si attiva;
- le scorte si tengono per variante, in rastrelliera, nei pozzi e fra le esche; la famiglia resta quella delle
  regole dell'originale;
- la posizione 7 del Flight II sono i pozzi verticali: dopo un lancio la cella successiva è subito carica;
- un missile in un tubo da siluri ha le regole del tubo 7 dell'originale; il TLAM-C vuole solo la quota periscopica;
- le mine si lanciano dal tubo come le altre armi (non contano fra le armi in acqua): la posa la fa il campo minato
  della missione (sotto, «Mine»);
- l'esca mobile (`Arma::esca_mobile`) si ricarica, si arma («esca pronta») e si lancia come un siluro e conta fra le
  armi in acqua come le esche dei lanciatori; al lancio la missione mette in mare l'esca della variante uscita dal
  tubo, non un siluro;
- ordine `scegli_arma_tubo` (`StazioneArmi::scegli_arma(tubo, id, tempo)` lo compone): un tubo vuoto prende l'arma
  alla prossima ricarica; carico la scambia con una ricarica intera, in coda se due sono già in corso; in ricarica
  ricomincia con l'arma nuova; armato o in armamento va prima disarmato;
- per il pannello: `variante(tubo)`, `lanciata(tubo)`, `pozzo(tubo)`, `rastrelliera()`, `pozzi()`, `esche()`,
  `varianti()`.

**Nella missione** (`StazioniMissione::imposta_carico(m, carico, catalogo)`, che l'app chiama dalla sala siluri prima
del primo passo):
- i campi della missione (`deposito`, `tipi_negati`, `tipo_missione`, `variante_battello`, `ospiti`, `what_if`,
  `docs/09_schema_missioni.md` §5.3) arrivano in `Missione::limiti_carico()`, da passare a `SalaSiluri`;
- `Missione::imposta_tubi` dà al gestore di lancio la famiglia di ogni tubo; a ogni lancio la stazione aggiorna le
  famiglie e passa con `Missione::variante_tubo` il modello della variante uscita dal tubo, cosi' un ADCAP o un SET-65
  corre con il suo modello C6b;
- il TLAM-C lanciato diventa solo l'evento `lancio_giocatore` nel registro (nessun bersaglio in mare);
- senza il carico della sala siluri resta il comportamento di prima (tubi carichi dalla dotazione).

**Efficacia** (`SchedaArma::efficacia`, `stelle(bersaglio)`): 0–3 stelle contro `sottomarino` (in immersione),
`superficie`, `terra` e come `mine`, per la tabella della sala siluri (dossier 15 §7.5); STIMA del gioco dalla guida e
dalla testata (dossier 14 §2), con nota, fonte e confidenza.

**Mine** (M2b; `nucleo/mine.hpp`, dati in `carico.toml` `[mine]`; prove `mine:`):
- Mk 67 SLMM (`influenza`): corre dal tubo lungo la rotta e la distanza (`rte`) impostate per il tubo (senza: rotta
  del battello e corsa intera, 13 km a 20 kn), si posa sul fondo fra 10 e 100 m e scoppia (150 kg) quando una nave o
  un sottomarino passa entro 60 m;
- Mk 60 CAPTOR (`captor`): ormeggiata 150 m sopra il fondo fra 200 e 900 m; un sottomarino in immersione entro 1000 m
  le fa liberare un Mk 46 (il modello dell'Enhanced); ignora le navi;
- PMR-1 e PMR-2 (`mina_razzo`): mine-razzo antisom da 1 posto, ormeggiate a 300 m (al piu' sul fondo) su fondali
  fra 200 e 1000 m; un sottomarino in immersione sopra di loro, entro 100 m (PMR-1) o 200 m (PMR-2), riceve il razzo,
  che scoppia a 10 m da lui (200 o 175 kg); ignorano le navi (dossier 14 §2.12, che corregge la mina-siluro di Naval
  Encyclopedia);
- MDT (`influenza`): mina corta da fondo, 0,5 posti (due per tubo), fino a 50 m di fondale, 600 kg entro 50 m;
- fuori dalle sue profondità (o arrivata a terra) la mina resta inerte; si arma 600 s dopo la posa; non distingue amici
  e nemici; agisce una volta sola. Raggi, profondità e cariche dove le fonti tacciono: STIMA;
- nella missione (`Missione::campo_minato()`, con `siluri_enhanced`, che legge anche le mine): la famiglia `mina` del
  tubo posa nel campo minato; gli scoppi passano per la regola dei colpi con un'arma della mina (lanciatore = chi l'ha
  posata, `Colpo::tnt_kg` e `influenza` con la geometria vera; il razzo delle PMR scoppia accanto al bersaglio), i
  siluri della CAPTOR sono unità del modello; `da_mina(id)` riconosce le une e gli altri, che non sono lanci del giocatore;
- registro M1: eventi `mina_posata` (livello 1: inerte) e `mina_esplosa` (livello 1: ha lanciato il siluro), conti
  `mine_posate`, `mine_inerti`, `mine_esplose` nell'esito, obiettivo `minare` (`docs/09_schema_missioni.md` §5.2);
  i colpi delle mine contano in `per_arma` sotto il tipo della mina, e la carriera li conta fra i colpi ma non fra i
  lanci.

**Messaggi:** 361 «cambio d'arma, ricarica iniziata», 362 «arma scelta per il tubo», 363 «prima va disarmato»,
364 «posa non possibile», 365 «arma non in rastrelliera», 366 «mina armata», 367 «mina posata», 368 «fondale
inadatto: inerte», 369 «esplosione di una mina».

## Carriera e fascicolo (M3)

`nucleo/carriera.hpp`, `nucleo/fascicolo.hpp` e l'adattatore `missioni/carriera_missione.hpp`; dati in
`data/motore/carriera.toml` (`tools/pipeline/carriera.py`, dal dossier `docs/ricerca/13_carriera_gradi_decorazioni.md`
§2.3, §3.7 e §5); prove `carriera:`. È deterministica: nessun dado, a parità di partite la stessa carriera. Vale con la
regola di simulazione; il classico tiene il rapporto d'esito dell'originale.

**Punteggio** (`punteggio_missione`): P = (R + C + S + O + E + D + V) × M, mai sotto zero.
- R riuscita (fino a 400); C completezza, classificazioni e rapporti radio (150); S discrezione, con i pesi delle
  esposizioni (ping 5, fix 25, avvistamento 20, ESM 15, transitorio 2, boa 10; a 100 vale zero); O ordini e serie radio,
  meno 30 per infrazione alle ROE (100); E colpi su lanci e carico vicino allo standard, meno 10 per arma incoerente
  (100; senza lanci la parte dei colpi vale 0,7);
- D danni, feriti, morti e armi perse (fino a −300, −300 con il battello perso); V neutrali, amici, fuoco senza ordine e
  zone vietate (fino a −200; squalifica da −400);
- M = situazione (pace 1, crisi 1,3, guerra 1,6) × avversario (il minore fra il dichiarato e quello calcolato dalle
  unità ostili; 0,8–1,5) × realismo (0,5–1) × (1 + pericolo, fino a +25%);
- con il punteggio, un giudizio a parole: il livello e la voce più forte e la più debole;
- acque territoriali straniere dichiarate dalla missione, in pace e in crisi (UNCLOS art. 20, dossier 22; S-363,
  «U-137», incagliato a Karlskrona nel 1981, dossier 23 E14):
  - ogni entrata toglie 30 agli ordini, come un'infrazione alle ROE;
  - farsi scoprire dentro (avvistati, un fix, un ping, l'avvertimento o il fuoco delle navi) è un incidente
    diplomatico: −150 alle violazioni, come un neutrale colpito, e il richiamo (USN) o il biasimo (VMF) col blocco
    delle promozioni;
  - nelle operazioni speciali l'entrata è l'ordine e la scoperta vale −75, senza richiamo; in guerra niente;
  - il riassunto (`riassumi`) conta le entrate, il tempo dentro e la scoperta dagli eventi del registro. STIMA.

**Carriera** (`Carriera::chiudi_missione`, che aggiorna il fascicolo):
- fasi compresse dal grado più basso al comando (USN da Ensign, VMF da lejtenant), oppure «carriera breve» da
  Commander o kapitan 2-go ranga con un fascicolo precompilato (`Carriera::nuovo`);
- uscite per numero di missioni, per qualifica (dolphins, esami di guardia) e per commissione: sotto zona (promozione
  anticipata), in zona e sopra zona; due mancate portano al congedo, o alla pensione dopo 20 anni; per la VMF la
  caratteristica del zampolit (±10% secondo ordini e ROE), e la promozione anticipata con una decorazione dalla Stella
  Rossa in su;
- calendario: 2 mesi per missione, 3 al comando; rendimenti decrescenti (1, 0,5, 0,25, 0) per la stessa missione;
- provvedimenti: neutrale o amico colpito → rimozione (corte marziale con infrazioni alle ROE; VMF degradazione);
  battello perso con l'equipaggio salvo → inchiesta (colpa o fatalità); perso con tutti → morte; incaglio, danno al
  reattore o fuoco senza ordine in pace → richiamo e due commissioni ferme; tre esiti mediocri → trasferimento a terra;
- indulgenza: realistica; indulgente (una seconda possibilità per carriera); arcade (nessuna rimozione, solo un malus);
- contano le originali, le rapide con il seme registrato e non modificate, e le missioni dell'editor con il blocco
  `carriera` solo nella carriera «aperta».

**Decorazioni:**
- una personale per missione, dalla tabella della marina per la situazione (pace e crisi insieme, guerra a parte);
  la seconda uguale vuole ×1,5 la soglia, la terza ×2; le classi del Servizio alla Patria e della Distinzione salgono;
  l'Eroe dell'Unione Sovietica porta con sé l'Ordine di Lenin; nessuna con violazioni;
- al coraggio (Navy and Marine Corps Medal, Ordine «Per il coraggio personale») per un incendio o una falla domati fuori
  dalla guerra, se è più alta;
- di reparto (MUC, NUC, PUC; encomio e Bandiera Rossa al battello per la VMF); automatiche di servizio (Sea Service
  Deployment, Expeditionary, Arctic, Combat Action Ribbon, Purple Heart; Servizio impeccabile per gli anni); distintivi
  (dolphins, Command at Sea, Comandante di sottomarino); fine del comando dopo sei missioni secondo la media;
- postuma, dopo la perdita del battello, solo per un'azione valorosa (P ≥ 600);
- `motivazione()` in italiano e in inglese dai modelli del dossier (§5.1–5.2), con i fatti della missione; vaga per
  le operazioni speciali;
- `barretta()`: i nastrini in ordine di precedenza con il numero e il dispositivo; i colori a righe sono in
  `carriera.toml` (`[decorazioni.*].righe`, in trentaseiesimi; orizzontali solo per la PUC).

**Fascicolo** (`nucleo/fascicolo.hpp`): TOML con la versione di formato, in `$XDG_DATA_HOME/melchior688/fascicoli/` o
`~/.local/share/melchior688/fascicoli/`. `salva_fascicolo` scrive in un file temporaneo e lo rinomina; contiene
anagrafica, grado, fase, calendario, incarichi, qualifiche, note caratteristiche (con la graduatoria «1 di N» fra i
pari grado simulati), decorazioni con le motivazioni, commissioni, provvedimenti e stato.

**Dalla missione** (`missioni/carriera_missione.hpp`): `riassumi(m, opzioni)` ricava il riassunto dall'esito
dettagliato e dal registro della M1 (armi incoerenti dai lanci, fatti dai momenti salienti d'azione, scarto dal carico
M2, armi perse con la camera di lancio); `contesto_carriera(m, regole)` dà situazione, avversario e difficoltà dal blocco
`carriera` della missione o dalla tabella delle originali e li confronta con quelli calcolati dalle unità ostili (`minaccia_calcolata`).

## Interazioni fra IA, fase 2: la parte acustica (docs/18_interazioni_ia.md)

La proposta è nella nota condivisa `ia-acustica-fase2-proposta.md`; la parte nella missione è del runtime.
- **TMA che risolve da sola** (`nucleo/tma_ia.hpp`, `risolvi_tma(tma, t, sigma_deg)`). Dal passivo l'IA avrà solo i
  rilevamenti, come il giocatore. La posizione la ricava cercando la soluzione col minimo scarto: una griglia di
  distanza, rotta e velocità, poi il raffinamento.
  - Dice se la distanza è osservabile dalla forma del minimo. Senza una manovra propria non lo è, e la posizione non
    si dà.
  - Prove `tma dell'IA`: con due gambe e 1° d'errore la distanza esce entro il 20%.
- **Il mare sui sensori vicino alla superficie** (`rumore_mare_proprio_db`), per tutti, da mare 5:
  - sonar di scafo e boe fino a +10 dB, niente oltre i 100 m;
  - schiere trainate fino a +6 dB, niente oltre i 150 m;
  - con mare 5 il 60%. STIMA (24 §7.4, 21 §6.1).
- **Il vantaggio fra le classi** del 24 §5.1 esce dal motore: la prova lo misura per sette coppie, entro 4 dB. Esempi:
  688 → Victor III +16,7 dB (dossier +15), 688 → Akula +6,7 (+3), 688 → Alfa +26,7 (+23).

## Statistiche (roadmap 6)

`nucleo/statistiche.hpp`; prove `statistiche:`. La proposta approvata è nella nota condivisa `statistiche-proposta.md`.
Vale dove c'è la traccia del replay, cioè con la regola di simulazione: il classico non ha statistiche.

- **Report di una missione** (`report_missione(traccia, catalogo)`): si ricalcola dalla traccia, anche da quella di
  una missione vecchia. Contiene:
  - tempi: in superficie (sotto 2 m), a quota periscopica (2-25 m), sopra la cavitazione; quota e velocità media e
    massima; distanza percorsa;
  - esposizioni: periscopio, antenna, schiera, trasmissioni, ping;
  - armi: per classe e per bersaglio, e gli affondamenti;
  - rilevamenti: la prima scoperta nemica (come, chi, quando, a che distanza); i nostri contatti, le classificazioni
    giuste ed errate e il tempo medio alla classificazione;
  - il livello irradiato al minuto, dalla firma della classe sui campioni di velocità e quota;
  - la heatmap a celle di 1 km.
  I campi degli eventi che servono (bersaglio, distanza, classi, lato, classificazione) sono nel record `E` della
  traccia dal 26/10/2026.
- **Cumulate** (`StatisticheCumulate`, `statistiche.toml` accanto ad `achievement.toml`): per comandante le missioni
  che contano per la carriera, per i globali tutte quelle non riprese (`registra_statistiche`). Somme, medie, primati
  e la heatmap per teatro a celle di 2 km.

## L'Alfa nell'Enhanced: Akkord e Boksit (dossier 19 §3.1-3.4 e §6)

Solo nell'Enhanced e solo con l'Alfa come battello del giocatore: il classico e il 688 non cambiano. La proposta
approvata è nella nota condivisa `alfa-boksit-tan-poks-akkord-proposta.md`; prove `alfa:`.

**Akkord** (`StazioneArmi`):
- fino a 3 bersagli designati (`akkord_designa`, e anche il `designa_bersaglio` di sempre): il primo slot libero,
  altrimenti il meno recente; `akkord_scegli` sceglie lo slot che va al lancio; i contatti persi liberano il loro slot;
- l'avaria: con la direzione del tiro (`Sistema::sonar_tiro`) sotto il 50% (STIMA) non si arma, non si lancia, non si
  imposta e non si guida. I tubi armati tornano carichi. La ricarica meccanica УБЗ continua fino al tubo carico. Solo
  dai danni, senza guasti casuali;
- messaggi 440-442.

**Boksit** (`StazioniMissione`):
- modi MANUALE, AUTOMATICO e PROGRAMMA per rotta e quota (`modo_boksit`);
- PROGRAMMA segue i punti di rotta della carta col pilota automatico. A ogni punto prende la quota (se anche la quota
  è a programma) e la velocità della sua tappa (`programma_tappa`), se date;
- toccare il timone lascia il programma; messaggi 447 e 448.

Gli ordini sono nello storico, quindi replay e ripresa li rifanno uguali. Tan, SFOGA, COMPENSA e il guasto delle
casse (ordini `tan`, `sfoga`, `compensa`) toccano il modello delle casse e sono del runtime.

## Traccia del replay (roadmap 16)

`nucleo/traccia_replay.hpp` e l'adattatore `missioni/traccia_missione.hpp`; prove `traccia:`. La proposta approvata è
nella nota condivisa `replay-traccia-proposta.md`. Legge soltanto: il classico non cambia.

- **Cosa registra** (`RegistratoreTraccia`): un fotogramma ogni 5 s, ogni 2 s per aerei e armi in acqua, e uno a ogni
  evento nuovo del registro M1. Contiene:
  - la verità di tutte le unità: posizione, quota, rotta, velocità, stato, alberi, lanciatore e filo;
  - il quadro del giocatore: contatti con lettera, qualità, rilevamento, posizione stimata e classe creduta;
  - le linee di rilevamento nuove della TMA e il bersaglio;
  - `DatiApp`, che riempie l'app: modo del sonar, sensore, fascio, filtro, stazione, silenzio, bersaglio e soluzione
    proposta.
- **File** `.m688tr`, autodescritto:
  - l'intestazione di testo ha i metadati e lo schema dei record coi nomi dei campi, il tipo e la quantizzazione: un
    lettore più nuovo legge per nome, ignora i campi e i record che non conosce e lascia ai predefiniti quelli che
    mancano;
  - i corpi sono interi quantizzati in varint zigzag, in differenza per unità e per contatto;
  - un fotogramma chiave ogni 60; ogni fotogramma porta la sua lunghezza, quindi uno rovinato si salta fino alla
    chiave dopo.
  - Due ore con 45 unità e un'arma in acqua: 1,28 MB.
- **Lettura**: `carica_traccia`, poi `Traccia::istante(t)` interpola posizione, quota e velocità in modo lineare e la
  rotta sul cerchio, mentre il quadro va a gradini; `eventi` e `durata` servono alla timeline.
- **Segnalibri**: gli eventi del registro col tipo, i testi, l'interesse, se erano noti e l'unità si scrivono nel
  fotogramma in cui compaiono (record `E`). `segnalibri(traccia, interesse_min, noti)` li dà in ordine di tempo; una
  traccia senza i testi dà il solo tempo e l'id. `fotogramma_da(..., reg.ultimo_evento())` copia solo quelli nuovi.
- **Ripresa**: il metadato `ripresa_da` segna da dove il giocatore comanda di nuovo.
- **Cartelle**: `replay/<comandante>/` in carriera, col nome del file in `VoceMissione::replay` del fascicolo;
  `replay/liberi/` fuori carriera e per le riprese, dove `pulisci_replay_liberi(20)` tiene gli ultimi 20.
- **Versione 2** (sandbox, docs/19; note condivise `sandbox-statistiche-contratto.md` e `sandbox-v2-viste-contratto.md`).
  Le tracce della versione 1 si leggono ancora, e un lettore della 1 salta i record nuovi.
  - **Record V:** stima della posizione, ellisse intera, allerta ed emissioni di ogni unità. Si scrive solo quando
    cambia: la stima di 10 m, l'ellisse di 5 m o 5°.
  - **Record E:** prende anche l'id dell'arma, la classe creduta e la posizione.
  - **Record I** (`EventoIa`), i fatti dell'IA che il registro non ha. Li raccoglie `RaccoltaTraccia`:
    - i contatti nuovi, solidi e persi di tutte le unità;
    - con `Missione::registra_statistiche` anche il primo istante possibile, i lanci con la stima, la stima della TMA,
      le evasioni con la minaccia, le emissioni e i punti nave dell'IA;
    - gli eventi dei siluri.
  - **Record K** (`ContattoUnita`), i contatti di ogni unità. C'è una riga ai cambi di stato; ai cambi di posizione o
    di rilevamento al più ogni 15 s; e una alla perdita, dopo 30 s d'assenza. `Traccia::quadro_unita(osservatore, t)`
    ricostruisce il quadro.
  - **Vista dal vivo:** `aggiungi_fotogramma(traccia, fotogramma)`.
  - **Dimensione:** due ore con 50 unità sintetiche fanno 1,63 MB; 1800 s della sandbox su S4, 26 kB.

## Statistiche tecniche della sandbox (docs/19 §4)

`nucleo/statistiche_sandbox.hpp`, prove `statistiche sandbox:`. `statistiche_sandbox(traccia)` calcola, dalla verità
della traccia v2 e per unità:
- **catene di scoperta:** chi vede chi, il sensore, la distanza, il ritardo dal primo istante possibile, la traccia
  solida e la classificazione giusta o no;
- **reazioni:** dalla prima scoperta di un ostile all'allerta, all'ingaggio e al lancio; dal primo siluro sentito
  all'evasione;
- **TMA:** gambe, tempo alla soluzione, rinuncia, e l'errore della stima al lancio, contro la verità;
- **armi:** esito (a segno, uccisione, esca, mancata, esaurita, in corsa), vittima, tempo col filo;
- **esposizione:** periscopio, antenna, schiera, radar, ping, radio, contro-scoperte;
- **navigazione:** errore medio e massimo della stima, punti nave.

Per parte (`StatParte`, chiave 0x08, 0x10, 0):
- perdite subite e inflitte, scambio;
- scoperte fatte e subite;
- Pk per classe d'arma, esche ed esche efficaci;
- medie di reazione, ingaggio, soluzione ed errore al lancio.

I valori mancanti sono −1: tracce vecchie, oppure dati che la partita non ha registrato. `StatisticheSandbox::esito`
ed `esito_testo` vengono dall'evento di fine del registro. Le partite sandbox sono fuori carriera, quindi non entrano in
`nucleo/statistiche`.

**Serie** (V3, nota condivisa `sandbox-v3-aggregazione-contratto.md`). `riassunto_partite(partite)` dà un
`RiassuntoSerie`:
- per parte e per nome di `numeri_parte`: media, deviazione campionaria, minimo, massimo, q10, mediana e q90, per
  interpolazione lineare. I mancanti non entrano, e `n` dice quante partite avevano il valore;
- le frequenze degli esiti, comprese le partite non finite (−1);
- il Pk per parte e per classe d'arma, sommato sulle partite.

Il vettore delle partite non si riordina. `riassunto_serie` è la sola parte `.parti`, per chi la usava già.

## Achievement e albo d'onore (roadmap 15)

`nucleo/achievement.hpp`, `nucleo/albo_onore.hpp` e l'adattatore `missioni/achievement_missione.hpp`; dati in
`data/motore/achievement.toml` e `data/motore/albo_onore_storico.toml` (`tools/pipeline/achievement.py`). Le prove sono
`achievement:` e `albo d'onore:`. La proposta approvata è nella nota condivisa `achievement-albo-onore-proposta.md`.
Vale con la regola di simulazione: il classico non cambia.

**Achievement:**
- sono 41, in sette categorie: tattico, stealth, tecnico, storico, carriera, editor e nascosto;
- hanno bronzo, argento e oro, oppure solo l'oro;
- le condizioni sono dichiarative, un elenco di (misura, operatore, valore), senza Lua;
- `misure_achievement(m, contesto, {armi_a_bordo, fascicolo, regole})` ricava le misure dall'esito, dal registro M1,
  dal contesto e dal fascicolo; una misura sconosciuta nel file è un errore al caricamento;
- ambiti:
  - `missione`: una sola missione;
  - `somma`: sommata sulle missioni, oppure i valori diversi di una chiave (missioni originali, codici delle rapide,
    marine);
  - `carriera`: dal fascicolo. Con `fascicolo_prima` conta solo quello che la missione cambia: il grado, le
    qualifiche e le decorazioni d'ingresso della carriera breve non si guadagnano.
- `valuta_achievement` restituisce i livelli saliti; un livello non scende;
- `registra_achievement(catalogo, misure, comandante, conta_comandante, ripresa, data, missione)` legge e salva
  `achievement.toml` accanto ad `albo.toml`:
  - per il comandante conta la missione che conta per la carriera (`Carriera::conta`);
  - per i globali conta ogni missione non ripresa;
  - una ripresa (`ContestoMissione::ripresa`) non conta per nessuno.
- origine `libreria`: conta per la carriera come `editor`, cioè col blocco `carriera`.

**Albo d'onore** (`albo_onore.toml`):
- le 20 migliori missioni, solo senza aiuti (realismo 1) e mai riprese; le rapide col codice;
- le 10 migliori carriere chiuse, caduti compresi, con la decorazione più alta per precedenza;
- `registra_missione_onore` e `registra_carriera_onore` leggono e salvano il file;
- `carica_storici` dà gli 8 comandanti storici del dossier 13 §3.3 e §3.6, con i fatti, la fonte e la confidenza,
  senza punteggi.

## Navigazione e posizione propria incerta (roadmap 26)

`nucleo/navigazione.hpp`; dati in `data/motore/navigazione.toml` (`tools/pipeline/navigazione.py`, dossier 06 §2, §6
e §7.3: quasi tutto STIMA DI GIOCO, con fonte e confidenza per voce). Le prove sono `navigazione:`. È solo il nucleo:
la missione e la carta del giocatore lo agganciano dopo, dietro l'Enhanced; il classico non cambia.

**Sistemi per classe** (`CatalogoNavigazione::sistema_di`; senza voce si toglie il suffisso, poi il predefinito):
- DMINS 1,5 nm/giorno (688 Flight I e Sturgeon), WSN-3 0,5 (688 con Tomahawk e 688i), SINS 1 (Trafalgar);
- Medvedica 1,75 (Victor III, Akula, Oscar, Typhoon), Soz 3 (Alfa), Sigma 3 (Charlie II);
- Kilo, Foxtrot e chi non ha voce: solcometro e girobussola, l'errore è la corrente non nota del teatro (0,2-1 kn).
- La deriva è l'errore radiale a 1 sigma dopo un giorno; sopra c'è l'oscillazione di Schuler (84,4 min, 0,15 nm);
  oltre 80°N la deriva raddoppia.

**`NavigazioneStimata`**, il navigatore di un battello (giocatore o IA, uguale):
- un filtro di Kalman a 4 stati: errore di posizione ed errore di velocità per asse, l'ultimo un Gauss-Markov
  (72 ore a inerziale, 12 ore la corrente);
- l'errore vero è simulato con lo stesso modello, dal seme; il filtro lo vede solo dai punti nave;
- `avanza(dt, latitudine, inerziale_ok)`: a inerziale guasto si passa alla stima;
- `stimata(vera)`, `errore()`, `ellisse(k)` (semiassi e orientamento; `ELLISSE_95` per la regione del 95%),
  `sigma_m()`, `sigma_velocita_kn()`;
- `punto_nave(valutazione)`: aggiorna stima e covarianza; due fix distanziati stimano anche la deriva.

**Punti nave** (11, `valuta_punto_nave(def, condizioni)`): dicono se si può e perché no
(`MotivoPuntoNave`), la precisione, la durata dell'esposizione, se emette e se vuole l'albero fuori:
- Transit e Parus: quota periscopica, sotto 8 kn, 0,05-0,15 nm più 0,2 nm per nodo d'incertezza della velocità;
  passaggi ogni 40-110 min secondo la latitudine, un quarto inutili (`prossimo_passaggio_s`, deterministico);
- GPS solo con l'opzione «1990+» e dal 1990;
- Loran-C e Čajka con la copertura per teatro; Omega (2-4 nm) e RSDN-20 col filo galleggiante fino a 60 m;
- l'Omega conta le corsie (8 nm) dalla stima: se questa è fuori di più di mezza corsia, il fix cade nella corsia
  sbagliata e il filtro ci crede;
- astronomico col cielo sereno e di notte o al crepuscolo; radar entro 20 nm dalla costa (emette); visivo in vista;
- batimetrico: un quarto della spaziatura delle isobate, tre volte peggio lungo l'isobata (l'ellisse si allunga), e
  con l'errore della carta degradata, che il filtro non sa: la stima va dove dice la carta.
- Marine: `nato` vale per tutte le occidentali, `vmf` per la sovietica.

## Interazioni fra IA, fase 4: manovre di TMA e armi dell'IA (docs/18_interazioni_ia.md)

Moduli puri del nucleo; il comportamento dell'IA nella missione lo collega il runtime. Dati in
`data/motore/interazioni.toml` (`tools/pipeline/interazioni.py`, il file unico del piano: le altre sezioni le aggiunge il
runtime); prove `manovre tma:` e `armi dell'IA:`. Il classico non cambia.

**Manovre di TMA** (`nucleo/manovre_tma.hpp`; dossier 02 §5.1-5.2, 24 §5.2):
- `dottrina_tma(catalogo, marina, classe)`: `[tma.nato]` e `[tma.vmf]`, con le differenze delle classi (Alfa più veloce,
  diesel più lenti);
- `ManovreTma(dottrina, seme)`, un seme per coppia (unità, contatto): il lato della prima gamba a parità e la durata
  delle gambe (±15% di max−min) sono ripetibili;
- `ManovreTma::passo(situazione)` dà la gamba:
  - il bersaglio sul mascone (45 gradi NATO, 60 VMF), poi sull'altro; la virata fra le gambe resta fra 30 e 90 gradi;
  - gambe di 3-6 minuti (4-8 VMF), più lunghe da lontano;
  - sotto i 4 km il bersaglio va oltre il traverso, così la gamba apre;
  - con la schiera, dopo la virata il raddrizzamento (120 s, 180 VMF) prima di contare la gamba;
  - il fondale: con `SituazioneTma::fondale` (la carta dell'unità) la gamba si controlla a un quarto, a metà e alla
    fine; sotto quota + 30 m il lato si scarta e si prova l'altro, con tutti e due sul basso si tiene la rotta
    (`fondo_rifiutato`); senza la carta vale `fondale_m`;
- `DecisioneTma::rinuncia`: contatto perso oltre la tenuta (600 s), specchio irrisolto dopo 3 gambe, nessuna soluzione
  dopo 8 gambe: chi comanda l'unità torna alla pattuglia;
- `soluzione_pronta` quando `risolvi_tma` è osservabile e la sigma della distanza sta sotto il 15% (20% VMF); se si
  guasta, le gambe riprendono;
- effetto (prova, 10 semi): un ascoltatore a 5 kn con un bersaglio a 15 km ha la soluzione in circa 12 minuti con il
  5% d'errore; senza manovre mai.

**TMA dell'IA corretta** (`nucleo/tma_ia.hpp`): l'intervallo delle distanze compatibili è ora quello a 1 sigma del
profilo, `N (s² − s_min²) / σ² ≤ 1`, coi bordi interpolati fra i punti della griglia; un secondo minimo staccato rende
la distanza ambigua, e un minimo sul bordo della ricerca (300 m) non è una soluzione. Prima la soglia non contava i
rilevamenti, e con 90 rilevamenti l'intervallo era cinque volte troppo largo: la distanza era quasi mai osservabile.

**Carico dell'IA** (`nucleo/armi_ia.hpp`, `carico_ia(catalogo, ia, classe, anno, tipo)`):
- 688 e Alfa: i carichi standard di `carico.toml`, come il giocatore; le altre classi `[armi_ia.<classe>]` (dossier 01
  §2, 14 §1): Sturgeon, Trafalgar, Victor III, Akula, Oscar, Typhoon, Charlie II, Kilo, Foxtrot;
- le varianti dell'anno (`variante_dell_anno`): non ancora in servizio o ritirate diventano la predefinita della
  famiglia, poi la più recente in servizio della famiglia (il Trafalgar del 1985 col Mk 48); il tetto dei primi due anni
  (ADCAP 6); niente what-if né nucleari;
- i tubi da 650 restano vuoti finché il 65-76 non è nel catalogo (`tubi_650_vuoti`);
- `carico.toml` ha il Tigerfish (Regno Unito, solo per l'IA), con la predefinita `[predefinite.uk]`.

**Catalogo unico:** `carica_interazioni(percorso)` legge insieme `[tma.*]`, `[armi_ia.*]` e `[filo_ia.*]`; le sezioni
del runtime (allerta, messaggi, zone, reazioni) le legge il suo loader in `missioni/`.

**Scelta dell'arma** (`scegli_arma_ia(carico, catalogo, sottomarino|nave, distanza, filo_possibile)`, con la variante,
il modello del siluro, il tipo per il percorso di lancio, la famiglia e il tubo): fra le armi nei
tubi, prima quelle in portata (90%), poi più stelle contro il bersaglio, poi la filoguidata se si può guidare, poi la
più lunga. Il Victor III contro un sottomarino a 10 km prende il TEST-71M, oltre 15 km l'USET-80, contro una nave il
53-65K.

**Guida a filo** (`GuidaFiloIa`, dottrina `[filo_ia.<esperienza>]` del sonarista):
- corregge la rotta verso la stima del bersaglio (o la linea del rilevamento) quando lo scarto supera la soglia (ogni
  10-60 s, oltre 5-15 gradi);
- rifiuta un'eco stazionaria lontana dalla stima (l'esperto e il medio; la recluta no);
- alla perdita del contatto: attivo, veloce e sulla stima nuova;
- taglia dopo l'aggancio (10-60 s) o vicino alla stima, quando il lanciatore evade e quando il filo è a rischio;
- `vincoli()`: `{attivi, velocita_max_kn, virata_max_deg_s}`, mentre guida il lanciatore sotto 12 kn e 2 gradi al
  secondo;
- effetto (prova): un Mk 48 lanciato 35 gradi fuori passa a 4 km dal bersaglio senza guida, guidato lo aggancia.

## Suoni del classico (nucleo/suoni_classici, docs/originale/suoni.md)

I suoni in partita di GAME.EXE del 1989: i sei campioni `.T` del disco 2 e i bip dell'altoparlante.
- **Dati:** `tools/pipeline/suoni_classici.py` copia i `.T` senza modifiche in `data/motore/suoni_classici/` (PCM a 4
  bit, prima il nibble alto) e scrive `suoni_classici.toml` con la provenienza (disco, byte, sha1, uso). I file
  restano nibble perché la frequenza di riproduzione la decide l'evento.
- **Il modulo osserva soltanto:** `SuoniClassici` riceve gli eventi e accoda i suoni in ordine:
  - i messaggi del gestore mostrati in barra;
  - gli altri testi in barra;
  - ping, lancio e ANALIZZA accettati;
  - la vista del periscopio, i colpi e i ridisegni della vista.

  Poi `prendi()` restituisce i suoni, e `pcm()` e `pcm_bip()` danno il PCM per `Mixer::aggiungi`.
- **Regole** (dall'eseguibile, indirizzi in `docs/originale/suoni.md`):

  | Evento | Suono |
  |---|---|
  | messaggi 7 e 50 | EXP1 a 5000 Hz, salvo con la vista del periscopio a schermo |
  | al periscopio, al quarto ridisegno di un'unità colpita nel campo | EXP1 a 5000 Hz |
  | messaggi 19, 20, 26 e 30 | EXP1 a 4000 Hz |
  | ping proprio | sonar1 a 5500 Hz |
  | messaggio 8 (il PING di qualunque altra unità) | sonar1 a 6200 Hz; un 8 ripetuto dà solo un bip grave, senza testo |
  | ogni lancio da tubo del giocatore | TORPRAMP a 6500 Hz |
  | ANALIZZA | prop2, propv2 o whale2 alla frequenza della classe |
  | ogni testo in barra | bip a 398 Hz |
- **Modi:**
  - `senza_campioni` è Ctrl-N: i bip restano;
  - `muti` è Ctrl-Q;
  - il bip del tasto c'è sempre.
- **Stime:**
  - la durata dei bip, presa su un XT (7,3 µs a giro del ciclo d'attesa);
  - il loro volume (0,35);
  - la cadenza dei ridisegni del periscopio, che nell'originale dipendeva dalla CPU.
- **Differenza dall'originale:** lì ogni suono blocca il gioco e ferma il tempo. Qui il gioco non si ferma, e
  l'ordine resta lo stesso mandando tutto in coda su un solo canale.

## Fisica dei danni

`nucleo/danni.hpp` (`docs/10_danni_affondamento_esplosioni.md` §2, fase D3; numeri dal dossier
`docs/ricerca/07_danni_affondamento_esplosioni.md`) simula una nave di superficie colpita: `DanniNave(dati,
seme)`, `colpo(Esplosione)`, `avanza(dt)`, `posa()` per il disegno.

- **Scafo:** le sezioni vere del modello del kit con l'opera viva, 60 stazioni, con la carena tarata sul
  dislocamento pubblicato (0,94–0,99). Celle stagne dalla compartimentazione per classe: paratia di collisione,
  locali uguali, casse laterali, macchine, depositi, stive o cisterne.
- **Idrostatica per stazioni a qualunque assetto:** le sezioni tagliate dal piano del mare, anche a nave sul
  fianco, capovolta o in verticale. Il GM intatto della classe fissa la quota del baricentro.
- **Acqua imbarcata:** come peso, con il pelo libero orizzontale nel mondo, quindi le superfici libere tolgono
  stabilita' da sole.
- **Flussi:** `Q = Cd A sqrt(2 g dh)` attraverso falle, perdite delle paratie, paratie squarciate o cedute
  (oltre l'altezza dello scafo di differenza di livello, l'80% sui mercantili) e aperture di coperta, che
  allagano quando la coperta va sott'acqua.
- **Dinamica:** sussulto, rollio e beccheggio con masse aggiunte e smorzamento, a 10 passi al secondo; livelli
  dell'acqua ricalcolati due volte al secondo. Costo: circa 0,6 ms per secondo simulato a nave danneggiata.
- **Trave di scafo:** momento flettente con i carichi d'inerzia (d'Alembert), contro la resistenza dal modulo
  minimo di regolamento (IACS) per un fattore della classe. Oltre la resistenza la nave si spezza: al piu' tre
  tronconi, e solo pezzi lunghi almeno il 30% della nave cedono ancora.
  - Ogni troncone prosegue per conto suo, con la sezione del taglio aperta, le paratie vicine squarciate e lo
    scafo scosso.
- **Esplosioni:**
  - a contatto, falla di D = 1,25 W^(1/3);
  - a distanza, fondo squarciato entro il raggio della bolla;
  - porte e paratie deformate entro 1,5 D;
  - rottura della chiglia per i siluri sotto la chiglia nel tratto centrale, con la regola tarata del dossier
    (sotto 130 m sempre con 150 kg di TNT per la bolla; 130–200 m nel 70% dei casi con 400 kg; navi grandi
    raramente);
  - prua o poppa asportate fuori dal tratto centrale, con probabilita' che cala con il dislocamento;
  - missili: falla sopra il galleggiamento e incendio all'interno.
- **Incendi:** per locale e sopra coperta (hangar, lanciatori). Crescono raddoppiando in 2 minuti, si estendono
  ai locali vicini e consumano il combustibile.
  - **Depositi:** allagamento d'emergenza in 2 minuti, il doppio a mano senza energia; nel frattempo rischio di
    esplosione (0,6% al minuto a fuoco pieno), che spezza la nave.
  - **Energia:** si perde se tutti i locali macchine sono allagati o in fiamme; senza energia pompe e collettore
    antincendio rendono meno.
- **Controllo danni**, con le squadre e la qualita' della classe e le priorita' del dossier 02 §9.2:
  - tamponatura delle falle fino a 1 m²;
  - lotta agli incendi;
  - pompe fisse (0,02 m³/s ogni 1000 t) e motopompe;
  - controallagamento oltre 4° di sbandamento.
  - Dopo l'abbandono nave (sbandamento oltre 25°, assetto oltre 8°, nave spezzata) nessuno lavora piu'.

**Verifica sui casi del dossier** (`engine/<cartella di build>/prova_danni CLASSE CASO [SEME] [SECONDI]`; 15 classi × 6 casi,
6 ore):

| Caso | Esito tipico | Dossier 07 |
|---|---|---|
| Mk 48 sotto la chiglia, fregate e corvette | spezzate; poppa a fondo in 3–8 min, prua in 4–9 | poppa 0,3–5 min, prua 2–120 (Cheonan, Torrens) |
| un siluro a contatto, corvette (Koni, Nanuchka, Krivak) | capovolte, a fondo in 12–21 min | fregate con falla grave 3–30 min |
| due siluri, Perry | capovolta in 2 min, a fondo in 23 | Royal Oak 13 min, Coventry < 20 |
| due siluri, incrociatori (Kara, Slava) | a fondo in 25 min – 4 ore | Belgrano circa 1 ora |
| un siluro, cacciatorpediniere | a galla, gravemente danneggiati | — |
| Iowa, Nimitz, Sacramento, Kirov | a galla con uno o due siluri | portaerei: 3–4 siluri |
| petroliera carica | a galla anche con due siluri | 11 affondate su circa 451 attacchi |
| mercantile | a galla con uno, a fondo con due in circa 3 ore | 10 minuti – 2 ore |
| missile da 165 kg su una nave militare | incendio di 1–2 ore in 5–9 locali; quasi sempre salva | Stark salva, Sheffield persa |

**Sottomarini in emersione** (24/10/2026, base della fase D6): `scafi/<classe>.toml` anche per gli 11
sottomarini (`tipo = "sottomarino"`, `tools/navi/compartimenti.py`).
- In emersione la fisica e' quella delle navi, con le sezioni dello scafo esterno senza il cassero allagabile.
  Compartimenti e casse zavorra di prua e di poppa sono celle; nei doppi scafi l'intercapedine ai fianchi e' una
  cella di zavorra per lato.
- Portelli chiusi a tenuta: nessuna apertura di coperta, anche dove la cima dello scafo e' sott'acqua. Le
  casse zavorra non bruciano e non perdono verso i compartimenti.
- Riserva di spinta tarata su quella pubblicata senza spostare il galleggiamento degli sprite
  (`docs/06_kit_navi.md`):
  - dove la geometria ne ha troppa, sezioni piu' strette sopra il mare (scafo leggero allagabile);
  - dove ne ha poca, zavorra iniziale nelle casse (`CellaScafo::acqua_iniziale`), tolta dal peso del battello
    con lo stesso equilibrio. Non conta come acqua imbarcata e le pompe non la toccano;
  - quando il battello imbarca acqua, l'esaurimento d'emergenza svuota le casse integre a 10 m³/s ciascuna.
- In piu', per D6 (`DatiScafo::sottomarino`, `Stazione::r_resistente`, `z_resistente`, `y_resistente`,
  `fuori_resistente`): scafo resistente per stazione (due scafi affiancati nel Typhoon), quote di collaudo e di
  collasso, dislocamenti pubblicati, depositi di missili fra i due scafi.
- Prova `danni: sottomarini in emersione`:
  - scafo resistente dentro lo scafo esterno, con il volume del file;
  - intatti in equilibrio entro 5 cm e 0,3°;
  - un Mk 48 sotto la chiglia spezza e affonda il 688;
  - un siluro leggero a contatto affonda il 688 (scafo singolo) ma non il Kilo (doppio);
  - riserva di spinta entro 0,05 da quella pubblicata (tranne il Trafalgar, 0,20 contro 0,12);
  - l'Akula colpito da un siluro leggero esaurisce le casse e resta a galla.
- Le missioni non cambiano: la fisica dei danni vale solo per le unita' di categoria `nave`.

**Sottomarino danneggiato in immersione** (`nucleo/battello.hpp`, fase D6a; progetto e numeri in
`docs/10_danni_affondamento_esplosioni.md` §10):
- `BattelloDanni(dati, seme)` sugli `scafi/<classe>.toml` dei sottomarini: compartimenti nello scafo resistente,
  casse zavorra, paratie, aria delle bombole, quota di collasso dal seme.
- `aggiungi_falla`, `squarcia_cassa`, `esaurisci`; `avanza(dt, {quota, velocita'})` restituisce peso netto e
  momento d'assetto.
- **Fisica:**
  - allagamento con la portata del dossier 02 §9.1, fermato dall'aria compressa;
  - paratie che cedono;
  - esaurimento con la legge di Boyle;
  - collasso con le implosioni in sequenza (energia p V), con gli eventi per messaggi e rumori.
- Non muove il battello: `rateo_verticale_regime` e' l'adattatore per il moto cinematico finche' non arriva la
  dinamica della Fase 2.
- **Nelle missioni** (`missioni/danni_sottomarini.hpp`, fase D6c, solo con la regola di simulazione):
  `DanniSottomarini` trasforma i colpi sui sottomarini in urti e limita la velocita' con i sistemi. Fornisce:
  - i messaggi 270–281;
  - `esaurisci(id)` per Ctrl+E;
  - `sistemi(id)`, `limiti(id)` per le stazioni;
  - `rateo_quota(id)` per il moto.

  - **Moto:** la quota dei battelli appesantiti o alleggeriti segue il peso netto della fisica
    (`OrdineMotore::quota_fisica`) invece della barra.
  - **Dove scoppia il colpo:** con la geometria vera (siluri del modello, bombe, mine) lo scoppio si porta nel
    riferimento della vittima:
    - la distanza e' quella dallo scafo, come da un cilindro lungo quanto il battello, non dal centro: la spoletta del
      modello scatta a 10-15 m dal centro, che su un battello di 70-110 m e' spesso a contatto;
    - il punto lungo lo scafo e' quello dello scoppio, non piu' scelto a caso;
    - nei doppi scafi l'intercapedine (`intercapedine_m` dello scafo, 1-1,2 m sui sovietici) si aggiunge alla distanza.
  - **Teste in TNT equivalente:** Mk 48 544 kg (295 kg di PBXN-103), Mk 46 108 kg (44 kg di PBXN-103), dossier 07 §1.5;
    AT-1 105 kg (70 kg x 1,5, STIMA).
  - **Misura ripetibile:** `engine/build/letalita <dati> <arma> <classi,...> <contatto|lato:D|sotto:D|spoletta> [semi]
    [ore] [quote]`. Un colpo con la geometria vera, poi ore di missione; per ogni prova fattore d'urto ed esito, per
    classe e quota il riepilogo (numeri in `docs/10` §10.17).
  - **Esiti misurati** (25/10/2026, un colpo di Mk 46 con la spoletta del modello, 12 semi, 4 ore):
    - a 50 m il Victor III va perso 7 volte su 12 e le altre emerge danneggiato; il Foxtrot 5 su 12;
    - a 150 e 250 m vanno persi sempre, perche' sotto i 150 m l'emersione d'emergenza non riesce (dossier 07 §8.4);
    - fuori combattimento quasi sempre (Foxtrot a 50 m: 9 su 12; gli altri 3 restano in quota con piccole falle).
    - Prima della correzione la distanza era misurata dal centro e la testa contava 44 kg: nessun affondamento in 4 ore.
  - **Collegamenti nell'app:** in `main.cpp` e nelle stazioni con `--regola simulazione` (`docs/10` §10.8).
  - **Emersione violenta** (E1, `docs/10` §10.14): un battello dell'IA che arriva in superficie dopo l'esaurimento
    delle casse a piu' di 1 m/s entra in `emersioni()`:
    - la posa (`posa_emersione`) fa uscire la prua fino a 10-35 gradi, poi la fa ricadere, rollare e assestare;
    - `Effetti::emersione_violenta` fa l'acqua che cola, la colonna alla prua, la schiuma e gli spruzzi del colpo.
  - **Lanci e piuma** (E2, `docs/10` §10.15):
    - `Effetti::lancio_missile` disegna capsula, booster e fumo dei missili lanciati dall'immersione (Harpoon,
      Tomahawk, Granit, Malakhit, Vodopad);
    - `Effetti::piuma` fa lo spruzzo e la scia degli alberi alzati, crescenti con la velocita';
    - `Particella::propulsa` e' il moto rettilineo dei corpi e delle fiammate.
- Prova `battello: sottomarino danneggiato in immersione`: portate della tabella del dossier, paratie, esaurimento
  che riesce a 100 m e non a 400 m, salvezza con avanti tutta e barra in salita, collasso fra 1,1 e 1,8 volte la
  quota nominale, determinismo.
- **Urto e sistemi** (fase D6b):
  - `urto(tnt, distanza, angolo, x, quota)` calcola il fattore d'urto del dossier 02 §9.4, con la soglia ridotta in
    profondita';
  - ne seguono scosse, guasti graduati di 12 sistemi, SCRAM, perdite, falle, casse squarciate e rottura dello scafo
    resistente;
  - l'equipaggio tampona, pompa e ripara con le priorita' della dottrina (`priorita(sistema)` del giocatore);
  - `limiti()` da' velocita' massima, timone, idroplani, periscopio, antenna, sonar e schiera per i comandi.
  - Prova `battello: urto, guasti dei sistemi e riparazioni`: Mk 48 a 10 m sotto letale in almeno 40 casi su 50,
    carica da 135 kg a 30 m senza falle a 50 m e con perdite a 250 m, riparazioni, SCRAM di 5–20 minuti (nessuno
    sui diesel), priorita'.

**Limiti:**
- sottomarini in emersione con celle a tutta sezione; in immersione il modello D6a non e' ancora agganciato al
  moto (D6c);
- sovrastrutture senza spinta;
- nessun moto orizzontale, tranne la separazione dei tronconi;
- l'acqua dei pompieri non si accumula;
- resistenza della trave e soglie di cedimento delle paratie sono stime tarate sui casi, perche' dati pubblici
  per le navi militari non ce ne sono.

## Effetti visivi

`grafica/effetti.hpp` (`docs/10_danni_affondamento_esplosioni.md` §5.3-5.6, fase D4) mette in scena le esplosioni
e la fisica dei danni con particelle nel mondo 3D. I numeri vengono da `nucleo/fenomeni.hpp`, che codifica il
dossier 07 (§1.4, §2, §11).

- **Esplosione subacquea** (`Effetti::esplosione_subacquea(pos, profondita, W_bolla, fondale)`), con il regime
  dalla profondità relativa al raggio massimo della bolla:
  - **anello scuro** dell'urto, `r = sqrt((1500 t)² − d²)` fino a 18·W^(1/3) m, meno di 0,3 s;
  - **cupola di spruzzi** di Kolsky (v₀ = 86·W^(1/3)/d − 6,6), alta 0,45·v₀²/2g ma mai più di metà del
    pennacchio;
  - **pennacchi** dalla tabella di Swisdak (y = d/W^(1/4)), all'impulso della bolla (T₁ di Willis), salita in
    0,8·sqrt(2H/g):
    - **sfiato** (d/A < 1): colonna cava con la corona scura dei gas in cima;
    - **getto** (1–1,5): getto sottile con la gonna di spruzzi;
    - **pennacchi** (1,5–3): da 2 a 5 getti inclinati fino a 15°;
    - **cupola bassa** o **profonda**: un pennacchio basso; oltre y = 9 solo una gobba di qualche metro;
  - **ricaduta, base surge e nebbia** sospesa per 4 + 0,15·H secondi;
  - **chiazza** di schiuma (marrone di fango se la bolla tocca il fondo) che ribolle per minuti;
  - **pulsazioni** successive a T₁ + T₂ e T₁ + T₂ + T₃.
  - Il seme varia numero, forma e inclinazione dei getti, e altezze e raggi di ±20%.
- **Esplosione nell'aria** (`esplosione_aerea`):
  - lampo sulla palette (`Effetti::lampo()`, `Palette::schiarita`);
  - palla di fuoco di 3,5·W^(1/3) m;
  - schegge balistiche che finiscono in mare con uno spruzzo;
  - sbuffo di fumo del missile, oppure fiammata e fungo del deposito (45·W^(1/3) m).
- **Nave seguita** (`Effetti::segui(id, &danni)`):
  - gli scoppi registrati da `DanniNave::scoppi()` diventano esplosioni nel punto giusto dello scafo in posa;
  - **fiamme** dai locali che bruciano: calore stimato in 0,5 MW/m², altezza di Heskestad;
  - una **colonna di fumo** per troncone, pennacchio di Briggs piegato dal vento:
    - nera per gasolio e carico, grigia per locali e depositi;
    - di notte accesa dal basso vicino alle fiamme;
  - **vapore** dove l'acqua spegne il fuoco;
  - **orlo di schiuma** sul galleggiamento inclinato;
  - **ribollio d'aria** sopra le falle che imbarcano acqua;
  - **spruzzi** quando lo scafo si muove in fretta o si spezza;
  - **dopo l'affondamento**: ribollio, vortice di schiuma, chiazza di nafta per ore, rottami e zattere alla deriva.
- **Disegno**: dischi con bordo frastagliato (rumore di valore) e trasparenza retinata con Bayer.
  - Fuoco su una rampa emissiva (175–178 e i posti liberi 190 e 192), che di notte non si scurisce.
  - Fumo, spruzzi e schiuma sui grigi della vista. Fango e fuliggine in 196–198. Colori nuovi da
    `tools/pipeline/esporta_motore.py`.
  - Chiazze, nafta e anello scuro scuriscono o schiariscono il mare: dalla quota del periscopio sono quasi sempre
    una riga sola.
  - Niente si disegna sotto il pelo dell'acqua; oltre l'orizzonte vale il limite delle navi, quindi una colonna
    di fumo si vede anche quando la nave non si vede più.
- **Opzioni di prova**:
  - `--onde M`, `--visibilita KM` (onde e foschia, sotto);
  - `--scoppio KG,PROF,DIST,RIL[,FONDALE]` (esplosione subacquea);
  - `--aerea KG,QUOTA,DIST,RIL[,deposito]` (esplosione nell'aria);
  - `--vento DA,M/S` (predefinito da 250° a 7 m/s).
- **Tavole di verifica**: `tools/pipeline/tavola_effetti.py USCITA.png "titolo|opzioni|t1,t2,..."`. Esempio:
  `extracted/navi/effetti_d4.png`.
- **Costo** (CPU di sviluppo):
  - un incendio con la sua colonna: 4–6 ms per fotogramma;
  - il caso peggiore, due pennacchi di 250 m a 4 km al 6× che riempiono la vista: circa 20 ms.

**Deformazioni del modello** (§5.2), nella posa (`PosaNave::segni`, `parti`, `flessione_*`):
- **falle nere** dal bordo frastagliato (rumore di valore 3D), con un alone di lamiere bruciate, dove la fisica
  ha aperto lo scafo: si vedono sopra il galleggiamento e sulla carena di una nave capovolta;
- **fuliggine** a chiazze attorno ai locali che hanno bruciato (`Cella::annerimento`: annerita in un quarto
  d'ora di fuoco pieno);
- **alberi che cadono**: gruppi di parti del kit (`albero*`, `palo*` e cio' che portano) ruotati attorno alla
  base come un'asta rigida, θ'' = 3g/(2h) sin θ, fino a coricarsi in circa 2 s; cadono per uno scoppio vicino
  (lontano dallo scoppio), per la frusta della bolla (probabilita' 1/2) o per la rottura dello scafo vicina. Se
  la cima finisce fuori bordo, spruzzo;
- **nave sollevata e inarcata** dalla bolla sotto la chiglia (dossier 07 §3.3): spinta verso l'alto di 3 m/s
  nella fisica, e nel disegno un inarcamento di 0,4 A_max (circa 4 m per un Mk 48) che sale per mezzo periodo
  della bolla e poi diventa insellamento con un'oscillazione smorzata.

**Onde e foschia** (`VistaPeriscopio::altezza_onde_m`, `visibilita_m`; opzioni `--onde M`, `--visibilita KM`):
- le creste davanti a navi e colonne ridipingono di mare le righe fra cresta e galleggiamento. Le onde sono
  lunghe 15 Hs + 10 m e corrono alla velocita' di fase sqrt(gL/2π);
- la foschia schiarisce i grigi degli sprite verso il chiarore dell'orizzonte con la legge di Koschmieder,
  contrasto exp(−3,912 r/V), e toglie opacita' alle particelle.

**Limiti:**
- niente schiuma sulle creste e niente mare mosso disegnato: le onde nascondono soltanto;
- gli alberi caduti restano sulla nave (non scivolano in mare).

## Stazioni della Fase 3: grafica del 688 e dell'Alfa (dossier 19)

Il restyle delle stazioni (docs/ricerca/19_consolle_688_alfa.md) nella libreria grafica. Le stazioni lo collegano
dietro il flag dell'Enhanced: il classico non lo vede.

- **Font** (`tools/pipeline/font_stazioni.py` → `data/motore/font/*.fnt`, formato FNT2 di `grafica/font.hpp`):
  - disegnati da zero da un tracciato 5×7 alla Hershey;
  - «Fosforo 688» 5×7 e 7×9 col suo alone; «Incisione 688» alla Gorton con i capi a mezzo tono;
  - «Stampino URSS» alto 10 e 7 nelle larghezze del GOST tipo Б, con le varianti a ponticelli e inclinata;
  - cifre a 7 segmenti coi segmenti spenti a mezzo tono.
  - `Font::scrivi_toni` disegna i tre livelli. FNT1 (l'originale) si disegna come prima, una prova lo confronta pixel per pixel.
- **Temi** (`grafica/tema_stazioni`): negli indici 16-47 smalto, accenti, lampade, targhette e fosforo del battello.
  La luce della sala (bianca, rossa, blu) tinge le superfici; lampade e fosfori sono emissivi. Le fasce della palette
  sono in `grafica/palette.hpp`.
- **Strumenti** (`grafica/consolle`):
  - pannelli, targhette, quadranti scuri o bianchi con il comandato, barre;
  - lampade a lente o tonde, finestrelle d'allarme, pulsanti illuminati e coperti, manopole, levette;
  - tamburi, 7 segmenti, CRT rettangolari e tondi sotto la cappa, cloche, albero di Natale.
- **Sale e pannelli** (`grafica/sale`), coi dati in struct semplici (DatiGoverno, DatiArmi, DatiStato, DatiRadioEsm) e
  le aree cliccabili coi comandi di `nucleo/mappa_tasti`:
  - pannelli di governo e d'armi dei due battelli;
  - Ritm dell'Alfa; radio ed ESM;
  - la centrale del 688 (pedana dei periscopi) e quella dell'Alfa (ferro di cavallo);
  - Okean a ventaglio; la cornice del periscopio e della TV-1; la sala sonar in luce blu; il Sozh col pennino.
- **Stazione Sonar** (`app/sonar_enhanced`):
  - `disegna(tela, font, area)` mette cascate, LOFAR, testo e libreria in un rettangolo qualsiasi, per esempio quello
    libero di `cornice_sala_sonar` (688); nella vista intera le immagini sono identiche a prima;
  - `disegna_okean(tela, font, font_consolle, area)` è la stazione dell'Alfa (dossier 19 §6.6): lo schermo tondo di
    Enisej al posto della cascata dei rilevamenti, col ventaglio dei rilevamenti relativi (l'ultima riga integrata),
    il fascio e i tracker; accanto, il LOFAR a cascata e sotto il testo; le targhette OKEAN, ENISEJ e la lampada di
    Luch;
  - `livelli_relativi()` dà i 360 livelli per Okean.
- **Controllo:**
  - `engine/build/anteprima_stazioni <dati> <uscita> 688|alfa bianca|rossa|blu strumenti|pannelli|viste|cornici|sala`, con le tavole in `extracted/stazioni/`;
  - le prove in `test/prove_stazioni_grafica.cpp`.

## Immagini del Museo (enciclopedia, requisito 14)

Tre disegni per i tipi d'immagine dei descrittori di `data/motore/enciclopedia.toml` che non sono sprite, modelli o foto.
Li chiama la schermata del Museo (`app/museo`, sessione principale) e il controllo `strumenti/anteprima_enciclopedia`.

- **Cascata** (`grafica/cascata_firma`):
  - BTR e LOFAR fermi, nella scala in fosforo verde della stazione (`applica_palette_cascata`, indici 200-231);
  - la classe passa al traverso di un ricevitore in ascolto a 3 kn, per un quarto d'ora; rumore ambiente medio (mare 3, traffico 0,5);
  - nella metà più vecchia va piano, sotto la cavitazione, e nel LOFAR spiccano le righe dei macchinari;
  - poi accelera (rampa dal 50 al 60%): le righe saltano per Doppler, la pala e la cavitazione accendono il BTR e coprono le righe;
  - fisica della stazione (`cascata`, `spettro_fascio` con la grana), più il normalizzatore a due passi degli schermi LOFAR (±4 Hz attorno a ogni bin, il centro escluso);
  - la distanza è la più lontana, tra 40 km e 500 m, in cui nella metà lenta spiccano tre righe di 8 dB; i battelli quieti (Los Angeles, Kilo in batteria) restano quasi muti, come nel modello.
- **Carta** (`disegna_carta_museo` in `app/carta`):
  - il disegno VGA della carta d'epoca (`disegna_fondale_vga`: fasce, ombre, isobate, zone non rilevate);
  - attorno a una base, 150 km di larghezza col segno rosso e bianco; per un teatro, la griglia intera;
  - scala in miglia in basso a sinistra.
- **Spaccato** (`disegna_spaccato_sistema` in `grafica/spaccato`):
  - lo spaccato intatto col contorno rosso del sistema, come un guasto del quadro danni;
  - i nomi dei descrittori fuori da `Sistema` si traducono: reattore → propulsione, centrale → periscopi e idroplani, direzione del tiro → consolle del tiro, boa radio e filo → antenna;
  - il contorno dei sistemi guasti ora si disegna sopra l'acqua: la sfera del BQQ-5 sta nella cassa di prua, sempre piena, e prima il suo guasto non si vedeva.

Prove in `test/prove_museo_grafica.cpp`:
- la scena della cascata, i sensori sostitutivi, la tonale del Victor III a 250 Hz nella metà lenta e il determinismo per seme;
- il segno della base e il teatro intero;
- i nomi dei sistemi e il contorno del reattore e della sfera.

## Prossimi passi (Fase 1)

- audio con ymfm (OPL2/OPL3) e PCM;
- comandi del giocatore (telegrafo, timone, quota, armi) e API di livello alto per le missioni Enhanced;
- debriefing con il rapporto dell'esito;
- danni ai sistemi dopo un urto col fondo (§9.4 dell'originale: sonar, schiera, periscopio, motori…), con le
  stazioni del giocatore.
