# T2. L'ambiente come arma

*Autore: disassemblatore. Revisione del 31/10/2026. Stato: bozza (curatore: kit). I numeri sono misurati col motore
(`tools/re/prove/suite_acustica/sonda_tattiche/t2*.cpp`): la sfera del BQQ-5 di un 688 a 5 nodi contro un Victor III,
col tracciamento dei raggi (livello B), il profilo del suono del teatro e del mese, il vento a 7 m/s salvo dove è detto.*

Il mare decide chi sente chi prima delle macchine. Lo stesso Victor III a 5 nodi si sente a 18 km o a 5 km secondo la
quota e il mese. Chi conosce il mare sceglie dove stare.

## 1. Le regole pratiche

1. **Prima di tutto, lo strato.** D'inverno nei teatri del Nord c'è uno strato rimescolato in superficie (fino a 225 m
   nelle Fær Øer a febbraio, 375 nel Barents, 400 nella Celtica). Stessa parte dello strato: ci si sente lontano. Parti
   diverse: ci si sente da vicino. Mettiti dall'altra parte dello strato rispetto a chi ti cerca.
2. **D'estate lo strato non c'è.** Ad agosto la superficie è calda e il suono si piega verso il basso: due battelli a
   50 m si sentono da vicino (2,5 km nelle Fær Øer, 1,5 km nella Celtica). In profondità si sente molto più lontano
   (7,5 e 21 km a 250 m).
3. **Il vento è il tuo rumore di copertura.** Il mare mosso sommerge il segnale: alla stessa quota e nello stesso mese
   la portata passa da 49 km col vento a 2 m/s a 18 km a 7 m/s, e sotto i 2 km a 15 m/s. Nella burrasca, vicino alla
   superficie, sei quasi invisibile al sonar passivo; anche gli altri lo sono a te.
4. **La velocità si paga cara.** Nelle Fær Øer a febbraio, a 250 m, il Victor III si sente a 6 km a 5 nodi, a 25 a 10, a
   57 a 15, a 99 a 20. Ogni 5 nodi in più, il raggio a cui ti sentono si moltiplica.
5. **La zona d'ombra e la convergenza.** In acqua profonda, oltre il raggio diretto, c'è un anello in cui non arriva
   niente; più in là il suono ritorna dal profondo. Nelle Fær Øer a febbraio, a 250 m e 15 nodi, il Victor si sente fino
   a 30 km, poi niente fino a 42, poi a pezzi fra 42 e 57 km. Nell'ombra puoi passare; nella zona di convergenza ti
   sentono anche da lontano.
6. **Il fondale basso cambia tutto.** Nel Barents (300 m) il fondo riflette: le portate sono più lunghe che in acqua
   profonda alla stessa quota (14,5 km a 250 m d'inverno, contro i 5,5 delle Fær Øer). Nel Baltico (60 m) d'inverno ci
   si sente a 6-9 km; d'estate, senza strato, a meno di 2 km.
7. **Il traffico lontano conta poco per la sfera.** Il rumore delle navi lontane sta sotto le frequenze della sfera: dal
   traffico scarso a quello intenso la portata cambia di meno di un chilometro. Per la schiera filata, che ascolta più in
   basso, conta di più (capitolo C3).

## 2. Il perché

- La propagazione, lo strato, l'ombra, la convergenza, il fondo e il tracciamento dei raggi: volume 2, capitolo C2.
- Il rumore del mare, del vento, della pioggia, del traffico e del ghiaccio: capitolo C3.
- Il meteo per teatro e mese: capitolo E7; il mare del giocatore e il vento nel punto: E7 §2.

## 3. I numeri

**La portata di scoperta del Victor III a 5 nodi**, quota della sorgente → quota del ricevitore, vento 7 m/s:

| Teatro | Mese | Strato (m) | 50 → 50 | 50 → 250 | 250 → 250 | 250 → 50 |
|---|---|---|---|---|---|---|
| Fær Øer | febbraio | 225 | 18 km | 5 km | 5,5 km | 5 km |
| Fær Øer | agosto | 0 | 2,5 km | 3,5 km | 7,5 km | 4 km |
| Barents | febbraio | 375 | 27 km | 10,5 km | 14,5 km | 5 km |
| Barents | agosto | 0 | 8,5 km | 5,5 km | 11,5 km | 6,5 km |
| Celtica | febbraio | 400 | 22 km | 5 km | 6 km | 5 km |
| Celtica | agosto | 0 | 1,5 km | 2,5 km | 21 km | 3,5 km |
| Gibilterra | febbraio | 125 | 13,5 km | 6,5 km | 22 km | 6 km |
| Gibilterra | agosto | 0 | 8 km | 9 km | 16 km | 6,5 km |
| Otranto | febbraio | 400 | 18 km | 6 km | 11 km | 5,5 km |
| Otranto | agosto | 0 | 13,5 km | 7 km | 17 km | 8 km |
| Curili | febbraio | 400 | 25,5 km | 14 km | 12 km | 15 km |
| Curili | agosto | 0 | 16,5 km | 2,5 km | 20 km | 3 km |

Fondali della prova: Fær Øer 2.500 m, Barents 300, Celtica 4.000, Gibilterra 900, Otranto 800, Curili 3.000.

**Il Baltico** (fondale 60 m, un Kilo a 4 nodi): a febbraio, con lo strato a 80 m, 8,75 km fra 20 e 20 m, 6,75 fra 20
e 40, 6,25 fra 40 e 40; ad agosto 0,75-1,5 km.

**Il vento** (Fær Øer, febbraio, 50 → 50, Victor III a 5 nodi): 49,5 km a 2 m/s, 18,25 a 7 m/s, 1,75 a 15 m/s. Il
traffico, da 0,2 a 0,9, sposta la portata di meno di un chilometro.

**La velocità** (Fær Øer, febbraio, 250 → 250): 5,75 km a 5 nodi, 25 a 10, 57 a 15, 99 a 20, oltre 117 a 25.

## 4. Come lo usa l'IA

Il sonarista dell'IA sente con lo stesso mare: lo strato, il vento e la velocità valgono per lui come per te. Una nave
dell'IA col VDS o con la schiera filata porta il pesce 30 m sotto lo strato (capitoli E6 e C6 §2.10): d'inverno, contro una
nave così, stare sotto lo strato non basta. Tieni d'occhio la sua velocità: oltre quella di traino recupera il pesce, e per
quel tempo è sorda sotto lo strato.

## 5. Per provarlo

- **Sandbox, Fær Øer a febbraio:** un Victor III a 5 nodi e il 688 a 18 km, tutti e due a 50 m. Poi il 688 scende a
  250 m: il contatto sparisce.
- **La stessa scena ad agosto:** a 50 m il Victor si perde a 2-3 km; a 250 m torna a 7 km.
- **Libreria S09 «Aport»** (la barriera Islanda-Fær Øer) e **I08 «Ombra lunga»:** lo strato e la velocità contro una
  barriera.
