# T11. Acque difficili

*Autore: disassemblatore. Revisione del 31/10/2026. Stato: bozza (curatore: kit). I numeri vengono dai modelli del
gioco (le costanti del BQS-15 e la distribuzione delle chiglie); le tabelle dei dati restano generate nel volume 2.*

Fondali bassi, mine, una posizione che non sai più, e il ghiaccio sopra la testa. In queste acque il nemico è anche il
mare: si va piano, si misura, e si sceglie dove passare.

## 1. Il fondale e l'incaglio

**Le regole:**
1. **Guarda la pagina del fondale (Ctrl+H) prima di accelerare.** Il quadro del fondale è quello che il battello ha
   misurato, non la carta: davanti alla prua c'è solo quello che l'ecoscandaglio e il BQS-15 hanno visto.
2. **Tieni il franco.** Il pilota dell'IA tiene la chiglia 7,6 m (25 piedi) sopra il fondo, lungo la prua; fai lo
   stesso, e di più se la navigazione è a stima (§3): l'errore di posizione è anche un errore sul fondale.
3. **Il fondo basso nasconde e tradisce.** Sul fondo, posato, il rumore irradiato scende di 10 dB e l'eco si confonde
   col fondo; ma ogni ping dell'ecoscandaglio si intercetta (emissione `fathometer`).
4. **Da una secca si esce verso acqua più profonda.** L'incaglio ferma il battello e lo danneggia; l'uscita è sempre
   verso il fondale più profondo, a 50 m dal punto.

**Il perché:** volume 2, capitolo C12 §2.3 e §2.11-2.12.

## 2. Le mine

**Le regole:**
1. **Accendi il sonar da mine prima del campo, e vai piano.** La portata del BQS-15 (o dell'MG-70 e MG-24 sull'Alfa)
   dipende dalla tua velocità. Su una mina ormeggiata:

| Velocità | Portata |
|---|---|
| 10 nodi | 935 m |
| 15 nodi | 770 m |
| 25 nodi | 440 m |

   Una mina sul fondo si vede a meno della metà (450 m di portata di base contro 1100).
2. **Ogni spazzata ti scopre a chi ha l'intercettatore.** È un impulso di 205 dB a 25 kHz ogni 8 s: lo sentono il
   WLR-9 e l'Enisej, a pieno livello a prua, 25 dB sotto ai lati. In pace, con un intercettatore vicino, spazza solo
   dove serve.
3. **Le mine a razzo stanno in alto.** La PMR-1 e la PMR-2 si ancorano fino a 300 m e colpiscono i sottomarini sotto
   i 30 m che passano sopra di loro. Su un fondale di 200-1000 m non basta stare alti: bisogna girarci attorno.
4. **Gira largo.** Il pilota dell'IA aggira le mine viste a 300 m; fai almeno lo stesso. L'achievement «Aggirate»
   chiede 200 m.

**Il perché:** capitolo C7 §2.9 (il BQS-15) e D4 (le mine).

## 3. La navigazione incerta e i punti nave

**Le regole:**
1. **L'errore cresce da solo.** Senza punti nave il 688 Flight I perde 1,5 miglia al giorno a 1σ, il Flight II
   mezzo miglio, il Victor III 1,75, l'Alfa 3. Un Kilo a stima accumula tutta la corrente non nota.
2. **Scegli il punto nave per quello che costa** (la tabella completa, generata dai dati, è in C12 §2):
   - il **GPS** (dal 1990, se l'opzione c'è) e il **Transit** sono i più precisi, ma chiedono l'albero fuori entro 18 m,
     e il Transit due minuti sotto 8 nodi;
   - il **Loran-C** chiede tre minuti con l'albero fuori, e c'è solo in alcuni teatri;
   - l'**Omega** si fa col filo galleggiante entro 60 m, senza albero: costa meno esposizione e vale qualche miglio;
   - il **batimetrico** non chiede antenne, ma un quarto d'ora d'ecoscandaglio, che emette;
   - l'**astronomico** al periscopio vuole la notte o il crepuscolo e il cielo sereno.
3. **L'ellisse della posizione è la tua verità.** Sulla carta l'ellisse dice dove puoi essere; un'ombra di secca dentro
   l'ellisse è una secca possibile.

**Il perché:** capitolo C12 §2 (i sistemi e i punti nave).

## 4. Sotto il ghiaccio

**Le regole:**
1. **Stai sotto le chiglie, con margine.** La cima della vela del 688 sta 15,8 m sopra la sua chiglia (10,1 m di scafo
   e 5,7 di vela). Le chiglie del primo anno hanno un pescaggio medio di 3,8 m; quanto spesso superano una profondità:

| Pescaggio della chiglia oltre | Quante, nel primo anno |
|---|---|
| 5 m | 20% |
| 8 m | 2,6% |
| 10 m | 0,7% |
| 15 m | 0,03% |
| 20 m | mai (è il massimo) |

   Con 4 creste per chilometro di rotta, in 100 km ne attraversi circa 400: 3 oltre i 10 m. Per non toccarne nessuna
   tieni la chiglia del 688 sotto i 36 m (20 m di chiglia più la vela); l'IA tiene la cima della vela 25 m sotto le
   chiglie che ha misurato. Nel pack pluriennale, oltre gli 80°, le creste arrivano a 50 m.
2. **Tieni acceso il BQS-15 e guarda la volta.** La pagina del fondale diventa la pagina della volta: le chiglie a prua,
   la volta che scende (486, 487). Vai alla velocità che il quadro dice sicura: oltre, l'avviso 484.
3. **Risali solo dove si può.** Niente periscopio né alberi sotto la banchisa (489); si risale in acqua libera (488) o
   nel nilas, il ghiaccio nuovo sottile che il 688 sfonda. Le polynye del quadro hanno un'età: il pack deriva fino a un
   nodo, e una polynya di un'ora fa non è più lì.
4. **Il ping sotto il pack mente.** Ogni cresta più profonda di 6 m è un'eco ferma fino a 8 km; il sonar non la distingue
   da un bersaglio fermo. Anche le teste dei siluri vicino alla volta si fanno ingannare: lancia più in basso.
5. **Sotto il pack si sente diverso.** Le creste fanno rumore attorno ai 100 Hz (3-4 dB sopra il mare aperto a 100-200 Hz);
   sopra i 500 Hz il pack è più quieto del mare aperto, fino a 15 dB sopra i 10 kHz. Col freddo, sotto −10 °C, le
   fessure alzano i 2 kHz di una decina di dB. Le alte frequenze funzionano meglio, il basso è più sporco.
6. **La radio arriva solo in ELF,** e il punto nave che chiede un'antenna non c'è: resta il batimetrico. L'errore di
   posizione cresce per tutta la permanenza sotto la banchisa.

**Il perché:** capitolo E11.

## 5. Per provarlo

- **Libreria I03 «Acque basse, nervi tesi»** (il Baltico basso), **S06 «Petroliere»** (le mine), **I16 «Il bastione
  bianco»** e **I17 «La soglia di Ekaterina»** (il ghiaccio dell'Okhotsk; nella I17 anche le PMR-2 sulla soglia).
- **Sandbox nel pack:** un 688 a 30 m e uno a 50 m sulla stessa rotta di 50 km, nell'Okhotsk a marzo. Conta gli urti
  della vela del primo e gli avvisi del secondo.
