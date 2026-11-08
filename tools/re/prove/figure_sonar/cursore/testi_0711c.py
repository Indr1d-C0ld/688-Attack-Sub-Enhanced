#!/usr/bin/env python3
# Secondo via della principale (kit, 07/11/2026): analizzatore (testo di portata), colori delle tacche (tabella di ghiaccio-d4), T12 §12.
import sys
M = "/home/randolph/Claude/melchior-688/docs/manuale_enhanced/"
S = [
("1_uso/03_sonar.md", """**ANALIZZA** (4) confronta le righe del fascio con la banca dati delle classi e dà le ipotesi con la confidenza.
Con poche righe le ipotesi restano vicine; con molte righe, e le pale lette dal DEMON, può prevalerne una. Il confronto è
per rapporti fra le frequenze, quindi non lo inganna il Doppler. **L'ipotesi può essere sbagliata, anche su un contatto forte
e vicino: non prenderla per buona** e confrontala con quello che sai del contatto (volume 2, C5 §7). Sul fascio di un tracker
l'analisi dà il passo di classificazione:

- **NONSUB**: non è un sottomarino;
- **POSSUB**: possibile sottomarino;
- **PROBSUB**: probabile sottomarino;
- **CERTSUB**: sottomarino certo.

Quando un'analisi arriva a PROBSUB o CERTSUB, o una nave supera metà confidenza, il contatto più vicino a quel
rilevamento è classificato. La classificazione resta agli atti: nel debriefing conta se era giusta.
""", """**ANALIZZA** (4) confronta le righe del fascio con la banca dati delle classi e scrive un nome solo se le righe lo
reggono: almeno 3 righe spiegate, almeno 2 righe diverse della voce, almeno il 60% delle righe lette. La scritta è
«PESCHERECCIO 5/5 RIGHE O ROMPIGHIACCI 4/5»: le prime due voci, con quante righe ciascuna spiega su quante lette.
Altrimenti è «NESSUN NOME: 2 RIGHE NON BASTANO». Non ci sono percentuale né fattore K. Il confronto è per rapporti fra le
frequenze, quindi non lo inganna il Doppler.

**Un nome non è una certezza:** il fascio è largo e le righe possono essere di un'altra sorgente vicina. Nelle prove il
nome era vero il 93% delle volte nelle scene, il 74% con la sfera in moto, l'80% con la TB-16 filata; il bersaglio del
fascio era fra i nomi molto più di rado. Confrontalo con quello che sai del contatto (volume 2, C5 §7).

Sul fascio di un tracker l'analisi dà il passo di classificazione, dalle stesse due voci:

- **PROBSUB**, probabile sottomarino: lo sono tutte e due, o ce n'è una sola ed è un sottomarino;
- **POSSUB**, possibile sottomarino: lo è una delle due;
- **nessun passo e nessun rapporto:** non lo è nessuna delle due. Senza un nome il tracker resta com'era.

La stazione non arriva a CERTSUB e non dice mai NONSUB: le righe di una nave nel fascio non escludono un battello. Il
sonarista ha la sua scala, che resta a parte e può ancora dire NONSUB.

Quando un'analisi arriva a PROBSUB, il contatto più vicino a quel rilevamento, entro 5°, è classificato. La
classificazione resta agli atti: nel debriefing conta se era giusta.

**La stazione tace spesso, anche quando un sottomarino c'è.** Con la sfera in moto, su 119 letture di sottomarini,
nessun allarme, e sui civili l'1,4%. Con la TB-16 filata 7,5% sui sottomarini, tutti Alfa, e 0,8% sui civili. Nelle scene
8,3% e 5,1%, e lì sui civili sono quasi sempre righe di un sottomarino vero, l'Alfa vicinissimo della SCENE5A. Il silenzio
dell'analizzatore non vuol dire che il mare sia vuoto.
"""),
("1_uso/03_sonar.md", """*La pagina dell'analizzatore: il LOFAR del fascio a tutta larghezza, sotto a sinistra il DEMON, a destra l'ipotesi
migliore e gli otto slot dello spettro. Il fascio è sulla Perry della figura del DEMON: l'ipotesi, un Charlie II al 4%,
è sbagliata. È un indizio, non una risposta.*
""", """*La pagina dell'analizzatore: il LOFAR del fascio a tutta larghezza, sotto a sinistra il DEMON, a destra la scritta e gli
otto slot dello spettro. Il fascio è sulla Perry della figura del DEMON, a 15 nodi: si legge una riga sola, oltre a quelle del
tuo battello, e l'analizzatore non dà un nome.*

![L'analizzatore con un nome](../figure/sonar_analizzatore_nome.png)

*La stessa pagina nella SCENE2A, col fascio verso tre mercantili e un Victor: «PESCHERECCIO 5/5 RIGHE O ROMPIGHIACCI 4/5».
Le righe sono di un peschereccio che sta nello stesso fascio: i mercantili, a 18-22 km, non si sentono. Il nome è vero, ma
non è quello del bersaglio.*
"""),
("1_uso/03_sonar.md", """- a destra l'esito: il confronto con la voce sovrapposta (nome, confidenza, fattore Doppler e velocità radiale) o
  l'ipotesi migliore, e la famiglia di righe più chiara (indizio, frequenza, pale);
""", """- a destra l'esito: la scritta con le voci che le righe reggono e la famiglia di righe più chiara (indizio, frequenza,
  pale);
"""),
("1_uso/03_sonar.md", """Con la TB-16 l'analizzatore riconosce mercantile, cargo e petroliera fino a 20 km se vanno a 4-8 nodi. A 12 nodi il
mercantile si riconosce fino a 10 km, cargo e petroliera fino a 20 km; a 16 nodi non si riconoscono. Il peschereccio
resta incerto: l'ipotesi giusta arriva, con una confidenza di 0,36. Sul traffico civile il riconoscimento è raro.
""", """Anche con la TB-16 filata un nome è raro: nelle prove 75 letture su 1.149 con righe, e il bersaglio era fra i nomi in 17.
"""),
("2_riferimento/C5_elaborazione_e_presentazione.md", """| NONSUB | la migliore ipotesi non è un sottomarino |

**Le famiglie armoniche**""", """| NONSUB | la migliore ipotesi non è un sottomarino |

È la scala del sonarista (§2.7) e di ACINT. La stazione ha regole sue (§2.6).

**Le famiglie armoniche**"""),
("2_riferimento/C5_elaborazione_e_presentazione.md", """ai contatti forti. Senza segnale per 30 s si perdono. ANALIZZA sul fascio di un tracker dà il passo della scala NATO e
il rapporto al comando.
""", """ai contatti forti. Senza segnale per 30 s si perdono. ANALIZZA sul fascio di un tracker dà il passo e il rapporto al
comando. Legge il confronto per rapporti (§2.5), con regole sue:
- **il nome** (`voci_rette`): una voce è retta con almeno 3 righe spiegate, almeno 2 righe diverse della voce e almeno il 60%
  delle righe lette. La scritta (`lettura_analisi`) è «PESCHERECCIO 5/5 RIGHE O ROMPIGHIACCI 4/5»: le prime due voci, con
  quante righe ciascuna spiega su quante lette. Altrimenti è «NESSUN NOME: 2 RIGHE NON BASTANO». Niente percentuale,
  niente fattore K;
- **il passo** (`passo_da_voci`): PROBSUB se le due voci mostrate sono sottomarini (ssn, ssgn, ssbn, ssk), o se ce n'è una
  sola ed è un sottomarino; POSSUB se lo è una delle due; altrimenti nessun passo e nessun rapporto. Sotto le soglie il
  tracker resta com'era. La stazione non arriva a CERTSUB e non dice mai NONSUB: le righe di una nave nel fascio non
  escludono un battello. Il sonarista ha la sua scala, che resta a parte e può ancora dire NONSUB (§2.7);
- **la classe** (`classifica_dai_tracker`): a PROBSUB il contatto più vicino a quel rilevamento, entro 5°, è classificato
  con la prima voce.
"""),
("2_riferimento/C5_elaborazione_e_presentazione.md", """| Scala NATO NONSUB-CERTSUB | dossier 09 §2.9 | alta (i nomi); **STIMA** le soglie (0,8 e 5 righe, 0,55 e 3) |
""", """| Scala NATO NONSUB-CERTSUB | dossier 09 §2.9 | alta (i nomi); **STIMA** le soglie (0,8 e 5 righe, 0,55 e 3) |
| Soglie del nome di ANALIZZA (3 righe, 2 della voce, 60%) e passo dalle due voci | misure del 07/11/2026 (§7) | **STIMA DI GIOCO** |
"""),
("2_riferimento/C5_elaborazione_e_presentazione.md", """- **ANALIZZA sbaglia classe sui contatti veri.** È una misura provvisoria del 07/11/2026, con la diagnosi in corso: cinque
  letture su tre scene, modo moderno, seme 1, e nessuna giusta.
  - Le Spruance a 285-295° (SCENE1A, S2): rompighiaccio 64% a 200 s, 44% a 400 s.
  - Tre mercantili a 150°, con un Victor a 160° (SCENE2A, 250 s, S1): peschereccio 67%.
  - Un Alfa a 2 km (SCENE2A, 250 s, S2): mercantile 97%, «non è un sottomarino».
  - Una Perry a 25 nodi, +47 dB, con 5 pale lette dal DEMON (scena di prova, 250 s): Charlie II 4%, «possibile sottomarino».

  Sono poche letture: non dicono la causa, né da quando è così, né se tocca le partite.
""", """- **ANALIZZA tace spesso, e un nome non è una certezza.** Misura del 07/11/2026 col binario: 145 corse, 2.731 letture con
  righe. Allarme vuol dire POSSUB o PROBSUB.

  | Colonna | Allarme: sottomarini | Allarme: guerra | Allarme: civili | Nomi, veri | Col bersaglio fra i nomi |
  |---|---:|---:|---:|---:|---:|
  | Scene | 6 su 72, 8,3% | 0 su 35 | 21 su 413, 5,1% | 69, 93% | 4 |
  | Sfera in moto | 0 su 119 | 0 su 108 | 12 su 835, 1,4% | 62, 74% | 1 |
  | TB-16 filata | 11 su 146, 7,5% | 0 su 116 | 7 su 887, 0,8% | 75, 80% | 17 |

  - **Il nome.** Il fascio è largo e le righe possono essere di un'altra sorgente vicina: il nome è spesso vero, ma il
    bersaglio del fascio è fra i nomi molto più di rado.
  - **Il silenzio.** La stazione tace spesso, anche quando un sottomarino c'è. Con la sfera in moto nessun allarme sui
    sottomarini (prima di guardare le sole due voci mostrate 3 su 119, meno del 6% dei civili).
  - **Chi dà l'allarme.** Sui sottomarini Alfa, Victor III e Akula nelle scene, il solo Alfa con la TB-16. Sulle navi, gli
    allarmi con righe di un sottomarino vero fra le sorgenti sono 20 su 21 nelle scene (19 nella SCENE5A, l'Alfa vicinissimo),
    0 su 12 con la sfera in moto, 3 su 7 con la TB-16.
  - **I limiti della misura.** Letture a missione ferma, col fascio sul rilevamento vero (il caso migliore), scene scelte
    a mano, senza un giocatore che manovra.
"""),
("1_uso/10_carriera.md", """- **la lettura**, in alto a destra: gli ultimi quattro eventi e la quota degli ultimi dieci minuti, del battello e del
  bersaglio, sul fondale e coi termoclini; in basso a destra la scala delle 10 miglia;
""", """- **la lettura**, in alto a destra: gli ultimi quattro eventi, in rosso chiaro quelli che sulla linea del tempo sono rossi, e
  la quota degli ultimi dieci minuti, del battello e del bersaglio, sul fondale e coi termoclini; in basso a destra la scala
  delle 10 miglia;
"""),
("1_uso/10_carriera.md", """ogni evento è una tacca. Il colore è quello della sua riga nel registro: rosso lanci, colpi,
  affondamenti e urti fra unità dell'IA; ciano scoperte, classificazioni tue e ingaggi; giallo avvistamenti, fix ed
  evasioni; bianco inizio e fine; grigio il resto, anche una collisione del battello. L'altezza dice l'interesse
""", """ogni evento è una tacca. Il colore dice il tipo. Rosso ciò che colpisce, fa danno o è un'arma che parte:
  lanci, colpi, affondamenti, urti, incagli, artiglieria, danni, mine. Giallo il pericolo, scampato o in corso: siluri in
  acqua ed evitati, esche, passaggi ravvicinati, colonne d'acqua, emersioni, ping che ti cercano, zone proibite. Ciano le
  scoperte, le classificazioni tue, gli ingaggi e le mine trovate. Bianco inizio e fine. Grigio il resto: radio, routine
  dell'IA, misure. L'altezza dice l'interesse
"""),
("1_uso/12_sandbox.md", """- **I colori:** rosso le armi e i colpi, ciano le scoperte, giallo le evasioni e i fatti dei siluri, bianco inizio e
  fine.
""", """- **I colori** sono quelli delle tacche del replay (§10.4): rosso, qui rosso chiaro, armi, colpi e danni; giallo il
  pericolo, scampato o in corso; ciano le scoperte; bianco inizio e fine; grigio il resto.
"""),
("2_riferimento/F5_replay_e_ripresa.md", """  Il colore è quello della riga del registro (`colore_evento`): rosso `lancio_giocatore`, `lancio_nemico`, `lancio_ia`,
  `colpo`, `affondamento`, `artiglieria_ia`, `collisione_ia`; ciano `scoperta`, `scoperta_ia`, `classificazione`,
  `ingaggio_ia`; giallo `avvistato`, `fix_radiogoniometria`, `evasione_ia`; bianco `inizio` e `fine`; grigio il resto.
""", """  Il colore (`colore_evento`) dipende dal solo tipo, ed è quello della riga del registro; nella lettura e nel registro i rossi
  si scrivono in rosso chiaro.
  - **Rosso** (24), ciò che colpisce, fa danno o è un'arma che parte: `lancio_giocatore`, `lancio_nemico`, `lancio_ia`, `colpo`,
    `colpo_esercitazione`, `affondamento`, `artiglieria_salva`, `artiglieria_colpo`, `artiglieria_ia`, `collisione`,
    `collisione_ia`, `incaglio`, `danno_subito`, `scram`, `camera_di_lancio_persa`, `cavo_impigliato`, `avaria_ia`,
    `fuoco_amico`, `fuoco_amico_ia`, `provocazione`, `infrazione_roe`, `mina_posata`, `mina_esplosa`, `esito_tlam`.
  - **Giallo** (27), il pericolo scampato o in corso: `avvistato`, `fix_radiogoniometria`, `evasione_ia`, `ritirata_ia`,
    `emersione_emergenza`, `emersione_forzata`, `siluro_evitato`, `allarme_siluro`, `esca_sedotta`, `esca_ia`, `danno_domato`,
    `posti_di_combattimento`, `artiglieria_vicino`, `quasi_collisione`, `incsea`, `cavo_toccato`, `attacco_simulato`,
    `marcatore_avvistato`, `esposizione_lunga`, `esm_rischio_alto`, `ping_subito`, `ping_sonar_immersione`, `aereo_in_sorvolo`,
    `boa_vicina`, `ingresso_zona`, `acque_territoriali`, `attraversamento_mine`.
  - **Ciano** (5), le scoperte: `scoperta`, `classificazione`, `scoperta_ia`, `ingaggio_ia`, `mina_scoperta`.
  - **Bianco** (2): `inizio` e `fine`.
  - **Grigio** (27), il resto: radio, routine, misure. `intervento` è l'unico a tutta altezza, perché ha già il triangolo col
    colore della famiglia sopra la barra. `classificazione_ia` lo è di proposito: 74 eventi in S5, in ciano coprirebbero le
    scoperte vere.

  Conta il tipo, non il caso: `posti_di_combattimento` è giallo anche per «Condizione normale», che con interesse 20 è un
  punto, `acque_territoriali` anche in uscita, `esito_tlam` è rosso anche se il missile è perso.
"""),
("4_tattiche/T12_casi_di_studio.md", """  - **Il traffico civile oltre 30 km** resta al livello A di propagazione, senza il campo.
""", """  - **Il traffico civile oltre 30 km** resta al livello A di propagazione, senza il campo. Il LOFAR, e quindi l'analizzatore, sta al livello A per tutte le sorgenti, non solo per quel traffico.
"""),
]
for f, a, b in S:
    p = M + f
    t = open(p, encoding="utf-8").read()
    if t.count(a) != 1:
        sys.exit(f"{f}: {t.count(a)} occorrenze di: {a[:70]!r}")
for f, a, b in S:
    p = M + f
    t = open(p, encoding="utf-8").read()
    open(p, "w", encoding="utf-8").write(t.replace(a, b, 1))
print(len(S), "sostituzioni")
