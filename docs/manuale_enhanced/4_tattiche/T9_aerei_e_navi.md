# T9. Contro aerei e navi

*Autore: runtime; curatela: kit. Revisione del 04/11/2026. Stato: bozza curata. **Questo capitolo non ha serie nuove:** i
numeri riprendono le tarature di E5 §6 (ASW aereo) ed E4 §6 (cannoni), 100 semi per scena, in
`tools/taratura/risultati/fondo_2710/`. Sono del 27/10/2026, e vanno rifatte se cambia il modello. Il MAD è la formula di E5
§2.4, ricalcolata.*

Un aereo non si vede arrivare, e aerei ed elicotteri non si possono abbattere (E5 §7): un sottomarino può soltanto non
farsi trovare. Vince col datum fresco
e col battello rumoroso o in superficie; contro un SSN lento, profondo e silenzioso vince di rado. Una nave ha più sensori
di te sopra lo strato, ma è rumorosa, e oltre i suoi tubi manda l'elicottero. Tutto questo capitolo si riduce a due
domande: che cosa ho appena fatto che li ha portati qui, e a che quota mi sta cercando l'orecchio che ho sopra.

## 1. Le regole pratiche

**Non dare il datum.**
1. **L'aereo arriva sull'ultima cosa che hai fatto.** Un albero visto al radar è un datum fresco di 500 m. Un rapporto di
   contatto, un alleato attaccato o affondato entro 60 km, una nave colpita ne danno uno di circa 2 km.
   Dopo una di queste cose sposta il punto subito, piano e a fondo: l'incertezza del datum cresce col tempo, e l'aereo la
   insegue con lo schema.
2. **Antenna e periscopio, otto secondi.** Un P-3C a 9 km senza contatti vede la testa con 0,20 per giro: 8 s di testa fuori
   fanno 0,20-0,36. Con l'operatore allertato (un contatto su di te, o l'EMCON in ingaggio) 0,47 per giro: 8 s fanno
   0,47-0,72. Dopo un albero visto il radar resta continuo, e 60 s fanno 0,998. Alza poco, guarda in fretta, abbassa (T1).
3. **L'ESM è la tua carta (EMCON).** Senti il P-3C fino a circa 100 km quando cerca a 450 m, ma è acceso 6-18 s ogni 2-8
   minuti: un silenzio radar di qualche minuto non vuol dire che se n'è andato. In hover col sonar calato il radar è sempre
   spento; sul contatto sonar anche, se non ti ha visto. L'ESM muto non vuol dire che non ti cerca. L'S-3 col modo periscopio ti dà l'avviso: ti
   stanno cercando la testa, giù subito. Una nave in silenzio non la senti; una che accende il radar di tiro è in ingaggio,
   con te o con un aereo.

**Quando è lì.**
4. **Riconosci il cacciatore da quello che senti.** Un tonfo è una boa o un siluro; un ping è l'immersione o una DICASS; il
   sorvolo, nella finestra di Snell, è un aereo che passa proprio sopra di te (quasi sempre lo senti, 0,8 in profondità).
   Il fumogeno al periscopio è un punto MAD o uno sgancio: ti hanno già.
5. **Al tonfo non scappare a tutta forza.** A 60-90° dalla rotta, a velocità silenziosa, sotto lo strato: è quello che fa
   l'IA, e lo fa perché il rumore è ciò che le boe aspettano. Scappa a tutta forza solo col siluro in acqua (T8).
6. **Il ping che senti non ti ha ancora trovato.** Un ping si intercetta più lontano di quanto la sua eco ti scopra: la prova
   vuole da 5 a 12 volte per il sonar a immersione, e l'AQS-13B si sente fino a 27 km. C'è tempo per allontanarsi: lentamente,
   via dal punto.
7. **La quota contro il MAD.** Il MAD vede il ferro, a qualunque rumore, anche posato sul fondo, ma solo da vicino: a 430 m,
   appena sopra lo schiacciamento, il 688 è a 0,27 sulla verticale di un P-3C, a 150 m 0,99. L'Alfa in titanio è sparita già
   a 300 m.
8. **Scendi sotto l'orecchio, se puoi.** Le boe americane e le RGB-NM stanno a 18-30 m o attorno ai 300 m; quelle dell'Il-38 a
   35 m, quelle del Tu-142 a 150. Il sonar a immersione va metà delle volte 20 m sopra lo strato, metà 30 m sotto, fin dove
   arriva il cavo (125 m l'AQS-13B). Sa dov'è lo strato anche senza batitermografo. Sotto lo strato sfuggi alle boe basse, non
   al resto.
9. **Il fondo conta più della quota.** Sul fango della piana di Porcupine un 688 silenzioso si trova 2 volte su 100; lo
   stesso battello sulla sabbia delle scarpate delle Fær Øer 39 volte su 100. Se puoi scegliere, caccia e fuggi sul fango.
10. **Il tempo e il mare sono dalla tua.** Il sonar a immersione non si cala oltre mare 5 o col vento oltre 35 nodi; le RGB-NM
    si perdono oltre mare 3, la SSQ-53B oltre mare 6; il MAD con mare 6 ha 0,6 della portata. Di notte i Ka-25 e Ka-27 usano solo le boe.

**Quando sgancia.**
11. **Fra due sganci hai minuti.** L'aereo vuole un fix entro 914 m e sgancia 366 m davanti alla tua rotta stimata; poi
    aspetta 6-9 minuti per il contatto. Il Mk 46 è a 45 nodi per 11 km, e 0,8 di affidabilità: T7 dice che da oltre 1 km l'Alfa lo
    distanzia. Il 688 a 32 nodi no, e il siluro cade vicinissimo: contano l'esca e l'accostata (T8). Il Mk 46 scende fino a
    450 m, lo Sting Ray a 800. Su meno di 40 m d'acqua il Mk 46 non si sgancia (30 lo Sting Ray).
12. **Posarsi vale.** Contro un aviolanciato il dossier dà al battello posato una Pk non oltre 0,1 (dossier 17 §6.7, D2 §6).
    È l'obiettivo del modello, non una misura: nessuna taratura lo prova. Dietro ci sono la portata a un terzo e il rifiuto 7
    volte su 10 della testa attiva (T8). In 50-150 m d'acqua è la prima cosa da provare, fermo e prima del tonfo: il MAD ti
    vede lo stesso.

**Contro le navi.**
13. **Guardati dal cannone solo se ti esponi.** Immerso non può niente; il periscopio e la superficie sì. Dentro 5 km da un
    pezzo da 100-130 mm (4 dal 76 mm, 2 dall'AK-630) tieni la testa fuori meno di 15-20 s: al posto di combattimento il primo
    colpo parte in mediana a 22,6 s dalla testa fuori, e a 3 km vola altri 4 s. Una Perry a 3 km su un'Alfa con la testa fuori per 30 s la coglie 0,41 volte.
14. **Sorpreso in superficie: giù subito.** Il 688 che a 8 km da un Kara s'immerge 10 s dopo il primo colpo è sotto in media 18 s
    dopo il colpo, e nel frattempo ne prende uno diretto nel 49% delle partite.
15. **Di notte gli illuminanti dicono che ti cercano.** Una nave con un pezzo di almeno 100 mm ne lancia uno ogni 50 s sul punto
    stimato. Se li vedi, sei già visto. Il tiro notturno vale 0,8 del pieno con l'illuminante o col radar di tiro; 0,3 solo se
    la nave non illumina e ha il radar in silenzio. La Perry, col suo 76 mm, non ne ha, ma col radar acceso spara a 0,8 lo stesso.
16. **Lo strato non ti nasconde da una nave col pesce.** Knox, Udaloy, Kara, Krivak II e Grisha III hanno un VDS, e lo portano
    30 m sotto lo strato (il Grisha III lo cala solo da fermo, e la Vega arriva al più a 100 m); Perry, Spruance e Ticonderoga
    la schiera SQR-19. Ma recuperano prima di accelerare oltre la
    velocità di traino, e cacciano a sprint and drift. Muoviti quando corrono: in quel momento sentono meno.
17. **Oltre i tubi, l'elicottero.** Perry (7 km, Mk 46), Spruance e Knox (9 km, ASROC), Krivak II, Kara, Kirov, Slava e Udaloy
    (8 km, SET-65). Oltre quella distanza, con un contatto su di te entro 60 km, una Perry fa decollare l'SH-60B, una Knox
    l'SH-2F, una Type 22 il Lynx, e la nave lo guida col suo contatto. Restare fuori tiro non basta: bisogna perdere il
    contatto. Oltre mare 5 dal ponte non si parte (oltre mare 4 l'SH-2F e il Ka-25).
18. **In pace non sganciano.** Inseguono; gli americani col SUS Mk 84 (P-3C e S-3) fanno l'attacco simulato, che è un messaggio
    e non un'arma; gli altri fanno rapporto. In crisi niente armi né SUS (E5 §2.9). Non rispondere: le regole d'ingaggio
    sono in T10.

## 2. Il perché

- Boe, sonar a immersione, MAD, schemi di posa, macchina a stati: E5 §2.2-2.6. Il sorvolo, la finestra di Snell e il ping che
  si intercetta: E5 §2.13.
- Il radar contro la testa, l'ESM, l'EMCON: C10 §2.4-2.7. Il periscopio: C9.
- La reazione, la portata, le correzioni del tiro e gli illuminanti: E4 §2.1-2.5.
- Il VDS e le schiere delle navi: C6 §2.10. Le armi antisom delle navi: D1 §2.11. L'elicottero imbarcato: E5 §2.11.
- Lo strato, il fondo e perché il fango non riflette: C2 e T2.
- **La simmetria.** Il sottomarino dell'IA cacciato sente gli stessi tonfi e ping, e fa le stesse cose: stimoli e manovre
  sono in E6 §2.12. Se vuoi cacciarlo, sai che cosa farà al primo tonfo.
- **Nel classico** l'unico aereo ASW è l'SH-60B mosso dallo script, che scopre con un numero del sensore classico (E5 §4),
  e le navi non sparano col cannone al battello (E4 §4).

## 3. I numeri

**La caccia dell'aereo** (E5 §6, scoperta e colpito per sortita, 100 semi per scena, 27/10/2026; l'ultima colonna è colpiti su
sortite con almeno un lancio, quindi con pochi lanci vale poco: l'Il-38 è 2 su 6):

| Scena | Scoperta | Colpito | Pk per sortita con lancio |
|---|---|---|---|
| SH-60F, datum di 10 minuti | 0,85 | 0,10 | 0,30 |
| Datum profondo, Victor | 0,84 | 0,17 | 0,28 |
| P-3C dal cue SOSUS | 0,88 | 0,31 | 0,79 |
| Il-38 dal cue SOSUS | 0,31 | 0,02 | 0,33 |
| 688 silenzioso, piana di Porcupine (fango, oltre 3000 m) | 0,02 | 0 | — |
| Lo stesso, scarpata delle Fær Øer (sabbia, 1000 m) | 0,39 | 0,04 | 0,17 |
| Foxtrot | 0,90 | 0,24 | 0,52 |
| Be-12 solo | 0,34 | 0,03 | 0,18 (AT-1) |
| Be-12 in coppia | 0,66 | 0,10 | 0,17 (AT-1) |
| P-3C nel traffico | 0,49 | 0,13 | 0,59 |

Da leggere così: un datum fresco si paga anche se sei un SSN. Lo stesso cue SOSUS di 37 km e 30 minuti trova un Victor a 150 m
e 8 nodi 88 volte su 100; un 688 a 300 m e 5 nodi, sul fondo giusto, è quasi invisibile. Nel traffico il P-3C ha i falsi
allarmi: è la scena del cue SOSUS con 6 mercantili e 4 banchi biologici in più, e la scoperta scende da 0,88 a 0,49.

**Il MAD** (P-3C a 76 m, sulla verticale, formula di E5 §2.4, mare calmo):

| Quota del battello | 688 (R50 451 m) | Alfa (R50 227 m) |
|---|---|---|
| 100 m | 1,00, striscia ±415 m | 0,87, striscia ±143 m |
| 150 m | 0,99, striscia ±390 m | 0,51, striscia ±20 m |
| 300 m | 0,80, striscia ±249 m | 0 |
| 430 m (schiacciamento del 688 a 435 m) | 0,27 | 0 |

La portata scende a ×0,8 con mare 4-5 e a ×0,6 da mare 6.

**Il radar contro la testa** (C10 §2.10): P-3C con l'APS-115, mare 2, 9,3 km: 0,47 per giro con l'operatore allertato, 0,20
senza. A mare 5 il radar non vede: resta la vista.

**Il cannone** (E4 §6):

| Prova | Risultato |
|---|---|
| Alfa con la testa fuori 30 s a 3 km da una Perry al posto di combattimento | testa colpita 0,41; primo colpo in mediana a 22,6 s |
| 688 in superficie a 8 km da un Kara, immersione d'emergenza 10 s dopo il primo colpo | sempre sotto, in media 18 s dopo il primo colpo; colpi diretti nel 49% delle partite |

**Le portate contro la testa:** AK-630 2 km, AK-725 3 km, Mk 75 e AK-726 4 km, Mk 45, AK-100 e AK-130 5 km.

## 4. Una scena per provarlo

- **Taratura A6.3 e A6.3s** (`engine/strumenti/taratura.cpp`, missioni in `tools/taratura/missioni`): il 688 silenzioso sulle
  due acque, piana e scarpata, con lo stesso P-3C. La stessa manovra, due risultati.
- **Strumento `scene_interazioni`:** le scene T4 (Ka-27) e T7 (Il-38) cacciano il giocatore; S2 e S7 fanno cacciare da P-3C e
  SH-3H un'unità dell'IA. La storia degli stati del controllore (tonfo, boa, ping, fix) la stampa solo lo strumento
  `taratura`: il registro dello spettatore non la mostra.
- **Vetrino di Dio:** intervento `meteo` durante una caccia di un elicottero, con mare 6: guarda il sonar a immersione che non
  si cala più. Le SSQ-53B si perdono da mare 7, le RGB-NM già da mare 4. Intervento `strato` per spostare lo strato sotto
  l'orecchio dell'aereo (F8). In sandbox non hai un battello: guardi soltanto.
- **Il cannone:** missione rapida con l'Alfa, finché fra le scorte esce una Perry (la classe non si sceglie), a 3 km al
  periscopio: conta quanti secondi resisti prima del primo colpo, di giorno e di notte. La scena esatta è la taratura G7.1.
