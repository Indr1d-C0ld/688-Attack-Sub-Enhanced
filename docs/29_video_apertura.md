# Il video d'apertura (roadmap 37)

**Proposta di trailer-1, 31/10/2026, realizzata il 06/11/2026 (vedi Stato; dove il testo sotto e la realizzazione differiscono vale la realizzazione).** Dall'idea dell'utente del 31/10: un video all'avvio del gioco,
girato in tempo reale col motore, saltabile con un tasto, senza riquadri promozionali. Un mini-racconto simbolico, dal
688 e dall'Alfa, che cresce fino al culmine.

---

## 1. L'idea

**«Lo stesso mare.»** Due battelli che non si sono mai visti, ciascuno nel suo silenzio, si cercano. Il racconto è
tutto in specchio: quello che vede il 688, poco dopo lo vede l'Alfa, con gli strumenti dell'altra parte. Nessuna
parola, nessun nome, nessun riquadro. Solo suoni, strumenti e mare.

- **Cresce in quattro tempi:** il silenzio, la traccia, la caccia, il culmine.
- **Il culmine non si risolve.** Due siluri in acqua, uno per parte, le due cascate che si riempiono di rumore, poi
  il nero e il silenzio. Dal nero esce il titolo del gioco e, sotto, il menu. Chi ha vinto lo decide il giocatore.
- **Simbolico, non spettacolare:** niente esplosioni. Il punto è l'ascolto, come nel gioco.

## 2. Come si gira: dal vivo, non un filmato

Il video non è un file mp4: è una **regia** che il motore recita all'avvio, come i vetrini.
- Una o due scene di prova nuove (le scrivo io, come le T1-T9 in `data/missioni/prove`): il 688 e l'Alfa nello stesso
  teatro, sui fianchi di un gruppo di superficie, con un seme fisso, così la partita va sempre uguale.
- **Un file di regia** (`data/motore/apertura.toml`): un elenco di inquadrature, ognuna con la scena, l'istante, la
  durata, la stazione (periscopio, sonar, carta, sala siluri, manovra), il battello da cui si guarda, la compressione
  del tempo e i parametri della vista (rilevamento, ingrandimento, lettura della cascata). Il motore salta da una
  all'altra come fa la principale col vetrino dal vivo.
- **La musica:** un tema breve in FM (ymfm, roadmap 38), scritto come quelli dei trailer, sulle battute del montaggio.
  I suoni sono quelli del gioco (ping, transitori, il tonfo del tubo, il rumore del siluro in cascata).
- **Saltabile:** qualsiasi tasto o clic porta al titolo. Fra le opzioni: «video d'apertura: sempre, solo la prima
  volta, mai».
- **Durata:** 1 minuto e 40 circa, sulle battute.
- **Lingua:** non serve, perché non c'è testo.

## 3. La scaletta

Le battute sono a 72 alla semiminima (3,33 s), come nel trailer n. 3: 30 battute, 1:40. Le scene «A» e «B» sono le due
scene di prova nuove; i tempi sono provvisori.

| # | Battute | Tempo | Inquadratura | Da dove |
|---|---|---|---|---|
| 01 | 1-2 | il silenzio | nero; un ping lontanissimo e il suo eco nel buio | solo audio |
| 02 | 3-4 | il silenzio | la cascata del 688: rumore di mare, nessuna riga | A, il 688, sonar |
| 03 | 5-6 | il silenzio | la cascata dell'Alfa, nei colori sovietici: rumore di mare, nessuna riga | A, l'Alfa, sonar |
| 04 | 7-8 | il silenzio | la carta del 688 vuota, la sua sola posizione e l'ellisse | A, il 688, carta |
| 05 | 9-10 | la traccia | sulla cascata del 688 nasce una riga debole | A, il 688, sonar |
| 06 | 11-12 | la traccia | sulla cascata dell'Alfa nasce una riga debole, allo stesso tempo | A, l'Alfa, sonar |
| 07 | 13-14 | la traccia | il periscopio del 688 al tramonto: una nave sovietica all'orizzonte | A, il 688, periscopio |
| 08 | 15-16 | la traccia | il periscopio TV dell'Alfa: una fregata americana, in specchio | A, l'Alfa, periscopio |
| 09 | 17-18 | la caccia | le due carte affiancate: due rilevamenti che si incrociano | A, carte dei due |
| 10 | 19-20 | la caccia | il 688 che accosta (Crazy Ivan) nel profilo di quota; l'Alfa che scende sotto lo strato | A, manovra |
| 11 | 21-22 | la caccia | la sala siluri del 688: un tubo che si allaga | A, il 688, sala siluri |
| 12 | 23-24 | la caccia | la sala siluri dell'Alfa: lo stesso | A, l'Alfa, sala siluri |
| 13 | 25-26 | il culmine | il tonfo del lancio e il transitorio sulla cascata del 688; subito dopo, quello dell'Alfa | B, sonar |
| 14 | 27-28 | il culmine | le due cascate affiancate che si riempiono del rumore dei siluri; la musica sale | B, sonar |
| 15 | 29 | il culmine | nero e silenzio | — |
| 16 | 30 | il titolo | il titolo del gioco esce dal nero; poi il menu | titolo |

## 4. Che cosa serve, e da chi

| Pezzo | Chi | Stima |
|---|---|---|
| Le scene A e B (scenario, geometria, seme, durate), la regia `apertura.toml`, la scaletta finale | trailer-1 | 2-3 g |
| Il lettore della regia all'avvio: scene senza giocatore, cambi di stazione e di battello, compressione, salto con un tasto, l'opzione | principale | 2 g |
| La vista di due cascate o due carte affiancate (un'inquadratura «doppia», oggi non c'è) | principale con trailer-1 | 1 g |
| Il tema in FM e i suoni sulle battute | trailer-1, con chi farà la roadmap 38 | 1 g |
| Il titolo che esce dal nero (dalla schermata del titolo che c'è) | kit | 0,5 g |
| Le scene girano nell'Enhanced e non cambiano il classico | runtime | niente di nuovo |

Le stime sono in giorni di lavoro di una sessione e sono **STIMA**.

## 5. Domande aperte per l'utente

1. **Il culmine:** due siluri e il nero, senza esito, ti va? L'alternativa è un solo siluro, con l'altro battello che
   si salva con un'esca: più narrativo, meno simbolico.
2. **Il titolo:** solo il titolo del gioco, o anche il nome del remake?
3. **Le inquadrature doppie** (le due cascate, le due carte) sono il centro dello specchio: tenerle anche se costano
   una vista nuova?
4. **La musica:** un tema nuovo per l'apertura, o il tema del menu che c'è?

## Stato

- 31/10/2026: proposta di trailer-1, da portare all'utente.
- 31/10/2026: risposte dell'utente al §5: culmine con **due siluri e il nero**, senza esito; titolo **del gioco e del
  remake**; le **inquadrature doppie** si tengono; **tema nuovo** in FM. Realizzazione **rimandata**: l'utente la tiene
  per quando si potrà fare, nel modo descritto (dal vivo col motore, saltabile, senza riquadri promozionali).
- 06/11/2026: **realizzato** da trailer-1, prima versione. Non ancora visto né sentito in finestra: la prova tocca all'utente.
  - **File:** `engine/app/video_apertura.*` (il lettore), `data/motore/apertura.toml` (la regia), `data/motore/apertura/APERTURA_688.lua` e
    `APERTURA_ALFA.lua` (da `scene_interazioni --apertura`), `data/motore/musica/fm/apertura.mid` (da `tools/pipeline/musiche_fm.py`),
    `engine/test/prove_video_apertura.cpp`. Senza finestra: `melchior688 --schermata apertura --secondi T --istantanea F [--audio-wav W]`.
  - **Che cosa cambia rispetto alla proposta:**
    - due partite vere della stessa scena, una per punto di vista, in un thread solo, invece delle scene A e B: il 688 è il giocatore
      in una, l'Alfa nell'altra. Il battello «del giocatore» riceve dalla regia gli ordini veri, l'altro segue le regole a tempo;
    - l'Alfa ha il polare OKEAN con lo SPEKTR, non una cascata: la doppia del culmine mette la cascata BQQ-5 accanto all'OKEAN;
    - il tubo che si allaga è sulla stazione Armi dal vivo (`SchermataSiluri` è il carico prima della partenza, ferma);
    - il titolo è scritto col font `fosforo688_7x9`, senza `TITLE.img`, che è dell'originale;
    - 31 battute invece di 30 (103 s): il titolo ne ha due.
  - **Opzione:** A nel menu, «VIDEO D'APERTURA: SEMPRE, LA PRIMA VOLTA, MAI», in `preferenze.toml` (`[avvio] video`, `visto`). `--senza-apertura`
    salta il video per quell'avvio. Si salta con Esc, Invio, Tab, P, un clic, le lettere, le cifre e i tasti di stazione; i tasti che non
    hanno un comando, come Spazio se non è mappato, non saltano.
  - **Misure** (`SDL_VIDEODRIVER=dummy`, carico 2-3): 103,3 s di film in 104,4 s, 21 fotogrammi al secondo, partite al massimo 1,6 s dietro al
    film. Il film segue il tempo vero; se il calcolo non basta, le partite restano indietro e il culmine si riempie meno. In una finestra vera
    non è misurato. Due missioni in più: circa 340 MB finché il film gira.
  - **Da riguardare dopo la chiusura della portata:** la riga che nasce e le quote del suono dipendono dal modello acustico. Gli istanti
    sono fissi: la nave della riga a 108 s, quelle dei periscopi a 162 s, lanci a 243,6 e 247 s, in `apertura.toml` e in `scene_interazioni.cpp` (una prova controlla i lanci).
- 06/11/2026: **giro di rifinitura** di trailer-1, sulla griglia a 300 km e senza le civili abbassate. Non ancora visto né sentito in finestra.
  - **Niente parole:** ogni strumento è ritagliato e ingrandito nel nero (`ritaglio`, `scala`), senza le scritte d'aiuto delle stazioni. Nel silenzio e nella traccia c'è solo la
    cascata o il polare: il LOFAR, con le sue righe, non si vede più. Il 10a mostra la bussola, il 10b il profilo, l'11 e il 12 il solo pannello dei tubi. Restano i nomi sulle
    consolle (SHIP CONTROL, WEAPON CONTROL PANEL, SARGAN, OKEAN, LUCH, ENISEJ) e la scritta QUOTA PERISCOPICA sul profilo.
  - **La riga e la nave sono due cose:** la riga è una Perry a 108 s, a circa 10,8 km da tutti e due i battelli, che la sentono allo stesso tempo; le navi dei periscopi entrano a 162 s,
    a 2 km. Sotto i 9 km la riga è già forte, oltre gli 11 sparisce: il margine è stretto, e la portata può spostarlo.
  - **Il tramonto:** ora 21:33 UTC, sole a circa un grado alla salita dei periscopi. Il colore è solo nella palette del film (`tramonto`), che tinge le rampe del cielo e del mare. Nessun
    disco del sole, perché il periscopio non lo disegna. Le nubi alte restano, accese dallo stesso colore: le colonne verticali che facevano nel campo stretto le ha corrette il kit in `cielo.cpp`.
  - **Il suono:** il silenzio e il nero del culmine tacciono (nel nero la musica è a zero: livello nullo misurato), il ping è sceso a -18 dB di picco, nessun campione al limite. Sono misure su un film senza finestra: nessuno l'ha ancora ascoltato. La
    crescita si misura per tempi: -29,3 dB nel silenzio, -25,5 nella traccia, -23,8 e -20,5 nelle due metà della caccia, -14,7 nel culmine (RMS); picco massimo -1,8 dB. `cuffia` regola il mare in ogni inquadratura, `bordo` abbassa il lancio vero della partita, che arriva 1,2 s dopo l'effetto sul battere e faceva da solo il picco del film.
  - **I tempi** (`SDL_VIDEODRIVER=dummy`, carico 14): 103,3 s di film in 105,5 s, 18 fotogrammi al secondo, partite al massimo 1,8 s dietro al film. In una finestra vera non è misurato.
  - **Un limite nelle Armi:** il tubo comincia ad allagarsi 7 s prima che l'inquadratura cominci (l'armamento dura 20 s e il lancio è fisso), quindi si sente l'allagamento ma la spia è già
    accesa.
- 06/11/2026: **gli scatti**, di trailer-1. In finestra vera (x11, opengl, 75 Hz, pulseaudio) il film scattava e perdeva il 28,9% dell'audio. Il costo di presentare era 1,5 ms: la causa non era la finestra.
  - **Causa:** la riga del sonar, ogni 2 s di missione, 165-200 ms sul thread dei fotogrammi, a qualunque stazione, con una coda audio di 0,1 s. Il 99% erano i 5 spettri del LOFAR. La stessa causa toglieva il 5-9% dell'audio nelle partite normali.
  - **La musica restava indietro:** avanza solo quando viene accodata. All'inizio del nero erano passati 68,2 s di musica su 96,3: il culmine della musica non arrivava al suo posto.
  - **Correzioni:** gli spettri su più fili (`app/spettri_fasci.hpp`, al massimo 5, mai oltre i core); l'audio su un filo proprio, che chiede i campioni da solo (`UscitaAudio::tira`, `RegiaAudio::produci_dal_filo`, un lucchetto in `Mixer`); cuffia e bordo a livello chiuso, 0,1 s di anticipo in partita e 0,2 s nel film, invece di accumulare fino a oltre 10 s. Musica ed effetti non dipendono più dai fotogrammi. Le partite non cambiano: rilievo 0/110, schermate al byte, suono senza finestra al byte.
  - **Musica e film** partono insieme e il film segue l'orologio, anche dopo un fotogramma lungo.
  - **Registro dei tempi:** `MELCHIOR_REGISTRO_TEMPI=file`, spento di norma (`app/registro_tempi.hpp`). Una riga per fotogramma, nel film e in partita.
  - **Limiti:** un fotogramma più lungo dell'anticipo lascia un buco in cuffia e bordo, non in musica ed effetti: se si sente lo dice l'ascolto. Gli effetti hanno qualche zero isolato ai confini dei blocchi del ricampionatore, 2-4 a effetto, come prima.
  - **La doppia carta:** il fondale si ricomponeva (55-60 ms) a ogni disegno mentre il battello cambiava quota, perché il contorno di sicurezza è la quota più 30 m; e la cache era una sola per due carte diverse. Ora una cache per parte; mentre la quota cambia il contorno resta indietro di 10 m al massimo, e un secondo dopo che si è fermata è esatto. Vale anche in partita: alla Carta mentre si cambia quota, da 9,8 a 22,4 fotogrammi al secondo.
- 06/11/2026: **riga e quote sui dati finali della portata**, di trailer-1.
  - **La banda a 165°** non era un difetto: un peschereccio a strascico del paesaggio sonoro, a 6,9 km per 160°, 2,06 dB sopra il rumore, sentito fin dal silenzio. Le partite del film hanno ora un seme proprio (`seme = 3` in `apertura.toml`): nessuna sorgente ambientale sopra il rumore a 40 s di missione, misurato sui primi 12 semi (il 2 e il 7 hanno una risacca a 8 km e un peschereccio a 100 m).
  - **La riga:** la nave lontana da 10 km per 150° dal centro a 9 km per 140°, più vicina all'Alfa, che ha un sonar più sordo: 3,85 dB sul 688 a 10,2 km e 3,60 dB sull'Alfa a 9,1 km, a 130 s di missione; prima 4,1 e 2,8. Il margine resta stretto: a 11,7 km il 688 non la sente più.
  - **Le quote non cambiano:** -29,4 dB nel silenzio, -25,5 nella traccia, -23,8 e -20,6 nella caccia, -14,8 nel culmine; picco -1,7 dB, nessun campione al limite.
  - **Limite:** sul polare dell'Alfa la riga resta tenue, si vede appena. Non visto né sentito in finestra.
- 06/11/2026: **la fluidità**, di trailer-1. L'utente: audio a posto, grafica scattosa.
  - **Il film si prepara in anticipo** (`gioca_apertura`, `CodaFilm`): un filo lo calcola a passi fissi di 1/30 s, immagine e suono insieme, come il ramo senza finestra, fino a 20 s avanti (circa 190 MB); la finestra mostra il fotogramma che l'audio ha raggiunto, con un orologio regolare agganciato all'audio. Partite a passo pieno, nessun ritardo: si vede esattamente ciò che danno i provini.
  - **Sotto le stazioni che lo coprono non si disegna il periscopio**; mare, nubi e maschera dell'oculare vanno su più fili (`grafica/src/per_righe.hpp`, stessi byte); la scansione della costa è spenta nel film.
  - **Misure** (video e audio finti, carico 19-24 per altre sessioni): 29,9 fotogrammi al secondo fino al titolo, 2608 intervalli su 2791 esattamente di 33,3 ms, massimo 70,5 ms; coda mai sotto 116 fotogrammi; nessun silenzio; 6,5 s di attesa nera prima di partire. Preparazione: 83,7 s di calcolo per 103 s di film. In finestra vera (x11, opengl, 75 Hz, pulseaudio, carico 30, registro dell'utente delle 18:21): 29,8 fotogrammi al secondo, 8 intervalli oltre 66 ms su 2784, massimo 155 ms, nessun silenzio, coda mai sotto 108 fotogrammi. Gli intervalli sono 26,7 e 40 ms alternati: è il vsync.
  - **Il registro non scrive più a ogni riga:** il suo `fflush` col disco occupato fermava tutto fino a 1,2 s. Gli «stalli della macchina» misurati prima erano quasi certamente questi. Si apre in aggiunta, con una riga per avvio.
  - **Anche in partita niente periscopio sotto le stazioni che lo coprono** (`InterfacciaMissione::copre_la_vista`): radio, carta, danni, Sonar, centrale, Manovra e Armi col periscopio giù. Solo disegno: la visione delle navi e i contatti stanno in `Missione`. Disegno da 31 a 14 ms alla Manovra, da 21 a 1,3 ms al Sonar; da 20 a 33-47 fotogrammi al secondo in gara a carico 30. Il periscopio resta a 24 fotogrammi al secondo. 70 schermate al byte sulle 8 stazioni, classico e Alfa compresi.
  - **La prova dell'utente sul binario unico:** «Sembra vada tutto bene». Il film resta così, sempre all'avvio e saltabile; l'anticipo di cuffia e bordo resta 0,1 s in partita e 0,2 s nel film.
