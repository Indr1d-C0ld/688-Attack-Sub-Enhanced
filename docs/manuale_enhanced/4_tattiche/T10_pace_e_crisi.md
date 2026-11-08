# T10. Pace e crisi

*Autori: runtime (il comportamento dell'IA, §1.A e §4) e kit (la vista, le luci, le acque territoriali, la scena;
curatore). Revisione del 03/11/2026. Stato: bozza completa. I numeri vengono dal codice (`missione_reazioni.cpp`) e
dal volume 2 (C9, E6, F2).*

In pace e in crisi la guerra non c'è, ma il mestiere è lo stesso: vedere senza farsi vedere. Cambia il prezzo
dell'errore. Un siluro sul bersaglio sbagliato, un'entrata nelle acque di un altro Stato, un periscopio visto vicino
alla costa diventano un incidente, e il debriefing lo conta.

## 1. Le regole pratiche

### A. Pedinare ed essere pedinati, le ROE, chi spara quando

1. **In pace chi ti sente ti segue, non ti spara.** Una nave militare o un sottomarino dell'IA che ha su di te una
   traccia solida, sentita nell'ultimo minuto, ti pedina:
   - la nave a 3,5 km;
   - il sottomarino a 6 km con la schiera, a 1,2 km senza.
   Se è oltre una volta e mezza quella distanza, va alla tua velocità più 5 nodi. Col solo rilevamento ti viene incontro
   a 10 nodi, finché la TMA o un ping non gli danno la posizione. Non pedinano i lanciamissili balistici, le portaerei, le
   ausiliarie, l'Iowa, le navi senza sonar attivo e i neutrali. Per staccarti devi rompere la traccia: un minuto fuori
   dal suo ascolto basta perché smetta di seguirti, finché non ti risente.
2. **Il ping «ti tengo» non è un attacco.** La nave che ti pedina pinga una volta al minuto, con una probabilità che
   sale con la sua allerta: 0,05 da A0, 0,7 da A3. Ti dice che sei tenuto, e intanto ti dà la sua posizione.
3. **L'INCSEA vale fra le navi, non per te.** Due navi di parti diverse fuori dalla guerra non scendono sotto le 1000
   yard (914 m): la nave dell'IA accosta via di 3 km ad almeno 8 nodi. Gli aerei antisom non passano sotto i 500 piedi
   entro un miglio da una nave dell'altra parte. Per un sottomarino in immersione l'accordo non esiste: nessuno si sposta
   per te.
4. **Fuori dalla guerra l'IA non spara per prima.** In pace e in crisi l'IA non usa le armi. In crisi un lancio, un
   colpo a segno o una salva d'artiglieria a segno di una parte sull'altra porta la coppia in guerra, e da lì le armi sono
   libere da tutte e due le parti. Una salva d'avvertimento non conta. Quindi in crisi il primo colpo decide la guerra:
   se lo tiri tu, ogni unità avversaria diventa libera di ingaggiarti. Su un neutrale l'IA non spara mai, salvo in
   guerra con lui, o in allerta A3 su un contatto senza classe: lì decide una volta per coppia, con P 0,3.
5. **Prima dei colpi vengono gli avvertimenti.** In crisi, e in pace nelle acque territoriali del paese che le
   difende, le navi tirano colpi d'avvertimento: tre colpi a 300-500 m di lato, poi una pausa di 2 minuti. Nelle acque
   territoriali, col battello immerso e localizzato entro 3 km, cadono due bombe d'avvertimento ogni 2 minuti, a
   500-1000 m dalla stima, e a 200-400 m dopo 20 minuti dentro. Non fanno danni, ma vogliono dire che ti hanno trovato:
   per il debriefing è l'incidente della regola 12 qui sotto.
6. **Scoperto, la forza si sveglia.** Un POSSUB alza la formazione di un livello, al più ad A2. Un PROBSUB la alza
   fino ad A3, e il resto della forza sale al FLASH, 5-10 minuti dopo per la NATO e 7,5-15 per il Patto. In pace non
   reagisce nessuno: c'è solo il pedinamento. In crisi la nave antisom più vicina ti tiene sotto, in attivo ogni 30 s,
   per un quarto d'ora; i sottomarini amici coprono con P 0,5; gli aerei antisom entro 60 km ricevono un datum. Se
   attacchi, le navi antisom vanno sul datum e le altre si allontanano. L'allerta scende di un livello ogni 45 minuti
   senza eventi, mai sotto A1 in crisi.
7. **Un sottomarino dell'IA che pinghi si allontana.** In pace, un sottomarino dell'IA che sente un ping ostile entro
   2 miglia va via con P 0,7: 10 km dalla parte opposta, a 12 nodi al più, per 10 minuti. Il ping alza anche l'allerta
   della sua forza. Un ping solo può bastare a liberare un passaggio, ma la forza da quel momento sa che ci sei.

### B. Vedere, identificare, restare fuori

8. **Identifica prima di decidere.** Il sonarista dà una classe, il libretto un'altra: le ROE leggono quella che
   dichiari tu con IDENTIFICA (volume 1, §4.3). Se colpisci un neutrale identificato male, l'errore resta tuo, e il
   debriefing confronta le due classi. Nel dubbio, guarda: altezza d'albero, sagoma, armi, segni distintivi.
9. **L'angolo sulla prora va nella TMA.** Al periscopio l'AOB, a passi di 10°, diventa un'osservazione di rotta (Z, X,
   poi Spazio), con ±10° a 6× e ±15° a 1,5×. Ti controlla la rotta della soluzione; la distanza, su una gamba sola,
   viene solo con quattro occhiate (T5, regola 9). Vicino alla prua e alla poppa la sagoma inganna: può essere
   specchiata (C9 §2.16).
10. **Di notte leggi i fanali.** Le navi accendono i fanali del regolamento:
   - verde a dritta, rosso a sinistra, bianco a poppa;
   - uno o due bianchi d'albero, il più alto a poppavia, che danno la rotta dall'allineamento;
   - alla fonda un bianco tutt'intorno;
   - in pesca verde o rosso sopra bianco (verde lo strascico, rosso il cianciolo): attento alle reti;
   - la manovra limitata rosso, bianco, rosso.
   In guerra le militari e i sottomarini vanno a luci spente: una nave senza luci in una notte di pace è un'informazione.
   Coi fanali l'AOB dell'IA sbaglia meno (×1,5 sulla sigma invece di ×2), e così la tua.
11. **Di notte sei più invisibile.** La testa si vede a 135 m a occhio nudo e a 770 m con l'intensificatore, contro i
   3,9 km del giorno (Type 18 a 3 nodi, C9 §2.8). La notte è il momento per guardare a lungo, non per correre: la piuma
   si vede anche al buio con i sensori termici.
12. **Le acque territoriali.** Le dichiara la missione, in pace e in crisi (F2). Ogni entrata toglie 30 agli ordini;
   farsi scoprire dentro (avvistati, un fix, un ping, l'avvertimento o il fuoco di una nave) è un incidente diplomatico:
   −150 alle violazioni, e il richiamo o il biasimo con le promozioni bloccate. Nelle operazioni speciali entrare è
   l'ordine, e la scoperta vale −75. In guerra non contano.
13. **Vicino alla costa sei più esposto.** Le navi di pattuglia ti cercano proprio lì, e il fondale basso ti costringe
   vicino alla superficie (T11). Tieni la testa fuori pochi secondi e la velocità sotto i 6 nodi.

## 2. Il perché

- Il libretto, IDENTIFICA, l'AOB e le luci di navigazione: volume 2, C9 §2.16-2.17; volume 1, §4.3.
- La testa a vista, il mare e la notte: C9 §2.8.
- Le acque territoriali, le ROE e il debriefing: F2 §2.1, F1.
- L'IA: E6 §2.6 (allerta), §2.7 (situazione e ROE), §2.10 (reazioni), §2.11 (pedinamento e INCSEA).
- I colpi e le bombe d'avvertimento: E4 §2.6-2.7. La radiogoniometria delle trasmissioni: C11.

## 3. I numeri

| Che cosa | Quanto |
|---|---|
| Entrata nelle acque territoriali | −30 agli ordini, a ogni entrata |
| Scoperto dentro (pace o crisi) | −150 alle violazioni, richiamo o biasimo, promozioni bloccate |
| Scoperto dentro (operazione speciale) | −75, senza richiamo |
| Neutrale colpito | −150 alle violazioni; amico −200; fuoco senza ordine −100 |
| Testa del Type 18 a 3 nodi, di giorno / di notte a occhio / con l'intensificatore | 3,9 km / 135 m / 770 m |
| Pedinamento: nave / sottomarino con la schiera / senza | 3,5 km / 6 km / 1,2 km |
| Ping «ti tengo», una volta al minuto, da A0 ad A3 | P 0,05 / 0,2 / 0,4 / 0,7 |
| INCSEA: navi / aerei | 914 m (1000 yard) / 500 piedi entro un miglio |
| Reazione della formazione | dopo 30-120 s (dall'aereo alla nave 60-180 s) |
| FLASH al resto della forza | 300-600 s NATO, 450-900 s Patto; perso con P 0,05 |
| Discesa dell'allerta | un livello ogni 45 minuti senza eventi |
| Colpi d'avvertimento | 3 colpi a 300-500 m di lato, poi 120 s di pausa |
| Sottomarino dell'IA pingato entro 2 miglia, in pace | va via con P 0,7: 10 km a 12 nodi, per 10 minuti |

## 4. Come ti vede l'IA

L'IA ti scopre con gli stessi sensori e le stesse regole che usi tu (E6 §2.2, la simmetria). Non sa la verità: sa
quello che sente, vede e le dicono.
- **Il sonar.** Ti sente col modello acustico di tutti, e il suo sonarista è lo stesso del tuo equipaggio: prima la
  traccia, poi la categoria, poi la classe. Un POSSUB o un PROBSUB su di te alza la sua allerta. I colpi dello scafo
  (portelli, attrezzi, i lanci) li sente anche lui. In silenzio o ai posti di combattimento ne fai un quarto.
- **Il ping.** Quello che fai tu gli dà la tua posizione: lo intercetta col rilevamento, e la sua forza sale. Il suo ping
  dà a lui la tua posizione e la quota.
- **La testa e gli alberi.** Le vedette e i periscopi dell'IA vedono la tua testa con le regole del C9 §2.8; un
  periscopio dell'IA guarda 10 s ogni 3-12 minuti. Il radar vede alberi e teste: per giro con P 0,3 se non è allertato,
  0,7 se ha un contatto, 0,9 se ha la posizione. Un albero visto è un evento d'allerta; in pace porta al più alla
  vigilanza (A1).
- **Le emissioni.** Il tuo radar acceso lo sente l'ESM di navi, aerei e sottomarini esposti. Una tua trasmissione la
  localizza la radiogoniometria delle navi e degli aerei nemici entro 50 km (C11).
- **La radio della forza.** Chi ti scopre lo dice alla formazione in 1-2 minuti e al resto della forza col FLASH. Un
  sottomarino lo sa al broadcast, quando sale. La posizione riferita porta l'errore di chi la manda.
- **I limiti.** Col solo passivo l'IA ha il rilevamento: la posizione le viene dalla TMA, con le gambe, o da un ping.
  Con la schiera non sa da che lato sei finché non accosta di 30°. E crede la parte che il suo sonarista le dà: un
  sottomarino preso per un mercantile non la allarma.

## 5. Una scena per provarlo

- **Di notte, in pace:** una missione rapida col 688 vicino a una rotta di traffico, con i pescherecci. Riconosci a
  periscopio le navi dai fanali, identifica una militare col libretto e manda l'AOB alla TMA. Poi guarda nel
  debriefing se la tua classe era quella del sonarista.
- **Le acque territoriali:** una missione di pace con le acque di un altro Stato sulla carta. Entra per seguire un
  bersaglio e fatti vedere col periscopio: guarda nel debriefing che cosa costa.
