# T5. La TMA in pratica

*Autore: disassemblatore. Revisione del 03/11/2026. Stato: bozza curata (curatore: kit). I numeri sono misurati col
risolutore del motore, `risolvi_tma` e `ManovreTma` (`tools/re/prove/suite_acustica/sonda_tattiche/t5.cpp` e
`t5_aob.cpp`; esiti in `t5.txt` e `t5_aob.txt`), non stimati. Salvo dove è detto: il 688 a 6 nodi, un bersaglio a 10 km
che taglia a 8 nodi, rilevamenti ogni 5 s con 1° d'errore, mediane su 20 semi. «Soluzione buona» vuol dire distanza
osservabile con sigma sotto il 15%, la soglia della dottrina NATO dell'IA.*

Il sonar passivo ti dà solo il rilevamento. La distanza, la rotta e la velocità del bersaglio le ricavi dalla serie dei
rilevamenti, e solo se ti muovi nel modo giusto. È la TMA: il MATE, il regolo di Ekelund, il pannello
tempo-rilevamento. Il gioco non te la risolve; l'IA la risolve da sola, con la stessa matematica.

## 1. Le regole pratiche

1. **Su una gamba sola la distanza non c'è.** Con rotta e velocità costanti, un bersaglio vicino e lento e uno lontano
   e veloce danno gli stessi rilevamenti. In un'ora di gamba diritta la soluzione non diventa mai buona. Il MATE ti
   mostra scarti verdi a 5 km come a 15: non è un errore dello strumento, è la geometria.
2. **Fai due gambe, e accosta di 60-90°.** Una prima gamba di 3-6 minuti, poi un'accostata che metta il bersaglio
   sull'altro mascone. Con 60-90° la soluzione è buona in 9-13 minuti dall'inizio. Con 30° ne servono 20, e con una
   gamba di 3 minuti e 30° di virata, in un'ora, è buona solo 6 volte su 20.
3. **Le gambe lunghe sono più giuste, quelle corte più rapide.** Con gambe di 3 minuti e 90° la soluzione è buona a 9
   minuti con il 13% d'errore. Con gambe di 10 minuti arriva a 14 minuti, ma con l'8%, e mai oltre il 20%.
4. **Cambiare velocità funziona, ma peggio.** Sulla stessa rotta, da 6 a 15 nodi la soluzione arriva a 16 minuti; da 6
   a 3 nodi a 27. L'accostata di 90° fa lo stesso in 12. E a 15 nodi la sfera sente circa la metà (capitolo T3).
5. **Il rateo ti dice la distanza, se sai la velocità.** La regola del 1934 in unità comode:

   ```
   rateo [°/min] ≈ 1,8 × velocità attraverso la linea di vista [nodi] / distanza [km]
   ```

   A 10 km un bersaglio che taglia a 8 nodi gira di 1,4° al minuto; a 3 km di 4,7°. Un rateo forte vuol dire vicino o
   veloce. **Un rateo nullo è un allarme**: il bersaglio ti viene incontro o si allontana dritto, e a rilevamento
   costante e distanza che cala ti finisce addosso.
6. **Ekelund dà la distanza da due ratei.** Misura il rateo sulla gamba prima e dopo l'accostata, sul pannello
   tempo-rilevamento (tasto 9):

   ```
   distanza [km] ≈ 1,8 × differenza della tua velocità attraverso la linea di vista [nodi] / differenza dei ratei [°/min]
   ```

   Prendi i ratei corti, a cavallo della virata: Ekelund ignora che il rilevamento accelera. È una partenza per il
   MATE, non la soluzione.
7. **Il MATE è buono quando gli scarti sono per lo più verdi su tutte e due le gambe, e senza tendenza.** Verde vuol dire
   sotto 1°: con un contatto debole l'errore del sensore basta a farne giallo qualcuno. Parti da Ekelund o dalla proposta
   del CPA del verniero (Invio). Una fila di scarti che sale o scende vuol dire soluzione sbagliata anche se è verde: la
   distanza, la rotta o la velocità vanno ancora mosse. Tieni aperto il MATE: una missione può contare le tue stime di rotta
   (C13 §2.3; la S20, T12).
8. **La sigma è ottimista.** Quando il risolutore dice «buona», l'errore vero mediano è del 10%, ma il peggiore dei 20
   casi è al 44%. Prima di lanciare, una terza gamba, il CPA del verniero o un ping confermano la distanza.
9. **L'AOB del periscopio vale solo se è preciso.** L'angolo sulla prua del bersaglio fissa la sua rotta. La tua
   osservazione ha un'incertezza di ±10° a 6× e di ±15° a 1,5× (volume 1, cap. 4; Z, X, poi Spazio). Sul risolutore, in
   mezz'ora di gamba sola:
   - a ±10° servono quattro occhiate per avere la distanza, con una sigma del 15-17%;
   - a ±20° non basta mai;
   - a ±5°, che il periscopio non dà, ne basterebbe una.

   La sigma è ottimista anche qui: con quattro occhiate a ±10° l'errore vero è arrivato al 31%. Nel MATE l'osservazione
   non dà la distanza: compare nella riga PERISCOPIO con lo scarto dalla soluzione, e ti dice se la rotta regge. Con
   l'accostata aiuta: due AOB a ±20° e la virata danno la soluzione a 9 minuti con l'8% d'errore, invece di 12 col 10%.
10. **Se il bersaglio manovra, la striscia degli scarti te lo dice.** Una sua accostata di 30° porta gli scarti dei
    rilevamenti nuovi a 2° in cinque minuti, di 60° a 4°, di 90° a quasi 6°. La soluzione vecchia sbaglia allora del
    12% dopo 5 minuti e del 21% dopo 10 (con 60°). Ricomincia: il MATE usa tutti i rilevamenti, e quelli di prima della
    sua manovra ormai ti tirano fuori strada. Cambiare bersaglio (G) e tornare al primo azzera la serie.
11. **Con la schiera risolvi prima lo specchio.** La schiera dà il rilevamento vero e il suo specchio rispetto all'asse.
    Su una gamba sola lo specchio è una soluzione coerente quanto la vera. La stessa accostata di 30° o più che serve
    alla TMA lo risolve: il vero si sposta in modo coerente con la manovra, lo specchio no (capitolo T3, regola 6). L'IA,
    con lo specchio irrisolto, rinuncia dopo 3 gambe.
12. **Lontano e con rilevamenti cattivi ci vuole più tempo.** A 3 km la soluzione è buona in 8 minuti col 6%
    d'errore, a 20 km in 13 col 17%, a 40 km in 17. Con 0,3° d'errore sui rilevamenti bastano 8 minuti; con 4° ne
    servono 20, e l'errore sale al 18%.

## 2. Il perché

- Il modello del bersaglio, l'inosservabilità di una gamba, la regola del 1934 ed Ekelund: volume 2, capitolo C13
  §2.2, §2.4 e §2.8.
- Il MATE, il regolo (con la figura in C13 §2.7), le linee e il pannello tempo-rilevamento: C13 §2.3, §2.5-2.7; nel
  volume 1, §5.4.
- L'errore dei rilevamenti, che cresce col segnale debole: C1 e C13 §2.1.
- La proposta dal CPA del verniero: C13 §2.3 e C5.
- Lo specchio della schiera: C6 §2.6.
- La TMA e le gambe dell'IA: C13 §2.9 ed E6 §2.5.

## 3. I numeri

**Due gambe: quanto tempo alla soluzione buona** (dall'inizio della prima gamba; 10 km, 1°):

| Prima gamba | Virata 30° | Virata 60° | Virata 90° |
|---|---|---|---|
| 3 min | 34 min, 25% (buona 6 volte su 20) | 11 min, 11% | 9 min, 13% |
| 6 min | 20 min, 11% | 13 min, 11% | 12 min, 10% |
| 10 min | 23 min, 5% | 17 min, 6% | 14 min, 8% |

In ogni casella il tempo mediano e l'errore mediano della distanza nel momento in cui la soluzione diventa buona.

**La distanza, l'errore dei rilevamenti, il bersaglio** (gamba di 6 minuti, virata di 90°):

| Caso | Tempo | Errore mediano | Il peggiore |
|---|---|---|---|
| 3 km | 8 min | 6% | 22% |
| 5 km | 10 min | 8% | 31% |
| 10 km | 12 min | 10% | 44% |
| 20 km | 13 min | 17% | 37% |
| 40 km | 17 min | 17% | 45% |
| rilevamenti a 0,3° | 8 min | 6% | 23% |
| rilevamenti a 2° | 14 min | 12% | 40% |
| rilevamenti a 4° | 20 min | 18% | 43% |
| bersaglio a 3 nodi | 10 min | 13% | 26% |
| bersaglio a 25 nodi | 14 min | 20% | 42% |
| bersaglio che viene incontro | 13 min | 7% | 20% |

**Rotta o velocità** (10 km; dopo 6 minuti a 6 nodi per 315):

| Manovra | Tempo | Errore mediano |
|---|---|---|
| virata di 90° | 12 min | 10% |
| da 6 a 15 nodi, stessa rotta | 16 min | 15% |
| da 6 a 3 nodi, stessa rotta | 27 min | 10% |

**L'AOB al periscopio su una gamba sola** (10 km; le occhiate dopo 5 minuti, ogni 2 minuti e mezzo; la soluzione a
30 minuti; la sigma del risolutore, non l'errore vero, su 3 semi):

| AOB | 1 occhiata | 2 | 4 | 8 |
|---|---|---|---|---|
| ±5° | sigma 15-19% | 8-13% | 5-10% | 3-4% |
| ±10° | non osservabile | non osservabile | 15-17% | 11-13% |
| ±20° | non osservabile | non osservabile | non osservabile | non osservabile |

**Il bersaglio che manovra** (la soluzione buona di prima, portata avanti; 10 km):

| Sua accostata | Scarto dei rilevamenti nuovi dopo 5 min | Errore dopo 5 min | Dopo 10 min |
|---|---|---|---|
| 30° | 2,0° | 7% | 12% |
| 60° | 4,0° | 12% | 21% |
| 90° | 5,8° | 17% | 32% |

## 4. Come ti trova l'IA

L'IA non sa dove sei finché non ha fatto le sue gambe. Dal passivo ha solo il tuo rilevamento, come tu il suo. Usa lo
stesso risolutore:
- un rilevamento ogni 20 s;
- una soluzione al minuto;
- la tua posizione sulla sua carta solo quando la distanza è osservabile.

Per renderla osservabile manovra secondo la dottrina della sua marina (`[tma.*]` di `interazioni.toml`; E6 §2.5):

| Dottrina | Gambe | Mascone | Velocità di TMA | Basta con sigma | Dopo la virata, con la schiera |
|---|---|---|---|---|---|
| NATO (usa, uk) | 3-6 min | 45° | 6 nodi | 15% | 2 min |
| sovietica | 4-8 min | 60° | 8 nodi | 20% | 3 min |
| Alfa | come la sovietica | | 12 nodi | | |
| Kilo e Foxtrot | come la sovietica | | 4 nodi | | |

Nella missione la gamba finisce appena la distanza è osservabile: la sigma della dottrina è più severa, e non arriva
quasi mai a decidere (E6 §2.5). Sotto i 4 km mette il bersaglio oltre il traverso (100°), così la gamba apre la
distanza. Le gambe le fa solo immerso, nelle missioni Enhanced e dell'editor, su un contatto ostile con la traccia
solida, quando ha le armi libere o lo ha agganciato; nelle missioni originali no. Rinuncia dopo 8 gambe senza soluzione,
dopo 3 con lo specchio irrisolto, o dopo 10 minuti senza rilevamenti.

**Quanto ci mette** (le sue gambe sul risolutore del motore; rilevamenti a 1°):

| Chi | A 5 km | A 10 km | A 20 km |
|---|---|---|---|
| un 688 (NATO) | 19 min, 11% | 20 min, 14% | 23 min, 9% |
| un Victor III | 21 min, 14% | 23 min, 16% | 25 min, 21% |
| un Alfa | 13 min, 12% | 15 min, 11% | 24 min, 9% |
| un Kilo | 25 min, 17% | 25 min, 18% | 28 min, 21% |

In tutti e venti i casi trova la soluzione, in una o due gambe.

**Che cosa ne ricavi:**
- **Riconosci la sua TMA.** Un sottomarino che accosta di 30-90° ogni 3-8 minuti, sempre tenendoti su un mascone, ti
  sta cercando la distanza. Fra 13 e 28 minuti, se non fai niente, ti ha.
- **Rovinagli la soluzione.** La sua TMA crede che tu vada dritto. Una tua accostata di 60° gli porta l'errore al 21%
  in dieci minuti, e lo scarto dei suoi rilevamenti glielo dice: deve ricominciare.
- **Il suo periscopio conta poco.** L'IA stima l'AOB con una sigma di 10° da vicino, che raddoppia al limite della
  portata e arriva a 60° di notte sulla sola sagoma (C9 §2.16; E6 §2.3). A ±20° non le dà mai la distanza, a ±10° gliene
  servono quattro, e un periscopio dell'IA guarda 10 s ogni 3-12 minuti: aiuta le gambe, non le sostituisce.
- **La posizione si perde.** Dopo tre soluzioni di fila non osservabili, cioè tre minuti, la posizione che la sua TMA
  ti aveva dato si toglie dal contatto, e il battello ricomincia le gambe (E6 §2.5). Gli resta il rilevamento.

## 5. Per provarlo

- **Una missione dell'editor, Fær Øer:** il 688 a 6 nodi e un cargo a 10 km che taglia a 8 nodi.
  1. Designalo (G) e apri il MATE (T): dopo 10 minuti di gamba diritta trovi scarti verdi a 5 km e a 15 km. È la regola 1.
  2. Accosta di 90° e aspetta altri 6 minuti: solo una distanza resta verde.
  3. Prova Ekelund (Q, poi Invio) e confronta.
- **La stessa missione, col cargo che accosta:** nelle Rotte dell'editor dai al cargo un secondo punto, 60° più in là,
  a mezz'ora di moto. Fatta la soluzione, aspetta l'accostata e guarda la striscia degli scarti diventare gialla.
- **In sandbox e in guerra, un Victor III a 7 km dal 688** (V sulla missione): non giochi, guardi; il 688 lo comanda l'IA.
  Nella vista dal vivo guarda le accostate del Victor quando lo sente. Sono le sue gambe di TMA: contale. Dopo la
  seconda, nel vetrino di Dio (G) dai al 688 un ordine «vai a» 60° più in là, e guarda se il Victor deve ricominciare.
  A 10 km, alle Fær Øer, il 688 è spesso nella sua zona d'ombra e la traccia solida non arriva: è il capitolo T2.
