# 14 — Periscopi nell'Enhanced: piano d'implementazione

Richiesta dell'utente del 25/10/2026: la centrale del 688 con **due periscopi** selezionabili, il Type 2 d'attacco
e il Type 18 di ricerca, «importante per la simulazione e l'approfondimento del gameplay». Poi l'Alfa col suo
«Сигнал» e la TV-1, e i periscopi dell'IA.

Si appoggia a:
- requisito 8 di `docs/00_ROADMAP.md`;
- `docs/ricerca/20_periscopi.md` (il «dossier 20», citato per paragrafo; proposta al §8);
- dossier 19 §2.1 e §3.2 (il posto in centrale) e §9 (caratteri);
- dossier 11 §5 (radar contro l'albero).

Tutto vale **solo con la regola di simulazione**. Il classico resta identico: un periscopio, due ingrandimenti, su
e giù a 20 ft.

## Che cosa c'è oggi

- La vista (`grafica/periscopio`) ha due ingrandimenti fissi (1,5× e 6×) e la luce ambiente per la notte. La
  vista è rettangolare, con la scala dei rilevamenti.
- Lo stadimetro (`interfaccia`) misura dall'altezza dell'albero, con un errore di lettura.
- Il periscopio è un flag (`flag54 & 2`) alzato o abbassato da un ordine (`TipoOrdine::periscopio`).
- Nell'Enhanced il radar nemico vede l'albero con la probabilità per giro del dossier 11 §6.4, in
  `nucleo/elettronica`. La testa è unica, senza sezione per modello.
- L'IA non usa periscopi: le scoperte visive sono quelle dell'originale.

## Pezzi e chi li fa

| Pezzo | Contenuto | Chi |
|---|---|---|
| **P0 Dati** | `tools/pipeline/periscopi.py` → `data/motore/periscopi.toml` (dossier 20 §8.4). Per modello: ingrandimenti e campi, elevazione minima e massima, telemetro (sdoppiato o reticolo) e suoi limiti, notte (niente, bassa luminosità, intensificatore, TV, termico), TV, foto, ESM, stabilizzazione, filtri, passi del reticolo, sezione radar, fattore di piuma, tempi d'alzata e discesa, velocità libera, di tremolio, inservibile e di danno. Per classe: l'elenco dei periscopi, la quota periscopica e la TV con la sua quota massima (МТ-70). Fonte e confidenza su ogni valore, STIME segnate | kit |
| **P1 Modello** | `nucleo/periscopi.{hpp,cpp}`, puro e deterministico. Stato del posto: periscopio scelto, alzata in corso (frazione), elevazione, ingrandimento, modo (giorno, bassa luminosità, TV). Contatore dei secondi d'esposizione e storia delle esposizioni. Tremolio e piuma dalla velocità e dal mare, rischio di danno oltre la velocità di danno. **Visibilità del periscopio del giocatore** per un osservatore: portata visiva dalla testa, dall'altezza esposta, dalla piuma, dal sole, dal mare, dalla notte e dai suoi sensori notturni. Sezione radar da passare a `elettronica` al posto di quella unica. **Periscopi dell'IA:** pianificazione delle esposizioni (circa 10 s ogni 3-12 min, prima l'avvisatore radar per i sovietici, TV МТ-70 fino a 50 m) e cosa vedono. Prove in `test/prove_periscopi.cpp` | runtime |
| **P2 Missione** | In `Missione`, con la regola di simulazione: gli ordini nuovi `scegli_periscopio`, `elevazione_periscopio` e `foto_periscopio` in ordini.* (ingrandimento e modo restano dell'interfaccia: non cambiano la simulazione). Alzata con i suoi secondi: il flag si alza a fine corsa. Esposizione e sezione del periscopio alzato verso `elettronica`. Scoperta visiva del periscopio del giocatore da parte di navi, aerei e periscopi nemici; esposizioni dell'IA con le loro teste visibili al giocatore, alla vista e al radar. Danno al sistema «periscopio» oltre la velocità di danno. Registro M1: `foto_periscopio` (con la fotografia della vista per la minicartolina, come gli eventi visibili) ed `esposizione_lunga` per la carriera. Messaggi nella fascia del runtime | runtime, dopo P1 |
| **P3 Vista** | In `grafica/periscopio`: l'**oculare tondo** con vignettatura; i reticoli per modello (croce e tacche americane in 1° e 15′ a 6×, 4° e 1° a 1,5×; griglia sovietica). Niente numeri dentro il cerchio. Il 12× del Type 18. L'**elevazione**: la vista sale verso il cielo e mostra gli aerei con lo sprite dell'elevazione giusta. Lo **stadimetro con l'immagine fantasma**, sovrapposta e spostata. La **bassa luminosità** (verde o grigia granulosa, schiarita) e l'**intensificatore sovietico** verde giallino. La **TV** monocroma con più grana: monitor della centrale per il Type 18, TV-1 sul pulto dell'Alfa. Il **tremolio** con la velocità e il mare, minore col Type 18 stabilizzato. I filtri. La telecamera verticale della vela dell'Alfa, verso il ghiaccio e la luce | kit |
| **P4 Stazione** | In `app/` (Periscopio F5 e Centrale): tasto per scegliere il periscopio, alzata e discesa coi loro tempi, elevazione con le frecce e fermi a 14° e 44°, ingrandimenti a giro (con il 12×), modo notte e TV, foto. Il **contatore dei secondi d'esposizione** sulla cornice. Nel modo moderno la **lista NWP**: tre giri da 8 s, «nessun contatto vicino», ricerca aerea, giro da 45 s, ESM nei primi 24 s; con le chiamate. Stadimetro a immagine sdoppiata sul 688, misura col reticolo sull'Alfa (si sceglie l'altezza del bersaglio, si contano le divisioni). Monitor TV in centrale. Minicartoline delle foto nel debriefing | principale |
| **P5 Taratura** | Le STIME (sezioni, piuma, velocità, portate visive) sulle missioni di prova: periscopio d'attacco contro una fregata col radar in mare 2 e mare 5, di giorno e di notte, a 4 e a 10 nodi. Esposizioni dell'IA contro il giocatore in agguato | runtime col principale |

## Contratto (da fissare all'inizio di P1)

Il runtime pubblica per prima l'intestazione `nucleo/periscopi.hpp`, così kit e principale lavorano in parallelo:

```cpp
enum class ModoVista { giorno, bassa_luminosita, intensificatore, tv, termico };
struct DatiPeriscopio { std::string id, nome; std::vector<double> ingrandimenti, campi_gradi; double elevazione_min,
                        elevazione_max; std::string telemetro; std::vector<ModoVista> modi; bool tv, foto, esm,
                        stabilizzato; double sezione_m2, piuma, alzata_s, discesa_s, v_libera_kn, v_tremolio_kn,
                        v_inservibile_kn, v_danno_kn; std::vector<double> reticolo_gradi; };
struct StatoPeriscopio { std::string scelto; double alzata = 0;  /* 0 giu', 1 su */ double elevazione = 0;
                         int ingrandimento = 0; ModoVista modo = ModoVista::giorno; double esposto_s = 0;
                         double tremolio = 0; /* 0-1 per la vista */ };
```

La vista (P3) riceve `DatiPeriscopio` e `StatoPeriscopio` nella `VistaPeriscopio`, senza dipendere dal modello.

## Ordine di lavoro

1. **In parallelo:**
   - kit: P0, poi P3 sull'intestazione;
   - runtime: l'intestazione di P1 e gli ordini nuovi, poi P1.
2. Runtime: P2. Principale: P4 sopra P1 e P3.
3. P5, poi la documentazione qui e nella roadmap.

## Verifiche

- La regressione del classico resta **IDENTICO**.
- Nell'Enhanced cambia solo quello che dipende dal periscopio. Le differenze si guardano una per una.
- Prove nuove:
  - alzata e discesa;
  - esposizione;
  - tremolio e danno con la velocità;
  - portate visive per testa, mare e notte;
  - pianificazione delle esposizioni dell'IA;
  - geometria dei reticoli e dello stadimetro.
- Istantanee:
  - oculare dei due periscopi del 688 a 1,5×, 6× e 12×;
  - elevazione verso un elicottero;
  - notte;
  - TV;
  - Alfa con intensificatore e TV-1.

## Stato

- 25/10/2026: piano scritto; l'utente ha dato il via.
- 25/10/2026, a fine giornata:
  - P0 e P3 fatti (kit): `periscopi.toml` per tutte le classi, oculare tondo coi reticoli, 12×, elevazione, stadimetro
    con l'immagine fantasma, notte, TV, tremolio, acqua e gocce sulla finestra, mare dinamico (Pierson-Moskowitz).
  - P1 e P2 fatti (runtime): modello, ordini `scegli_periscopio`, `elevazione_periscopio` e `foto_periscopio`,
    esposizione verso l'elettronica, viste dell'IA, danno, registro, messaggi 400-407; l'onda di `nucleo/onde.hpp`
    decide se la finestra è coperta.
  - P4 fatto (principale): stazione Periscopio (F5) coi periscopi veri.
    - Tasti: 1 periscopio successivo, 2 su/giù, 3 ingrandimento, 4 modo, 5 foto, 6 filtro.
    - Le frecce cambiano l'elevazione e si fermano ai fermi del modello.
    - Contatore d'esposizione: verde, giallo oltre 8 s, rosso oltre 30 s.
    - Nel modo moderno la lista NWP: giri rapidi e ricerca aerea.
    - La vista viene da `parametri_oculare`, con le onde della simulazione; Z e ingrandimento seguono il modello.
    - Su/giù usa l'ordine (`comandato_su`), non il flag a fine corsa.
    - Visto in prova: Type 2 alzato su SCENE1B, poi il Type 18 al suo posto con l'ESM nella testa.
  - Da fare:
    - un ordine `scegli_periscopio` al secondo 0 si perde (segnalato al runtime);
    - lo stadimetro sdoppiato coi comandi;
    - la TV in centrale;
    - le minicartoline delle foto;
    - P5 taratura.
- 25/10/2026, sera (principale):
  - **stadimetro sdoppiato** (Type 2 e Type 18). H apre l'immagine fantasma, tarata a 3000 m sull'altezza d'albero
    (quella della classe se identificata, altrimenti 30 m). Le parentesi quadre la spostano del 2% (con Maiuscolo del
    10%) finché la testa d'albero fantasma tocca il galleggiamento. H legge R = altezza / tan(angolo). La precisione è
    quella dell'allineamento del giocatore, senza estrazioni a caso. Messaggio 395;
  - **TV in centrale:** col Type 18 alzato, fuori dalla stazione Periscopio la vista in alto è il monitor TV. Il bianco
    e nero è chiesto al kit;
  - **foto nel debriefing:** la minicartolina della foto è la vista dall'occhio del battello, nell'oculare del
    periscopio con la fotocamera, al rilevamento e all'ingrandimento dello scatto. Al runtime è chiesto di rendere
    l'evento «noto»;
  - l'ordine `scegli_periscopio` al secondo 0 ora funziona (runtime).
- 25/10/2026, taratura P5 (runtime), con la Krivak II:
  - a mare 2 il radar vede la testa: R50 di circa 10 km con 10 s d'esposizione, in linea coi 4-5 nm;
  - a mare 5 il radar non vede, resta la vista: il Type 18 a 10 kn si nota per la piuma;
  - in agguato l'IA guarda ogni 7 minuti.
  - Approvato: f_op 0,3 per le navi non allertate.
- Meteo M6: σ della quota periscopica ×0,4 («il comandante tiene la quota»), perché la finestra coperta torni ai
  valori del dossier 21 §8.8.
