# T1. Principi

*Autore: kit. Revisione del 03/11/2026. Stato: bozza. I numeri sono misurati col motore
(`tools/re/prove/suite_acustica/sonda_tattiche/kit_t1_t7.cpp`, uscita in `kit_t1_t7.txt`): le Fær Øer a febbraio,
battelli a 150 m, vento 7 m/s, la sfera che guarda la sorgente e la schiera filata che la prende al traverso. Fra
parentesi, la distanza più lontana a cui si sente ancora, nelle zone di convergenza.*

Una caccia sottomarina è una gara a chi sente l'altro per primo. Chi sente prima sceglie: avvicinarsi, aspettare,
andarsene. Tutto il resto, dalla quota alla velocità, serve a vincere quella gara.

## 1. Le regole pratiche

1. **Il ciclo.** Ogni scontro passa per quattro tappe: scoprire, classificare, localizzare, attaccare. Saltarne una
   costa caro. Un siluro lanciato su un contatto non classificato può finire su un peschereccio. Un lancio senza una
   soluzione TMA buona va a vuoto e rivela dove sei.
2. **La velocità è rumore.** Fino a 10 nodi il 688 è quasi muto. Da 12 nodi in su si fa sentire in fretta: un Victor
   III che ascolta a 5 nodi lo sente a 1,5 km a 5 nodi, a 6 km a 12, a 14 km a 15, a 19 a 20. A 25 nodi, il rumore del
   688 arriva oltre i 60 km.
3. **La velocità ti rende sordo.** La sfera del 688 sente un Victor III a 5 nodi fino a circa 6 km, finché il 688 sta
   sotto i 10 nodi. A 15 nodi sente a 4 km, a 20 a meno di 2. La schiera filata regge fino a 15-17 nodi (16-17 km), poi è
   inutile (capitolo T3).
4. **Sprint and drift.** Per coprire strada senza restare sordi si alterna: un tratto veloce, poi un tratto lento ad
   ascoltare. Lo sprint va fatto dove nessuno ascolta, o dall'altra parte dello strato (capitolo T2).
5. **La schiera e il cono sordo.** La schiera filata sente più lontano della sfera, ma è sorda lungo il suo asse: dritto
   a prua e dritto a poppa. Per tenere un contatto con la schiera, mettilo al traverso. Per vedere che cosa ti segue,
   accosta: chi ti pedina nel cono sordo di poppa salta fuori sul fianco.
6. **Esposizione.** Periscopio, antenna, radar e ping ti scoprono più del rumore. Ogni minuto col periscopio alzato è un
   minuto in cui un radar o una vedetta possono vederti (volume 2, C9 §2.6-2.8). Alza poco, guarda in fretta, abbassa.
7. **Classificare prima di avvicinarsi.** Le righe del LOFAR e il DEMON dicono la classe prima che la distanza la
   renda pericolosa (capitolo T4). Se non sai che cos'è, non avvicinarti come se fosse un bersaglio.

## 2. Il perché

- La firma di ogni battello cresce con la velocità e salta con la cavitazione: volume 2, capitoli C4 e B1 (la soglia di
  cavitazione del 688 a quota zero è 5 nodi, e sale con la quota).
- Il rumore proprio del ricevitore, che cresce con la velocità e tappa la sfera e la schiera: l'equazione del sonar,
  capitolo C1.
- La schiera filata, il cono sordo e lo specchio: capitolo C6.
- L'esposizione del periscopio, dell'antenna e del radar: capitoli C9 e C10.
- Il ciclo nel gioco: la scoperta e la classificazione (C4, C5), la TMA (C13), le armi (D1-D5).

## 3. I numeri

**Quanto lontano ti sentono** (km): il 688 alle varie velocità, ascoltato da un Victor III e da un Alfa che vanno a 5
nodi.

| Nodi del 688 | 3 | 5 | 8 | 10 | 12 | 15 | 20 | 25 |
|---|---|---|---|---|---|---|---|---|
| ti sente il Victor III | 1,2 | 1,5 | 2,2 | 3,8 | 6,0 (11) | 14,2 (16) | 18,8 (39) | 68,2 (100) |
| ti sente l'Alfa | 0,8 | 1,0 | 1,5 | 2,8 | 4,0 | 6,8 (13) | 17,5 (26) | 65,0 (100) |

**Quanto lontano senti** (km) un Victor III a 5 nodi, alle varie velocità del 688.

| Nodi del 688 | 3 | 5 | 8 | 10 | 12 | 15 | 20 | 25 |
|---|---|---|---|---|---|---|---|---|
| sfera BQQ-5 | 6,5 (12) | 6,5 (12) | 6,2 (12) | 6,2 (12) | 5,5 | 3,8 | 1,8 | 0,8 |
| schiera TB-16 | 17,2 (26) | 17,2 (26) | 17,2 (26) | 17,2 (26) | 17,0 (24) | 16,5 (24) | 0,2 | 0,2 |

Le portate cambiano molto con la quota e con il mese (capitolo T2). A 50 m, dentro lo strato, la stessa sfera sente il
Victor a 18 km e la schiera a 48 (capitolo T3); qui, a 150 m, a 6,5 e a 17.

Che cosa dicono le due tabelle insieme:
- **Fino a 10 nodi vinci tu.** Il 688 sente il Victor III a 6 km con la sfera e a 17 con la schiera; il Victor sente
  il 688 sotto i 4 km.
- **A 15 nodi è pari.** Il Victor ti sente a 14 km, e la tua sfera lo sente a 4: solo la schiera ti tiene avanti.
- **A 20 nodi perdi.** Ti sentono a 19 km (fino a 39 nelle zone di convergenza), e tu sei quasi sordo.

## 4. Una scena per provarlo

Una missione rapida col 688 contro un Victor III, nelle Fær Øer a febbraio (capitolo 11 del volume 1):
1. a 5 nodi e 150 m, con la schiera filata, guarda a che distanza compare il Victor;
2. rifallo con lo stesso seme a 18 nodi, e guarda chi scopre chi.

Nella sandbox (capitolo 12) la stessa partita si guarda dal vivo con la verità accesa: le linee di rilevamento dicono
chi ha sentito chi, e quando.
