# Aggiunge a proposta.md §9 le soglie del cancello del filo (scelta dell'utente del 06/11) e corregge cio' che avevo scritto su ESM e radar. Un solo giro
import pathlib
P=pathlib.Path(__file__).parent/'proposta.md'; t=P.read_text()
assert '### 9.8' not in t
def r(a,b):
    global t
    assert t.count(a)==1,a
    t=t.replace(a,b)
r("**Il costo è quello del giocatore:** ESM e radar nemici vedono l'albero (`missione.cpp:3418-3423`), senza alcuna regola nuova.",
  "**Il costo è quello del giocatore:** i radar nemici vedono l'albero (`missione.cpp:3418-3423`), senza alcuna regola nuova. L'ESM no: sente solo le emissioni (corretto il 06/11, §9.8).")
r("""Il codice aspetta il via della principale, dopo la portata del sonar e il passo di chiusura.
""","""Il codice aspetta il via della principale, dopo la portata del sonar e il passo di chiusura. **Il filo passa da una prova, con soglie scritte prima (§9.8): se non rende si toglie, e i rapporti (P3) restano comunque.**
""")
r("""- **Il vero cambiamento è un altro:** dove parte in A2 il battello non sale quasi più, quindi non mostra più l'albero. Sparisce il costo di salire.
""","""- **Il vero cambiamento è un altro:** dove parte in A2 il battello non sale quasi più, quindi non mostra più l'albero. Sparisce il costo di salire. **In I02, I05, I06, I07, S03 e S07 la salita d'ascolto era l'unica esposizione:** lì senza filo rotto non resta niente da scoprire (§9.8).
""")
i=t.index("**Domanda nuova, per la principale.**"); j=t.index("### 9.7 Costo reale")
t=t[:i]+"""**Il limite di A2, deciso.** Quando ho proposto «filo solo in A2 e A3» non avevo guardato le allerte: A2 è il minimo di partenza di 23 missioni su 27 e di 7 delle 9 misurate, e in ottobre 52 salite su 62 erano già in A2 o più. Portato all'utente, ha scelto **A2 con la prova** («Sì, A2 con prova»): §9.8.

**Due correzioni a quello che avevo scritto:**
- Avevo detto che il giocatore perde «ESM e radar» sui battelli. **Non è così, perché oggi non li ha.** Il radar dei sottomarini resta spento e il BPS-15 non vede un albero: §9.8.
- Avevo detto che l'albero sparisce «in 7 missioni su 9». **Sono 6:** in S15 il Charlie II sta esposto circa 3750 s per conto suo, e ci resta anche col filo.

"""+t[j:]
r("""**L'ordine:** corsa di base e numeri; P3 con le sue serie; prima versione del filo; cancello; prove, guasti e serie del filo. **Il cancello viene dopo la prima versione del filo e prima di prove e guasti,** così, se non funziona, si spende poco.""",
  """**L'ordine:** corsa di base e numeri; P3 con le sue serie; prima versione del filo; cancello (§9.8); prove, guasti e serie del filo. **Il cancello viene dopo la prima versione del filo e prima di prove e guasti,** così, se non rende, si spende poco.""")
r("""| Cancello: S07 semi 1 e 2, I07 seme 1, S11 con il seme in cui il Los Angeles passa ad A2 | circa 1 h. Se S07 non copia dati ostili, ci si ferma e si riferisce |
""","""| Cancello, 11 partite (§9.8) | **circa 1,5 h, non 1:** il controllo 2 e il controllo 3 chiedono partite in più |
""")
r("| **Totale** | **circa 14-15 h** |","| **Totale, se il filo passa** | **circa 15 h** |\n| **Totale, se il filo si ferma al cancello** | **circa 7,5 h:** base, P3, cancello |")
t=t.rstrip('\n')+"""

### 9.8 Il cancello del filo: le soglie

Scelta dell'utente del 06/11: filo da A2 in su, **con il cancello come uscita.** Qui ci sono le regole; i numeri si fissano sulla corsa di base di §9.4, prima del codice, e non si riscrivono. **Se il controllo 1 o il 2 non reggono mi fermo, e la principale porta l'esito all'utente.**

**Che cosa si può scoprire davvero, oggi.** Riscontrato sul codice e sulle tracce di ottobre (`occasioni_tracce.py`; `engine/build_radio/misura_allerta/occasioni.txt`), che spiegano il ragionamento e non sono soglie.

| Strumento | Sul battello dell'IA | Dove |
|---|---|---|
| **ESM, giocatore e IA** | **Niente:** il radar dei sottomarini resta spento, salvo dopo un lancio di missili. In tutte le tracce non si è mai acceso | `nucleo/src/elettronica.cpp:261` |
| ESM sulla trasmissione (con P3) | solo unità dell'IA entro 50 km, solo sull'UHF degli USA. Il giocatore no | `missione_forze.cpp:1167` |
| **Radar del giocatore** | **non vede un albero:** BPS-15 e Snoop hanno portata zero. Vede solo il battello emerso | `emettitori.toml`, `[radar.bps15]`, `[radar.snoop]` |
| **Radar dell'IA** (aerei, navi con portata contro i periscopi) | **vede l'albero: è l'unica occasione vera,** e arriva al giocatore come rapporto se è della stessa parte | `missione.cpp`, `radar_contro` |
| Periscopio del giocatore; vedette | vedono l'albero. Non è fra le occasioni chieste, ma si riporta | |

**Dove sta l'esposizione** (albero, testa o emerso), per battello:

| Missione | Per conto suo: resta col filo | Salite d'ascolto: col filo sparisce | Scoperte al radar dell'IA, sommate sui semi |
|---|---|---|---|
| S08, Foxtrot URSS | **2016-5399 s** | 0-226 s (sotto A2: resta) | 7 |
| S11, Foxtrot | **6349 s, tutta la partita** | 0 | 12 |
| S11, Los Angeles | 0 | 1177-1437 s (resta finché è sotto A2) | 0 |
| S15, Charlie II | **circa 3750 s** | 466-989 s | 0; 8 alla vista |
| S5, Victor, seme 7 | **1282 s** | 0 | 9 |
| S5, semi 1 e 3 | 0 | 0 | 0 |
| I02, Typhoon | 0 | circa 300 s | 2 |
| I05, Kilo | 0 | circa 770 s | 0 |
| I06, Los Angeles | 0 | 321-595 s | 1 |
| I07, Oscar | 0 | 1158-1505 s | 0 |
| S03, Foxtrot | 0 | 0-1177 s | 3 |
| S07, Akula | 0 | 240-301 s | 0 |
| M4, Victor | 0 | 0 (le salite si chiudono prima) | 0 |

**Previsione, scritta ora:** il controllo 1 dovrebbe reggere, con poco margine e solo in I07 e S07. **Il controllo 2 è il più a rischio:** S08 e S11 non cambiano, ma fra le partite che partono in A2 restano quasi solo S5 seme 7 e i battelli col filo rotto. In I02, I05, I06, I07, S03 e S07 il filo toglie l'unica esposizione.

**Le partite del cancello, 11, con P2+P3 contro la base agli stessi semi:** S07 semi 1 e 2; I07 seme 1; S03 seme 1; S08 seme 1; S15 seme 1; S11 con il seme in cui il Los Angeles passa ad A2; S11 con il seme in cui nessun battello tocca A2; M4 A seme 1; S5 con il seme in cui il Victor si espone; S03 seme 1 col filo rotto dal primo istante. I semi si leggono nella corsa di base.

**Il binario di misura ha due aggiunte, che non cambiano la partita:** la riga `QUADRO occasione` e l'opzione del filo rotto dall'inizio. Entrano nella corsa di base con `sonda_quadro.py`, tranne l'opzione, che serve solo al cancello.

#### Controllo 1: il dato radio viene usato di più

- **Si conta:** ogni ciclo in cui un battello orienta la caccia su un dato (`QUADRO usato`), una volta per battello, bersaglio e ciclo.
- **Dalla base:** `Ug` i dati usati nelle partite del cancello; `Ucg` i cicli candidati, con la regola di `Uc` di §9.4 sulle sole partite del cancello.
- **Regge se** i dati usati sono almeno `Ug + max(1, 25% di Ucg)`, e almeno uno viene da una copia arrivata dal filo, senza albero.
- **Nello stesso tempo:** 0 lanci sul solo dato. Un solo lancio così fa cadere il controllo.
- **Sulla carta:** se `Ucg` è 0 il controllo non può reggere. Lo dico prima di scrivere il filo.

#### Controllo 2: restano occasioni reali di scoprire un battello

- **Un'occasione** è un periodo di almeno 60 s in cui un battello USA o URSS dell'IA è esposto e un radar nemico acceso ha probabilità per giro maggiore di zero, o in cui un ESM nemico lo intercetta. Conta una volta per battello, osservatore e periodo. Le riga `QUADRO occasione`.
- **Dalla base:** `Ob` le occasioni in S08 e S11; `Oa` quelle nelle partite che partono in A2 (S03, S07, I07, S15, S5, M4).

| Gruppo | Minimo | Perché |
|---|---|---|
| **S08 e S11,** sotto A2 | almeno l'80% di `Ob`, e almeno 1 in ogni partita | Il filo non dovrebbe toccarle: è la garanzia che il limite di A2 fa quello che dice |
| **Partite che partono in A2** | almeno `max(1, 10% di Oa)`, in tutto il gruppo | Da solo il primo gruppo non basterebbe: reggerebbe anche se il filo azzerasse tutto dove lavora |

- **Regge se reggono tutte e due.**
- **ESM:** non ha un minimo a parte, perché oggi è zero e non si può chiedere di conservarlo. Se capita, conta come occasione.
- **Vista e periscopio:** si riportano e non contano.

#### Controllo 3: le partite restano coerenti, anche col filo rotto

| Prova | Regge se |
|---|---|
| **Filo rotto dal primo istante,** S03 seme 1 | stessa impronta di P3: rotto vale come non averlo |
| **Partita senza A2,** S11 con quel seme; se nella base non ce n'è, S23 seme 1 | stessa impronta di P3 |
| **Rotture vere,** tutte le partite del cancello | almeno una rottura vista in M4 A. Dopo la rottura nessuna copia, ROE o falso ordine dal filo; il filo non torna |
| **Salite** | al più 1 per ciclo; ogni salita si chiude, nessun battello resta a quota d'antenna oltre il limite dell'ascolto |
| **Prefisso,** dove c'è P3 (S03, S08, S11, S15) | le righe `QUADRO` coincidono fino al primo effetto del filo |
| **Navigazione** | nessun incaglio in più della base. Senza salite il battello perde il punto nave (`missione_navigazione.cpp:127`): riporto l'errore a fine partita |

**Se il controllo 3 non regge** è un difetto del mio codice, non un esito del filo: lo correggo, rigioco le partite toccate, una volta sola. Se non regge ancora, mi fermo e lo dico.

#### Che cosa succede dopo

| Esito | Che cosa faccio |
|---|---|
| Reggono tutti e tre | prove, guasti e serie di §9.7; poi vale §9.5 |
| **Non regge l'1 o il 2** | **mi fermo,** senza prove né guasti. Passo alla principale la tabella con soglie e risultati. Il filo resta dov'è finché l'utente non sceglie |
| L'utente toglie il filo | riporto i file alla copia salvata prima del filo (`prima_filo/`, perché qui non c'è git) e verifico l'impronta di P3 su S03 seme 1. Circa 1 ora di lavoro e 15 min di macchina. **P3 resta** |

**Se cade il controllo 2, una possibilità da offrire all'utente, che ora non decido io:** una salita ogni tanto per il punto nave anche col filo. Ha una ragione vera, perché il battello senza salite perde il punto nave, e rimetterebbe occasioni al radar proprio dove il filo le toglie. Costerebbe circa 1 ora di lavoro e il rifacimento del cancello.
"""
P.write_text(t+"" if t.endswith("\n") else t+"\n")
