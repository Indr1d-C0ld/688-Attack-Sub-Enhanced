# 13 — ASW aereo nell'Enhanced: piano d'implementazione

Richieste dell'utente del 25/10/2026: animazioni degli elicotteri ASW (sonar a immersione, sonoboe, armi) e
comportamento reale degli aerei ASW secondo la dottrina della Guerra Fredda, con le manovre scelte con probabilità e
cosa se ne vede e se ne sente dal battello. Requisito 13 di `docs/00_ROADMAP.md`; ricerca
`docs/ricerca/17_asw_aereo.md` (il «dossier 17», citato per paragrafo).

Tutto vale **solo con la regola di simulazione**. Il classico resta identico all'originale: SH-60B che pattuglia,
va sul punto della nave colpita e fa comparire la boa marcatrice (docs/originale/comportamenti_ia.md §3.C).

## Che cosa c'è oggi nel codice

- Gli aerei sono unità della VM delle missioni (`flag & 3 == 3` o categoria «aereo») mosse dagli script.
  Nell'Enhanced l'unico strato che prevale sugli script è `OrdineMotore` (reazioni ai danni).
  **Non c'è ancora un'IA di dottrina**: l'ASW aereo sarà la prima, e il suo schema servirà poi alle navi (Fase 4,
  «IA a livelli»).
- Le quote degli aerei sono finte: 150 m per elicotteri, 300 m per gli aerei, solo per radar ed ESM
  (`QUOTA_ELICOTTERO_M`, `QUOTA_AEREO_M` in missione.cpp).
- Gli aerei sono esclusi dalla scena acustica: niente sorgente, niente sensori (`sorgente_acustica`,
  `sensori_acustici`).
- L'ESM riconosce già il radar di un «aereo ASW» (`EventoEsm::Tipo::aereo_asw`, messaggio 335).
  Fra gli emettitori ci sono APS-115, APS-124 e Initsiativa-2K.
- Il registro M1 ha già `boa_vicina` e `ping_subito`.
- `siluri.toml` ha `mk46` e `at1`; `acustica.toml` non ha sensori di boe né sonar a immersione.
- Il kit ha sprite di p3c, il38, tu142, sh60b e ka25, ma nessun effetto di hover, cavo, boa, paracadute o
  marcatore.

## Architettura

```
            cue (SOSUS/intel radio, nave colpita, siluro sentito, radar/ESM sull'albero, contatto da nave)
                │
                ▼
  nucleo/asw_aereo  ── ControlloreAsw (macchina a stati §7, una per aereo o gruppo) ── puro, deterministico
     │  ▲    schemi di posa (§6.1, §6.6), MAD (§6.3), ciclo di dip (§6.4-6.5), TOS (§6.8), equipaggio (§7.4)
     │  │
     │  └── Percezione: rivelazioni delle boe e del sonar a immersione (dal motore acustico), MAD, radar, cue
     ▼
  ComandoVolo (meta, quota, velocità, hover) + Azioni (posa boa, cala/ping/recupera, arma, marcatore, SUS, passa
  contatto, rapporto)
     │
     ▼
  missioni/Missione  ── esegue: unità e boe, siluri del modello Enhanced, transitori sul bus acustico, registro M1,
     │                   messaggi, radio (pace → attacco simulato → escalation)
     ▼
  grafica (kit): hover e spruzzi, cavo, boa espulsa con tonfo/galleggiante/colorante, MAD, paracadute, Mk 25/58,
                 luci; app: carta, debriefing, briefing, opzioni
```

Principi:
- **La rivelazione resta del motore acustico.** Boe e sonar a immersione sono **ricevitori** con quota, banda e
  direttività, sulla stessa equazione del sonar e con le stesse sorgenti dei battelli. Il controllore riceve
  soltanto «la boa N sente qualcosa a questo livello, con questo rilevamento». Non c'è un tiro di dado parallelo
  per la scoperta.
- **Il MAD sta in asw_aereo**, perché è geometria e magnetismo e non acustica. Usa la formula OPTEVFOR (§6.3) con le
  posizioni vere e il dislocamento dalla classe.
- **Un flusso casuale proprio** (`caso_asw_`, seminato da `seme_` come `caso_acustica_`). Il controllore riceve
  `Casuale&` e non ne crea.
- **Ritmo:** il controllore pensa ogni 5 s. Le boe ascoltano ogni 5 s solo i sottomarini, più le sorgenti che danno
  falsi allarmi (biologici, mercantili vicini). Il costo deve restare sotto il **+10%** sull'ora headless.

## Pezzi e chi li fa

| Pezzo | Contenuto | Chi |
|---|---|---|
| **A0 Dati** | `tools/pipeline/asw_aereo.py` → `data/motore/asw_aereo.toml`: mezzi (sensori, carico di boe e armi, quote e velocità di transito, ricerca e posa, TOS(d), equipaggio), boe (§4.1-4.2: quota, tempi di discesa e stabilizzazione, vita, guasto, soglia, banda), marcatori (§4.3), schemi (§6.1, §6.6), tabelle di probabilità (§7.2-7.3), moltiplicatori di equipaggio (§7.4) e di difficoltà (§10.3). Ogni valore porta `conf` e la sigla della fonte, e le STIME sono segnate. In `classi.toml` le classi nuove `s3a`, `s3b`, `sh3h`, `ka27`, `be12` e `sh60f` (quest'ultima solo dal 1990). In `emettitori.toml` APS-116 e APS-137 col modo periscopio, Korshun con l'agilità di frequenza, Initsiativa-2B e 2KM, e «Short Horn» per il Ka-25 | runtime |
| **A1 Modulo** | `nucleo/asw_aereo.{hpp,cpp}`, puro: `Sonoboa` (tipo, quota, posa, impatto, attiva, fine, deriva, guasto); funzioni di posa degli schemi (datum, ritardo, velocità stimata, portata → punti); MAD; ciclo del sonar a immersione USA e URSS; `ControlloreAsw` con gli 11 stati del §7.1, le transizioni del §7.2 e la scelta della manovra del §7.3; TOS e cambio del turno; dottrina di pace, crisi e guerra (§5.5); gruppi (coppia Be-12, gruppo di Ka-27 col capogruppo, rilevo del P-3). Prove in `test/prove_asw_aereo.cpp`: geometria degli schemi, MAD sulla tabella §6.3, tempi del ciclo di dip, transizioni con seme fisso, falsi allarmi e boe guaste in proporzione | runtime |
| **A2 Acustica** | Sensori delle boe e dei sonar a immersione in `acustica.toml`: SSQ-41B, 53B, 77A, 47B, 62B; RGB-NM, RGB-1, RGB-2, RGB-75, RGB-15, RGB-25, RGB-55A; AQS-13, VGS-3, con banda, quota e direttività (DIFAR σ 5-9°, RGB-2 5-7°) e la soglia delle sovietiche. `ricevitore_boa()` e `ricevitore_dip()`. La **sorgente aerea** con la finestra di Snell (§8.3: `x ≈ 0,18·h + 1,2·d`, toni di elica e rotore, P 0,8 o 0,95). I **transitori** come sorgenti impulsive del bus acustico: tonfo di boa, di trasduttore e di siluro; ping di AQS-13, VGS-3, DICASS e SSQ-47B; esplosive MGAB; SUS Mk 84. Il sonar del giocatore e la cascata li mostrano col rilevamento. L'asimmetria del §8.3 (il ping si sente 7-12 volte più lontano della scoperta dell'eco) esce dall'equazione, e una prova la verifica | disassemblatore |
| **A3 Missione** | In `Missione`, con la regola di simulazione, un `ControlloreAsw` per ogni aereo la cui classe è in asw_aereo.toml. Il volo diventa vero: quota, hover, virate; le costanti finte di 150 e 300 m spariscono. Le boe stanno in un elenco (`boe()`). Le armi entrano dal paracadute in `siluri_enhanced`: Mk 46 Mod 5, AT-1M, UMGT-1, APR-2 (le ultime due da aggiungere a siluri.toml); le bombe PLAB passano per la fisica delle esplosioni. I **cue** sono: nave amica colpita (il GOTO_TGT degli script diventa un datum), contatto d'intelligence della radio (SOSUS), siluro lanciato sentito da una nave amica (flaming datum), albero visto al radar o all'ESM, contatto passato da una nave. Le ROE vengono da `blocco_carriera().situazione` e dalla radio: in pace SUS o rapporto, messaggio, escalation secondo il file radio. Nel registro M1: `aereo_in_sorvolo`, `ping_sonar_immersione`, `marcatore_avvistato`, `attacco_simulato`, più le esposizioni per la carriera. I messaggi stanno nella fascia **380-389** (ceduta dal principale). La visibilità al periscopio segue il §8.1: orizzonte, portate, foschia, notte, luci spente in caccia. L'API Lua per le missioni (`aereo_asw{...}`: tipo di missione, zona, base, cue) va documentata in API.md | runtime, dopo A1 e A2 |
| **A4 Kit** | Effetti in `grafica/effetti`: anello di spruzzi sotto l'hover (12 m SH-3H, 30 m Ka-27); cavo e trasduttore che scendono e risalgono; boa espulsa con tonfo, galleggiante con antenna e macchia di colorante; MAD trainato (180 ft); siluro col paracadute e tonfo; Mk 25 (fiamma e fumo per 10-20 min), Mk 58 (due candele per 40-60 min), OMAB sovietici; luci anticollisione e di navigazione, spente in caccia. Sprite nello stile del 1989 per s3a/s3b, sh3h, ka27, be12 e sh60f. Icone delle minicartoline: aereo in sorvolo, boa, elicottero in hover, marcatore | kit |
| **A5 App** | Partita e vista: boe, marcatori e luci passati agli effetti; carta del giocatore con le sole boe viste o sentite (tonfo col rilevamento). Debriefing: in «Verità» le tracce degli aerei e gli schemi di boe, e le minicartoline dei nuovi eventi. Briefing: il livello «aerei ASW» con i raggi d'azione dalla base e il TOS. Testi dei messaggi 380-389. Opzione di difficoltà ASW (§10.3: facile, normale, realistico, minaccia percepita) in `--asw` e poi nel menu. Missioni di prova (vedi sotto) | principale |
| **A6 Taratura** | Uno strumento headless gira N semi per scenario e misura la probabilità di scoperta, il tempo dal cue al lancio e le uccisioni. Si confrontano col §9: un datum di 10 min con elicottero dà P ≈ 0,14; l'Il-38 è 8-10 volte peggio del P-3; un SSN silenzioso, profondo e lento sopravvive quasi sempre. Si ritoccano solo le STIME in asw_aereo.toml | runtime con il principale |
| **A7 Ricerca** | Riprovare le fonti della §12 del dossier (DTIC, CIA, Proceedings) quando tornano in linea. Possono cambiare solo i valori di asw_aereo.toml, non il codice | chi è libero, bassa priorità |

## Decisioni prese (modificabili)

1. **Le 22 missioni originali nell'Enhanced.** L'SH-60B passa al controllore con la dotazione vera: boe, MAD
   trainato, 2 Mk 46, niente sonar a immersione. Lo script decide *quando* (pattuglia, datum sulla nave colpita), il
   controllore decide *come*. La boa marcatrice della missione 7 diventa una vera posa di una boa attiva
   (DICASS), che il giocatore sente pulsare come nell'originale. La missione resta risolvibile, e una prova lo
   verifica.
2. **Difficoltà ASW predefinita «normale»** (equipaggi medi, avvisi come al §8). L'opzione «minaccia percepita» è
   fuori dalla carriera, o pesa sul punteggio come la difficoltà della missione.
3. **SH-60F e Ka-27 col gruppo di 4-6** vanno solo nelle missioni che li chiedono: rapide, editor, prove. Nessuna
   originale li riceve.
4. **L'Alfa** affronta l'ASW USA (P-3, S-3, SH-3H) con gli stessi moduli: nessun lavoro a parte, solo missioni di
   prova.

## Contratto fra i pezzi (da fissare all'inizio di A1)

Il runtime pubblica per primo l'intestazione `nucleo/asw_aereo.hpp`, anche con le funzioni vuote, così A2, A4 e
A5 lavorano in parallelo:

```cpp
struct Sonoboa { int id, lanciatore; std::string tipo; Vec2 pos, deriva; double quota_m, t_posa, t_impatto,
                 t_attiva, t_fine; bool guasta, attiva_sonar; };
enum class StatoAsw { transito, ricerca, datum, contatto, localizzazione, inseguimento, attacco, verifica, perso,
                      cambio, rientro };
struct ComandoVolo { Vec2 meta; double quota_m, velocita_kn; bool hover; double cavo_m; };
struct AzioneAsw { enum class Tipo { posa_boa, cala_sonar, ping, recupera_sonar, lancia_arma, marcatore, sus,
                   passa_contatto, rapporto } tipo; Vec2 pos; std::string oggetto; int bersaglio = -1; };
struct Rivelazione { int boa_o_aereo; double t, livello_db; std::optional<double> rilevamento, distanza_m; };
struct PercezioneAsw { std::vector<Rivelazione> acustiche; std::vector<int> mad; std::vector<Vec2> radar;
                       std::vector<Cue> cue; double ora_locale; int mare; };
class ControlloreAsw { /* passo(t, PercezioneAsw, Casuale&) -> std::vector<AzioneAsw>; volo(); stato(); */ };
```

Gli effetti del kit ricevono stato e geometria (posizione, quota, `hover`, `cavo_m`, elenco delle boe con tempi):
nessuna dipendenza dal controllore.

## Ordine di lavoro

1. **In parallelo:**
   - runtime: A0 e l'intestazione di A1, poi A1;
   - disassemblatore: A2 sull'intestazione;
   - kit: A4;
   - principale: le missioni di prova, i testi e l'opzione `--asw`.
2. **Runtime: A3.** File condivisi a turni: missione.*, stazioni_missione.*, CMakeLists. Principale: A5 nella
   partita, nella vista e nelle schermate.
3. **A6, taratura** con le missioni di prova, poi la documentazione in questo file e nella roadmap.

## Missioni di prova

Si scrivono in Lua con la radice di prova e i symlink, senza sovrascrivere API.md.

| Prova | Scena | Che cosa deve accadere |
|---|---|---|
| `asw_giuk` | P-3C da Keflavik su un datum SOSUS di 3 h nel varco Islanda-Fær Øer, 688 a 5 kn sotto lo strato | barriera sulla rotta probabile, poche rivelazioni; a 15 kn la scoperta sale di 3-5 volte |
| `asw_datum` | SH-3H su una fregata colpita, 688 a 300 m | spirale di dip; ping che si sentono da lontano; P ≈ 0,14 sul datum di 10 min |
| `asw_kiev` | gruppo di 4 Ka-27 in linea, di giorno e poi di notte | dip passivo a 4-6 km; di notte solo boe |
| `asw_il38` | Il-38 con barriera di RGB-1 nel Mar di Norvegia | barriere d'intercettazione ogni ~26 min; molti falsi allarmi |
| `asw_alfa` | Alfa contro P-3C e S-3B | DIFAR e DICASS; il modo periscopio dell'APS-137 all'ESM |
| `asw_pace` | P-3C in pace contro un Victor III | pedinamento e SUS, nessuna arma; messaggio radio |
| missione 7 Enhanced | l'originale con la regola di simulazione | boa DICASS posata sull'SSBN, missione risolvibile |

## Verifiche

- La regressione del classico resta **IDENTICO**.
- La regressione dell'Enhanced cambia solo nelle missioni con aerei. Le differenze si guardano una per una, poi si
  rifà la base.
- Tutte le prove passano, con le nuove di A1, A2, A3 e A6.
- L'ora headless dell'Enhanced non rallenta più del 10%.
- Nella vista si vedono hover, cavo, boe, paracadute e marcatori. Al sonar si sentono tonfi, ping e sorvoli.

## Stato

- 25/10/2026: piano scritto e approvato dall'utente.
  - Avviati: runtime su A0 e A1 (prima l'intestazione `nucleo/asw_aereo.hpp`); disassemblatore su A2, dopo le
    dimensioni del PMR-1; kit su A4, dopo le sagome di armi e mine.
  - La fascia di messaggi 380-389 è ceduta al runtime.
  - Intestazione `nucleo/asw_aereo.hpp` pubblicata dal runtime (stub che compilano). Le aggiunte al contratto:
    - Cue con raggio, rotta e velocità facoltative;
    - in Rivelazione, la verità (sorgente, sottomarino), usata solo per la classificazione;
    - in PercezioneAsw, lo stato dell'aereo, il vento, la temperatura e la situazione;
    - in AzioneAsw, una boa già creata;
    - in ComandoVolo, luci e radar;
    - le boe tenute dal controllore, con id = lanciatore·1000 + progressivo;
    - `CicloDip` con fasi e cavo per il kit.
  - I sensori di acustica.toml si chiamano ssq41b … vgs3.
  - Sprite segnaposto per le 6 classi nuove, finché il kit non fa quelli veri.
  - A0 e A1 finiti (runtime):
    - `asw_aereo.toml` con 17 boe, 4 marcatori, 4 sonar a immersione e 11 mezzi, ogni voce con fonte, conf e le
      stime da tarare;
    - 5 radar nuovi; l'agilità di frequenza fa identificare il radar all'ESM con p 0,3;
    - 9 prove in `prove_asw_aereo.cpp`; il classico resta identico.
  - Altre aggiunte al contratto: `SensoreMad`, `OpzioneSchema`, carichi alternativi, `boe_seguite`, quota del
    trasduttore.
  - A3 avviato dal runtime, con il turno su missione.*. L'aggancio acustico di A2 si concorda col disassemblatore.
  - Nel Mondo una quota negativa indica un aereo in volo: il periscopio lo disegna all'altezza vera (kit).
  - A2 finito (disassemblatore):
    - `nucleo/acustica_asw` con 21 sensori di boe e sonar a immersione; soglia delle boe sovietiche; Snell per gli
      aerei; tabella `[transitori]`;
    - asimmetria ping/eco 7,4 (AQS-13B) e 10,6 (DICASS);
    - circa 8 µs per coppia boa-sorgente.
    - I due limiti sono chiusi (dossier 17 §8.4):
      - riga di pala di Ross nelle firme, sentita dalle boe con la gamma bassa: la RGB-75 contro un 688 sente 0,9 km a
        8 kn, 3 km a 12 kn e ~30 km a 20 kn; i 15-20 km a 12 kn valgono solo contro battelli rumorosi;
      - riverberazione di superficie (Chapman-Harris), di volume e di fondo: l'AQS-13B contro un 688 scopre l'eco a
        5,4 kyd con lo strato, 2,7 kyd con mare 5, 9-11 kyd in buona propagazione.
    - **Per A6:** su fondali bassi (150 m) la riverberazione di fondo dimezza la portata attiva delle navi (SQS-53 da
      9,6 a 4,6 km). Va controllata nei teatri d'acqua bassa.
  - A4 finito (kit):
    - effetti `aerei_asw` e `boe_asw` a ogni passo: spruzzi dell'hover, cavo e trasduttore, MAD, luci;
      espulsione, tonfo, colorante e galleggiante delle boe;
    - una volta per evento: `marcatore_asw` (Mk 25, Mk 58, OMAB) e `arma_paracadute`, che restituisce i secondi al
      tonfo;
    - icone delle minicartoline per i 5 eventi nuovi;
    - sprite di s3a, s3b, sh3h, sh60f, ka27 e be12 (222 vedute ciascuno), al posto dei segnaposto;
    - prove in `prove_effetti_asw.cpp`; il classico resta identico.
  - Sprite dalle foto (kit): S-3A e S-3B, SH-3H, Ka-27 e Be-12, calibrati su 9 foto di Commons. Lo sprite del Be-12
    deriva da una foto CC BY-SA 2.0 di Alan Wilson, quindi va attribuito ed è CC BY-SA. Il kit prepara i crediti
    centralizzati (`data/motore/crediti.toml`, `docs/CREDITI.md`).
  - A7 tentato (disassemblatore). DTIC dà 403, la sala di lettura CIA è in manutenzione, GAO e Proceedings non sono
    accessibili. Nessun valore cambia; il tentativo è annotato nel dossier 17 §12. Si riprova quando i siti tornano.
  - A3 (runtime):
    - la parte dell'aereo viene dalla classe;
    - configurazione delle originali in `data/missioni/asw/<ID>.lua`;
    - la 1A in pace con il SUS; la 7 in ricognizione con la DICASS al posto della boa marcatrice;
    - messaggi 380-389 fissati.
  - Il principale fa A5 quando c'è l'intestazione e le missioni di prova quando c'è l'API Lua di A3.
  - A5 in corso (principale, 25/10/2026):
    - nella vista gli aerei alla quota vera, gli elicotteri con hover, cavo, MAD e luci; le boe coi loro tempi; i
      marcatori una volta (OMAB di giorno o di notte con la luce del momento); i siluri aviolanciati col paracadute
      (le PLAB senza, lo scoppio lo fa la fisica). Verificato al periscopio: l'SH-60B della 7A a 7 km e 150 m;
    - testi dei messaggi 380-389, col rilevamento al posto di `{RIL}`, nella barra e nello storico dei messaggi;
    - nella «Verità» del debriefing le boe posate (croce bianca in vita, grigia finita).
    - Restano: carta del giocatore (boe sentite o viste, marcatori), livello «aerei ASW» nel briefing, opzione di
      difficoltà ASW (da far passare da `DanniMissione`, d'accordo col runtime).
    - Carta del giocatore (25/10/2026, sera): ogni avviso ASW con un rilevamento diventa una linea sul lucido, dalla
      posizione del battello in quel momento, con la fonte e l'ora. Le fonti sono TONFO, SUS e FUMOG. in giallo; DIP,
      BOA ATT. e SIL.AER. in rosso; ESM AER. in verde. La boa non compare mai: la sua posizione resta ignota, come a
      bordo. Le linee entrano anche nel pannello tempo-rilevamento e sbiadiscono dopo 10 minuti come le altre.
      Verificato con un Be-12 in caccia (missione di prova con `--cartella-missioni`): due tonfi a 139° e 168°.
- 25/10/2026, sera (principale): livello «PORTATE ASW NEMICHE» del briefing dagli aerei previsti
  (`Missione::aerei_asw_previsti`): zona di pattuglia a puntini rossi (o 25 nm attorno a base o unità), classe e
  compito, base con la B. Nelle 22 originali gli aerei ASW sono tutti amici del 688, quindi il livello resta grigio:
  si vedrà nelle missioni rapide e di prova. Opzione `--asw facile|normale|realistico|minaccia`
  (`Missione::imposta_difficolta_asw`).
- 25/10/2026, taratura A6 (runtime, `engine/strumenti/taratura.cpp`, missioni in `tools/taratura/missioni`, dati in
  `tools/taratura/risultati`).
  - Corretti cinque errori:
    - dip in localizzazione;
    - orbite a 130 kn;
    - incrocio DIFAR mal condizionato;
    - distanza dal rilevamento singolo;
    - siluro aviolanciato senza ricerca.
  - Risultati prima dei ritocchi:

    | Scena | P di scoperta | Confronto storico |
    |---|---|---|
    | SH-60F sul datum | 0,95 | 0,14: manca l'evasione dell'IA |
    | Il-38 | 0 | conferma da indagare |
    | Be-12 col campo di boe | 0,55 | come la storia |
    | 688 silenzioso | 0,03 | come la storia |

    - Sortite per scoperta: P-3C 1,1, contro le 3 del Tu-142M nel 1983.
    - Uccisioni: nessuna. Si conterà l'uccisione di missione (`menomato`), e il kit rivede la letalità delle teste
      leggere.
  - Approvati:
    - Pk dell'AT-1 riportata a 0,15-0,18;
    - indagine sull'Il-38;
    - schema di boe lungo il rilevamento;
    - scena col traffico per i falsi allarmi;
    - evasione dei sottomarini dell'IA (ping, tonfi, MAD), come modulo di dottrina dell'Enhanced.
- 29/10/2026 (runtime): **il ghiaccio**, per la LIB-I16 di trailer-1 («Il bastione bianco», Okhotsk sotto il pack).
  - `PercezioneAsw::acqua_libera(p)`: dove una boa, il trasduttore del sonar a immersione o un'arma entrano in acqua.
    Vuota vuol dire ovunque (il mare senza ghiaccio, le prove). La missione la dà dal campo del ghiaccio
    (missione_asw.cpp): libera senza ghiaccio o nelle polynye e nei canali, non sul nilas, come per il periscopio. Senza
    il campo vale la copertura del meteo sotto 0,5 (STIMA).
  - **Le boe:** ogni punto dello schema che cade sul ghiaccio va nell'acqua libera più vicina entro la portata della
    boa, se lì non ce n'è già un'altra a meno di mezza portata. Se l'errore di posa la mette sul ghiaccio, la boa è
    persa. Se tutto lo schema del datum cade sul pack, l'aereo ci rinuncia: passa a «perso» col motivo «datum sul pack».
  - **Il sonar a immersione:** la stazione sul ghiaccio si sposta nell'acqua libera entro mezzo salto; se non c'è, si
    salta. Se finiscono tutte, l'elicottero rinuncia al datum come l'aereo.
  - **Le armi:** il punto di sgancio va nell'acqua libera entro 1 km; se non c'è, niente lancio.
  - **La prova:** prove_asw_aereo.cpp, «sotto il pack le boe e il sonar a immersione solo in acqua libera». Ci sono
    quattro casi: P-3C col pack fino a un margine, tutto pack, SH-3H, e senza ghiaccio, dove tutto resta come prima.
