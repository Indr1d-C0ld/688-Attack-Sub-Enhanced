# Interazioni fra unità dell'IA e simmetria giocatore/IA (roadmap 31)

Piano approvato dall'utente il 26/10/2026. Fonti:
- il dossier di ricerca `docs/ricerca/24_interazioni_fra_unita.md`, citato «24 §»;
- l'inventario delle asimmetrie del runtime, nella nota condivisa `ia-simmetria-inventario.md`, citato «inv. §».

## Principio

Di regola tutto quello che il giocatore può fare e vedere lo può fare anche l'IA, secondo i mezzi e il contesto, verso
di noi e fra unità dell'IA: nemico contro amico e amico con amico. Il contesto comprende:
- teatro e situazione (pace, crisi, guerra);
- dottrine e ROE di ogni parte;
- unità, sensori e armi;
- meteo;
- livello d'allerta secondo chi c'è nel teatro.

Il giocatore deve poter assistere alle interazioni, e saperne solo quello che il suo equipaggio può sentire o vedere.

## Vincoli

- **Il classico resta identico all'originale.** Non si toccano:
  - `passo_sensori`, `valuta`, `scelta_bersaglio`, la rotazione di `visita`, gli opcode di `API[]`;
  - le `SCENE*.lua`, `esm_`;
  - il pool di `nuovo_contatto`, fuori dagli interruttori dell'Enhanced (inv. §0).
- **Registro:** i tipi d'evento nuovi vanno in coda, perché gli id sono chiavi del replay.
- **Dati:** un solo file, `data/motore/interazioni.toml` dalla pipeline (`tools/pipeline/interazioni.py`). Ogni valore
  ha fonte e confidenza (24 §8.10).
- **Taratura:** dopo le fasi 2 e 4 si rifanno le prove ASW, la G7 e le scene di prova. L'IA diventa più forte, e la
  difficoltà resta la leva per il giocatore.

## Fasi

| Fase | Contenuto | Chi | Stima |
|---|---|---|---|
| **1 Correzioni** | Parte neutrale vera (24 §8.7): **fatta** il 26/10 da trailer-1. Posa delle mine dell'IA senza campo iniziale (inv. §10). Siluri dell'IA lanciati sul contatto e non sulla posizione vera (inv. §6) | trailer-1, runtime | P |
| **2 Percezione simmetrica** | Stessa regola del passivo per tutti (solo rilevamento senza TMA); ESM e radar contro gli alberi per tutti, con le emissioni del giocatore scopribili; teste e alberi dell'IA visti dall'IA; meteo nel punto del ricevitore (inv. §1-4, §16) | runtime con disassembler (acustica, sonarista) | M-G |
| **3 Allerta e comunicazioni** | Allerta della forza per parte e teatro con eventi, salita e discesa (24 §8.1), riusando `paranoia`; messaggi di contatto fra unità dell'IA con ritardo ed ellisse, sugli stessi canali della sala radio (24 §8.2); zone per i sottomarini amici e fuoco amico (24 §8.3); provocazione per parte e ROE dell'IA, così anche un attacco fra IA fa salire la crisi (inv. §9, §12) | runtime | M |
| **4 Comportamento** | Tabelle di reazione per situazione (24 §8.4): alleato scoperto, attaccato, affondato. Difesa dell'IA: esche, evasione dai siluri, ritirata dei colpiti, anticollisione (inv. §7, §13-15). Armi dell'IA: carico, varianti, guida a filo (inv. §5-6). Pace e INCSEA: pedinamento, marcatura, divieti (24 §8.5) | runtime con disassembler (armi e carico) | M-G |
| **5 Spettatore e taratura** | Eventi dell'IA nel registro (`lancio_ia`, `scoperta_ia`, `ingaggio_ia`, `evasione_ia`, `artiglieria_ia`, `collisione_ia`, in coda, con `noto`); nell'app quello che l'equipaggio può sapere (sonarista, radio, vedetta), e nel debriefing e nel replay tutto. Le scene S1-S7 del 24 §8.9 come missioni di prova in `data/missioni/prove/`, con partite su più semi | registro: runtime; app: principale; scene: trailer-1 | M |

L'ordine è 1 → 2 → 3 → 4. La parte del registro della fase 5 va in parallelo da subito, così ogni fase si vede
lavorare.

## Scene di prova (fase 5)

Le sette scene del 24 §8.9, più la S8 della parte del luogo (M7), sono missioni in `data/missioni/prove/`, scritte con lo scrittore dell'editor da
`engine/strumenti/scene_interazioni.cpp`: una per scena, più le varianti S1G (guerra) e S3A (dal lato dell'Alfa). Il
menu non le mostra; si giocano con `--cartella-missioni data/missioni/prove`. Ogni scena ha nel commento del sorgente
le attese del dossier e quello che oggi le sostituisce.

| Scena | File | Teatro, situazione | Cosa c'è oggi al posto delle reazioni che mancano |
|---|---|---|---|
| S1 KPUG e SSN alleato | `PROVA_S1_KPUG`, `PROVA_S1G_KPUG_GUERRA` | Barents, crisi / guerra, notte | 2 Krivak e un Kara (al posto dell'Udaloy), 2 Ka-27 a 5 min col cue sul datum; il broadcast è un messaggio a 30 min |
| S2 pedinamento | `PROVA_S2_PEDINAMENTO` | Mar di Norvegia, pace | Perry in `R_pedina` a 5 km, SH-60B, P-3C che entra a 45 min; Crazy Ivan dell'Enhanced; contatto al giocatore a 30 min |
| S3 bastione | `PROVA_S3_BASTIONE`, `PROVA_S3A_BASTIONE_ALFA` | Barents, guerra, inverno | il sensore fisso è un cue di 18 km a 10 s; Il-38 da Severomorsk, KPUG, Ka-27 a 90 min, Victor in pattuglia nella sua zona |
| S4 due SSN amici | `PROVA_S4_DUE_SSN` | canale Faroe-Shetland, guerra | aree A e B e fascia come zone; lo spostamento dell'area è un messaggio a 15 min; un Victor attraversa |
| S5 convoglio | `PROVA_S5_CONVOGLIO` | Atlantico, guerra, mare 4 | 6 mercantili in 2 colonne, 3 Perry che cacciano solo dal primo colpo; il colpo fa accostare il convoglio di 60°, dà il cue e fa decollare l'SH-60B; FLASH al giocatore |
| S6 uscita dell'SSBN | `PROVA_S6_USCITA` | Kola, pace (dal lato sovietico) | Alfa del giocatore, Typhoon in superficie, nave spia norvegese, LA che aspetta, 4 pescherecci; il messaggio della nave spia è una regola a 30 min che manda il LA verso l'uscita |
| S7 portaerei | `PROVA_S7_PORTAEREI` | Mare di Alborán, crisi → guerra | Nimitz, 2 Spruance, Perry, 2 SH-3H; Victor in `R_pedina` a 6 nm, Kara a 3 nm; guerra per tutti a 60 min, il Victor passa a `R_sottomarino` a 75 min |
| S8 parte del luogo | `PROVA_S8_LUOGO` | Otranto, al largo di Valona; pace fra USA e URSS, il luogo in crisi con gli USA | Koni del luogo (M7) di pattuglia nel golfo, Foxtrot sovietico in superficie vicino a lei; a 15 min il luogo dichiara la guerra agli USA (azione «situazione» fra due parti) e la FLASH al giocatore |

**Prove** (`engine/test/prove_interazioni_scene.cpp`): i file si rileggono, si verificano senza errori, si riscrivono
uguali e si giocano 3 minuti su due semi senza errori Lua. Otto prove «da completare» (`doctest::may_fail`) portano le
attese del dossier; giocano fino a 90 minuti e girano solo con `M688_PROVE_LUNGHE=1`. Quando una passa stabilmente le si
toglie `may_fail`.

**Riassunto su più semi**: `scene_interazioni --semi 3` gioca ogni scena per la sua durata, col giocatore fermo ad
ascoltare, e scrive chi vede chi fra parti ostili e quando, le armi in acqua (chi, su chi), chi è colpito o esce;
l'ultimo è in `tools/scene_interazioni_riassunto.txt`.

**Cosa si vede oggi** (semi 1-3, 26/10/2026; giocatore fermo):

| Scena | Attesa del dossier | Oggi |
|---|---|---|
| S1 | VDS del Krivak di destra a t=0, gli altri a 1-3 min; Ka-27 sulle boe a 5-15 min; lo Sturgeon esce e il contatto si perde a 60-120 min | i tre tengono lo Sturgeon da 20-270 s e non lo perdono in 2 ore; i Ka-27 non lo prendono mai; il 688 a 15 miglia, fermo, è trovato a 22-65 min |
| S1G | attacco a ~20 min, dopo il PROBSUB | Sturgeon affondato a 160-190 s; il 688 fermo affondato a 69-72 min dalle cariche dei Krivak e del Kara |
| S2 | P-3 in contatto dopo 30-60 min; Crazy Ivan; due ping «ti tengo» | la Perry ha il Victor a 60-110 s; il P-3 non lo prende mai; il Victor si accorge della Perry a 100-110 min (2 semi su 3); nessun lancio |
| S3 | barriera di RGB-1 a 30-60 min, contatto al KPUG a 60-90 | il giocatore fermo non è trovato in 2 ore |
| S3A | come S3, visto dall'Alfa | il LA in movimento è trovato solo a 118-120 min, dalle navi; Il-38 e Victor mai |
| S4 | il Trafalgar sente un contatto sul confine e non lancia nella fascia | il Trafalgar trova il Victor a 7-29 min e lo attacca a 54-81 min (affondato in 1 seme, contrattacco del Victor in 2); il Trafalgar non ha il 688 fra i contatti nei primi 10 min |
| S5 | dopo il colpo, DIFAR in circolo a 5-15 min | la petroliera colpita a 100 s (2 semi su 3); le scorte affondano il Victor 8-19 s dopo |
| S6 | l'SSN fuori trova il Typhoon dopo il messaggio della nave spia (30-60 min) | lo trova a 20 s, insieme all'Alfa e ai pescherecci |
| S7 | alla guerra il Kara lancia per primo, il Victor dopo la sua sessione (10-20 min) | il Kara lancia a 3610 s (subito dopo la guerra) in tutti i semi; il Victor nello stesso istante in 1 seme su 3, e in quello due Spruance affondano a 27 e 32 min e il Victor a 37 |

In comune: i contatti dell'IA sono quasi immediati a 6-15 km (la scorciatoia della posizione dal passivo, nota
ia-simmetria-inventario §1, e la proposta della fase 2), gli aerei ASW non trovano i sottomarini, e le navi attaccano
appena hanno il contatto (S1G, S5): sono le cose che le fasi 2-4 devono cambiare, e le prove «da completare» le
controllano.

## Stato

- 26/10/2026: piano approvato. Fase 1: neutrali fatti (trailer-1). Avvio delle altre fasi.
- 26/10/2026: fase 1, siluri dell'IA lanciati sul contatto (runtime, `RTE_SOLO_RILEVAMENTO_M` col solo
  rilevamento). Fase 2, la parte del nucleo è fatta (disassembler):
  - TMA che risolve da sola per l'IA (`nucleo/tma_ia`);
  - mare sul rumore proprio dei sensori vicino alla superficie;
  - vantaggio fra le classi del 24 §5.1 dal motore entro 4 dB (688 → Victor III +16,7 contro +15).
  Il collegamento nella missione è del runtime.
- 26/10/2026 (runtime): **fase 1 fatta.**
  - Mine dell'IA posate anche senza un campo iniziale.
  - Siluri dell'IA lanciati sul contatto del lanciatore (`Missione::nuovo_siluro`):
    - con la posizione stimata: rotta, RTE e quota da quella;
    - col solo rilevamento: quello, RTE di 2000 m (STIMA) e la quota del lanciatore;
    - la verità solo senza nessun contatto, cioè lo script che lancia alla cieca.
    - Vale anche per i lanci del giocatore da script senza le impostazioni della stazione.
  - Prova: `prove_siluro.cpp`, «siluro dell'IA: si lancia sul contatto…».
- 26/10/2026 (runtime): **registro della fase 5 fatto**, coi tipi in coda.
  - `lancio_ia`: ogni lancio di un'unità dell'IA, di qualsiasi parte. `unita` = lanciatore, `altra` = bersaglio,
    `arma`, `classe_arma`.
  - `scoperta_ia`: il primo contatto visibile di un'IA su un'unità ostile, guardato una volta al secondo.
  - `ingaggio_ia`: l'IA aggancia (ENGAGE) un'unità ostile.
  - `evasione_ia`: una manovra d'evasione nuova; `valore` = quante finora.
  - `artiglieria_ia`: la prima salva di una nave su un'unità che non è il giocatore, per coppia; `valore` = calibro.
  - `collisione_ia`: un urto fra due unità dell'IA; `valore` = energia normale in MJ, `pos`.
  - `noto`: l'equipaggio del giocatore ha un contatto visibile su chi agisce (lancio, salva, urto). Scoperte,
    ingaggi ed evasioni sono decisioni interne: `noto` falso.
  - Prove: `prove_artiglieria.cpp` (fra_navi: `artiglieria_ia` e `scoperta_ia`) e `prove_siluro.cpp` (`lancio_ia`).
- 26/10/2026: fase 5, scene S1-S7 come missioni di prova (trailer-1): 9 missioni, strumento con il riassunto su più semi,
  prove; le 8 attese del dossier sono «da completare».
- 26/10/2026 (runtime): **fase 2, prima parte fatta.** Suite verde, classico 0/110.
  - **Passivo dell'IA col solo rilevamento.** La posizione viene dalla sua TMA (`nucleo/tma_ia` del disassemblatore):
    - per coppia ostile con traccia solida, un rilevamento ogni 20 s e una soluzione al minuto;
    - la posizione solo se la distanza è osservabile;
    - la quota dalla classe di superficie, se nota, altrimenti la propria.
    - Tolta la posizione col 10% d'errore, che fra l'altro sovrascriveva a ogni ascolto quelle precise dell'eco e del
      radar.
  - **Ping intercettato:** il solo rilevamento, a tutti.
  - **Meteo nel punto di chi ascolta** in `ambiente_acustico`: celle di 5 km rinnovate ogni 10 s. Vicino al giocatore
    resta il suo; col meteo fisso o semplificato non cambia niente.
  - **ESM dell'IA:** ogni unità con un ricevitore nel catalogo intercetta le emissioni. I contatti hanno il solo
    rilevamento (sensore «esm») e sono solo sugli emettitori ostili.
  - **Radar contro gli alberi di tutti i sottomarini esposti** (`Missione::radar_contro`): emersi, alberi, teste
    dell'IA. Per il giocatore la regola è la stessa di prima.
  - **Teste dell'IA viste** dalle vedette e dai periscopi dell'IA dell'altra parte, con la regola della testa del
    giocatore. Il periscopio dell'IA che guarda vede le navi entro la visibilità (15 km di giorno, 3 di notte: STIMA).
  - Prove: `test/prove_percezione_ia.cpp`.
  - Restano:
    - il radar del giocatore fra le emissioni (ordine `radar`, contratto alla principale);
    - il sonarista dell'IA, col disassemblatore;
    - il paesaggio per gli ascoltatori dell'IA;
    - il transitorio di lancio verso l'IA, che va con la fase 4.
  - **Effetto da tarare:** senza le manovre di TMA (fase 4) l'IA ricava di rado la posizione dal passivo; in cambio
    tiene quelle dell'eco e del radar. Nella SCENE8B le navi NATO affondano l'Alfa passivo in 7 minuti.
- 26/10/2026 (runtime): **fase 2, seconda parte.**
  - **Radar del giocatore:** `TipoOrdine::radar` e `radar_giocatore_acceso()`; messaggi 458 e 459. Le emissioni le sente
    l'ESM dell'IA, e il radar vede alberi e battelli emersi dell'altra parte.
  - **Sonarista dell'IA** (`missione_sonarista.cpp`, nucleo del disassemblatore):
    - un `Sonarista` per unità dell'IA;
    - livello dall'avversario del blocco carriera per le ostili al giocatore, regolare per le altre, chiave d'unità
      `sonarista`;
    - le righe LOFAR solo sulle coppie ostili con traccia solida;
    - la durata dei contatti acustici dell'IA resta sotto 800 finché il sonarista non li ha riferiti, sotto 1400
      finché non li ha classificati;
    - le classificazioni vanno nel registro come `classificazione_ia` (classe creduta, giusta, confidenza, noto
      falso);
    - `usa_sonaristi_ia(false)` per i confronti.
  - Restano il paesaggio per gli ascoltatori dell'IA e il transitorio di lancio, che va con la fase 4 (il paesaggio
    era già nella fase 2 F: vedi il 28/10).

- 27/10/2026: le scene S1-S7 rigiocate dopo la fase 2 (trailer-1; `scene_interazioni --semi 3`, giocatore fermo,
  `tools/scene_interazioni_riassunto.txt`). Confronto con il 26/10:
  - **Chi vede chi.** Quasi uguale: le prime scoperte dell'IA restano a 10-130 s a 6-15 km (S1 KPUG sullo Sturgeon
    20-130 s, prima 20-270; S7 le scorte sul Victor a 10-30 s, prima 80-100). Novità: un Ka-27 prende lo Sturgeon in S1
    (65 min, un seme su tre); il LA di S3A sente l'Alfa prima (37-46 min, prima 59-78). P-3, Il-38 e Victor di S3A
    ancora senza contatti; in S6 il LA vede il Typhoon sempre a 20 s.
  - **Armi in acqua.** Come prima in S4 (Trafalgar sul Victor a 52-69 min, il Victor risponde a 90-92 min), S5 (il
    Victor lancia a 30 s) e S7 (il Kara alla guerra in tutti i semi, il Victor anche lui in un seme su tre).
  - **Colpiti.** S1G come prima (Sturgeon a 160-210 s, il 688 fermo affondato a 73-110 min, prima 69-72). S5: la petroliera non è
    più colpita in nessun seme (i siluri del Victor, lanciati a 30 s, vanno a vuoto), quindi le scorte non reagiscono.
    S7: solo il Victor, affondato dagli SH-3H a 66-75 min.
  - **Due difetti dello scrittore trovati e corretti** (libreria 2, editor e libreria): un'azione di regola su
    un'unità già affondata finiva sul giocatore (in S7 il Victor passava a `R_sottomarino` da morto: errore Lua);
    `R_aereo_sopra` attaccava il bersaglio osservato senza guardare che fosse un sottomarino nemico (in S7 un SH-3H
    colpiva anche Nimitz, Spruance e Perry). Libreria e scene rigenerate.
  - Le otto attese «da completare» falliscono ancora tutte: nessun `may_fail` tolto.

- 27/10/2026 (runtime): **risposte alle scene e fase 3 (contratto nella nota condivisa ia-fase3-contratto.md, approvato
  dalla principale).**
  - Correzioni dalle scene:
    - la traccia solida dell'IA (su cui gli script accostano, ingaggiano e lanciano) arriva solo col rapporto del suo
      sonarista, come la durata;
    - `scoperta_ia` nel registro scatta su un contatto che l'IA può usare (traccia solida o posizione), non sul primo
      segnale;
    - nell'Enhanced `m688.attacca` non colpisce oltre la portata (per asse, almeno 2 nm) e non fa niente per un aereo
      guidato dal controllore ASW, che sgancia da sé con le sue regole;
    - `m688.soluzione(u)`: il bersaglio ha una posizione stimata, da chiedere prima di un lancio.
  - **A, allerta della forza** (`missioni/src/missione_forze.cpp`, dati in `[allerta]` di `interazioni.toml` da
    `tools/pipeline/interazioni_forze.py`):
    - A0-A3 per parte e per formazione (le unità della parte entro 20 km), dalla `paranoia` dell'editor;
    - eventi: POSSUB e PROBSUB dei sonaristi dell'IA, albero visto (radar e vista), ping nemico sentito, armi
      (lancio, colpo, salva), alleato affondato, situazione;
    - la forza sale al FLASH dalla formazione al teatro; discesa di 1 ogni 45 minuti fino ai minimi (A1 crisi, A2
      guerra);
    - effetti: navi al posto di combattimento da A2, EMCON di combattimento ad A3, falsi contatti dei sonaristi
      dell'IA (il biologico preso per sottomarino) ×1 / 1,5 / 2 / 3 da A0 ad A3, col setter del profilo del
      disassembler;
    - `m688.allerta(u)` (nil nel classico), registro `allerta_forza`.
  - **D, situazione per coppia e ROE dell'IA:**
    - in crisi un lancio, un colpo o una salva di una parte su un'altra porta in guerra la coppia; registro
      `provocazione`;
    - artiglieria e ASW aereo seguono la situazione della coppia;
    - `m688.armi_libere(u [, bersaglio])` sulla parte creduta (quella vera, salvo un errore di classe del sonarista);
    - i neutrali mai, salvo ad A3 su un contatto non classificato (P 0,3);
    - `m688.situazione(u, nome [, parte, parte])` per le regole che dichiarano la guerra.
  - **B, messaggi di contatto:**
    - un'IA riferisce un contatto ostile che può usare, quando sale la classificazione e ogni 10 minuti con la
      posizione;
    - canali: formazione (30-120 s; KPUG 60-180), aereo-nave (Be-12 12 min al 16%), teatro (FLASH o IMMEDIATE, URSS
      ×1,5), sottomarini al broadcast con l'antenna o a quota periscopica;
    - chi riceve ha il rapporto nel suo quadro radio (`contatto_radio`, `quadro_radio`) con l'ellisse `r0 + u·t`; un
      aereo dell'ASW aereo ne fa un datum;
    - il giocatore della parte riceve i rapporti come contatto d'intelligence (sensore `forza`) dalla sua sala radio;
    - registro `messaggio_ia`.
    - Scelta diversa dal contratto: il quadro radio sta a parte dai contatti, che il motore classico fa decadere senza
      segnale.
  - **C, zone e fuoco amico:**
    - area d'azione mobile di ogni sottomarino amico (15 nm avanti, 30 indietro, 5 per lato); per il giocatore le
      aree d'azione dichiarate dalla missione, se ci sono;
    - navi e aerei non usano armi antisom su un contatto subacqueo dentro, salvo un contatto classificato per classe
      (anche per errore) e la P della prima ora dopo un attacco (0,05 USA, 0,15 URSS);
    - zona ignota se il messaggio è perso;
    - `m688.in_zona_amica(u [, bersaglio])`, registro `fuoco_amico_ia`.
  - Restano per la fase 4:
    - le zone dentro il controllore dell'ASW aereo;
    - il FLASH «avviso radio» per l'IA.

- 27/10/2026 (runtime): **fase 4, parti A e B** (contratto nella nota condivisa ia-fase4-contratto.md, approvato dalla
  principale con tre condizioni: le originali tengono i loro tempi di lancio, lo script vince nello stesso istante,
  le firme a trailer-1).
  - **A, difesa** (`missioni/src/missione_difesa.cpp`, `usa_difesa`):
    - ogni sottomarino dell'IA evade ed esca su un siluro ostile sentito (3 km, con contatto o entro 1,5 km), su un
      ping ostile intercettato e sul transitorio di lancio entro 20 km (P 0,5: occhiata lungo il rilevamento e
      manovra antisiluro), non solo sull'aereo che lo caccia;
    - le navi con un contatto su un siluro ostile entro 4 km si allontanano a tutta forza per 3-5 minuti;
    - il sottomarino colpito si ritira dal lanciatore per 20 minuti;
    - anticollisione: CPA sotto 150 m entro 1000 m e quota vicina, 40 m di quota e accostata per 2 minuti;
    - le manovre le applica il movimento (`manovra_ia`), sopra lo script e sotto i limiti dei danni; finite, torna
      la velocità dello script;
    - registro `esca_ia` e `ritirata_ia`.
  - **B, armi** (`missioni/src/missione_armi_ia.cpp`, `usa_armi_ia`), coi moduli del disassembler:
    - carico dell'anno per ogni sottomarino dell'IA con i dati (`carico_ia`);
    - il lancio dello script prende dai tubi il siluro con più stelle contro il bersaglio (`scegli_arma_ia`, solo
      siluri), il tubo si ricarica in 5 minuti dalla riserva; senza siluri in tubo l'arma si perde alla nascita e il
      registro non la conta come lancio;
    - gambe di TMA (`ManovreTma`) su un contatto ostile col solo rilevamento, con le armi libere o l'ingaggio, sul
      bersaglio dello script o sul contatto più forte; finiscono con la soluzione o la rinuncia;
    - mai nelle originali (senza blocco `editor`); mai nell'istante in cui lo script ha dato rotta o velocità
      (`ordini_script_` in `salva_buffer`); la difesa ha la precedenza;
    - guida a filo (`GuidaFiloIa`) dei filoguidati dell'IA verso il contatto del lanciatore, coi vincoli del filo
      sulla sua velocità;
    - `m688.armi_rimaste(u [, famiglia])`, registro `tma_ia`.
- 27/10/2026 (runtime): **fase 4, parti C e D** (`missioni/src/missione_reazioni.cpp`, `usa_reazioni`; non nelle
  originali; nell'istante di un ordine dello script vince lo script).
  - **C, reazioni del 24 §8.4.** Eventi: alleato colpito (anche un lancio contro di lui sentito), alleato affondato,
    sottomarino ostile classificato da un sonarista dell'IA. Reagisce la formazione della parte, entro 20 km, dopo
    il ritardo della voce. Per ruolo:
    - nave antisom più vicina (30 km): sul datum; in guerra entro 1-15 minuti; su una scoperta in crisi «tiene
      sotto» col ping ogni 30 s;
    - altre navi: via dal punto (in guerra a tutta forza per 10-20 minuti);
    - sottomarini amici: fermi ad ascoltare a 3 nodi per 10 minuti; su una scoperta in crisi coprono con P 0,5;
    - aerei dell'ASW aereo: il datum (`cue_asw`).

    Registro `reazione_ia` (ruolo, reazione, causa). Le manovre passano da `manovra_ia`, in quest'ordine: difesa,
    reazione, TMA.
  - **D, pace e INCSEA del 24 §8.5:**
    - in pace la nave o il sottomarino dell'IA con un sottomarino ostile riferito dal suo sonarista lo pedina: nave
      3,5 km; sottomarino 6 km con la schiera, 1,2 km senza;
    - col solo rilevamento si va lungo il rilevamento;
    - la nave fa il ping «ti tengo» con la P del `ping` dell'allerta, ogni minuto;
    - un sottomarino dell'IA che riceve un ping ostile entro 2 nm in pace si allontana con P 0,7 (`ritirata_ia`
      «ping»);
    - navi di parti diverse non in guerra sotto i 1000 yd: registro `incsea`, e l'IA accosta via. La chiave della
      missione `incsea = false` lo toglie (lo scenario d'urto voluto);
    - gli aerei sotto i 500 ft entro 1 nm restano da fare: la quota la decide il controllore dell'ASW aereo (fatto il
      28/10).
- 27/10/2026 (runtime): **scene S1-S7 rigiocate con la fase 4** (`scene_interazioni --semi 3`, giocatore fermo; scene
  rigenerate da trailer-1 alle 04:10 con la libreria 2: `armi_libere`, `allerta`, `soluzione`, `armi_rimaste`,
  `m688.situazione` nelle regole, aree d'azione in S4; lo strumento conta la scoperta solo sul contatto usabile).
  Confronto col giro del 27/10 mattina:
  - **Chi vede chi:**
    - le scoperte dell'IA passano da 10-130 s a 80-320 s (S1: KPUG sullo Sturgeon a 160-320 s; S7: scorte sul
      Victor a 90-130 s, SH-3H a 450-720 s);
    - S2: la Perry ha il Victor a 21-43 minuti, prima a 60-110 s; il **P-3 lo trova a 79-86 minuti** in tutti i
      semi (prima mai), l'SH-60B in un seme a 54 minuti;
    - S6: il LA vede il Typhoon a 100 s, prima a 20 s;
    - S3A: il LA vede le navi a 2-11 minuti.
  - **Armi e colpi:**
    - S5: il Victor lancia dopo la soluzione, a 220-400 s (prima a 30 s, a vuoto), e **colpisce la petroliera in
      tutti i semi**; le scorte affondano il Victor a 17-108 minuti;
    - S4: il Trafalgar lancia sul Victor a 62-76 minuti, senza colpi (evasione ed esche del Victor); il Victor non
      contrattacca;
    - S7: alla guerra il Kara lancia sulla Perry, e gli SH-3H sganciano sul Victor da 3610 s, senza colpirlo; il
      Victor non lancia;
    - S1G: lo Sturgeon è colpito a 170-300 s, e il 688 fermo è affondato dalle cariche a 70-73 minuti.
  - Da guardare nella ritaratura:
    - i Mk 46 degli SH-3H contro un Victor che evade ed esca: 0 colpi su 9 lanci in 3 semi;
    - il Victor di S4 e S7 che non risponde;
    - le cariche dei Krivak sul 688 fermo in S1G (25-37 colpi: l'ATTACK delle navi della libreria, di trailer-1).
- 27/10/2026 (runtime): **navigazione incerta del giocatore agganciata** (`missioni/src/missione_navigazione.cpp`; nota
  condivisa navigazione-aggancio-contratto.md; modulo `nucleo/navigazione.hpp`). Spenta di norma
  (`usa_navigazione`; nell'app un'opzione del menu e due bit del codice partita, della principale).
  - Accesa, con la regola di simulazione: un `NavigazioneStimata` della classe nel teatro, con la chiave di missione
    `navigazione = { sigma_iniziale_nm, inerziale_guasto, gps }`.
  - Ordine `punto_nave` (testo = id), valutato sulle condizioni vere: quota, velocità, alberi, filo, nubi, notte,
    costa dal fondale, teatro, anno (con l'opzione GPS vale il 1990), marina.
  - Il satellite aspetta il passaggio; l'esposizione deve reggere fino alla fine, se no è interrotto; il radar della
    costa accende il radar del giocatore, il batimetrico fa un evento acustico.
  - Il pilota automatico naviga sulla stima.
  - API per la carta: `posizione_stimata`, `ellisse_navigazione`, `sigma_navigazione_m`, `punto_nave_in_corso`,
    `punti_nave_possibili`.
  - Registro `punto_nave`; messaggi 470-474.
  - **L'IA con l'errore di posizione** (stesso interruttore; `passo_navigazione_ia`):
    - un navigatore per ogni sottomarino dell'IA, della sua classe;
    - esposto (alberi su, filo, o quota periscopica) prende il punto nave più preciso che marina, teatro e anno
      consentono, senza emissioni, un satellite solo se passa, al più ogni 30 minuti e solo se migliora la stima;
    - l'errore entra nelle posizioni dei suoi rapporti di contatto (fase 3 B).
    - le zone e le mete dei suoi script (28/10/2026, approvato dalla principale):
      - sulla stima: le prove di zona sull'unità stessa (`in_rettangolo`, `in_cerchio`, `x_maggiore`) e la meta di
        `vai_a`, governata dalla stima, sicché il battello arriva spostato del suo errore;
      - vero: i gestori d'esito (le funzioni dello script che chiamano `messaggio` con un codice d'esito, anche
        attraverso `script.X(u)`, lette dal testo al caricamento: `leggi_gestori_esito`), un `vai_a` col gestore
        d'arrivo d'esito, l'inseguimento e `vai_al_bersaglio`, il giocatore e i timer della missione; uno script
        illeggibile (codice non letterale, o un messaggio fuori dalle funzioni) resta tutto vero;
      - prove: l'elenco delle 22 originali (`GESTORI_ESITO=file`, in
        `tools/taratura/risultati/errore_ia/gestori_esito.txt`) e il Victor con 5 nm di sigma iniziale
        (`prove_navigazione_missione.cpp`).
  - L'inerziale del giocatore viene dai danni (`Sistema::inerziale` del kit; DanniSottomarini chiama
    `inerziale_giocatore`).
- 27/10/2026 (runtime): **ritaratura dopo la fase 4** (100 semi per scena; `tools/taratura/risultati/fase4/`).
  Scoperta e colpito, dopo la fase 2 → ora:

  | Scena | Fase 2 | Ora |
  |---|---|---|
  | a6_1 SH-60F | 0,63 / 0,08 | 0,65 / 0,08 |
  | a6_1p datum profondo | 0,76 / 0,13 | 0,89 / 0,16 |
  | a6_2 Il-38 | 0,15 / 0,08 | 0,17 / 0,05 |
  | a6_2 P-3C | 0,70 / 0,15 | 0,69 / 0,18 |
  | a6_3 SSN silenzioso | 0,06 / – | 0,08 / 0 |
  | a6_4 Foxtrot | 0,64 / 0,26 | 0,65 / 0,27 |
  | a6_5 Be-12 solo | 0,36 | 0,34 / 0,01 |
  | a6_5p Be-12 in coppia | 0,58 | 0,73 / 0,07 |
  | a6_7 P-3C nel traffico | 0,30 / 0,07 | 0,32 / 0,07 |

  Artiglieria: G7.1 0,63 (atteso circa 0,5); G7.2 sotto 1,00, danni gravi 0,02; G7.3 sentiti 0,75-0,76 (prima
  0,91).
  - Le fasi 3-4 non spostano le scene ASW, che hanno un bersaglio solo: dentro il rumore di 100 semi, salvo il
    datum profondo e la coppia di Be-12. La coppia sale coi messaggi fra i due aerei e con l'allerta.
  - Da fare:
    - ~~la spaziatura del campo di boe del Be-12~~: non serve, chiusa nella taratura del 27/10 (sotto);
    - i Mk 46 contro un battello che evade ed esca, che nelle scene S7 non colpiscono mai;
    - G7.3 da riguardare con l'acustica.
- 27/10/2026, sera: le scene S1-S7 rigiocate dopo le fasi 3 e 4 (trailer-1; libreria 2 aggiornata, sotto). Il
  riassunto conta ora solo i contatti che l'IA può usare (traccia solida o posizione, come `scoperta_ia`):
  - **Chi vede chi.** Le scoperte dell'IA passano da 10-130 s a 70-330 s sui sottomarini (KPUG sullo Sturgeon 150-330
    s; scorte di S5 sul Victor 80-120 s; S7 sul Victor 70-150 s). Le navi in superficie si vedono ancora a 10-50 s. Il
    P-3 di S2 trova il Victor a 79-86 min; un SH-60B a 54 min (un seme). In S3A nessuno trova il LA in 2 ore; in S6 il LA
    vede il Typhoon a 100-120 s, sempre prima del messaggio della nave spia.
  - **Armi in acqua.** S5: il Victor lancia quando ha la soluzione, a 220-400 s (prima 30 s); gli SH-60B sganciano Mk 46
    più tardi. S4: il Trafalgar lancia a 62-76 min senza colpire e il Victor non risponde. S7: il Kara lancia alla guerra
    in tutti i semi, il Victor mai prima della sua copia dell'ordine; gli SH-3H sganciano sul Victor senza colpirlo.
  - **Colpiti.** S5: la petroliera colpita in tutti i semi (310-510 s); il Victor affondato in due semi, 10 e 105 min
    dopo il colpo. S1G: lo Sturgeon affondato a 10-46 min (prima 3 min), il 688 fermo a 65-80 min, con 2-14 cariche.
  - **Attese:** S5 e S7 passano su tre semi e non sono più «da completare». S1G manca di poco (lo Sturgeon a 14,8 min
    in un seme, l'attesa è 15). Le altre cinque falliscono ancora.
  - **Libreria 2** (editor, libreria, scene): regole d'ingaggio da `m688.armi_libere`, allerta da `m688.allerta` (ping,
    aggancio, tracce medie, inseguimento, vigilanza), lancio solo con `m688.soluzione` e con siluri
    (`m688.armi_rimaste`), inseguimento solo con la soluzione (le gambe di TMA le fa il motore), una salva di nave ogni
    20 passaggi sul contatto (**STIMA**), `m688.situazione` nelle regole che cambiano la situazione, voci ASW senza
    `armi = false`. Aree d'azione della radio nell'editor: «dalla radio» sulla zona, «dichiara/toglie la zona» sul
    messaggio; S4 le usa. Nel classico le funzioni nuove danno nil e la libreria si comporta come prima, salvo la
    cadenza delle salve delle navi, che vale nei due livelli (il classico delle 22 originali non cambia).
- 27/10/2026 (runtime): **taratura chiusa, e i tre punti aperti delle scene** (risultati in
  `tools/taratura/risultati/fase4/`, vedi LEGGIMI.md; diagnosi con `test/diag_scena_runtime.cpp`, una prova spenta
  di norma che gioca una scena e stampa una unità: `DIAG_SCENA`, `DIAG_UNITA`, `DIAG_SEME`, `DIAG_S`).
  - **Be-12** (fascia 0,5-0,6 per il campo di 60-70 boe della coppia, dossier 17 §9, fonte alta):
    - la coppia era a 0,73, e non per le fasi 3-4 (0,72-0,73 anche con forze, reazioni, difesa o armi spente);
    - due cause: posava 113 boe (60 per aereo) contro le 60-70 del campo, e il MAD a 85 m spazzava da solo un quinto
      dell'area, mentre il dossier dà al solo MAD 0,01-0,02;
    - carico portato a 32 boe per aereo (40 in sola ricerca) e MAD del Be-12 a 0,7, dove era 0,9;
    - risultato su 100 semi: **coppia 0,63, AT-1 0,22 a lancio; aereo solo 0,35**;
    - la spaziatura del campo sulla portata contro il bersaglio non serve: il campo storico è ancora più rado
      (circa 8,5 km).
  - **G7.3** (sentiti 97%; dopo la fase 2 0,91; con le fasi 3-4 0,75): non è l'acustica.
    - Senza forze torna identica a 0,91.
    - Con le forze la guerra porta le due navi al posto di combattimento da subito: il Kara spara a 33 s, prima che
      la Perry abbia la posizione al radar; la Perry spara 37 colpi invece di 213 (seme 1) e la battaglia cambia.
    - SE massimo nella fascia (62,8, 49,3 e 40,1 dB a 10, 20 e 40 km).
    - Resta da riguardare la quota dei campioni sentiti con meno colpi della nave più vicina all'ascoltatore.
  - **Mk 46 contro chi evade** (fascia 0,2-0,4 contro un SSN allertato con esche):
    - nella taratura il Victor del datum profondo dà 0,36 a lancio, dentro la fascia, anche senza la difesa della
      fase 4 (0,89 e 0,16 uguali);
    - in S7, su 8 semi, 0 colpi anche con la difesa spenta. Senza evasione colpi in 7 semi su 8; senza le sole
      MG-74 ancora 0: il Victor è allertato da un'ora di ping dei sonar calati e al primo sgancio mette in acqua tutte
      le esche;
    - la scena è più dura della fascia e lo lascio così: è il caso peggiore, non un errore del motore.
  - **S4, il Victor che non risponde: risolto.** Il Victor non sente mai il Trafalgar, molto più silenzioso.
    - Mancava lo snap shot del 24 §8.6: io l'avevo letto come un'occhiata, è il tiro di risposta.
    - Ora (`tiro_di_risposta`, `missione_armi_ia.cpp`) il sottomarino che sente un lancio ostile entro 20 km, con P
      0,5 e le armi libere, lancia dai suoi tubi lungo il rilevamento.
    - Difetto trovato: i generatori della difesa e delle reazioni non avevano il seme della missione (ogni seme
      uguale). Corretto.
    - Su tre semi il Victor ora risponde sempre.
  - **S6, il LA vede il Typhoon a 100-120 s:** il LA sente il Typhoon in superficie a 8 nodi a 61 km con la schiera
    TB-16, con 15-23 dB di eccesso; i pescherecci a 20-29 km con 40-50 dB. È la propagazione e la sorgente nel
    Barents d'inverno, e va all'acustica (disassembler).
  - **S3A, nessuno trova il LA:**
    - il LA parte a 160 km dal KPUG;
    - l'Il-38 è sul datum di 18 km a 46 minuti e ha rivelazioni, ma il controllore le scarta come falsi allarmi;
    - contro un 688 a 120 m e 6 nodi è coerente con la taratura dell'Il-38 (0,17 sul cue SOSUS);
    - l'attesa del dossier, contatto al KPUG a 60-90 minuti, è una narrazione, non una probabilità: è da rivedere con
      trailer-1 la geometria della scena.
  - Corretto anche il testo del registro «lancia un un Mk 48».
- 27/10/2026 sera (trailer-1): **non combattenti e S3.**
  - La libreria 2 (editor, libreria, scene) salta i non combattenti, cioè mercantili, pescherecci e navi spia col flag
    40h, finché le armi non sono libere: R_nave, R_sottomarino, R_pedina e R_aereo non li agganciano, e l'aereo sopra
    il bersaglio non li attacca. In guerra un convoglio nemico resta un bersaglio (S5: col salto sempre, il Victor
    sparava alle Perry invece che alle petroliere).
    Usa `m688.bersaglio_non_combattente` del runtime. Nasce dalla diagnosi di S6, dove il LA in pace agganciava il
    peschereccio sovietico. La libreria 1 non cambia; libreria e scene sono rigenerate.
  - S3 e S3A: l'intruso entra a 25 km a nord del KPUG, non più a circa 160 km, così l'attesa del contatto a 60-90
    minuti ha senso.
  - Le scene in acqua bassa (S1-S7 nel Barents) si rigiocano dopo la propagazione nuova del disassembler e dopo l'acqua
    del mese della data, che il runtime deve ancora collegare.
- 27/10/2026 sera (trailer-1): **S1-S7 col motore nuovo**: propagazione in acqua bassa del disassembler, acqua del mese
  della data, snap shot del runtime. Tre semi, riassunto in `tools/scene_interazioni_riassunto.txt`.
  - **S3A, ora ferma su tre semi:** il KPUG trova l'intruso a 250-330 s; prima, da 160 km, nessuno lo trovava in due
    ore. L'Alfa del giocatore lo sente a 950-2640 s.
  - **S5, ferma:** il Victor lancia sulla petroliera a 280-340 s e colpisce un mercantile in tutti e tre i semi.
  - **S4:** il Trafalgar trova il Victor a 970-2650 s e i due si scambiano i siluri; resta «da completare» perché il
    contatto sul confine entro 10 minuti non arriva.
  - **S1, S1G, S2, S6:** ancora «da completare». S1G manca di 10 s, il colpo a 890 s contro 900; il KPUG di S1 non ha
    lo Sturgeon in tre minuti; il P-3 di S2 non prende il Victor entro l'ora; il LA di S6 trova il Typhoon prima del
    messaggio della nave spia.
  - Dopo le proposte del runtime:
    - **S6:** l'SSN che aspetta sta ~75 km a ovest della rotta dell'SSBN, non più a 15 km. Col mare d'autunno lo
      sentiva subito; ora in tre semi non lo trova prima del messaggio dei 30 minuti.
    - **S1G:** l'attesa è fra 14 e 26 minuti, cioè 15-25 con un minuto di tolleranza. Passa nei semi 1 e 2 (880-890
      s); nel 3 il colpo arriva a 470 s, quindi resta «da completare».
    - **S1 e S2** li guarda il runtime col diag_scena_runtime: KPUG che non tiene lo Sturgeon, P-3 che non prende il
      Victor.
  - Dalla diagnosi del runtime su S1 e S2, nessun difetto del motore: cambiano le attese, non le scene.
    - **S1:** il «VDS a t=0» del dossier è il primo contatto, entro un minuto; la traccia solida viene per integrazione,
      e l'attesa è entro 4 minuti per tutte e tre le navi. Passa nei semi 1 e 2; nel 3 una nave non è solida a 4
      minuti, quindi resta «da completare».
    - **S2:** si conta dal cue della Perry col TACTAS, cioè dal più tardo fra l'entrata del P-3 a 45 minuti e il
      contatto della Perry; l'attesa è il primo contatto del P-3 entro 35 minuti. Nei tre semi arrivava in 18-31
      minuti; dopo l'ultima build del runtime il seme 3 non ha contatto in 80 minuti, quindi resta «da completare».
  - **S1G, ora ferma su tre semi.** Prima una nave della libreria 2 attaccava al primo contatto solido (runtime, diagnosi
    del seme 3). Ora, tutte STIME:
    - la prima salva antisom arriva dopo `PRIMO_ATTACCO_S` = 780 s sul contatto attuale (`m688.tempo_contatto` del
      runtime; nel classico dopo 330 passaggi), la caccia fino al PROBSUB;
    - poi una salva ogni `ATTESA_SALVA` passaggi;
    - dopo `SALVE_MASSIME` = 6 salve la nave torna ad ascoltare sulla sua rotta, e riattacca dopo `RIPRESA` = 120
      passaggi sul contatto riacquisito. Così non si sparano decine di salve a vuoto: nella sandbox, in S1G seme 2, la
      principale ne aveva viste 67-68 per nave.
    - Colpo a 880, 1550 e 910 s nei tre semi. Con la ripresa a 80 passaggi il seme 2 non colpiva più in 30 minuti.
- 27/10/2026, sera (runtime): **ritaratura ASW a6 dopo la perdita al fondo nuova** (risultati in
  `tools/taratura/risultati/fondo_2710/`, 100 semi per scena).
  - **La causa del salto.** Il disassemblatore ha corretto la perdita al fondo: sotto l'angolo critico sabbia e roccia
    riflettono quasi tutto (0,5 dB a rimbalzo a 250 Hz sulla sabbia, 1,8 a 1 kHz; roccia 0,3 e 1,0), dove prima ogni
    rimbalzo perdeva 5 dB a ogni angolo e frequenza. Le boe sentono i sottomarini sotto i 500 Hz per la via del fondo,
    e sulla scarpata delle Faroe le scoperte salgono. Il mese non c'entra (febbraio e ottobre danno gli stessi numeri
    col meteo fisso della taratura); il binario di prima delle 15:02 rifaceva la fase 4 seme per seme.
  - **Faroe (scarpata e piattaforma, sabbia e roccia), fase 4 → ora:**
    - SH-60F sul datum 0,65 → 0,85; datum profondo 0,89 → 0,84;
    - P-3C da SOSUS 0,69 → 0,88; Il-38 da SOSUS 0,17 → 0,31;
    - Foxtrot 0,65 → 0,90; P-3C nel traffico 0,32 → 0,49.
  - **Barents (Be-12):** quasi uguale: boe 0,35 → 0,34, coppia 0,63 → 0,66.
  - **A6.3 spostata** (decisione della principale): il riferimento storico (0,05) vale per un SSN silenzioso in acque
    profonde; la scena stava sulla scarpata delle Faroe, su 1000 m di sabbia. Ora sta nella piana di Porcupine (teatro
    celtico, fango, oltre 3000 m entro 45 km): **0,02**. La scena di prima resta come A6.3s
    (`a6_3s_ssn_scarpata.lua`), fascia fisica senza riferimento storico: **0,39** (0,42 su 12 semi del
    disassemblatore). L'ASW non si ritocca per inseguire un numero: la differenza è la fisica della sabbia sotto
    l'angolo critico.
  - Le fasce del dossier 17 §9 vanno lette per fondale: in acque profonde col fango valgono come prima; sulla sabbia e
    sulla roccia della piattaforma e della scarpata le portate passive sono più lunghe.
  - **P5, M6, G7 con lo stesso motore** (P5 agguato 20 semi, P5 Krivak e M6 40, G7 100):
    - P5 agguato uguale (34,6 guardate, 0,1 e 0,0); P5 Krivak nelle stesse fasce di prima (le celle differiscono entro
      il rumore di 20 contro 40 semi);
    - M6 nelle stesse fasce per teatro e mese (il meteo è casuale, 40 semi contro i 10 di prima);
    - G7.2 Kara 0,50 → 0,49; G7.3 sentiti 0,75 a 10, 20 e 40 km, SE massimo a 40 km 40,1 → 36,3 dB (la via del fondo);
    - **G7.1 Perry 0,63 → 0,41.** Stessi semi, stessi colpi, stessi tempi: i 22 colpi in meno sono granate arrivate dopo
      che il periscopio era sceso (primo colpo fra 16 e 31 s su un'esposizione di 30 s). È la verifica all'arrivo
      dell'esito dell'artiglieria (stessa sera): prima l'esito si decideva allo sparo. L'atteso «circa 0,5» non si
      insegue.
- 28/10/2026 (trailer-1): **S1 e S6 ferme su tre semi** con le proposte del runtime dopo la propagazione definitiva;
  pronte anche in `data/missioni/prove/vetrini.toml`.
  - **S1:** la finestra della traccia solida è 1-6 minuti, non 4. Su sei semi tutte e tre le navi sono solide fra 150 e
    350 s; con 5 minuti cadevano i semi 3 e 5. Il primo contatto del VDS resta entro un minuto.
  - **S6:** l'SSN che aspetta sta sul fango della scarpata (70,1N 32,4E, oltre 220 m d'acqua), fuori dalla sabbia che
    riflette, a circa 49 km dalla rotta d'uscita. Nei tre semi non sente il Typhoon prima del messaggio dei 30 minuti.
    Nelle due ore del vetrino lo trova a 4370 e 4410 s nei semi 1 e 2; nel 3 no. Le prove:
    - a 41 km (70,05N 32,6E) il seme 1 lo sentiva a 670 s;
    - a 90 km (70,5N 31,6E) nessun seme lo trovava in due ore.
  - La sonda dei punti è `tools/taratura/sonde/fango_s6.cpp`.
  - **Con la schiera agganciata (D),** la TB-16 dell'SSN in attesa, col cavo tutto filato a 90 m e 3 nodi, stava sul
    fondo a 388 m: lo sfregamento portava il Typhoon a −45 dB (disassemblatore). Il runtime ha fatto filare all'IA (e al
    comandante che guida il giocatore) solo il cavo che non tocca il fondo (`cavo_sicuro_m`).
    - Così la TB-16 ascolta a ~180 m sopra il fango, e al punto di prima (49 km dalla rotta, 89 km dal Typhoon) lo
      sentiva in 8-13 minuti.
    - L'SSN ora aspetta a 70,20N 32,3E: 225 m d'acqua, 54 km dalla rotta, 101 km dal Typhoon, appena oltre il salto
      della propagazione (TL da 124 a 155 dB).
    - Nei tre semi non lo sente nei primi 30 minuti. Nel vetrino lo trova a 5560 e 6580 s nei semi 1 e 2, nel 3 no,
      come prima della schiera (allora 4370 e 4410 s).
    - Le sonde sono `tools/taratura/sonde/schiera_s6.cpp`, `rotta_s6.cpp` e `fango_s6.cpp`, che ora prende fondale e
      distanze da riga di comando.
- 28/10/2026 (trailer-1): **S2 e S4 ferme su tre semi**, con la TMA corretta e la schiera vera. Non c'è più nessuna
  scena «da completare».
  - **S2:** il P-3 prende il Victor a 1010-1750 s dal cue della Perry, entro i 35 minuti. Non è cambiato niente nella
    scena: bastavano la TMA e la schiera di oggi.
  - **S4:** l'attesa del dossier è «un contatto debole sul confine, non classificato». La prova chiedeva il contatto
    utilizzabile (traccia solida o posizione); ora conta il primo contatto di qualsiasi qualità (`quando_sente`).
    I due SSN partono a 2 km (il 688) e a 4 km (il Trafalgar) dal confine: a 14 km due battelli silenziosi non si
    sentivano nei primi 10 minuti. Nessuno lancia sul 688.
  - Le scene dei missili M1 e M2 sono accese in `data/missioni/prove/`, coi vetrini «in ritaratura» (docs/21 §11.13).
- 28/10/2026 (runtime): **missili del modello nella missione, passo 1** (docs/21 §11.9; `missioni/src/missione_missili.cpp`;
  nota condivisa missili-aggancio-runtime-proposta.md §8). Solo nell'Enhanced, con `data/motore/missili.toml`, letto
  insieme ai siluri.
  - **La stazione:**
    - gli ordini `imposta_missile` (`CampoMissile`: fonte, modo, taglia, srch, dstr, i quadranti del MATE) e
      `prepara_missile`;
    - le letture `tiro_missile(tubo)` e `missili_in_volo()` (solo stime), più `slide_libera(tubo)`: una slide valve
      per lato, 20 s sullo stesso lato e 4 s sull'altro, per ora solo per i missili;
    - il missile caricato prima della missione è già pronto, quello ricaricato si prepara.
  - **Il piano:** sulla posizione stimata del battello, sul contatto designato e sui contatti noti, con la classe e la
    parte credute.
  - **Il volo:** dalla posizione vera, in un'unità della missione (categoria arma, classe = il missile). Le EcoNave
    sono le navi e i sottomarini emersi, senza armi, aerei e falsi bersagli; il seeker acceso entra nelle emissioni.
  - **L'impatto:** la difesa della classe (il byte dell'ATTACK_RND), poi il danno della testata.
  - **Il gestore di lancio della missione** gira ancora, per i conteggi e il messaggio 49, ma il suo `attacco_casuale`
    no, né nei timer nati da lui.
  - **Il comandante IA** prepara, designa, passa al BOL senza distanza e lancia col piano valido: nella 0A partono i
    quattro TASM e il gestore li conta.
  - **Prove:** `prove_missili_missione.cpp`. Rilievo del classico 0/110; casi con le impronte identiche.
  - **Restano:** la fonte radio, il TLAM, l'IA, il messaggio «missile ha agganciato», `esaurito` e `guasto` nel
    registro (tutti fatti entro il 28/10).
- 28/10/2026 (runtime): **traffico civile nella missione** (docs/20 §3; `missioni/src/missione_traffico.cpp`; il modulo
  è `nucleo/traffico` del disassemblatore).
  - **L'interruttore:** `usa_traffico(bool)`, spento di norma; la chiave di missione `traffico = false` lo spegne
    sempre. Funziona solo con l'acustica. L'app lo accende nelle partite dell'Enhanced (principale).
  - **Le unità:** una per nave entrata, con id da 10000; neutrali e non combattenti (flag 42h), di classe civile.
    Escono senza essere affondate. Una nave colpita, affondata o incagliata esce dal traffico (`perduta`) e resta
    un'unità come le altre. Un colpo entro 10 km fa fuggire le navi per mezz'ora.
  - **Solo sorgenti:**
    - mai osservatori, e il loro radar emette ma non scopre;
    - la firma dell'individuo;
    - il livello A oltre i 30 km;
    - nessuno script le sceglie come bersaglio;
    - l'IA non le attacca per l'errore d'identificazione ad A3.
  - **I civili per dati:** `classe_civile` da `ClasseNave::civile` (classi.toml), anche nelle reazioni; la rete a
    strascico dal tipo civile (anche i BMRT); il radar di navigazione `nav_civ` alle classi civili nuove e ai
    pescherecci (emettitori.py).
  - **Il costo**, su 10 semi da 2 ore a Gibilterra con 40 navi: 1,69 volte il tempo senza traffico, dopo il traffico
    valutato un turno su 4 e il modulo a 1 Hz (prima 2,55; `tools/taratura/risultati/traffico/`).
  - **Prove:**
    - `prove_traffico_missione.cpp`: spento di norma, tetto, neutrali, mai osservatori né bersagli degli script, la
      chiave, il determinismo;
    - in `diag_originali.cpp`, «comandante e IA non sparano alle navi del traffico» (5A, 8A e 9A, due semi, 40 minuti):
      fino a 37 lanci per partita, nessuno su una nave del traffico.
- 28/10/2026 (runtime): **la schiera trainata dei sottomarini, D** (docs/20 §7; `missioni/src/missione_schiera.cpp`; il
  modulo è `nucleo/schiera` del disassemblatore). Solo con l'acustica, per il giocatore e per l'IA: le classi col
  sensore lineare, cioè tb16 e tb23, e lo skat aggiunto dal disassemblatore a Victor III, Akula, Oscar e Typhoon.
  - **Il cavo:** chi fila (il bit 1 di +54, dagli script come nel classico) srotola il cavo alla velocità di filata del
    sistema; se è filata dall'inizio della missione, è tutta fuori.
  - **La forma:** col ritardo del cavo (`CavoSchiera`). Il Ricevitore della schiera ascolta alla quota vera del centro,
    dietro la poppa, col cavo filato (le righe di vibrazione), lo sfregamento e il fondo, e `schiera_danneggiata`.
  - **Il fondo:** ai 16 punti del profilo, lungo la rotta all'indietro.
  - **Ogni minuto le regole del dossier 04 §6.3** (STIMA):
    - il fondo: 0,15 + 0,02·v, ×2 su roccia; 70% danneggiata, 30% persa;
    - oltre V_max: 0,02·(v − V_max), ×3 e 50% persa oltre V_max + 6 kn;
    - oltre la quota massima: schiacciata, 0,2;
    - una rete a strascico al lavoro (passo_reti, anche del traffico) entro 150 m: 0,5; 60% persa, 40% danneggiata.
  - **Un'esplosione** entro 500 m: 50% persa.
  - **Danneggiata e persa:** la danneggiata ascolta 6 dB peggio e senza le bande sopra 300 Hz (nell'acustica). Una
    seconda volta la si perde. La persa vale per la missione: `schiera_cala` e la filata degli script sono rifiutate.
  - **Messaggi** 396-399, del blocco della principale; testi in esporta_motore.py. Per la vista: `stato_schiera(id)`.
  - **Righe proprie (F del blocco 2 del sonar):**
    - `Ricevitore::proprio` è la sorgente del battello, una volta per passo, per tutti i sensori;
    - il sonarista dell'equipaggio e quello dell'IA ricevono le righe proprie (`AscoltoSonar::proprie`).
  - **Prove:** `prove_schiera_missione.cpp`:
    - in acqua profonda a 3 kn la schiera a 358 m, sotto il battello a 60 m, e nessun contatto;
    - in 200 m tocca e si perde in pochi minuti, e la persa non si fila più;
    - un SSN dell'IA con la stessa regola;
    - niente senza l'acustica;
    - il determinismo.
- 28/10/2026 (runtime): **l'intercettazione, E del blocco 2 del sonar** (docs/22 §2; `missioni/src/missione_intercettazione.cpp`;
  il modulo è `nucleo/intercettazione` del disassemblatore). Solo con l'acustica.
  - **Chi emette** registra l'impulso (`emetti_impulso`), col livello del suo sensore:
    - il sonar attivo di scafo;
    - il BQS-15, col settore e i lobi laterali (−25 dB fuori);
    - le teste dei siluri alla loro cadenza (il livello del transitorio della testa).
  - **Chi ascolta:** a fine passo ogni unità con l'intercettatore della classe (WLR-9, Skat, Rubikon, Rubin) ascolta
    gli impulsi degli altri, col Ricevitore del suo sensore d'intercettazione.
  - **L'allarme che sale:** al giocatore il 311 («siluro in acqua»), e un codice per l'acquisizione quando la principale
    lo dà. Il sottomarino dell'IA evade (`stimola_evasione`) da 3 km sul rilevamento della traccia (STIMA). La scoperta
    del contatto dal ping (`intercetta_impulso`) resta quella di prima.
  - **Per la vista:** `intercettatore_giocatore()` e `intercettatore(id)`.
  - **Prove:** `prove_intercettazione_missione.cpp`: il WLR-9 del 688 ha la traccia del Victor che pinga ogni 20 s (a
    nord, PRI 20 s, senza allarme); lo Skat del Victor sente il ping del 688; niente senza l'acustica. L'evasione
    dall'allarme non ha ancora una prova da capo a fondo: serve un siluro con la testa attiva sul Victor.
- 28/10/2026 (runtime): **missili, passo 2** (`missione_missili.cpp`).
  - **La fonte radio:** dal quadro radio (`contatto_radio`) con `da_rapporto`, geografica, con l'ellisse di adesso
    (relativo = falso: il piano somma l'ellisse di navigazione). Il moto c'è se il giocatore l'ha messo sul pannello.
  - **La TMA dell'equipaggio del giocatore** (`tma_equipaggio_`, solo con i missili): la stessa TMA dell'IA sui
    contatti con traccia solida, senza dare posizioni al giocatore, che ha il suo MATE. Serve:
    - come fonte TMA quando il MATE non ha mandato i quadranti;
    - per i ContattoNoto, cioè l'area di ricerca e l'avviso dei neutrali anche in immersione.
    L'IA usa la sua `tma_ia_`.
  - **Le prove:** nella prova del tiro, la fonte radio senza rapporto non dà un bersaglio. Restano da provare la TMA
    dell'equipaggio (con le gambe) e la fonte radio con un rapporto vero.
- 28/10/2026 (runtime): **missili, passo 3: il TLAM** (`missione_missili.cpp`; docs/21 §3.4 e §11.8).
  - **La libreria di bordo:** il campo `tlam = { { id, bersaglio, plp = {x, y}, landfall = {x, y}, tot_s, finestra_s,
    tratta_terra_m }, ... }` della missione, con tot_s dall'inizio della missione. Per la vista: `libreria_tlam()`.
  - **L'ordine d'esecuzione:** la chiave `esecuzione_tlam = "<id>"` in un messaggio radio della missione. Quando il
    giocatore lo riceve, la missione diventa eseguibile (`tlam_eseguibile(id)`).
  - **Il pannello:** `CampoMissile::missione_tlam` sceglie la missione. `TiroMissile` dà `tlam`, `stato_tlam` (validità
    e P(landfall) sulla posizione STIMATA e la sigma dichiarata) e `ordine_esecuzione`. `rdy` vuole la missione valida
    (o da ripianificare), l'allineamento e l'ordine.
  - **Il lancio:** la tratta e l'esito con la posizione VERA e l'errore vero di navigazione (`valuta_tlam` ed
    `esito_tlam`). Lo stato del pannello e quello vero restano in `lanci_tlam()`, per il debriefing e il replay
    «verità». Senza una missione scelta il lancio è rifiutato. Il gestore `armi.attacco_terra` della missione gira come
    prima.
  - **I nomi dei tubi:** riconosce sia le famiglie del carico (`attacco_terra`) sia le armi (`tlam_c`, `tasm`).
  - **Il comandante IA** sceglie la prima missione della libreria e aspetta `rdy`.
  - **La prova:** in prove_missili_missione.cpp, libreria → missione → ordine dalla radio dopo un minuto → lancio →
    esito (arrivato in 655 s).
- 28/10/2026 (runtime): **missili, passo 4: l'IA** (docs/21 §7; `missione_missili.cpp`), e due correzioni.
  - **Il MISSILE di uno script dell'IA:** se la classe ha un missile antinave nel catalogo (`per_classe`, la tabella
    `[[lanciatore]]` di trailer-1: Oscar e Kirov P-700, Slava P-500, Charlie II e Nanuchka P-120, i Los Angeles, Sturgeon,
    Trafalgar e le navi americane l'Harpoon), parte il Missile del modello. Il piano usa il bersaglio dello script come
    lo sa l'unità: la posizione del contatto, la sua TMA (`tma_ia_`), o il solo rilevamento, e allora BOL. L'ATTACK_RND
    del resto dello script e dei suoi timer non fa niente.
  - **Senza missile antinave** (Kara e Krivak con l'SS-N-14 antisom, Victor III, Akula, Alfa) **o senza un piano
    valido**, resta lo script classico.
  - **La prova:** in prove_missili_missione.cpp, lo script di una Spruance sul Krivak a 25 km; l'Harpoon 84C vola col
    modello e colpisce il Krivak, e nessun ATTACK_RND a 20 s.
  - **Alberi a quota periscopica, solo con la dinamica dell'Enhanced** (trailer-1):
    - l'ordine `quota_periscopica` punta a 5,5 m, come Ctrl+P e il comandante;
    - periscopio e antenna si ammainano da soli oltre 6,096 + 1 m. Prima il battello che oscillava attorno ai 20 ft li
      perdeva, e con loro la radio (in M2 l'ordine TLAM non arrivava);
    - il classico non cambia.
  - **La TMA dell'equipaggio**, dopo la correzione del solutore del disassemblatore (con una gamba sola non è più
    osservabile): la prova usa due gambe, e il Krivak a 10 km è stimato a 0,5 km dal vero.
- 28/10/2026 (runtime): **il TLAM, esito e preparazione** (`missione_missili.cpp`, `registro_missione.cpp`).
  - **L'esito:** all'ora d'arrivo l'evento del registro `esito_tlam`, con la verità e `noto` falso:
    - classe_arma = la missione, classe_creduta = il bersaglio;
    - giusta = arrivato, valore = i secondi di volo;
    - pos = il landfall, o la caduta.
    Il giocatore lo sa dal BDA, un messaggio della sala radio mezz'ora dopo (STIMA: senza collegamento col missile nel
    1989): «bersaglio colpito», oppure «nessun effetto osservato».
  - **La preparazione:**
    - il TLAM caricato prima della missione non parte pronto: l'ora vera resta, come vuole la decisione 3 di docs/21 §10;
    - il primo di bordo prende 15 minuti, gli altri in parallelo `preparazione_altri_s` (PreparazioneMissile con
      primo = falso);
    - Harpoon e TASM restano pre-caricati e pre-allineati.
  - **La prova:** nella prova del TLAM il primo è pronto in circa 15 minuti, e l'esito finisce nel registro.
- 28/10/2026 (runtime): **posarsi sul fondo, i rischi** (roadmap 30, docs/11 §6; `missione_fondo.cpp`). Solo
  nell'Enhanced con l'acustica: il tipo di fondo è quello del teatro (`ClimaTeatro::fondo`: il Baltico tutto fango,
  le Faroe, Gibilterra e le Curili roccia sulla piattaforma). Tutti i numeri sono STIMA DI GIOCO.
  - **Il fango trattiene:**
    - la presa comincia dopo un minuto posato ed è piena in dieci;
    - al primo tentativo di lasciare il fondo, la chiglia resta sul fondo per presa x 180 s da fermo; le macchine
      avanti accorciano l'attesa (a 4 nodi la metà);
    - poi si libera, e la presa riparte da capo. Messaggio 476.
  - **Le rocce:** posato e strisciando oltre mezzo nodo, ogni minuto con P = 0,05 + 0,05 v:
    - il timone o la cupola del sonar di prua perdono il 30% (`guasti_`, applicati da DanniSottomarini);
    - lo scafo perde il 2% dell'integrità massima;
    - messaggio 477, una volta per appoggio.
    Fermo sulle rocce non succede niente.
  - **Le prese a mare:** posato sul fango più di mezz'ora, ogni minuto con P = 0,1 aspirano il sedimento: il
    raffreddamento è in avaria e la propulsione perde il 50%, una volta (messaggio 478). Per questo i nucleari evitano
    di posarsi.
  - **L'IA ha la stessa regola** (simmetria): il suo comandante tiene 25 piedi sotto la chiglia, quindi la tocca da
    battello danneggiato che affonda.
  - **Il classico non cambia:** senza il clima del teatro non c'è nessuno stato.
  - **Non fatto:** il rumore del distacco dal fango (docs/11 §6 «tempo e rumore»), che vuole una fascia di id dei
    transitori.
  - **La prova:** `prove_fondo_missione.cpp`, cinque casi: presa e trattenuta nel Baltico, prese a mare di giocatore e
    IA, rocce delle Faroe a 4 nodi (giocatore e IA), fermo sulle rocce, classico.
- 28/10/2026 (runtime): **i residui dell'IA** (docs/18 fasi 2 e 4).
  - **Il paesaggio per gli ascoltatori dell'IA** era già fatto (fase 2 F, `passo_paesaggio`: ogni 10 s ed entro
    30 km, dentro il disco del giocatore). La voce «restano» del 26/10 era indietro.
  - **Le zone amiche dentro il controllore dell'ASW aereo** (`Missione::sgancio_vietato`, `missione_forze.cpp`):
    - il punto di sgancio dentro l'area d'azione di un sottomarino amico, o in quella dichiarata dalla radio al
      giocatore: l'equipaggio non sgancia;
    - salvo il contatto classificato per classe e la P della prima ora dopo un attacco nemico alla parte, estratta per
      coppia come per navi e aerei (`ignora_zona_amica`);
    - `zona_amica_in(id, punto)` è la parte comune con `in_zona_amica`.
  - **Il FLASH «avviso radio» per l'IA** (24 §8.1, evento `flash` di interazioni.toml, prima mai generato): il
    messaggio FLASH della missione alla parte del giocatore, alla prima messa in onda, alza di uno l'allerta della
    forza di quella parte.
    - Arriva anche se il giocatore in quota non lo riceve. Vale anche per i sottomarini (STIMA).
    - Ne restano fuori i rapporti di contatto della forza al giocatore, che hanno già i loro eventi.
  - **La G7.3** (sentiti 0,75 con le forze): non c'è niente da correggere nell'acustica.
    - La taratura conta anche i transitori sentiti almeno una volta.
    - Con e senza forze, a 10, 20 e 40 km, ogni colpo ha l'arrivo sentito e la partenza no (0,50 dei transitori, con
      due transitori per colpo).
    - La quota dei campioni misura le code dei transitori ancora in scena, e scende con la cadenza del tiro
      (`tools/taratura/risultati/fase4/LEGGIMI.md`).
  - **Le prove:** in prove_forze.cpp lo sgancio vietato dentro la zona e permesso fuori, il FLASH che alza l'allerta
    USA e non quella sovietica, l'IMMEDIATE che non la alza.
- 28/10/2026 (runtime): **INCSEA degli aerei** (24 §4.2, STIMA; `missione_reazioni.cpp`).
  - **La regola:** nessun passaggio sotto i 500 ft entro 1 nm da una nave dell'altra parte, fuori dalla guerra.
    - Vale per gli aerei del controllore dell'ASW aereo, con la loro quota vera; gli altri aerei hanno solo una quota di
      crociera di comodo e ne restano fuori.
    - Restano fuori anche i neutrali e il traffico civile: l'accordo è fra le due marine.
  - **L'IA la rispetta:** da 2 nm l'aereo sale a 500 ft più 100 (`quota_minima_m` dell'aereo); l'elicottero sale a
    5 m/s e così arriva in tempo. In hover col sonar a immersione non sale.
  - **La violazione** va nel registro come `incsea`, una per passaggio: «sorvola … sotto i 500 piedi».
  - **La chiave** `incsea = false` della missione toglie anche questa.
  - **La prova:** in prove_difesa.cpp, un SH-60B che caccia un Alfa scortato da una Krivak:
    - con l'accordo resta a 182 m entro 1 nm, senza scende a 45,7 m;
    - la Krivak portata accanto all'elicottero già basso dà la violazione e il testo da aereo.
- 28/10/2026 (runtime): **missili, gli ultimi residui** (docs/21 §11.9; `missione_missili.cpp`).
  - **Il messaggio 479** «Il missile ha agganciato il bersaglio» arriva al primo aggancio di un missile del giocatore
    (una volta per missile), solo se il giocatore lo sa:
    - lo vede, con la testa del periscopio fuori ed entro l'orizzonte del periscopio contro una sovrastruttura di 20 m;
    - o glielo riferisce un'unità amica che ha il bersaglio in contatto (non un sottomarino in immersione).
    STIMA: nel 1989 il missile non ha collegamento col battello.
  - **`fine_missile` nel registro** per il missile che finisce senza impatto, per tutti i lanciatori:
    - codice 0 esaurito, 1 guasto in volo;
    - con la classe, i secondi di volo e il punto;
    - noto falso: per chi l'ha lanciato il missile semplicemente non arriva.
  - **La prova:** in prove_missili_missione.cpp, una Perry vicino allo Slava riferisce l'aggancio; col piano a 60 km
    il missile non trova niente e finisce nel registro.
- 28/10/2026 (runtime): **il quadro del fondale nella missione** (docs/24 §3; `missione_fondale.cpp`; i moduli del
  nucleo sono del disassemblatore).
  - **Gli ordini:** `ecoscandaglio_ping`, `ecoscandaglio_periodo` (a = minuti, 1-30, 0 a comando) ed
    `ecoscandaglio_potenza` (n = 0 piena, 1 bassa), in coda a TipoOrdine.
  - **Il quadro:** un `QuadroFondale` per il giocatore sulla carta della sua parte (d'epoca, o il fondale vero con la
    carta «vera»), creato al primo passo.
    - I sondaggi sono fatti sul fondale vero dalla posizione vera e messi sulla stimata, con la sigma della
      navigazione.
    - Il BQS-15 acceso aggiunge i ritorni del fondo a ogni spazzata (`spazza_fondo`).
    - I punti nave correggono il mosaico.
  - **La contro-scoperta:** ogni ping è un'emissione `fathometer` per i ricevitori d'intercettazione.
  - **Gli avvisi** vengono da `QuadroFondale::avvisi`, quindi solo dalle misure. Ognuno è detto quando diventa vero:
    - 480 meno di 100 m sotto la chiglia, 481 meno di 30;
    - 482 il fondo sale, 483 ostacolo a prua;
    - 484 troppo veloci per la portata del BQS-15;
    - 485 nessun eco.
  - **Per le viste:** `quadro_fondale(id)` ed `ecoscandaglio(id)`, cioè periodo, potenza e ultimo sondaggio.
  - L'IA non sonda ancora. Il classico non cambia.
  - **La prova:** `prove_fondale_missione.cpp`:
    - il ping a comando che il Victor intercetta;
    - il periodico a bassa potenza;
    - gli avvisi solo dopo un ping;
    - il BQS-15;
    - il classico.
- 28/10/2026 (runtime): **`m688.attacca_unita(u, bersaglio [, senza_soluzione])`** (libreria I03 e I05;
  `missione_armi_ia.cpp`).
  - L'attacco forzato di un sottomarino dell'IA ignora parte, ROE e non combattenti.
  - Con la soluzione: le gambe di TMA sul bersaglio forzato, poi il lancio.
  - Con `senza_soluzione`: il lancio subito, sul rilevamento vero (il tiro d'esercitazione pianificato).
  - Un siluro dai tubi, poi l'ordine finisce.
  - **La prova:** in prove_armi_ia_missione.cpp, il Victor in pace lancia sulla Krivak della sua parte.
- 28/10/2026 (runtime): **quattro punti del kit.**
  - **Il Pk dell'artiglieria fra unità dell'IA:**
    - il registro scrive `colpo` anche per i colpi mancati, col valore 0 come le cariche dell'ATTACK, e le
      statistiche contano a segno su sparati;
    - il testo dice «Colpo mancato»;
    - interesse 20.
  - **Le collisioni delle navi:** l'elica piegata limita la velocità massima per sempre (`NaveColpita::velocita_urto`,
    o l'ordine del motore senza scafo). Restano fuori:
    - il rumore in più (`rumore_urto_db`): vuole un campo nella sorgente di `nucleo/acustica`, del disassemblatore;
    - il duomo e il timone delle navi: fatti dopo (perdi_sonar_scafo e timone_bloccato da valuta_urto, in
      danni_missione.cpp; prova «la nave col duomo perso non sente più, col timone bloccato non accosta»);
    - il rollio e lo schianto a bordo (`AmbienteBordo::urto`): sono dell'app, che li legge da `urti()`.
  - **La navigazione:**
    - la qualità del fix dipende dal mare e dalla velocità;
    - il batimetrico legge la carta della parte: la spaziatura delle isobate dalla pendenza (20 m in piattaforma,
      100 oltre) e la direzione debole lungo l'isobata;
    - l'errore della carta viene dallo spostamento della campagna, 1 nm dove non è rilevata;
    - `usa_gps(bool)` per l'opzione «1990+»;
    - l'inerziale dei sottomarini dell'IA si guasta coi danni (`inerziale_ia`).
  - **La luce della missione:**
    - col cielo coperto la luce del giorno si dimezza (STIMA);
    - `luogo_del_sole(lat, lon)` sposta il sole delle opzioni --lat e --lon senza spostare la geografia.
- 28/10/2026 (runtime): **per S13 e I07.**
  - **I missili del modello in esercitazione** (S13 «Outlaw Shark»): con la chiave di missione `esercitazione = true`
    o `armi_esercitazione = true` sul lanciatore, il missile nasce inerte (`crea_missile`).
    - All'impatto vale la difesa della classe, poi niente danno.
    - Il colpo d'esercitazione va in `colpi_esercitazione` (registro `colpo_esercitazione`, obiettivo `colpire`), coi
      messaggi 456 e 457 come per i siluri.
    - **La prova:** in prove_missili_missione.cpp l'Harpoon d'esercitazione sullo Slava.
  - **`m688.trasmesso(u, tipo [, dopo_s])`** (libreria I07): vero se il giocatore ha fatto una trasmissione di quel
    tipo dopo dopo_s secondi, anche da una boa SLOT, dal registro. I tipi: contatto, attacco, situazione, conferma,
    richiesta, previsione.
    - **La prova:** in prove_radio.cpp, il rapporto di contatto a 500 s trovato a 540, non dopo 520, non d'attacco.
  - **La chiave d'unità `emcon`** (S13): lo stato di partenza dell'EMCON, al posto della dottrina della parte. I
    valori sono silenzio, navigazione, ricerca, intermittente e ingaggio. Gli eventi lo alzano, la discesa torna lì.
    Con `ricerca` il bersaglio tiene accesi anche i radar di ricerca aerea (Top Sail, Head Net-C) per l'ESM del TASM.
    - **La prova:** in prove_elettronica.cpp, la Kara americana in pace: navigazione senza la chiave, ricerca con.
  - **L'ordine d'esecuzione TLAM non autentico** (I12):
    - un messaggio con `autentico = false` non rende eseguibile la missione, e RDY resta spento. La sala radio già
      non consegnava gli inganni fra i nuovi; ora è anche esplicito;
    - `TiroMissile::ordine_non_autentico` è vero quando è arrivato solo un ordine col gruppo che non torna: la
      consolle lo può dire.
  - **L'obiettivo `tlam`** (I12), `{ tipo = 'tlam', missione = 'T1' [, minimo = n, finestra = false] }`: i TLAM della
    missione arrivati. Con la finestra di serie, l'arrivo deve cadere entro tot ± finestra_s; il registro
    `esito_tlam` ha codice 1 quando l'arrivo è nella finestra.
  - **La scatola dell'Harpoon** (I11):
    - il lancio di un missile del giocatore ha nel registro `livello` = taglia + 1 (1 piccola, 2 media, 3 grande) e
      `codice` = modo (0 RBL, 1 BOL);
    - l'obiettivo `{ tipo = 'scatola', massima = 'media' [, classi = { 'harpoon_84c' }] }` fallisce al primo lancio
      oltre la massima, ed è raggiunto a fine missione.
  - **Le prove:** in prove_missili_missione.cpp:
    - l'ordine autentico e quello falso, col segnale per la consolle;
    - la scatola media di serie, contro le massime piccola e grande;
    - l'obiettivo del TLAM raggiunto.
  - **Il P-500 dell'IA contro il 688** vale solo col battello in superficie (l'antinave vede solo la superficie).
    Immerso, lo Slava risponde con le armi antisom: la scheda di I11 la corregge la principale.
- 28/10/2026 (runtime): **M7, la parte del luogo** (S08 «Valona», S11 «Tenete il confine»; piano approvato
  dall'utente, nota condivisa m7-parte-del-luogo-piano.md).
  - **Una quarta parte, lato 24 (0x18 nel flag), solo per le unità marcate.** La chiave d'unità
    `schieramento = 'luogo'` mette i due bit della parte nel flag. Nessuna missione li usava, quindi il classico e le
    originali non cambiano; il traffico resta neutrale; le armi lanciate dal luogo ne ereditano la parte.
  - **La situazione di partenza:** chiave di missione `situazione_luogo = { usa = 'guerra', urss = 'pace' }`. Senza
    la chiave il luogo è in pace con tutti.
    - `m688.situazione(u, 'guerra', 'luogo', 'usa')` cambia la coppia;
    - una dichiarazione generale (fra USA e URSS) non tocca le coppie del luogo.
  - **Ostilità:** `ostili()` vede il luogo ostile a chi è con lui in guerra o in crisi. Armi libere, ROE dell'IA,
    allerta per lato, zone ed EMCON valgono già per lato. L'INCSEA resta fra le due marine.
  - **Il registro:** per il giocatore un'unità del luogo è nemica se la coppia è in guerra o crisi, neutrale se in
    pace, ricalcolato in partita. `allerta_forza` ha codice 24 per il luogo.
  - **Nazionalità:** i controlli di nazionalità sul bit 0x10 (marina vmf, EMCON sovietico, segno occidentale) vedono
    il luogo come sovietico. Per una forza locale con armi sovietiche del 1989 va bene.
  - **Statistiche della sandbox (principale):** parte_da_flag deve dare 24 per il luogo.
  - **Le prove:** in prove_forze.cpp, la motovedetta del luogo:
    - in guerra con gli USA e in pace con l'URSS, armi libere solo sugli americani;
    - la pace dello script a 30 s.
  - **Il luogo nelle serie:** `nome_parte_serie` e `parte_serie` di app/serie_dati.cpp conoscono «luogo» (0x18), e
    serie.toml ha la sua sezione. parte_da_flag e parti_ostili lo avevano già. La prova è in prove_serie_sandbox.cpp.
- 28/10/2026 (runtime): **il rumore d'urto nella sorgente**: il campo `UnitaAcustica::urto_db` del disassemblatore
  prende `DanniSottomarini::rumore_urto_db` (`urto_acustico_`, da raccogli_danni), nell'Enhanced con le variazioni
  R6.
  - **La prova:** in prove_collisioni_missione.cpp, il Victor che sperona il 688 ha 6,4 dB in più nella sorgente.
- 28/10/2026 (runtime): **`m688.evento(u, nome [, dopo_s])`** (libreria I03): vero se nel registro c'è un evento con
  quel nome (i nomi di nome_evento: incaglio, collisione, colpo_esercitazione, lancio_nemico…) dopo dopo_s.
  - **La prova:** in prove_radio.cpp.
- 28/10/2026 (trailer-1): **la scena S8 della parte del luogo** (`PROVA_S8_LUOGO`, con il vetrino), scritta con le
  chiavi M7 dello scrittore: la Koni del luogo in crisi con gli USA e in pace con l'URSS, la guerra col luogo a 15
  minuti.
  - **La prova:** in prove_interazioni_scene.cpp, la crisi e poi la guerra fra il luogo e gli USA, la pace col Foxtrot
    (nessuna arma libera della Koni su di lui) e fra il 688 e il Foxtrot.
- 28/10/2026 (runtime): **i lanci dei missili del modello per gli effetti** (la fiammata al periscopio, partita.cpp
  della principale).
  - Ogni missile del modello, del giocatore o dell'IA, da sottomarino o da nave, inerti compresi, va in
    `lanci_missili()`. L'arma è l'id del modello e la rotta è quella iniziale del piano.
  - Quando lo script MISSILE o il gestore della stazione registrano lo stesso lancio nello stesso istante, la voce
    resta una sola, con l'arma del modello.
  - Anche il TLAM ha la sua voce, con la rotta verso il landfall.
  - **Le prove:** in prove_missili_missione.cpp.
- 28/10/2026 (runtime): **il duomo e il timone delle navi dopo un urto** (valuta_urto, DanniMissione).
  - **Il duomo perso** (`Missione::perdi_sonar_scafo`): la nave perde i sonar di scafo e tiene la schiera trainata.
  - **Il timone bloccato:** la nave tiene la prua.
    - `NaveColpita::timone_bloccato`, o l'ordine del motore per le navi senza scafo per i danni;
    - il moto classico delle navi ora rispetta `OrdineMotore::timone`, che prima valeva solo per la dinamica.
  - Nel classico non ci sono collisioni modellate, quindi non cambia niente.
  - **La prova:** in prove_collisioni_missione.cpp, la Perry a 5 km da un Victor:
    - lo sente e accosta;
    - col duomo perso non lo sente;
    - col timone bloccato resta a 090.
- 29/10/2026 (runtime): **il tiro antinave dell'IA senza script** (scheda I11, regola di simmetria; `passo_missili_ia`
  in missione_missili.cpp).
  - **Chi tira:**
    - le navi col missile antinave della classe (`per_classe`: Slava, Kirov, Nanuchka, Perry, Spruance, Kidd, Iowa);
    - i sottomarini lanciamissili (Oscar, Charlie II), che prima salgono alla quota e rallentano alla velocità di
      lancio del missile;
    - gli SSN occidentali con l'Harpoon nei tubi del loro carico, con la stessa manovra. Il tubo si vuota e si ricarica
      come per i siluri.
    - Lo script e la difesa di quell'istante hanno la precedenza sulla manovra; le gambe di TMA e il Crazy Ivan la
      lasciano fare. Finita la salva, o dopo 15 minuti senza riuscirci, il battello riprende quota e velocità di prima.
  - **Su chi:** un bersaglio di superficie classificato. Può essere una nave, o un sottomarino in superficie visto da
    radar o vedetta.
    - È classificato con la classe del sonarista, l'identificazione visiva (il libretto, sotto), il suo radar
      identificato all'ESM o un contatto lungo (durata ≥ 1400).
    - Oppure con una traccia solida tenuta 5 minuti, ma solo se il contatto si comporta da unità militare. Lo decide
      il centro di valutazione: oltre i 18 nodi della crociera mercantile, in manovra (accostata o cambio di
      macchina), o entro 10 km da un'unità militare della stessa parte di classe nota. Lo è sempre, invece, se la
      missione dichiara `zona_di_guerra = true`. È la decisione dell'utente del 29/10: niente colpi sui mercantili
      non identificati.
    - Servono anche la posizione aggiornata da 30 s al massimo e le armi libere (`armi_libere_ia`).
    - Senza un contatto proprio vale una nave del quadro radio con la posizione e la classe (il dato oltre
      l'orizzonte, fonte radio).
    - Mai su chi crede neutrale né sui non combattenti (flag 40h).
  - **Come:**
    - un minuto di reazione;
    - la salva e la riserva della classe, nelle chiavi `salva` e `riserva` di `[[lanciatore]]` in missili.toml
      (generato da tools/pipeline/missili.py; STIMA: Slava, Kirov, Oscar e Iowa 4, Nanuchka 3, le altre 2);
    - su un sottomarino emerso la salva è sempre di 1;
    - poi si aspetta la fine dei missili più 120 s per la valutazione;
    - il piano è quello del giocatore: fuori portata, o sotto la distanza minima, il missile non parte.
  - Solo nell'Enhanced con le forze e le armi dell'IA, mai nelle missioni originali (senza il blocco `editor`). Il
    classico non cambia.
  - **Limiti visti nelle prove:**
    - le navi non hanno un ricevitore ESM nei dati (`[esm_di]` di emettitori.toml ha solo sottomarini);
    - il sonarista dell'IA non classifica una Perry a 25 nodi in 20 minuti;
    - la TMA del Charlie II su rotta costante stima la Perry a 2 km invece di 11, e il P-120 non parte (sotto i 10
      km).
  - **Le prove:** in prove_missili_missione.cpp:
    - la Perry tira un Harpoon al Victor emerso che ha identificato a vista, ma non se lo prende per un Los Angeles,
      non in pace, non sul Victor immerso;
    - lo Slava non tira sull'eco radar lenta del 688, se non nella zona di guerra;
    - la Perry non tira sul mercantile sovietico a 12 nodi a rotta costante;
    - lo Slava tira la salva di 4 sulla Perry a 20 nodi;
    - il Charlie II sale e rallenta per il P-120.
  - `prove_stazione_missili.cpp` (M1): lo Slava trattiene le armi sui sottomarini, perché in guerra affonderebbe il 688
    in superficie.
- 29/10/2026 (runtime): **il MISSILE degli script col piano non valido** (segnalato da trailer-1). Nell'Enhanced, se la
  classe ha il missile del modello e il piano non è valido, il lancio non parte: niente rumore, niente voce in
  `lanci_missili`, niente avviso al giocatore, ATTACK_RND muto. Una classe senza missile del modello tiene il lancio
  dello script. `missile_dello_script` restituisce `EsitoMissileScript`.
  - **La prova:** in prove_missili_missione.cpp, lo Slava senza contatto e il Krivak.
- 29/10/2026 (runtime): **il libretto di riconoscimento** (docs/25 §3.2, §3.4, §3.5; missione_libretto.cpp).
  - **`identifica_visiva`** (n = contatto, testo = classe; `ordine_identifica_visiva`): la classe dichiarata dal
    giocatore va in `identificazione_visiva(osservatore, contatto)`, nel registro come classificazione con `visiva`, e
    in `classi_credute`.
    - `giudica_lancio_giocatore` la legge per la categoria e la classe credute e per «identificato»; la categoria vera
      giudica, quindi colpire un neutrale identificato male resta un'infrazione.
  - **`osserva_aob`** (n = contatto, a = AOB, + dritta, b = incertezza): la rotta è rilevamento + 180 − AOB, in
    `osservazioni_aob()`. Col periscopio alzato il rilevamento è quello preso a vista. Per la TMA dell'equipaggio
    aspetta la `OsservazioneRotta` del disassemblatore.
  - **L'IA a vista:**
    - su un contatto «vista», o radar entro la portata della vista (la vela emersa entro metà), una classe;
    - sbagliata con una P che cresce con la distanza sulla portata (la foschia la accorcia), la notte, l'aspetto di
      prua o di poppa e la sola vela (STIMA);
    - l'errore è un'altra classe dello stesso genere fra quelle in mare;
    - va in `classificazioni_ia()` con `visiva`;
    - `lato_creduto` la preferisce al sonarista per 15 minuti.
  - **Le prove:** prove_libretto_missione.cpp (la classe sbagliata nel registro, l'AOB, il classico, l'IA che sbaglia
    di più da lontano).
  - `difficolta_asw()` per l'aiuto del telemetro (principale).
- 29/10/2026 (runtime): **due agganci al nucleo del disassemblatore.**
  - **I campi dei raggi condivisi per il traffico:** `raggi_.condividi(id)` quando una nave entra nel traffico, e
    `raggi_.dimentica(id)` quando ne esce restando un'unità (colpita, incagliata), che da lì ha il suo campo.
  - **La rotta da periscopio nella TMA dell'equipaggio:** a ogni `osserva_aob` un'`OsservazioneRotta` (tempo, rotta,
    incertezza; almeno 1°) su `tma_equipaggio_`, che si legge con `tma_equipaggio(contatto)`. La prova è in
    prove_libretto_missione.cpp.
  - **Verifica:** rilievo 0/110, impronte dei casi identiche (anche e5), suite 929/929.
- 29/10/2026 (runtime): **la posizione dalla TMA dell'IA decade** (proposta del disassemblatore).
  - Dopo 3 soluzioni di fila non osservabili (3 minuti), la posizione che la TMA aveva dato al contatto torna non nota,
    se nessun altro sensore l'ha rinnovata (`TmaIa::non_osservabili` e `data`, missione.cpp).
  - Le prime soluzioni «osservabili» a 1-2 km restavano ferme e facevano rifiutare il tiro al P-120 del Charlie II;
    il criterio di osservabilità lo stringe il disassemblatore.
  - Rilievo 0/110. Casi e suite aspettano il chiarimento sui profili WOA nuovi, con cui cadono anche prove che non
    dipendono da questo.
- 29/10/2026 (runtime): **il sigma della TMA dell'IA e dell'equipaggio** (col disassemblatore). risolvi_tma vuole
  l'errore tipico dei rilevamenti che stanno nella Tma, non quello dell'ultimo. Ora gli passo la media quadratica degli
  errori dei rilevamenti (`TmaIa::sigma_deg`, con lo stesso minimo di 0,3°).
  - A 19 km l'ultimo errore era 3° contro uno scarto vero di circa 2,2°, e la soluzione non diventava mai osservabile.
  - Il sonarista dell'IA ha ora la cache delle righe proprie per sorgente (`SonaristaIa::proprie`), come il giocatore.
  - **Le prove di prove_missili_missione.cpp:**
    - il Charlie II tira la salva di 2 P-120 sulla Perry dopo una ventina di minuti (la prova dura 30), da sotto i
      50 m e sotto i 5 nodi;
    - la Perry tira sul Victor emerso in almeno 4 semi su 8: in qualcuno il Victor si immerge prima della fine della
      reazione, e allora non è più un bersaglio.
- 29/10/2026 (runtime): **la verità delle classificazioni del sonarista e il DEMON del passo B** (missione_sonarista.cpp).
  - **`errata_per_verita`.** In SCENE1B ogni contatto «Portarinfuse» risultava giusto nel registro, perché il Sonarista
    segna errata solo quando sceglie apposta la seconda ipotesi. Ora il runtime confronta con la sorgente vera, per gli
    avvisi 411 e per `classificazioni_ia()`:
    - una classe detta è giusta solo se è quella vera;
    - per la sola categoria conta la natura della sorgente;
    - un biologico è giusto solo se detto biologico, o non sottomarino senza classe.
    - `lato_creduto` usa ancora `errata` del Sonarista.
  - **Il DEMON del fascio** (`leggi_demon_fascio(demon_fascio(...))`) e `sottomarino` in ogni ascolto, col ricalcolo
    delle righe e la stessa cache (`demon_sonarista_`, `SonaristaIa::demon`), per la classificazione a orecchio del
    disassemblatore.
  - **Le prove:**
    - in prove_sonarista_missione.cpp, SCENE1B con la recluta: 9 classificazioni con la classe, 8 sbagliate e
      segnate errate;
    - le prove fragili coi profili WOA nuovi: armi_ia (la Perry a 7 km), difesa INCSEA (la Krivak ferma sul punto della
      segnalazione), S03 (i neutrali a 80 km: un neutrale sentito può tenere giù il Foxtrot).
  - **Verifica:** rilievo 0/110, suite 938/938. I casi aspettano il link del gioco (app/main.cpp, `chiudi_partita`,
    in corso).
- 29/10/2026 (runtime): **il telemetro e gli obiettivi delle missioni di riconoscimento** (LIB-I13 e I14 di trailer-1;
  missione_libretto.cpp, registro_missione.cpp).
  - **L'ordine `misura_telemetro`** (n = contatto, a = distanza letta, b = altezza del quadrante;
    `ordine_misura_telemetro`): va in `misure_telemetro()` con la distanza vera, e nel registro come evento
    `misura_telemetro` (valore = letta, distanza_m = vera, misura_m = altezza). Il testo è per il debriefing, mai per
    i messaggi dell'equipaggio.
  - **Gli obiettivi:**
    - `identificare` (unita, minimo, entro_s): l'ultima identificazione visiva giusta di ogni unità;
    - `telemetro` (minimo, errore come frazione, unita): unità diverse lette entro l'errore relativo;
    - `rotta_stimata` (minimo, errore in gradi, unita): unità diverse con un AOB entro l'errore dalla rotta vera al
      momento (`OsservazioneAob::rotta_vera`).
  - **Le prove:** in prove_libretto_missione.cpp. Verifica: rilievo 0/110, impronte dei casi identiche, suite 952/952.
- 29/10/2026 (runtime): **achievement, esposizioni e statistiche di docs/26** (§2-4; achievement_missione.cpp,
  carriera_missione.cpp, missione_conti.cpp, nucleo/statistiche).
  - **Gli eventi nuovi del registro:** `osservazione_aob` (valore = errore in gradi), `ping_ecoscandaglio`,
    `allarme_siluro` (codice 1 in acqua, 2 in acquisizione), `attraversamento_mine` (un campo di mine ostili col
    BQS-15 sempre acceso e senza danni) e `verifica_radio` (giusta = autentico).
    - La classificazione visiva del giocatore ha `codice = 2`, così la legge anche la traccia del replay.
    - Il lancio del missile del modello porta i neutrali nella scatola (valore) e la distanza vera al lancio.
  - **`ContiGiocatore`** (missione.hpp): accensioni del radar, secondi con meno di 20 m sotto la chiglia, mare massimo,
    allarmi dell'intercettatore, attraversamenti di mine, verifiche radio.
  - **Le misure, coi nomi del disassemblatore:** visive_giuste, telemetro_entro_5, aob_entro_10, prima_la_vista,
    affondati_missile_oth (oltre l'orizzonte radar, 27,4 km, STIMA), salve_neutrale_illeso, siluri_evitati_allarme
    (entro 10 minuti dall'allarme), sotto_chiglia_20_s, navigazione_incerta, gps, errore_finale_m,
    non_autentici_respinti, campi_minati_attraversati, mare_max, colpi_artiglieria_subiti.
    - `righe_nuove_acint` vale 0: il rapporto ACINT è dell'app.
  - **Le esposizioni nuove:** ecoscandaglio (i ping), radar (le accensioni), missile (i lanci).
  - **Le statistiche (§3), in ReportMissione e StatisticheCumulate e in statistiche.toml:** visive giuste ed errate,
    telemetro ed errore medio, AOB ed errore medio, missili lanciati, a segno e neutrali nella scatola, ping
    dell'ecoscandaglio, punti nave con l'errore medio e massimo, allarmi e siluri evitati dopo l'allarme, mine scoperte
    e attraversamenti, verifiche radio e non autentici, collisioni, quasi collisioni e colpi d'artiglieria.
    - Il tempo sotto il ghiaccio non c'è: nel motore non c'è il pack.
  - **Le prove:** prove_statistiche.cpp, prove_libretto_missione.cpp (le misure), prove_fondale_missione.cpp (i conti).
- 29/10/2026 (runtime): **le luci di navigazione nel motore** (docs/27; missione_luci.cpp di trailer-1).
  - **Nei file del runtime:** le righe in missione.hpp, l'ordine `luci_giocatore`, la chiave d'unità `luci` e l'evento
    `luci` del registro (valore = lo stato).
  - **La vedetta notturna:** il periscopio dell'IA vede le navi fino a `portata_notte` (luci e sagoma) al posto dei 3 km
    fissi; il libretto dell'IA identifica di notte entro la stessa portata.
  - **La scoperta automatica a 9 miglia del modello originale** (navi e aerei su un'unità in superficie, il periscopio
    alzato sulle navi) di notte vale solo entro la portata notturna (`vista_automatica_di_notte`). Senza le luci
    dell'Enhanced resta com'era.
- 29/10/2026 (runtime): **il siluro antisom vero delle navi** (collaudo della libreria, S12; missione_antisom_navi.cpp).
  - Nell'Enhanced l'ATTACK di uno script dell'IA su un sottomarino tira l'arma della classe col modello del siluro:
    - il Mk 46 dai Mk 32 della Perry, entro 7 km;
    - con l'ASROC di Spruance e Kidd, posato vicino al punto del contatto, entro 9 km;
    - il SET-65 dai tubi di Krivak II, Kara, Kirov e Slava, entro 8 km.
  - Senza la posizione del contatto, fuori portata o per le classi senza siluri (i lanciarazzi RBU) resta la carica
    a dado. Il classico non cambia.
  - **La prova:** in prove_armi_ia_missione.cpp.
- 29/10/2026 (runtime): **l'AOB dell'IA al periscopio** (simmetria del libretto, docs/25 §3.5; missione_libretto.cpp).
  - Un sottomarino dell'IA col periscopio fuori stima la rotta delle navi entro la portata della vista: al più una
    volta al minuto per bersaglio, con sigma 10° vicino e fino a 30° alla portata (STIMA).
  - La rotta va nella sua TMA (`aggiungi_rotta`), come quella del giocatore con osserva_aob.
  - `tma_dell_ia(id, contatto)` la espone per le prove e il debriefing.
  - **La prova:** in prove_periscopi.cpp, l'Alfa nelle sue guardate stima la Perry (090) con 2 rotte e un errore medio
    di 10°. Una Perry col radar acceso tiene giù il periscopio dell'Alfa: l'avvisatore sente il radar.
- 29/10/2026 (runtime): **la navigazione incerta dell'IA, il resto della simmetria** (approvato dalla principale: B, C, D
  e A1; solo con la navigazione incerta accesa).
  - **A1, il pilota prudente** (missione.cpp, `acqua_ia` e `pilota`). Un sottomarino dell'IA col navigatore chiede più
    acqua sotto la chiglia, 1 m ogni 20 m di sigma e al più 40 m, quindi si tiene più al largo. Guarda anche più avanti
    (2σ in più, al più 5 km). Il pilota resta sul fondale vero: nessun incaglio in più.
  - **B, la rotta a punti:** il primo punto della rotta della missione è una meta assoluta, governata dalla stima come
    vai_a. I gestori d'esito restano veri.
  - **C, le mine dell'IA:** niente da cambiare. Si posano dove il battello è davvero, e ci arriva navigando sulla stima,
    quindi il campo risulta spostato del suo errore.
  - **D, il registro:** i punti nave dei sottomarini dell'IA si registrano sempre (prima solo con la sandbox) e vanno nel
    registro come `punto_nave_ia` (valore = spostamento della stima, distanza_m = sigma dopo, noto falso).
  - **La prova:** prove_navigazione_missione.cpp. L'SSN della rotta a punti arriva a 8,7 km dal punto vero; senza la
    navigazione incerta tutto vero.
  - **Restano** A2 (il pilota sulla stima e sul quadro del fondale del battello) ed E (l'errore delle navi), da
    decidere con l'utente. Approvati il 29/10/2026 e fatti (le due voci qui sotto).
- 29/10/2026 (runtime): **E, l'errore di posizione delle navi dell'IA** (missione_navigazione.cpp; solo con la
  navigazione incerta accesa).
  - Le navi dell'IA (non il traffico) hanno il navigatore della classe, come i sottomarini, e partono con la sigma della
    missione. Con l'antenna sempre fuori prendono un punto nave (LORAN-C, Transit, Cajka; il GPS dal 1990) al più ogni
    10 minuti.
  - Dopo un quarto d'ora la Perry ha 387 m di errore e la Krivak II 2450 m. La meta governata dalla stima le porta
    spostate del loro errore, come i sottomarini.
  - **La prova:** «le navi dell'IA con l'errore piccolo dei loro punti nave», in prove_navigazione_missione.cpp; senza
    l'opzione l'errore è zero.
- 29/10/2026 (runtime): **A2, il sottomarino dell'IA governa sulla stima e sul suo quadro del fondale** (missione.cpp,
  `fondo_creduto`, `profondita_pilota`, `carta_prudente`; solo con la navigazione incerta accesa e l'Enhanced, che dà
  al battello il suo quadro, fondale_ia_). Gli incagli e gli urti restano sul fondale vero.
  - **Il fondo creduto:** quello del quadro (la carta della sua parte e i suoi sondaggi) nel punto dove il battello si
    crede, meno una sigma della cella. Lo usano il pilota (rotte, quota, freno), secca_avanti ed evita_secche.
  - **Le rotte** si pianificano sulla carta della sua parte, nel riferimento della stima, con le secche allargate di
    1,5 sigma di posizione (al più 3 km, a passi di 500 m: `allarga_secche`, un minimo a finestra sulla griglia). I
    punti si riportano nel vero, togliendo l'errore. Se il largo chiude un passaggio (uno stretto), si usa la carta
    com'è. Se il battello è già più vicino alla secca del largo voluto (la deriva, un Crazy Ivan), prima torna al largo
    (la cella più vicina, entro due raggi); se no tiene il largo che ha.
  - **Il comandante prudente:**
    - con più di 500 m di sigma, in meno di 100 m d'acqua sotto la chiglia (sul fondo creduto) rallenta a 5 nodi;
    - un sovietico non fa il Crazy Ivan verso una secca: la guarda 2σ più lontano e tiene la rotta del pilota;
    - per salire guarda lungo la prua alla portata di sempre, non a quella allungata di A1, che serve per accostare: se
      no la terra di lato in una virata lo farebbe venire a galla.
  - **sigma_navigazione_m(id)** dà ora anche la sigma delle unità dell'IA col navigatore: il sondaggio dell'IA del
    disassemblatore la riceve.
  - **La prova:** in prove_navigazione_missione.cpp, «il sottomarino dell'IA aggira la secca sulla stima». Un'isola di 3
    km di raggio sta sulla rotta di un SSN dell'IA con 1 nm di sigma iniziale. Con la posizione incerta passa a 5,2 km dal
    centro, senza urti, e arriva; sul vero passa a 2,9 km.
  - **Corretto insieme:** i Crazy Ivan dei sovietici, dopo il primo, si ripetevano ogni 5-15 minuti (missione_collisioni.cpp:352,
    la rotta vista diventava quella dell'Ivan e il ritorno contava come un'accostata): ora la rotta vista resta quella
    ordinata. In 6 ore un Alfa in rotta ne fa 5 (col difetto 17): prove_collisioni_missione.cpp, «dopo un Crazy Ivan il
    ritorno in rotta non ne chiama un altro». Col via della principale; il kit avvisato.
- 29/10/2026 (runtime): **l'IA sotto il pack, la simmetria del ghiaccio** (missione_diesel.cpp e missione_navigazione.cpp;
  il quadro della volta dei diesel col solo sondatore alto è del disassemblatore, in missione_ghiaccio.cpp).
  - **Il punto nave:** sotto la banchisa un sottomarino dell'IA non lo prende, né con l'albero né col filo, nemmeno a
    quota periscopica, come il giocatore.
  - **Lo snorkel e l'emersione forzata dei diesel** solo in acqua libera. Sotto la banchisa il battello resta a 50 m e
    va a batteria al rifugio:
    - la polynya più recente vista dal suo sondatore alto (il quadro della volta, riportato nel vero togliendo
      l'errore di navigazione); in silenzio il sondatore non sonda;
    - se no il margine del pack che sa dalle carte del ghiaccio: la copertura del meteo sotto 0,3, entro 100 km.
    Nell'acqua libera sale e ci resta piano (1,5 nodi) sul punto dove l'ha trovata; se il pack la richiude, torna
    sotto e cerca di nuovo. Sotto il ghiaccio non ricarica.
  - **Le prove**, in prove_navigazione_missione.cpp:
    - «il Kilo scarico va all'acqua libera e fa snorkel lì»: alle Curili a marzo, con copertura 0,47, non sale mai
      sopra i 100 m sotto il ghiaccio, trova l'acqua libera a 2,7 km e ricarica (da 0,239 a 0,255 in 6 ore);
    - «il sottomarino dell'IA a quota periscopica non prende il punto nave»: un Victor a 15 m; sotto il pack non lo
      prende, col mare libero sì.

- 29/10/2026 (runtime): **l'IA legge i fanali di notte** (missione_luci.cpp, `leggi_fanali` e `portata_sagoma`;
  missione_libretto.cpp).
  - **Cosa leggono la vedetta e il periscopio** dai fanali che arrivano loro (accesi, nel settore, entro la loro
    portata col binocolo; i ruoli di luci.toml, COLREG 1972):
    - lo stato: pesca, senza governo (i due rossi), manovra limitata, alla fonda, altrimenti in moto (albero, laterali,
      coronamento);
    - il tipo, quando le luci lo dicono: peschereccio dai fanali da pesca, sottomarino dal faro ambra USN;
    - l'aspetto: rosso e verde insieme di prua (σ 5°), il solo rosso o il solo verde dal lato (σ 30°), il solo
      coronamento di poppa (σ 20°). STIMA.
  - **Niente classe dai soli fanali:** di notte, oltre la portata della sagoma (`portata_sagoma`, la parte di
    portata_vista senza i fanali; il totale non cambia) e dentro quella dei fanali, l'identificazione dell'IA non dà
    più una classe. Resta la lettura (`fanali_visti(osservatore, contatto)`). L'AOB coi fanali resta quello di stima_aob
    del disassemblatore.
  - **Le prove**, in prove_libretto_missione.cpp:
    - «i fanali di notte dicono lo stato, il tipo e l'aspetto, non la classe»: un mercantile visto di prua, dal lato e
      di poppa; il mercantile alla fonda; il peschereccio; la Krivak a luci spente; i fanali troppo lontani;
    - «la Krivak vede di notte il faro ambra del 688 oltre la sagoma»: sottomarino in moto, nessuna classe.
- 29/10/2026 (runtime): **le condizioni sui ping di tutte le unità** (inventario della simmetria, §2). Ogni ping attivo,
  anche il PING degli script nel classico, si annota con il tempo, l'unità e la posizione (`ping_unita()`; viene da
  impulso_sonar_enhanced). Due funzioni nuove per gli script, in API.md:
  - `m688.ha_pingato(u, id [, ultimi_s])`: l'unità id ha fatto ping; con -1 una qualunque diversa da u;
  - `m688.ping_entro(u, raggio_m [, ultimi_s])`: un'unità qualunque, giocatore compreso, ha fatto ping entro il raggio.
  `ping_giocatore_entro` resta com'è. La prova è «script: m688.ha_pingato e m688.ping_entro…», in prove_missioni.cpp.
- 30/10/2026 (runtime): **dal secondo collaudo della libreria** (collaudo-libreria-30-10.md).
  - **S12, la cadenza dei siluri antisom delle navi** (missione_antisom_navi.cpp). Prima due Perry tiravano 12 Mk 46 in
    tre minuti su un Alfa fermo. Ora la nave tira un'arma alla volta; la successiva parte solo dopo l'esito della prima
    e 90 s di verifica. Ci sono i tubi carichi della classe e la ricarica dalla riserva (STIMA):
    - la Perry ha 6 tubi (2 Mk 32 tripli) e 18 di scorta, un tubo ogni 5 minuti a mano;
    - Spruance e Kidd hanno 8 celle ASROC e 16 di scorta, una ogni 2 minuti;
    - Krivak II ha 8 tubi, Kara, Kirov e Slava 10, senza ricariche.
    Una nave con l'arma vera non ricade più sul dado delle cariche, nemmeno senza la posizione o fuori portata: in quei
    casi non tira. Prova: «la nave tira un siluro antisom alla volta e aspetta l'esito», in prove_armi_ia_missione.cpp.
    La prova della ritirata in prove_difesa.cpp usa ora una Type 22, che non ha il siluro nella tabella, per avere la
    carica a dado.
  - **S08, i Foxtrot neutrali ai moli che partivano.** A 74 s due Foxtrot cominciavano il pedinamento in pace del
    giocatore a 10 nodi. Poi l'anticollisione fra loro salvava come «velocità di prima» quella del pedinamento e, a fine
    manovra, la rimetteva: uno correva per 25 minuti e si incagliava. Tre correzioni:
    - i neutrali non pedinano (missione_reazioni.cpp);
    - la manovra di difesa prende la velocità di prima di una reazione in corso (missione_difesa.cpp);
    - un sottomarino fermo in superficie (all'ormeggio) non fa l'anticollisione.
    Prova: «S08: i Foxtrot neutrali ai moli restano fermi», in prove_difesa.cpp, mezz'ora della missione vera: i quattro
    restano a 0 m dal posto.
- 30/10/2026 (runtime): **l'elicottero imbarcato come arma della nave oltre la portata dei tubi**
  (missione_antisom_navi.cpp; la principale).
  - **Le macchine:**
    - SH-60B LAMPS III sulle Perry, Spruance e Ticonderoga;
    - SH-2F Seasprite (LAMPS I) sulle Knox, senza sonar a immersione: aggiunto il 30/10/2026 in
      tools/pipeline/asw_aereo.py (dossier 17 §2.1b, con le fonti), in classi.toml (sprite segnaposto dell'SH-60B, da
      esporta_motore.py) e nell'enciclopedia;
    - Ka-25 sulla Kara, Ka-27 sull'Udaloy e sulla Kirov; la Krivak non ne ha.
  - **Quando decolla:** quando l'ATTACK dello script trova il contatto oltre la portata dei tubi, entro 60 km, e col
    mare al più a 5 (SH-60B, Ka-27) o a 4 (SH-2, Ka-25), STIMA. Diventa un aereo ASW come gli altri (missione_asw.cpp):
    il controllore ha per base la nave, il contatto della nave come cue e il bersaglio. Transito, boe o sonar a
    immersione, Mk 46, il ghiaccio e il mare del dip sono le sue regole.
  - **Il ritorno:** in rientro, entro 1,5 km dalla nave o dal punto di partenza, appontato, è pronto di nuovo dopo 30
    minuti. Se è abbattuto, la nave non ne ha un altro. Se la missione ha già un elicottero sulla nave (base_unita, o
    uno della parte entro 1 km all'inizio), la nave non ne fa un altro.
  - Nella tabella dei tubi entrano anche Udaloy (2 × 4 da 533), Ticonderoga e Knox (ASROC).
  - **La prova:** «oltre i tubi la nave fa decollare il suo elicottero ASW», in prove_armi_ia_missione.cpp. Una Perry
    col contatto a 20 km fa decollare un SH-60B, che posa 7 boe in 20 minuti; col mare 7 non decolla; con un SH-60B già
    della missione sulla nave non ne fa un altro; una Knox, col contatto passato dalla formazione, fa decollare l'SH-2F,
    che posa 8 boe. La Knox non ha ancora il radar nei dati dell'elettronica (emettitori.py), quindi da sola non vede il
    contatto in superficie: va aggiunto. Nella stessa voce, le prove degli eventi dello spettatore che mancavano:
    `evasione_ia` (prove_difesa, il Victor sul siluro) e `collisione_ia` (prove_difesa, due sottomarini senza
    l'anticollisione).
- 30/10/2026 (runtime): **i missili dell'IA non tirano sul contatto lungo del solo sonar passivo** (missione_missili.cpp,
  decisione della principale per realismo). Con la SQR-19 la Perry teneva per ore il cargo sovietico a 30 km: per durata
  diventava «classificato» e in guerra gli tirava 4 Harpoon. Ora la classe nota viene dalla vista, dall'ESM
  (l'identificazione del radar del bersaglio) o dal sonarista (la sua classificazione, giusta o creduta). Il contatto
  lungo vale ancora solo se è al radar. La traccia solida e la regola «militare» restano come prima. In più, un contatto
  che la vista o il sonarista credono di classe civile non è mai un bersaglio dei missili: col VDS del disassemblatore
  il sonarista della Perry classificava il cargo come portarinfuse e la Perry gli tirava gli Harpoon. La prova «la
  Perry non tira sul mercantile non identificato» resta com'è; coi missili, la libreria, il VDS, le collisioni, la
  difesa e le armi dell'IA passano 127 prove su 127.
- 30/10/2026 (runtime): **la scatola pulita dei missili dell'IA** (missione_missili.cpp; la diag
  «traffico: comandante e IA non sparano alle navi del traffico», segnalata dal kit). In SCENE8A, col seme 2, l'Harpoon
  di una Perry sull'Alfa in superficie a 17 km agganciava in volo un peschereccio del traffico nella sua area di
  ricerca. Due cose:
  - **La regola:** come il giocatore vede sulla carta i neutrali nella scatola (docs/26), l'IA non tira se fra i contatti
    che conosce nell'area di ricerca del piano, oltre al bersaglio, ce n'è uno creduto neutrale o di parte ignota.
    Riprova dopo 20 s.
  - **La diag:** non conta più le armi. La testa di un missile può agganciare una nave del traffico che il lanciatore
    non conosceva, come per il giocatore: è un danno collaterale, non un tiro sul traffico. I lanci su una nave del
    traffico restano controllati. Ora passa: SCENE5A, 8A e 9A, due semi ciascuna, nessun lancio sul traffico.
- 30/10/2026 (runtime): **l'IA vede le mine col suo sonar e le aggira** (missione_bqs15.cpp, `rileva_mine` ed
  `evita_mine`; una riga in missione.cpp, nel pilota; due nel missione_fondale_ia.cpp del disassemblatore, col suo via).
  Prima il BQS-15 teneva le mine scoperte solo per il giocatore, e l'MG-70 dei sovietici, che è un sonar da mine, non
  le vedeva.
  - **La scoperta:** ogni sottomarino dell'IA col BQS-15 acceso, o con l'MG-70 o l'MG-24 a ogni spazzata del sonar del
    fondo, tiene le sue mine scoperte, con gli stessi errori di misura del giocatore ma un flusso casuale suo
    (`mine_scoperte_ia(id)`).
  - **Il campo minato noto:** il sonar da mine si accende in un campo minato noto, cioè mine della sua parte, o mine
    già viste, entro 5 km (`SituazioneFondo::campo_minato` di decidi_fondo, che prima nessuno impostava).
  - **Il pilota:** una mina vista nell'ultima ora entro 2 km davanti, che la rotta passerebbe a meno di 300 m: accosta
    quanto basta dal lato opposto, piano; dentro il margine va via dalla mina. STIMA.
  - **La prova:** «BQS-15: il sottomarino dell'IA vede la mina e la aggira», in prove_bqs15.cpp. In 80 m d'acqua, con
    una Mk 67 della sua parte sulla rotta, il Los Angeles dell'IA passa a 241 m; uno Sturgeon, senza sonar da mine, ci
    passa sopra.
- 30/10/2026 (runtime): **pezzi piccoli per le missioni nuove di trailer-1 e per il registro.**
  - **I sonar delle navi dell'IA classificano sul LOFAR giusto.**
    - Le righe del sonarista dell'IA vengono dal LOFAR più fine fra i sensori in ascolto, cioè dalla schiera filata, non
      dal sonar di prua che sente di più a banda larga (la riga è del disassemblatore, in missione_sonarista.cpp).
    - La tonale che fa crescere la durata del contatto viene dal sensore che la vede meglio (missione.cpp, i passivi).
      Prima la Perry con l'Alfa sulla SQR-19 fermava la durata a 540 appena l'SQS-53 diventava il sensore migliore,
      con -19 dB di tonale.
    - La rivalutazione delle tracce già classificate (ogni 300 s, solo in meglio) è del disassemblatore, in
      nucleo/sonarista.
  - **`m688.distanza_da(u [, id])`:** i metri veri fra due unità, per le regole che fanno da arbitro (la S14), in API.md.
  - **Gli SSBN non pedinano:** ohio, typhoon, delta3, delta4, yankee e lafayette tengono la rotta della pattuglia
    (missione_reazioni.cpp). Prova in prove_difesa.
  - **Pedina solo chi ha il ruolo e il sensore** (la principale: «Nimitz pedina Trafalgar» nella S01 e nella I08):
    portaerei e ausiliarie (nimitz, kitty_hawk, kiev, kuznetsov, sacramento, ugra, petroliera, boris_chilikin) e la
    iowa non pedinano mai; le altre navi solo con un sonar attivo nella suite. Prova in prove_difesa: in pace la Nimitz
    e la sua Spruance col Victor a 6 km, la Spruance pedina e la portaerei no. Rifatte S01 e I08 coi semi 1 e 2:
    pedinano solo Perry, Spruance e Kidd; la S01 finisce a 4300 s con l'esito 41 (la portaerei all'imboccatura, per la
    sua rotta), la I08 non ha esito in 6000 s.
  - **Il registro:**
    - «Ci stanno pingando» si scrive una volta per episodio: un ping dopo 2 minuti di silenzio ne apre uno nuovo, e il
      numero dei ping va in valore e nel testo. Le statistiche contano ancora i ping, non gli episodi. Prova in
      prove_difesa.
    - L'ingaggio dell'IA (`ingaggio_ia`) è «attacca» solo in guerra; senza guerra è «tiene sotto …, senza fuoco».
  - **Le mine:**
    - `sicura = true` sulla mina dello scenario la tiene inerte per sempre, al suo posto e visibile ai sonar (la I17).
      Il campo in MinaScenario è di trailer-1.
    - Sulla carta del giocatore (app/carta.cpp) le mine dello scenario della sua parte appaiono col simbolo delle sue,
      «NOTA» e senza ora d'armamento; le nemiche solo se viste dal BQS-15.
    - **Il varco:** il pilota dell'IA non accosta più su una mina alla volta. Cerca la rotta più vicina alla sua che
      resta a 300 m da tutte le mine viste per 2 km, a passi di 5 gradi da una parte e dall'altra, fino a girare
      indietro. In prove_bqs15, una fila di dieci CAPTOR in sicurezza con un varco di 1 km: il Los Angeles dell'IA
      passa dal varco, a 326 m dalla più vicina.
  - **La mina comandata da terra (la S18):** `comandata = true, gruppo = '…'` sulla mina dello scenario (i campi in
    MinaScenario sono di trailer-1). Finché resta comandata e in attesa è in sicurezza: inerte e visibile.
    `m688.comanda_mine(u [, gruppo])` passa alla stazione le comandate del gruppo (senza gruppo, tutte). Da lì
    l'operatore la fa scoppiare quando un sottomarino è sopra di lei, dopo il suo ritardo (10-30 s, STIMA, fisso per
    mina): lo segue sul suo tracciato entro tre raggi (in pianta, STIMA; dal 31/10/2026: col raggio solo, a 10 nodi un
    battello passava sopra la MDT in 10 s e non scoppiava mai, la S18), e il danno lo dà la distanza vera allo scoppio. Se il sottomarino se ne va prima, non succede niente; le navi no. Prova in
    prove_bqs15: lo Sturgeon passa sopra; con l'ordine scoppia entro il ritardo, senza l'ordine resta in sicurezza.
- 30/10/2026 (runtime): **il Terne III della Oslo** (S16; la principale; missione.cpp, f_attacca e salve_asw_classe).
  - **L'arma:** il lanciatore sestuplo Terne III, sei salve da sei razzi (36), carica da 50 kg con spoletta a tempo, di
    prossimità o a contatto; portata 425-1600 m (Wikipedia «Terne ASW»; designation-systems.net dà 400-5000 m e
    avverte che le fonti discordano: si tiene la fascia stretta).
  - **Nel gioco:** come l'RBU della Koni resta il colpo a dado dell'ATTACK dello script (una salva per ATTACK, danno
    30), con le sei salve della dotazione e solo fra 425 e 1600 m dal bersaglio. Fuori dalla fascia l'ATTACK non tira
    e non consuma la salva. Il classico non cambia.
  - **Non fatto:** i due Mk 32 tripli della Oslo (montati alla fine degli anni Settanta; il Sting Ray arriva dopo,
    nel 1989 sarebbero Mk 46, STIMA) non sono nella tabella dei siluri antisom: con la classe in arma_antisom l'ATTACK
    tirerebbe solo il siluro e il Terne non servirebbe più. La Oslo non ha ancora una suite acustica (il Sintra/Simrad
    TSM 2633 di scafo e VDS e il sonar del Terne): ascolta col sonar dell'originale per schieramento.
  - **La prova:** «il Terne della Oslo tira sei salve fra 425 e 1600 m», in prove_armi_ia_missione.cpp. Con l'ATTACK
    ogni 20 s per 10 minuti su un Victor emerso: a 1 km sei colpi; a 300 m e a 3 km nessuno; la Koni a 3 km tira.
- 30/10/2026 (runtime): **la Luna nella vista notturna dell'IA** (la principale; missione_luci.cpp, `Missione::cielo_notte_in`).
  - **Il cielo:** con la data della missione (data e ora_utc; tutte e 37 le missioni della libreria le hanno) e il
    fondale georeferenziato (GEO1) la sagoma si vede contro il cielo di nucleo/sagoma_notte con la Luna vera: fase,
    altezza, estinzione, controluce, nel punto dell'osservatore e all'ora della partita (dopo la mezzanotte il giorno
    seguente). Senza data o senza GEO1 resta il cielo di prima: la luce del Sole e le nubi, senza Luna. Il classico non
    cambia.
  - **Dove:** portata_notte (luci e sagoma) e portata_sagoma, quindi la vedetta, il periscopio e il libretto dell'IA.
  - **La vista:** prepara_vista (app/interfaccia.cpp, col via della principale) mette lo stesso cielo in
    `VistaPeriscopio::cielo`, con `cielo_lunare` acceso nell'Enhanced: il kit ci disegna sopra la Luna e il giocatore
    vede la luce che hanno le vedette nemiche.
  - **La prova:** «di notte la luna piena alta allunga la portata della sagoma, la nuova no», in
    prove_libretto_missione. Alle Faroe a mezzanotte UTC il mercantile a 4 km: il 13/12/1989 (quasi piena, alta) 0,15
    lux e la sagoma a 9,0 km; il 28/12/1989 (nuova, sotto l'orizzonte) 0,0018 lux e 5,0 km. Senza il fondale le due
    notti sono uguali.
  - **Da ritarare:** le missioni notturne (I15, S14, S15, T8, trailer-1): con la Luna alta le vedette vedono anche
    il doppio più lontano.
- 30/10/2026 (runtime): **le fregate della S16 che non cacciavano** (trailer-1; missione.cpp e missione_difesa.cpp).
  - **La causa:** `m688.bersaglio_nemico` era l'opcode dell'originale (i bit 18h diversi). Per la parte del luogo
    (18h, M7) i nemici erano i neutrali e mai le parti in crisi con lei: R_nave andava in `ignora`, la fregata non
    accostava, non pingava, non tirava (oslo o Koni). Ora, come `Missione::ostili`, il luogo è nemico di chi è con
    lei in crisi o in guerra; senza il luogo resta l'opcode, e il classico non cambia. Tocca la S16 e la S18.
  - **L'effetto:** con la caccia vera la fregata di guardia accostava dritta sull'Alfa che emergeva e lo investiva.
    Ora le navi dell'IA fanno l'anticollisione come i sottomarini: sul sottomarino vicino alla superficie (sopra il
    pescaggio della nave più 15 m di vela, STIMA) di cui hanno la posizione, con un CPA sotto i 150 m entro 1 km,
    accostano via per 120 s. Le armi antisom tirano da lontano, quindi passare sopra non serve.
  - **Le prove:** la S16 di trailer-1 (10 salve del Terne in 30 minuti con l'Alfa a 1 km; emerso, l'esito 43);
    «la nave dell'IA accosta via dal sottomarino in superficie sulla sua rotta» in prove_difesa (la Perry passa a
    800 m dal Victor fermo).
  - **Aperto:** investito dalla fregata, l'Alfa scendeva a 83 m e ci restava con la quota ordinata a 0 (integrità da
    175 a 165). È da capire se è il danno dell'urto (aria o casse) o un difetto.
- 30/10/2026 (runtime): **le anomalie del giro della libreria di trailer-1** (nota condivisa
  giro-controllo-libreria-30-10.md; l'ordine della principale).
  - **1. A3 al secondo 0-2 (14 missioni).** Una `classificazione_ia` possub o probsub alza l'allerta solo per un
    sottomarino (la classe detta è di un sottomarino della missione, o senza classe la confidenza), creduto della parte
    ostile o di una parte ignota. Il luogo è ostile solo in crisi o in guerra. Non la alzano un amico, un neutrale,
    una nave, un «biologico», né un osservatore neutrale, civile o non combattente (il cargo appoggio della S08).
    Prova in prove_forze: due Perry e un mercantile per 5 minuti, 6 classificazioni, nessun A3.
  - **2. S15.** Un'unità della propria parte in superficie entro 5 km si conosce a vista e per radio: né il
    sonarista né la vedetta la cambiano (lato_creduto; sott'acqua l'errore fra amici resta, 24 §8.7). Portaerei e
    ausiliarie (`non_pedina_mai`) non fanno più la nave antisom che «tiene sotto». Dopo un urto (missione_collisioni.cpp)
    le unità dell'IA si separano: arretrano di quanto sono entrate più 10 m (lungo la normale, o in quota fra
    sottomarini); chi urta perde l'abbrivio (al più metà della velocità dell'altro) e si ferma sulla sua rotta per
    120 s; chi è urtato accosta via; il giocatore non si sposta. Nella S15 a 2400 s non ci sono più urti, né A3, né il
    «tiene sotto».
  - **3. S12.** Il cannone tira solo sul sottomarino esposto: nel giro l'Alfa colpito dal Mk 46 risaliva da solo in
    superficie (quota 0 con l'ordinata a 60), e lì i colpi erano legittimi. Il kit conferma che il 76 mm non affonda
    un sottomarino moderno (dossier 22 §5.4): il modello dei danni non cambia. Il fuoco senza fine era un difetto: ora
    ogni pezzo ha il suo deposito (`DatiPezzo::deposito`, dalla ricerca 36 di trailer-1, docs/ricerca/36) e tace quando
    l'ha finito; con la stessa ricerca le ricariche: Mk 75 il tamburo in 3 minuti (STIMA), Mk 45 continuo, AK-726
    raffiche di 45 colpi per canna e 3 minuti di raffreddamento, AK-725 100 per canna e 2 minuti. Prova in
    prove_artiglieria: il Krivak in guerra sul 688 in superficie tira i 700 colpi dei due AK-100 e poi tace. Nella
    stessa ricerca, i pezzi della Knox e delle nordiche in artiglieria.py: Mk 42 (Knox), due 3"/50 Mk 33 binati (la
    Oslo, con la dotazione di tre su cinque nel 1989), Bofors 40/70 (Hauk), Bofors 57 Mk 2 e 40/70 (Stockholm); dati
    dei pezzi da NavWeaps, tavole di volo, precisione e portate per analogia (STIMA). La Grisha III ha l'AK-725 binato a
    poppa e l'AK-630 (prima non sparava).
  - **4. S16.** Vedi la voce delle fregate che non cacciavano.
  - **5. S08.** Nella S08 a 2400 s non ci sono più urti fra i Foxtrot. guida_batteria non c'entrava (prende il minimo
    delle velocità). La separazione è stata rifatta: la nave che urta perde l'abbrivio e si ferma; i sottomarini tengono
    l'abbrivio (senza velocità i timoni orizzontali non li portano in quota) e si separano in quota di 40 m, chi sta
    sotto scende e chi sta sopra sale; gli altri accostano via. Nella prova dei due sottomarini a 10 nodi prua contro
    prua la strisciata del passaggio dura 7 s (8 contatti a contatto continuo) e poi non si riurtano.
  - **6. Le raffiche di ping.** Le boe SSQ-47B pingano già una ogni 10 s per aereo. Le raffiche venivano dalle navi:
    il PING dello script a ogni passaggio (0,7 in A3). Ora un sonar di scafo dell'IA non ripinga prima di 10 s, l'eco
    da circa 7,5 km (STIMA); il giocatore e il classico come prima. Nella S12 l'episodio più lungo scende da 8585 a
    418 ping in 40 minuti, nella S16 da 1355 a 175. Prova in prove_difesa: lo script ogni 2 s, 6 ping in un minuto.
  - **Al disassemblatore:** il sonarista dell'IA classifica le navi di superficie con la confidenza 3 (CERTSUB) e un
    cargo senza sonar classifica i Foxtrot a 16 km («Cargo classifica Los Angeles»).
- 31/10/2026 (runtime): **il pedinamento ravvicinato** (S24 «Quota zero», il Tautog e il K-108; la principale;
  missione_pedinamento.cpp, `m688.pedina_vicino`).
  - **Chi:** un sottomarino dell'IA su un altro sottomarino, di qualunque parte (la simmetria: anche un sovietico su un
    americano). Solo nell'Enhanced; il classico non cambia.
  - **Dove:** nel cono sordo del pedinato, dalla poppa lungo la rotta che il pedinatore vede, a `distanza_m` (400 per
    difetto) e alla quota vista più `sopra_m`. Il cono viene dai sensori in ascolto del pedinato: il più stretto fra i
    `baffles_gradi` dei sonar di scafo e l'endfire della schiera filata (contato simmetrico, STIMA). L'Alfa ha 45
    gradi; il 688 35, e 25 con la schiera filata.
  - **Quello che vede:** la posizione dal suo contatto; la rotta, la velocità e la quota con un ritardo: un cambio
    oltre 10 gradi, 10 m o 2 nodi arriva dopo 20-60 s (la deriva del rilevamento, il segnale che cambia, STIMA), e la
    quota con un errore di 8 m (STIMA).
  - **L'urto:** l'anticollisione dell'IA non guarda la coppia; la fa il pedinatore stesso sulla geometria vista (CPA
    sotto 120 m in 2 minuti, entro 30 m di quota: accosta via e cambia quota di 40 m), quindi col ritardo. Un arresto
    o un'accostata brusca del pedinato può portare all'urto.
  - **La fine:** `pedina_vicino(u, -1)`, o senza contatto per 5 minuti. Registro: `pedina_vicino` e `fine_pedina_vicino`.
  - **Le prove,** in prove_difesa:
    - lo Sturgeon tiene il cono sordo dell'Alfa per 20 minuti a 384-433 m;
    - segue la salita dell'Alfa da 150 a 100 m: a 615 s non l'ha ancora vista, a 915 s è a 105 m;
    - l'arresto brusco dell'Alfa con lo Sturgeon a 250 m porta all'urto in 7 partite su 8;
    - il cono del 688 si restringe da 35 a 25 gradi con la schiera filata.
- 31/10/2026 (runtime): **la cassa zavorra squarciata, l'effetto in missione** (il modello è del kit, DanniSottomarini:
  la falla nella cella della cassa e la cassa che non tiene più l'aria; la principale).
  - **È una scelta di comando, non un limite fisico.** Il sottomarino dell'IA in superficie con una cassa squarciata,
    o che sta appena scendendo (sopra i 10 m), resta a galla: senza quella riserva il comandante teme di non
    riemergere (STIMA). Vale anche sotto il tiro, dove prima si immergeva (22 §9.8). Missione::casse_squarciate_ia,
    chiamata da raccogli_danni; solo con l'artiglieria dell'Enhanced.
  - **Il giocatore sceglie da sé:** nessun divieto. Lo avvisa il rapporto del kit («cassa zavorra … squarciata») e la
    fisica fa il resto: l'assetto, la galleggiabilità persa, l'esaurimento che non porta aria a quella cassa.
  - **La simmetria:** l'IA sceglie, il giocatore sceglie.
  - **La prova:** «con una cassa zavorra squarciata il sottomarino dell'IA resta a galla», in prove_artiglieria. Una
    Krivak in guerra sul 688 dell'IA in superficie, coi danni dei sottomarini, su più semi: quando una cassa si
    squarcia, la quota ordinata torna a 0 (sotto il tiro sarebbe 60 m). La quota vera resta della fisica del kit:
    l'acqua imbarcata può appesantirlo comunque.
- 31/10/2026 (runtime): **i contatti degli aerei ASW nella loro lista** (trailer-1; la principale; missione_asw.cpp, in
  fondo a passo_asw).
  - Prima la stima del controllore (`TracciaAsw`) restava dentro l'ASW e alle navi arrivava solo coi datum. Ora a ogni
    passo la traccia viva (meno di 2 minuti) dell'aereo controllato entra come contatto nella sua lista, sulla
    sorgente con più rivelazioni associate.
  - **Il contatto:** la qualità dalle rivelazioni (sotto la traccia solida finché non è confermata, poi solida), la
    posizione stimata, il rilevamento dall'aereo, il sensore «boe». Confermata, ha la durata lunga: è classificata per
    `m688.classificato`.
  - **La simmetria anche negli errori:** se la sorgente è una nave o un biologico, il falso contatto entra come tale.
  - **L'effetto:** le condizioni «unità vede» e «tenuto da» e la storia dei contatti (`contatto_cumulato_s`) contano anche
    gli aerei.
  - **La prova:** «il contatto del P-3C dalle boe entra nella sua lista e nella storia dei contatti», in
    prove_asw_missione: col seme 3 il P-3C tiene l'Alfa dalle boe, lo conferma, e la storia lo conta (320 s a 1180 s).
- 31/10/2026 (runtime): **gli aerei e l'allerta in pace** (la principale, dal giro della S23 di trailer-1).
  - **(b) Un aereo neutrale non tira mai:** il controllore ASW di un aereo della parte neutrale ha sempre la
    situazione di pace, anche con una voce `aereo_asw` di guerra (missione_asw.cpp, all'avvio del controllore).
  - **(a) Un aereo senza voce `aereo_asw`** prende la situazione della missione fra la sua parte e quella del
    giocatore (le forze), poi il blocco carriera, poi la guerra. Le missioni originali restano in guerra (la base delle
    forze); una missione di pace è in pace.
  - **(c) L'albero visto in pace:** un albero, una testa o un battello visto da un osservatore in pace con la parte del
    giocatore, o neutrale, porta l'allerta al più alla vigilanza (1, STIMA), non al combattimento (missione_forze.cpp,
    evento_allerta).
  - **Le prove:** «in pace un aereo senza voce non tira, e un aereo neutrale mai» (prove_asw_missione: tre ore
    sull'Alfa, nessun lancio, mentre lo stesso P-3C con la voce di guerra tira); «in pace l'albero o il battello visto
    porta al più alla vigilanza» (prove_forze: la vedetta della Krivak vede di notte il 688 coi fanali, allerta 1).
- 31/10/2026 (runtime): **anche il giocatore non ripinga prima di 10 s** nell'Enhanced (la simmetria, l'utente: «Sì,
  stesso limite»). Il ping troppo presto non parte e il sonar avvisa: «il ping precedente non è ancora tornato»
  (messaggio 414). Il classico non cambia. Prova in prove_difesa.
- 31/10/2026 (runtime, col diff del disassemblatore): **l'MG-70 e l'MG-24 per un sovietico del giocatore**
  (missione_bqs15.cpp). Lo stesso ordine del BQS-15 accende il sonar delle mine della sua classe (la voce `bqs15_fondo`
  di acustica.toml: l'Alfa ha l'MG-24, con le mine e la portata 0,6, STIMA), coi suoi messaggi (MG-70 493-495, MG-24
  496-498), le spazzate, la portata ridotta e l'emissione agli intercettatori della sua voce. Prova in prove_bqs15:
  l'Alfa del giocatore accende l'MG-24 e vede due mine. I nomi sulla consolle e i tasti restano quelli del BQS-15
  (l'interfaccia è della principale).
- 31/10/2026 (runtime): **il giro lungo di trailer-1, le armi e gli urti** (nota giro-lungo-tutte-31-10.md; la
  principale).
  - **1. Le armi dell'IA (SCENE8C):** nell'Enhanced l'arma che uno script fa lanciare all'IA (nuova_unita_da_buffer) è
    della dotazione del lanciatore, adatta al bersaglio, e mai su un amico. Una nave che tira un siluro tira quello
    antisom della sua classe (la tabella di missione_antisom_navi.cpp: la Kidd e la Perry il Mk 46, non più Mk 48 e
    Spearfish); senza un siluro antisom nella dotazione non tira. Un antinave non parte su un sottomarino immerso,
    né come arma dello script né come MISSILE (lancia_missile_ia). L'ATTACK non colpisce un'unità della propria parte
    (la Nimitz e il suo SH-60B). Col seme 1-3, in 15 minuti di SCENE8C, solo Mk 46 sull'Alfa e nessun fuoco amico.
  - **2. L'artiglieria della S12:** i 1063 colpi del giro col seme 2 stanno dentro il deposito (600 per Mk 75, due
    Perry): ora il fuoco finisce. Che non affondi è il modello dei danni (il kit: il 76 mm non affonda un sottomarino
    moderno).
  - **3. Gli urti all'avvio (SCENE1B, 1C, 4A):** le coppie già sovrapposte alla partenza (le posizioni dell'originale)
    non danno urti finché restano a contatto; quando si staccano, un urto vero conta di nuovo. Solo nell'Enhanced (le
    collisioni ci sono solo lì); le posizioni non si spostano. Nessun urto nei primi due minuti di 1B, 1C e 4A col seme 2.
  - **4. L'Oscar della SCENE9C:** dottrina coerente. Otto P-700 e poi salve di Type 53 sui cargo, con un nuovo attacco
    dopo ogni salva mancata, 28 armi in tutto, dentro la dotazione. Il punto è la resa: un solo colpo su 20 Type 53
    (seme 2, 2 ore). Da guardare la soluzione di tiro dell'IA sui cargo o il modello del Type 53 (aperto).
  - **L'«emerso» al radar:** il battello emerso si vede fino all'orizzonte della sua vela vera
    (`[sagome.*].falsatorre_altezza_m` di artiglieria.toml, il kit: 688 7,6 m, Typhoon 14,3), non a 6 m fissi
    (probabilita_giro, parametro `altezza_vela_m`). Prova in prove_elettronica.
- 31/10/2026 (runtime): **la resa dell'Oscar della SCENE9C, la soluzione di tiro** (la principale). Nel seme 2, 12 Type
  53 e USET-80 e un colpo. Il siluro fa quello che deve (il 53-65K aggancia una scia e finisce l'autonomia inseguendola,
  l'USET-80 cerca a 15 m e passa a 3,6 km). La colpa è del lancio: lo script lancia una salva su ogni traccia solida, da
  16-28 km con 18-19 km di portata, la rotta andava al punto stimato senza anticipo, e la TMA dell'IA dava soluzioni
  sbagliate (osservabile con 15-28 km d'errore e 0,1 nodi, o 34 nodi a 90° dalla rotta vera). Nell'Enhanced (armi
  dell'IA):
  - **l'anticipo:** il siluro dell'IA va al punto d'incontro, dalla rotta e dalla velocità della soluzione della sua
    TMA (stima_tma_ia, se osservabile), come il giocatore con la sua (nuovo_siluro, punto_d_intercetto);
  - **la portata:** oltre 0,9 della portata (STIMA), misurata fino al punto d'incontro, il siluro resta in tubo
    (arma_ia_al_lancio). Se la soluzione dice che non lo raggiunge, nemmeno;
  - **la posizione vecchia:** niente salva se, dall'ultimo aggiornamento, il bersaglio può essersi spostato più di 2 km
    (STIMA: l'acquisizione passiva tipica di una testa, 2-2,5 km in siluri.toml), alla velocità della soluzione o, senza,
    a 15 nodi (al minimo 3). La posizione della TMA ha l'età della sua ultima soluzione osservabile
    (TmaIa::ultima_osservabile), le altre (eco, vista, radar) quella dell'ultima scoperta. Si lancia di nuovo quando
    arriva una posizione fresca.
  - **Aperto:** la TMA dell'IA sui cargo (risolvi_tma, tma_ia.cpp), al disassemblatore. Finché la soluzione è sbagliata,
    né l'anticipo né la portata possono fare molto: nella SCENE9C restano un colpo e due lanci in meno.
  - **Prova** in prove_armi_ia_missione: il Victor con la posizione della Perry tira a 6 km, non a 40 km né con una
    posizione di 10 minuti prima; il tubo resta carico.
- 31/10/2026 (runtime): **la simmetria dei sensori, punti 4 e 5** (nota disassemblatore-simmetria-sensori.md).
  - **4. Lo specchio della schiera per l'IA:** l'IA, come il giocatore, non sa da che lato della schiera sta il
    rilevamento. Al primo rilevamento della schiera ne crede uno a caso (0,5, dal seme della coppia) e usa quello per
    il contatto e per la TMA. Lo risolve un'accostata di almeno 30° dalla rotta a cui ha scelto (STIMA, la regola pratica
    di 30-60°) o un sensore senza specchio (la sfera, lo scafo). Se il lato era quello falso, la TMA si rifà da capo e la
    posizione data dalla TMA decade. Il nucleo/aob_ia (probabilita_specchio) è un'altra cosa: l'AOB a vista.
  - **5. I danni a sonar e schiera dell'IA:** la cupola e la schiera guaste dai danni (LimitiBattello::sonar e
    ::schiera, DanniSottomarini) tacciono anche per i sottomarini dell'IA (Missione::sonar_ia); restano il VDS e gli
    idrofoni d'intercettazione. Il raddrizzamento della schiera dopo una virata (fine_virata_) si misura per tutti anche
    senza il giocatore.
  - **Prove** in prove_percezione_ia: in 12 semi il 688 dell'IA con la sola schiera crede il lato vero e quello falso
    (almeno 2 volte ciascuno), e dopo un'accostata di 60° il lato è sempre quello vero; con cupola e schiera guaste non
    sente il Victor, riparate sì.
- 31/10/2026 (runtime): **la simmetria dei sensori, punto 1: il sonarista dell'IA senza filtro sulla parte vera**
  (missione_sonarista.cpp). Le righe LOFAR e il DEMON del fascio arrivano al sonarista dell'IA per ogni contatto con
  traccia solida, di qualunque parte, come a quello del giocatore: un Victor classifica anche il Krivak della sua parte,
  un neutrale classifica chi sente. Il ripiego sul costo: le righe solo per gli 8 contatti più forti di ogni unità
  (RIGHE_CONTATTI_MAX, STIMA). La CPU non sale (1800 s di gioco, tempo utente: S12 18,6 → 16,6 s, S15 10,4 → 10,1,
  I13 91,7 → 89,6). Prova in prove_percezione_ia. Nelle prove della libreria le unità rese «cieche» (neutrali) ora si
  fanno anche sorde dove serve (sordi(), Missione::sonar_ia): un neutrale che sente l'Alfa lo classifica e brucia la S10.
- 31/10/2026 (runtime, col disassemblatore): **la nave dirige il suo elicottero di continuo** (MATCH;
  missione_antisom_navi.cpp). Finché l'elicottero imbarcato non è in rientro, ogni 45 s (STIMA, 30-60 s) la nave gli
  passa il suo contatto col bersaglio (Cue contatto_nave), se ne ha la posizione, col raggio dall'errore della traccia:
  dalla TMA hypot(sigma_distanza, sigma_trasversale), se no 300 m (eco, radar, vista), fra 300 e 3000 m. Prima il
  contatto passava una volta sola, al decollo. Il controllore (disassemblatore, asw_aereo.cpp) per un mezzo col solo MAD
  usa il cue per tenere viva la traccia e seguire il sottomarino; il fix resta al MAD. Prova in prove_armi_ia_missione
  (la Type 22 col Lynx su un Victor III a 100 m).
- 31/10/2026 (runtime): **la simmetria dei sensori, punto 6: le chiglie anche al ping dell'IA** (missione_ghiaccio.cpp,
  echi_chiglie_di; impulso_sonar_enhanced). Sotto la banchisa il ping di un'unità dell'IA ha gli stessi echi delle
  chiglie di quello del giocatore. Nessun contatto fantasma: una chiglia più forte del bersaglio nello stesso fascio
  (±3°) e nella stessa cella di distanza (±500 m, STIMA) dà all'IA la posizione e la quota della chiglia al posto di
  quelle del bersaglio. Con le forze di bersaglio dei dati è un effetto dei bersagli al limite della scoperta: le chiglie
  rendono 0-5 dB, un 688 a 3 km 27 dB. Prova in prove_ghiaccio_missione (nessun fantasma; in acqua libera nessuna
  confusione).
- 31/10/2026 (runtime): **i transitori dello scafo** (missione_scafo.cpp; nota disassemblatore-transitori-scafo.md;
  voci [transitori.*] di acustica.toml, categoria «scafo»). Il portello che sbatte, l'attrezzo che cade, il colpo dei
  piani, la pompa che parte, la TDU, la soffiata d'emergenza e lo scafo che scricchiola nascono dagli stati della
  missione, per tutti i sottomarini immersi, giocatore e IA (la simmetria). Le frequenze sono quelle della nota (STIMA),
  per l'equipaggio (×3 recluta, ×1 medio, ×0,4 esperto; il giocatore da equipaggio_giocatore, l'IA dal suo livello) e ×0,25
  in silenzio di manovra o ai posti di combattimento (il giocatore da silenzio_giocatore, l'IA con l'allerta da A2). La
  soffiata parte sempre all'emersione d'emergenza (l'ordine del giocatore, l'E1 dell'IA); lo scricchiolio nei cambi di quota
  oltre 100 m, sotto il 40% e il 70% della quota di collaudo. Vanno nella scena acustica come i transitori delle armi
  (fascia 950000-959999; il proprio scafo non si ascolta da sé). L'IA li sente coi sensori passivi (ascolta_transitorio):
  un contatto sul battello, e sugli ostili l'allerta come un POSSUB (PROBSUB se il contatto c'era, o per la soffiata).
  Letture: scafo_nati(), scafo_sentiti(). Aperto: il sonarista dell'IA che dal transitorio dà subito la categoria (il
  modulo del disassemblatore). Prove in prove_scafo (lo scricchiolio del Kilo sentito a 2 km, non a 6; la soffiata a 25 km).
- 31/10/2026 (runtime): **i posti di combattimento del battello del giocatore** (missione_scafo.cpp, per l'allarme sonoro
  di trailer-1): ci si va al primo allarme siluro, a un lancio proprio o con un contatto ostile classificato almeno per
  categoria entro 10 km; la condizione normale torna dopo 15 minuti senza nessuno di questi (STIMA). Nel registro
  `posti_di_combattimento`, livello 1 o 0, la causa in classe_arma. Ai posti i transitori dello scafo del giocatore calano
  come in silenzio.
- 05/11/2026 (quadro-radio): **il quadro radio ai sottomarini dell'IA** (missione_forze.cpp, `passo_radio_sottomarini`;
  progetto e decisioni dell'utente in `tools/re/prove/quadro_radio/proposta.md`). Solo nell'Enhanced con le armi dell'IA, mai
  nelle originali; interruttore `usa_quadro_sottomarini`.
  - **Ricezione:** come il giocatore con l'albero, a quota d'antenna (albero, periscopio o fino a 6,6 m, predicato unico con ROE e
    falso ordine), niente banchisa, al ciclo, 80 s di copia, al 50% oltre 10 nodi. Prima: fino a 20 m anche senza albero. Il filo
    e l'ELF restano del giocatore. La spia verso i sottomarini passa dal canale (docs/28).
  - **Salita:** in crisi e in guerra alle finestre, 1800 s o 3600 s in A3, un tentativo per ciclo; mai in pace, con una TMA o un bersaglio
    agganciato. **Caccia sul dato:** con le armi libere, fino a `r` di 30 km, senza un contatto suo. Il dato non è una soluzione:
    il siluro vuole il contatto proprio.
  - **Spettatore:** `ascolto_broadcast`, `dato_radio_usato`, `dato_radio_lasciato` in `reazione_ia`.
  - **Prove:** 16 in prove_forze, 34 guasti rossi, rilievo 0/110. Nuova scena `PROVA_M4_QUADRO`, fuori dai vetrini pronti.
  - **Limiti misurati** (manuale E6 §6-§7; partite del 15 ottobre 1989, di prima della correzione della data, da rimisurare con
    T12): si vede poco. Il dato è usato due volte su 18 partite, tutte e due nella stessa
    (I07 seme 1, stesso battello). Il confronto col quadro spento arriva solo a 7200 s; oltre, il giudizio si basa su perdite,
    esiti e lanci. La M4 non misura la caccia, perché il Victor sente il convoglio da solo fino a circa 95 km; in S5 il
    Victor non sale mai, lo ferma il bersaglio agganciato. Nelle missioni il freno principale sono i pochi rapporti su unità
    ostili e le armi non libere in crisi (letture delle tracce, non una misura). Il contatto col solo rilevamento continua a
    bloccare il dato, per scelta dell'utente (06/11/2026).
- 06/11/2026 (trailer-1): **stato vero delle scene dopo la data** (note condivise, `trailer1-punto9-06-11.md`). Con la data della scena, ora anche
  in `scene_interazioni --semi` e nelle prove lunghe, su tre semi: passano S1, S2, S3A, S4, S5, S6, S8; **S1G cade** (la prima arma in acqua è a
  790 s, il colpo a 1010-5470 s: la prova conta il colpo, ma le navi ora tirano siluri veri); **S7 cade nel seme 3** (il Victor lancia). S6
  passa solo perché la prova non chiede il ritrovamento: 100 min in un seme, mai negli altri due. Attese e codice non toccati, in attesa di decisione.
- 06/11/2026 (trailer-1): **S7, seme 3, la causa.** Nessuna regressione: stesso lancio dal binario del 29/10 a oggi. La regola «guerra» a 60 minuti vale per tutta la
  parte, quindi `R_pedina` del Victor ha già `armi_libere`, e lancia appena ha la soluzione (seme 3: 3770 s); la «copia» a 75 minuti cambia solo il comportamento. Con
  la riga di lancio tolta al Victor in una copia della scena i lanci vanno a zero. Proposta e valori attesi nella nota `trailer1-punto9-06-11.md`; scena non toccata.
- 06/11/2026 (trailer-1): **S7 corretta, solo nella scena.** Alla guerra il Victor passa a `R_pedina_vicino`, che non lancia, e a 75 minuti a `R_sottomarino`. Variabile `100 + id`
  scartata: lo scrittore delle missioni ammette solo le variabili dell'autore, e `R_sottomarino` legge le stesse salve. Sei semi, 7200 s: nessun lancio del Victor prima
  della copia (dopo: 6510 s nel seme 2, 6230 s nel seme 6), il Kara lancia in tutti a 3600-3610 s. Cambia solo `PROVA_S7_PORTAEREI.lua`.
- 06/11/2026 (chiusura): **isolamento di un compartimento e priorità delle squadre, solo il giocatore** (docs/10 §10.19). **Due limiti scritti alla simmetria, decisi dall'utente:** l'IA non isola, e non cambia le priorità (tiene l'ordine di partenza della dottrina). Misura sul binario di partenza, `tools/re/prove/isola/serie_ia.sh`: 576 colpi vicini di Mk 46, la regola proposta (fuoco oltre 0,5 per 120 s fuori dal comando) non scatta mai, quindi si vedrebbe poco. Non misurati: artiglieria, collisioni, colpi ripetuti. Per riprenderla: nota `punto4-isolamento-proposta-06-11.md`.
