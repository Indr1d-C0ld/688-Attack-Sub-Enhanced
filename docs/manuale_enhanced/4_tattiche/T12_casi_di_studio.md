# T12. Casi di studio

*Autore: trailer-1 (la libreria). Bozza del 03/11/2026, rimisurata il 06-07/11/2026 (curatore: kit). I casi sono le
missioni storiche della libreria (LIB-S01 … LIB-S25, `data/missioni/libreria`). Le schede dei soggetti e le note di
costruzione sono in docs/17. I numeri vengono dalle prove di vittoria e sconfitta di ogni missione
(`test/prove_libreria_missioni.cpp`), come le schede li riportano. Quando una cifra viene da una partita sola, o da pochi
semi, lo si dice. Le soglie e gli esiti, controllati uno a uno il 03/11/2026 contro i Lua e le prove, sono del motore.
Le cifre di §9 e §10 e le righe «senza ordini» di S10, S14, S15 e S19 sono state giocate di nuovo su una build del 06/11/2026
(casi uguali al riferimento, rilievo classico 0 su 110): col sonar a 300 km, le navi civili corrette, il deposito dei
proietti, i rapporti e il quadro radio dei sottomarini e la data giusta delle scene. Le serie del 03-04/11 sono superate.
Le prove della libreria passano su questa build, 53 su 53, e le cifre che stampano (S23, S24, S25) sono quelle scritte qui. Il resto
delle cifre al secondo è osservato in una partita del 03/11: la prova non le garantisce.*

Gli altri capitoli del volume spiegano una regola alla volta. Le missioni storiche le mettono insieme, ciascuna su un
episodio vero della Guerra fredda, raccontato nel volume 3 al capitolo 10 (il codice E.. di ogni caso). Battelli e
persone sono di fantasia, gli episodi contesi restano aperti, e nessuna morte è un obiettivo: nei soccorsi si contano
i salvati.

Ogni caso dice:
- che cosa chiede la missione;
- la lezione, in regole numerate;
- come si vince e come si perde nel motore;
- i capitoli del volume dove la regola ha il suo perché.

## 1. Come si studia un caso

1. **Prima l'episodio.** Leggi la voce E.. nel volume 3: la missione ne è la trasposizione, e il briefing ha la sua
   nota storica.
2. **Giocala una volta senza piano.** Il primo errore è quello che il caso vuole mostrarti.
3. **Poi il replay, in due letture** (volume 1, §10.4). La VERITA' mostra dove erano tutti; il QUADRO mostra quello che sapevi.
   Lo scarto fra le due è la lezione: il Victor che credevi a nord, la Krivak che ti teneva da dieci minuti.
4. **Lascia giocare l'IA** (volume 1, capitolo 12): V sulla missione apre la sandbox, e il vetrino la gioca con le stesse regole. Il
   comandante IA del battello fa quello che farebbe un comandante prudente. Spesso si vede subito che cosa non basta.
5. **Confronta due modi di giocarla** (volume 1, §12.9). Un esperimento A/B del vetrino di Dio fa la stessa missione sugli
   stessi semi con un intervento e senza, e dice se la differenza è vera o è il caso.

## 2. Nascosti e zitti: vince chi non si fa sentire

### S02 «Nave mineraria» (E03, il progetto Azorian) — 688, pace, difficoltà 2

Il Los Angeles fa la guardia, senza armi, a una nave civile ferma sul bacino profondo delle Curili. Arrivano una nave
di misura sovietica e un rimorchiatore, che a un'ora si porta a duecento iarde. Se il 688 si fa sentire, entra un
Victor III.

1. **Stare fermi non vuol dire essere muti.** Un battello fermo ha ancora il suo rumore: nel classico una Krivak lo
   sente anche da lontano. Nell'Enhanced conta la distanza (T1, T3).
2. **La guardia si fa da fuori.** Il 688 parte a 14 km dalla nave, fuori dal giro della scorta: avvicinarsi per
   «vedere meglio» è il modo di farsi scoprire.
3. **Classificare senza farsi sentire:** il secondario si fa col passivo, non col ping (T4).

Si vince a 1 h 50', col braccio a bordo (esito 40), e il giudizio lo danno gli obiettivi; scoperti, entra il Victor.

### S04 «Campane d'edera» e S05 «Il guardiano del cavo» (E08, Ivy Bells) — le due facce

S04 col 688: passare le Curili, restare due ore sopra il cavo del Mare di Okhotsk, uscire senza essere scoperti. Ha la
difficoltà massima (5). S05 con l'Alfa: proteggere la nave di soccorso e trovare l'intruso fermo sul
fondo. Vedi il §8.1.

### S14 «Il pedinatore in attesa» (E13, Holy Loch 1974) — Alfa, pace, notte, difficoltà 3

L'Alfa aspetta fuori dal canale l'SSBN americano che esce in superficie, lo riconosce a vista e lo registra dopo
l'immersione. Davanti a lui una Knox pulisce l'uscita col VDS, la schiera e l'SH-2F.

1. **Di notte si riconosce dalle luci** (T10): la luce ambra, la sagoma; il telemetro sul fanale d'albero dà la
   distanza entro il 15%.
2. **Il VDS non si nasconde sotto lo strato** (T2): lo strato protegge da uno scafo, non da un sonar calato sotto.
3. **L'attesa si fa piano e lontano dalla rotta della scorta.** Senza ordini, con l'Alfa fermo a 8 km dal punto dove la
   Knox entra a 25 minuti, la Knox lo scopre fra 78 e 138 s dopo (3 semi).

La firma conta solo entro 5 km dopo l'immersione, per 30 minuti (esito 40). Se la Knox classifica l'Alfa almeno
POSSUB, o lo tiene 10 minuti, getta la granata di segnalazione e la firma non conta più. Un Alfa sulla rotta
d'immersione provoca la collisione.

### S25 «Ombra del boomer» (E23, K-219) — 688, pace, 6 ore

In coda a un SSBN sovietico in pattuglia: tenerlo, registrarlo, non farsi classificare. Poi l'SSBN emerge in
emergenza, e l'ordine è restare nascosti, riferire e non interferire.

1. **Il pedinamento lungo è disciplina di distanze.** Mai sotto le 1000 iarde: a 800 m il secondario cade.
2. **Il rapporto al momento giusto:** entro 30 minuti dall'emergenza, da quota periscopica, e a più di 5 miglia
   dalle navi sovietiche.
3. **Non fare niente è la missione** («Niente, e niente è difficile»).

La prova: in coda a 2,5 km fino all'emergenza (7220 s), il rapporto da 12 km, il rimorchio a 11035 s, il cavo che si
spezza e il richiamo, esito 40 a 14305 s.

## 3. Da vicino: pedinare chi ti può urtare

### S10 «Il cavo della fregata» (E19, K-324 e la *McCloy*) — Alfa, pace, difficoltà 3

L'Alfa si infila dietro la poppa di una Perry che fila la SQR-19, nel cono sordo del sonar di scafo, e registra la
schiera. A un'ora la Perry fa il suo Crazy Ivan, un'accostata di 100° a 18 nodi.

1. **Il cono sordo è dietro, ma dietro c'è il cavo** (T3, T10): con lo sprint and drift la schiera sta fra 180 e 280 m
   e il cavo scende in diagonale dalla poppa.
2. **La firma si prende da vicino:** conta solo entro 5 km dal cavo, per 30 minuti.
3. **Classificato, bruciato.** L'operazione fallisce quando una delle due Perry classifica l'Alfa almeno POSSUB, o lo
   tiene in contatto per 23 minuti, non al primo rilevamento.

Sotto i 50 m dal cavo la missione finisce (esito 41). L'Alfa fermo alla partenza non vince: una Perry lo classifica e la
missione finisce con l'esito 42, a 665, 695 e 726 s coi semi 1, 2 e 3.

### S19 «Pietra santa» (E05, *Gato* e K-19) — 688, pace, difficoltà 4

Holy Stone: stare sotto la coda di un SSBN che esce, oltre le 12 miglia, e registrarlo 40 minuti entro 4 km. L'SSBN
fa i Crazy Ivan di 120-180°.

1. **Più vicino non registri meglio, e il Crazy Ivan fa più paura.** La registrazione vale uguale ovunque entro 4 km. Alla
   stessa quota, sulla sua rotta di ritorno, c'è la collisione del 1969 (esito 41).
2. **Mai sotto i 500 m** (secondario), mai dentro le 12 miglia (primario).
3. **Restano le Grisha III con lo Shelon:** la scorta antisom c'è anche in pace (T10).

La prova vince a 2 km di poppa, ma col Crazy Ivan spento e la scorta resa cieca: quella cifra non dice quanto rischi con l'SSBN che accosta.
Senza ordini il 688 a 6 km di fianco non vince (tempo scaduto, esito 21, in 3 semi su 3) e viene scoperto: la prima volta a
2979 s dal Delta col seme 1, a 4202 e 8174 s da una Grisha coi semi 2 e 3.

### S20 «Cinquecento iarde» (E24, l'*Augusta* 1986) — 688, pace, difficoltà 3

La firma di un SSBN in pattuglia, che migliora con la vicinanza, fino a 1800 punti.

1. **La distanza è una scelta di prezzo.** La firma vale 10 a ogni controllo entro 5 km e 20 in più entro 2 km: da
   1,5 km la si chiude in meno di 15 minuti, da 4 km ce ne vogliono più di 28.
2. **Sotto le 500 iarde (457 m) il secondario cade,** e dopo un'ora l'SSBN scende a 130 m e rallenta a 3 nodi. I suoi
   Crazy Ivan non hanno un orario: lì la collisione è possibile (esito 41).
3. **Tre rotte stimate dell'SSBN, al MATE:** la TMA da vicino (T5, regola 7), col Victor III di scorta che gira intorno. Il
   secondario conta tre stime entro 20° dalla rotta vera, ad almeno 5 minuti l'una dall'altra.

### S24 «Quota zero» (E06, *Tautog* e K-108) — Alfa, esercitazione, difficoltà 3

Il lato del pedinato. Durante un'esercitazione d'attacco l'Alfa deve trovare, classificare e allontanare lo Sturgeon
che gli sta nel cono sordo.

1. **Chi ti segue sta dove non senti** (T3, T10): senza manovre lo Sturgeon passa da 1500 a circa 300 m in dieci minuti (3 semi). Il
   primo contatto c'è già a 100-112 s, a 1,45 km, ma l'obiettivo chiede due minuti di contatto in tutto.
2. **Il Crazy Ivan lo rivela:** con un'accostata ogni 5 minuti, nella prova (seme 1), i due minuti di contatto arrivano a 420 s.
3. **Il passivo dà il rilevamento, non la quota.** Più stretto lo cerchi, più rischi l'urto alla stessa quota
   (esito 41).
4. **Il ping lo allontana:** classificato a mano e pingato, se ne va (nella prova, seme 1, a 1380 s è a 8,5 km).

### S17 «Sotto la prora» (E20, K-314 e la *Kitty Hawk*) — Alfa, pace, difficoltà 4

Ritrovare un gruppo portaerei col mare che sale e tenerlo 60 minuti, senza farsi classificare dalla scorta e mai a meno
di 1000 m dalla portaerei.

1. **Ritrovarlo è facile:** a 15 nodi la portaerei si sente da 20 miglia. Ma a 8 nodi l'Alfa si sente altrettanto.
2. **Restare di poppa:** a 40 minuti il gruppo accosta di una cinquantina di gradi verso l'Alfa. Chi risale davanti alla
   prora ripete il 1984 (esito 41); sotto i 1000 m dalla portaerei, da qualunque lato, cade il secondario.
3. **La scorta ha due orecchie diverse** (T3): il VDS della Knox con l'SH-2F, la SQR-19 della Spruance.

La prova: un'ora a 5 km di poppa con la svolta e il rapporto, esito 40.

## 4. Crisi: il confine, la scala, chi spara per primo

In crisi la regola di T10 vale per tutti questi casi: **il primo colpo decide la guerra**. Si vince stando un passo
prima della linea.

### S03 «Quarantena» (E02, i Foxtrot di Kama) — 688, crisi, difficoltà 3

Tenere il contatto con un Foxtrot finché la batteria lo costringe a emergere, senza armi. Vedi il §8.2.

### S08 «Valona» (E01, Pasha Liman 1961) — Alfa, crisi, difficoltà 2

Portare fuori dalla baia la nave appoggio e quattro Foxtrot, sotto gli occhi di un 688 e di un Orion della NATO, e
riportare in colonna il Foxtrot che a 20 minuti torna verso i moli. La presenza basta: l'Alfa che entra nella sua zona
lo riporta in colonna. È fra le più facili dell'Alfa, e insegna che in crisi vince chi si fa vedere al posto giusto.

### S11 «Tenete il confine» (E14, U-137) — Alfa, crisi, difficoltà 3

Coprire un Foxtrot incagliato dentro le acque di un paese neutrale, restando fuori dalle 12 miglia, e accompagnarlo a
casa quando il rimorchiatore lo porta fuori (a 7 nodi, in superficie).

1. **Il confine è un muro:** entrare nella zona delle 12 miglia chiude con l'esito 41 (T10).
2. **Il dubbio:** a 50 minuti due eco radar si avvicinano e scatta l'allarme: le navi neutrali possono pingare, ma di rado,
   con la loro allerta bassa. Sono mercantili. Chi spara qui ha perso.

### S15 «L'ancoraggio» (E11, la 5a Eskadra 1973) — 688, crisi, notte, difficoltà 4

Sapere chi c'è all'ancoraggio sovietico prima dell'alba: riconoscere a vista almeno quattro unità e riferire, senza
farsi classificare.

1. **Di notte prima le luci di fonda, poi quelle di moto** (T10): quando l'ancoraggio salpa, i fanali cambiano.
2. **Il VDS Vega a 100 m rende inutile lo strato** (T2), e una seconda Krivak gira a luci spente.
3. **In crisi ti cacciano, ma non sparano:** classificato, Kara e Krivak ti danno la caccia col sonar attivo. Senza
   ordini, in tre ore, il sonarista avverte da 360 a 527 volte «ci stanno pingando» (3 semi), e non parte nessun lancio.

### S16 «La scala» (E12, l'Hardangerfjord 1983) — Alfa, crisi, difficoltà 4

Entrare in un fiordo largo 600-1000 m, fotografare la nave appoggio da meno di 2 km e uscire. Il paese applica la scala
vera: il segnale di emergere, le cariche di segnalazione, poi l'attacco coi Terne a 15 minuti.

1. **La scala si sente arrivare:** parte con la classificazione (almeno POSSUB) o con 5 minuti di contatto; persi per
   10 minuti, riparte da capo.
2. **Lo strato del fiordo è quello termico del teatro** (T2, T11): l'acqua dolce dei fiordi veri non è nel modello.
3. **Emergere è una scelta:** dopo il segnale ferma la scala, ma è l'esito 43, «richiamato». La foto e il largo danno
   il 40.

### S18 «L'arcipelago» (E15, Hårsfjärden 1982) — 688, crisi, difficoltà 5

Fotografare la base in fondo all'arcipelago e uscire, attraverso uno sbarramento di 89 mine comandate da terra
(ogni 300 m, con tre tratti più larghi, su fondali fra 20 e 50 m; scoppiano 10-30 s dopo che il battello è arrivato a
150 m, se è ancora lì).

1. **ROE a zone:** fuori ti avvertono e ti scortano; scoperto nelle acque interne, l'area si chiude, le mine passano
   alla stazione e le corvette attaccano.
2. **Acqua bassissima** (T11): fondali da 0 a 45 m, si parte fra 30 e 45.
3. **La mina la decide qualcuno a terra:** non scoppia perché passi, ma perché ti hanno scoperto.

La prova: la foto entro 3 km e il largo, esito 40, nessuna mina scoppiata. Scoperti dentro, la mina scoppia sotto il 688.

### S06 «Petroliere» (E26, la guerra delle petroliere) — 688, crisi, difficoltà 3

Scortare da sotto tre petroliere nello stretto di notte. Un «mercantile» senza luci posa mine, e l'ordine di
intercettarlo arriva dopo che la prima petroliera ne ha urtata una, o al più tardi a 90 minuti.

1. **Non sparare prima dell'ordine** (T10): il posamine nega, e l'ordine FLASH «intercettare» arriva col primo colpo.
2. **Le mine appena posate sono fra te e lui** (T11): tre Mk 67, una ogni 2 minuti, sulla rotta delle due petroliere
   che restano. Hai circa mezz'ora per fermarlo.

## 5. Barriere e inganni

### S09 «Aport» (E21, l'operazione Aport 1985) — Alfa, pace, difficoltà 4

Passare la soglia Islanda-Fær Øer sotto il SOSUS senza un fix, fino all'area di pattugliamento.

1. **Ogni datum ha un ritardo** (T3, T9): il briefing dà un'ora perché un Orion arrivi sul punto. Si gioca a scatti e
   derive, sotto lo strato, aspettando che il datum invecchi.
2. **Quando la NATO guarda altrove:** a 1 h 30' un Victor III del gruppo viene scoperto e martellato. Restare in
   silenzio, o scattare?

Oggi i cue SOSUS sono a tempo (a 1, 2 e 3 ore, sulla rotta probabile) e non seguono la velocità dell'Alfa: la regola
dello scatto «che si paga» qui vale meno che in partita libera.

### S22 «La rotta lunga» (E25, l'operazione Atrina 1987) — Alfa, pace, difficoltà 4

Cinque battelli d'attacco su cinque rotte. Si vince se, a cinque minuti dalla fine, almeno tre su cinque sono senza un inseguitore
NATO, senza armi.

1. **Si gioca per gli altri.** Il SOSUS dà un datum, accorrono P-3C, boe e cacciatori. La scheda iniziale diceva che uno scatto a 40 nodi
   vicino a un Victor inseguito tirava via i cacciatori. Non è così, e l'idea è stata tolta dalla missione (05/11/2026): i contatti non si
   escludono, il conteggio di un Victor continua a crescere finché resta nel raggio, e «tenuto» non si cancella.
2. **Chi passa dipende molto dal seme:** con lo stesso giocatore, su 36 semi, i battelli liberi vanno da 1 a 3 da nascosto, da 0 a 4 di corsa.

| Build del 06/11/2026 | semi | battelli liberi | media | esito 40 (intervallo al 95%) |
|---|---|---|---|---|
| l'Alfa nascosto a 4 nodi | 1-12 | 2 2 2 3 1 2 2 2 2 2 1 2 | 1,92 | 1 su 12 (1-35%) |
| l'Alfa che scatta vicino a un Victor inseguito (16 scatti) | 1-12 | 0 2 2 2 1 2 1 2 2 0 0 3 | 1,42 | 1 su 12 (1-35%) |
| l'Alfa a 20 nodi per tutta la missione | 1-12 | 4 2 3 4 1 0 4 0 2 2 4 2 | 2,33 | 5 su 12 (19-68%) |
| l'Alfa nascosto a 4 nodi | 13-36 | 3 2 2 2 3 2 1 3 3 3 3 1 2 1 2 2 3 2 3 2 1 3 2 3 | 2,25 | 10 su 24 (24-61%) |
| l'Alfa a 20 nodi per tutta la missione | 13-36 | 2 3 4 4 4 1 3 4 2 3 2 2 3 2 2 3 1 2 4 2 2 4 0 1 | 2,50 | 11 su 24 (28-65%) |

Una sola politica di gioco: il pilota dello scatto sa chi è inseguito, più di quanto sappia un giocatore vero, e un giocatore
potrebbe trovarne un'altra (`tools/re/prove/s22_esca/`). Lo scatto, giocato solo sui semi 1-12, non dà più liberi: la media scende da 1,92 a 1,42, e dodici semi non dicono con sicurezza di quanto. Le cifre del 05/11, 2,25 liberi e 4 su 12 da nascosto contro 0,92 e 0 su 12
con lo scatto, sono superate.

**Il vantaggio della corsa non ha retto alla prova.** I semi 1-12 l'avevano suggerito, 5 su 12 contro 1 su 12, quindi la prova sono i 24 giocati dopo, con la stessa build e lo stesso pilota: 11 su 24 contro 10 su 24. Sei semi vincono solo di corsa, cinque solo da nascosto; lo scarto è di 4 punti, e al 95% sta fra -22 e +30. I liberi salgono in 13 semi, scendono in 7 e restano pari in 4, in media di 0,25 a partita (da -0,25 a +0,75). Ventiquattro semi non trovano una differenza e non ne escludono una di una ventina di punti, in un verso o nell'altro. L'1 su 12 da nascosto era un gruppo di semi sfortunato: tre semi rigiocati ridanno la stessa partita, e lo scarto con 10 su 24 viene solo dai semi.

Cambia la forma della partita. Su tutti i 36 semi, a 20 nodi Kidd e i due Los Angeles hanno l'Alfa in 70-72 minuti; da nascosto lo tiene comunque il secondo Los Angeles, fra 74 e 226 minuti. Il secondario sull'Alfa è perduto in tutti i semi, in entrambi i modi. Da nascosto i liberi stanno sempre fra 1 e 3; di corsa ce ne sono 4 in 10 semi su 36 e nessuno in 3. Nascosti non si perde tutto e non si libera tutti; correndo capitano entrambe le cose. Di tutto questo non si sa quanto regga con un giocatore vero: è una rotta sola, tenuta per quattro ore. Un battello è «tenuto» quando un cacciatore ha 70 minuti di contatto con lui, anche sommati. Un'arma lanciata dà l'esito 42.

## 6. Guerra ed esercitazione

### S01 «Vestfjord» (E30, Northern Wedding '88) — 688, esercitazione, difficoltà 1

La missione d'ingresso della libreria: bonificare un fiordo dall'«arancione», un Trafalgar che conosce il posto, prima
che entri la portaerei. A 45 minuti un FLASH dice che la portaerei anticipa di mezz'ora, e la scorta riempie l'acqua di
rumore.

1. **Acque strette, fondale che sale in fretta** (T11): non toccare il fondo, anche se non è un obiettivo.
2. **Cercare prima del rumore** (T3): dopo la svolta il traffico amico copre l'«arancione».
3. **Classificarlo è un obiettivo a parte** (T4), prima di «colpirlo».

### S07 «Tappo» (P07, il porto minato di E09 come dottrina) — 688, guerra, difficoltà 4

Posare almeno sei Mk 67 nel canale di Kola (un cerchio di 1500 m su fondali fra 30 e 90 m) e lasciare la zona. A 45
minuti un Akula esce dal golfo.

1. **Le proprie mine sono un terreno anche per te** (T11): posa e vattene prima che si armino.
2. **L'Akula è una scelta:** se arriva al mare aperto, o se lo colpisci, scatta l'allarme.

La prova nell'Enhanced: sei Mk 67 nel campo, la ritirata, esito 40. Nel classico le mine non ci sono e il campo non si
posa.

### S12 «Il gruppo cacciatore» (E10, *Hangor* e *Khukri*, come dottrina) — Alfa, guerra, difficoltà 3

Due Perry e un SH-60B cercano l'Alfa da sei ore. A 20 minuti una delle due rallenta a 12 nodi per provare il sonar,
fino a 35: per quindici minuti la preda è lei.

1. **La finestra è breve:** a 12 nodi la fregata sente meglio, ma è anche un bersaglio più facile (T6, T7).
2. **Il lancio ti scopre:** al primo lancio l'altra Perry accorre a 28 nodi, e l'elicottero cala il sonar sul punto
   della prova (T8, T9).
3. **Uscire conta solo dopo:** con almeno una fregata affondata, l'uscita dalla zona dà l'esito 40.

### S13 «Outlaw Shark» (dossier 27, le prove del TASM) — 688, esercitazione, difficoltà 2

Il tiro antinave oltre l'orizzonte: un TASM inerte su un vecchio Kara rimorchiato a circa 100 miglia, nel traffico, col
dato dell'E-2 che arriva per radio a 10, 40 e 70 minuti con 3 km d'errore.

1. **Il dato invecchia** (T6): al secondo aggiornamento il bersaglio ha già accostato (a 30 minuti), e un mercantile col
   radar di navigazione acceso gli passa vicino.
2. **L'ESM del TASM sceglie da sé:** sente fino a circa 70 km solo Top Sail e Head Net C e ignora i radar di
   navigazione. Il Kara li ha accesi, il mercantile non ha quelli.
3. **Lanciare entro un'ora dal primo dato** (secondario): aspettare un dato migliore costa.

## 7. Senza nemici: S23 «Acqua a due gradi» (E27, *Komsomolets*) — Alfa, pace, difficoltà 3

Un battello sperimentale è emerso con l'incendio a poppa. L'Alfa è il più vicino, a 60 km; il peschereccio-madre va
verso una posizione sbagliata. Non ci sono colpi da tirare: si contano i salvati.

1. **La velocità serve:** arrivare entro 2 km dal battello entro 90 minuti.
2. **La trasmissione al momento giusto:** il rapporto da quota periscopica, col battello entro 5 km, fa accostare il
   peschereccio a tutta forza.
3. **Accettare l'aiuto:** a 2 h 30' di missione la Norvegia offre una nave, e la si accetta con una trasmissione.

| Nella prova | salvati |
|---|---|
| il rapporto da 3 km a 50 minuti: il peschereccio arriva prima dell'affondamento | 40 |
| senza rapporto, ma con l'aiuto norvegese chiesto subito | 35 |
| senza rapporto e con l'Alfa lontano | 15 |
| senza ordini | 15 |

Il 35 è del seme 1 (la prova chiede solo più di 15), e la riga «senza ordini» viene dalla partita della scheda. Le cifre
dei salvati per minuti in acqua sono una STIMA DI GIOCO (35 entro 20 minuti, 25 entro 45, 15 entro 80, 5
oltre).

## 8. Due episodi da vicino

### 8.1 Ivy Bells (E08): il cavo visto dai due lati

Fra il 1971 e il 1981 un dispositivo americano ascoltava un cavo militare sovietico sul fondo del Mare di Okhotsk.
Quando e come i sovietici lo scoprirono resta incerto. La libreria ne fa due missioni, una per lato. Giocarle di seguito
è il modo migliore di capire che cosa vuol dire un sottomarino fermo.

**S04 «Campane d'edera» (688, pace, difficoltà 5, 4 h 30').** Passare lo Stretto di Ekaterina, restare due ore nella
zona del cavo, uscire senza essere scoperti.
- Le due ore si contano con una regola: una variabile cresce ogni 10 s col 688 nella zona (720 controlli). La regola
  non guarda velocità e quota, ma la Krivak, l'Il-38 e il Ka-27 sì.
- Dopo 28 minuti di lavoro una nave sovietica parte per il cavo, da 10 km. Ci arriva a circa un'ora di lavoro, si ferma
  e riparte dopo una quarantina di minuti. Restare fermi sotto di lei o abbandonare il lavoro? Se nessuno ti ha sentito,
  il conto delle due ore continua.
- L'uscita conta solo a lavoro finito. Con l'allarme entra un Victor III.

**S05 «Il guardiano del cavo» (Alfa, pace che può diventare crisi, difficoltà 3).** L'intruso è un Los Angeles fermo
quasi sul fondo, a 400 m dal punto del cavo.
- **Il passivo lo sente appena.** Un sottomarino fermo sul fondo è il silenzio più perfetto: in passivo ci si arriva solo
  da vicino, a 2,5 km nel collaudo, e il sonar attivo lo trova da più lontano (T3, T4).
- Un ping entro 3 km lo fa fuggire allo stretto a 20 nodi, e la radio porta le ROE dei sottomarini a «stretto».
  Uscito l'intruso, esito 40; persa la nave di soccorso, 41.

La lezione delle due insieme: **chi aspetta fermo vince contro il passivo, e perde contro il ping.** Nel 688 di S04 sei
tu il Los Angeles fermo di S05.

### 8.2 Kama (E02): la caccia fino alla batteria

Nell'ottobre 1962 quattro Foxtrot sovietici andarono verso Cuba durante la quarantena. I cacciatori americani, con
segnali non letali (le bombe di profondità da esercitazione), ne costrinsero tre in superficie senza un colpo sparato,
uno dopo circa 14 ore di contatto continuo, uno con le batterie quasi scariche. Il quarto non fu mai confermato. Il
racconto del siluro nucleare è controverso e resta fuori dalla missione.

**S03 «Quarantena» (688, crisi, difficoltà 3, 3 h).** Tenere il contatto col Foxtrot finché emerge, senza armi, e
ritrovare il secondo diesel segnalato una volta sola.
- **La batteria è il tempo della missione.** Col modello del runtime un diesel in caccia si trascina a 2 nodi, e la
  batteria piena gli durerebbe 175 ore. Per questo il Foxtrot parte, dopo quattro giorni di caccia, col 6%, e il
  briefing lo dice.
- **Sotto il 30% andrebbe a snorkel,** ma non se è in caccia: se un ostile ne conosce la posizione da meno di dieci
  minuti. Il 688 che lo pinga e lo tiene lo costringe a emergere al 5% (con 5 minuti di contatto, esito 40).
- **Chiunque tenga conta,** anche il P-3C. Ma se nessuno lo tiene per dieci minuti, va a snorkel, ricarica, e la
  vittoria si allontana.
- A un'ora il Foxtrot accosta verso ovest; a 90 minuti la radio segnala il secondo, con 8 km d'errore.
- **La missione è sul filo.** Fra la carica di partenza al 6% e la soglia al 5% c'è poco, e il tempo è di tre ore: su dieci
  semi il comandante IA arriva all'esito 40 una volta sola, al seme 1. Con la regola di prima dei rapporti dei battelli
  era sempre una su dieci, ma al seme 10. Non è un effetto della regola: togliere un rapporto sposta i numeri casuali di tutti,
  e la partita prende un'altra strada. Dieci semi bastano a vedere che lo scarto va nei due versi, non a dare una frequenza.

La lezione: **in crisi il sonar attivo è l'arma.** Il ping non fa danni, ma tiene il diesel in caccia, e la caccia gli
toglie la ricarica (T10).

## 9. Tre originali nel replay

Tre missioni del 1989 giocate dal vetrino (volume 1, capitolo 12), col comandante IA al posto del giocatore e tre semi per missione. I
numeri vengono da quelle tracce (`serie_sandbox --semi 3 -- --missione SCENE1A`, build del 06/11/2026): tre partite
non fanno una statistica, e dicono che cosa è successo, non quanto spesso succede. Il comandante non vince nessuna delle
tre, e per questo sono utili: fa quello che dice il briefing, e il replay mostra dove non basta. Che cosa basta per
vincere qui non è misurato: le righe «Da provare» sono da giocare.

### SCENE1A «TORPEX '89» — quindici minuti per tre navi

Si devono affondare tre cacciatorpediniere in 15 minuti: a 900 s lo script chiude col conto delle navi a galla, e con
tre a galla è l'esito 5, un fallimento. Nel registro le navi si chiamano Spruance.

| Seme | Scoperto da una nave | Primo lancio | Lanci | A segno | Esito |
|---|---|---|---|---|---|
| 1 | 94 s | 302 s, a 6,1 km | 4 | 0 | 5 |
| 2 | 104 s | 505 s, a 4,1 km | 4 | 0 | 5 |
| 3 | 115 s | 491 s, a 5,4 km | 4 | 0 | 5 |

Che cosa mostra il replay, in VERITA':
1. **Il tempo se ne va nella TMA.** Il comandante fa le gambe sul primo contatto e lancia il primo siluro fra i 5 e gli
   8 minuti e mezzo. Da lì ne parte uno ogni due minuti: in 15 minuti sono quattro, e nessuno va a segno.
2. **La nave sente il siluro.** La prima ritirata arriva da 1 a 33 s dopo il primo lancio, e il registro la ripete
   (da 6 a 11 ritirate per partita). L'ultimo lancio parte da più lontano del primo: a 7,9, 5,9 e 8,0 km dalla Spruance più vicina.
3. **La testa si ferma sul biologico.** Dopo l'attivazione il siluro prende per buono il biologico della scena, e il
   filo lo rifiuta (`esca_rifiutata`, sorgente 1): 9 volte su 10 attivazioni. In tre partite non c'è un solo aggancio di una nave.
4. **Il tuo siluro confonde anche te.** Col seme 1 Sierra 2 passa dalla Spruance al Mk 48 (`scambio_tracce`, da 4 a 33 per
   partita): il sonarista segue il tuo siluro al posto del bersaglio (T4).

**Da provare:** una soluzione di massima e il primo lancio ben prima dei 5 minuti, da vicino, col filo tenuto per
guidare (T6). Il prezzo è farsi sentire prima (T1).

### SCENE2A «Scrollatevelo di dosso» — seminare senza sparare

Un Alfa ti sta addosso: al primo contatto è a 1,3 miglia. Si vince se perde il contatto e non lo riprende per 200 s
(esito 5), o se a 20 minuti l'ha ancora ma oltre le 9 unità (esito 6). Altrimenti, a 1200 s, è l'esito 4, un
fallimento. Non si spara, salvo che i russi facciano sul serio.

| Seme | L'Alfa ti scopre | Il Victor ti scopre | Lanci | Esito |
|---|---|---|---|---|
| 1 | 105 s | 140 s | 0 | 4 |
| 2 | 103 s | 163 s | 0 | 4 |
| 3 | 110 s | 166 s | 0 | 4 |

In tutti e tre i semi il comandante gioca lo sprint and drift di T3, agli stessi istanti: scatto oltre lo strato a 11 s,
deriva a 241 s, di nuovo scatto a 600 s, deriva a 841 s. Non spara mai, e il secondario lo tiene. Ma l'Alfa non molla.

**La lezione:** lo schema da manuale non è una garanzia. A un miglio e mezzo, in questi tre semi, non ha fatto perdere il
contatto una sola volta per 200 s: l'unico contatto perso è del seme 3, a 546 s, e l'Alfa lo riprende 60 s dopo. **Da provare:** rompere il contatto nei primi 100 s, prima che l'Alfa ti scopra, e
variare i tempi (T2, T3).

### SCENE0A «Colpisci e fuggi» — il lancio è la metà facile

Quattro Tomahawk (nel registro il missile è un TASM) sugli obiettivi entro 5 minuti (a 300 s, se ne manca uno, è l'esito 3), poi le acque di Copenaghen
(esito 5, o 6 se è mancata almeno una unità sovietica). Fuori dalla zona è l'esito 4.

| Seme | I quattro lanci | Esposizioni lunghe del periscopio (durata) | Primo colpo subito | Colpi in tutto | Battello perduto | Fine (esito 2) |
|---|---|---|---|---|---|---|
| 1 | 222-252 s | 250 s, 104 s | 831 s | 6 | 6201 s | 6257 s |
| 2 | 22-53 s | 51 s, 879 s | 2287 s | 3 | 2602 s | 2700 s |
| 3 | 222-252 s | 250 s, 88 s | 898 s | 2 | 969 s | 1029 s |

1. **Il lancio riesce sempre.** Il comandante lancia in tempo in tutti e tre i semi, da quota periscopica.
2. **La fuga no.** Nessuno dei tre arriva a Copenaghen: il battello è colpito e perduto. Coi semi 1 e 3 la prima a prenderlo è
   la Krivak: lancia i Type 40 da 474 e 561 s, e due vanno a segno a 831-835 e 898-901 s. Col seme 2 la caccia arriva più tardi e
   da più parti: Type 40 di Kara e Slava e Type 53 dell'Alfa, nessun Type 53 a segno. Il Type 40 è l'arma dello
   script del 1989, che lo porta sul bersaglio: non è il siluro col modello di D1, e le manovre di T8 non lo riguardano.
3. **Si scappa troppo in alto.** Il primo colpo arriva sempre vicino alla superficie: a 2-4 m dopo una fuga a 25-29 nodi.
   Col seme 1 il battello urta il fondo a 849 s, subito dopo i primi due colpi. Col seme 2 lo urta a 1694 s, e di nuovo a
   2299 s, 12 s dopo il primo colpo. Solo il seme 3 non tocca il fondo (T11).
4. **Una volta colpito non si riprende.** Col seme 3 il battello è perduto 68 s dopo i primi colpi. Col seme 1 il comandante passa a
   «ritirata, colpiti» e il battello resta a galla a 2-3 nodi per oltre 80 minuti, finché fra 6072 e 6095 s lo prendono due P-700 e due
   Type 40 del Kirov. Fra la perdita e la fine della partita passano 56-98 s.
5. **Un TASM non sa dov'è la guerra.** Col seme 2 tutti e quattro colpiscono un traghetto, fra 437 e 470 s, e il traghetto
   affonda a 755 s. Col seme 1 tutti e quattro finiscono la corsa senza bersaglio, fra 2105 e 2135 s. Col seme 3 la
   partita finisce prima che lo si possa dire. Tre semi non dicono quanto spesso capiti, ma il TASM cerca da sé (T6).

**Da provare:** uscire subito dopo l'ultimo lancio, a quota e velocità da transito, guardando il fondale (Ctrl+H) prima di
accelerare. Non è detto che lanciare prima sia meglio: il seme 2 lancia a 22 s e il primo colpo arriva a 2287 s, mentre i semi
1 e 3, che lanciano a 222 s, lo ricevono a 831 e a 898 s. Tre semi non separano l'istante del lancio dal resto.

## 10. Quattro esperimenti sul convoglio

Un esperimento A/B del vetrino di Dio (volume 1, §12.9) gioca la stessa partita due volte sugli stessi semi: A senza intervento, B con. Qui
la scena è la S5 del vetrino, il convoglio: sei navi, tre Perry di scorta, un Victor III e il Los Angeles, per 7200 s. Dieci coppie
per esperimento, build del 06/11/2026 (`serie_ab`, esperimenti in `data/motore/esperimenti`). Tutte le partite finiscono allo
scadere del tempo (esito 21), quindi il confronto sta nei numeri. Dieci coppie vedono un effetto grosso, non ne misurano uno piccolo.

Le prime quattro colonne sono totali sulle dieci partite, A e poi B. Le navi perdute comprendono le Perry. L'ultima colonna è la
differenza delle mediane che il rapporto dà per netta, con l'intervallo al 95%.

| Esperimento | Navi perdute | Siluri del Victor a segno | Lanci del Victor | Victor affondato | La differenza più netta |
|---|---|---|---|---|---|
| L'allerta anticipata (scorta in A3 da 1 s) | 8 → 8 | 15 → 15 | 24 → 24 | 5 → 5 | allerta della NATO 830 s prima (374-874 s) |
| Il falso ordine di rientro (a 60 s, autenticazione 0,5) | 8 → 7 | 15 → 13 | 24 → 24 | 5 → 3 | la NATO lancia 428 s prima dopo la scoperta (163-1089 s) |
| La nave spia che vede tutto (da 0 s) | 8 → 8 | 15 → 15 | 24 → 24 | 5 → 5 | nessuna: la verità di B è uguale a quella di A |
| Lo scoppio nel transito (300 kg a 120 s) | 8 → 5 | 15 → 15 | 24 → 24 | 5 → 3 | allerta 380 s prima per la NATO (307-493 s), 477 s prima per il Patto (477-553 s) |

1. **L'allerta non basta.** Con la scorta in A3 dal primo secondo il Victor lancia nello stesso istante in tutte e dieci le coppie, e
   lanci, siluri a segno e navi perdute non cambiano. In A il primo lancio cade fra 598 e 758 s. Non manca la scoperta: col seme 1 una
   Perry scopre il Victor a 81 s, in A come in B, e il Victor lancia a 758 s. La A3 non cambia che cosa la scorta fa col contatto che ha
   già: in quattro coppie le posizioni non si separano mai, nelle altre sei si separano fra 1362 e 6875 s. Il Victor è affondato negli
   stessi cinque semi (1, 4, 5, 9, 10), e cambia solo l'istante: di 3 s col seme 4, di 146 s col seme 9.
2. **Il falso ordine sparpaglia, ma non come ci si aspetta.** L'ipotesi era che le Perry lasciassero il convoglio e il Victor
   colpisse di più. L'ordine lo ricevono le nove navi di ogni partita e, in tre partite su dieci, l'SH-60B, quando entra in scena
   fra 827 e 918 s: 93 unità in tutto. Di queste 38 ci credono, 23 lo ignorano e 32 chiedono conferma, e chi chiede lo smaschera
   sempre. Delle 30 Perry ci credono 10. La scorta si allarga davvero: a 1200 s la Perry più lontana dal centro
   del convoglio sta a 15,1 km contro 7,6 (mediane). Il Victor però non colpisce di più: i siluri a segno vanno da 15 a 13 e le navi
   perdute da 8 a 7. Arriva alla soluzione più tardi in 6 coppie su 7 (mediana 2471 s contro 758), ma l'intervallo della differenza
   comprende lo zero (da -252 a +2091 s). Dieci coppie non distinguono questi scarti dal caso.
3. **Il quadro serve a chi sa usarlo.** Con la spia il confronto non trova una divergenza in nessuna delle dieci coppie: nei numeri
   si muove solo la media dell'antenna alzata della NATO, di decimi di secondo, in cinque coppie. Il perché è nelle regole (E6). Verso un sottomarino la
   spia passa dal canale radio: il quadro arriva solo a quota d'antenna, al ciclo del broadcast. Il dato orienta il battello e non è
   mai una soluzione, e se ha un contatto suo solido non conta: il Victor ha il primo contatto solido già a 32-33 s. Fra i lettori del quadro
   ci sono i lanci di missili, e il Victor III non ne ha. Questa scena quindi non dice quanto vale l'informazione: l'ipotesi
   dell'esperimento qui non si può provare. La prova è nella scena M3, qui sotto.
4. **Lo scoppio sveglia tutti, anche il nemico.** A 120 s vanno in A3 sia la formazione del convoglio sia quella del Victor. Il
   primo lancio cambia al più di un secondo, i lanci restano 24 e i siluri a segno 15. Le navi perdute scendono da 8 a 5 e il Victor
   affondato da 5 a 3, ma l'ipotesi che la scorta scoprisse prima il Victor non ne esce provata: la differenza delle mediane delle navi
   perdute comprende lo zero (-0,5, da -1,5 a +1,0). Sono partite che prendono un'altra strada: coi semi 3 e 8 due siluri a segno e nessuna nave
   perduta, contro due navi in A; col seme 9 una petroliera perduta e il Victor che si salva, contro nessuna nave e il Victor affondato in A.

**Il cannone e il deposito.** Se il Victor colpito risale e resta a galla, le Perry lo prendono col Mk 75, entro 8 km. In A capita in 2 semi su 10.
Col seme 4, un Mk 48 del Los Angeles a 1168 s, poi 561 colpi nel registro, 196 a segno, fra 1287 e 4018 s, e a 5117 s lo affonda un Harpoon
di una Perry. Col seme 9, un Mk 48 a 1490 s, poi 922 colpi, 445 a segno, fra 2802 e 5920 s, quando l'integrità arriva a zero (la regola è in volume 2, E4).
Il registro conta i colpi, rimbalzi compresi, e il deposito i proietti, che contro un sottomarino sono 450 per pezzo: col seme 9 la prima Perry si
ferma a 4035 s con 538 righe, e il resto lo tira l'altra. Che il suo deposito sia finito lo dice il conto dei tempi: i proietti
sparati non sono nel registro. Con gli interventi il cannone si sposta: col falso ordine sparano nei semi 4 e 10, e nel 9 non più; con lo scoppio solo nel 4.
Coi semi 1, 5 e 10 di A il Victor affonda col solo Mk 48, da 58 a 352 s dopo il colpo.

Se non si interviene, la S5 dà sempre lo stesso conto: le partite A sono uguali in tutti e quattro gli esperimenti. La NATO lancia in media 153,3 volte
a partita, ma la mediana è 4, da 2 a 926: la media viene quasi tutta dai 2 semi del cannone, e non descrive una partita tipica. Le
contro-scoperte della NATO sono in media 3,7 (0 2 0 9 1 3 10 4 7 1). Sono le volte in cui un'unità viene scoperta mentre si espone, dopo almeno 30 s di
silenzio di quella coppia: non quanti nemici l'hanno scoperta.

### La spia oltre l'orizzonte

La spia nella S5 non misurava nulla, così l'esperimento è stato rifatto dove il quadro serve a chi lancia: la scena M3, «Oltre
l'orizzonte» (`spia_oltre_orizzonte.m688v`). In guerra, a sud delle Faroe, uno Slava con una Krivak e, a 120 km, due Perry e un Los Angeles, per
3600 s. Ai 300 km della griglia le unità veloci si sentono: contatto solido senza posizione nota. Il Patto non lancia; il Los Angeles del
comandante IA sì, da lontano (§12). Esito 21 in 10 su 10, e nessuno script comanda il tiro. Le Perry hanno il contatto solido sullo Slava
fra 83 e 135 s. Dieci coppie, build del 06/11/2026.

| | Senza spia (A) | Con la spia (B) |
|---|---|---|
| P-500 lanciati dallo Slava, totale | 0 | 90 |
| Primo lancio | mai | 62 s, in tutte e dieci |
| P-500 a segno, totale | 0 | 56 |
| Abbattuti dalle Perry, totale | 0 | 17 |
| Guasti in volo, totale | 0 | 10 |
| A fine corsa, totale | 0 | 4 |
| Perry affondate | 0 su 20 | 20 su 20 |

Stavolta l'effetto è grosso e dieci coppie bastano: tutte e dieci le partite B cambiano nello stesso senso, e il confronto le separa
da A a 62 s, quando compare il primo P-500. Il primo lancio cade a 62 s perché il primo quadro arriva dopo il primo passo e lo Slava si prende i suoi 60 s di designazione (STIMA). La prima Perry
affonda fra 343 e 756 s, la seconda fra 825 e 1335 s. Tre P-500 non hanno una riga di fine. Senza spia lo Slava resta con tutti i missili a bordo: sente,
ma oltre l'orizzonte non sa dove tirare. Quanto valga l'informazione dipende quindi da chi la riceve: per il Victor della S5 niente, per lo Slava della
M3 tutto. È una scena costruita apposta, e il suo risultato non si trasferisce alle missioni della libreria.

### Il Victor e il quadro: la M4

La M4 mette un Victor III di pattuglia a 40 km da un convoglio con una Perry, in guerra, per 7200 s. Sono due esperimenti, dieci coppie ciascuno (`quadro_m4_spia.m688v`, `quadro_m4_krivak.m688v`). In uno la spia dà il quadro al
Victor, nell'altro una Krivak in più vede il convoglio e lo riferisce al teatro.

| | Senza intervento (A) | Con la spia | Con la Krivak |
|---|---|---|---|
| Esito 21 | 10 su 10 | 10 su 10 | 10 su 10 |
| Semi in cui il Victor lancia | 3 | 3, gli stessi | 3, altri (1, 2, 5) |
| Lanci del Victor, totale | 12 | 12 | 12 |
| A segno, totale | 3 | 3 | 5 |
| Semi in cui affonda la Perry (sola nave perduta) | 3 | 3 | 2 |
| Primo contatto solido del Victor sul mercantile | 113-235 s | uguale | 101-275 s |

Con la spia la partita non cambia in nessuna coppia. Con la Krivak cambia tutto da subito, perché è un'unità in più che fila il VDS e trasmette (in mediana 81,5 trasmissioni a partita), e dieci coppie non
dicono se il Victor renda di più: i semi in cui lancia sono altri, e gli scarti di 3 contro 5 e 3 contro 2 vanno letti come il caso. In tutte e due le serie il Victor sente il
convoglio da solo prima che il dato gli serva. Nella M4 con la spia, nella M4 con la Krivak e nella S5 con la spia, nei semi rigiocati dopo la regola dei rapporti (06/11), il dato radio non è mai stato usato.

**Dove il quadro serve a un battello.** Le 18 partite di controllo del quadro radio (I02, I05, I06, I07, S03, S07, S08, S11 e S15, due semi ciascuna, 06/11/2026, con la data delle scene) danno 51 salite d'ascolto,
27 gruppi di copie, un solo dato usato e nessun lancio sul solo dato. I rapporti dei battelli sono 22, tutti da quota d'antenna; su 3 trasmissioni sovietiche e 13 americane o di altre parti non esce un fix,
e il giocatore non riceve contatti da fix. Il giocatore è affondato una volta. Di queste 18 sono state rigiocate qui solo S03 e I07, seme 1, con lo stesso esito: il resto è la misura del 06/11, non di questa build.

## 11. Riepilogo

| Caso | Episodio | Battello | Situazione | La lezione | Capitoli |
|---|---|---|---|---|---|
| S01 Vestfjord | E30 | 688 | esercitazione | cercare in acque strette prima del rumore | T3, T4, T11 |
| S02 Nave mineraria | E03 | 688 | pace | la guardia si fa da fuori | T1, T3 |
| S03 Quarantena | E02 | 688 | crisi | il ping tiene il diesel giù | T10, §8.2 |
| S04 Campane d'edera | E08 | 688 | pace | entrare, restare fermi, uscire | T2, T11, §8.1 |
| S05 Il guardiano del cavo | E08 | Alfa | pace, crisi | il fermo si trova col ping | T3, T4, §8.1 |
| S06 Petroliere | E26 | 688 | crisi | niente colpi prima dell'ordine; le mine fresche | T10, T11 |
| S07 Tappo | P07, E09 | 688 | guerra | posare e andarsene | T11 |
| S08 Valona | E01 | Alfa | crisi | in crisi conta esserci | T10 |
| S09 Aport | E21 | Alfa | pace | il datum invecchia | T3, T9 |
| S10 Il cavo della fregata | E19 | Alfa | pace | il cono sordo e il cavo | T3, T10 |
| S11 Tenete il confine | E14 | Alfa | crisi | il confine è un muro | T10 |
| S12 Il gruppo cacciatore | E10 | Alfa | guerra | la finestra breve; il lancio ti scopre | T6, T7, T8 |
| S13 Outlaw Shark | dossier 27 | 688 | esercitazione | il dato che invecchia, l'ESM del TASM | T6 |
| S14 Il pedinatore in attesa | E13 | Alfa | pace | le luci, il VDS sotto lo strato | T2, T10 |
| S15 L'ancoraggio | E11 | 688 | crisi | guardare di notte; cacciati senza colpi | T2, T10 |
| S16 La scala | E12 | Alfa | crisi | le ROE a gradini; emergere come scelta | T10, T11 |
| S17 Sotto la prora | E20 | Alfa | pace | mai davanti alla prora | T3, T10 |
| S18 L'arcipelago | E15 | 688 | crisi | ROE a zone, mine decise da terra | T10, T11 |
| S19 Pietra santa | E05 | 688 | pace | vicino e il Crazy Ivan | T3, T10 |
| S20 Cinquecento iarde | E24 | 688 | pace | la distanza ha un prezzo | T3, T5 |
| S22 La rotta lunga | E25 | Alfa | pace | si gioca per gli altri; lo scatto non libera nessuno | T3, T7 |
| S23 Acqua a due gradi | E27 | Alfa | pace | si contano i salvati | — |
| S24 Quota zero | E06 | Alfa | esercitazione | chi ti segue sta nel cono sordo | T3, T10 |
| S25 Ombra del boomer | E23 | 688 | pace | il pedinamento lungo e non interferire | T10 |

S21 «La linea di base» (E28, *Baton Rouge* e *Kostroma*) è una scheda proposta e non approvata: non si costruisce. Il caso
sarebbe la caccia dalla parte di casa, col ping che allontana un 688 fermo nel suo cono poppiero.

## 12. Stato

- **Fatto:** i casi delle 24 storiche costruite (S01-S20, S22-S25), coi numeri delle loro prove; i due episodi da
  vicino (E08 ed E02); §9 e §10 rigiocati il 06/11/2026; lo scatto di S22 su dodici semi, la corsa su trentasei (§5).
- **Da fare:**
  - i «Da provare» del §9: giocarli, e dire che cosa basta per vincere le tre originali;
  - le cifre su più semi con un giocatore vero: S19 col Crazy Ivan acceso, S14, S15, S16. Le partite a giocatore fermo del
    06/11, tre semi ciascuna, dicono solo che cosa succede a chi non fa niente.
- **Corretti nelle missioni il 03/11/2026 e non ancora visti in partita** (trailer-1):
  - S04: la nave che arriva sul cavo a circa un'ora di lavoro è un conto (10 km a 10 nodi);
  - S11: i ping delle navi neutrali all'allarme sono misurati su un seme solo: nessuno prima dell'allarme, uno nei 400 s dopo,
    fra tutte e tre le navi (a 3180 s);
  - S20: la stima al MATE è misurata su una geometria simile, senza partita, e il tratto dall'interfaccia alla missione non
    ha una prova. Che cosa succede alle accostate dell'SSBN non è provato. Il comandante IA non registra stime, quindi il
    vetrino non può fare quel secondario.
- **Non verificato:** S10, la Perry del Crazy Ivan ha l'ordine di 18 nodi, col modello classico a 15.
- **Limiti delle misure, da tenere presenti leggendo §9 e §10:**
  - **Il Mk 48 del comandante IA da lontano** (difetto noto, non corretto): nella M3 senza spia il Los Angeles lancia su una Krivak
    che dista davvero 103-109 km, in 5 semi su 10, fra 546 e 2433 s: 12 lanci, 0 a segno. Lancia sulla soluzione della sua TMA, e non controlla la distanza
    col contatto. Nella M4 capita in un seme, a 100-101 km, 0 su 2. Riguarda solo il vetrino.
  - **La TMA del comandante IA sul Los Angeles non è verificata.** Su 40 semi della SCENE5B, 99 stime su 103 sono su biologici vicini e 4 sul Los Angeles,
    di cui 3 oltre i 20°. Il campione è piccolo.
  - **Filo ed ELF:** l'IA non li ha, né i battelli né il comandante. È una scelta, e un'asimmetria col giocatore (E6).
  - **L'aereo ASW sul fix:** nelle 30 partite misurate nessun fix ha mandato un aereo; non è misurato quanto spesso parta né che cosa renda.
  - **Una Perry sente troppo a 60 km:** un Victor a 3 nodi, su 1817 m di fondale, è solido in 4 casi su 5. È generosa, e c'era già prima della griglia a 300 km.
  - **Il traffico civile oltre 30 km** resta al livello A di propagazione, senza il campo. Il LOFAR, e quindi l'analizzatore, sta al livello A per tutte le sorgenti, non solo per quel traffico.
  - **Il deposito:** S07 cambia in un seme su tre, senza toccare le cifre di questo capitolo; S12, SCENE0A e S5 in nessuno su tre. S15 e S19 a giocatore fermo non hanno
    raffiche; S11 e S13 non sono state rigiocate: ci si aspetta poco, ma non è provato.
  - **La scena M1 non è come era attesa** dopo il sonar a 300 km. Non entra in questo capitolo, ma chi la confronta lo sappia: esiti 2 in 5 semi e 21 in 5 (prima 6 e 4), lanci NATO 0,7 di media (prima 0,1), del Patto 2,5 (prima 3,5). La causa probabile, meno neutrali creduti che sbloccano l'Harpoon, non è provata.
  - **Nella M4** il Victor arriva al contatto solido sul mercantile a 113-235 s: l'attesa era 110-180 s, e due semi ne escono.
- I numeri seguono la missione: se una missione cambia, la sua riga qui va riletta insieme alla scheda di docs/17.
