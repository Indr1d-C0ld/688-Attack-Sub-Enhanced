# Volume 2 — Riferimento tecnico

*Indice approvato (26/10/2026).* Ogni capitolo spiega a fondo un modello della simulazione, sullo
schema di `SCHEMA_AUTORI.md`: a che serve, il modello con formule e parametri, fonti e confidenza, classico ed
Enhanced, come si vede in gioco, taratura e prove, limiti noti.

La colonna «autore» dice chi scrive il capitolo; la principale assegna i capitoli via via.

## Parte A — Fondamenti

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| A1 | Il motore | passo fisso di 1/30 s, determinismo e seme, storico degli ordini, impronta della partita; cosa usa il classico e cosa l'Enhanced; la regressione delle 110 partite | principale |
| A2 | Unità, coordinate e tempo | piedi, nodi, metri; coordinate dei teatri e proiezione locale; ora di missione e gruppo data-ora | principale |
| A3 | Le missioni originali | la VM degli scenari (118 opcode), la conversione in Lua, i gestori del giocatore, la regola d'impatto ricavata dall'eseguibile | disassemblatore |

## Parte B — Il battello

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| B1 | Dinamica del sottomarino | spinta e resistenza, timone (Nomoto), quota e assetto, autorità dei timoni orizzontali, cavitazione con la quota, battello appesantito | runtime |
| B2 | Casse, assetto e governo automatico | casse principali e d'assetto, soffiare e sfogare, compressione dello scafo, Tan, Boksit, posarsi sul fondo | runtime |
| B3 | Danni del battello | compartimenti, falle e portata con la quota, paratie, pompe, sistemi graduati, squadre e priorità, incendi ed equipaggio, SCRAM, collasso dello scafo resistente | kit |

## Parte C — Acustica e sensori

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| C1 | L'equazione del sonar | SL − TL − (NL − DI) ≥ DT, passivo e attivo, eccesso di segnale e probabilità di scoperta, cadenza della scoperta | disassemblatore |
| C2 | Propagazione | profili di velocità del suono (WOA23) per teatro e mese, strato, zone d'ombra e di convergenza, canale SOFAR, fondale, perdite | disassemblatore |
| C3 | Rumore ambiente | curve di Wenz, mare, pioggia, traffico, biologici, ghiaccio, fulmini; rumore proprio e di flusso | disassemblatore |
| C4 | Firme acustiche | banda larga con velocità e quota, toni, pale e DEMON, cavitazione, transitori, variazioni individuali | disassemblatore |
| C5 | Elaborazione e presentazione | cascate, LOFAR, DEMON e DEMON a cascata, integrazione, tracker, righe automatiche, storia lunga, classificazione con la confidenza, sonarista dell'equipaggio, modo 1989 e moderno, righe del proprio battello e maschera | disassemblatore |
| C6 | Antenna rimorchiata | filatura e recupero, catenaria e quota, fondo, danni e reti, ambiguità, rumore di flusso, rischi, taglio | runtime |
| C7 | Sonar attivo e BQS-15 | impulsi, riverbero, contro-scoperta, sonar per le mine, intercettazione e allarme siluro | runtime |
| C8 | Audio | mixer deterministico, sintesi dalle firme, campioni a licenza libera, cuffia, suoni a bordo attraverso lo scafo, resa Sound Blaster | disassemblatore |
| C9 | Periscopi | ottiche e modalità del Type 2, Type 18 e dei periscopi sovietici, stadimetria, luce e notte, esposizione della testa al radar e alla vista, mare sul vetro, libretto di riconoscimento, luci di navigazione | runtime (modello), kit (vista) |
| C10 | Radar ed ESM | emettitori e bande, intercettazione, identificazione, rischio d'essere visti, EMCON dell'IA | runtime |
| C11 | Radio | VLF ed ELF, filo galleggiante, trasmissioni circolari a orario, telescrivente, trasmettere e radiogoniometria, boe SLOT | runtime |
| C12 | Navigazione | carta d'epoca NATO e sovietica, errori di posizione, fondali reali, incagli, strumenti della carta, quadro del fondale (ecoscandaglio e BQS-15) | kit, con la revisione della principale per gli strumenti |
| C13 | TMA | rilevamenti, strumento MATE, proposta dal CPA, regolo di Ekelund, regola del 1934, qualità della soluzione | principale |

## Parte D — Armi

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| D1 | Siluri | modelli per tipo, impostazioni (modo, velocità, RTE, soffitto e pavimento, schema), ricerca e aggancio, guida via filo, testa contro esca, probabilità di colpire | runtime |
| D2 | Esche e contromisure | ADC Mk 2, MOSS, MG-74, Nixie; vita, quota, efficacia contro teste passive, attive e filoguidate | runtime |
| D3 | Missili e difese | Harpoon e Tomahawk dal sottomarino, piano di tiro, volo e seeker, pagina di tiro di F7, difese delle navi, esiti delle salve | runtime |
| D4 | Mine | Mk 67, Mk 60 CAPTOR, PMR-1; posa, armamento, innesco, campi dell'IA, BQS-15 | runtime |
| D5 | Carico e ricarica | deposito, varianti, anni di servizio, carichi standard, posti, tempi e rumore della ricarica, silenzio a bordo | disassemblatore |

## Parte E — Il mondo

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| E1 | Esplosioni | bolla e pulsazioni, frusta dello scafo, colonne d'acqua e fenomeni in superficie per carica e quota, esplosioni nell'aria | kit |
| E2 | Danni e affondamento delle navi | idrostatica per stazioni a qualunque assetto, allagamento e superfici libere, stabilità, trave di scafo e tronconi, incendi e depositi, controllo danni, modi di affondare, tarature sui casi storici | kit |
| E3 | Collisioni | energia d'urto con le masse aggiunte, capacità degli scafi a cascata, Minorsky, vela, duomo, squarci; avvisi e conseguenze | kit (danni), runtime (missione) |
| E4 | Artiglieria | tiro delle navi, dispersione, colpi d'avvertimento, danni a teste, alberi, vela e scafo; artiglieria fra navi | runtime (tiro), kit (danni) |
| E5 | ASW aereo | elicotteri e pattugliatori, boe sonore, sonar a immersione, MAD, armi, dottrine USA e URSS, difficoltà | runtime |
| E6 | L'IA | livelli (dottrina, missione, tattica, manovra), allerta e «paranoia», pilota del fondale, simmetria, interazioni fra unità (in arrivo) | runtime |
| E7 | Meteo e mare | climatologia dei teatri, vento e onde (Pierson-Moskowitz), stato del mare, visibilità, nebbia, temporali e fulmini, effetti su ogni sistema | runtime (modello), kit (dati e vista) |
| E8 | Giorno, notte e cielo | posizione del Sole e della Luna, luce ambiente, crepuscoli, illuminanti | kit |
| E9 | Grafica | la tela 640×480 a 256 colori, palette per scena, kit 3D e sprite, navi danneggiate dal modello, effetti, consolle e font, la rifinitura estetica | kit |
| E10 | Traffico civile | rotte e zone di pesca dei teatri, i tipi civili, la presenza nella missione | disassemblatore (rotte e modello), runtime (missione) |
| E11 | Sotto il ghiaccio | il campo procedurale, la volta e il sondatore verso l'alto, l'acustica del pack, i falsi echi delle chiglie, le regole di bordo, l'urto della vela, l'IA sotto la banchisa | disassemblatore (campo, volta, acustica), runtime (regole, urto, IA) |

## Parte F — Attorno alla missione

| # | Capitolo | Contenuto | Autore |
|---|---|---|---|
| F1 | Registro e debriefing | eventi, interesse, minicartoline, esito e obiettivi | runtime (registro), principale (schermate) |
| F2 | Carriera | punteggio di missione, gradi, commissioni, decorazioni con le soglie per pace, crisi e guerra, rimozione dal comando | disassemblatore |
| F3 | Achievement e albo d'onore | i 41 achievement e i nascosti, le classifiche, i comandanti storici | disassemblatore |
| F4 | Statistiche | report di missione dalla traccia, cumulate, primati, heatmap | disassemblatore |
| F5 | Replay e ripresa | traccia campionata, «verità» e «quadro del giocatore», riprendere da un istante | disassemblatore (traccia), principale (ripresa) |
| F6 | Missioni rapide | il generatore deterministico, i parametri, il codice di 13 caratteri | trailer-1 |
| F7 | Editor e pacchetti | lo scenario, i trigger, il formato `.m688m`, l'ambiente Lua isolato, la libreria | trailer-1 |
| F8 | La sandbox | la partita senza giocatore, le regolazioni, il codice sandbox, la vista dal vivo, le statistiche tecniche, le serie, i vetrini pronti, il piano delle originali | kit (curatore), dal codice di principale, runtime, trailer-1 e disassemblatore |

## Appendici generate dai dati

Le genera `tools/pipeline/manuale.py` dai file di `data/motore`; nessuna si scrive a mano.

| App. | Tabella | Fonte |
|---|---|---|
| G1 | Classi: dimensioni, velocità, pescaggio, vela | `classi.toml`, `scafi/*.toml` |
| G2 | Scafi: quote di collaudo e collasso, compartimenti, masse | `scafi/*.toml` |
| G3 | Siluri | `siluri.toml` |
| G4 | Carico: armi per marina, posti, anni di servizio, efficacia | `carico.toml` |
| G5 | Firme acustiche | `acustica.toml` |
| G6 | Emettitori radar | `emettitori.toml` |
| G7 | Periscopi | `periscopi.toml` |
| G8 | Aerei ed elicotteri ASW | `asw_aereo.toml` |
| G9 | Pezzi d'artiglieria | `artiglieria.toml` |
| G10 | Meteo dei teatri | `meteo.toml` |
| G11 | Gradi e decorazioni | `carriera.toml`, `decorazioni_grafica.toml` |
| G12 | Riconoscimenti | `achievement.toml` |
| G13 | Tasti | `tasti_predefiniti.toml` |
| G14 | Messaggi di bordo | i messaggi del motore e della traduzione |

## Stato dei capitoli

| Capitolo | File | Stato |
|---|---|---|
| A1 Il motore | `A1_motore.md` | rivisto (principale; curatore: kit), 29/10/2026; col bit del traffico e il codice partita v2, col GPS |
| A2 Unità, coordinate e tempo | `A2_unita_coordinate_tempo.md` | rivisto (principale; curatore: kit), 26/10/2026 |
| B3 Danni del battello | `B3_danni_del_battello.md` | rivisto (kit), 28/10/2026 |
| A3 Le missioni originali | `A3_missioni_originali.md` | rivisto (disassemblatore, curatore kit), 26/10/2026 |
| C1 L'equazione del sonar | `C1_equazione_del_sonar.md` | rivisto (disassemblatore, curatore kit), 28/10/2026; con la schiera sovietica |
| C2 Propagazione | `C2_propagazione.md` | rivisto (disassemblatore, curatore kit), 28/10/2026; con l'acqua bassa e le frange d'interferenza |
| C3 Rumore ambiente | `C3_rumore_ambiente.md` | rivisto (disassemblatore, curatore kit), 27/10/2026 |
| C4 Firme acustiche | `C4_firme_acustiche.md` | rivisto (disassemblatore, curatore kit), 28/10/2026; con le navi civili generate e le righe legate all'asse |
| C5 Elaborazione e presentazione | `C5_elaborazione_e_presentazione.md` | rivisto (disassemblatore, curatore kit), 28/10/2026; con le righe del proprio battello |
| C8 Audio | `C8_audio.md` | rivisto (disassemblatore, curatore kit), 28/10/2026; con le registrazioni del blocco 3 |
| D5 Carico e ricarica | `D5_carico_e_ricarica.md` | rivisto (disassemblatore, curatore kit), 26/10/2026 |
| C12 Navigazione | `C12_navigazione.md` | rivisto (kit), 29/10/2026; con la navigazione incerta, il GPS 1990+ e il quadro del fondale; gli strumenti della carta da rivedere con la principale |
| C13 TMA | `C13_tma.md` | rivisto (principale; curatore: kit), 28/10/2026; con la proposta dal CPA e la TMA dell'IA corretta |
| E1 Esplosioni | `E1_esplosioni.md` | rivisto (kit), 28/10/2026 |
| E2 Danni e affondamento delle navi | `E2_danni_e_affondamento_navi.md` | rivisto (kit), 28/10/2026 |
| E3 Collisioni | `E3_collisioni.md` | rivisto (kit e runtime; curatore: kit), 29/10/2026; con il suono a bordo, il rollio al periscopio e gli effetti nella missione |
| E8 Giorno, notte e cielo | `E8_giorno_notte_cielo.md` | rivisto (kit), 29/10/2026; la luce della missione col coperto e `--lat`/`--lon` |
| E9 Grafica | `E9_grafica.md` | rivisto (kit), 29/10/2026; con la rifinitura estetica (cascate, lamiera, quadro, documenti del briefing e del debriefing, gruppo 4, libretto di riconoscimento) |
| E10 Traffico civile | `E10_traffico_civile.md` | rivisto (disassemblatore; runtime per la missione; curatore: kit), 28/10/2026 |
| E11 Sotto il ghiaccio | `E11_sotto_il_ghiaccio.md` | rivisto dagli autori (disassemblatore; runtime per §2.5-2.7; curatore: kit), 31/10/2026 |
| B1 Dinamica del sottomarino | `B1_dinamica_del_sottomarino.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| B2 Casse, assetto e governo automatico | `B2_casse_assetto_governo.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| C6 Antenna rimorchiata | `C6_antenna_rimorchiata.md` | rivisto (runtime; curatore: kit), 28/10/2026; con quota, fondo e danni |
| C7 Sonar attivo e BQS-15 | `C7_sonar_attivo_e_bqs15.md` | rivisto (runtime; curatore: kit), 28/10/2026; con l'intercettazione |
| C9 Periscopi | `C9_periscopi.md` | rivisto (runtime; curatore: kit), 29/10/2026; col libretto di riconoscimento, il quadrante e l'AOB; le luci di navigazione (§2.17) |
| C10 Radar ed ESM | `C10_radar_ed_esm.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| C11 Radio | `C11_radio.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| D1 Siluri | `D1_siluri.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| D2 Esche e contromisure | `D2_esche_e_contromisure.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| D3 Missili e difese | `D3_missili_e_difese.md` | rivisto (runtime; curatore: kit), 28/10/2026; col piano, il volo, la pagina di tiro, il TLAM e i lanci dalle navi al periscopio |
| D4 Mine | `D4_mine.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| E4 Artiglieria | `E4_artiglieria.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| E5 ASW aereo | `E5_asw_aereo.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| E6 L'IA | `E6_ia.md` | rivisto (runtime; curatore: kit), 28/10/2026; con la parte del luogo (M7) |
| E7 Meteo e mare | `E7_meteo_e_mare.md` | rivisto (runtime; curatore: kit), 27/10/2026 |
| F1 Registro e debriefing | `F1_registro_e_debriefing.md` | rivisto (principale; curatore: kit), 29/10/2026, col debriefing nella variante B e i colpi mancati; la sezione del registro (§2.1): rivisto (runtime; curatore: kit), 27/10/2026 |
| F2 Carriera | `F2_carriera.md` | rivisto (disassemblatore, curatore kit), 26/10/2026 |
| F3 Riconoscimenti e albo d'onore | `F3_riconoscimenti_e_albo_onore.md` | rivisto (disassemblatore, curatore kit), 26/10/2026 |
| F4 Statistiche | `F4_statistiche.md` | rivisto (disassemblatore, curatore kit), 28/10/2026 |
| F5 Replay e ripresa | `F5_replay_e_ripresa.md` | rivisto (principale; curatore: kit), 26/10/2026; traccia e replay rivisti (disassemblatore, curatore kit), 27/10/2026, con la traccia v2 |
| F6 Missioni rapide | `F6_missioni_rapide.md` | rivisto (trailer-1, revisione del kit), 26/10/2026 |
| F7 Editor e pacchetti | `F7_editor_e_pacchetti.md` | rivisto (trailer-1, revisione del kit), 26/10/2026; con la libreria e le scene di prova dello spettatore |
| F8 La sandbox | `F8_sandbox.md` | rivisto (kit), 29/10/2026; con le serie nella variante C, i nove vetrini pronti, il piano delle originali e la parte del luogo |

Le tabelle generate che ci sono già: `tasti`, `scafi_sottomarini`, `compartimenti_battelli`, `scafi_navi`,
`sistemi_navigazione`, `punti_nave` (`tools/pipeline/manuale.py`); `rapide_tipi_teatri`, `rapide_forze`,
`libreria_missioni`, `scene_prova` (`tools/pipeline/manuale_missioni.py`); `sensori_sonar`, `suite_sonar`,
`profili_suono`, `sorgenti_ambiente`, `firme_acustiche`, `righe_firme`, `transitori_acustici`,
`campioni_audio`, `armi_carico`, `locali_siluri`, `carichi_standard`, `fasi_carriera`, `soglie_decorazioni`,
`carriera_originali`, `giudizi_carriera`, `achievement_elenco`, `achievement_nascosti`
(`tools/pipeline/manuale_acustica.py`).
