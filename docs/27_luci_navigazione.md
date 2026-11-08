# 27 — Le luci di navigazione e di fonda al periscopio (piano)

Scheda di trailer-1 del 29/10/2026, approvata dall'utente lo stesso giorno (§5). Solo nell'Enhanced.

Proposta senza codice, da portare all'utente. Nasce dalla LIB-I14: oggi di notte al periscopio si vedono solo sagome
scure contro il cielo, e nessuna luce. Le fonti sono il COLREG 1972 (Convenzione di Londra, in vigore dal 1977) con
l'Annesso I, e le pratiche delle marine del 1989. I numeri sono quelli del regolamento; le scelte di gioco sono STIME.

## 1. Che cosa si vedeva nel 1989

**Le navi mercantili in moto** (COLREG, regole 21-23):
- **fanale di testa d'albero**, bianco, sui 225° di prora;
- dai 50 m in su un **secondo fanale d'albero** più alto e più a poppavia. I due fanali allineati danno la rotta a
  colpo d'occhio: è il trucco classico per l'angolo sulla prora di notte;
- **fanali laterali**, rosso a sinistra e verde a dritta, sui 112,5° ciascuno: chi vede il rosso guarda il fianco
  sinistro;
- **fanale di coronamento**, bianco, sui 135° di poppa;
- portate minime (Annesso I):
  - dai 50 m in su: testa d'albero 6 miglia, laterali e coronamento 3;
  - fra 12 e 50 m: 5 (3 sotto i 20 m) e 2.

**Le navi alla fonda:** un fanale bianco tutt'intorno, e due dai 50 m in su (prora alta, poppa bassa). Le grandi navi
accendono anche le luci di coperta.

**I pescherecci** (regola 26), il caso più importante per il gioco:
- **a strascico:** verde sopra bianco, tutt'intorno;
- **le altre pesche:** rosso sopra bianco;
- in pratica sono illuminati a giorno dalle luci da lavoro della coperta. Una flotta di pescherecci di notte è un campo
  di luci.

**Gli altri casi:**
- nave con manovra limitata: rosso-bianco-rosso;
- nave che non governa: due rossi;
- rimorchio: due o tre fanali d'albero e il giallo di rimorchio.

**Le navi da guerra:**
- la regola 1(c) consente luci diverse per le navi militari;
- in guerra, e spesso in esercitazione, navigavano **a luci spente** («darken ship»);
- in pace, in acque trafficate come lo Stretto, accendevano i fanali regolari. Le portaerei e le navi grandi avevano
  configurazioni proprie;
- **i sottomarini americani in superficie** potevano mostrare, oltre ai fanali, un **faro ambra intermittente**: un lampo
  al secondo per tre secondi, poi tre secondi spento. Chi lo vede sa di avere davanti un sottomarino. È la norma della
  Marina americana 32 CFR 707.7, in vigore dal 1983 (verifica in §6).

**Al periscopio:**
- le luci si vedono **ben oltre la sagoma**, perché un fanale a 20 m d'altezza sta sopra l'orizzonte più a lungo;
- la foschia le spegne secondo la legge di Allard: la luce cala con la distanza e con l'estinzione della nebbia;
- le onde le coprono a intermittenza, col periscopio a pelo d'acqua;
- di notte il riconoscimento cambia natura: si riconosce dalle luci (un peschereccio, una nave alla fonda, una nave che
  ci viene incontro), e una nave da guerra a luci spente è proprio quella che non si vede.

## 2. Che cosa servirebbe

**Dati (kit):**
- per ogni classe la configurazione delle luci, col tipo «mercantile», «peschereccio», «militare» o «sottomarino»;
- le posizioni dei fanali dal modello 3D: testa d'albero, laterali, coronamento, il secondo fanale d'albero sopra i
  50 m, e le luci di coperta dei pescherecci e dei mercantili;
- la portata dalla lunghezza (Annesso I).

**Motore (runtime):**
- lo **stato delle luci** di ogni unità:
  - accese in moto, alla fonda (ferma vicino a costa) o in pesca (peschereccio lento);
  - spente per le navi militari in guerra, o se la missione lo dice;
  - per ogni unità, una chiave dello scenario `luci = "accese" | "spente" | "fonda" | "pesca" | "manovra_limitata"`;
- la **luce gialla del sottomarino** del giocatore in superficie in pace, e la scelta di spegnerla (un ordine);
- **simmetria:** la vedetta dell'IA vede una nave illuminata ben oltre i 3 km notturni di oggi (fino alla portata del
  fanale, con la foschia), e una a luci spente solo da vicino. Il giocatore che emerge di notte con le luci si fa vedere.

**Vista del periscopio (kit, grafica, e principale per il cablaggio):**
- i fanali come punti luminosi col loro colore, visibili solo nel settore giusto rispetto al rilevamento relativo;
- l'intensità con la distanza e la visibilità (Allard), un alone nella foschia, l'occultamento delle onde con lo
  stato del mare;
- i fanali oltre l'orizzonte della sagoma (altezza dell'albero);
- il riflesso sul mare calmo e lo scintillio.

**Libretto e riconoscimento:**
- una tavola «luci» nel libretto, con gli schemi COLREG: mercantile da prora, da dritta e da poppa; peschereccio;
  alla fonda; manovra limitata;
- l'AOB dalle luci, cioè dal colore del laterale e dall'allineamento dei due fanali d'albero, con l'incertezza;
- l'identificazione di notte ammessa solo per «tipo» (mercantile, peschereccio, militare) finché la sagoma non si vede.

**Missioni:**
- la LIB-I14 potrebbe avere una variante notturna vera;
- molte missioni originali di notte guadagnano realismo: i pescherecci illuminati come falsi bersagli, il gruppo
  sovietico a luci spente.

## 3. Stima

| Chi | Lavoro | Costo |
|---|---|---|
| Kit | configurazioni e posizioni delle luci per le 40-60 classi, dati in `classi.toml` o `libretto.toml`; tavola «luci» del libretto | M |
| Kit e principale | i fanali nella vista del periscopio, con settori, Allard, alone e onde; il cablaggio | M |
| Runtime | lo stato delle luci per unità e la chiave dello scenario; la luce del sottomarino e l'ordine; la vedetta dell'IA con la portata dei fanali | S-M |
| Disassemblatore | la portata visiva con le luci e la foschia, come modulo puro, nel modello della vista | S |
| Trailer-1 | la chiave `luci` nello scrittore e nell'editor; la variante notturna della I14 | S |

Il totale è di circa una settimana, divisa fra le sessioni. Il pezzo più visibile e più utile è la vista; il più
delicato è la simmetria, perché la vedetta dell'IA cambia gli esiti delle missioni notturne originali.

## 4. Decisioni per l'utente

1. **Farlo** o lasciarlo per dopo.
2. **Il classico:** nel 1989 l'originale non aveva le luci. Propongo solo l'Enhanced, come il libretto.
3. **Le navi militari in pace:** luci accese secondo il COLREG (consigliato, ed è la regola) o spente.
4. **La luce gialla del 688:** accesa di serie in pace, con l'ordine per spegnerla (consigliato), o niente.

## 5. Decisioni dell'utente (29/10/2026)

- **Si fa,** solo nell'Enhanced, dopo i lavori in corso (gruppo 5, missioni di riconoscimento, achievement). Il
  classico resta senza luci, come nel 1989.
- **Le navi militari in pace:** luci accese secondo il COLREG; spente in guerra o quando la missione lo dice.
- **La luce gialla dei sottomarini americani in superficie:** accesa di serie in pace, con un ordine per spegnerla. La
  frequenza è verificata sulle fonti in §6.

## 6. Verifica della luce del sottomarino (trailer-1, 29/10/2026)

- **Fonte primaria:** Code of Federal Regulations, Title 32, Part 707 «Special Rules With Respect to Additional Station
  and Signal Lights», **§ 707.7 «Submarine identification light»**.
  - La sezione è stata aggiunta con **48 FR 4284 del 31 gennaio 1983**: nel 1989 valeva.
  - La parte 707 è del 1977 (42 FR 61596), con autorità 33 U.S.C. 182 e 1606 ed E.O. 11964.
  - Testo letto nell'edizione 2002 su govinfo.gov.
- **Che cosa dice:**
  - i sottomarini **possono** mostrare un **faro ambra** (amber beacon) **intermittente**, come segno distintivo:
    è facoltativo;
  - la sequenza: **un lampo al secondo per tre secondi, poi tre secondi spento**. È un ciclo di 6 s, cioè 30 lampi al
    minuto in media, in gruppi di tre;
  - la posizione: dove si vede meglio, il più possibile tutt'intorno all'orizzonte, ad almeno due piedi sopra o sotto i
    fanali d'albero.
- **Quando:**
  - la parte 707 riguarda le navi della Marina americana nelle acque internazionali e interne;
  - le luci speciali si aggiungono ai fanali regolari e non li sostituiscono;
  - la norma non dice di più. Scelta di gioco (STIMA): il faro si accende col sottomarino in superficie e i fanali
    accesi, cioè in pace (in transito, all'entrata e all'uscita dai porti); a luci spente, in guerra o se la missione lo
    dice, è spento anche lui. Questo è l'ordine per spegnerlo deciso dall'utente.
- **Correzione:** la scheda diceva «circa 90 lampi al minuto». Era sbagliato: sono tre lampi da un secondo ogni sei
  secondi.
- **Non trovato:** una luce equivalente per i sottomarini sovietici. Senza una fonte, l'Alfa in superficie mostra solo i
  fanali COLREG.
- Per confronto, non vale per il 1989: i Collins australiani hanno oggi una luce gialla a lampi molto rapidi, 120 al
  minuto (avvisi ai naviganti australiani).
