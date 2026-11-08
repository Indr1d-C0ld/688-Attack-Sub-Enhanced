# Testi di tacche, minicartoline e carta del debriefing nel manuale (kit, 07/11/2026). Ogni sostituzione deve trovare il testo una volta sola.
import pathlib, sys
M = pathlib.Path("/home/randolph/Claude/melchior-688/docs/manuale_enhanced")
TACCHE = ("ogni evento è una tacca. Il colore è quello della sua riga nel registro: rosso lanci, colpi,\n"
"  affondamenti e urti fra unità dell'IA; ciano scoperte, classificazioni tue e ingaggi; giallo avvistamenti, fix ed\n"
"  evasioni; bianco inizio e fine; grigio il resto, anche una collisione del battello. L'altezza dice l'interesse\n"
"  dell'evento (il dato del registro, 0-100): da 70 a tutta altezza (un affondamento, un colpo subito), da 50 più corta\n"
"  (un tuo lancio), da 30 bassa (la scoperta di una nave), sotto un punto sulla linea. Le più interessanti stanno sopra\n"
"  le altre, il cursore dell'ora sopra tutte. Nella lettura QUADRO gli eventi che non sapevi non compaiono. La ripresa è\n"
"  una riga verde.")
S = [
("1_uso/10_carriera.md",
"""gli eventi già
  visti sono segnaposto numerati, quello corrente è giallo;
""",
"""gli eventi già
  visti sono segnaposto numerati, quello corrente è giallo e resta sempre sopra. Le etichette che si toccherebbero
  diventano una sola: «12-18» se i numeri sono di seguito, «12+6» se no, cioè il 12 e altri 6. Avvicinandoti tornano
  separate;
"""),
("1_uso/10_carriera.md",
"""la **minicartolina** incollata (la scena
  com'era in quell'istante, come una nave colpita, un'emersione o un missile che parte, oppure un'icona d'epoca per
  quello che non si vede, come un contatto sonar o un ping), il testo, la distanza a puntini e l'interesse a tacche;
""",
"""la **minicartolina** incollata, il testo,
  la distanza a puntini e l'interesse a tacche;
"""),
("1_uso/10_carriera.md",
"""**Comandi:** frecce o Invio""",
"""**La minicartolina.** Ogni evento ha la sua figurina. La scena viene dalla fotografia di quell'istante, se c'è, come
una nave colpita, un'emersione o un missile che parte. Se no si ricostruisce da dove stavano le unità in quell'istante,
con il soggetto a galla e vicino: è una figurina, non la vista vera, e senza la fotografia non ha fumo né colonne
d'acqua. Ciò che non si vede, come un contatto sonar, un ping, la radio, i danni o una zona, ha la sua icona d'epoca,
e così il periscopio esposto troppo a lungo.

**Comandi:** frecce o Invio"""),
("1_uso/10_carriera.md",
"""**S** le
statistiche della missione; I e O lo zoom. La riga d'aiuto sopra il piede li ricorda.""",
"""**S** le
statistiche della missione. La riga d'aiuto sopra il piede li ricorda. **Sulla carta:** trascina per spostarla, rotella
per avvicinarti attorno al punto sotto il puntatore, oppure I e O. La carta torna da sola sull'evento solo quando ne
scegli uno che è fuori vista."""),
("1_uso/10_carriera.md",
"""- **la linea del tempo**, una fascia sottile sotto la carta, con le tacche degli eventi (i lanci nemici in rosso, la
  ripresa in verde): P gioca""",
"""- **la linea del tempo**, una fascia sottile sotto la carta: """ + TACCHE + """ P gioca"""),
("1_uso/10_carriera.md",
"""- **la carta** segue il battello; dopo averla spostata, un clic""",
"""- **la carta** segue il battello. Trascinandola la sposti, e la rotella la avvicina attorno al punto sotto il puntatore,
  come I e O; dopo averla spostata, un clic"""),
("1_uso/12_sandbox.md",
"""la barra con le tacche (in rosso lanci e urti) e
  i triangoli""",
"""la barra con le tacche (colori e altezze come nel replay, §10.4,
  qui con tutti gli eventi, anche nei quadri) e i triangoli"""),
("1_uso/12_sandbox.md",
"""apre i replay affiancati di una
coppia. Da riga di comando""",
"""apre i replay affiancati di una
coppia: lì le tacche sulla linea del tempo sono gli interventi applicati di B, col colore della famiglia, e la linea
gialla è il punto dove le due partite divergono; rotella, I e O e trascinamento muovono le due carte insieme. Da riga di comando"""),
("1_uso/appendici.md",
"""| `I`, `O` | ingrandisce o riduce la carta |
| `G` | il pannello degli interventi""",
"""| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore |
| `G` | il pannello degli interventi"""),
("1_uso/appendici.md",
"""| `I`, `O` | ingrandisce o riduce la carta |
| `Invio`, `Tastierino Invio` | riprende la missione da quell'istante |""",
"""| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore |
| `Invio`, `Tastierino Invio` | riprende la missione da quell'istante |"""),
("1_uso/01_introduzione.md",
"""chiusi gli
   ordini, le frecce spostano la carta e I e O fanno lo zoom.""",
"""chiusi gli
   ordini, le frecce o il trascinamento spostano la carta, I e O o la rotella fanno lo zoom."""),
("2_riferimento/F1_registro_e_debriefing.md",
"""corrente in giallo. La scala iniziale è la più grande che contiene la traccia e gli eventi; la carta segue l'evento
  quando esce dal riquadro;
""",
"""corrente in giallo, con la cornice, fuori dai gruppi e sopra tutti. Le etichette che si toccherebbero alla scala di
  adesso diventano una sola (`app/etichette_carta.hpp`): «12-18» se i numeri sono di seguito, «12+6» se no, cioè il 12 e
  altri 6, con la cornice della parte dell'evento più interessante del gruppo; avvicinandosi tornano separate. Quelle
  fuori dal riquadro non si raccolgono. La scala iniziale è la più grande che contiene la traccia e gli eventi. La carta
  va sull'evento solo quando se ne sceglie uno fuori dal riquadro, con un margine di 24 pixel; sullo stesso evento resta
  dove l'ha messa chi guarda. In basso a sinistra la nota «I O O LA ROTELLA ZOOM, TRASCINA SPOSTA»;
"""),
("2_riferimento/F1_registro_e_debriefing.md",
"""alta 80 pixel (senza fotografia, la classe
  su fondo blu); il testo""",
"""alta 80 pixel (sotto); il testo"""),
("2_riferimento/F1_registro_e_debriefing.md",
"""| (riga d'aiuto) | I, O | lo zoom della carta |
""",
"""| (riga d'aiuto) | I, O | lo zoom della carta |
| (sulla carta) | rotella | lo zoom attorno al punto sotto il puntatore (`app/trascina_carta.hpp`); fuori dalla carta non fa nulla |
| (sulla carta) | trascinare | sposta la carta |
"""),
("2_riferimento/F1_registro_e_debriefing.md",
"""**Le minicartoline** (`app/partita.cpp`, `Partita::avanza`; `grafica/cartolina`):
- durante la missione la partita fotografa il mondo e gli effetti all'istante di ogni evento che si vede; per colpi,
  affondamenti e lanci 4 s dopo, quando la colonna d'acqua o il missile sono su;
- al più 120 fotografie; oltre, solo gli eventi con interesse ≥ 60; oltre 300, solo la fine e gli affondamenti. I colpi
  d'artiglieria di un tiratore dividono la sua prima fotografia: la cartolina di un colpo successivo mostra quella
  (`app/scelta_fotografie.hpp`);
- la scena si disegna dal punto di vista suggerito dal registro (soggetto, secondo, distanza, elevazione, azimut);
- la foto al periscopio si rifà dall'occhio del battello, col rilevamento e l'ingrandimento dello scatto;
- quello che non si vede ha un'icona animata d'epoca: telescrivente, ESM, radiogoniometro, danni, zona, cascata,
  mina, aereo, boa, elicottero, marcatore, sonar;
""",
"""**Le minicartoline** (`app/partita.cpp`, `Partita::avanza`; `app/figurina_evento.hpp`; `grafica/cartolina`). Ogni evento
ha la sua figurina, mai un riquadro vuoto (scelta dell'utente del 07/11/2026). La sceglie `figurina_evento`:
l'icona per ciò che non si vede; la scena se c'è un soggetto da inquadrare; se no l'icona di riserva, scelta dal tipo.
- durante la missione la partita fotografa il mondo e gli effetti all'istante di ogni evento che si vede; per colpi,
  affondamenti e lanci 4 s dopo, quando la colonna d'acqua o il missile sono su;
- gli eventi di routine dell'IA non hanno fotografia (`classificazione_ia`, `comandante_ia`, `reazione_ia`, `tma_ia`,
  `scoperta_ia`, `ingaggio_ia`, `ritirata_ia`, `evasione_ia`): nella S5 prendevano 68 fotografie su 127, a danno di quelli
  veri. La loro cartolina si fa dalle pose;
- al più 120 fotografie; oltre, solo gli eventi con interesse ≥ 60; oltre 300, solo la fine e gli affondamenti. I colpi
  d'artiglieria di un tiratore dividono la sua prima fotografia: la cartolina di un colpo successivo mostra quella
  (`app/scelta_fotografie.hpp`);
- la scena viene dalla fotografia, se c'è, con il soggetto e il secondo rimessi dalle pose (`EventoMissione::pose`) se nel
  frattempo sono usciti dal mondo. Senza fotografia viene dalle sole pose: è una figurina, non la vista vera, e non ha
  fumo né colonne d'acqua;
- il punto di vista è quello suggerito dal registro (soggetto, secondo, distanza, elevazione, azimut), con queste
  correzioni, solo nella scena (**STIMA**, scelte di aspetto): un siluro in acqua non ha sagoma, e si inquadra chi lo
  riguarda, da 800 m a pelo d'acqua; la camera sta al più a 1500 m, e oltre 1000 m senza alzo; il secondo resta in campo
  solo entro 500 m dal soggetto; un battello immerso sta a galla, un aereo a 20 m di quota;
- la foto al periscopio si rifà dall'occhio del battello, col rilevamento e l'ingrandimento dello scatto;
- quello che non si vede ha un'icona animata d'epoca: telescrivente, ESM, radiogoniometro, danni, zona, cascata,
  mina, aereo, boa, elicottero, marcatore, sonar e periscopio, per il periscopio esposto. Il SUS dell'attacco simulato
  usa il marcatore;
- l'icona di riserva (`icona_di_riserva`; **STIMA**, una scelta di aspetto): danni per colpi, affondamenti, incagli,
  collisioni ed emersioni d'emergenza; cascata per scoperte, classificazioni dell'IA e TMA dell'IA; telescrivente per
  comandante, reazioni e ingaggi dell'IA; periscopio per la foto al periscopio; sonar per il resto;
- misura del 07/11/2026, ghiaccio-d4, partite senza giocatore, seme 1: nessun riquadro blu e nessuna riserva in S5, S5
  con la regola di simulazione, M3, S07 e SCENE1A. Nella S5, 336 eventi: 39 con fotografia, 233 dalle sole pose, 64 con
  icona. Nella S07, ferma a 3764 s, 334 eventi: 77, 134 e 123;
"""),
("2_riferimento/F5_replay_e_ripresa.md",
"""- I e O ingrandiscono; trascinare sposta la carta, che smette di seguire il battello finché non si clicca sulla
  destra della testata («SEGUI IL BATTELLO»); in basso a destra la scala delle 10 miglia;
""",
"""- I e O ingrandiscono; la rotella fa lo stesso, a passi di 1,5×, attorno al punto sotto il puntatore
  (`app/trascina_carta.hpp`), ma non toglie il seguire: mentre la carta segue il battello, al passo dopo torna su di lui.
  Trascinare sposta la carta, che smette di seguire il battello finché non si clicca sulla destra della testata («SEGUI
  IL BATTELLO»); in basso a destra la scala delle 10 miglia;
"""),
("2_riferimento/F5_replay_e_ripresa.md",
"""- sotto la carta la **linea del tempo** (`quadro::fascia_tempo`) coi segni degli eventi, i lanci nemici in rosso e la
  ripresa in verde: P gioca""",
"""- sotto la carta la **linea del tempo** (`quadro::fascia_tempo`): ogni evento è una tacca, la ripresa una riga verde.
  Il colore è quello della riga del registro (`colore_evento`): rosso `lancio_giocatore`, `lancio_nemico`, `lancio_ia`,
  `colpo`, `affondamento`, `artiglieria_ia`, `collisione_ia`; ciano `scoperta`, `scoperta_ia`, `classificazione`,
  `ingaggio_ia`; giallo `avvistato`, `fix_radiogoniometria`, `evasione_ia`; bianco `inizio` e `fine`; grigio il resto.
  L'altezza dice l'interesse (`EventoMissione::interesse`, 0-100): da 70 tutta la fascia, da 50 due terzi, da 30 un terzo,
  sotto un punto sulla linea. Le più interessanti si disegnano sopra le altre, il cursore dell'ora sopra tutte. Nella
  lettura quadro gli eventi non noti non compaiono; nella vista dal vivo della sandbox ci sono tutti. P gioca"""),
("2_riferimento/F8_sandbox.md",
"""La schermata del confronto (`app/schermata_confronto`) apre i replay affiancati.
""",
"""La schermata del confronto (`app/schermata_confronto`) apre i replay affiancati (`app/replay_affiancati`). Le
due carte si muovono insieme, con I e O, la rotella e il trascinamento. Sulla linea del tempo le tacche sono gli interventi
applicati di B, a tutta altezza, col colore della famiglia, e una riga gialla segna il punto di divergenza.
"""),
("2_riferimento/E9_grafica.md",
"""- **Minicartoline** del debriefing: la fotografia del mondo all'evento, ridisegnata in un riquadro con i suoi colori
  negli indici 160-191 (16-47 sono della lamiera della consolle).
""",
"""- **Minicartoline** del debriefing: la fotografia del mondo all'evento, o le sole pose se non c'è, ridisegnata in un
  riquadro con i suoi colori negli indici 160-191 (16-47 sono della lamiera della consolle); per ciò che non si vede,
  un'icona (capitolo F1 §2.5).
"""),
("2_riferimento/E9_grafica.md",
"""- il riquadro sopra una carta (la legenda e la lettura del replay) e la linea del tempo come fascia sottile
  (`fascia_tempo`, scelta dell'utente del 28/10/2026 per il replay a carta grande).
""",
"""- il riquadro sopra una carta (la legenda e la lettura del replay) e la linea del tempo come fascia sottile
  (`fascia_tempo`, scelta dell'utente del 28/10/2026 per il replay a carta grande), con le tacche a colori e ad altezze
  (`TaccaTempo`, capitolo F5 §2.1).
"""),
]
for f, a, b in S:
    p = M / f
    t = p.read_text(encoding="utf-8")
    n = t.count(a)
    if n != 1:
        sys.exit(f"{f}: {n} volte: {a[:60]!r}")
for f, a, b in S:
    p = M / f
    p.write_text(p.read_text(encoding="utf-8").replace(a, b), encoding="utf-8")
print("fatte", len(S))
