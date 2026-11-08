# T3. Cercare

*Autore: disassemblatore per i numeri, misurati col motore (`tools/re/prove/suite_acustica/sonda_tattiche/t3.cpp`); il
kit per la cura e i diagrammi. Revisione del 03/11/2026. Stato: bozza (curatore: kit). Salvo dove è detto: un Victor III a
5 nodi e 50 m, il 688 a 50 m, Fær Øer a febbraio (lo strato a 225 m), vento a 7 m/s, tracciamento dei raggi.*

Si trova chi ascolta meglio di quanto viene ascoltato. Il 688 ha due orecchi, la sfera di prua e la schiera trainata, e
tutti e due perdono l'udito con la velocità. Cercare vuol dire andare piano dove si ascolta, e in fretta dove non si
perde niente.

## 1. Le regole pratiche

1. **Sotto i 10 nodi la sfera sente come da fermo.** Il rumore del flusso prende il sopravvento verso gli 8 nodi; fino ai
   10 la portata resta quella da fermo (18 km). A 12 nodi scende a 12,5 km, a 15 a 9,5, a 17 a 6,3, a 20 a meno di 2.
2. **La schiera sente il doppio e mezzo, ma non oltre i 17 nodi.** Filata, la TB-16 sente lo stesso Victor a 48,5 km fino
   a 10 nodi, 39,5 a 12, 34 a 15, 28 a 17. Oltre i 17 nodi è sorda: mezzo chilometro.
3. **Sprint and drift.** Alterna un tratto veloce, in cui sei sordo, a uno lento in cui ascolti tutto. Con la schiera,
   10 minuti a 15 nodi (2,5 miglia di strada, ma sordo oltre 34 km) e 10 minuti a 5 nodi in ascolto a 48 km: la media fa
   10 nodi, e il quadro si rinnova a ogni deriva. Le navi dell'IA col VDS fanno lo stesso: 5 minuti in deriva a 6 nodi,
   poi 10 di sprint (capitolo C6 §2.10).
4. **La sfera non vede dietro.** Il cono d'ombra poppiero (`baffles_gradi`, 35° per parte) toglie la sfera dalla poppa:
   lo stesso Victor, dietro di te, si sente a mezzo chilometro. A poppa ascolta la schiera, che sente anche lì, col fascio
   largo dell'endfire posteriore. Se non hai la schiera, «schiarisci il cono»: un'accostata di 60-90° ogni tanto.
5. **La schiera non vede davanti, e accecata in virata.** L'endfire anteriore, 25° per parte attorno alla prua, è cieco
   (−30 dB); in virata oltre 0,3 °/s la schiera è cieca, e per 2 minuti dopo la virata perde ancora fino a 10 dB. A 5
   nodi con 15° di barra resta in ascolto; a 10 nodi con la stessa barra si acceca.
6. **Lo specchio della schiera si risolve accostando.** La schiera dà due rilevamenti, il vero e lo specchio rispetto al
   suo asse. Accosta di 30° o più (`VIRATA_LATO_DEG`): il vero si sposta in modo coerente con la manovra, lo specchio no.
   Anche l'IA crede un lato a caso finché non accosta.
7. **Cerca dalla parte giusta dello strato.** D'inverno la stessa caccia vale 18 km sopra lo strato e 5 km attraverso
   (capitolo T2): scegli la quota dove sta, o starà, il bersaglio.
8. **Prima l'ESM, poi il periscopio.** L'ESM sente un radar più lontano di quanto quel radar veda te: la portata
   d'intercettazione è l'orizzonte radar per 1,1-1,3 (C10). Alza l'antenna ESM per primo, pochi secondi: se nessuno
   trasmette vicino, alza il periscopio. L'ESM ti dà il radar della Krivak a 30 km, prima che lei possa vedere il tuo
   albero.
9. **Guarda poco.** L'equipaggio avverte dopo 30 s di testa fuori, e la guardata lunga va nel registro (C9 §2.6). Il
   radar ti cerca un giro d'antenna alla volta: ogni giro ha la sua probabilità, e i giri si sommano. Un P-3C che cerca
   a 5 miglia ti vede col 47% a ogni giro del suo APS-115, e in un minuto quasi certamente; l'APS-137 dell'S-3B fa
   cinque giri al secondo. Una guardata di 10 secondi è una scommessa; una di un minuto, sotto un aereo, è persa.
10. **Lento, basso, col mare.** La testa si vede soprattutto per la piuma: a 3 nodi una Type 18 alzata si vede a 3,9 km,
    a 10 nodi a 7,9. Tieni fuori 1,5 m, non di più: la sezione al radar cresce col quadrato dell'altezza. Col mare
    forte sei quasi invisibile (a mare 5 la Type 2 si vede a 650 m), e di notte molto di più (135 m a occhio nudo).
    Contro il sole basso, la lente brilla.
11. **Il Type 2 per guardare, il Type 18 per lavorare.** Il periscopio d'attacco (Type 2) ha la testa piccola: 0,45 m²
    al radar e 2,2 km a vista a 3 nodi. Il Type 18, con le sue ottiche, è grande: 3,4 m² e 3,9 km. Per una guardata di
    sicurezza o di tiro, il Type 2.
12. **Alzare il periscopio cambia la caccia.** Il nemico risponde a quello che vede. Nella scena `PROVA_T4_ELICOTTERO`,
    con lo stesso seme: col battello a quota periscopica e il periscopio giù, il Ka-27 a cinque minuti e mezzo è in
    hover a 1,4 km col sonar calato; col periscopio alzato da 100 s, allo stesso istante punta dritto sul battello a 100
    m di quota, da un'altra direzione.
13. **Il tuo radar ti tradisce.** Acceso, il radar del 688 vede le navi entro 20 km ma non le teste, e l'ESM di un
    sottomarino nemico con l'antenna fuori lo sente fino a 20 km. In pace, per il punto nave sulla costa, va bene; in
    caccia no.

## 2. Il perché

- Il rumore proprio, il flusso, i baffles: volume 2, capitoli C1 §2.1 e C6 §2.4.
- La schiera, l'endfire, la virata, lo specchio: C6 §2.5 e §2.6.
- Lo strato e la propagazione: C2, e la tattica T2.
- Il periscopio, l'ESM e il radar: C9 (§2.6 l'esposizione, §2.7 la testa al radar, §2.8 la testa a vista) e C10 (§2.4
  il radar contro la testa, un giro alla volta; §2.9 il radar del giocatore; §2.10 i due esempi).

## 3. I numeri

**La portata contro la velocità propria** (il Victor III a 5 nodi, a ovest del 688):

| Velocità propria | Sfera a prua | Sfera al traverso | Sfera a poppa | Schiera al traverso | Schiera a poppa |
|---|---|---|---|---|---|
| 3 kn | 18,25 km | 18,25 km | 0,5 km | 48,5 km | 48,5 km |
| 5 kn | 18,25 km | 18,25 km | 0,5 km | 48,5 km | 48,5 km |
| 8 kn | 18,25 km | 18,25 km | 0,25 km | 48,5 km | 48,5 km |
| 10 kn | 18 km | 18 km | 0,25 km | 48,5 km | 48,5 km |
| 12 kn | 12,5 km | 12,5 km | 0,25 km | 39,5 km | 39,5 km |
| 15 kn | 9,5 km | 9,5 km | 0,25 km | 34,25 km | 34,25 km |
| 17 kn | 6,25 km | 6,25 km | 0,25 km | 27,75 km | 27,75 km |
| 18 kn | 2,5 km | 2,5 km | 0,25 km | 0,5 km (sorda) | 0,5 km |
| 20 kn | 1,75 km | 1,75 km | 0,25 km | 0,25 km | 0,25 km |

La schiera a prua non è in tabella: l'endfire anteriore è cieco a ogni velocità.

**Che cosa vuol dire per la ricerca.** La larghezza che spazzi è due volte la portata. A 5 nodi con la schiera spazzi 97
km di larghezza a 9,3 km all'ora di strada: 900 km² all'ora. A 15 nodi 68,5 km di larghezza a 27,8 km/h: 1.900 km²
all'ora. Spazzi di più, ma a 15 nodi anche il tuo rumore sale: il Victor III della tattica T2 si sente a 6 km a 5 nodi e a
57 km a 15, e un 688 non è molto diverso. Lo sprint and drift di §1.3 spazza circa 1.400 km² all'ora; per metà del tempo
sei silenzioso, e a ogni deriva il quadro si rinnova.

**La testa a vista** (C9 §2.8): la portata di riferimento, a cui una vedetta allertata ha metà del tasso massimo, con
1,5 m fuori, di giorno, a mare 2 (fra parentesi le vedette non allertate).

| Velocità | Type 2 | Type 18 | Type 2, mare 5 | Type 18, notte, occhio | Type 18, notte, intensificatore |
|---|---|---|---|---|---|
| 3 nodi | 2,2 km (1,5) | 3,9 km (2,7) | 650 m | 135 m | 770 m |
| 6 nodi | 2,8 km (1,9) | 5,1 km (3,6) | 820 m | 180 m | 1,0 km |
| 10 nodi | 4,1 km (2,9) | 7,9 km (5,6) | 1,2 km | 280 m | 1,6 km |

**La testa al radar** (C10): la R50, la distanza a cui un giro d'antenna vede un periscopio con probabilità 0,5.

| Radar | Giro | R50, mare 2 | R50, mare 4 |
|---|---|---|---|
| APS-115 (P-3C) | 6 s | 11,1 km | 3,7 km |
| APS-137 (S-3B) | 0,2 s | 18,5 km | 7,4 km |
| SPS-55 (navi NATO) | 3,75 s | 9,3 km | 3,7 km |

## 4. Come ti cerca l'IA

L'IA usa le stesse regole e gli stessi numeri: lo stesso rumore di flusso, gli stessi baffles, la schiera che si acceca
in virata. Un sottomarino dell'IA che pedina accosta per «schiarire il cono» (il Crazy Ivan, che non ripete subito;
capitolo E6); le navi col VDS fanno sprint and drift. Se la segui da dietro sei nel suo cono sordo finché non accosta.

## 5. Per provarlo

- **Sandbox, Fær Øer a febbraio:** il 688 con la schiera filata e un Victor III a 5 nodi a 40 km. A 5 nodi lo senti;
  sali a 18 nodi e lo perdi, torna a 5 e lo ritrovi.
- **Libreria S10 «Il cavo della fregata»:** mezz'ora nel cono sordo di una nave, e la scia.
- **Libreria S14 «Il pedinatore in attesa»:** cercare un SSBN che esce dal canale, con lo specchio della schiera da
  risolvere.
- **Sandbox col P-3C:** il 688 a quota periscopica e un P-3C in ricerca a 10 km, con la verità accesa. Alza il Type 18
  per un minuto a 6 nodi e guarda quando l'aereo ti scopre; rifallo col Type 2 per 10 secondi a 3 nodi, prima con
  l'ESM.
