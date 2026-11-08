# Dossier 18 — Fattori di variazione della firma acustica (688 Attack Sub, 1989)

Ricerca su fonti aperte, in italiano, per integrare il modello acustico esistente (livello a banda larga per velocità, righe di macchinario 50/60 Hz, righe di pala, cavitazione con la profondità) con i fattori che oggi mancano: regime dei macchinari, aspetto, danni, variazione individuale fra unità gemelle e altri fattori operativi.

Data della ricerca: 25 ottobre 2026. Nessun file del progetto è stato modificato.

## 0. Come leggere questo dossier

- **Confidenza**: *alta* = misura o dato primario in una fonte rivista/controllata, letto direttamente da me; *media* = fonte seria ma secondaria, oppure dato primario su un tipo di nave diverso da un sottomarino nucleare (trasferimento per analogia); *bassa* = letteratura grigia, giornalismo, riportato da terzi, o fonte non verificata direttamente.
- **STIMA** = numero mio, ricavato da una legge o da un'interpolazione; non è un dato di fonte. Ogni STIMA dichiara da dove nasce.
- **Nessun dato** = ho cercato e non ho trovato nulla di citabile in fonti aperte. Non ho inventato valori.
- Fonti che ho letto direttamente (testo completo o PDF): Ross 1976 (scansione DTIC), Trevorrow et al. 2008, McKenna et al. 2012, Gassmann et al. 2017, Sponagle 1988 (DREA), Shin et al. 1991 (NPS), Miasnikov 1994 e 1998, MIL-STD-1399 sez. 300B, Mitson 2002, Wittekind et al. 2017, ZoBell et al. 2023, Chion et al. 2019, Howard 2011, Moore 2019. Gli abstract di Arveson & Vendittis 2000, Wales & Heitmeyer 2002, Scrimger & Heitmeyer 1991, Gray & Greeley 1980 sono stati letti via Crossref (testo completo a pagamento).
- **Non consultabili** (bloccati o a pagamento): Urick 1983, Burdic 1984, Polmar, Friedman, Stefanick 1987, McIntosh 2014 (Inter-Noise), rapporto DSB 1968 «Submarine Noise Levels», ICES CRR 209 originale. Dove li cito, li cito attraverso un'altra fonte e lo dico.
- Nota di correzione: una citazione di Arveson & Vendittis come «JASA 118:129, 2000» (in un articolo di Canadian Acoustics) è sbagliata; la citazione corretta è **JASA 107(1), 118–129 (2000)**, doi:10.1121/1.428344.

---

## 1. Tabella riassuntiva

Convenzione: dB = variazione del livello (relativo al caso di riferimento indicato). «Nave» = nave di superficie; «sub» = sottomarino. Le fonti sono numerate come nella bibliografia (sezione 9).

| # | Fattore | Cosa cambia | Quanto | Fonte | Conf. |
|---|---|---|---|---|---|
| **A. Macchinari** | | | | | |
| A1 | Velocità di rotazione di una macchina rotante (pompa, ventilatore) | Livello della riga di rotazione e dei multipli; frequenza scala con i giri | Potenza sonora da squilibrio ∝ n⁴ (PWL = A + 13,3 log hp). **STIMA** per pompa a due velocità con rapporto r: ΔL = 40·log₁₀ r → r=2: +12 dB; r=1,5: +7 dB | [R1] Ross, cap. 10.1 | alta (legge); STIMA (applicazione alla pompa) |
| A2 | Pompe refrigerante reattore (RCP) | Sono fra le sorgenti principali di rumore del sub, con riduttore e cavitazione; i reattori a circolazione naturale le spengono/rallentano a bassa potenza | S5G: pompe «necessarie solo ad alta velocità/emergenza»; circolazione naturale utilizzabile fino a una potenza corrispondente a ~10 nodi (testo riprodotto da GlobalSecurity, fonte originaria non verificata) | [W1] [W2] [W3] | media/bassa |
| A3 | Pompe a due velocità sui 688 | — | **Nessun dato**: non trovo conferma aperta di pompe RCP a due velocità sulla S6G | — | — |
| A4 | Frequenza di rete (turbogeneratori) | Posizione e larghezza delle righe 60/120/… Hz | MIL-STD-1399/300B tipo I 60 Hz: tolleranza ±3 % (**±5 % per i sommergibili**), modulazione 0,5 %, transitorio ±4 %, recupero 2 s. Banda di una riga misurata: ≥0,1–1 Hz | [S1] [M1] | alta (norma, limiti di progetto); media (banda) |
| A5 | Righe elettromagnetiche | Trasformatori/statori: tono a **2× frequenza di rete** (120 Hz a 60 Hz) con armoniche; motori c.a.: 2f e sub-armonica a f; frequenza di cava = R·n ± 2f | Motori bipolari fino a +20 dB di ronzio rispetto ad altri; ingranaggi elicoidali ~10 dB più silenziosi dei dritti | [R1] Ross, cap. 10.2–10.3 | alta (come riportato da Ross) |
| A6 | Ingranaggi del riduttore | Righe di ingranamento e armoniche (fino a 6); crescono col carico; tolleranze | Tolleranza dente 0,1→0,01 mm: −30/−40 dB di SL (testimonianza di specialisti russi) | [R1] [M1] [M2] | media/bassa |
| A7 | Righe che dipendono dalla velocità della nave | Righe di macchinario di bordo (diesel, ausiliari) restano a frequenza fissa; la cavitazione cresce | Nave: righe a 178/360/540 Hz uguali entro ±2 dB fra 9,5 e 13,4 nodi, mentre il livello a banda larga cala di 17–20 dB passando da 20,4 a 13,4 nodi | [G1] Gassmann 2017; [W4] | alta (nave) |
| A8 | Ausiliari che si accendono/spengono | Righe nuove o scomparse; asimmetria del campo radiato a seconda di quale gruppo gira | Somma incoerente: una sorgente >~12 dB sotto il totale non cambia il totale; se spenta, il calo dipende da quanto era vicina al totale. Nave di ricerca: +44 kW di carico e −0,2 nodi → livello in banda ×3 (~+10 dB) | [R1] Ross, App. B; [I1] Mitson 2002 | alta (formula); media (caso) |
| A9 | Transitori (cambi di profondità, timoni/piani, apertura portelli di prua, «igiene del rumore») | Impulsi brevi e intensi | **Nessun dato** quantitativo aperto. Solo elenco qualitativo | [X1] [M1] nota 21 | bassa |
| A10 | Stato «ultraquiet» → «max low noise» → transito | SL cresce con la velocità | 4 kn → 8 kn: +5–10 dB; 8 → 15 kn: SL ∝ V⁶ (+18 dB per raddoppio); transito 15–20 dB più rumoroso di «quiet» | [M2] Miasnikov 1998 | media |
| **B. Aspetto** | | | | | |
| B1 | Banda larga, nave piccola (40 m) | Massimo al traverso, poppa e prua attenuate | Prua −12 dB, poppa −9 dB rispetto al traverso, indipendente dalla frequenza; modello cos^1,95(θ−97°)+0,08; poppa 2–4 dB sopra prua | [T1] Trevorrow 2008 | alta |
| B2 | Cargo (Overseas Harriette) | Dipolo a bassa frequenza | 16 dB fra prua e traverso a 350 Hz, ~6 dB asimmetria prua-poppa; toni <30 Hz più omnidirezionali | [A1] via [T1] | alta (citazione di seconda mano) |
| B3 | Navi commerciali, passaggi opportunistici | Poppa più rumorosa della prua | +5…+10 dB (poppa−prua), a bassa freq.; sopra 300 Hz calo 3–5 dB in avanti/indietro | [N1] McKenna 2012 | alta (ma vedi B4) |
| B4 | Container, riga di pala 8 Hz | Asimmetria poppa-prua e dritta-sinistra | poppa−prua <6 dB (e fino a 8 dB a certe inclinazioni), dritta−sinistra <9 dB (dritta +4 dB per elica destrorsa); riga diesel 48 Hz: sinistra +5 dB. Parte dell'«asimmetria» storica era effetto specchio di Lloyd (12/10 dB fra 45° e 15° di inclinazione) | [G1] Gassmann 2017 | alta |
| B5 | Sottomarino | Aspetto | **Nessun dato misurato aperto**. Qualitativo: i toni a banda stretta dipendono dall'aspetto; sorgenti dominanti a poppa | [W5] [X1] | bassa |
| **C. Danni** | | | | | |
| C1 | Elica danneggiata/sporca/incagliata | Cavitazione molto maggiore, inizio a velocità inferiore | «Danni o sporco aumenterebbero molto la cavitazione»; +10 % resistenza → +20 % indice di inizio cavitazione → >10 dB (stima approssimata dell'autore) | [S2] Sponagle 1988 | media |
| C2 | Inizio cavitazione | Salto di livello | +8…+15 dB (citato in un lavoro DRDO, non verificato); sub WWII: >40 dB fra cavitante e non cavitante ad alta frequenza | [D1] [R1] Ross, 8.5 | media/bassa |
| C3 | Difetto di bordo d'attacco | Velocità limite senza cavitazione | Difetto 0,5 mm (tolleranza ISO 484-1): occorre navigare al 45 % della velocità di un'elica perfetta | [E1] | media |
| C4 | Pale non identiche | Modulazione alla frequenza di rotazione dell'albero | «Piccole differenze fra pale» → modulazione a frequenza d'albero (righe laterali ±1× rpm attorno al tono di pala) | [R1] Ross, 8.6 | alta |
| C5 | Elica «cantante» (singing) | Toni 100–1000 Hz | Una elica su un set identico può cantare; spesso una sola pala; cessa quando la cavitazione è forte | [R1] Ross, 9.5 | alta |
| C6 | Squilibrio/disallineamento macchine | Riga di rotazione + armoniche/sub-armoniche; righe laterali | Soglie di monitoraggio vibrazioni: +6…+8 dB = significativo; +20 dB = guasto serio | [S3] Shin et al. 1991 | alta (vibrazioni); media (trasferimento al radiato) |
| C7 | Usura nel ciclo di vita | Squilibrio crescente | +5 dB in media nella vita operativa | [M2] | media |
| C8 | Distacco di piastrelle anecoiche | Meno assorbimento (sonar attivo); flusso turbolento | 5–7 % di rivestimento perso su 3 Virginia (Marina, 2010). Effetto sul radiato: **nessun dB**, solo qualitativo («più rumoroso») | [P1] [J1] | bassa |
| C9 | Allagamenti, rumori di scafo, alberi sbilanciati | — | **Nessun dato** aperto | — | — |
| **D. Variazione individuale** | | | | | |
| D1 | Stessa nave, misure ripetute | Livello a banda larga (cavitazione) | σ = 2,4 dB (pochi giorni) e 3,3 dB (anni); IC95 ±4,8 / ±6,5 dB | [S2] Sponagle 1988 | alta |
| D2 | Navi diverse (insieme) | Livello a banda larga | σ = 5 / 5,5 / 6,8 dB (3 bande, 50 navi); residuo ±6 dB nel modello JOMOPANS-ECHO; ±4/±5 dB attorno alle curve di Ross | [SH] [MG] [R1] | alta |
| D3 | Navi gemelle | — | «Gli spettri di navi gemelle possono essere molto diversi» (nessun numero) | [S2] | media |
| D4 | Sub: impronta | Insieme delle righe discrete = «ritratto acustico» unico per ogni sub; a volte identifica il singolo scafo | Nessuna percentuale in Hz | [M2] [H1] | media/bassa |
| **E. Altri** | | | | | |
| E1 | Virata | Eccesso rispetto alla marcia dritta | Inizio virata +4…+10 dB; a 160–250 Hz lineare ~1,65 dB per °/s fino a ~10 dB; ≥500 Hz fino a 18 dB con >3,5 °/s; massimo ~+20 dB sul traverso | [T1] Trevorrow 2008 | alta (nave 40 m) |
| E2 | Carico/immersione (pescaggio) | Profondità sorgente (specchio di Lloyd) e inizio cavitazione | +1,5 m di pescaggio → <2 dB in più di livello radiato medio-alto; effetto monopolo −5,2 dB a bassa freq. dopo il rifacimento dell'elica | [Z1] ZoBell 2023; [S2] | alta |
| E3 | Velocità (confronto fonti) | Livello a banda larga | 53 log V (Ross); V⁴ (nave 40 m); ~1 dB/nodo (flotta); 1,4–2,8 dB/nodo (ECHO); V⁶ (sub, flusso) | [R1] [T1] [V1] [C1] [M2] | alta/media |
| E4 | Mare grosso, vento di prua | Carico dell'elica, cavitazione | «Aumenta molto la cavitazione»; +5…20 % resistenza con vento apparente 15 m/s | [S2] | media |
| E5 | Contenuto di gas dell'acqua | Rumore di cavitazione ad alta freq. | Fino a ~10 dB fra inverno ed estate (lago, esperimento WWII) | [R1] Ross, 8.2 | media |
| E6 | Pulizia carena | Resistenza | Vernici convenzionali: +20–30 % di resistenza nel primo anno | [S2] | media |

---

## 2. Regime dei macchinari

### 2.1 Come cambia il livello di una riga con i giri (pompe, ventilatori, motori)

- Ross (1976), cap. 10.1: tutti i sistemi rotanti hanno uno squilibrio statico e dinamico residuo; la forza e il momento di squilibrio sono ∝ ω². Poiché le velocità vibratorie sono proporzionali alla forza e le pressioni acustiche alle velocità vibratorie, **la potenza sonora radiata da squilibrio cresce con la quarta potenza della velocità di rotazione** (PWL = A + 13,3 log hp, con hp la potenza meccanica erogabile). Ross dice che la relazione vale in generale per le pompe centrifughe e che «le singole macchine variano considerevolmente fra loro per il grado di bilanciamento».
- Lo spettro da squilibrio è dominato da singoli toni alla frequenza di rotazione; piccoli giochi nei cuscinetti producono seconde armoniche deboli e sub-armoniche; lo squilibrio **modula** altri toni (frequenza di pala di pompe e ventilatori, frequenza di ingranamento), producendo righe laterali a multipli della frequenza d'albero.
- **STIMA** (applicazione della legge di Ross, non un dato misurato): una pompa che passa da bassa ad alta velocità con rapporto di giri r cambia livello di 40·log₁₀(r): r=2 → +12 dB; r=1,5 → +7 dB; r=1,3 → +4,6 dB. La frequenza della riga cambia in proporzione esatta ai giri (f = rpm/60; frequenza di passaggio palette = n. palette × rpm/60). Il rapporto di velocità reale delle pompe RCP del 688 non è pubblico.
- Un'immagine di spettro in banda stretta di una pompa refrigerante di reattore in un rapporto della NPS mostra la riga di rotazione dell'albero, i suoi multipli e la riga del numero di pale della girante (Shin, Robinson, Rossano 1991, fig. IV-6, attribuita a Ref. 33); il testo non riporta valori numerici in Hz utilizzabili.
- Confidenza: alta per la legge (fonte primaria), bassa per l'applicazione numerica alla specifica pompa (nessun dato aperto).

### 2.2 Pompe RCP, circolazione naturale e regime di potenza

- Il testo di S5G su GlobalSecurity (e Wikipedia) afferma che le pompe primarie sono «una delle principali sorgenti di rumore» dei sottomarini, insieme al riduttore e alla cavitazione; l'S5G (USS Narwhal) poteva funzionare in circolazione forzata o naturale; le pompe erano più piccole, più silenziose e meno numerose di quelle dell'S5W e «necessarie solo ad alta velocità/in emergenza».
- TWZ (Amick, aggiornato 2020): la circolazione naturale non è sempre possibile; a bassa potenza, a reattore critico, può mancare l'energia per sostenerla e servono le pompe; ad alta potenza serve la pompa; **nel campo intermedio le turbine funzionano normalmente senza pompe principali**. Nessun numero di giri né percentuali di potenza.
- GlobalSecurity «Natural Circulation Reactor»: «il reattore a circolazione naturale può funzionare senza pompe intrinsecamente rumorose solo fino a una potenza corrispondente a circa dieci nodi»; strategia di Rickover per ridurre il rumore: eliminare l'apparecchiatura, spegnerla durante il funzionamento silenzioso, rallentarla o riprogettarla senza parti rotanti. Il paragrafo sembra tratto da letteratura storica (probabilmente Duncan, *Rickover and the Nuclear Navy*, 1990, e un rapporto sugli SSBN); **non ho verificato la fonte originaria**.
- Il S8G (Ohio) è il discendente dell'S5G (circolazione naturale a «frazione significativa» della piena potenza). Il 688 usa l'S6G (la sigla S6W è del Seawolf): non ho trovato in fonti aperte né pompe a due velocità né soglie di potenza per il passaggio.
- Confidenza: media (concetto), bassa (soglia ~10 nodi), «nessun dato» per le pompe a due velocità del 688.

### 2.3 Rete elettrica, turbogeneratori, righe 50/60 Hz

- Miasnikov (1998, appendice 1, e 1994): la riga più caratteristica dello spettro SL è a 50 Hz (sub russi) e 60 Hz (americani), corrispondente alla frequenza base dei generatori; i sub di prima generazione mostrano armoniche di 50/60 Hz (fino a 5 armoniche registrate); i sub di quarta generazione (fine anni '80) non avrebbero righe discrete sopra 100 Hz, «giustificato in misura minore sotto gli 8 nodi» (Gorbachev 1994). Confidenza: media (sintesi di letteratura russa, non misure).
- Ross (cap. 10.2): nuclei c.a. (trasformatori, statori) vibrano a **due volte la frequenza di rete**; la fondamentale è di solito la più forte, con armoniche; una risonanza della carcassa coincidente con un'armonica la fa irradiare molto di più. Nei motori c.a. esistono un ronzio a 2f indipendente dai giri e, per l'asimmetria dell'albero nei cuscinetti, una sub-armonica alla frequenza di rete; il ronzio dei motori bipolari modulato alla frequenza di scorrimento può essere fino a 20 dB più forte; frequenza di cava di rotore = R·n, con bande laterali a ±2f_rete.
- MIL-STD-1399(NAVY) sez. 300B (24 aprile 2008; sostituisce la 300A(1) dell'11 marzo 1992): per l'alimentazione tipo I a 60 Hz, tolleranza di frequenza ±3 % (**±5 % per i sommergibili**), modulazione di frequenza 0,5 %, tolleranza ai transitori ±4 %, escursione massima ±5,5 %, tempo di recupero 2 s. Sono **limiti di progetto per gli utenti**, non valori tipici: un generatore sano resta molto più vicino al nominale (**STIMA**). Calcolo dai limiti: modulazione 0,5 % corrisponde a ±0,3 Hz sulla riga a 60 Hz, ±0,6 Hz a 120 Hz, ±1,5 Hz a 300 Hz. Non ho trovato l'edizione in vigore nel 1989; l'edizione A del 1992 è la più vicina verificabile (i valori della edizione del 1989 potrebbero differire: **non verificato**).
- Larghezza di riga: Miasnikov stima un limite inferiore di 0,1–1 Hz per la banda di un tono, ricordando che dati pubblicati per navi e sub mostrano tipicamente alcuni Hz (con strumenti di risoluzione limitata) e che oltre pochi km l'allargamento in propagazione porta comunque a ≥0,1 Hz. Più a lungo il sub procede a velocità costante e più stabile è la macchina, più stretta è la riga. Ross definisce il fattore di qualità Q = f/Δf e dice che la larghezza è «misura diretta della stabilità della sorgente».
- Confidenza: alta (norma), media (larghezza di riga).

### 2.4 Ingranaggi del riduttore

- Ross (cap. 10.3): l'ingranamento produce toni ai multipli della frequenza di contatto dei denti; la fondamentale di solito è la più forte (Rosen: fino a 6 armoniche in riduttori planetari a denti dritti, la seconda la più forte); gli ingranaggi elicoidali sono in genere ~10 dB più silenziosi dei dritti a pari potenza; ingranaggi Novikov 6–8 dB più rumorosi dei elicoidali involuti; il rumore cresce con il carico e con la velocità periferica; a carico leggero possono comparire sub-armoniche della frequenza di contatto (ingranamento anomalo ogni 2º/3º dente); l'errore di «bobbing» (ondulazione dal taglio) produce toni eliminabili con la rettifica.
- Miasnikov (nota 24 del 1994 e appendice del 1998): migliorare la tolleranza dimensionale del dente della turbina principale da 0,1 a 0,01 mm avrebbe permesso una riduzione del livello di sorgente di 30–40 dB «secondo alcune fonti informate»; è un'indicazione di sensibilità, non una misura.
- Confidenza: alta (Ross), bassa (30–40 dB).

### 2.5 Ausiliari che si accendono e spengono, transitori, frequenza degli eventi

- **Somma di sorgenti** (Ross, App. B): se una sorgente è >~12 dB sotto il totale, accenderla o spegnerla non cambia il totale a banda larga; ma una riga stretta emerge dallo spettro continuo anche se non cambia il totale (le righe si vedono in banda stretta). Questo è utile per decidere quando far comparire/sparire una riga nel modello.
- Nave di ricerca (Mitson 2002, ICES CM 2002/J:10, fig. 6): contro vento, con +44 kW di carico elettrico e −0,2 nodi, il livello è salito «circa 3 volte (10 dB)» in una banda di frequenza della figura. Mostra che il carico dei diesel-generatori e dell'elica cambia il livello radiato di un ordine di grandezza in un'ora. L'autore osserva anche che con un solo gruppo elettrogeno in funzione si crea asimmetria del campo radiato a seconda di quale macchina gira. Il documento EUROFLEETS2 D11.1 raccomanda di ripartire il carico su più motori per ridurre le vibrazioni di ciascuno e di mappare i «profili URN» per configurazione. Dati da navi di ricerca, non da sub.
- **Transitori nei sub**: nessun valore aperto. Un documento tecnico commerciale australiano (Xenthos, senza data) classifica la firma in transitori, toni e banda larga e indica come cause di transitori: cambi di profondità e compensazione strutturale, movimento dei piani, apertura dei portelli di prua, «trasgressioni di igiene del rumore». Miasnikov (nota 21, 1994): un operatore di sonar può rilevare «rumori relativamente brevi ma molto intensi» di un sub che manovra, come «stridore del timone» o rumori legati a un cambio di regime di flusso. Una tesi NPS del 1988 (Kratovil, SUBTRAN) modella i transitori e i falsi transitori come processi di Poisson indipendenti: è una scelta di modello, non un tasso misurato.
- **Frequenza degli eventi**: nessun dato aperto sul tasso di commutazioni di ausiliari, compressori, condizionamento, pompe di sentina o frigoriferi in un sub. Qualsiasi tasso sarebbe una scelta di progetto (da dichiarare tale).
- Confidenza: media (principi), bassa (elenchi di cause transitorie), «nessun dato» per i tassi.

### 2.6 Velocità: macchinario, flusso e cavitazione

- Miasnikov (1998): in «ultraquiet» (~4 nodi) le sorgenti principali sono eliche e impianto; alla «velocità massima di basso rumore» (~8 nodi) SL può superare quella di «quiet» di 5–10 dB; in transito (fino a ~15 nodi) predomina il rumore di flusso, SL ∝ V⁶ (raddoppio velocità = +18 dB); transito 15–20 dB più rumoroso di «quiet». Tabella A1 (SL a 5–200 Hz / livello spettrale a 1 kHz per 1 Hz, dB re 1 µPa a 1 m): rumoroso 140/120; quiet 120/100; molto quiet 100/80, a ~4 nodi. Nei modelli di Miasnikov (1994) il livello a 300 Hz si assume 10 dB più basso di quello a 30 Hz.
- Moore (2019, Proceedings of ACOUSTICS 2019), con dati da Miasnikov e Lurton: sotto gli 8 nodi domina il rumore di macchinari e cresce con la velocità; sopra gli 8 nodi flusso e propulsione crescono più in fretta. La stessa fonte riporta la tabella di ordini di grandezza di Lurton (2002): sub WWII elettrico 140/120; moderno elettrico 100/80; moderno diesel 140/120; SSN recente 110/90; SSBN recente 120/100 (RNSL/RNL1K, a 4 nodi).
- Wittekind et al. (DAGA 2017, portacontainer 3600 TEU): la dipendenza dalla velocità riguarda solo 20–90 Hz (cavitazione); sopra 100 Hz compaiono strutture tipiche dei diesel; le righe dei diesel ausiliari a 40–50 Hz sono indipendenti dalla velocità e mascherate dalla cavitazione a 75 giri/min.
- Gassmann et al. (2017): M/V MONT a 13,4 e 9,5 nodi: stessi picchi spettrali a 178, 360, 540 Hz con livelli concordi entro 2 dB; livello a banda larga 192 e 189 dB re 1 µPa²·m²; 17–20 dB più basso che per M/V SCS a 20,4 nodi.
- Confidenza: media (sub, secondaria), alta (navi).

---

## 3. Aspetto (direttività)

### 3.1 Nave di superficie, misure dirette

- **Trevorrow, Vasiliev, Vagle (2008)**, nave oceanografica di 560 t e 40 m: massimo al traverso, prua e poppa ridotte di circa 12 e 9 dB «indipendentemente dalla frequenza» (abstract); nel testo, picco di banda larga al traverso 8–12 dB sopra la prua, con poppa 2–4 dB sopra la prua, soprattutto a bassa frequenza, e variazione trascurabile entro 20° dalla prua. Modello empirico frequenza-indipendente: **S_AA = 10·log₁₀[cos^1,95(θ − θ₀) + 0,08] dB, con θ₀ = 97° (lobo principale 7° a poppavia del traverso)**, vicino al coseno quadro di un dipolo orizzontale. Ipotesi dell'autore: schermatura da scafo, direttività dell'elica e assorbimento del bulbo di bolle del cavo. Gli autori citano Arveson & Vendittis: 16 dB fra prua e traverso a 350 Hz, ~6 dB di asimmetria prua-poppa, e comportamento più omnidirezionale per toni <30 Hz. Confidenza alta.
- **Arveson & Vendittis (2000)**, bulk carrier di 173 m: radiazione «generalmente dipolare a bassa frequenza»; scostamenti che possono indicare interazioni con lo scafo; riga di pala a 9 Hz a 16 nodi: 174 dB re 1 µPa a 1 m; concordanza ragionevole col modello di Gray & Greeley (1980) per la fondamentale, meno per le armoniche; toni di generatore diesel di servizio, di frequenza di accensione del motore principale e armoniche di pala da cavitazione. Confidenza alta.
- **McKenna et al. (2012)**, sette tipi di navi commerciali (passaggi opportunistici, ricevitore sul fondo, ~3 km di CPA): livelli di poppa di **5–10 dB** superiori a quelli di prua, a basse frequenze. Nella discussione ricordano che studi precedenti a controllo del moto della nave indicavano una radiazione dipolare e, sopra 300 Hz, un calo di 3–5 dB in avanti/indietro (Arveson & Vendittis 1996), e per una nave costiera di 560 GT una riduzione di 12 dB a prua e 9 dB a poppa per angoli >40° (Trevorrow et al.). Spiegazione: la prua è schermata dallo scafo, la poppa è in parte assorbita nella scia di bolle. Confidenza alta per l'osservazione; **avvertenza** (vedi 3.2): parte dell'asimmetria è confusa con l'angolo di inclinazione.
- **Mitson (2002)**: il campo radiato ha forma «a farfalla» (butterfly) per via dello scafo che si restringe verso prua: calo significativo di livello direttamente davanti alla nave; i livelli a dritta e a sinistra possono differire in modo importante, e per questo nelle misure ICES si conservano per ciascun lato. Confidenza media.
- **Catineau et al. (2022)** (Canadian Acoustics 50(3), 108–109, abstract esteso): nave da carico/passeggeri a 14,3 nodi; bande da cavitazione (70–105 e 340–360 Hz) più alte fra 90° e 140° dalla prua, «coerente con la letteratura: il rumore dell'elica verso prua è mascherato dallo scafo». Confidenza media.
- Standard: ANSI/ASA S12.64-2009 e ISO 17208-1 (2016) misurano a 15°, 30°, 45° di inclinazione sui fianchi; STANAG 1136 per le navi militari. NSWC definisce i settori di aspetto: traverso circa 80–110° e settori di prua/poppa entro ~10° dall'asse (riportato da Xenthos, bassa confidenza).

### 3.2 Avvertenze: effetto specchio di Lloyd e rotazione dell'elica

- Gassmann, Wiggins, Hildebrand (2017, JASA 142, 1563): riga di pala a 8 Hz di un portacontainer da 10 036 TEU: asimmetria poppa-prua **<6 dB** e dritta-sinistra **<9 dB**. In dettaglio, per M/V SCS a 20,4 nodi: livelli a dritta di 4 dB superiori a sinistra a 30° di elevazione (elica unica destrorsa) e a poppa di 8 dB superiori a prua per 60° di inclinazione (riga di pala); la terza armonica della frequenza di accensione del diesel (48 Hz) ha asimmetria prua-poppa meno marcata e asimmetria dritta-sinistra *inversa* (sinistra +5 dB). I livelli «affetti dalla superficie» calano di 12 e 10 dB fra 45° e 15° di inclinazione (effetto dipolo dello specchio di Lloyd), mentre i livelli corretti per la superficie differiscono di soli ±3 dB. Le stime consecutive variano a scatti fino a 10 dB. **Implicazione**: una parte dei 5–10 dB di McKenna è effetto geometrico dell'inclinazione, non direttività della sorgente. Gli stessi autori stimano che i livelli a 10° di McKenna possano essere ~12 dB inferiori ai livelli standard ANSI/ISO. Confidenza alta.

### 3.3 Sottomarini

- **Nessuna misura aperta di direttività di sottomarini** (broadband o righe). Cosa si trova:
  - Howard (2011, Submarine Review/NPS): la natura dipendente dall'aspetto di molti toni a banda stretta permette di dedurre la direzione di transito di un sub (qualitativo).
  - Xenthos: le sorgenti dominanti di un sub sono spesso localizzate attorno all'apparato di propulsione a poppa (qualitativo).
  - Wikipedia (incidente Kola 1968): il HMS Warspite seguiva l'Echo II rimanendo nelle sue «baffles», cioè nella zona d'ombra del sonar del bersaglio, non una direttività della sorgente.
  - Moore (2019) riporta che la *forza di bersaglio* attiva di un sub è massima al traverso: è un'informazione sul sonar attivo, non sul radiato.
- **STIMA** per il simulatore, da usare come punto di partenza e dichiarare come tale: usare per il banda larga la forma di Trevorrow (traverso 0 dB, prua −10…−12 dB, poppa −6…−9 dB, massimo 7° a poppavia del traverso) con la poppa attenuata meno di una nave perché l'elica sta in asse a poppa e non c'è bulbo di bolle superficiale; per le righe di rete/macchinario sotto 100 Hz usare direttività debole (±3 dB), come i toni <30 Hz di Arveson & Vendittis. Confidenza bassa.

---

## 4. Danni

### 4.1 Elica

- **Sponagle (1988, DREA TM 88/210)**: «qualsiasi danno all'elica o incrostazione (alghe, cime, reti) aumenterebbe molto la cavitazione». Cambi del pescaggio (profondità dell'elica), delle appendici, del flusso trasversale, dell'azione del timone e della resistenza modificano le velocità di inizio. Stima approssimata di un caso: +10 % di resistenza → +20 % sull'indice di cavitazione di inizio → **oltre 10 dB** (usando dati su un profilo idrodinamico, «da considerare solo una stima grezza»). Confidenza media.
- **Ross (1976)**:
  - Cap. 8.6: «piccole differenze fisiche fra le pale producono modulazione alla frequenza di rotazione dell'albero»; la modulazione a frequenza di pala/albero permette agli operatori di classificare i bersagli dal «turn count» (giri).
  - Cap. 9.5: *propeller singing*, toni forti fra 100 e 1000 Hz, dipendono da dettagli di poco conto (spigolo d'uscita); «non è raro che un'elica su un set di eliche apparentemente identiche canti mentre le altre no»; spesso una sola pala e solo per una parte del giro; cessa quando la cavitazione diventa forte; curabile con bordi anti-singing o smorzamento. Confidenza alta.
- **Sub WWII (Ross 8.5)**: ad alta frequenza i sub erano «virtualmente non rilevabili» senza cavitazione e «facilmente rilevabili» con essa: differenza >40 dB; velocità di inizio a quota periscopica 3–5 nodi, perché eliche progettate per flusso uniforme operavano nello strato limite dello scafo. I sub nucleari con elica centrale singola hanno velocità di inizio molto superiori. Confidenza alta (WWII).
- **Salto all'inizio della cavitazione**: un articolo DRDO (non verificato direttamente, citato dai risultati di ricerca) riporta che l'inizio è accompagnato da un aumento improvviso di 8–15 dB misurato con idrofono sul fondo. Confidenza bassa-media.
- **Difetti di bordo d'attacco** (studio finanziato da Transport Canada con Memorial University, DRDC Atlantic e Dominis Engineering, riportato da ECO Magazine): difetti di 94, 250 e 500 µm; con 0,5 mm (la tolleranza ISO 484-1) la nave dovrebbe navigare al 45 % della velocità di un'elica senza difetto per evitare il rumore di cavitazione; difetti minori richiedono riduzioni minori. Nessun dato in dB. Confidenza media.
- **Caso storico** (secondario, Wikipedia, incidente Kola 1968): l'Echo II (K-131) ridusse la potenza a una delle due eliche; ridusse insieme rumore e velocità, e il sonar del Warspite notò il cambiamento acustico. Mostra che un cambio di regime d'asse è un evento rilevabile; non è un caso di danno. Confidenza bassa.

### 4.2 Macchinari fuori asse, alberi sbilanciati, usura

- **Shin, Robinson, Rossano (1991, NPS-ME-91-002, DTIC ADA242189)**: rassegna di monitoraggio e diagnostica per macchinari di sottomarino. Nel monitoraggio in banda stretta, i componenti individuali di frequenza sono «molto più sensibili al danno» delle misure a banda larga. Criterio di allerta usato nel programma della Marina per navi di superficie: **+6 dB** di una riga rispetto alla misura precedente; un'altra fonte: **+6…+8 dB** su una base di riferimento è significativo e **+20 dB** indica un problema serio. La crescita non è lineare nel tempo (livello stabile prima del guasto improvviso); molti guasti non cambiano una singola riga ma producono armoniche e *bande laterali* (cuscinetti, ingranaggi disallineati). Il rapporto osserva che gli strumenti per la diagnostica possono anche indicare i livelli di rumore radiato. Confidenza alta per le soglie di vibrazione; media come proxy del radiato.
- Ross (cap. 10.1): squilibrio rotazionale → tono alla frequenza di rotazione, debole seconda armonica e sub-armoniche. Dalla legge n⁴: a parità di giri, uno squilibrio doppio in forza produce +12 dB nella riga (**STIMA**, potenza ∝ forza² per l'ipotesi di Ross).
- Miasnikov (1998): l'usura e lo squilibrio fanno crescere il rumore di un sub di **~5 dB in media** nella vita di servizio (parere di specialisti russi). Confidenza media.

### 4.3 Rivestimenti anecoici

- Marina USA, ottobre 2010 (riportata da quotidiani; fonte primaria non consultata): le USS Virginia, North Carolina e Texas persero ciascuna il 5–7 % del rivestimento; poi ridotto a <2 %. Polmar: quando i pezzi si staccano il flusso sullo scafo viene perturbato e il sub diventa più rumoroso; Thompson (Lexington Institute): rischio di rilevamento maggiore. Non esiste un valore in dB.
- J. Mar. Sci. Eng. 11, 1671 (2023): lo stato di distacco incide soprattutto sull'assorbimento a bassa-media frequenza (<7 kHz): riguarda il sonar attivo, non il radiato.
- Moore (2019) mostra un esempio di piastrelle mancanti su HMS Triumph e ricorda che mantenere la copertura per tutta la vita è importante.
- Confidenza bassa per l'effetto sul radiato; media per i fatti.

### 4.4 Allagamenti, rumori di scafo

Nessun dato aperto sull'effetto di un allagamento o di danni di scafo sulla firma radiata (né dB né righe). Non ho trovato casi documentati. È un buco da riempire con una scelta di progetto esplicita (non come dato storico).

---

## 5. Variazione individuale fra unità della stessa classe

### 5.1 Dati quantitativi (navi)

- **Ripetibilità della stessa nave** — Sponagle (1988, DREA TM 88/210): analisi di misure ripetute di tre navi da guerra canadesi con diverse eliche. Varianza di 5,92 dB² (σ = 2,43 dB) in serie di pochi giorni; 10,99 dB² (σ ≈ 3,3 dB) in serie distanziate di anni; distribuzione ben approssimata dalla normale. Intervalli di confidenza per misura singola: 50 %: ±1,63 dB (breve) / ±2,22 dB (lungo); 95 %: ±4,77 / ±6,50 dB; 99 %: ±6,25 / ±8,52 dB. La variabilità non dipendeva dal livello, dalla frequenza né dai giri, ma dal tempo fra le misure. L'autore scrive anche che «gli spettri di rumore radiato di navi gemelle, nelle stesse condizioni nominali, possono essere piuttosto diversi». Valido per frequenze sopra il picco di cavitazione. Confidenza alta.
- **Fra navi diverse**:
  - Ross: le singole navi si discostano di **±5 dB** dalla curva livello-velocità di punta d'elica; i valori medi di più navi di **±4 dB** dalla curva 53 log V; navi grandi (corazzate, portaerei) sono circa **10 dB** più rumorose delle piccole alla stessa velocità. Confidenza alta.
  - Scrimger & Heitmeyer (1991): 50 spettri di navi mercantili (Genova): istogrammi di livello di sorgente approssimativamente gaussiani con σ = **5, 5,5 e 6,8 dB** nelle tre bande; passeggeri/traghetti, mercantili e petroliere hanno medie e σ comparabili. Confidenza alta.
  - Wales & Heitmeyer (2002): su 54 spettri, correlazione trascurabile fra livello e velocità o lunghezza; il modello classico di Ross sovrastima la variabilità. Confidenza alta.
  - Trevorrow et al. (2008): «variabilità tipica di ±6 dB attorno alla media» per spettri d'insieme di navi civili e militari.
  - MacGillivray & de Jong (2021): modello JOMOPANS-ECHO, incertezza (σ del residuo fra modello e misura) stimata in **6 dB**; Chion et al. (2019): fino a **30 dB** di discrepanza riportata fra studi per navi della stessa classe nelle stesse condizioni, in gran parte per metodi di misura e correzione dello specchio di Lloyd (non tutta variazione fisica). Confidenza alta per i numeri; media come stima della variazione fra gemelle (contengono metodo + condizioni).
  - McKenna et al. (2012): range da 177 dB (chimichiera 26 kGT) a 188 dB (portacontainer 54 kGT) di livello di sorgente a banda larga; modelli semplici da lunghezza, stazza e velocità non possibili.
- **Righe a frequenza individuale**: nessuna fonte aperta con percentuali di differenza di Hz fra righe di unità gemelle. Fatti qualitativi: Ross (propeller singing, squilibrio «varia considerevolmente da macchina a macchina»). **STIMA** (ragionamento da principi, non da fonti): le righe legate alla frequenza di rete (60, 120 Hz) sono nominalmente identiche in tutta la classe; le righe legate ai giri di macchine individuali (pompe, compressori, motori a induzione con scorrimento diverso) possono differire fra unità di qualche punto percentuale e variare con il carico. Per la riga di pala a 8–9 Hz, vale f = N_pale × rpm/60 con rpm scelti dal comandante, quindi la «firma» dipende dal regime più che dalla nave. Confidenza bassa.

### 5.2 Riconoscimento del singolo scafo (ACINT, SOSUS)

- Miasnikov (1998, appendice 1): «l'insieme delle componenti discrete dello spettro (il cosiddetto "ritratto acustico") è unico per ogni sottomarino e può essere usato per identificare la sorgente». Confidenza media (sintesi di letteratura, non misura).
- Howard (2011, LT John Howard, NPS, *The Submarine Review*, aprile 2011): i toni a banda stretta associati a pompe, generatori e riduttori «possono anche agire come una forma di impronta acustica per identificare una data classe di sottomarini e talvolta persino un singolo scafo»; l'LOFAR è nato così. Confidenza media.
- Wikipedia *Acoustical intelligence* (che cita OPSEC, *Intelligence Threat Handbook*, 1996): l'analisi in banda stretta serve a identificare «categoria, tipo e idealmente nome individuale»; difetti di lavorazione «per quanto piccoli» introdotti in fabbrica; pompe rilevabili nei nucleari. Confidenza bassa.
- GlobalSecurity (S5G): «l'arte del rilevamento sonoro era diventata così raffinata che ascoltatori esperti potevano identificare i modelli sonori unici dei singoli sottomarini». Confidenza bassa.
- Caso: 9 ottobre 1968, HMS Warspite identificò un sommergibile sovietico (Echo II) tramite la propria libreria di firme acustiche (Wikipedia, incidente Kola 1968); è identificazione di classe, non di scafo. Confidenza media-bassa.
- Non ho trovato caso documentato in fonti aperte di identificazione di un singolo sub sovietico da SOSUS con dati quantitativi (frequenze, percentuali): risultato negativo dichiarato.

---

## 6. Altri fattori utili

### 6.1 Virate e manovre

- **Trevorrow et al. (2008)**: durante virate di 90° e 180° la nave oceanografica ebbe firme **fino a 20 dB sopra** i valori al traverso di marcia dritta alla stessa velocità. Dopo correzione per aspetto e velocità: all'inizio della virata un salto di **4–10 dB** (timone e sbandamento); a 160–250 Hz eccesso circa lineare con la velocità di virata (pendenza ~1,65 dB per °/s) fino a ~10 dB; da 500 Hz in su pendenze 2,4–3,1 dB per °/s e fino a **18 dB** con velocità di virata >3,5 °/s, in prossimità del punto di velocità minima (nel caso studiato la velocità scese al 72 % con giri costanti, riducendo il rapporto di avanzamento J da 0,674 a 0,488). Un secondo picco si presentò circa 50 s dopo l'inizio. Il caso era una virata di 180° con raggio di 68 m in circa 55 s. A 4 kHz il livello salì da ~125 a 141 dB. Confidenza alta per la nave; **da trasferire per analogia ai sub** (media).
- **Trevorrow (2021, JASA 149, 2270)**: durante i massimi giri di timone compare un meccanismo secondario, di alta ampiezza, a distribuzione lognormale. Confidenza alta.
- Miasnikov: il rumore di un sub cresce quando manovra; rumori brevi e intensi (stridore di timone, cambi del regime di flusso). Confidenza media.
- La velocità di modulazione dei livelli in virata è molto più lenta di un transitorio impulsivo: si misura in decine di secondi.

### 6.2 Carico, pescaggio e immersione

- **ZoBell et al. (2023, PLOS ONE 18, e0282677)**: 12 navi gemelle Maersk classe G (accorpate come identiche nell'analisi statistica); dopo il rifacimento +1,5 m di pescaggio medio (da 10,9±0,8 a 12,4±1,1 m) comportò livelli radiati **<2 dB** maggiori in media e alta frequenza, per minore effetto dipolare; i livelli monopolo a bassa frequenza (8–40 Hz) scesero invece di **5,2 dB** per le modifiche di elica (da 6 a 4 pale, +10 % giri; seconda armonica da 12 a 10 Hz) e prua. Confidenza alta.
- Sponagle (1988): variare il pescaggio cambia la profondità dell'elica e quindi le velocità di inizio della cavitazione. Mitson (2002): navigando col vento in poppa a carico leggero o contro vento a carico maggiore il livello cambia anche di ~10 dB.
- Ross (cap. 8.5): per i sub la cavitazione dipende da V/√(profondità + 9 m) (grandezza del 1944, sub WWII); «effetto profondità anomalo»: il rumore ad alta frequenza di sub cavitanti poteva *aumentare* con la profondità. Già coperto dal modello esistente.

### 6.3 Stato di manutenzione e invecchiamento

- Moore (2019), citando McIntosh (2014, Inter-Noise 2014: «Real variability in ship systems' noise and vibration») [non consultato]: la firma acustica non dipende solo da velocità e configurazione dei macchinari ma cambia nel tempo con la manutenzione, e con l'incrostazione biologica dello scafo che aumenta resistenza e quindi rumore di propulsione e di flusso. Confidenza media.
- Miasnikov: +5 dB in media nella vita operativa. Sponagle: la varianza cresce col tempo fra le misure (σ 2,4 → 3,3 dB). Vernici convenzionali: resistenza +20–30 % nel primo anno.

### 6.4 Mare, vento, gas disciolto

- Sponagle: il mare mosso «aumenta molto la cavitazione» e fa variare il carico dell'elica col moto (aria intrappolata sotto le chiglie di rollio può cambiare molto il rumore); vento apparente di prua di 15 m/s aumenta la resistenza del 5–20 %.
- Ross (cap. 8.2): lo stesso grado di cavitazione ha prodotto fino a ~10 dB più di rumore ad alta frequenza in inverno che in estate in un lago, per il diverso contenuto di gas dell'acqua.

### 6.5 Velocità: confronto degli esponenti (per tarare la curva di base)

| Fonte | Relazione | Note |
|---|---|---|
| Ross 1976, Eq. 8.31 | L = 170 + 53·log(V/10 kt) dB (>100 Hz, dati WWII 8–24 kn) | medie di più navi ±4 dB |
| Ross 1976, Eq. 8.33/8.34 | pendenze 50 e 60 log V con dipendenza dalla stazza | non adatte a >30 000 t |
| Ross 1976, Eq. 8.35 | 60 log della velocità di punta d'elica (15–50 m/s) + 10 log (n. pale/4) | ±5 dB singola nave |
| Trevorrow 2008 | broadband ∝ V⁴ (2,6–6,1 m/s) | nave 40 m; nel caso V>4 m/s concorda con Ross |
| Chion et al. 2019 | 15,39 dB × log₁₀(V/1 kn) (modello misto); pendenze 11,7–49,9 (mediana ~21) | dipende dal metodo |
| Veirs et al. 2016 | ~+1 dB/nodo | flotta mista, Haro Strait |
| Chion (dati ECHO) | 1,4–2,8 dB/nodo; −40 % velocità ≈ −10 dB | classe-dipendente |
| Miasnikov 1998 | V⁶ (sub, flusso) | +18 dB per raddoppio |

---

## 7. Proposte di parametrizzazione per il simulatore

Tutto ciò che segue sono **STIME o scelte di progetto**, ricavate dai dati sopra; non sono dati di fonte.

1. **Riga di macchinario a velocità variabile**: L(n) = L₀ + 40·log₁₀(n/n₀); f ∝ n; larghezza di riga 0,1–1 Hz (Miasnikov), cresce durante i cambi di regime.
2. **Righe di rete**: base 60 Hz e 120 Hz (ronzio magnetico a 2f) e armoniche fino a ~5; deriva e modulazione entro il limite 0,5 % della norma (±0,3 Hz a 60 Hz); transitori ±4 % per ≤2 s dopo un distacco di carico. Il livello di ciascuna riga è indipendente dalla velocità (±2 dB).
3. **Pompe RCP**: (a) tre stati: spente (solo circolazione naturale), lente, veloci; (b) i passaggi di stato fanno comparire/scomparire la riga e la sua serie di armoniche (livello secondo il punto 1); (c) soglia di potenza attorno a quella di ~10 nodi, **non confermata** per il 688. I livelli assoluti delle righe di pompa non hanno fonte aperta e restano da decidere.
4. **Ausiliari**: modellarli come righe on/off con somma incoerente (Ross, App. B): una riga che sta 12 dB sotto il fondo non muove il totale ma si vede in banda stretta.
5. **Transitori**: nessun valore pubblico; trattarli come eventi di Poisson con tasso e livello scelti dal progettista, e cause da elenco (cambi di profondità, piani, portelli, urti).
6. **Aspetto**: forma di Trevorrow per banda larga; per i sub fattore di poppa meno attenuato (**STIMA**); per le righe <100 Hz direttività debole.
7. **Danni**: modificatori discreti sul modello di cavitazione: velocità di inizio abbassata e salto +8…+15 dB (non verificato); +6…+8 dB sulle righe di una macchina con cuscinetto danneggiato, +20 dB per guasto grave (soglie di monitoraggio, non misure di radiato); modulazione d'albero per pale non identiche.
8. **Variazione individuale**: scarto per scafo a banda larga σ ≈ 3 dB (da ripetibilità, Sponagle) — per unità gemelle un valore plausibile sta fra 3 e 6 dB (**STIMA** da D1–D2, non misura di navi gemelle); righe di rete identiche per la classe, righe da giri con piccoli scostamenti per scafo.
9. **Virate**: sommare all'SL un eccesso dipendente dalla velocità di virata (Trevorrow: ~1,65 dB per °/s a bassa frequenza, 2,4–3,1 dB per °/s sopra 500 Hz, con picchi fino a +18 dB), salto iniziale 4–10 dB, durata decine di secondi.
10. **Invecchiamento**: +5 dB (Miasnikov) distribuito nella vita; resetta con la manutenzione.

---

## 8. Lacune e cose da non fidarsi

- Pompe RCP a due velocità del 688: nessuna conferma aperta. Il termine S6W appartiene al Seawolf; il 688 monta l'S6G.
- Transitori e tassi di commutazione degli ausiliari: nessun numero aperto.
- Direttività misurata di sottomarini: nessuna fonte aperta.
- Danni di scafo, allagamenti, alberi sbilanciati in dB radiati: nessuna fonte aperta; solo soglie di vibrazione.
- Identificazione di scafi sovietici da SOSUS: solo affermazioni qualitative.
- Valori forniti da siti non tecnici (forum, siti sonori, Grokipedia) sono stati scartati; i file GitHub di un gioco con «prua −5 dB, traverso 0, poppa −2,5 dB» sono dichiaratamente fittizi e non vanno citati come dati.
- Fonti che meritano un secondo passaggio se ci sarà accesso: Urick (1983) cap. 10; Burdic (1984); Stefanick (1987); McIntosh (2014); rapporto DSB 1968 «Submarine Noise Levels» (https://www.esd.whs.mil/Portals/54/Documents/FOID/Reading%20Room/MDR_Releases/FY11/11-M-1228_0001.pdf — il server rifiuta l'accesso automatico, 403); ICES CRR 209 (1995).

---

## 9. Bibliografia

### Libri e rapporti
- [R1] Ross, D. (1976). *Mechanics of Underwater Noise*. Pergamon Press, New York; ristampa Peninsula Publishing, Los Altos CA, 1987, ISBN 0-932146-16-3. Testo consultato: scansione DTIC ADA075101 (https://archive.org/details/DTIC_ADA075101), cap. 1.3, 8.2, 8.5–8.6, 9.2, 9.5, 10.1–10.3, App. B. Nota: la scansione contiene errori di OCR; formule riportate solo dove leggibili.
- [S2] Sponagle, N. (1988). *Variability of Ship Noise Measurements*. Defence Research Establishment Atlantic, Technical Memorandum 88/210, giugno 1988. DTIC ADA201248 (https://archive.org/details/DTIC_ADA201248).
- [S3] Shin, Y.S., Robinson, J., Rossano, G. (1991). *Submarine-Installed Machinery Monitoring and Diagnostics: A State-of-the-Art Review*. Naval Postgraduate School, NPS-ME-91-002. DTIC ADA242189 (https://archive.org/details/DTIC_ADA242189).
- [M2] Miasnikov, E.V. (1998). «What is known about the character of noise created by submarines?», Appendix 1 in *The Future of Russia's Strategic Nuclear Forces: Discussions and Arguments*, CAC/MIPT, Mosca. https://spp.fas.org/eprint/snf03221.htm
- [M1] Miasnikov, E.V. (1994). «Can Russian Strategic Submarines Survive at Sea? The Fundamental Limits of Passive Acoustics». *Science & Global Security* 4, 213–251. https://scienceandglobalsecurity.org/archive/sgs04miasnikov.pdf
- [S1] MIL-STD-1399(NAVY) Section 300B (24 aprile 2008), *Electric Power, Alternating Current*. https://atecorp.com/getmedia/b7b6c882-f8fe-450d-801d-70a783daecd8/mil-std-1399-300b.pdf
- [I1] Mitson, R.B. (2002). «Research Vessel Standards: Underwater Radiated Noise». ICES CM 2002/J:10. https://www.ices.dk/sites/pub/CM%20Doccuments/2002/J/J1002.PDF (cita ICES Cooperative Research Report No. 209, Mitson ed., 1995).
- [E2] EUROFLEETS2 D11.1 (2014), *Underwater radiated noise from research vessels*. https://www.eurofleets.eu/download/EUROFLEETS2_WP11_D11.1_031214_V1.3.pdf
- [X1] Xenthos (s.d.). *Radiated Noise Ranging for Submarines: A Discussion Paper on the Principles of Noise Ranging and System Implementations*. http://www.xenthos.com.au/Ranging%20Paper.pdf (letteratura grigia, in sé «commercial in confidence»).
- [H1] Howard, J. (LT, USN) (2011). «Fixed Sonar Systems: The History and Future of the Underwater Silent Sentinel». *The Submarine Review*, aprile 2011, Naval Postgraduate School. https://nps.edu/documents/103449515/0/HOWARDAPR2011.pdf/1219db41-a727-4940-ab0b-b953c54f6e01
- [A2] Moore, S. (2019). «Acoustic Signature Control on Maritime Platforms». *Proceedings of ACOUSTICS 2019*, Cape Schanck. https://acoustics.asn.au/conference_proceedings/AAS2019/papers/p72.pdf (cita Lurton 2002; Miasnikov 1998; McIntosh 2014).

### Articoli
- [T1] Trevorrow, M.V., Vasiliev, B., Vagle, S. (2008). «Directionality and maneuvering effects on a surface ship underwater acoustic signature». *J. Acoust. Soc. Am.* 124(2), 767–778. doi:10.1121/1.2939128. Copia DTIC ADA486708 (https://archive.org/details/DTIC_ADA486708).
- Trevorrow, M.V. (2021). «Evidence for non-Rayleigh characteristics in ship underwater acoustic signatures». *J. Acoust. Soc. Am.* 149(4), 2270–2282. doi:10.1121/10.0004132.
- [A1] Arveson, P.T., Vendittis, D.J. (2000). «Radiated noise characteristics of a modern cargo ship». *J. Acoust. Soc. Am.* 107(1), 118–129. doi:10.1121/1.428344.
- [N1] McKenna, M.F., Ross, D., Wiggins, S.M., Hildebrand, J.A. (2012). «Underwater radiated noise from modern commercial ships». *J. Acoust. Soc. Am.* 131(1), 92–103. doi:10.1121/1.3664100. https://www.cetus.ucsd.edu/docs/publications/McKennaJASA2012.pdf
- [G1] Gassmann, M., Wiggins, S.M., Hildebrand, J.A. (2017). «Deep-water measurements of container ship radiated noise signatures and directionality». *J. Acoust. Soc. Am.* 142(3), 1563–1574. doi:10.1121/1.5001063. https://pmc.ncbi.nlm.nih.gov/articles/PMC5612799/
- [SH] Scrimger, P., Heitmeyer, R.M. (1991). «Acoustic source-level measurements for a variety of merchant ships». *J. Acoust. Soc. Am.* 89(2), 691–699. doi:10.1121/1.1894628.
- Wales, S.C., Heitmeyer, R.M. (2002). «An ensemble source spectra model for merchant ship-radiated noise». *J. Acoust. Soc. Am.* 111(3), 1211–1231. doi:10.1121/1.1427355.
- Gray, L.M., Greeley, D.S. (1980). «Source level model for propeller blade rate radiation for the world's merchant fleet». *J. Acoust. Soc. Am.* 67(2), 516–522. doi:10.1121/1.383916.
- [MG] MacGillivray, A., de Jong, C. (2021). «A Reference Spectrum Model for Estimating Source Levels of Marine Shipping Based on Automated Identification System Data». *J. Mar. Sci. Eng.* 9(4), 369. doi:10.3390/jmse9040369.
- [C1] Chion, C., Lagrois, D., Dupras, J. (2019). «A Meta-Analysis to Understand the Variability in Reported Source Levels of Noise Radiated by Ships From Opportunistic Studies». *Front. Mar. Sci.* 6, 714. doi:10.3389/fmars.2019.00714. https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2019.00714/pdf
- [V1] Veirs, S., Veirs, V., Wood, J.D. (2016). «Ship noise extends to frequencies used for echolocation by endangered killer whales». *PeerJ* 4, e1657. doi:10.7717/peerj.1657.
- [Z1] ZoBell, V.M., Gassmann, M., Kindberg, L.B., Wiggins, S.M., Hildebrand, J.A., Frasier, K.E. (2023). «Retrofit-induced changes in the radiated noise and monopole source levels of container ships». *PLOS ONE* 18(3), e0282677. doi:10.1371/journal.pone.0282677.
- [W4] Wittekind, D., Schuster, M., Landsberg, N., Greitsch, L. (2017). «Analyzing underwater radiated noise of a 3600 TEU containership». *DAGA 2017*, Kiel. https://pub.dega-akustik.de/DAGA_2017/data/articles/000070.pdf
- Catineau, H., Cauchy, P., Robin, O., Gervaise, C., Mercure-Boissonnault, P., Lafrance, S., St-Onge, G. (2022). «Identification of Sources and Their Directivity in the Global Underwater Radiated Noise from a Merchant Ship». *Canadian Acoustics* 50(3), 108–109. https://jcaa.caa-aca.ca/index.php/jcaa/article/view/3872
- [J1] «Global Sound Absorption Prediction for a Composite Coating Laid on an Underwater Submersible in Debonding States». *J. Mar. Sci. Eng.* 11(9), 1671 (2023). doi:10.3390/jmse11091671.
- [D1] Articolo DRDO (Defence Science Journal) sull'inizio della cavitazione, +8–15 dB (non verificato): https://publications.drdo.gov.in/ojs/index.php/dsj/article/download/7885/4794
- [E1] ECO Magazine (2023). «Small Propeller Defects Can Result in Increased Underwater Radiated Noise» (studio Transport Canada / Memorial University / DRDC Atlantic / Dominis Engineering). https://ecomagazine.com/news/research/small-propeller-defects-can-result-in-increased-underwater-radiated-noise/

### Fonti web secondarie
- [W1] GlobalSecurity, «S5G Natural Circulation». https://www.globalsecurity.org/military/systems/ship/systems/s5g.htm
- [W2] GlobalSecurity, «Natural Circulation Reactor». https://www.globalsecurity.org/military/systems/ship/systems/natural-circulation-reactor.htm
- [W3] Amick, A., «How The Experimental USS Narwhal Ended Up Being The Navy's Stealthiest Nuclear Submarine», The War Zone (aggiornato 30/4/2020). https://twz.com/33244/how-the-experimental-uss-narwhal-ended-up-being-the-navys-stealthiest-nuclear-submarine
- Wikipedia, «S5G reactor». https://en.wikipedia.org/wiki/S5G_reactor
- [W5] Wikipedia, «Acoustical intelligence». https://en.wikipedia.org/wiki/Acoustical_intelligence
- Wikipedia, «Submarine incident off Kola Peninsula (1968)». https://en.wikipedia.org/wiki/Submarine_incident_off_Kola_Peninsula_(1968)
- [P1] Bangor Daily News (3/10/2010), «Maine shipyard repairs Navy subs after stealthy skin falls away». https://www.bangordailynews.com/2010/10/03/news/maine-shipyard-repairs-navy-subs-after-stealthy-skin-falls-away/ (riassunto da risultati di ricerca; articolo non letto per intero).
- Kratovil, J.B. (1988), tesi NPS su SUBTRAN (modello di rilevamento di transitori fra sottomarini), segnalata da risultato di ricerca; non letta. https://calhoun.nps.edu/entities/publication/c58de655-2cdb-4e90-9de1-ce6080945cf9
- Fonti citate indirettamente e non lette: Urick, R.J. (1983). *Principles of Underwater Sound*, 3ª ed., McGraw-Hill, ISBN 0-07-066087-5; Stefanick, T. (1987). *Strategic Antisubmarine Warfare and Naval Strategy*, Lexington Books; McIntosh, D.J. (2014), Inter-Noise 2014, Melbourne; Lurton, X. (2002), *An Introduction to Underwater Acoustics*, Springer; Duncan, F. (1990), *Rickover and the Nuclear Navy*, Naval Institute Press.
