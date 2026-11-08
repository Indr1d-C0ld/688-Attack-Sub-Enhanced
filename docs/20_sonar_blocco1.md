# 20 — Sonar, blocco 1: varietà civile, traffico lontano, frange sul LOFAR (piano)

*Disassemblatore, 28/10/2026. Stato: piano da approvare (principale), contratti da concordare con runtime, vista e kit.*

È il primo blocco dell'overhaul del sonar (memoria «sonar e radio: priorità»). Sono i punti 1, 2 e 3 della nota
`sonar-confronto-27-10.md`:
- **A**: navi civili generate per individuo;
- **B**: il traffico mercantile come contatti veri;
- **C**: l'interferenza di Lloyd e dei cammini multipli sul LOFAR.

Il classico non cambia: rilievo 0/110 e casi classici identici. Anche la scoperta non cambia: `valuta` resta
incoerente, e le frange sono solo di quello che si vede e si sente.

## 1. Che cosa c'è oggi

- **Civili:**
  - 4 firme (`mercantile`, `cargo`, `petroliera`, `peschereccio`) più `agi`;
  - diesel a righe fisse che non seguono i giri, rete 50 o 60 Hz secondo la firma;
  - per l'individuo ci sono solo le variazioni R6 (±1-3% sui giri, ±2,4 dB).
- **Traffico:**
  - solo il termine di Wenz nel rumore ambiente (`ClimaTeatro::traffico`, 0,3-1,0 per teatro);
  - le navi civili esistono solo se la missione le mette.
- **LOFAR:**
  - `spettro_interno` (`nucleo/analisi_acustica.cpp`) usa la perdita del livello A, interpolata su 7 nodi in log f;
  - il Lloyd del livello A è coerente solo sotto π/4 (il dipolo) e incoerente oltre, quindi senza frange;
  - i raggi sono incoerenti (C2 §7).

## 2. A — Navi civili per individuo

**Ricerca.** Il dossier `docs/ricerca/26_navi_civili_rumore.md`, solo da fonti aperte e senza scaricare file:
- livelli per tipo e velocità: Ross; Wales e Heitmeyer 2002; McKenna 2012 (misure per tipo); il modello JOMOPANS-ECHO
  di MacGillivray e de Jong 2021 (CC BY), che dà i parametri per categoria, la velocità e la lunghezza di riferimento;
- righe: Arveson e Vendittis 2000 (portarinfuse: asse, pala, accensioni del diesel, generatori);
- propulsione tipica degli anni '80 per tipo: diesel lento a 2 tempi in presa diretta, diesel medio a 4 tempi con
  riduttore ed elica a passo variabile, diesel-elettrico, turbina a vapore delle grandi petroliere;
- rete a 50 o 60 Hz secondo la bandiera.

**Tipi (12 circa):**
- **mercantili:** portacontainer, portarinfuse, mercantile generico;
- **petroliere:** grande (turbina o diesel lento), costiera o chimichiera;
- **altri:** traghetto ro-ro e passeggeri, rimorchiatore, rompighiaccio diesel-elettrico (Barents e Baltico), nave da
  ricerca con l'ecoscandaglio a 12 e 38 kHz;
- **pescherecci:** a strascico, da posta o palangaro, a circuizione, grande trawler sovietico (BMRT).

**Dati:**
- `tools/pipeline/civili.py` scrive la sezione `[civili.*]` di `data/motore/acustica.toml`;
- per tipo, gli intervalli dell'individuo: lunghezza, velocità di servizio, pale 3-6, cilindri, giri di progetto,
  generatori (numero e giri 720-1800), rete;
- per tipo anche il livello di riferimento, la classe visiva (modello) e le fonti con la confidenza.

**Modulo puro `nucleo/navi_civili.hpp`:**
- `FirmaRumore genera_firma_civile(const TipoCivile&, uint64_t seme)`;
- dà una firma completa con le righe:
  - asse, pala e armoniche;
  - accensioni del diesel principale: legate ai giri in presa diretta, fisse col passo variabile;
  - accensioni dei generatori, armoniche della rete, ausiliari;
- dà anche i punti velocità → livello (ECHO o Ross per lunghezza e tipo), la tonale e la TS;
- stesso seme, stessa nave, per tutta la missione e nel replay.

**Due estensioni compatibili di `nucleo/acustica.hpp` (mie):**
- `RigaFirma::segue_giri`: la riga scala con la velocità come asse e pala. Le firme di oggi hanno `false`, quindi non
  cambiano;
- `UnitaAcustica::firma`: un puntatore che, se c'è, sostituisce la ricerca per classe. Le firme individuali non
  entrano nel catalogo: la libreria e `confronta_firma` restano sulle classi.

**Libreria (S5b):**
- i tipi civili diventano voci con gli intervalli, per esempio «traghetto: pale 4-5, diesel medio 500-750 giri, rete
  50/60 Hz»;
- `voce_libreria` ed `elenco_libreria` le danno nella scheda «navale», sottocategoria «civile»;
- nessun cambio d'API per la vista.

## 3. B — Traffico come contatti veri

**Ricerca.** Nello stesso dossier 26:
- rotte e densità degli anni '80 per teatro, con fonti e STIMA:
  - **Gibilterra:** lo schema di separazione dello stretto;
  - **Celtica:** Ouessant e Western Approaches;
  - **Otranto:** Bari e Brindisi verso Igoumenitsa e Patrasso, la pesca a strascico italiana;
  - **Baltico:** i traghetti Rostock–Gedser e Sassnitz–Trelleborg, Kiel;
  - **Barents:** gli accessi a Murmansk e Kola, la pesca di Murmansk, i rompighiaccio d'inverno;
  - **Faroe:** le rotte nordeuropee e i pescherecci faroesi e scozzesi;
  - **Kunashir:** le flotte da pesca sovietiche e giapponesi, gli stretti delle Curili;
- partenza dal dossier 03 e dal 09 §5.7.

**Dati.**
- `tools/pipeline/traffico.py` scrive `data/motore/teatri/<teatro>.traffico.toml`, sul piano del teatro con la stessa
  proiezione dei toponimi;
- il file contiene:
  - **corsie:** spezzata, larghezza, senso unico o doppio, navi al giorno per senso, miscela dei tipi, velocità;
  - **zone di pesca:** poligoni con densità, mesi e comportamento (strascico a 3-4 kn, deriva, trasferimento);
  - **linee di traghetto:** corsie con l'orario.
- Le corsie si controllano col fondale (`AcqueNavigabili`): niente navi in secca.

**Modulo puro `nucleo/traffico.hpp`.** Ricalca il paesaggio sonoro, che funziona già così:
- `RotteTraffico carica_traffico(percorso)`;
- `popola_traffico(rotte, catalogo, mese, ora, centro, raggio, seme, id_base)` → `vector<NaveTraffico>`:
  - campo di ogni nave: id, tipo, classe visiva, pos, rotta, velocità, corsia e progresso, scarto laterale, seme
    dell'individuo;
  - distribuzione allo stato stazionario: Poisson lungo le corsie dalla densità;
- `muovi_nave(nave, rotte, dt)`: segue la corsia con virate dolci, esce a fine corsia; i pescherecci girano nella
  zona;
- `ricambio(...)`: le entrate dal bordo del disco quando il giocatore si sposta, deterministiche dal seme, come
  `passo_paesaggio`.

**Contratto col runtime.** La presenza nella scena è del runtime, e queste sono le proposte da concordare:
- **Unità:** unità vere in `unita_`, `categoria = "nave"`, lato neutrale (flag 0), classe = tipo civile, firma
  individuale con `UnitaAcustica::firma`.
- **Contatti e simmetria:** sono contatti come gli altri, per il giocatore e per l'IA (simmetria): sonar, radar di
  navigazione acceso (`radar_da_mercantile`, per l'ESM), periscopio e ROE mercantili.
- **Comportamento:** niente obiettivi né IA tattica, seguono `muovi_nave`; il runtime decide se reagiscono a
  un'esplosione vicina (fermarsi, accostare).
- **Raggio e tetto:** il disco è di 100 km attorno al giocatore, come il paesaggio, con un tetto di unità (proposta:
  40) per il costo.
- **Costo dei campi:** ogni sorgente in più chiede un campo di raggi. Propongo per il traffico il livello A, o un
  `ParametriRaggi` più rado oltre i 30 km, e lo fornisco io se serve.
- **Classi civili per nome:** `classe_civile()` (missione.cpp:2370) va generalizzata, per nazionalità «civile» o per
  un elenco dai dati, e non per nome.
- **Interruttore:** il traffico c'è solo nell'Enhanced, spento di base nelle scene di prova (`data/missioni/prove`),
  nel trailer e dove la missione dice `traffico = false`. Il Lua attuale non cambia.
- **Rumore lontano:** il Wenz del teatro resta, ma se le navi esplicite alzano il rumore medio di oltre 2 dB abbasso
  `ClimaTeatro::traffico` per non contarle due volte (dati miei).

**Concordato col runtime il 28/10:**
- **Chi decide:** il modulo decide e il runtime applica.
  - `popola_traffico` dà le navi iniziali; `ricambio` dà entrate e uscite, con l'uscita oltre il disco più un
    margine (110 km).
  - Il seme lo ricava il modulo dal seme della partita.
  - Il runtime crea le unità con id suoi (da 10000 in su, mai riusati), le marca neutrali (`neutrali_`), le toglie
    con `dimentica()` dalle cache e coi contatti degli osservatori. L'uscita non è un affondamento.
- **Solo sorgenti:** le navi del traffico non hanno sensori né IA, quindi il costo è osservatori × navi.
- **Raggi:** il livello A oltre i 30 km, e oltre i 30 km nessuna richiesta di campo. Il campo vero entro i 30 km con
  la cache asincrona. Le misure le fa il runtime sulle serie della sandbox (10 semi da 2 ore).
- **Esplosioni:** `NaveTraffico::fuga_da` e `fuga_fino_s`, accesi dal runtime entro ~10 km; `muovi_nave` accosta via,
  accelera e poi rientra in corsia.
- **Script:** la VM delle missioni (scelta del bersaglio, conteggi, `unita_esiste`) ignora gli id del traffico. È
  del runtime.
- **Classi civili:** `civile = true` per classe in `classi.toml`, scritto da `esporta_motore.py` (io, con le classi
  della parte A); finché manca, l'elenco per nome.
- **Regola `neutrali_a3`:** il traffico ne è escluso (proposta a del runtime, la decisione è della principale).

**L'aggancio fatto (runtime, 28/10, `missione_traffico.cpp`; suite 802/802, classico invariato):**
- **quando c'è:** `usa_traffico` è spento di norma; `traffico = false` della missione lo spegne sempre; l'app lo
  accende nell'Enhanced (`--traffico`, riga della sandbox, caso e5 della regressione);
- **unità:** id da 10000, flag 0x42 (nave, non combattente, neutrale). Non sono mai osservatori: il loro radar emette
  ma non scopre;
- **ricambio:** un'uscita toglie l'unità; una nave colpita chiama `perduta`; un rientro ha un id nuovo;
- **fuga:** un colpo entro 10 km chiama `fuggi` per 1800 s;
- **costo** (SCENE5A, 40 navi, 10 semi da 2 ore): rapporto 1,69 sul tempo senza traffico, con i sensori che valutano
  il traffico un turno su 4 e `avanza` a 1 Hz. Con tutto a ogni passo era 2,55. Il `ParametriRaggi` rado non è
  servito. Oltre i 30 km la perdita è quella del livello A;
- **rete e radar:** `passo_reti` usa `TipoCivile::strascico`; il radar `nav_civ` va alle classi nuove e ai
  pescherecci.

**Cosa tocca agli altri:**
- **Principale:**
  - i casi di regressione Enhanced (e1-e4) cambiano impronta se il traffico è acceso: nuove impronte, o il traffico
    spento nei casi;
  - nella vista i neutrali appaiono come contatti normali.
- **Trailer-1:**
  - la traccia v2 registra anche le navi del traffico, e la prova della dimensione (1,65 MB) va guardata;
  - il fotogramma delle neutrali senza contatti ogni 25 s, a carico mio in `traccia_missione` (concordato il 28/10):
    sotto i 30 s di `SENZA_INTERPOLARE_S`, così il replay le fa scorrere senza scatti.
- **Kit:**
  - i tipi nuovi senza modello usano il modello del tipo più vicino (rimorchiatore → peschereccio, traghetto e
    rompighiaccio → mercantile);
  - i modelli veri, se e quando li vuole fare;
  - le classi nuove vanno in `classi.toml` attraverso `esporta_motore.py` (file condiviso, lo annuncio).

## 4. C — Frange d'interferenza sul LOFAR

**Modello.**
- Nel modulo puro `nucleo/interferenza.hpp`: il rapporto coerente su incoerente delle immagini (metodo delle
  immagini):
  - diretto e superficie (Lloyd), più i rimbalzi sul fondo in acqua bassa fino al 4° ordine;
  - le ampiezze vengono da `perdita_fondo_db` (per angolo e frequenza) e dalla coerenza di Eckart della superficie
    con l'Hs.
- `F(f) = |Σ a_i e^{i ω τ_i}|² / Σ a_i²`, mediato sul bin: ogni coppia porta `cos(ω τ_ij) · sinc(π b τ_ij)`. Dove le
  frange sono più fitte del bin si spianano da sole.
- Ha anche lo sfumato del moto nel tempo d'integrazione e una perdita di coerenza con la distanza (STIMA).
- Il rapporto si applica sopra la perdita del livello A, quindi il livello medio non cambia:
  - sotto π/4 il dipolo è già nel livello A e il rapporto vale 1;
  - nei percorsi rifratti (condotto, CZ) non si applica.
- Ne viene l'invariante di guida β ≈ 1 in acqua bassa: le frange a «vasca da bagno» che si piegano al CPA.

**Dove entra:**
- `spettro_fascio`, `spettro_verniero`, `spettro_continuo` e `righe_ricevute`, sul continuo e sulle righe: le righe
  svaniscono e tornano con la distanza, come dal vero;
- il DEMON no, e nemmeno il BTR (un'ottava: si media) né `valuta`.

**Contratto con la vista (principale):**
- un argomento finale `bool coerente = false` sulle quattro funzioni;
- la stazione dell'Enhanced lo passa `true` in tutti e due i modi, perché è fisica e non un aiuto;
- con `false` gli spettri restano uguali bit per bit, quindi le prove di oggi non cambiano;
- costo: con 16 immagini, circa 120 coppie per bin e per sorgente; da misurare sulla stazione.

**Kit:**
- la pipeline dB → colore non cambia;
- le frange sono di 3-10 dB e si leggono meglio con più livelli: è un argomento in più per i livelli o il truecolor
  del punto 33.

## 5. File

**I miei:**
- moduli nuovi: `nucleo/navi_civili`, `nucleo/traffico`, `nucleo/interferenza` (hpp e cpp);
- prove nuove: `prove_navi_civili`, `prove_traffico`, `prove_interferenza`;
- pipeline e dati nuovi: `tools/pipeline/civili.py`, `traffico.py`, `data/motore/teatri/*.traffico.toml`;
- ricerca: `docs/ricerca/26_navi_civili_rumore.md`;
- moduli esistenti: modifiche in `nucleo/acustica` e `nucleo/analisi_acustica`, `tools/pipeline/acustica.py` e
  `data/motore/acustica.toml`;
- manuale: i capitoli C2, C4 e C8, più uno nuovo sul traffico.

**Condivisi, da annunciare:**
- `engine/CMakeLists.txt`;
- `tools/pipeline/esporta_motore.py` (le classi civili nuove);
- `engine/README.md`;
- `missioni/traccia_missione` (solo il campionamento delle neutrali, se trailer-1 è d'accordo).

**Non miei:** `app/sonar_enhanced` (principale); la presenza nella scena e `missione.*` (runtime); i modelli e il
manuale come cura (kit).

## 6. Ordine e prove

1. **Ricerca** (dossier 26) e questi contratti, approvati. Pochi giorni.
2. **A**, le navi civili: dati, generatore, `segue_giri` e `UnitaAcustica::firma`, la libreria. Prove:
   - firme ripetibili dal seme;
   - righe legate ai giri che scalano con la velocità;
   - livelli dentro le misure (McKenna, ECHO);
   - le firme di oggi uguali bit per bit;
   - suite verde e rilievo 0/110.
3. **B**, il traffico: dati e modulo. Il runtime aggancia appena c'è il modulo; il contratto si può agganciare anche
   prima, su un modulo vuoto. Prove:
   - densità allo stato stazionario;
   - nessuna nave in secca;
   - ripetibile;
   - casi classici identici.
4. **C**, le frange. Prove:
   - passo delle frange di Lloyd uguale alla formula;
   - media sulle frequenze entro 0,5 dB;
   - pendenza β ≈ 1 in acqua isoveloce bassa;
   - frange spente col mare forte;
   - con `coerente = false` tutto uguale.

C è indipendente: se la principale preferisce vedere prima le frange, la anticipo. Stima complessiva: 3-4 settimane di
lavoro mio, più l'aggancio del runtime per B.

## 7. D — La schiera trainata dei sottomarini: quota, fondo, reti (in coda al blocco, richiesta dell'utente del 28/10/2026)

Oggi solo le schiere delle navi (SQR-19, `missione_collisioni.cpp`) hanno la geometria del cavo, e il cavo che tocca il
fondo non ha conseguenze. La schiera dei sottomarini (TB-16 del 688, quelle sovietiche) non ha una quota sua. Il
classico non cambia: tutto quanto segue vale solo nell'Enhanced, per il giocatore e per l'IA (simmetria).

1. **Geometria del cavo** (disassemblatore, modulo puro): la quota della schiera da velocità, cavo filato, quota del
   battello e flottabilità di cavo e schiera (la TB-16: circa 800 m di cavo e 70 m di schiera; fonti e STIMA nel
   dossier). A bassa velocità la schiera scende sotto il battello: l'acustica riceve alla quota vera della schiera,
   che può stare sotto lo strato (tattica reale).
2. **Contatto col fondo** (runtime, con il rumore del disassemblatore): rumore di sfregamento sui fasci della schiera,
   righe spurie, avviso del sonarista; rischio di danno e di perdita che cresce con la velocità e col fondo duro
   (roccia, relitti, dal tipo di fondo del teatro); con un aggancio forte il cavo si spezza.
3. **Reti dei pescherecci al lavoro** (runtime, col traffico di B): la schiera che passa nella rete a strascico si
   danneggia o si perde, e il peschereccio se ne accorge (casi documentati).
4. **Mine:** niente innesco. Le mine di fondo sono a influenza e un cavo leggero quasi amagnetico non le attiva; le
   ormeggiate del gioco (CAPTOR, PMR) scoprono il battello da sole. Al più, raro, la schiera che s'impiglia
   nell'ormeggio di una mina ormeggiata: danno o perdita della schiera, come un aggancio.
5. **Vista** (principale): la quota della schiera nella stazione sonar e nella manovra, gli avvisi, l'icona della
   schiera danneggiata o persa. **Manuale** (kit).

Prove: quota della schiera contro velocità (curva monotona, valori del dossier); schiera sotto lo strato a bassa
velocità che sente un contatto che il battello non sente; nessun contatto col fondo in acqua profonda; rumore e danno
in acqua bassa a velocità bassa; classico identico e rilievo 0/110.

### 7.1 Contratto di D (disassemblatore, 28/10/2026; da approvare)

La fisica e i numeri sono già nel dossier 04:
- la quota viene dalla catenaria all'angolo critico (Pode), `w cos φ = R sin² φ`, più la flessione della schiera (§1.5,
  tabella da 2 a 20 kn; ±30%);
- fondo e reti: §2;
- parametri per sistema e regole di danno, tutte STIMA: §6.2 e §6.3.

Nel dossier aggiungo solo lo sfregamento sul fondo e la vibrazione del cavo.

**Modulo puro `nucleo/schiera.hpp` (mio):**
- **`DatiSchiera`** per sistema: TB-16, TB-23, sovietica (Skat), SQR-19. Ha:
  - cavo e schiera (lunghezze, diametro, peso in acqua, `C_n`);
  - flessione `vD`, velocità di filata e di recupero, velocità di manovra e massima, quota massima.

  Stanno in `[schiere.*]` di `acustica.toml` (`tools/pipeline/acustica.py`), con la chiave del sensore (`tb16`...).
- **`forma_schiera(dati, v_kn, quota_battello_m, cavo_filato_m)`** → `FormaSchiera`:
  - l'angolo del cavo;
  - la quota del centro della schiera (quella dell'acustica) e del punto più basso;
  - la distanza orizzontale dietro la poppa;
  - un profilo di 16 punti (distanza dietro, quota) per il fondo.

  Il cavo più corto del pieno scende in proporzione. La quota è limitata alla massima del sistema.
- **`CavoSchiera::avanza(dt, forma)`:** la quota non cambia di colpo. Va verso quella d'equilibrio col tempo del
  cavo, `τ ≈ (cavo + schiera) / v`, minimo 60 s (STIMA): rallentando la schiera affonda in qualche minuto.
- **`contatto_fondo(forma, profondita_dietro)`:** la frazione di schiera sul fondo e il tratto. Le profondità lungo
  la scia passata le dà il runtime, ai 16 punti del profilo.
- **Rumore di sfregamento:** `rumore_sfregamento_db(f, v_kn, frazione, TipoFondo)`. Sale con la velocità e col fondo
  duro (roccia > sabbia > fango), sopra i 20 dB sotto 500 Hz a pieno contatto (STIMA). Entra nel rumore proprio del
  sensore della schiera, da due campi nuovi di `Ricevitore` con i valori di default: `sfregamento` (0..1) e
  `fondo_sfregamento`. Ci sono anche le righe spurie: impulsi irregolari, come transitori del paesaggio.
- **In più (proposta, poco costo):** la vibrazione del cavo (*strum*). Il distacco dei vortici dà righe a
  `f = 0,2 v / d` (Strouhal) nel rumore proprio della schiera: cavo da 9,4 mm a 5 kn circa 55 Hz, che sale con la
  velocità. È una riga propria da riconoscere sul LOFAR, come quelle del battello (punto 5 della nota del 27/10).

**Il runtime:**
- il cavo filato nell'Enhanced, alle velocità di filata del dossier, invece dei timer del classico;
- il profilo del fondo lungo la scia e la quota della schiera nel `Ricevitore` del sensore della schiera, per il
  giocatore e l'IA;
- i danni e le perdite del §6.3: fondo, reti col traffico, eccessi di velocità, ormeggio delle mine;
- gli avvisi.

**La principale:** la vista. **Il kit:** il manuale.

**Le mie prove:**
- la quota contro la tabella del §1.5 entro il 20%, monotona con la velocità e limitata alla massima;
- il ritardo nel tempo;
- il contatto in 200 m d'acqua sotto i 5 kn col battello a 60 m, e mai in acqua profonda;
- lo sfregamento che cresce con la velocità e la durezza del fondo;
- le righe dello strum alla frequenza di Strouhal;
- `valuta` con la schiera sotto lo strato che sente un contatto che il sensore di scafo non sente;
- il classico identico e il rilievo 0/110.
