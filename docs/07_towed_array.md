# Towed array (schiera idrofonica rimorchiata): specifica di progetto

Stato: 23/10/2026, aggiornata con il dossier `docs/ricerca/04_towed_array.md` (fonti e gradi di
affidabilità; i parametri completi, le formule e le regole di danno sono nel suo §6). Dove il dossier
indica una **stima di gioco**, il valore è da tarare in prova.

## 1. Com'era nell'originale (1989)

- Pulsante TOWED ARRAY nella stazione sonar: un clic per filare, uno per recuperare; "qualche minuto"
  per ciascuna operazione, con i messaggi "still deploying / still retrieving towed array".
- Effetto sulla sensibilità solo per settori, in percentuale (figura 5.6 del manuale): senza schiera
  100% a prua, 50% di lato e 0–5% a poppa; con la schiera il settore di poppa sale al 65%.
- Velocità massima ridotta a 2/3 sul 688 e a metà sull'Alfa.
- Se serve accelerare con la schiera filata si può **tagliarla** ("we cut the towed array captain"):
  persa per il resto della missione. Messaggi anche per danno ("the towed array is gone") e
  impossibilità di filarla ("cannot deploy towed array captain").

Da correggere (dossier 04 §6.4):
- **l'Alfa (Progetto 705) non aveva una schiera rimorchiata**: le fonti concordano;
- la riduzione della velocità massima non ha basi fisiche: la resistenza di schiera e cavo è circa il
  3% di quella dello scafo del 688, cioè meno dell'1% di velocità. Al suo posto: velocità massima di
  traino e assordamento progressivo (§3.4–3.5).

## 2. Dati reali disponibili

| Sistema | Piattaforma (1989) | Dati | Fonte |
|---|---|---|---|
| TB-16 | 688, Sturgeon (con BQQ-5) | schiera ~73 m, Ø 8,9 cm, cavo 730–790 m, ~50 idrofoni | dossier 01, media |
| TB-23 thin-line | alcuni 688 a fine anni '80 | schiera ~290 m, frequenze più basse del TB-16 | dossier 01, bassa |
| Schiera nel "pod" sulla deriva | Victor III, Akula (Skat-KS, Skat-3) | dati quantitativi non pubblicati | dossier 01 |
| AN/SQR-19 TACTAS | FFG-7 con LAMPS III, Spruance, Ticonderoga da CG-54 | schiera ~242 m su ~1.700 m di cavo, fino a 360 m di quota | dossier 04 |
| AN/SQR-18A | Knox (dal corpo del VDS) | schiera ~222 m su ~1.524 m di cavo | dossier 04 |
| Schiere sovietiche di superficie | — | nessuna in servizio nel 1989 (Vega e Polinom sono sonar a profondità variabile) | dossier 04 |

Note: la classe Kidd nel 1989 molto probabilmente non aveva la TACTAS ("in valutazione" nel 1987).
La schiera dei Victor III, Sierra e Akula esce dal "baccello" sopra il timone; Oscar e Typhoon
l'avevano, con punto d'uscita non documentato. TB-23 confermata sul 688I (SSN-751, 1988).

Principi (dossier 02, §5): la schiera misura un **angolo conico** rispetto al proprio asse
(ambiguità destra/sinistra), è degradata verso la propria nave (endfire anteriore), ha precisione
fino a ~0,5° al traverso e oltre 5° all'endfire, impiega 2–5 minuti a raddrizzarsi dopo una virata
e **elimina in pratica i baffles**.

## 3. Modello per il gioco

### 3.1 Stati
`STIVATA → FILATURA → FILATA → RECUPERO → STIVATA`, più `DANNEGGIATA` (funziona con meno idrofoni
e più rumore) e `PERSA` (tagliata o strappata: non più disponibile nella missione).
- Filatura e recupero a velocità costante del verricello, solo sotto una velocità di manovra
  (TB-16: circa 0,9 m/s in filata, 0,75 m/s in recupero, sotto 12 nodi; TB-23 circa 1,0 e 0,85 m/s
  sotto 10 nodi; stime tarate sui ~15 minuti della vecchia BQR-15). Tempi risultanti: TB-16
  14–17 minuti, TB-23 30–45 minuti. La schiera è utile quando sono fuori almeno la sua lunghezza più
  250 m di cavo, con 6 dB di rumore in più finché il cavo filato è meno della metà del previsto.
- Il verricello produce un **rumore transitorio** contro-rilevabile all'inizio e alla fine
  dell'operazione.

### 3.2 Posizione e forma: la schiera segue la scia
Modello a "collana di perline": la schiera e il cavo sono una catena di punti che percorrono la
**traccia passata del battello** a distanza fissa lungo il percorso (cavo + metà schiera dietro la
poppa). Ne seguono, senza regole ad hoc:
- dopo una virata la schiera resta **curva** sulla vecchia rotta per il tempo necessario a
  percorrere la lunghezza del cavo: rilevamenti degradati per qualche minuto, proporzionali alla
  velocità (a 5 nodi ~800 m richiedono circa 5 minuti);
- dopo la manovra, la schiera, rettilinea su una nuova rotta, risolve l'**ambiguità** confrontando
  i rilevamenti delle due rotte (tecnica reale di 20–30° di accostata).

### 3.3 Profondità: la schiera "affonda" quando si va piano
Il cavo assume un angolo che dipende dalla velocità (peso in acqua contro resistenza idrodinamica):
calcolato con la formula della catenaria del dossier 04 §6.2 (forma semplice: `sin φ ≈ min(1, v0/v)`,
v0 ≈ 1,15 nodi per la TB-16), più l'insaccamento della schiera stessa alle basse velocità. Valori per
la TB-16: circa **165 m sotto il battello a 5 nodi** e **85 m a 10 nodi** (±30%). Sotto 3 nodi le
schiere "grosse" non sono efficaci (NAVSEA): insaccano e perdono guadagno. Conseguenze di gioco:
- **sotto lo strato termico**: a bassa velocità la schiera può stare sotto il termoclino mentre il
  battello è sopra, e ascoltare entrambi gli strati. È una tattica reale e da premiare;
- **acque basse**: nei teatri di piattaforma (Baltico, Barents meridionale, Piattaforma Celtica) la
  schiera può **toccare il fondo** se si rallenta troppo. Il motore confronta la quota di ogni tratto
  di schiera con la batimetria reale (heightmap dei teatri) e applica danni o perdita.

### 3.4 Prestazioni acustiche
- Guadagno di direttività della schiera lineare `DI ≈ 10·log(2L/λ)` e fascio a −3 dB
  `θ₃dB ≈ 50,8°·λ/L`, che si allarga verso l'endfire (dossier 02 §1): la schiera lunga lavora bene in
  **bassa frequenza** (toni a banda stretta, rilevazione a grande distanza e in zona di convergenza).
- Rumore proprio quasi senza macchinari del battello (è lontana), ma con **rumore di flusso** che
  cresce rapidamente con la velocità oltre un "ginocchio" (TB-16 circa 8 nodi, TB-23 circa 6):
  `+60·log(V/V_ginocchio)` dB; in pratica sorda verso 18 nodi (TB-16) o 12 (TB-23).
- **Cieca durante le manovre**: con velocità angolare oltre 0,3°/s i rilevamenti non sono validi
  (per la TB-23 FAS indica fino al 50% di tempo perso nelle manovre); dopo la virata errore ×3 fino
  al raddrizzamento, `T = k·(cavo + schiera)/v` con k = min(1, Δψ/60°): circa 5 minuti a 5 nodi.
- Bande indicative: TB-16 10–600 Hz, TB-23 5–300 Hz (stime).
- Settore cieco o degradato verso la propria nave (endfire anteriore, ±20–30°); endfire posteriore
  utilizzabile ma con rilevamenti imprecisi.
- Il rilevamento fornito è conico: il motore genera due tracce speculari finché l'ambiguità non è
  risolta; la presentazione sonar le mostra entrambe.

### 3.5 Vincoli e rischi
- **Velocità**: nessuna penalità sulla velocità massima. Velocità massima di traino (TB-16 circa
  20 nodi, TB-23 circa 16; stime): oltre, probabilità di danno crescente per ogni minuto, e oltre il
  limite più 6 nodi la schiera si può strappare. Le ore trainate ad alta velocità si accumulano come
  fatica. Quota massima della schiera circa 700 m.
- **Manovre**: virate strette a bassa velocità e marcia indietro rischiano di avvolgere il cavo
  sull'elica (danno a schiera ed elica, rumore).
- **Fondale**: vedi §3.3.
- **Reti da pesca e pescherecci**: nelle zone di pesca (piattaforme del Barents, Mare del Nord, GIUK,
  Okhotsk) piccola probabilità oraria di impigliarsi, doppia di notte; il peschereccio può segnalare
  la posizione. Incidenti documentati nel dossier 04: K-324 con il cavo della USS McCloy avvolto
  sull'elica (1983), HMS Conqueror che taglia una schiera sovietica (1982), SSBN USS Alaska che
  trascina un peschereccio (1989).
- **Cavi altrui**: passare vicino alla poppa di una nave che traina una schiera può avvolgerne il cavo
  sulla propria elica e lasciare il battello senza propulsione (caso K-324).
- **Ghiaccio** (Barents d'inverno): limiti alla filatura in superficie.
- **Taglio d'emergenza**: come nell'originale, la schiera è persa per la missione.

### 3.6 Danni
Integrata nel modello dei danni (roadmap, punto 17): un'esplosione vicina, una virata sbagliata o un
contatto con il fondo possono ridurre il numero di idrofoni funzionanti (meno guadagno, più rumore,
più ambiguità), danneggiare il verricello (niente recupero: si può solo trainare o tagliare) o
strappare la schiera.

### 3.7 Unità controllate dall'IA
- Le stesse regole valgono per gli altri sottomarini dotati di schiera (Victor III, Sierra, Akula,
  Oscar, Typhoon; non l'Alfa) e per le navi di superficie con TACTAS (Perry LAMPS III, Spruance,
  Ticonderoga da CG-54; Knox con SQR-18A).
- Comportamenti: profilo "corri e ascolta" (sprint-and-drift); pedinamento a distanza con la schiera
  filata; meno "Crazy Ivan" per chi ha la schiera, perché i baffles sono coperti.
- L'IA nemica può sfruttare i punti deboli della schiera: avvicinarsi dall'endfire anteriore,
  approfittare delle virate, giocare con l'ambiguità.

### 3.8 Interfaccia (stazione sonar)
- Indicatori: stato, lunghezza filata, quota stimata della schiera rispetto al battello e al fondo,
  velocità rispetto ai limiti (utile e di traino), forma della schiera dopo le virate.
- Waterfall dedicata alla schiera (bassa frequenza, a banda stretta) accanto a quella della sfera.
- Contatti ambigui mostrati come coppie speculari finché non vengono risolti.
- Comandi: fila, recupera, taglia; avvisi dell'operatore sonar in italiano ("schiera filata,
  comandante", "schiera ancora in filatura", "attenzione, schiera vicina al fondo"…).

### 3.9 Statistiche e achievement
Tempo con la schiera filata, contatti scoperti solo grazie alla schiera, ambiguità risolte, schiere
perse e cause. Esempi di achievement: concludere una campagna senza perdere la schiera; scoprire un
bersaglio in zona di convergenza con la sola schiera; risolvere un'ambiguità con una sola accostata.

## 4. Parametri e lacune

Tabella completa dei parametri per TB-16, TB-23, schiere sovietiche e SQR-19, formule e regole di
danno: dossier 04 §6.2–6.3. Lacune principali (dossier 04 §6.5): limite reale di velocità di traino e
velocità di assordamento, velocità del verricello, bande e spaziatura degli idrofoni, peso in acqua del
cavo (che governa la curva di profondità), dati delle schiere sovietiche, presenza di un taglia-cavo
d'emergenza sui 688, rumore del verricello. Sono tutti parametri da tarare in prova.
