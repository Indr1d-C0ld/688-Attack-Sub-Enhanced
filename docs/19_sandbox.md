# La sandbox: il «vetrino» dell'IA (roadmap 32)

Piano approvato dall'utente il 27/10/2026. Nasce dalla richiesta dell'utente del 26/10: scegliere una missione e guardarla
svolgersi **senza nessun intervento umano**, come un vetrino da microscopio. Serve a vedere come si concretizzano
dottrine, ROE, allerta, meteo e sensori in tante situazioni, e a confrontarle con statistiche tecniche e replay.

Si appoggia su quello che c'è già:
- le fasi 1-4 delle interazioni fra IA (docs/18), con la ritaratura chiusa il 27/10;
- la navigazione incerta per tutti (roadmap 26);
- il replay di trailer-1 (app/replay: letture «verità» e «quadro», timeline, controlli);
- la traccia e le statistiche del disassembler (nucleo/traccia_replay, nucleo/statistiche);
- lo strumento delle scene di trailer-1 (strumenti/scene_interazioni, con `--semi N`);
- il codice partita (app/codice_partita).

## Principi

- **Solo Enhanced.** Il classico non cambia, e la sandbox non gira col livello classico.
- **Nessun privilegio.** Il battello che di solito è del giocatore lo comanda un **comandante IA**, con le stesse regole,
  gli stessi sensori e la stessa percezione di tutte le altre unità (docs/18, simmetria). Nessuna unità vede la verità.
- **Lo spettatore vede tutto, l'IA no.** Le viste per lo spettatore leggono la verità; il motore no.
- **Deterministica.** Stessa missione, stesse regolazioni e stesso seme danno la stessa partita, quindi la sandbox ha
  il suo codice partita e si può rigiocare o dare a un amico.
- **Fuori carriera.** Non conta per fascicoli, albo d'onore, riconoscimenti o statistiche cumulate del comandante.

## 1. Come si avvia

- Nella scelta della missione, un tasto nuovo (**V**, «vetrino») su una missione scelta in qualsiasi scheda: originali,
  rapide, libreria, editor, importate.
- Si apre la **schermata delle regolazioni** (§2), poi parte la partita in modo sandbox.
- Da riga di comando: `--sandbox` con la missione e le regolazioni, anche senza finestra (prove e serie, §6), sempre
  nell'Enhanced come dal menu.
- Le scene di prova S1-S7 (data/missioni/prove) diventano «vetrini pronti» nella scheda della libreria, con una riga
  che spiega cosa mostrano.

## 2. Le regolazioni

Per **parte** (NATO, Patto di Varsavia, neutrali):
- situazione di partenza: pace, crisi, guerra, oppure quella della missione;
- ROE per categoria, oppure quelle della missione;
- livello dell'IA, cioè esperienza dei sonaristi e dei comandanti: recluta, regolare, esperto;
- allerta di partenza (A0-A3), oppure quella della missione;
- carico delle armi: quello della missione o quello per classe e anno.

Globali:
- meteo fisso o realistico; epoca dei sensori (1989 o moderno); navigazione perfetta o realistica;
- interruttori di confronto delle fasi IA: difesa, armi, reazioni, forze, ESM, sonaristi. Sono quelli della taratura,
  qui spiegati in chiaro;
- seme, oppure uno nuovo;
- durata massima, e se fermarsi all'esito della missione.

Le regolazioni entrano nel **codice sandbox**, un codice partita esteso con una versione propria.

## 3. Le viste dal vivo

Una sola schermata, la stessa del replay ma dal vivo. Si sceglie la lettura con **V** e l'unità con **Tab**:
- **Verità**: tutte le unità vere coi simboli NTDS per parte, scie, armi in acqua col filo, alberi alzati, emissioni
  (ping, radar), zone e aree d'azione, ellissi della navigazione.
- **Quadro di una parte**: quello che la parte sa nell'insieme, cioè i contatti delle sue unità e i rapporti radio
  (Missione::quadro_radio), con le ellissi e l'età dei dati.
- **Quadro di un'unità**: i contatti di quell'unità con la qualità; la sua TMA (gambe, soluzione); l'allerta; la
  reazione in corso; la posizione stimata e l'ellisse; le armi e i tubi.
- **Registro**: la colonna degli eventi dell'IA che scorre (scoperte, classificazioni, ingaggi, lanci, esche,
  evasioni, reazioni, messaggi, allerta, INCSEA), con filtri per parte e per tipo. Un clic porta la carta sull'evento.
- Una **scheda dell'unità** scelta: classe, stato, velocità, quota, danni, sistemi.

Controlli:
- P pausa;
- compressione da 1× a 256× senza i limiti di bordo, con l'arresto facoltativo sugli eventi importanti;
- Tab e Maiuscolo+Tab: unità successiva e precedente;
- frecce: salto all'evento precedente o successivo del registro;
- S: segui l'unità;
- I e O: zoom;
- Esc: fine della partita, che porta al dopo.

La vista periscopio o 3D di un'unità può venire dopo, con la rifinitura estetica (roadmap 33).

## 4. Il dopo

- Il **replay** completo, con la traccia che c'è già, e con tutte le letture di §3.
- Il **debriefing dello spettatore**: tutti gli eventi, noti o no, con le minicartoline.
- Le **statistiche tecniche** della partita, per parte e per unità:
  - catene di scoperta: chi ha visto chi, quando, a che distanza, con quale sensore, quanto dopo il primo contatto
    possibile;
  - tempi di reazione: dalla scoperta all'allerta, all'ingaggio, al lancio, all'evasione;
  - TMA: gambe, tempo alla soluzione, errore della soluzione al lancio;
  - armi: lanciate, a segno, Pk per arma, esche e loro effetto, guida a filo;
  - esposizione: tempo con albero o periscopio su, emissioni attive, contro-scoperte;
  - navigazione: errore medio e massimo, punti nave;
  - esito: perdite e scambio delle perdite.

## 5. Il comandante IA del battello

È il pezzo che manca. Il runtime lo costruisce coi comportamenti che l'IA ha già:
- **Obiettivo:** dagli obiettivi della missione e dal briefing, cioè area da raggiungere, bersaglio da trovare,
  punto da difendere. Per le originali, quando gli obiettivi non bastano, una tabella per missione con la fonte
  nel briefing.
- **Condotta:** transito e pattuglia per dottrina, ascolto, TMA a gambe, attacco con la soluzione secondo le ROE,
  difesa con esche ed evasione, punti nave se la navigazione è realistica, ritirata se colpito.
- **Comandi:** gli stessi ordini del giocatore (RegistroOrdini), così replay e codice partita funzionano come sempre.

## 6. Le serie

`sandbox --semi N`, uno strumento senza finestra: gioca la stessa missione con le stesse regolazioni su N semi e scrive
il riassunto, cioè le medie e le dispersioni delle statistiche di §4, più le frequenze degli esiti. Una schermata lo
mostra con istogrammi semplici. Serve anche alla taratura e alle prove di regressione dell'IA.

## 7. Chi fa cosa

| Pezzo | Chi |
|---|---|
| Comandante IA del battello; partita senza giocatore; le regolazioni nella missione (situazione, ROE, livello, allerta, carico, per parte); il quadro di una parte | runtime |
| Menu e tasto V, schermata delle regolazioni, codice sandbox, viste dal vivo e controlli, debriefing dello spettatore, aggancio di tutto | principale |
| La carta delle viste, riusata dal replay (letture «parte» e «unità», registro filtrabile), d'accordo con la principale | trailer-1 |
| Strumento delle serie e la sua schermata; i vetrini pronti dalle scene S1-S7 | trailer-1 |
| Statistiche tecniche dalla traccia (nucleo), e quello che serve nella traccia per calcolarle | disassemblatore |
| Grafica: cornici della schermata delle regolazioni e delle viste, simboli, il capitolo del manuale | kit |

## 8. Fasi

1. **V1, il vetrino minimo:**
   - comandante IA e partita senza giocatore;
   - tasto V e regolazioni essenziali (situazione, ROE, seme, meteo, navigazione);
   - lettura «verità» dal vivo e registro;
   - replay e debriefing dopo.
2. **V2, le letture:** «parte» e «unità», regolazioni complete, codice sandbox, statistiche tecniche.
3. **V3, le serie:** lo strumento e la sua schermata, i vetrini pronti, il manuale.

Dopo ognuna: suite, rilievo del classico (0/110), e una partita sandbox per ogni scena S1-S7 su tre semi.

## Stato

- 27/10/2026: proposta scritta dalla principale e approvata dall'utente. Avvio di V1:
  - runtime: comandante IA, partita senza giocatore, regolazioni (prima il contratto);
  - trailer-1: modo «dal vivo» del replay e colonna del registro;
  - principale: tasto V, regolazioni, modo sandbox in main;
  - disassembler: dati della traccia per le statistiche (V2);
  - kit: prima stesura dei capitoli del runtime nel manuale, così il runtime resta sulla sandbox.
- 27/10/2026 (runtime): **V1, la parte del runtime** (contratto nella nota condivisa sandbox-v1-runtime-contratto.md,
  approvato dalla principale).
  - **Comandante IA** (`missioni/include/missioni/comandante_ia.hpp`, `missioni/src/comandante_ia.cpp`):
    - vede solo quello che vede il giocatore: la carta (`segni_contatti`, `barra_bersaglio`: la classe solo se nota), la
      posizione stimata, la stazione armi, gli obiettivi (`RegistroMissione::aree_obiettivi`), le ROE ricevute;
    - la parte di un contatto la deduce dalla classe nota, come l'IA;
    - fasi: transito verso l'area, pattuglia con gli sprint and drift, TMA a gambe con la propria soluzione (e un ping
      dopo 5 minuti o due gambe), attacco con la soluzione entro 15 km (designazione, rotta, RTE e quota di ricerca dalla
      soluzione, lancio; al più due siluri per bersaglio), difesa (fuga, salto di quota, esca dal tubo), punto nave
      con la navigazione incerta, ritirata se colpito o senza siluri;
    - comanda solo con `Ordine`: main li mette nel `RegistroOrdini` prima di ogni passo; nel replay bastano gli ordini
      (prova «gli ordini registrati rifanno la partita senza di lui»);
    - registro `comandante_ia` (fase, motivo, bersaglio).
  - `Missione::senza_giocatore` e `sandbox()`; `regolazioni(map lato → RegolazioniParte)`: situazione di partenza per
    parte e ROE per categoria (anche per la sala radio del giocatore); `interruttori_ia(InterruttoriIa)`.
  - Prove: `test/prove_comandante_ia.cpp`. Nella TORPEX (SCENE1A): pattuglia, TMA a 5 minuti, attacco a 10, tre lanci;
    esito 5, perché in 15 minuti non affonda tutti e tre i Forrest Sherman.
  - **In V2:**
    - livello, allerta e carico per parte; `quadro_parte`;
    - le liste per le statistiche del disassembler (R1-R6);
    - per il comandante: guida a filo, radio, mine, missili, esca semovente, tabella per le originali dove gli
      obiettivi non bastano, ROE di artiglieria e ASW aereo per categoria.
- 27/10/2026, **V1 chiusa.**
  - Il comandante IA del runtime (missioni/comandante_ia) comanda il battello con gli stessi ordini del giocatore.
  - Ci sono le regolazioni per parte (situazione e ROE), gli interruttori dell'IA e la partita senza giocatore, coi
    testi in terza persona.
  - Il tasto V e le regolazioni sono della principale.
  - La vista dal vivo è di trailer-1 (app/replay, dal vivo), alimentata da Partita::dopo_fotogramma, con la
    compressione fino a 256×.
  - Codice sandbox di 24 caratteri (P e --codice); dalla V2 è di 30, con livello, allerta, carico e durata.
  - La traccia v2 col RaccoltaTraccia del disassembler.
  - Istantanea senza finestra: `--sandbox --schermata vivo`.
  - Suite 718/718, classico 0/110, manuale aggiornato (§11.6, appendice A, E6).
  - Prossima: V2 (letture «parte» e «unità», livello, allerta e carico per parte, statistiche tecniche).
- 27/10/2026, sera (runtime): **V2, la parte del runtime** (contratto nella nota condivisa
  sandbox-v2-runtime-contratto.md, approvato dalla principale).
  - **Regolazioni per parte complete** (`Missione::RegolazioniParte`):
    - `livello`: sonaristi e dottrina del filo di tutte le unità dell'IA della parte (`esperienza_di_ia`); vince
      sull'avversario della carriera e sulla chiave `sonarista` dell'unità;
    - `allerta`: A0-A3 di partenza della forza e delle formazioni della parte; vince sul minimo della situazione solo
      alla partenza (una forza ad A0 in guerra è una forza sorpresa), poi le salite e le discese vanno come sempre;
    - `carico_per_classe`: l'IA ha già il carico per classe e anno; per il battello del giocatore main chiama
      `SalaSiluri::carico_standard()` al posto di `carico_iniziale()`.
  - **Liste per le statistiche tecniche** (R1-R6, `missioni/src/missione_quadro.cpp`), accese con
    `Missione::registra_statistiche(true)` prima del primo passo; la partita normale non registra niente:
    - `scoperte_contatti()`: nuovo, solido e perso di ogni coppia, al passo esatto (sensore vuoto: il contatto dato
      dalla missione o dallo script);
    - `scoperte_possibili()`: il primo istante fisicamente possibile per (osservatore, bersaglio, sensore): passivo,
      eco, ping intercettato, radar, vista, ESM;
    - `lanci()`: siluri del modello e missili di tutti, con la stima del lanciatore (`tma`, `contatto`, `giocatore`).
      Il giocatore la riceve da main con `soluzione_giocatore` (la soluzione del MATE); il comandante IA la passa da
      sé prima di ogni lancio. `EventoTmaIa::stima` nel tipo soluzione;
    - `evasioni()` (`ManovraEvasione`): l'inizio di ogni manovra d'evasione dei sottomarini dell'IA, con la minaccia;
    - `emissioni()`: ping, radar (dall'accensione allo spegnimento) e radio di tutti;
    - `punti_nave_ia()`.
  - **Quadro di una parte** (`quadro_parte(lato)`, `VoceQuadro`): una voce per bersaglio dell'altra parte col dato
    migliore fra contatti e rapporti radio (posizione da un sensore, soluzione della TMA dell'IA con l'ellisse, rapporto
    radio con l'ellisse in tondo, solo rilevamento), con le fonti. Nella traccia lo scrive il disassembler (record P,
    per lato, ogni 5 s quando cambia).
  - Per la lettura «unità» di trailer-1: `tma_ia(id)` (bersaglio, gambe, soluzione) e `tubi_carichi(id)`.
  - Prove: `test/prove_sandbox_v2.cpp` (livello, forza sorpresa, R1-R6 nella TORPEX col comandante, quadro, TMA e
    tubi). Classico 0/110.
  - **Il comandante IA in V2** (`missioni/comandante_ia.cpp`):
    - guida a filo dei suoi siluri con la dottrina dell'IA del suo livello (`GuidaFiloIa`, `Missione::dottrina_filo_ia`),
      coi comandi via filo del giocatore (`comando_filo`, `taglia_filo`, `spegni_siluro`); mentre guida sta sotto i
      12 nodi e accosta con 5° di barra;
    - ricarica dei tubi vuoti dalla rastrelliera (la ricarica realistica) e armamento di tutti i tubi carichi;
    - missili (Harpoon, TASM) sulle navi con la posizione fra 15 e 100 km, da quota periscopica;
    - mine: con l'obiettivo `minare` va a 12 km dal campo e lancia le Mk 67 con rotta e corsa a ventaglio sul cerchio
      dell'obiettivo, ricaricando i tubi; finché ci sono mine da posare non attacca (resta la difesa);
    - difesa: anche l'esca semovente dal tubo, di traverso alla minaccia;
    - radio: il rapporto di contatto (`radio_trasmetti`, contatto) al primo ostile dell'obiettivo o sottomarino ostile
      riconosciuto, a quota periscopica con l'antenna, al più ogni 10 minuti; l'ascolto del broadcast alle finestre
      della sala radio (non durante la TMA né l'attacco);
    - fasi nuove `radio` e `mine`.
  - **ROE per parte anche per l'artiglieria e l'ASW aereo** (`Missione::roe_parte`): libere, i pezzi sparano come in
    guerra e l'aereo sgancia; trattenute, niente tiro (nemmeno d'avvertimento) e l'aereo caccia senza sganciare.
  - **In V3:** la tabella per le originali dove gli obiettivi non bastano (`data/motore/sandbox_originali.toml`): è una
    ricerca sui briefing, missione per missione; oggi il comandante pattuglia sul posto quando manca l'area.
- 27/10/2026, sera (principale): **V2, la parte della principale e del modulo dei simboli.**
  - Il modulo comune dei simboli (kit: grafica/simboli_carta) disegna anche il debriefing: colori NTDS, telai APP-6,
    scie attenuate, armi orientate, boe e battello verde, con l'alone nero. Lo usa anche la croce della soluzione TMA
    dell'Enhanced.
  - `--sandbox` da riga di comando vale sempre per l'Enhanced, come dal menu.
  - Statistiche: le cariche e i razzi senza unità d'arma contano fra lanci, a segno e Pk (disassembler).
  - Controlli:
    - regressione: impronte uguali, cambiano solo le immagini e2 ed e4 per il contorno (riferimento aggiornato);
    - classico 0/110;
    - `tools/regressione/sandbox_scene.sh` (le nove scene di prova su tre semi): 27 partite senza errori;
    - suite 734/735: l'unica prova fallita era la S6 dell'acustica, per la scena riscritta, ora corretta dal
      disassembler.
  - Mancano, prima di chiudere V2:
    - il comandante del runtime: guida a filo, esca semovente, missili, mine, radio, ROE di artiglieria e ASW aereo
      per categoria;
    - due difetti dell'IA trovati nelle scene, girati al runtime: il Mk 46 dell'SH-60B sul mercantile amico in S5, le
      salve ASW senza limite in S1G seme 2;
    - i colpi d'artiglieria fra unità dell'IA nel registro, per il Pk.
  - In V3 anche la tabella per le originali, dove gli obiettivi non bastano: è un lavoro di dati sui briefing.
  - **Correzioni viste nelle scene sandbox** (solo Enhanced):
    - l'ASW aereo lancia siluri e bombe solo su un sottomarino: una traccia delle boe finita su una nave (un mercantile)
      non si attacca (S5, seme 2: un SH-60B sul mercantile amico);
    - le salve antisom delle navi (ATTACK) hanno una dotazione per classe (`Missione::salva_asw`, STIMA: Kara 20,
      Kresta II, Udaloy e Spruance 16, Grisha e Petya 8, le altre 12); prima erano infinite (S1G, seme 2: 67 salve
      per nave);
    - i colpi dell'artiglieria fra unità dell'IA entrano nel registro come `colpo`.
  - **API Lua** per le regole delle missioni: `m688.classificato(u [, b])` e `m688.tempo_contatto(u [, b])`
    (`Contatto::prima_scoperta`), per la libreria di trailer-1 (S1G).
- 27/10/2026, notte: **V2 chiusa.**
  - Comandante IA (runtime):
    - guida a filo, esca semovente, missili, mine e radio;
    - ROE di artiglieria e ASW aereo per parte;
    - il livello della parte del battello, NATO o Patto.
  - Salve ASW dell'IA dell'Enhanced con un tetto per classe (STIMA, runtime); nella libreria 2 la prima salva dopo
    13 minuti di caccia e una pausa d'ascolto dopo 6 salve (trailer-1).
  - Statistiche: le cariche contano fra lanci e Pk (disassembler).
  - Controlli, build pulita delle 21:26 più la correzione del livello:
    - suite verde (741, la prova acustica corretta dopo la rifinitura del fondo);
    - classico 0/110;
    - regressione: classico identico, Enhanced con le impronte nuove per l'acustica e l'IA di stasera (riferimento
      aggiornato);
    - sandbox_scene.sh: 27 partite senza errori, al più una dozzina di salve per nave in S1G.
  - Restano aperti, girati al runtime: il Mk 46 dell'SH-60B sul mercantile amico in S5, e i colpi d'artiglieria fra
    unità dell'IA nel registro (per il Pk).
  - Prossima: V3.
- 27/10/2026, notte: **V3 avviata**, contratti approvati (note condivise sandbox-v3-serie-contratto.md e
  sandbox-v3-aggregazione-contratto.md).
  - trailer-1: lo strumento `serie_sandbox` (un processo melchior688 --sandbox per seme, il riassunto in serie.toml), la
    schermata delle serie (tre pagine: esiti, per parte, istogrammi con le partite da cliccare per il replay) e
    `data/missioni/prove/vetrini.toml`.
  - disassembler: `riassunto_partite` e l'esito nelle statistiche.
  - kit: il cap. 12 del volume 1 e F8 del volume 2, i grafici delle serie (grafica/grafici_serie).
  - principale:
    - senza finestra il vetrino finisce come in finestra (durata, esito);
    - `--seme` vince sul seme del codice;
    - i codici delle missioni di prove/ si ritrovano;
    - l'arresto facoltativo sugli eventi (§3) fra le regolazioni e nel codice;
    - il vetrino fuori dalle statistiche cumulate;
    - i vetrini pronti nella scheda Editor;
    - S dalle regolazioni gioca una serie di 10 semi e la mostra;
    - `--serie FILE` apre una serie.
  - Poi, la stessa notte:
    - nella pagina degli istogrammi ogni partita ha il suo segno e il seme, e in pagina 1 c'è il Pk per arma (trailer-1);
    - il manuale descrive serie e vetrini (cap. 12, F8, appendice A; kit);
    - suite 749/749, regressione identica.
  - 28/10:
    - S1 e S6 ferme e pronte; tutti e 9 i vetrini hanno pronto = true (trailer-1);
    - le 27 partite di sandbox_scene.sh escono pulite;
    - il kit ha rigenerato la tabella dei vetrini nel manuale; la regressione è identica.
  - Mancava, prima di chiudere V3, la tabella per le originali: fatta la mattina del 28/10 (sotto).
- 28/10/2026 (runtime): **il piano del comandante per le 22 originali** (nota condivisa
  sandbox-v3-originali-contratto.md, aggiunte approvate dalla principale).
  - `data/motore/sandbox_originali.toml`, da `tools/pipeline/sandbox_originali.py`: per ogni originale la condotta
    (transito, caccia, scorta, pedinamento, evasione, attacco_terra), le armi (libere, roe, solo_difesa, classi), i
    punti, l'area, l'uscita, il rettangolo da cui non uscire, la rotta pianificata del convoglio scortato (PIM),
    distanza e impulso del pedinamento, l'attesa, il carico dei tubi. Ogni voce cita la frase del briefing e il gestore
    dello script o l'unità amica da cui vengono le coordinate. Il piano non usa mai le posizioni dei nemici, che fra
    le varianti cambiano: solo briefing, rettangoli degli script e rotte delle unità amiche (uguali in tutte le
    varianti).
  - Il caricatore `missioni/sandbox_originali.{hpp,cpp}`: `piano_originale(cartella_dati, id)` e
    `applica_tubi_piano(sala, piano)` per il carico prima del primo passo (0A: quattro TASM nei tubi da siluro, la
    dottrina dei 688 senza VLS; sul Flight II nei pozzi). `OpzioniComandante::piano`; in main due righe della
    principale.
  - Il comandante (`comandante_ia.cpp`) col piano: transito sui punti seminando chi pedina (scatto lungo la rotta,
    dall'altra parte dello strato e non verso la costa, poi deriva), scorta in stazione davanti alla PIM, pedinamento
    con la TMA senza impulsi e il contatto designato, evasione a scatti e derive, i Tomahawk da quota periscopica col
    periscopio alzato, l'attesa, il rettangolo anche nella fuga da un siluro.
  - Corretti nel comandante di V2, trovati qui (valgono anche per le scene S della sandbox):
    - la quota nell'Enhanced: `quota_con_angolo` e `giri_per_velocita` come la stazione del giocatore, gli ordini
      rimandati quando nasce la dinamica, e qualche nodo in più finché il battello non arriva alla velocità voluta;
    - la quota periscopica a 6,096 m (la scala della missione): a 18 m antenna e periscopio si ammainavano da soli e
      i missili non partivano. La principale ha corretto lo stesso numero per il giocatore (`PostoEnhanced`, 5,5 m);
    - l'antenna rialzata se l'Enhanced l'ha ammainata; il broadcast al più ogni 30 minuti (prima ogni finestra);
    - i contatti non classificati (la classe arriva dopo 1400 s): la parte dal colore della carta, la superficie dal
      simbolo, come li vede il giocatore;
    - la quota mai a meno di 25 m dal fondo (si era posato nel passo di Kunashir); la fuga solo da un'arma entro 5 km
      quando se ne sa la posizione (scappava dai siluri sul convoglio).
  - Prova: `test/diag_originali.cpp` (il piano di ogni originale nella geometria della missione; con `DIAG_ORIGINALE`
    una partita). Risultati in `tools/taratura/risultati/originali_v3/`: su 66 partite (22 × 3 semi, 2 ore) 13
    successi col piano e 13 senza. Il piano cambia la condotta, non le vittorie: vinte sempre 8A, 8C, 9A; perse
    sempre 1A, 2A, 2B, 9B; senza esito in 2 ore 1B, 1C, 3B, 5B, 5C, 7A, 7C.
  - Poi, la stessa mattina, la 0A nell'Enhanced (decisioni della principale):
    - i quattro Tomahawk nei tubi da siluro (TASM: il TLAM-C non chiama il gestore della missione) e, finché ce ne sono,
      niente fuga: l'esca e i lanci, la fuga dopo; il passo non arma due volte lo stesso tubo;
    - `Missione::colloca` sposta all'avvio il battello del giocatore solo sotto l'acqua minima (pescaggio + 0,5 m) o in
      acqua chiusa, anche nel classico: la 0A partiva a 10-35 km dal punto dell'originale e non vedeva il bersaglio dei
      missili. Rilievo: 3 partite della 0A cambiano solo t_scoperto, riferimento aggiornato
      (`engine/strumenti/riferimento/LEGGIMI.md`);
    - il comandante tiene la chiglia vera a 5 m dal fondo (`Missione::chiglia_vera`, ora pubblica): in 15-16 m resta in
      superficie e lancia da lì; le tappe lunghe seguono la rotta per acqua navigabile e le secche si evitano entro
      2 km.
    - Giro lungo rifatto: 13 successi col piano (fallimenti 18 → 17, battelli persi 5 → 6); la 0A lancia in tutte le
      varianti e fugge senza toccare il fondo. Suite 750/750, casi identici, rilievo 0/110 sul riferimento nuovo.
- 28/10/2026, mattina: **V3 chiusa** (principale). In `main.cpp` il piano delle originali (`piano_vetrino`, passato al
  comandante come `oc.piano`) e il carico dei tubi del piano prima del primo passo (`applica_tubi_piano`, sopra il
  carico per classe o quello iniziale). Fra le correzioni della stessa notte:
  - Ctrl+P dell'Enhanced a 5,5 m nella scala della missione (`PostoEnhanced::QUOTA_PERISCOPICA_M`; prima 18 m, e il
    periscopio non usciva);
  - la fascia periscopica delle statistiche a 2-7,1 m (disassemblatore), larga almeno 4 pixel nel grafico.
  - Controllo finale su una build da pulito: casi identici, rilievo 0/110 sul riferimento nuovo, 27 partite di
    `sandbox_scene.sh` senza errori, suite 750/750. Nell'app, sul seme 1, la 0A lancia i quattro TASM a 221-251 s.
  - Resta aperto, a parte: il TLAM-C non chiama il gestore «tomahawk» della missione (nella 0A conta solo il TASM).
  - Da provare con l'utente, in finestra: audio, vista dal vivo, scorrevolezza.
