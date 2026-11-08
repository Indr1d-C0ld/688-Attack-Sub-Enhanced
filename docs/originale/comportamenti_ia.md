# Comportamenti delle unità nell'originale (riferimento per l'IA del remake)

Ricavati dagli script delle 22 scene con `tools/re/tok_ia.py`. Lo pseudo-codice completo, scena per scena, è
in `re/tok/ia/SCENE*.txt`; le frequenze di condizioni e azioni per ruolo e per classe sono in
`re/tok/ia/statistiche.json`. Le distanze di `IF_TGT_NEAR` sono **per asse** (un quadrato, non un cerchio)
in unità di posizione: 50 = 1 nm.

## 1. Come funziona

- Ogni unità ha tre script: **all'arrivo** (`+0E`, quando raggiunge un punto o un bersaglio),
  **al bersaglio** (`+10`, eseguito una volta quando un contatto diventa bersaglio, anche dopo *n*
  aggiornamenti del contatto agganciato con `ENGAGE n`) e **bersaglio perso** (`+56`). Dettagli in
  [sensori_e_navigazione.md](sensori_e_navigazione.md) §8. Gli script se li riassegnano da
  soli (`BEHAVIOR`, `ENGAGE`, `PURSUE`, `LAUNCH`), quindi un comportamento è una piccola macchina a stati.
- L'unica "conoscenza" del nemico è il **contatto bersaglio** (`+58`) con la sua qualità di traccia: quasi
  ogni decisione parte da `IF_TRACK_GOOD` (traccia solida, qualità ≥ 800) e da `IF_TGT_NEAR`.
- Il sonar passivo sente **solo nel settore di prua**: ±30°, o ±90° con il bit 0 di `+54` (schiera trainata o
  ascolto ampio). Per questo quasi ogni script mette la prua sul rilevamento (`TURN_TO_TGT`). Modello
  completo in [sensori_e_navigazione.md](sensori_e_navigazione.md).
- I **timer** (`AT n`, n tick da 2 s) scandiscono attese, salve e fine delle evasioni; le **variabili di
  scena** coordinano più unità (per esempio il numero di siluri già lanciati dal gruppo).

### Flag d'unità (`+06`)

| Bit | Significato | Esempi |
|---|---|---|
| 0–1 | categoria: 1 sottomarino, 2 nave, 3 aereo | 688/Alfa 1, navi 2, Seahawk 3 (ANALYZE rifiuta i contatti con 3) |
| 08h / 10h | schieramento: lato del 688 / lato sovietico | 688 = 09h, Alfa = 11h |
| 20h | arma | Mk 48 = 29h, 533 mm = 31h |
| 40h | non combattente, bersaglio passivo | mercantili 42h/4Ah, petroliera 52h, Forrest Sherman di TORPEX 52h |
| 80h | falso bersaglio | biologici C0h, esca C1h, bersaglio per missili C2h |

Le condizioni `IF_TGT_F20`, `IF_TGT_F02`, `IF_TGT_F03`, `IF_TGT_ENEMY` e `IF_TGT_NEUTRAL` leggono questi bit:
«il bersaglio è un'arma», «è una nave», «è un aereo», «è nemico», «è neutrale».

## 2. Vocabolario usato

- **Condizioni** (in ordine di frequenza): traccia solida, bersaglio vicino, id del bersaglio, variabile di
  scena, bersaglio agganciato su di me (`IF_TGT_LOCKED`), bersaglio è un'arma, flag di scena dell'unità,
  traccia lunga o media, caso (`IF_RND`).
- **Azioni**: telegrafo (velocità in 1/256: 10–20 = ricerca lenta, 128 = metà, 256 = avanti tutta, 270 =
  quasi massima forza, 0 = ferma), accostata sul rilevamento (`TURN_TO_TGT`), virata opposta (`TURN_AWAY`),
  virata relativa, uguagliare velocità e quota del bersaglio (pedinamento), quota, navigazione a punti,
  inseguimento, lancio, attacco a distanza (`ATTACK`), attacco a esito casuale (`ATTACK_RND`), ascolto ampio
  (`F54_1`), periscopio (`F54_2`), impulsi del sonar attivo (`PING`) e rumori transitori (`TRANSIENT`).
- **Soglie di distanza più usate**: 450 (9 nm, 20 volte), 64 (1,3 nm, 19), 250 (5 nm, 17), 150 (3 nm, 11),
  500 (10 nm), 400 (8 nm), 380 (7,6 nm), 40 (0,8 nm).

## 3. Archetipi

### A. Sottomarino d'attacco sovietico (Alfa, Victor III, Akula; anche Typhoon e Oscar)
Esempi: SCENE1A `L_0214`, SCENE7A `L_0345` (lo stesso script guida l'Alfa e il Typhoon).
1. **Senza traccia solida**: avanza piano (telegrafo 10/256), prua sul rilevamento, e riprova.
2. **Traccia solida, lontano**: uguaglia velocità e quota del bersaglio e lo segue (pedinamento).
   In SCENE7A invece accelera a massima forza.
3. **Entro 7,6–8 nm (ma non entro 1,3 nm)**: lancia un siluro (533 mm), al massimo N per scena (variabile
   condivisa), e aspetta 120 tick (4 minuti di gioco) prima del successivo.
4. **Siluro in arrivo agganciato su di sé**: virata di 180°, avanti tutta, ascolto ampio spento (`F54_1 0`), per 30 tick,
   poi torna al comportamento normale. In SCENE7A, se il siluro è entro 1,3 nm, risale a 30 ft a massima
   forza.
5. Il Typhoon (SSBN) usa lo stesso script dell'Alfa: nell'originale un lanciamissili balistici caccia e
   attacca come un sottomarino d'attacco.

### B. Nave antisommergibile (Kirov, Slava, Kara, Krivak; Perry, Kidd)
Esempio: SCENE6A `L_0315`.
1. **Ricerca**: velocità bassa (telegrafo 20/256, circa l'8% della massima, per non disturbare il sonar),
   prua sul rilevamento del contatto.
2. **Contatto del giocatore**: annulla la navigazione, massima forza (270/256) sul rilevamento, marca i
   contatti per il gruppo.
3. **Attacco a distanza**: entro 5 nm, tramite una sottoroutine, **fa comparire un siluro da 450 mm vicino
   al bersaglio** (`JUMP_TO_TGT`). Così l'originale rappresenta SS-N-14 e ASROC: niente volo, l'arma nasce in
   acqua vicino al sottomarino.
4. **Difesa da un siluro in arrivo**: sceglie a caso uno di quattro punti di fuga e ci va a massima forza.
   Se il siluro è entro 0,8 nm con traccia solida, `ATTACK` con portata 300 e danno 5–19, cioè le bombe
   antisiluro RBU.
5. **Unità nemica di superficie entro 9 nm** con traccia solida: segnala (messaggio) e si collega al
   bersaglio per gli attacchi del gruppo.

### C. Elicottero ASW (SH-60B Seahawk)
Esempi: SCENE1A `L_0190`, SCENE7A `L_0159`.
- **Sorveglianza a protezione**: controlla se determinate navi amiche (per id) sono state colpite; se sì,
  vola sul punto del bersaglio (`GOTO_TGT`), cioè va a cercare chi ha sparato.
- **Boa marcatrice**: con traccia solida su un'unità designata fa comparire sul bersaglio una boa
  rumorosissima (`+3A` = 255, tipo 2Bh) che emette impulsi. Il giocatore la sente e localizza così l'SSBN
  (missione 7). Poi riprende la ricerca dopo 200 tick.

### D. Nave in pattugliamento o bersaglio d'esercitazione (TORPEX)
Esempio: SCENE1A `L_01f5`.
- Mantiene rotta (`HEAD_TO -1`) e virata lenta; se il contatto è il giocatore, accosta e rallenta
  (telegrafo 20). Con **traccia solida** fa scattare i messaggi di scoperta e l'esito di fine missione (il
  giocatore è stato scoperto). La "scoperta" è l'evento di gioco, non un attacco.

### E. Mercantili, convogli, rifornitrici
- Flag 40h e sonar a 0 (`+3B` = 0): non sentono niente e seguono rotte a punti, spesso scelte a caso fra
  varianti (`IF_RND` all'inizio della scena). Non reagiscono: la difesa del convoglio è affidata alle scorte.

### F. Biologici
- Unità con flag C0h, rumore costante, nessun comportamento: servono come falsi contatti.

## 4. Limiti e trucchi dell'originale (da non copiare nel remake)

- Distanze per asse (quadrato) invece che vere distanze; la soglia diagonale è circa il 41% più lunga.
- Le armi a distanza (SS-N-14, ASROC, siluri da elicottero) **compaiono vicino al bersaglio**; i missili
  antinave del giocatore sono solo un `ATTACK_RND` ritardato (vedi `docs/04_vm_missioni.md` §8).
- Nessuna dottrina di gruppo vera: il coordinamento passa da variabili di scena (contatori di siluri, flag)
  e dalla marcatura dei contatti.
- Le reazioni dipendono solo da traccia solida e distanza: niente valutazione della minaccia, della
  propria rumorosità o dell'ambiente (strati termici e fondale contano solo nella scoperta).
- L'evasione dai siluri è sempre la stessa: virata di 180° e massima forza.
- Il difetto del rumore (§8 di `docs/04`): un'unità che sta cambiando velocità diventa quasi inudibile.

## 5. Che cosa può tenere il remake

- La struttura a **fasi** (ricerca lenta → contatto → avvicinamento o pedinamento → attacco → evasione) è
  una buona base per la macchina a stati dell'IA (roadmap, fase 4), da arricchire con la dottrina reale dei
  dossier di ricerca.
- Il **pedinamento** con velocità e quota uguagliate, la **ricerca a bassa velocità** delle navi ASW e
  l'**intervallo tra le salve** sono comportamenti plausibili e già tarati sul ritmo del gioco.
- Il **datum** dell'elicottero (andare dove una nave amica è stata colpita) e la **boa marcatrice** sono
  idee da tenere, con un'implementazione realistica (sonoboe, sonar a immersione).
- I valori delle soglie (attacco tra 5 e 8 nm, difesa ravvicinata sotto 1 nm) sono un riferimento per
  tarare la versione «classica».
