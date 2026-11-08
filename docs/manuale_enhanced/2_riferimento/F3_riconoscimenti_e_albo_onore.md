# F3. Riconoscimenti e albo d'onore

*Autore: disassemblatore. Revisione del 26/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Tiene il conto di quello che il comandante ha fatto, oltre il punteggio: affondare senza mai pingare, passare fra le
boe senza farsi vedere, copiare tutte le serie radio, giocare le 22 missioni originali. I **riconoscimenti**
(achievement) si prendono a livelli, per il comandante e per il giocatore. L'**albo d'onore** tiene le migliori
missioni e le migliori carriere chiuse, e le schede di otto comandanti veri della Guerra fredda. Vale con la regola di
simulazione; il classico non cambia.

## 2. Il modello

Il codice è in `nucleo/achievement.hpp` e `nucleo/albo_onore.hpp`, con l'adattatore `missioni/achievement_missione.hpp`.
I dati sono in `data/motore/achievement.toml` e `data/motore/albo_onore_storico.toml`, da
`tools/pipeline/achievement.py`.

### 2.1 I riconoscimenti

Sono 68 (13 aggiunti il 31/10/2026), in sette categorie: tattico, stealth (la discrezione), tecnico, storico,
carriera, editor e nascosto. Ognuno
ha:
- una **misura** (per esempio `affondati_ostili`, `siluri_evitati`, `serie_copiate`);
- i **livelli**: bronzo, argento e oro, oppure solo l'oro (per esempio 1, 5 e 20 sottomarini affondati);
- l'**ambito**:
  - `missione`: la misura di una sola missione;
  - `somma`: sommata sulle missioni, oppure i valori diversi di una chiave (le missioni originali, i codici delle
    rapide, le marine);
  - `carriera`: dal fascicolo (capitolo F2). Con `fascicolo_prima` conta solo quello che la missione cambia: il grado,
    le qualifiche e le decorazioni d'ingresso della carriera breve non si guadagnano;
- le **condizioni**, dichiarative: un elenco di (misura, operatore, valore), con `>=`, `>`, `<=`, `<`, `==`, `!=`. Per
  esempio «Solo orecchie» vuole `ping_attivi == 0`. Non c'è codice Lua: una misura sconosciuta nel file è un errore al
  caricamento.

`misure_achievement(missione, contesto, opzioni)` ricava le misure dall'esito, dal registro (capitolo F1), dal
contesto della carriera e dal fascicolo. `valuta_achievement` restituisce i livelli saliti; un livello non scende mai.

`colpi_subiti` è il conto dell'esito (capitolo F1): le armi a segno e gli spari a segno, diretti o di schegge, dove una
raffica vale uno; i colpi d'avvertimento non contano. Una missione presa a cannonate, quindi, non vale per i riconoscimenti che vogliono
`colpi_subiti == 0` (Elusione, Occhio alle mine, Aggirate, Sotto il rotore). `colpi_artiglieria_subiti` conta le sole
cannonate e serve a «Sotto il fuoco»: le due misure non si sommano.

`registra_achievement(catalogo, misure, comandante, conta_comandante, ripresa, data, missione)` legge e salva
`achievement.toml` accanto all'albo dei comandanti:
- per il comandante conta la missione che conta per la carriera (`Carriera::conta`, capitolo F2);
- per i riconoscimenti globali del giocatore conta ogni missione non ripresa;
- una missione ripresa da un istante del replay (`ContestoMissione::ripresa`) non conta per nessuno;
- una missione della libreria conta per la carriera come una dell'editor, cioè col blocco `carriera`.

<!-- generato: achievement_elenco -->
| Achievement | Categoria | Che cosa | Livelli | Ambito | Condizioni |
|---|---|---|---|---|---|
| Primo sangue | tattico | Affondare un'unità ostile (`affondati_ostili`) | oro 1 | in una missione | — |
| Cacciatore di sottomarini | tattico | Sottomarini ostili affondati (`sottomarini_affondati`) | bronzo 1, argento 5, oro 20 | sommato sulle missioni | — |
| Contro la flotta | tattico | Navi ostili affondate (`navi_affondate`) | bronzo 3, argento 15, oro 50 | sommato sulle missioni | — |
| Un siluro, un bersaglio | tattico | Tutti i lanci di una missione a segno (`tutti_a_segno`) | bronzo 3, argento 5, oro 8 | in una missione | — |
| Doppietta | tattico | Due unità ostili affondate in un minuto (`doppietta`) | oro 1 | in una missione | — |
| Elusione | tattico | Siluri evitati senza colpi subiti (`siluri_evitati`) | bronzo 1, argento 3, oro 5 | in una missione | `colpi_subiti == 0` |
| Missione compiuta | tattico | Tutti gli obiettivi contro un avversario regolare, veterano o d'elite (`avversario`) | bronzo 1, argento 2, oro 3 | in una missione | `completezza >= 1` |
| Posa di precisione | tattico | Almeno quattro mine posate, nessuna inerte (`mine_posate`) | oro 4 | in una missione | `mine_inerti == 0` |
| Oltre l'orizzonte | tattico | Un bersaglio affondato con un missile oltre l'orizzonte (`affondati_missile_oth`) | oro 1 | in una missione | — |
| Scatola pulita | tattico | Una salva con un neutrale nella scatola, senza colpirlo (`salve_neutrale_illeso`) | oro 1 | in una missione | — |
| Allarme siluro | tattico | Siluri evitati dopo l'allarme dell'intercettatore (`siluri_evitati_allarme`) | bronzo 1, argento 3, oro 5 | sommato sulle missioni | — |
| Campo minato | tattico | Un campo di mine attraversato col sonar da mine (BQS-15 o MG-70), senza danni (`campi_minati_attraversati`) | oro 1 | in una missione | — |
| Nella tempesta | tattico | Una missione compiuta con mare 6 o più (`uno`) | oro 1 | in una missione | `successo >= 1`; `mare_max >= 6` |
| Sotto il fuoco | tattico | Missione compiuta dopo essere stati presi a cannonate (`uno`) | oro 1 | in una missione | `successo >= 1`; `colpi_artiglieria_subiti >= 1` |
| Occhio alle mine | tattico | Mine scoperte col sonar da mine, senza colpi subiti (`mine_scoperte`) | bronzo 5, argento 20, oro 50 | sommato sulle missioni | `colpi_subiti == 0`; `battello_perso == 0` |
| Aggirate | tattico | Almeno tre mine scoperte, nessuna avvicinata a meno di 200 m dopo (`mine_aggirate`) | oro 3 | in una missione | `colpi_subiti == 0` |
| Fantasma | stealth | Riuscita senza fix, avvistamenti nè ping subiti; poi senza contatto ASW; poi senza transitori nè ping propri (`fantasma`) | bronzo 1, argento 2, oro 3 | in una missione | `successo >= 1` |
| Silenzio radio | stealth | Riuscita senza trasmettere, con almeno una serie copiata (`serie_copiate`) | oro 1 | in una missione | `successo >= 1`; `trasmissioni == 0` |
| Fra le boe | stealth | Boe ostili vicine, senza mai essere avvistati (`boe_vicine`) | bronzo 1, argento 3, oro 6 | in una missione | `avvistamenti == 0`; `battello_perso == 0` |
| Solo orecchie | stealth | Un affondamento senza nessun ping attivo (`affondati_ostili`) | oro 1 | in una missione | `ping_attivi == 0` |
| Passaggio ravvicinato | stealth | A meno di 900 m da un sottomarino ostile, senza essere scoperti (`sottomarino_vicino`) | oro 1 | in una missione | `avvistamenti == 0` |
| Testa bassa | stealth | Missioni riuscite con meno di un minuto di periscopio fuori (`uno`) | bronzo 3, argento 10, oro 25 | sommato sulle missioni | `successo >= 1`; `esposizione_periscopio_s < 60` |
| Sotto la chiglia | stealth | Oltre 10 minuti con meno di 20 m sotto la chiglia, senza toccare (`sotto_chiglia_20_s`) | oro 600 | in una missione | `incaglio == 0` |
| Nei suoi baffles | stealth | Minuti in coda a un sottomarino (entro 3 nm, a poppa, col contatto), senza che lui ci classifichi (`in_coda_s`) | bronzo 1800, argento 3600, oro 7200 | in una missione | — |
| Sotto il rotore | stealth | Sorvoli di elicotteri ASW, senza avvistamenti nè colpi (`sorvoli_elicottero`) | bronzo 1, argento 3, oro 6 | in una missione | `avvistamenti == 0`; `colpi_subiti == 0` |
| Fra le code | stealth | Navi col VDS o la schiera filati passate entro 5 km, senza essere scoperti (`navi_traino_vicine`) | bronzo 1, argento 3, oro 6 | in una missione | `avvistamenti == 0` |
| Orecchio fino | tecnico | Classificazioni giuste, nessuna sbagliata (`classificazioni_giuste`) | bronzo 3, argento 6, oro 10 | in una missione | `classificazioni_errate == 0` |
| Prima di tutti | tecnico | Classificazione in meno di due minuti in media (almeno tre) (`classificazioni_giuste`) | oro 3 | in una missione | `anticipo_medio_s < 120` |
| Controllo danni | tecnico | Danni domati con il battello salvo (`danni_domati`) | bronzo 1, argento 5, oro 15 | sommato sulle missioni | `battello_perso == 0` |
| Al limite | tecnico | A meno del 5% dalla quota di collasso, e ritorno (`uno`) | oro 1 | in una missione | `margine_collasso < 0.05`; `battello_perso == 0` |
| Fotografo | tecnico | Foto col periscopio Type 18 (`foto_periscopio`) | bronzo 5, argento 20, oro 60 | sommato sulle missioni | — |
| Telegrafista | tecnico | Tutte le serie copiate e tutti i rapporti inviati (`serie_copiate`) | bronzo 2, argento 4, oro 6 | in una missione | `serie_perse == 0`; `rapporti_mancati == 0` |
| Alla lettera | tecnico | Missioni con tutti gli ordini eseguiti e le ROE rispettate (`uno`) | bronzo 3, argento 10, oro 25 | sommato sulle missioni | `ordini >= 1`; `ordini_mancati == 0`; `ordini_rispettati >= 1` |
| Occhio al libretto | tecnico | Identificazioni visive giuste al periscopio (`visive_giuste`) | bronzo 1, argento 10, oro 50 | sommato sulle missioni | — |
| Telemetrista | tecnico | Una misura del telemetro entro il 5% della distanza vera (`telemetro_entro_5`) | oro 1 | in una missione | — |
| Angolo sulla prora | tecnico | Un AOB entro 10 gradi del vero, mandato alla TMA (`aob_entro_10`) | oro 1 | in una missione | — |
| Prima la vista | tecnico | Un contatto identificato a vista prima del sonar, giusto (`prima_la_vista`) | oro 1 | in una missione | — |
| Punto nave | tecnico | Finire con la navigazione incerta e l'errore sotto i 500 m, senza GPS (`uno`) | oro 1 | in una missione | `navigazione_incerta >= 1`; `gps == 0`; `errore_finale_m < 500` |
| Non autenticato | tecnico | Un ordine radio non autenticato riconosciuto e non eseguito (`non_autentici_respinti`) | oro 1 | in una missione | — |
| Per l'intelligence | tecnico | Righe nuove nel rapporto ACINT, non spiegate dalla libreria (`righe_nuove_acint`) | bronzo 1, argento 5, oro 10 | sommato sulle missioni | — |
| Polynya | tecnico | Emergere nella banchisa, in acqua libera o nel ghiaccio sottile, senza urti col ghiaccio (`emersioni_ghiaccio`) | oro 1 | in una missione | `urti_ghiaccio == 0` |
| Leggere i fanali | tecnico | Identificazioni visive giuste di notte coi soli fanali, oltre la portata della sagoma (`visive_fanali_giuste`) | bronzo 1, argento 5, oro 15 | sommato sulle missioni | — |
| A luci spente | tecnico | Di notte, una nave a luci spente identificata giusta (`visive_luci_spente`) | oro 1 | in una missione | — |
| Catalogo | tecnico | Classi diverse classificate giuste, in tutte le partite (`uno`) | bronzo 10, argento 30, oro 60 | sommato sulle missioni (valori diversi di «classi_giuste») | — |
| Le missioni del 1989 | storico | Missioni originali diverse riuscite (`uno`) | bronzo 5, argento 12, oro 22 | sommato sulle missioni (valori diversi di «missione») | `successo >= 1`; `originale >= 1` |
| Silenzio nel Barents | storico | Riuscita nel Barents senza ping nè trasmissioni (`uno`) | oro 1 | in una missione | `successo >= 1`; `teatro == barents`; `ping_attivi == 0`; `trasmissioni == 0` |
| Sotto il ghiaccio | storico | Missioni artiche riuscite (`uno`) | bronzo 1, argento 3, oro 6 | sommato sulle missioni | `successo >= 1`; `artico >= 1` |
| Sangue freddo | storico | Una crisi riuscita senza aprire il fuoco senza ordine (`uno`) | oro 1 | in una missione | `successo >= 1`; `crisi >= 1`; `ordini_rispettati >= 1` |
| Guerra fredda, guerra calda | storico | Missioni di guerra riuscite (`uno`) | bronzo 1, argento 5, oro 10 | sommato sulle missioni | `successo >= 1`; `guerra >= 1` |
| Dalle due parti | storico | Missioni riuscite col 688 e con l'Alfa (`uno`) | oro 2 | sommato sulle missioni (valori diversi di «marina») | `successo >= 1` |
| Tetto bianco | storico | Ore sotto la banchisa senza urti col ghiaccio (`sotto_banchisa_s`) | bronzo 3600, argento 18000, oro 72000 | sommato sulle missioni | `urti_ghiaccio == 0` |
| Soccorso | storico | Tutti gli uomini in acqua salvati (`tutti_salvati`) | oro 1 | in una missione | — |
| Le storie vere | storico | Missioni storiche diverse della libreria riuscite (24 in tutto) (`uno`) | bronzo 5, argento 12, oro 24 | sommato sulle missioni (valori diversi di «missione») | `successo >= 1`; `libreria_storica >= 1` |
| Dolphins | carriera | La qualifica di sommergibilista (dolphins o VSOK) (`qualifica_sommergibilista`) | oro 1 | dal fascicolo | — |
| Al comando | carriera | Commander o kapitan 2-go ranga (`grado_ordine`) | oro 4 | dal fascicolo | — |
| Quattro galloni | carriera | Captain o kapitan 1-go ranga (`grado_ordine`) | oro 5 | dal fascicolo | — |
| Decorato | carriera | Decorazioni personali (`decorazioni_personali`) | bronzo 1, argento 5, oro 12 | dal fascicolo | — |
| Al valore | carriera | Silver Star o Bandiera Rossa; Navy Cross o Ordine di Lenin; Medal of Honor o Eroe dell'URSS (`decorazione_massima`) | bronzo 1, argento 2, oro 3 | dal fascicolo | — |
| Veterano | carriera | Missioni in carriera (`missioni_carriera`) | bronzo 15, argento 30, oro 60 | dal fascicolo | — |
| Fine servizio | carriera | Una carriera chiusa in pensione (`pensione`) | oro 1 | dal fascicolo | — |
| Autore | editor | Missioni dell'editor portate a termine (`uno`) | bronzo 1, argento 5, oro 20 | sommato sulle missioni | `editor >= 1` |
| Codici condivisi | editor | Codici di missioni rapide diversi giocati (`uno`) | bronzo 5, argento 20, oro 50 | sommato sulle missioni (valori diversi di «codice») | `rapida >= 1` |
| Biblioteca | editor | Missioni della libreria diverse riuscite (41 in tutto) (`uno`) | bronzo 5, argento 20, oro 41 | sommato sulle missioni (valori diversi di «missione») | `successo >= 1`; `libreria >= 1` |
<!-- fine: achievement_elenco -->

**I nascosti.** La bacheca li mostra con un «?» finché non si prendono. Chi non vuole saperli salti la tabella.

<!-- generato: achievement_nascosti -->
| Achievement | Categoria | Che cosa | Livelli | Ambito | Condizioni |
|---|---|---|---|---|---|
| Cinquecento iarde | nascosto | Cinque minuti entro 500 iarde da un sottomarino straniero, senza urto e senza essere scoperti (`entro_500yd_s`) | oro 300 | in una missione | `collisioni == 0`; `avvistamenti == 0` |
| Sasso nella scarpa | nascosto | Un incaglio, e la missione riesce lo stesso (`incaglio`) | oro 1 | in una missione | `successo >= 1` |
| Per un pelo | nascosto | Un siluro ostile passato a meno di 50 m (`uno`) | oro 1 | in una missione | `distanza_min_siluro_m < 50`; `battello_perso == 0` |
| L'ultimo siluro | nascosto | Riuscita con nessuna arma rimasta a bordo (`uno`) | oro 1 | in una missione | `successo >= 1`; `armi_a_bordo == 0` |
| Pendolare | nascosto | Un passaggio ravvicinato con un'unità che non sapevamo lì (`quasi_collisione_ignota`) | oro 1 | in una missione | — |
<!-- fine: achievement_nascosti -->

### 2.2 L'albo d'onore

`albo_onore.toml`, accanto all'albo dei comandanti:
- **le 20 migliori missioni** (`proponi_missione`): entrano solo senza aiuti (realismo 1) e mai se riprese. Le rapide
  hanno il loro codice, per rigiocarle. A parità di punteggio resta prima la più vecchia;
- **le 10 migliori carriere chiuse** (`proponi_carriera`): congedo, pensione, rimosso, trasferito o morto; i caduti
  restano. Ognuna ha la somma dei punti delle missioni, la decorazione più alta per precedenza e lo stato. La voce di
  un comandante già presente si aggiorna;
- una voce resta anche se il comandante si toglie dall'albo dei comandanti;
- `registra_missione_onore` e `registra_carriera_onore` leggono e salvano il file. Il modulo non legge l'orologio:
  la data reale la passa l'app.

**I comandanti storici** (`carica_storici`): otto schede fisse dal dossier 13 §3.3 e §3.6, con il battello, il
comandante, l'impresa, la decorazione, la fonte e la confidenza, senza punteggi. Le schede sono nel volume 3.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Elenco, livelli e condizioni dei riconoscimenti | dossier 15 §7.10; proposta approvata il 26/10/2026 | scelta di gioco |
| Regole dell'albo d'onore | dossier 15 §7.10 | scelta di gioco |
| Comandanti storici | dossier 13 §3.3 e §3.6 (fonte per scheda) | per scheda |

## 4. Classico ed Enhanced

Solo Enhanced, con la regola di simulazione. Il classico non ha riconoscimenti né albo d'onore.

## 5. Come si vede in gioco

- **Nuovi riconoscimenti**: la pagina dopo l'esito, con quelli appena presi (volume 1, §10.5).
- **Bacheca** (B dall'albo dei comandanti): i medaglioni per categoria, il livello raggiunto, la condizione, la data,
  la missione e l'avanzamento («12 su 20»); G alterna il comandante e il giocatore.
- **Albo d'onore** (O dall'albo dei comandanti): migliori missioni col codice da ricopiare, migliori carriere,
  comandanti storici.

## 6. Taratura e prove

Prove in `test/prove_achievement.cpp`:
- «achievement: il catalogo, i livelli e le misure note»;
- «achievement: una missione, le somme, i valori distinti; i livelli non scendono»;
- «achievement: il file, i globali e il comandante, le riprese»;
- «achievement: le misure di una missione vera e del fascicolo»;
- «albo d'onore: migliori missioni senza aiuti, carriere chiuse coi caduti, comandanti storici».

## 7. Limiti noti e in arrivo

- I riconoscimenti sono locali: niente classifiche in rete.
- Le soglie dei livelli sono di gioco, scelte perché bronzo arrivi presto e oro richieda una carriera.

## 8. Riferimenti

- Codice: `nucleo/achievement.hpp`, `nucleo/src/achievement.cpp`, `nucleo/albo_onore.hpp`,
  `nucleo/src/albo_onore.cpp`, `missioni/achievement_missione.hpp`; `app/albo_onore.cpp`, `app/bacheca.cpp`.
- Dati: `data/motore/achievement.toml`, `data/motore/albo_onore_storico.toml` (da `tools/pipeline/achievement.py`).
- Ricerca: dossier 13 §3.3 e §3.6; dossier 15 §7.10.
