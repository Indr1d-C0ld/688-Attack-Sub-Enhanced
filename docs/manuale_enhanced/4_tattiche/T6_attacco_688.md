# T6. L'attacco col 688

*Autore: kit (per la parte del runtime sull'IA: da rileggere col runtime). Revisione del 03/11/2026. Stato: bozza.
La letalità è misurata col motore (`strumenti/letalita`, 8 semi, bersagli a 150 m e 5 nodi; uscite in
`tools/re/prove/suite_acustica/sonda_tattiche/kit_t6_*.txt`). Le corse, le teste e i missili vengono da
`data/motore/siluri.toml` e `missili.toml` e dal volume 2 (D1, D3).*

Un attacco riuscito è la fine del ciclo del capitolo T1: si arriva al lancio con una soluzione buona, si lancia in
modo che la testa trovi il bersaglio prima che il bersaglio trovi il siluro, e ci si allontana senza perdere il filo.

## 1. Le regole pratiche

1. **Prima la soluzione.** La testa del Mk 48 sente in passivo a 4 km e in attivo a 1,5 km (l'ADCAP a 5 e 2 km), in un
   cono di ±45° (±50°). Se la soluzione TMA sbaglia di più, il siluro cerca nel posto sbagliato. Con una soluzione
   incerta, conviene la ricerca in serpentina, che copre di più.
2. **L'RTE.** La testa si apre all'RTE: mettilo 3-4 km prima del punto dove sarà il bersaglio. Troppo presto, la testa
   sente te o le esche; troppo tardi, il bersaglio è già passato.
3. **Soffitto e pavimento.** Contro un sottomarino, il soffitto sopra di lui e sotto le navi di superficie amiche e
   neutrali; il pavimento sotto di lui. Così la testa non si distrae con la superficie e con il fondo.
4. **Lenta o veloce.** A 40 nodi il Mk 48 corre 50 km ed è più silenzioso: per i lanci lunghi. A 55 nodi corre 38 km:
   per i bersagli veloci o vicini. L'ADCAP a 60 nodi corre 32 km.
5. **Il filo.** Finché il filo tiene, i rilevamenti della testa tornano a bordo e tu puoi correggere la rotta. Il filo
   si rompe sopra i 12 nodi o virando oltre 2° al secondo per più di 5 secondi. Dopo il lancio si manovra piano. Se il
   filo dice «sorgente ferma», è probabilmente un'esca.
6. **Un siluro a segno basta.** Ogni colpo diretto o sotto la chiglia rompe lo scafo resistente. Nella prova, un Mk 48
   sotto la chiglia o un Mk 46 a contatto affondano un Victor III o un Akula in circa due minuti, un Alfa in uno. Un Mk
   48 che scoppia a 10 m di fianco li affonda in 4-8 minuti. Non serve una salva per uccidere: serve per colpire.
7. **La salva.** Due siluri, con quote di ricerca o angoli diversi, coprono gli errori della soluzione e lo scarto
   dell'esca. Il secondo può partire dopo che il primo ha aperto la testa, guidato sul rilevamento che il primo manda.
8. **I missili.** Harpoon e TASM partono da 20-60 m a non più di 10 nodi. Il bersaglio deve essere oltre la distanza
   minima (5,6 km l'Harpoon, 9,3 il TASM) e dentro la portata (111 e 460 km). L'area del seeker (12-23 km per
   l'Harpoon, 15-35 per il TASM) va libera da neutrali e amici: il seeker aggancia il primo che vede. All'arrivo la
   difesa della nave può fermarlo, con la probabilità della sua classe: contro una nave ben difesa un missile solo
   spesso non basta. Il missile fermato scoppia in aria prima della nave, e al periscopio lo vedi.
9. **Il lancio ti rivela.** La capsula che esce dall'acqua, la fiammata e il fumo si vedono; il rumore del lancio si
   sente per cento secondi. Anche il lancio di un siluro si sente: entro 20 km un sottomarino nemico immerso ha, una
   volta su due, il rilevamento su di te. Allora evade, lancia le sue esche e, se ha le armi libere, ti risponde con un
   siluro su quel rilevamento (lo *snap shot*). Dopo un lancio cambia quota e vattene.
10. **La ritirata.** Dopo il lancio vira via piano, col filo; quando il siluro ha agganciato o il filo è rotto, scendi
    sotto lo strato e allontanati a velocità silenziosa (T1). Lascia acceso l'anti-circolare: se il tuo siluro gira,
    si spegne invece di tornare su di te. Il bersaglio reagisce quando sente il siluro: entro 3 km se ne ha il
    contatto, sempre entro 1,5 km, o quando il suo intercettatore sente la testa che pinga. Va a tutta forza con la
    poppa al siluro, scende sotto il pavimento della testa e lancia le esche (i nucleari sovietici anche l'MG-74
    semovente). Per questo il filo conta: la correzione arriva dopo la sua accostata. Una nave che sente il siluro
    entro 4 km scappa a tutta forza per 3-5 minuti.

## 2. Il perché

- Le teste, la corsa, la ricerca, il filo e le impostazioni: volume 2, D1; volume 1, §6.3.
- I missili, il piano di tiro, il seeker e la difesa delle navi: D3. Un missile fermato dalla difesa scoppia in aria
  circa 500 m prima della nave (dal 03/11/2026: `fine_missile` codice 2 nel registro, capitolo F1), e al periscopio si
  vede.
- I danni sul bersaglio sommerso (l'urto, lo scafo resistente, l'allagamento): B3.
- Le esche e come la testa le scarta: D2.
- La TMA e la soluzione: C13 e T5.

## 3. I numeri

**La letalità dei siluri sui sottomarini** (strumenti/letalita, 8 semi, bersagli a 150 m e 5 nodi):

| Colpo | Victor III | Akula | Alfa |
|---|---|---|---|
| Mk 48 sotto la chiglia (5 m) | 8 su 8 persi, in 114 s | 8 su 8, in 124 s | 8 su 8, in 57 s |
| Mk 48 al traverso, a 10 m dallo scafo | 8 su 8, in 402 s | 8 su 8, in 464 s | 8 su 8, in 258 s |
| Mk 46 a contatto | 8 su 8, in 114 s | 8 su 8, in 124 s | 8 su 8, in 57 s |

Il Mk 46 e il Mk 48 sotto la chiglia danno gli stessi tempi: tutti e due rompono lo scafo resistente (fattore d'urto
oltre 2), e da lì l'allagamento è lo stesso.

**I siluri e i missili del 688:**

| Arma | Velocità | Corsa o portata | La testa o il seeker |
|---|---|---|---|
| Mk 48 | 55 nodi (40) | 38 km (50) | passivo 4 km, attivo 1,5 km, ±45° |
| Mk 48 ADCAP (dal 1988) | 60 nodi (40) | 32 km (50) | passivo 5 km, attivo 2 km, ±50° |
| UGM-84 Harpoon | 240 m/s | 111 km, minimo 5,6 | 12, 18, 23 km per bersaglio piccolo, medio, grande |
| UGM-109B TASM | 245 m/s | 460 km, minimo 9,3 | 15, 25, 35 km |

## 4. Una scena per provarlo

Una missione rapida col 688 contro un Victor III (capitolo 11 del volume 1): fai la TMA fino a una soluzione buona,
lancia un Mk 48 col filo e l'RTE 3 km prima del bersaglio, e guarda nel replay quando la testa lo sente. Poi rifallo
con l'RTE troppo corto. Per i missili, una missione con una nave di superficie lontana 30-80 km e un peschereccio
vicino a lei: guarda che cosa aggancia il seeker se l'area non è libera.
