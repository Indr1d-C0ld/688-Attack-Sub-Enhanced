# F8. La sandbox

*Autore: kit (curatore), dal codice di principale, runtime, trailer-1 e disassemblatore. Revisione del 29/10/2026.
Stato: rivisto (curatore: kit); V1, V2 e V3 (serie, vetrini pronti e piano delle originali) agganciate.*

## 1. A che serve

La sandbox è una partita senza giocatore. Si sceglie una missione, se ne regolano le parti e la si guarda svolgersi.
Il battello lo comanda un comandante IA con gli stessi sensori e le stesse regole di tutti, e lo spettatore vede la
verità che nessuna unità vede. Serve a vedere le dottrine al lavoro e a misurarle: chi ha scoperto chi e quando,
quanto ci ha messo a reagire, quanto è stata buona la sua TMA, quanto hanno reso le armi. Il piano è
`docs/19_sandbox.md`; il volume 1 la spiega al capitolo 12.

## 2. Il modello

### 2.1 La partita senza giocatore

- **Avvio** (`app/main.cpp`). V nella scelta della missione (`scelta_missione.cpp`) apre `SchermataRegolazioni`
  (`app/regolazioni_sandbox`), poi parte la partita con `o.sandbox`. Da riga di comando: `--sandbox`, che impone la
  regola Enhanced; `--codice` accetta anche il codice sandbox.
- **La missione** gira con `Missione::senza_giocatore(true)`, che `Missione::sandbox()` rilegge:
  - fuori carriera: niente fascicolo, albo, riconoscimenti, esito e cerimonie, e nemmeno le statistiche cumulate,
    globali o del comandante. La traccia va fra i replay liberi (gli ultimi 20);
  - il registro racconta il battello in terza persona (capitolo F1);
  - il comandante IA (`missioni/comandante_ia`, capitolo E6 §2.17) comanda il battello con gli `Ordine` del giocatore,
    che passano per il registro degli ordini. Il suo livello è quello della parte del battello (il Patto se
    `flag & 0x10`, se no la NATO).
- **Le regolazioni** si applicano prima del primo passo:
  - `Missione::regolazioni` per parte (`RegolazioniParte`: situazione, ROE, livello, allerta, carico; capitolo E6
    §2.17);
  - `Missione::interruttori_ia` per gli interruttori (capitolo E6 §2.18);
  - `registra_statistiche(true)` per le liste R1-R6 (capitolo F1 §2.1).
- **La fine:** la partita finisce alla durata massima (`durata_min`), all'esito se `ferma_all_esito`, o con Esc;
  senza finestra allo stesso modo.
- **Il traffico civile** si spegne comunque nelle scene di prova (`prove/`, `PROVA_`).

### 2.2 Le regolazioni

`RegolazioniSandbox` (`app/regolazioni_sandbox.hpp`), 29 righe. La schermata è nella variante A della rifinitura
estetica (capitolo E9 §2.9): le tre parti in colonna, la partita, le parti dell'IA come levette con la spia; ciano il
valore cambiato rispetto ai predefiniti, giallo la riga scelta; il piede comune (Esc, S, Invio). Su e Giù scelgono la
riga, Sinistra e Destra cambiano il valore; un clic sulla riga scelta lo cambia.

| Campo | Valori | Predefinito |
|---|---|---|
| per parte (`nato`, `patto`, `neutrali`): `situazione` | missione, pace, crisi, guerra | missione |
| `roe` | missione, trattenute, libere | missione |
| `livello` | missione, recluta, regolare, esperto | missione |
| `allerta` | -1 (della missione), A0-A3 | -1 |
| `carico_per_classe` | falso, vero | falso |
| `meteo_realistico` | vero: del teatro e della stagione; falso: mare 3, sereno | vero |
| `navigazione_realistica` | vero: realistica; falso: perfetta | falso |
| `anni_89` | vero: stazioni e sensori 1989; falso: moderni | falso |
| `seme` | vuoto (nuovo a ogni partita), poi 1, 2, 3... | vuoto |
| `durata_min` | da 15 min a 24 h, a quarti d'ora | 120 |
| `ferma_all_esito` | | vero |
| `ferma_eventi` | in finestra la compressione torna a 1× a ogni `lancio_giocatore`, `lancio_nemico`, `lancio_ia`, `colpo`, `affondamento`, `emersione_emergenza`, `mina_esplosa` (`main.cpp`); oggi senza effetto nella vista dal vivo (§7) | falso |
| `traffico` | il traffico civile dei teatri (capitolo E10); spento comunque nelle scene di prova | vero |
| `difesa`, `armi`, `reazioni`, `forze`, `esm_ia`, `sonaristi_ia` | interruttori dell'IA | tutti accesi |

### 2.3 Il codice sandbox

`scrivi_codice_sandbox` e `leggi_codice_sandbox` (`app/codice_partita`): 30 caratteri, 150 bit.
- **La base** è comune al codice partita: missione, seme, opzioni (meteo, navigazione, epoca...).
- **Per ogni parte:** situazione 2 bit, ROE 2, livello 2, allerta 3, carico 1.
- **Il resto:** la durata in quarti d'ora (7 bit, da 1 a 96), «fermarsi all'esito» e i sei interruttori (7 bit),
  `ferma_eventi` (1 bit, preso dalla riserva: i codici di prima lo leggono spento), `traffico` (1 bit, 1 = senza,
  preso dalla riserva: i codici di prima lo leggono acceso), la riserva fino a 138 bit e un CRC a 12 bit.
- **La base** del codice sandbox non porta il bit del traffico del codice partita: lo porta questo campo. Meteo,
  navigazione, sensori e seme del vetrino stanno nella base.

Il codice sandbox non ha una versione propria: usa quella del codice partita (`VERSIONE_CODICE_PARTITA`) e si
riconosce dalla lunghezza (voluto). P nella scelta della missione lo legge e riapre le regolazioni compilate; da riga
di comando `--codice` lo gioca, e `--seme` vince sul seme del codice, che è come le serie giocano N semi dello
stesso vetrino. I codici delle missioni di prova (`data/missioni/prove`) si ritrovano da `--codice` e dal menu.

Le regolazioni per parte, la durata e gli interruttori non hanno opzioni proprie da riga di comando: la strada è il
codice sandbox, che le porta tutte. `--sandbox --missione X` senza codice usa i predefiniti.

### 2.4 La vista dal vivo

`SchermataReplay` con `dal_vivo(true)` (`app/replay`, di trailer-1), al posto delle stazioni.
- **I dati:** riceve ogni fotogramma che la partita scrive nella traccia (`dopo_fotogramma`). Oltre l'ultimo
  campione le unità avanzano a stima, per al più 6 s.
- **Il tempo:** lo fa avanzare `main`, da 1× a 256× (`RichiesteVivo`).
- **Le letture**, con V:
  - `verita`;
  - `parte_nato`, `parte_patto`: il quadro della parte, record P della traccia (`quadro_parte`, `VoceQuadro`);
  - `unita`: i contatti di un'unità, record K, con la sua TMA e i tubi (`tma_ia`, `tubi_carichi`).

  Nelle letture di parte C mette sotto, in grigio, la verità degli altri.

  Nel replay del dopo le letture sono solo `verita` e `quadro`.
- **Il registro:** dai record E (gli eventi del registro, `testo_it`) e da alcuni fatti I: traccia solida, contatto
  perso, aggancio, esca, esplosione e bersaglio perso dei siluri.
  - Le righe uguali della stessa unità entro 60 s dall'ultima uguale si fondono, con «(XN)».
  - Il filtro per parte e il filtro per tipo dividono le righe per sottostringa del tipo d'evento. Le classi sono:
    scoperte, armi, manovre, allerta e radio, comandante.
- **I simboli:** quelli di `grafica/simboli_carta` (capitolo E9; volume 1, §5.6).
- **L'aspetto:** la vista dal vivo tiene la disposizione di prima (testata, carta, colonna del registro, linea del
  tempo); la variante C della rifinitura estetica vale per il replay del dopo, per le statistiche e per le serie (capitolo
  E9 §2.9), non ancora per la vista dal vivo.

### 2.5 Le statistiche tecniche

`statistiche_sandbox(traccia)` (`nucleo/statistiche_sandbox`, del disassemblatore) e la schermata
`SchermataStatisticheSandbox` (`app/statistiche`), che si apre con S dal debriefing dello spettatore.

**Le fonti:**
- i campioni U della traccia, con gli alberi alzati, e i loro record V (stima della posizione, ellisse, allerta,
  emissioni). I record A (la TMA in corso e le armi a bordo) servono alla lettura «unità», non alle statistiche;
- gli eventi E del registro;
- i fatti I della raccolta e delle liste R1-R6: `contatto_nuovo`, `contatto_solido`, `possibile`, `lancio`,
  `evasione`, `emissione`, `emissione_fine`, `punto_nave`, `siluro` (`contatto_perso` serve al registro della vista
  dal vivo, non alle statistiche);
- i quadri del giocatore.

I record P e K non servono alle statistiche. Senza record I (una traccia vecchia) le scoperte ricadono sugli eventi
`scoperta_ia` e `scoperta`, e la schermata lo avverte.

**Le definizioni:**
- **Catena di scoperta**, per ogni coppia osservatore-bersaglio di parti diverse (armi escluse):
  - il contatto, col sensore e la distanza vera, dal primo `contatto_nuovo`; il solido dal primo `contatto_solido`;
  - il possibile dal primo `possibile` di qualunque sensore;
  - la classificazione dalla prima `classificazione_ia` (o `classificazione` del giocatore), giusta o no.
  - **Il ritardo** è contatto meno possibile: quanto ci ha messo l'osservatore a scoprire quello che la fisica gli
    permetteva già.
- **TMA**, dagli eventi `tma_ia`: le gambe; la soluzione (codice 1), dal primo contatto; la rinuncia (codice 2). L'errore
  al primo lancio su quel bersaglio è la distanza fra la posizione stimata nel lancio (R3) e la vera, più gli errori
  di rotta e velocità. Per il giocatore, senza R3, si usa la soluzione proposta del MATE.
- **Armi:** ogni unità di categoria arma che non sia un'esca.
  - La durata va dal primo all'ultimo campione; il filo è il tempo con `collegata` valida.
  - L'esito è uno di questi: a segno (un `colpo` con quell'arma), uccisione (l'ultimo colpo prima dell'affondamento),
    esca (sedotta), esaurita, in corsa alla fine, se no mancata.
  - Le cariche e i razzi senza unità d'arma contano come un attacco per ogni `colpo`, a segno se il colpo ha fatto
    danno (una carica fallita esplode con danno 0). Allo stesso modo contano i colpi d'artiglieria fra unità dell'IA,
    col pezzo come classe d'arma: il registro scrive il `colpo` anche per i colpi mancati, con valore 0, e il
    debriefing li dice «Colpo mancato».
- **Pk**, per parte e per classe d'arma: a segno su lanciate. La parte di un'arma è quella del lanciatore. Un'esca è
  efficace se ha sedotto un siluro.
- **Reazioni**, solo fra parti ostili: NATO e Patto fra loro, il luogo con l'una e con l'altra (la traccia non ha la
  situazione di ogni coppia: è una stima):
  - scoperta: il primo contatto su un ostile;
  - allerta: la prima `allerta_forza` in salita;
  - ingaggio: `tma_ia`, `ingaggio_ia`, o il comandante in TMA o in attacco;
  - lancio: `lancio_ia` o `lancio_giocatore`;
  - minaccia: il primo contatto su un'arma ostile;
  - evasione: `evasione_ia`, `esca_ia`, `ritirata_ia`, il comandante in difesa, o R4, il primo.
- **Esposizione:**
  - il tempo con periscopio, antenna o schiera fuori, dai campioni, senza i buchi oltre 30 s;
  - radar, ping e radio da R5; senza R5 il radar viene dalle emissioni dei campioni (record V), ping e radio dal
    registro (`ping_attivo`, `trasmissione`, `messaggio_ia`);
  - la contro-scoperta: un contatto nuovo di un'altra parte via ESM, o mentre l'unità emetteva, o entro 60 s da un
    suo ping.
- **Navigazione:** l'errore fra la posizione stimata e la vera, medio nel tempo e massimo, e i punti nave (eventi
  `punto_nave` per il giocatore, R6 per l'IA).
- **Per parte:** le perse sono gli affondamenti; le inflitte vanno a chi ha sparato l'ultimo colpo. Lo scambio è
  inflitte su perse (le inflitte se non ne ha perse). Le scoperte e le subite contano solo fra parti ostili. Le parti
  sono NATO (ciano), Patto (rosso), neutrali (verde) e, nelle missioni che la hanno, la parte del luogo, «LUOGO» in
  giallo (vedi §7).

**La schermata** (variante C della rifinitura estetica, capitolo E9 §2.9): la consolle «SALA ANALISI - STATISTICHE
DELLA SANDBOX» coi pezzi di `grafica/quadro`. Cinque schede (riepilogo per parte, catene di scoperta, armi, reazioni e
TMA, esposizione e navigazione), con Tab o un clic sulle linguette; le frecce scorrono le tabelle; le righe alterne
sono velate. La pagina 1 ha due schermi: a sinistra la tabella, a destra «IN FIGURA» (scoperte fatte e subite, lanci
ed esche, i tempi medi dalla scoperta all'allerta, alla soluzione e all'ingaggio). Le pagine 2-5 hanno uno schermo
largo. Il piede comune ha TAB PAGINA SEGUENTE ed ESC INDIETRO, anche al clic. Le schede sono descritte nel volume 1
§12.6. Un clic su una riga con l'ora apre il replay da lì (`SchermataReplay::vai`).

### 2.6 Le serie

**Lo strumento**, `strumenti/serie_sandbox` (trailer-1):

```
serie_sandbox --semi N [--da S] [--lavori J] [--uscita CARTELLA] [--secondi T] [--motore melchior688]
              (--codice <codice sandbox> | -- <argomenti di melchior688 --sandbox>)
```

- **Come gioca:** ogni partita la gioca `melchior688 --sandbox --codice ... --seme k --salva-traccia` in un processo
  suo, così è identica al vetrino col suo codice e la preparazione della sandbox resta una sola, in `main.cpp`.
  `--secondi` è solo un tetto (di norma 4 ore): la partita finisce da sé all'esito o a `durata_min` delle regolazioni,
  120 minuti se nessuno li cambia, anche con un `--secondi` più lungo e anche se la missione ha una durata scritta
  maggiore. Per giocarla tutta si alza `durata_min` nel codice. `--da` vale 1,
  `--lavori` i processori meno uno.
- **Il riassunto:** per ogni traccia calcola `statistiche_sandbox`, che dà anche l'esito della partita (`esito`,
  `esito_testo`), poi le aggrega con `riassunto_partite`:
  - per ogni numero media, deviazione, minimo, massimo, q10, mediana, q90 ed n, cioè le partite che hanno il valore;
  - le frequenze degli esiti, con le partite non finite (esito -1);
  - il Pk per parte e per classe d'arma, sommato sulle partite.

  I numeri per parte (`StatParte`, `numeri_parte`) sono: scambio, perse, inflitte, scoperte e subite, ritardo e
  distanza media di scoperta, lanci e a segno, esche, i tempi medi di reazione, ingaggio, lancio, soluzione ed
  evasione, le gambe medie, l'errore al lancio, l'esposizione (periscopio, antenna, radar, ping, radio,
  contro-scoperte), l'errore di navigazione e i punti nave. Scrive `serie.toml` (formato in `app/serie_dati.hpp`):
  - `[serie]`: missione, codice, primo seme, semi, durata, data;
  - `[esiti]`: per codice d'esito, partite, testo e frequenza;
  - `[parti.nato]`, `[parti.patto]`, `[parti.neutrali]`: il riassunto di ogni numero;
  - `[pk.X]`: per parte e per classe d'arma, a segno e lanciate;
  - `[[partita]]`: seme, esito, traccia, e i numeri della partita per parte (-1 se mancano).

  Accanto ci sono le tracce `partita_N.m688tr` e l'uscita di ogni partita, `partita_N.txt`. Dal gioco c'è anche
  `serie.txt`, l'uscita dello strumento.
- **L'esito:** 0 se tutte le partite hanno la traccia, 1 altrimenti. Le partite mancanti non entrano nel riassunto, e
  i semi del riassunto devono essere di seguito: dopo un buco il riassunto si ferma.

**Dal gioco** (S nelle regolazioni, `main.cpp`):
- 10 semi, dall'1 al 10, con `--lavori` pari ai processori meno uno, al più 4, e `--secondi 90000`; intanto la
  finestra conta le partite fatte;
- la cartella è `serie/<missione>_<data e ora>/` nella cartella dei dati del gioco (`~/.local/share/melchior688`,
  accanto a `replay/`);
- finita la serie si apre la schermata; `melchior688 --serie CARTELLA/serie.toml` la riapre, e con `--pagina K
  --istantanea X` la disegna senza finestra.

**La schermata** (`app/schermata_serie`, trailer-1) è nella variante C della sala analisi (capitolo E9 §2.9.3): la
consolle «SALA ANALISI - SERIE DELLA SANDBOX», lo schermo con la testata (missione, partite e semi) e le linguette,
il piede comune con la riga d'aiuto «TAB PAGINA  FRECCE NUMERO O SCORRONO  CLIC SU UNA PARTITA: IL REPLAY» e due
tasti, TAB PAGINA SEGUENTE ed ESC INDIETRO, anche col clic; Tab, il clic sulle linguette, Esc e Invio come nel volume
1, §12.8. La NATO è in ciano chiaro (indice 11), il Patto in rosso chiaro (12). Ha tre pagine. I grafici sono del
kit (`grafica/grafici_serie`):
- **esiti:** `barre_esiti`, col codice e il nome dell'esito e «volte SU totale», perché il font del 1989 non
  disegna il «%»; sotto, il Pk per arma di ciascuna parte («a segno SU lanciate», sommato sulle partite), il codice e
  la data;
- **per parte:** `riga_dispersione` per ogni numero di `numeri_parte`, NATO e Patto sulla stessa scala: media ± dev,
  min-max, e un grafico piccolo con la linea min-max, la barra p10-p90 e la tacca della mediana. La tabella scorre
  con le frecce (una «V» o una «^» dice che c'è altro); in fondo resta fissa la sintesi della pagina 1: l'esito più
  frequente, il Pk per arma e il codice della serie (scelta dell'utente del 28/10/2026);
- **istogrammi:** `istogramma` per parte del numero scelto (Su e Giù). Ha 10 classi, o una per intero coi conteggi
  piccoli, la mediana tratteggiata e la fascia p10-p90 sotto l'asse. Sotto, `barre_partite`: una barra per partita,
  e il clic apre il replay della sua traccia.

I percentili sono interpolati linearmente fra i valori ordinati; la deviazione è quella del campione. I valori
mancanti (-1 nel file) si saltano.

### 2.7 I vetrini pronti

`data/missioni/prove/vetrini.toml`, scritto da `strumenti/scene_interazioni` (trailer-1) insieme alle scene. Ogni voce
ha la missione, il titolo e il testo che dice che cosa mostra (in italiano e in inglese), `pronto` (falso: in
ritaratura, e la voce non compare), la durata e le regolazioni consigliate, cioè i campi di `RegolazioniSandbox`; i
campi assenti restano quelli predefiniti. Oggi l'app (`leggi_vetrini`) ne legge `durata_min`, `meteo_realistico`,
`navigazione_realistica`, `anni_89`, `ferma_all_esito` e `ferma_eventi`; una voce senza il file della missione non
compare.

La scelta della missione (`app/scelta_missione`) li mostra nella scheda Editor, dopo «Nuova missione» e «Da una
originale», con la scritta verde «VETRINO DELL'IA». V, G, Invio o il clic aprono le regolazioni compilate, poi il
vetrino. L'elenco dei vetrini,
coi loro testi, è nel volume 1, §12.8. Dal 28/10/2026 tutti e nove sono pronti: S1 ha la traccia solida fra 1 e 6
minuti, e in S6 l'SSN attende sul fango della scarpata a 70,1N 32,4E; nelle due ore trova il Typhoon sui semi 1 e 2,
sul 3 no.

### 2.7b Il vetrino di Dio

Il progetto è in `docs/28_vetrino_dio.md`. Gli interventi sono il catalogo di `missioni/src/missione_interventi.cpp`
(`catalogo_interventi`):
- forze: `unita_nuova`, `togli`, `ordine`, `cambia_parte`, `situazione`, `roe`, `allerta`;
- guasti: `macchine`, `sensore`, `tubi`, `timone`;
- ambiente: `meteo`, `ora`, `strato`, `ghiaccio_deriva`, `polynya`;
- comunicazioni: `disturbo_radio`, `ordine_falso`;
- inganno: `incidente`, `false_flag`, `spia`, `contatto_falso`.

Il pannello è `app/pannello_interventi.cpp`. Ogni intervento finisce nel registro e nella traccia (record X, con la
fase: programmato, applicato, finito, fallito), e il replay lo ridisegna. I segni sulla carta sono in `grafica/simboli_carta`
(`disegna_intervento` per famiglia, `disegna_doppia_freccia` per il falso contatto, dalla sorgente vera a dove la si crede, `disegna_portata_ridotta` per il
sensore guasto); il pannello sta nella cornice `pannello_interventi` di `grafica/cornici`. Il colore degli interventi
è il magenta chiaro (indice 13).

Il falso contatto è una `UnitaAcustica` con una fedeltà da 0 a 1 (capitolo C5 §2.7 e §2.11): righe che mancano o si
indeboliscono, niente pale sotto 0,7, l'eco di un'esca.

**Gli esperimenti (D2).** Il file `.m688v` (`app/esperimento.hpp`, in TOML) porta il codice sandbox e gli interventi
programmati. La serie A/B (`app/esperimento_ab.hpp`, `strumenti/serie_ab`) gioca le coppie con e senza interventi sugli
stessi semi. La schermata del confronto (`app/schermata_confronto`) apre i replay affiancati (`app/replay_affiancati`). Le
due carte si muovono insieme, con I e O, la rotella e il trascinamento. Sulla linea del tempo le tacche sono gli interventi
applicati di B, a tutta altezza, col colore della famiglia, e una riga gialla segna il punto di divergenza.

La false flag (D4) dà al siluro dell'attaccante vero la parte di un altro: chi è colpito attribuisce l'attacco alla
parte creduta (`missione_forze.cpp`, eventi `attribuisce_*`).

**La spia e chi la riceve (D3).** Finché dura, ogni 60 s (**STIMA**) la parte riceve il quadro vero dell'altra, come rapporti
con la posizione e la classe vere. Navi e aerei lo hanno subito. I sottomarini, fuori dalle missioni originali, lo
ricevono dal canale: al ciclo della parte, a quota d'antenna, con la perdita e il ritardo del disturbo (capitolo E6 §2.8).
Il dato li orienta, non basta per un siluro. Nella M3 la spia fa tirare lo Slava; nella S5 il Victor non sale e non cambia
nulla; la M4 non misura la caccia sul dato (capitolo E6 §7). Le cifre sono nel capitolo T12 §10.

**Il cambio di parte (D4).** `cambia_parte` (FORZE, istantaneo, solo navi e sottomarini) cambia subito i bit della parte
(`flag`), per lo spettatore e per l'unità. Il testo `parte` è la parte nuova, `annuncio` chi lo sa: `nuova_parte`
(predefinito), `tutti` o `nessuno`. Chi non lo sa la crede della parte di prima (`Missione::parte_nota`): la usano gli
ostili, la parte creduta (capitolo E6 §2.4) e `f_bersaglio_nemico` degli script. Si viene a sapere dall'annuncio o se
l'unità attacca: a un lancio o a un colpo con bersaglio, la vittima lo sa subito e la sua parte dopo `formazione_s`
(evento `cambio_saputo`). Quando lo sa tutta una parte, NATO o Patto, c'è una reazione `cambio_saputo_usa` o
`cambio_saputo_urss`, una volta sola per parte: il replay ne ricava la scritta CREDUTA (`app/parti_credute.hpp`). La traccia
scrive il flag di un'unità una volta sola, quindi il replay legge la parte nuova dal record X. `Missione::parte_creduta(osservatore, unita)` dà quella parte allo spettatore.
Limiti: `quadro_parte` e i confronti diretti dei bit (radar, periscopio, registro) usano la parte vera; gli scoperti di prima
non si rifanno; un secondo cambio riparte da capo; il battello del comandante non si cambia (l'intervento fallisce).

**Gli interventi a t=0.** `situazione`, `allerta`, `meteo`, `ghiaccio_deriva`, `polynya`, `false_flag`, `incidente` e `cambia_parte` aspettano il loro
sistema al più 2 s (`ATTESA_MONDO_S`), poi falliscono col motivo; con le forze spente falliscono subito.

**I segni delle comunicazioni** (`app/replay.cpp`, nelle letture dello spettatore, dagli eventi `reazione_ia` con `noto`
falso, solo con gli interventi):
- `messaggio_perso` (`altra` −1 per il FLASH): MESSAGGIO PERSO in grigio sotto l'unità, per un minuto;
- `localizza`: per due minuti la linea gialla tratteggiata «DF» da chi localizza a chi trasmette. Con `unita` −1, la rete
  strategica, niente linea, perché il suo fix sbaglia di 65-185 km (capitolo C11 §2.9): DF DELLA RETE in giallo sull'unità.

**Lo strato e il ghiaccio (D4; il modello in C2 §2.8 ed E11 §2.1).**
- `strato` (testo `modifica`: `sposta`, `togli`, `rinforza`, `canale`; `quota_m` o `fattore`, `transizione_s`, durata; col punto e
  il raggio una zona, senza tutto il teatro). Allo scadere della durata torna indietro con la stessa transizione. Il profilo
  dipende dal teatro e dal mese: nelle acque polari fallisce col motivo (`strato_applicabile`);
- `ghiaccio_deriva` (`rotta`, `velocita_kn`, durata). Alla fine la deriva dell'ultimo ancora attivo, se no quella del campo;
- `polynya` (punto e raggio obbligatori; testo `azione`: `apri` o `chiudi`; `transizione_s`, durata). Sul battello a galla o a
  quota periscopica dove si chiude arriva un avviso, una volta sola: per lo spettatore la reazione `ghiaccio_nuovo` nel
  registro, per il battello del comandante il messaggio 499. Nessun danno: a ghiaccio fatto vale `sotto_banchisa`. Un battello
  dell'IA non manovra per l'avviso.

La SLD di chi sceglie una quota (ASW, VDS) e la copertura di chi guarda se c'è ghiaccio (diesel, ASW, conti) passano da
`sld_in` e `copertura_ghiaccio`. La vista del periscopio segue il campo vero (`app/vista_ghiaccio.hpp`); la carta del giocatore
resta il clima, e la polynya la sa solo dalla volta che misura; la carta dello spettatore la disegna dai record X (`polynye_a`,
capitolo 12), anche nel replay da file, col pack attorno (`griglia_ghiaccio` nei metadati: E11 §2.1).

**Il profilo del suono per lo spettatore.** Solo nel vetrino: `metadati_conti` scrive `mese_acustica` nei metadati della traccia, e
dal vivo lo passa main alla prima immagine. Dal clima del teatro e dai record X, `profilo_spettatore` (`app/segni_ambiente.hpp`)
rifà il profilo nel punto dell'unità, con gli stessi passi di `Missione::profilo_in`: quello del mese, poi gli strati la cui zona
contiene il punto, a 4 passi, all'andata e al ritorno. **Limite:** il profilo è senza l'effetto pomeriggio, che dipende dall'ora,
dal vento e dalle nubi di ogni istante e nella traccia non c'è.

### 2.8 Le originali

Le 22 missioni originali non hanno gli obiettivi dell'editor, e le loro aree non bastano al comandante. Per queste c'è un
piano per missione in `data/motore/sandbox_originali.toml`, generato da `tools/pipeline/sandbox_originali.py` (non si
modifica a mano). Ogni voce cita la frase del briefing e il gestore dello script, o l'unità amica, da cui vengono le
coordinate. Il piano non usa mai le posizioni dei nemici, che cambiano fra le varianti. Usa solo il briefing, i
rettangoli degli script e le rotte delle unità amiche, che sono uguali in tutte le varianti.

- **Campi:** la condotta; le armi (`libere`, `roe`, `solo_difesa`, `classi`); quota e velocità; i punti, l'area,
  l'uscita e il rettangolo da cui non uscire (`limite`); la rotta pianificata del convoglio scortato (PIM); distanza
  e impulso del pedinamento; l'attesa; il carico dei tubi.
- **Condotte:** caccia (8 missioni), scorta (6), pedinamento (3), evasione (2), transito (2), attacco_terra (1, la
  0A).
- **Il caricatore** (`missioni/sandbox_originali.{hpp,cpp}`):
  - `piano_originale(cartella_dati, id)` legge il piano della missione, che `main.cpp` passa al comandante come
    `OpzioniComandante::piano`;
  - `applica_tubi_piano(sala, piano)` carica i tubi prima del primo passo, sopra il carico per classe o quello
    iniziale.
- **Il comandante col piano** (`comandante_ia.cpp`):
  - transito sui punti, seminando chi pedina: uno scatto lungo la rotta, dall'altra parte dello strato e non verso la
    costa, poi deriva;
  - scorta in stazione davanti alla PIM;
  - pedinamento con la TMA, senza impulsi, sul contatto designato;
  - evasione a scatti e derive;
  - l'attesa;
  - il rettangolo rispettato anche nella fuga da un siluro.
- **La 0A.** I quattro TASM sono nei tubi da siluro, la dottrina dei 688 senza VLS (sul Flight II nei pozzi). Finché
  ne restano, il comandante non fugge: lancia l'esca e i missili, da quota periscopica col periscopio alzato, e
  non arma due volte lo stesso tubo. Poi fugge per acqua navigabile verso il rettangolo di Copenaghen. Usa il TASM
  perché il TLAM-C non usa il gestore del Tomahawk antinave (§7).
- **Le correzioni trovate qui** valgono anche senza piano e nelle scene S:
  - quota e giri con la dinamica dell'Enhanced;
  - la quota periscopica a 6,096 m, la scala della missione;
  - l'antenna rialzata se l'Enhanced l'ha ammainata;
  - il broadcast al più ogni 30 minuti;
  - la parte dei contatti non ancora classificati, presa dal colore della carta;
  - la chiglia vera tenuta a 5 m dal fondo e le secche evitate entro 2 km;
  - le tappe lunghe per acqua navigabile (`nucleo/rotte`);
  - la fuga solo da un'arma entro 5 km di cui si sa la posizione.

**Risultati** (`tools/taratura/risultati/originali_v3/`, 22 missioni × 3 semi, 2 ore). Col piano: 13 successi, 3
parziali, 17 fallimenti, 6 battelli persi, 27 partite senza esito. Senza piano: 14 successi, 2 parziali, 19 fallimenti,
5 battelli persi, 2 richiami, 24 senza esito. Il piano non alza le vittorie, cambia la condotta.
- **Vinte sempre:** 8A, 8C, 9A.
- **Perse sempre:** 1A (tre cacciatorpediniere in 15 minuti), 2A (seminare l'Alfa in 20 minuti) e 9B.
- **Senza esito in 2 ore:** 1B, 1C, 3B, 5B, 7A e 7C; la 5C una volta su tre vince.
- **La 0A** lancia i quattro TASM in tutte le varianti, nell'app sul seme 1 fra 221 e 251 s. La fuga di 220 km verso
  Copenaghen dura più di 2 ore.

## 3. Fonti e confidenza

È un modello di gioco, non di realtà: non ha fonti esterne. Le soglie che decidono le statistiche sono **STIMA DI
GIOCO**, scelte per leggere bene la traccia campionata: 60 s per fondere le righe e per la contro-scoperta dopo un
ping, 30 s per i buchi dell'esposizione. Il comandante IA ha le sue fonti nel capitolo E6 §2.17.

## 4. Classico ed Enhanced

Solo Enhanced: la sandbox impone la regola Enhanced e il classico non cambia.

## 5. Come si vede in gioco

Volume 1, capitolo 12; i tasti nell'appendice A (regolazioni, in mare nella sandbox, statistiche, replay).

## 6. Taratura e prove

- **Prove** (in `engine/test/`):
  - `prove_sandbox_v2.cpp`: il livello e l'allerta della parte, «sandbox V2: R1-R6 nella TORPEX col comandante; senza
    registra_statistiche niente», il quadro di una parte, TMA e tubi, le ROE della parte anche per l'artiglieria, il
    comandante che guida a filo i suoi siluri e posa le mine;
  - `prove_statistiche_sandbox.cpp`: una traccia sintetica coi conti noti, una partita vera (S4), le cariche di
    profondità nel Pk, «statistiche sandbox: una serie vera di tre semi (S4 in sandbox, 600 s)»;
  - `prove_codice_partita.cpp`: «codice sandbox: andata e ritorno con le regolazioni, distinto dal codice partita»;
  - `prove_replay_schermo.cpp`: la vista dal vivo (traccia dai fotogrammi, indietro e al vivo, registro coi filtri, V
    sulle quattro letture);
  - `prove_statistiche_schermo.cpp`: «statistiche, schermo: la sandbox in cinque pagine, Tab, frecce e il replay da una
    riga»;
  - `prove_serie_sandbox.cpp` (`serie.toml` che si rilegge uguale, la schermata in tre pagine, i vetrini pronti),
    `prove_grafici_serie.cpp` (riassunto, numeri all'italiana, disegni nei rettangoli), `prove_comandante_ia.cpp`.
- **Le originali:** `test/diag_originali.cpp`, il piano di ogni originale nella geometria della missione; con
  `DIAG_ORIGINALE` una partita col piano e senza. Risultati e storie in `tools/taratura/risultati/originali_v3/`
  (LEGGIMI).
- **Regressione:** `tools/regressione/sandbox_scene.sh`, le nove scene di prova su tre semi senza finestra: 27
  partite senza errori al 28/10/2026.
- **Classico:** 0/110 dopo ogni fase (`docs/19`, Stato).

## 7. Limiti noti e in arrivo

- **La verità** della vista dal vivo non disegna le zone e le aree d'azione.
- **Il quadro di una parte** dice il sensore, non le fonti.
- **La scheda dell'unità** dice lo stato, non i danni per sistema.
- **Il replay del dopo** ha solo le letture verità e quadro, e parla in seconda persona.
- **Il TLAM-C.** Il TLAM-C non usa il gestore del Tomahawk antinave: una missione che vuole contare gli attacchi a terra dichiara il proprio gestore `armi.attacco_terra`; senza, il lancio resta solo nel registro. Nessuna missione del gioco lo dichiara oggi: nella 0A conta solo il TASM.
- **Le serie** dal gioco hanno 10 semi fissi (1-10) e non si interrompono; per altri semi c'è lo strumento.
- **La parte del luogo** (M7, capitolo E6): la pagina 2 della serie mostra solo le prime due parti, NATO e Patto.
  La colonna «LUOGO» c'è nella pagina 1 e nel riassunto.

## 8. Riferimenti

- **Piano:** `docs/19_sandbox.md`; note condivise sandbox-v1-runtime-contratto.md, sandbox-v2-viste-contratto.md,
  sandbox-vista-vivo-contratto.md, sandbox-v3-serie-contratto.md, sandbox-v3-originali-contratto.md.
- **Codice:**
  - app: `main.cpp`, `regolazioni_sandbox`, `codice_partita`, `replay`, `statistiche`, `debriefing`;
  - missioni: `comandante_ia`, `sandbox_originali`, `missione_quadro.cpp`, `missione.hpp` (`RegolazioniParte`, `InterruttoriIa`);
  - nucleo: `statistiche_sandbox`;
  - grafica: `simboli_carta`, `grafici_serie`;
  - strumenti: `serie_sandbox`, `scene_interazioni`; app: `schermata_serie`, `serie_dati`, `scelta_missione`;
  - dati: `data/motore/sandbox_originali.toml`, da `tools/pipeline/sandbox_originali.py`.
- **Capitoli:** E6 (l'IA e il comandante), F1 (registro e liste R1-R6), F4 (statistiche), F5 (replay), E9 (simboli).
