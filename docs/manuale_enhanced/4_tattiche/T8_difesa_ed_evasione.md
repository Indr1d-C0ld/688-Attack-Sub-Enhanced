# T8. Difesa ed evasione

*Autore: runtime; curatela: kit. Revisione del 04/11/2026. Stato: bozza curata. I numeri sono misurati col siluro del gioco
(`tools/re/prove/suite_acustica/sonda_tattiche/runtime_t8.cpp`, uscita in `runtime_t8.txt`, 300 semi per caso;
`kit_t8_misto.cpp` per il tiro dell'IA, 30 semi): le Fær Øer a febbraio, battello a 150 m e 5 nodi. **Il battello della sonda
è semplice (§3), e i numeri servono a confrontare le manovre, non a predire come finisce la tua.** La serie in partita vera
contro la Krivak (§3, «La prova in partita») non conferma tutto: le differenze sono scritte lì e nelle regole 3, 4 e 8, e le
tabelle sono rimaste quelle della sonda.*

Il manuale del 1989 divide la difesa in tre zone: lontano ci si allontana, a media distanza si accosta a 90°, vicino a
150°. Contro il tiro dell'IA l'idea regge ancora, ma nell'Enhanced le zone non sono un disegno: nascono dall'autonomia del
siluro, dalla sua velocità, da dove si apre la testa e da quanto è largo il suo cono. Prima di tutto conta la distanza a cui
lo senti.

## 1. Le regole pratiche

1. **Ascolta prima di doverlo fare.** L'intercettatore (Ctrl+W) sente le teste che pingano: «siluro in acqua», poi «siluro
   in acquisizione» (volume 1, §3). Una testa passiva non pinga: la sentono solo sonar e cuffia, e più tardi.
2. **Zona A, 8 km o più: scappa.** Poppa al siluro e tutta forza. Un SET-65 o un USET-80 lanciato da 8 km finisce la corsa
   prima di prendere il 688. Non serve altro.
3. **Zona B, attorno ai 4 km: 90°.** Da 4 km la fuga non salva più. Contro il tiro dell'IA, con la testa mista aperta al 60%
   della distanza, una rotta a 90° dalla sua lascia a zero sia il SET-65 sia l'USET-80, da tutti e tre i rilevamenti.
   L'Alfa contro il Mk 48 ci riesce da 8 km; da 4 km prende solo da prua. Da 8 km invece non serve: contro una testa
   passiva dell'USET-80 aperta tardi, a 90° ti prende dove la fuga non ti prendeva.
   **Viene solo dalla sonda:** in partita, da 150 m, la nave non ha tirato fra 1,25 e 4,45 km (§3). E lo zero vale se accosti al
   lancio. Accostando all'allarme, che suona tardi, fra 4,45 e 7,23 km il 90° ha preso 2 colpi su 9 e la fuga 1 su 9.
4. **Zona C, 2 km: esca e 150°, se può ancora bastare.** A 150° gli vai incontro di lato, e lui ti passa oltre prima che la
   testa si apra. Da solo funziona se la testa si apre tardi: al 70% della distanza batte il 90° contro SET-65 e Mk 48, al 60%
   dell'IA no (l'USET-80 da 4 km prende sempre). L'RTE non lo sai, per questo a 4 km conviene il 90°. A 2 km servono
   tutti e due: col SET-65 l'esca ferma e i 150° fanno 8 colpi su 100 in media, senza esca 67; con l'USET-80 41, ma da poppa
   prende sempre. Il Mk 48 non perdona. A 1 km prende quasi sempre: una testa passiva si fa sedurre dall'esca 9 volte su 10, ma poi le gira
   attorno e ti riprende, e una mista una volta su tre.
   **Viene solo dalla sonda:** in partita, da 150 m, la nave non ha tirato in questa fascia (§3). Da più lontano, fra 4,45 e 7,23 km,
   reagendo all'allarme i 150° hanno preso 4 colpi su 9 e i 150° con l'esca 3 su 9. Sotto 1 km, da 150 m, in partita non c'è stato
   nessun colpo su 7, nemmeno senza manovra, e non è spiegato: non contarci.
5. **Non fermarti dietro l'esca.** «Esca e via piano» è la peggior manovra della tabella, quasi come non farne nessuna: a 4
   nodi resti accanto al punto in cui il siluro gira, e lui ti ritrova. Vale anche a 8 km. L'esca compra tempo solo se quel
   tempo lo usi a tutta forza.
6. **La semovente è contro la testa solo attiva.** Una MG-74 (sull'Alfa; la MOSS sul 688 solo nel modo moderno) abbassa i colpi
   di una testa attiva da 100 a circa 20 su 100 se scappi lasciandola sulla tua rotta, salvo troppo vicino: sotto i 2 km contro l'USET-80, sotto i 4 contro il Mk 48.
   Contro una passiva, con la sua firma più debole dell'esca ferma, non cambia niente. Contro la mista dell'IA quasi
   niente: in ricerca la testa ascolta, e a tutta forza suoni più forte della semovente a 10 nodi.
7. **Lo strato da solo non salva.** Andare sotto dimezza la portata della testa quando siluro e battello stanno ai due
   lati, ma nella prova cambia l'esito di rado: il SET-65 attivo da 4 km e, contro la mista, un rilevamento su tre del SET-65
   fino a 2 km e dell'USET-80 a 4 km.
   Aiuta se stai già scappando, non al posto della fuga.
8. **Se sei già su e il siluro viene da lontano, resta sopra il soffitto.** Di serie la testa ignora tutto ciò che sta sopra i 15 m. Da 30 m, salire a
   10 m e correre ha evitato tutti i colpi in tutti i casi della sonda, anche contro la mista. **In partita sotto i 3,2 km non
   regge:** 3 colpi su 11 reagendo al lancio, 7 su 11 all'allarme. Da 5,96 km in su nessun colpo su 10, in tutti e due i casi.
   Che cosa succede restando a 30 m non è misurato. Il prezzo è che sei a un passo
   dalla superficie: radar, vista e cannoni (T9). L'IA non abbassa il soffitto contro un sottomarino; un giocatore sì. Vale
   per un siluro che corre a 30 m: se ha tirato quando stavi già sopra i 15 m, corre a 15, e la spoletta può scattare lo stesso,
   perché il raggio si misura dallo scafo (D1 §2.7). La prova non lo guarda.
9. **In acque basse, posati.** Sotto 2 m/s e a meno di 10 m dal fondo la portata attiva scende a un terzo, e 7 volte su 10 la
   testa ti scarta come fondo. L'IA lo fa su fondali fra 50 e 150 m. Una testa passiva invece no, e il rumore di prima non
   si cancella: posati prima di esserne sentito, e a macchine ferme. Sul fango lo scafo si attacca: in 10 minuti la presa è
   piena, e a ripartire da fermo resti trattenuto fino a 3 minuti (B2).
10. **Non vincerai la virata.** Il 688 gira con un raggio di 276 m. I siluri in alta velocità stanno sotto i 205 m: SET-65 197,
    USET-80 189, Mk 48 203. Solo l'Alfa, a 122 m, gira dentro di loro. Si vince col cono e con la verifica di 4 s, non con la
    centrifuga: esci dai ±40-50° e il siluro riparte a cercare dopo 6 s.
11. **Vicino al lanciatore il siluro si spegne, se ha l'anti-circolare.** Puntato entro ±45° sul suo lanciatore a meno di
    3 km, si spegne da solo. Vale anche per il tuo: se dopo l'attivazione torna verso di te, si spegne (T6). Gli
    aviolanciati non ce l'hanno.
12. **Dopo il tuo lancio sei già tu il bersaglio.** Ogni sottomarino dell'altra parte entro 20 km sente il lancio una volta
    su due, prende il tuo rilevamento e, con le armi libere, tira subito lungo quello. Lancia e accosta (T6).
13. **Il Crazy Ivan da dietro.** Un sovietico in moto ogni 30-120 minuti, e 5-15 minuti dopo un'accostata di oltre 30°,
    vira di 120-180°, scende a 4 nodi e ascolta per 2-5 minuti. Se gli stai in coda alla sua stessa quota è la strada per la
    collisione: stai ad almeno 60 m di quota da lui, perché a volte cambia quota di 20-40 m. Quando il rilevamento comincia
    a correre, scendi a 3 nodi e non accostare: nel cono eri nascosto, fuori conta solo quanto sei silenzioso. Dopo ogni
    sua accostata vera, il prossimo è fra 5 e 15 minuti.

## 2. Il perché

- La testa, il cono, la portata, la verifica di 4 s e i 6 s prima di riprendere: D1 §2.5-2.6. L'anti-circolare: D1 §2.8.
- L'esca ferma, la semovente e il disturbo della testa attiva: D2 §2.2-2.4.
- Il transitorio di lancio e il tiro di risposta: E6 §2.12. Il Crazy Ivan: E3 §2.7 e E6 §2.15.
- La velocità, la virata e il diametro tattico (5 lunghezze il 688, 3 l'Alfa): B1. L'emersione d'emergenza: B2 e B3.
- **L'autonomia fa la zona A.** Un USET-80 corre 18 km a 45 nodi, cioè 778 s: in quel tempo il 688, accelerando da 5 a 32
  nodi, fa circa 12 km. Il siluro prende soltanto se parte a meno di 6 km. Con la fuga di un'Alfa dal Mk 48 il limite sale a
  circa 10 km (T7).
- **La testa fa le zone B e C.** A 150° fai in tempo a trovarti di fianco o dietro di lui prima che si apra: fuori dal cono. Se
  si apre prima, ci finisci dentro di fronte, e a 90° invece ne stai uscendo. L'IA la apre al 60% della distanza, o dopo
  2000 m se ha il solo rilevamento (D1 §2.3); il comandante IA del vetrino al 70%. Un siluro guidato via filo ti trova
  meglio. A 1 km non c'è tempo per niente.
- **Perché l'esca ferma non basta.** Dopo la seduzione il siluro la perde dal cono e gira in cerchio: se sei ancora a
  portata, sei tu la sorgente più vicina che resta. Contro una testa attiva l'esca ferma si scarta 7 volte su 10.
- **L'IA fa lo stesso** (E6 §2.12): poppa al siluro, tutta forza, sotto il suo pavimento se può, fino a 4 esche ferme e 2
  semoventi sui nucleari sovietici, e sul fondo in acque basse. Il comandante IA del vetrino, dal 04/11/2026, tenta anche
  l'emersione d'emergenza: dopo un allagamento, dopo 5 minuti di incendio o fumo, o se è pesante sul fondo (E6 §2.17).
- **Nel classico** il siluro non ha testa né impostazioni: insegue il contatto del lanciatore, e la corsa è un timer dello
  script. L'esca sposta il bersaglio delle unità che ti cercano (D1 §4 e D2 §4). Le zone del 1989 sono quelle del manuale:
  questa prova non le misura nel classico.

## 3. I numeri

**Come è fatta la prova.** Il siluro parte puntato sul battello alla distanza della colonna, il battello lo sente subito
e comincia la manovra: la distanza è insieme quella di lancio e quella a cui lo senti. Rotta di lancio esatta, senza anticipo; testa aperta al 70% della distanza, almeno a 500 m (al 60% nell'ultima serie);
quota di ricerca quella del battello; niente filo. Il battello accelera a 0,257 nodi al secondo (0,33 l'Alfa, STIMA), con
il raggio del diametro tattico, e cambia quota a 2 m/s (STIMA). Il rumore è la firma alla velocità e alla quota di
ogni istante. Tre rilevamenti: 000, 090 e 180. Il modello è quasi deterministico: a parte le esche, un caso finisce
quasi sempre tutto da una parte, quindi il dato utile è «0» o «100», non il decimale.

Le prime tre tabelle provano una testa solo passiva e una solo attiva, aperte al 70%: l'IA non tira così, un giocatore sì. Ogni
casella: **colpi su 100, testa passiva / attiva**, il peggiore dei tre rilevamenti.

**688 contro SET-65** (40 nodi, 16 km, testa passiva 1500 m, attiva 800 m):

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 |
| A: poppa al siluro, tutta forza | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 | 0 / 0 |
| B: 90° dalla sua rotta | 100 / 100 | 100 / 100 | 100 / 0 | 0 / 0 | 0 / 0 |
| C: 150° dalla sua rotta | 100 / 100 | 100 / 0 | 0 / 0 | 0 / 0 | 0 / 0 |
| A con esca ferma | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 | 0 / 0 |
| C con esca ferma | 100 / 100 | 100 / 0 | 91 / 0 | 0 / 0 | 0 / 0 |
| Esca e via a 4 nodi | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 |
| A, esca, sotto lo strato | 100 / 100 | 100 / 100 | 100 / 0 | 0 / 0 | 0 / 0 |
| A con esca semovente | 100 / 23 | 100 / 26 | 100 / 19 | 0 / 0 | 0 / 0 |
| C con esca semovente | 100 / 100 | 92 / 74 | 88 / 0 | 0 / 0 | 0 / 0 |
| A, da 30 m su a 10 m | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 |

**688 contro USET-80** (45 nodi, 18 km, testa passiva 2500 m, attiva 1200 m):

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 |
| A: poppa al siluro, tutta forza | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 | 0 / 0 |
| B: 90° dalla sua rotta | 100 / 100 | 100 / 100 | 100 / 0 | 100 / 0 | 0 / 0 |
| C: 150° dalla sua rotta | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 | 0 / 0 |
| A con esca ferma | 100 / 100 | 100 / 100 | 100 / 73 | 0 / 0 | 0 / 0 |
| C con esca ferma | 100 / 100 | 100 / 100 | 92 / 100 | 0 / 0 | 0 / 0 |
| Esca e via a 4 nodi | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 |
| A, esca, sotto lo strato | 100 / 70 | 100 / 100 | 100 / 71 | 0 / 0 | 0 / 0 |
| A con esca semovente | 100 / 100 | 100 / 23 | 100 / 22 | 0 / 0 | 0 / 0 |
| C con esca semovente | 100 / 100 | 100 / 100 | 92 / 100 | 0 / 0 | 0 / 0 |
| A, da 30 m su a 10 m | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 |

**Alfa contro Mk 48** (55 nodi, 38 km, testa passiva 4000 m, attiva 1500 m; l'Alfa a 41 nodi):

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 |
| A: poppa al siluro, tutta forza | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 100 | 0 / 0 |
| B: 90° dalla sua rotta | 100 / 100 | 100 / 100 | 100 / 100 | 100 / 0 | 100 / 0 |
| C: 150° dalla sua rotta | 100 / 100 | 100 / 100 | 0 / 0 | 0 / 0 | 0 / 0 |
| A con esca ferma | 100 / 100 | 100 / 100 | 100 / 68 | 100 / 100 | 0 / 0 |
| C con esca ferma | 100 / 100 | 100 / 100 | 92 / 0 | 0 / 0 | 0 / 0 |
| Esca e via a 4 nodi | 100 / 100 | 85 / 100 | 100 / 100 | 100 / 100 | 100 / 100 |
| A, esca, sotto lo strato | 100 / 93 | 100 / 100 | 100 / 74 | 100 / 100 | 0 / 0 |
| A con esca semovente | 100 / 100 | 100 / 100 | 100 / 20 | 100 / 23 | 0 / 0 |
| C con esca semovente | 100 / 100 | 100 / 100 | 93 / 0 | 90 / 0 | 91 / 0 |
| A, da 30 m su a 10 m | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 | 0 / 0 |

**Il tiro dell'IA** (testa mista, RTE al 60%; `kit_t8_misto.cpp`, 30 semi, uscita in `kit_t8_misto.txt`). È la testa che
l'IA imposta davvero (D1 §2.3): cerca in passivo, verifica e insegue in attivo. Ogni casella: colpi su 100 nel peggiore dei
tre rilevamenti, e fra parentesi la media dei tre quando è diversa.

*688 contro SET-65:*

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 | 100 | 100 | 100 | 100 (67) |
| A: poppa al siluro, tutta forza | 100 | 100 | 100 (67) | 0 | 0 |
| B: 90° dalla sua rotta | 100 | 100 | 0 | 0 | 0 |
| C: 150° dalla sua rotta | 100 | 100 (67) | 0 | 0 | 0 |
| A con esca ferma | 100 (71) | 100 (74) | 100 (67) | 0 | 0 |
| B con esca ferma | 100 (76) | 100 (33) | 0 | 0 | 0 |
| C con esca ferma | 100 (48) | 23 (8) | 0 | 0 | 0 |
| Esca e via a 4 nodi | 100 | 100 (94) | 100 | 100 | 0 |
| A, sotto lo strato | 100 (67) | 100 (67) | 100 (67) | 0 | 0 |
| A, esca, sotto lo strato | 100 (77) | 83 (56) | 100 (67) | 0 | 0 |
| A con esca semovente | 100 | 100 | 100 (67) | 0 | 0 |
| C con esca semovente | 100 | 100 (68) | 0 | 0 | 0 |
| A, da 30 m su a 10 m | 0 | 0 | 0 | 0 | 0 |

*688 contro USET-80:*

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 | 100 | 100 | 100 | 100 (67) |
| A: poppa al siluro, tutta forza | 100 | 100 | 100 | 0 | 0 |
| B: 90° dalla sua rotta | 100 | 100 | 0 | 0 | 0 |
| C: 150° dalla sua rotta | 100 | 100 | 100 | 0 | 0 |
| A con esca ferma | 100 (99) | 100 (77) | 100 | 0 | 0 |
| B con esca ferma | 100 | 100 (41) | 0 | 0 | 0 |
| C con esca ferma | 100 | 100 (41) | 100 | 0 | 0 |
| Esca e via a 4 nodi | 100 | 100 (92) | 100 (72) | 100 | 100 |
| A, sotto lo strato | 100 | 100 | 100 (67) | 0 | 0 |
| A, esca, sotto lo strato | 100 (77) | 100 (68) | 100 (67) | 0 | 0 |
| A con esca semovente | 100 | 100 | 100 (80) | 0 | 0 |
| C con esca semovente | 100 | 100 | 100 | 0 | 0 |
| A, da 30 m su a 10 m | 0 | 0 | 0 | 0 | 0 |

*Alfa contro Mk 48:*

| Manovra | 1 km | 2 km | 4 km | 8 km | 12 km |
|---|---|---|---|---|---|
| Nessuna manovra, 5 nodi | 100 | 100 | 100 | 100 | 100 |
| A: poppa al siluro, tutta forza | 100 | 100 | 100 | 100 | 0 |
| B: 90° dalla sua rotta | 100 | 100 | 100 (33) | 0 | 0 |
| C: 150° dalla sua rotta | 100 | 100 | 100 | 100 (33) | 0 |
| A con esca ferma | 100 | 100 | 100 (88) | 100 | 0 |
| B con esca ferma | 100 | 100 | 100 (33) | 0 | 0 |
| C con esca ferma | 100 | 100 | 100 | 100 (33) | 0 |
| Esca e via a 4 nodi | 100 (98) | 80 (75) | 100 | 100 | 100 |
| A, sotto lo strato | 100 | 100 | 100 | 100 | 0 |
| A, esca, sotto lo strato | 100 (86) | 100 (99) | 100 (83) | 100 | 0 |
| A con esca semovente | 100 | 100 | 100 | 100 (86) | 0 |
| C con esca semovente | 100 | 100 | 100 (67) | 0 | 0 |
| A, da 30 m su a 10 m | 0 | 0 | 0 | 0 | 0 |

**Quanto conta l'RTE.** La stessa testa mista aperta al 70% (`kit_t8_misto_70.txt`) lascia a 0 la rotta a 150° contro il SET-65
da 2 km e contro il Mk 48 da 4 km, e a 33 di media contro l'USET-80 da 4 km (prende solo da poppa). Al 60% quelle tre caselle
sono 67, 100 e 100 di media. La rotta a 90° invece non cambia: 0 da 4 km contro SET-65 e USET-80 in tutti e due i casi.

**La prova in partita** (`kit_t8_partita.cpp`, `kit_t8_serie.sh`, `kit_t8_analisi.py`, 04/11/2026). Una Krivak II con lo script
delle missioni dell'editor contro il 688 con la dinamica vera e gli ordini dalle stazioni: Fær Øer a febbraio, guerra, 5 e 20
nodi, 3 semi, 2000 s per partita, 432 partite. La nave tira quando lo decide lei, dopo 780 s sul contatto, quindi la distanza non
si sceglie: si conta quella misurata al lancio. In 210 partite non ha tirato. Le manovre partono al lancio («subito») o
all'allarme vero del battello. Restano 18 lanci da 150 m, gli stessi per ogni manovra, e 21 da 30 m. È il solo SET-65, conta solo il primo
siluro, e tre semi non fanno una statistica. «Nessuna manovra» qui è tenere rotta e velocità, anche a 20 nodi.
Ogni casella: colpi su lanci, subito / all'allarme.

| Manovra, da 150 m | 0,26-0,84 km | 1,25 km | 4,45-7,23 km | 8,61 km |
|---|---|---|---|---|
| Nessuna manovra | 0 su 7 / 0 su 7 | 1 su 1 / 1 su 1 | 7 su 9 / 7 su 9 | 0 su 1 / 0 su 1 |
| A: poppa al siluro, tutta forza | 0 su 7 / 0 su 7 | 1 su 1 / 1 su 1 | 0 su 9 / 1 su 9 | 0 su 1 / 0 su 1 |
| B: 90° dalla sua rotta | 0 su 7 / 0 su 7 | 1 su 1 / 1 su 1 | 0 su 9 / 2 su 9 | 0 su 1 / 0 su 1 |
| C: 150° dalla sua rotta | 0 su 7 / 0 su 7 | 1 su 1 / 1 su 1 | 0 su 9 / 4 su 9 | 0 su 1 / 0 su 1 |
| C con esca ferma | 0 su 7 / 0 su 7 | 1 su 1 / 1 su 1 | 0 su 9 / 3 su 9 | 0 su 1 / 0 su 1 |

| Manovra, da 30 m | sotto 1 km | 1-2 km | 2-4 km | 4-8 km | oltre 8 km |
|---|---|---|---|---|---|
| A, su a 10 m | 3 su 6 / 3 su 6 | 0 su 2 / 2 su 2 | 0 su 3 / 2 su 3 | 0 su 9 / 0 su 9 | 0 su 1 / 0 su 1 |

Dove la partita non torna con le tabelle della sonda (nessuna è stata adattata):
- **L'allarme non suona al lancio.** La sonda suppone di sentire il siluro appena parte, e in partita questa ipotesi non è
  confermata. Da sotto 1 km l'allarme arriva 26-30 s dopo il lancio, col siluro a circa 280 m; da 4-8 km dopo 85-482 s (mediana
  300 s), col siluro a circa 3 km. In 68 tiri su 222 non è suonato: nessuno di questi tiri ha colpito.
- **Reagire tardi costa, e di più con le accostate.** Fra 4,45 e 7,23 km, subito, ogni manovra lascia a 0 su 9. All'allarme la fuga
  fa 1, il 90° 2, il 150° 4, il 150° con l'esca 3. Le tabelle danno 0 a 90° e a 150° da 4 km in su.
- **Sotto 1 km, da 150 m, nessun colpo**, nemmeno senza manovra (0 su 7, distanza minima mediana 77 m). Le tabelle, che partono da 1 km, lì danno 100.
  Il perché non è stato cercato.
- **Il soffitto non basta da vicino** (regola 8). Le tabelle danno 0 fino a 1 km; in partita 3 colpi su 11 sotto i 3,2 km
  reagendo subito, 7 su 11 all'allarme. Da 5,96 km in su nessun colpo. Manca il confronto con chi resta a 30 m.
- **Le zone B e C non sono provate da 150 m.** La nave non ha tirato fra 1,25 e 4,45 km, quindi il 90° attorno ai 4 km e
  l'esca a 2 km (regole 3 e 4) restano della sonda. «Da 4 km la fuga non salva più» (regola 3) non si può dire: da 4,45 e 4,63 km la
  fuga ha evitato i due lanci, ma lo ha fatto anche la rotta tenuta da uno dei due.

Torna invece la regola 2: da 7 km e oltre la fuga non ha mai preso colpi, reagendo subito. E torna il rumore: a 5 nodi la nave ha
tirato 7 volte su 8 da sotto 1 km, a 20 nodi 8 volte su 10 fra 4,45 e 7,23 km.

**Che cosa non misura.**
- Una partita vera, nelle tabelle: il loro battello ha un'accelerazione costante, il raggio del diametro tattico e 2 m/s di
  quota (STIME); non c'è dinamica, assetto né fondale. Il confronto è solo quello qui sopra.
- Nelle tabelle, un siluro sentito tardi. Lì la distanza a cui lo senti è quella di lancio: uno tirato da lontano e sentito a
  4 km ha meno corsa, ma la testa è già aperta.
- La tabella prende il peggior rilevamento. Da prua la fuga a 2 km, contro il SET-65, riesce: dipende dal punto in cui si
  apre la testa, ma non è una regola su cui contare. Il manuale del 1989 ha ragione nel dire che da prua hai meno tempo.
- L'esca parte subito e la semovente dopo 5 s, sulla rotta che lasci. Nel gioco la semovente esce dal tubo se è carica,
  e quei tempi li decidi tu. Sul 688 del 1989 la semovente non c'è: le sue righe valgono per il modo moderno.
- La soluzione sbagliata, che in partita è la regola, qui non c'è: il siluro parte sempre puntato giusto, e sul battello di
  allora, mentre l'IA con la sua TMA mira al punto d'incontro. Il siluro col filo e un operatore che rifiuta le esche (D2
  §2.5, D1 §2.10) non c'è nemmeno, ed è più difficile: un esperto scarta un'eco ferma oltre 10° dalla sua stima.
- Le acque basse e il fondo non ci sono: la regola 9 viene dal modello di D1 §2.5 e 2.6, non da questa prova.

## 4. Una scena per provarlo

- **Libreria S12 «Il gruppo cacciatore»** (Alfa, guerra): il lancio ti scopre, e la finestra per uscirne è breve.
- **Una missione rapida contro una Krivak II** (capitolo 11 del volume 1): la sua arma è il SET-65 fino a 8 km. Fatti tirare
  da circa 4 km, e rifai lo stesso seme prima con la fuga e poi con l'accostata a 90°; la carta, con la verità del replay,
  mostra dove passa il siluro. La distanza la decide lei: nella serie del §3, da 150 m, ha tirato oltre i 4 km solo col battello
  a 20 nodi, salvo una volta.
- **Il Crazy Ivan:** libreria S19 «Pietra santa» (T12), in coda a un SSBN che esce: guarda quando il rilevamento comincia a
  correre, e confronta la tua quota con la sua.
