# T4. Classificare

*Autore: disassemblatore. Revisione del 31/10/2026. Stato: bozza (curatore: kit). I numeri sono misurati col motore
(`tools/re/prove/suite_acustica/sonda_tattiche/t4.cpp`), non stimati.*

Sapere che c'è qualcosa non basta: prima di sparare, o di nasconderti, devi sapere che cos'è. Questo capitolo dice
come il sonarista arriva alla classe, quando sbaglia, e come aiutarlo.

## 1. Le regole pratiche

1. **Dai tempo al sonarista.** Dalla scoperta al rapporto passano 15, 30 o 60 s (esperto, medio, recluta); dal
   rapporto alla classe altri 30, 45 o 90 s. Un POSSUB al primo minuto è un'ipotesi, non una classe.
2. **Guarda le righe di rete.** I battelli americani e britannici hanno la rete a 60 Hz e le armoniche (120, 180,
   300); i sovietici a 50 Hz (100, 150, 250). È la prima cosa che separa le due marine, anche con una riga sola.
3. **Le pale le conti solo se cavita.** Il DEMON vede l'elica solo quando cavita: un battello profondo e lento non dà
   pale. A 150 m un Victor III cavita da 18 nodi, un Alfa da 16, un 688 da 20 (tabella al §3).
4. **Un colpo metallico è un sottomarino.** Un portello che sbatte, un attrezzo che cade, una pompa: il sonarista dice
   «transitorio metallico» e porta il contatto a PROBSUB subito, senza aspettare le righe.
5. **Le righe che sembrano di tutti sono tue.** Una riga uguale su due contatti diversi, nello stesso istante, è
   tua o della rete comune. Il medio e l'esperto la tolgono; la recluta a volte la lascia, e un contatto prende la
   classe del tuo battello.
6. **Un biologico può sembrare un sottomarino.** Un canto preso per una tonale diventa POSSUB: succede al 25% delle
   reclute, al 10% dei medi, al 3% degli esperti. Prima di sparare a un POSSUB senza classe, ascoltalo in cuffia.
7. **Una firma con dei buchi è sospetta.** Se mancano righe che la classe dovrebbe avere, o le pale non ci sono a una
   velocità a cui dovrebbe cavitare, può essere un falso contatto (il vetrino di Dio) o un'esca. Un ping lo svela
   (l'eco è troppo piccola), ma ti scopre.
8. **Al periscopio la classe la dici tu.** Il libretto di riconoscimento ti dà le sagome, il telemetro la distanza,
   l'AOB la rotta. Con la sagoma nitida, di giorno e al traverso, si sbaglia poco; di notte, lontano e di prua, molto
   di più. Per l'IA vale lo stesso, e lo puoi usare (§4).

## 2. Il perché

- Le firme, le righe e le pale: volume 2, capitolo C4.
- Il sonarista, i suoi profili, le righe proprie, i transitori e il falso contatto: capitolo C5 §2.7-2.11.
- Il periscopio, il libretto, il telemetro, l'AOB e i fanali: capitolo C9.

## 3. I numeri

**Le righe di rete, le pale e la velocità oltre la quale l'elica cavita** (generata da `data/motore/acustica.toml`
con la formula del motore, `velocita_cavitazione_kn`):

<!-- generato: cavitazione_classi -->
| Classe | Pale | Rete (Hz) | Cavita oltre, a 30 m | a 150 m | a 300 m |
|---|---|---|---|---|---|
| Los Angeles (SSN-688, Flight I) | 7 | 60, 120, 180, 300 | 9,9 kn | 19,7 kn | 27,4 kn |
| Sturgeon (SSN-637) | 5 | 60, 120, 180, 300 | 8,9 kn | 17,7 kn | 24,7 kn |
| Ohio (SSBN-726) | 7 | 60, 120, 180, 300 | 11,9 kn | 23,6 kn | 32,9 kn |
| Victor III (Progetto 671RTM) | 4 | 50, 100, 150, 250 | 8,9 kn | 17,7 kn | 24,7 kn |
| Alfa (Progetto 705) | 7 | 50, 100, 150, 250 | 7,9 kn | 15,8 kn | 21,9 kn |
| Akula I (Progetto 971) | 7 | 50, 100, 150, 250 | 10,9 kn | 21,7 kn | 30,1 kn |
| Sierra I (Progetto 945) | 7 | 50, 100, 150, 250 | 9,9 kn | 19,7 kn | 27,4 kn |
| Oscar II (Progetto 949A) | 7 | 50, 100, 150, 250 | 7,9 kn | 15,8 kn | 21,9 kn |
| Typhoon (Progetto 941) | 7 | 50, 100, 150, 250 | 7,9 kn | 15,8 kn | 21,9 kn |
| Delta III (Progetto 667BDR) | 5 | 50, 100, 150, 250 | 6,9 kn | 13,8 kn | 19,2 kn |
| Kilo (Progetto 877) | 6 | 50, 100, 150, 250 | 6,9 kn | 13,8 kn | 19,2 kn |
| Charlie II (Progetto 670M) | 5 | 50, 100, 150, 250 | 6,9 kn | 13,8 kn | 19,2 kn |
<!-- fine: cavitazione_classi -->

Il Victor III è l'unico con 4 pale: con un DEMON leggibile non si confonde.

**Quanto sbaglia il sonarista**, con tutte le righe della classe a 10 dB, su 200 semi per livello:

| Livello | Classe sbagliata |
|---|---|
| recluta | 33% |
| medio | 16% |
| esperto | 2% |

È lo stesso per il Charlie II, il Victor III, l'Akula, il 688, la Krivak e un mercantile: l'errore viene dalla seconda
ipotesi della libreria, non dalla classe. Con meno righe sopra la soglia l'errore sale, perché le ipotesi si avvicinano.

## 4. Come ti classifica l'IA

L'IA ha lo stesso sonarista, col livello dell'avversario della missione: impreparato è una recluta, regolare un medio,
veterano ed élite un esperto. Quindi:
- **non cavitare:** sotto la velocità della tabella il suo DEMON non conta le tue pale;
- **non fare rumori di scafo:** un portello che sbatte le dà subito PROBSUB. In silenzio di manovra succede un quarto
  delle volte;
- **alla vista,** la vedetta dell'IA sbaglia la tua classe con una probabilità che parte dal 3% e cresce:
  - con la distanza sulla sua portata, fino a +60% al limite;
  - di notte, +15%;
  - di prua o di poppa, entro 25°, +15%;
  - con la sola vela fuori, +10%;
  - al massimo l'85%.
  Di notte, di prua e con la sola vela, al limite della vista, l'IA ti prende per un'altra cosa quasi sempre.

## 5. Per provarlo

- **Libreria I13 «Sagome nello Stretto», I14 «Levante», I15 «Fanali»:** il riconoscimento a vista, di giorno, nella
  nebbia e di notte coi fanali.
- **Sandbox:** un'Alfa e un Victor III a 10 km, prima a 8 nodi e 150 m, poi a 20 nodi. Guarda quando compaiono le pale
  nel DEMON e quanto ci mette il sonarista a passare da POSSUB alla classe.
