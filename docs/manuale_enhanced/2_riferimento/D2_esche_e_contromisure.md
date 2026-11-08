# D2. Esche e contromisure

*Autore: kit, per il runtime. Revisione del 27/10/2026. Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Decide se un siluro in arrivo si lascia portare via. Un'esca acustica fa rumore, rimanda un'eco o tutte e due, e la
testa del siluro la scambia per il battello. Un'esca ferma inganna bene una testa che ascolta e male una che emette;
un'esca semovente inganna anche quella che emette. Contro un operatore che guida il siluro via filo, l'esca ferma
vale poco. Per chi gioca la scelta è quando lanciarla, a che quota, e che cosa fare subito dopo: virare, cambiare quota
e velocità mentre il siluro si occupa dell'esca. L'IA usa le stesse esche con le stesse regole.

## 2. Il modello

Le esche sono unità della missione con il flag dei falsi bersagli (80h). Il siluro dell'Enhanced le vede come
sorgenti con `esca = true` (`SorgenteAcustica`, `Missione::passo_siluri`) e decide nella verifica della testa
(capitolo D1 §2.6). I dati sono in `data/motore/siluri.toml [esche.*]`; il codice in `nucleo/siluro.hpp`
(`DatiEsca`, `rotta_esca_mobile`), `missioni/src/missione_esche.cpp` (esche semoventi), `missioni/src/missione_asw.cpp`
(`lancia_esca_ia`) e `missioni/src/missione_difesa.cpp` (quando l'IA le lancia). La manovra che le accompagna è in
`nucleo/evasione.hpp`.

### 2.1 Le esche del gioco

| Esca | Chi la porta | Da dove | Moto | Dati |
|---|---|---|---|---|
| ADC Mk 2 | 688 (6, due lanciatori) | lanciatore d'esche (tubi 5 e 6 della stazione Armi) | ferma | `[esche.adc_mk2]` |
| esca dell'Alfa (MG-84 Korund nel catalogo) | Alfa (4, un lanciatore) | lanciatore d'esche | ferma | come l'ADC Mk 2 |
| MG-74 Korund | Alfa; sottomarini nucleari sovietici dell'IA | tubo da siluri | semovente | `[esche.mg74]` |
| MOSS Mk 70 | 688, solo nel modo moderno (what-if) | tubo da siluri | semovente | `[esche.moss_mk70]` |

Le scorte e i posti sono nel capitolo D5. L'esca dei lanciatori è la stessa unità per tutte le marine: la missione le
dà i dati dell'ADC Mk 2.

### 2.2 L'esca ferma (ADC Mk 2)

**Il lancio.** Dal lanciatore la mette in mare lo script della missione, lo stesso dell'originale (il gestore
dell'arma `esca`):
- nasce nella posizione e alla quota del battello e ci resta;
- si muove a 0,9 nodi (0,47 m/s): per la testa è «quasi ferma», sotto i 2 m/s di `DOPPLER_ESCA_M_S`;
- lo script le dà il rumore dopo 2 s; per la testa dell'Enhanced il livello vale da subito (la tabella qui sotto);
- dopo 160 s lo script la toglie (60 s in quattro scene dell'originale), col messaggio 44.

**Per la testa** (`Missione::passo_siluri`):

| Grandezza | Con l'acustica dell'Enhanced | Senza |
|---|---|---|
| Livello sulla scala della testa | firma `adc_mk2` di `acustica.toml`: 150 dB in banda larga, più 18 dB della scala, 168 dB | 150 dB (`[esche.adc_mk2] livello_db`) |
| Forza di bersaglio | 0 dB | 0 dB |
| Estensione | 0 m (eco puntiforme) | 0 m |

**I due livelli.** I 18 dB fra le due colonne non sono un difetto: sono due modelli, ciascuno con la sua scala
(D1 §2.5).
- Con l'acustica il livello viene dalla firma, come per tutte le sorgenti, e passa alla scala della testa coi 18 dB
  di `SCALA_TESTA_DB`. Su questa scala un 688 a 4,5 nodi vale 130,5 dB: l'esca è più forte di circa 37 dB.
- Senza l'acustica le unità hanno la STIMA dal rumore dell'originale (`105 + 2,5 · rumore`, più 0,5 dB per nodo
  oltre 5), e l'esca il suo `livello_db`. Su questa scala un 688 a 5 nodi col rumore 6 vale 120 dB: l'esca è più
  forte di 30 dB.

In tutti e due i modelli l'esca supera di molto il battello, e una testa passiva la sceglie quasi sempre. Con 0 dB di
eco una testa attiva la vede a una portata minore che un sottomarino: per il Mk 48 1500 · 10^(−15/40), circa 630 m.

**L'IA** lancia la sua esca ferma con il suo codice (`Missione::lancia_esca_ia`): stessa unità, ma dura 600 s, la
`durata_s` di `[esche.adc_mk2]` (`ESCA_VITA_S` = 160 s se la voce manca). Il giocatore resta coi 160 s dello
script, come nell'originale. **In arrivo:** l'ADC Mk 2 del giocatore coi dati di `siluri.toml` (durata e quota).

### 2.3 Le esche semoventi (MG-74, MOSS Mk 70)

Si lanciano da un tubo da siluri come un siluro (`Missione::lancia_esca_mobile`) e contano fra le armi in acqua.

| Campo | MG-74 | MOSS Mk 70 | Che cosa fa |
|---|---|---|---|
| `velocita_kn` | 10 | 10 | velocità di corsa |
| `quota_max_m` | 250 | 300 | non scende oltre |
| `durata_s` | 1200 | 1200 | batteria: poi esce dal gioco |
| `livello_db` | 140 | 140 | rumore sulla scala della testa, senza l'acustica |
| `eco_db` | 15 | 15 | forza di bersaglio del ripetitore, come un sottomarino |
| `imita_rumore` | sì | sì | con l'acustica suona come il lanciatore |
| `virata_ogni_s`, `virata_gradi` | 240, 40 | 240, 40 | la manovra programmata |
| `in_servizio_dal` | 1974 | 1976 | |

- **Rotta e quota.** Vale la regola dei soli campi toccati, come per i siluri (D1 §2.3; `Missione::applica`, lancio
  dal tubo). Se il giocatore ha toccato la rotta del tubo, l'esca parte con quella rotta; se no, con la rotta del
  battello. Se ha toccato la quota di corsa, l'esca corre a quella quota; se no, alla quota del battello. La quota non
  supera la massima dell'esca; l'esca parte già a quella quota.
- **Manovra** (`rotta_esca_mobile`): dritta per i primi 240 s, poi a zig-zag di ±20° attorno alla rotta di lancio,
  cambiando lato ogni 240 s, «come un sottomarino». Vira a 3°/s e cambia quota a 2 m/s (STIMA).
- **Il rumore.** Con l'acustica dell'Enhanced l'esca prende la firma del lanciatore alla propria velocità
  (`Missione::sorgente_acustica`): un'Alfa che lancia una MG-74 mette in mare un'«Alfa a 10 nodi». Senza l'acustica,
  140 dB.
- **L'eco.** 15 dB, come un sottomarino, ma puntiforme (estensione 0 m).
- **Il moto.** 10 nodi sono 5,1 m/s: sopra la soglia del Doppler, la testa attiva non la smaschera come ferma.

### 2.4 Che cosa fa la testa davanti a un'esca

La testa sceglie la sorgente più forte e la verifica per 4 s (D1 §2.5-2.6). Alla fine della verifica, se il
candidato è un'esca, lo rifiuta con probabilità `p`; se non lo rifiuta, lo aggancia («esca sedotta») e lo insegue.

| Testa | ADC Mk 2 (ferma) | MG-74, MOSS (semoventi) | Perché |
|---|---|---|---|
| passiva | `p` = 0,1 | `p` = 0,1 | il passivo ha solo il rumore, e l'esca è più forte |
| attiva | `p` = 0,7 | `p` = 0,2 | 0,5 per il Doppler nullo, 0,2 per l'eco più corta di 20 m |
| mista | sceglie in passivo, verifica in attivo: `p` = 0,7 | `p` = 0,2 | la verifica è già in attivo |
| segui-scia | non la vede | non la vede | segue solo le navi in moto |

- Un'esca rifiutata resta rifiutata per quel siluro.
- Un'esca sedotta no. La spoletta non scatta sulle esche, quindi il siluro le passa accanto, le perde dal cono, gira
  in cerchio dopo 6 s e può riprenderle alla verifica successiva. In passivo può girare attorno all'esca finché
  questa si spegne o lui finisce l'autonomia.
- **Il disturbo** (`ACCECAMENTO_M`, `P_RIAVVIO_S`): con la testa in attivo, un'esca entro 1000 m e nel cono copre le
  eco vere. Ogni secondo, con probabilità 0,25:
  - la verifica di un bersaglio vero ricomincia da capo;
  - un siluro che insegue un bersaglio vero lo perde e passa alla riacquisizione.

  Vale anche per un'esca già rifiutata.

**Esempio.** Un Mk 48 in attivo verifica un'ADC Mk 2: la rifiuta 7 volte su 10. Se la incontra due volte, la rifiuta
almeno una volta 91 volte su 100. Lo stesso siluro in passivo si fa sedurre 9 volte su 10, a ogni verifica. Contro
una MG-74, anche in attivo, si fa sedurre 8 volte su 10.

### 2.5 L'esca contro il filo

Lungo il filo la testa riporta per ogni sorgente se è «quasi ferma» (D1 §2.9). Un operatore può rifiutarla con il
comando `rifiuta`.
- **IA.** Il sonarista medio rifiuta un'eco quasi ferma che sta a più di 15° dalla sua stima del bersaglio,
  l'esperto oltre 10°; la recluta non rifiuta (`[filo_ia.*]` di `interazioni.toml`, D1 §2.10). Le esche
  semoventi non sono «quasi ferme»: l'operatore dell'IA non le riconosce.
- **Giocatore.** Il comando esiste nel motore, ma la stazione Armi non ha un tasto per darlo (**in arrivo**,
  D1 §5).

### 2.6 Quando l'IA lancia le esche

Un sottomarino dell'IA lancia esche quando la manovra d'evasione riceve lo stimolo di un siluro
(`Missione::stimola_evasione`). Lo stimolo arriva:
- da un siluro ostile in acqua a meno di 3000 m, se il battello ne ha il contatto, o a meno di 1500 m comunque
  (`SILURO_SENTITO_M`, `SILURO_VICINO_M`; `Missione::passo_difesa`);
- dal transitorio di lancio di un sottomarino ostile entro 20 km, con probabilità 0,5 (`TRANSITORIO_LANCIO_M`,
  `P_OCCHIATA`);
- da un siluro aviolanciato del velivolo che lo caccia (capitolo sull'ASW aereo).

Che cosa lancia (`lancia_esca_ia`, STIME):
- un battello nucleare sovietico: una **MG-74** sulla rotta che sta lasciando, al più 2 per battello e una ogni 120 s
  (`ESCHE_MOBILI_IA`, `ESCA_MOBILE_OGNI_S`). I diesel no;
- ogni sottomarino: un'**esca ferma** alla sua quota, al più 4 per battello e una ogni 45 s (`ESCHE_IA`,
  `ESCA_OGNI_S`).

Le navi non hanno esche: si allontanano a tutta forza per 3-5 minuti da un siluro ostile di cui hanno il contatto
entro 4000 m (`SILURO_NAVE_M`, `EVITA_NAVE_MIN_S`, `EVITA_NAVE_MAX_S`).

### 2.7 La manovra che accompagna l'esca

Con lo stimolo del siluro l'IA sceglie la manovra (`Evasione::stimolo`, `nucleo/src/evasione.cpp`, STIME sulla
dottrina del dossier 17 §6.7 e §9):
- rotta: la poppa al siluro, con uno scarto a caso fino a 10°;
- velocità: la massima;
- quota: 50 m sotto il pavimento del siluro, se lo scafo ci arriva con 20 m di margine; altrimenti la quota massima.
  Il pavimento è la quota massima della classe del siluro (`passo_difesa`), non quello impostato: contro un Mk 48
  (760 m) nessun battello ci arriva, e la quota è sempre la massima;
- in acque basse (fondale fra 50 e 150 m) si posa sul fondo, due volte su tre un diesel, una su quattro un nucleare:
  una testa attiva lo vede male (D1 §2.5);
- dura 480 s, 1800 s se è sul fondo.

L'esca resta dove il siluro aspetta il battello, mentre il battello accosta e cambia quota: la «falsa scia» della
virata (knuckle) non ha un modello suo, la rappresenta l'esca.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| ADC Mk 2: ferma a quota fissa, elica intubata, batteria | dossier 05 §5.1 [GOVT]; dossier 14 §2.8 | alta |
| ADC Mk 2: 150 dB | dossier 05 §5.1 | **STIMA**: più rumorosa del battello |
| ADC Mk 2: 600 s di batteria | dossier 05 §5.1 [TWZ]: 6-45 minuti | **STIMA** |
| MG-74: tubo da 533, 3,9 m, fino a 250 m, imita rumore ed eco, manovra programmata | dossier 05 §5.2 [WARFOR] [KREM] | media |
| MG-74: 10 nodi, 20 minuti, eco 15 dB, zig-zag | dossier 05 §7 | **STIMA** |
| MOSS Mk 70: 10", eco e firma del battello, dal 1976, sugli SSBN | dossier 05 §5.1 [WPMOSS] | media |
| MOSS: velocità, quota, eco, manovra, durata | nessuna fonte | **STIMA** |
| Efficacia per tipo di testa | dossier 05 §5.3 (matrice qualitativa) | **STIMA** sui valori |
| Rifiuto per Doppler ed estensione; disturbo che riavvia la verifica | dossier 05 §5.6 [TWZ] [PAT5235924] | media sul principio, **STIMA** sui numeri |
| Esche dell'IA: quante e ogni quanto | dossier 05 §5.2 e §7 | **STIMA** |
| Manovra d'evasione | dossier 17 §6.7, §8.3 e §9; dossier 02 §6.4 | **STIMA** sulla dottrina |

Il dossier 05 §7.3 propone una probabilità di seduzione a ogni passo, con fattori per la geometria, la generazione del
siluro e il filo. Il codice usa un modello diverso: la scelta della sorgente più forte e il rifiuto alla fine della
verifica, con i numeri del §2.4. Le regole qualitative del §5.3 del dossier sono rispettate: l'esca ferma è forte
contro il passivo e media contro l'attivo, quella semovente forte contro tutti e due, nessuna contro il segui-scia.

## 4. Classico ed Enhanced

Nel classico (`docs/04_vm_missioni.md`, `docs/originale/prestazioni.md`):
- l'esca è un'unità dello script, silenziosa per 2 s, poi col rumore 40, più di qualunque nave;
- dopo 2 s esegue `RETARGET` (`devia_riferimenti`): le unità che avevano come bersaglio il battello passano
  all'esca;
- dura 160 s (60 in quattro scene); le armi non la colpiscono (falso bersaglio);
- non ci sono esche semoventi, né esche dell'IA, né manovre antisiluro: l'IA dell'originale vira di 180° e va a tutta
  forza per 30 tick.

Nell'Enhanced lo script dell'esca è lo stesso, ma decide la testa del siluro (§2.4): il `RETARGET` non tocca i siluri
del modello. Le esche semoventi, le esche dell'IA e la manovra d'evasione ci sono solo nell'Enhanced. La MOSS c'è solo
nel modo moderno; nel modo 1989 il 688 ha solo le ADC Mk 2 (roadmap, punto 25, decisione del 26/10/2026).

## 5. Come si vede in gioco

Volume 1, §6.5 (le esche) e §6.2 (i tubi).

- **Stazione Armi** (F7): i lanciatori d'esche sono tubi «ausiliari» (messaggio 224). Il clic sul tubo arma l'esca in
  20 s (messaggio 15 «Esca acustica pronta al lancio»); L la lancia (messaggio 43); allo spegnimento il 44.
- **Esche semoventi**: si caricano dalla rastrelliera come un siluro (Ctrl+V sceglie l'arma del tubo), si armano
  («esca pronta») e si lanciano con L. Partono con la rotta e la quota impostate per il tubo.
- **Sonar e cuffia**: l'esca semovente lanciata da un tubo fa i transitori del lancio come un siluro (capitolo C8).
- **Messaggi della testa** del proprio siluro: 264 aggancio (anche su un'esca), 265 contatto perso.
- **Registro e debriefing**: i lanci di esche del giocatore e dell'IA («lancia un'esca», «lancia un'esca
  semovente») e le esche sedotte (capitolo F1).

## 6. Taratura e prove

Taratura:
- **MOSS contro il Mk 46 di un aereo**: la probabilità di colpire del Mk 46 scende da 0,53 a 0,33 con l'esca
  (roadmap, punto 25, 26/10/2026).
- **Battello posato contro un siluro aviolanciato**: Pk ≤ 0,1 (dossier 17 §6.7), da cui la scelta di posarsi.

Prove:
- `test/prove_siluro.cpp`: «siluro: l'esca ADC Mk 2 seduce il passivo; l'attivo la rifiuta piu' spesso (Doppler zero,
  eco puntuale)»; «siluro: il noisemaker nel fascio della testa attiva riavvia la verifica e fa perdere l'aggancio»;
  «siluro: l'esca semovente (MG-74) inganna anche l'attivo, che smaschera l'ADC Mk 2 stazionaria»;
- `test/prove_missioni.cpp`: «esche semoventi: il tubo del giocatore lancia la MOSS con la rotta e la quota
  impostate»; «esche semoventi: l'Alfa del giocatore carica una MG-74 dalla rastrelliera e la lancia dal tubo»;
- `test/prove_asw_missione.cpp`: «asw missione: col Mk 46 in coda l'Alfa lancia un'MG-74 semovente sulla rotta che
  lascia»; «asw missione: l'Alfa dell'IA evade la caccia del P-3C, e senza evasione no»;
- `test/prove_difesa.cpp`: «difesa: il sottomarino dell'IA evade e lancia esche su un siluro qualsiasi, la nave si
  allontana»; «difesa: senza, nessuna evasione ne' esca fuori dalla caccia aerea»;
- `test/prove_asw_aereo.cpp`: «evasione dell'IA: tonfo, priorita' degli stimoli, durata e fondo in acque basse»;
- `test/prove_armi_ia.cpp`: «armi dell'IA: esche rifiutate dall'esperto, taglio per evadere, vincoli del
  lanciatore»;
- `test/prove_carico.cpp`: «carico: esche mobili da tubo, la MOSS solo nel modo moderno e la MG-74 sull'Alfa»;
- `test/prove_libreria_missioni.cpp`: «libreria: l'Alfa ha due esche semoventi MG-74 nel deposito, il 688 del 1989
  nessuna MOSS».

## 7. Limiti noti e in arrivo

- L'esca ferma del giocatore dura 160 s, quella dell'IA 600 s: la durata del giocatore resta quella dello script
  dell'originale, e i 600 s di `[esche.adc_mk2]` valgono solo per l'IA.
- L'esca ferma nasce alla quota del battello: non si imposta. La `quota_m` di 60 m di `[esche.adc_mk2]` la usa solo la
  classe `Esca` del nucleo, non la missione.
- **In arrivo:** l'ADC Mk 2 del giocatore coi dati di `siluri.toml`: 600 s e la quota impostata.
- Con l'acustica l'esca ferma vale 168 dB sulla scala della testa, senza 150 dB. Non è un difetto: sono due scale
  (§2.2), e in tutte e due l'esca supera il battello di 30 dB o più.
- Le probabilità di rifiuto non dipendono dalla classe del siluro, né dalla geometria fra siluro, esca e battello.
- Il giocatore non può rifiutare un'esca via filo: manca il tasto (**in arrivo**).
- Le esche semoventi non cambiano modo né manovra in base ai ping che ricevono, come faceva la MG-74 vera.
- **In arrivo** (roadmap, punto 25, «contromisure acustiche»): i generatori di bolle e le esche rimorchiate delle navi
  (Nixie, e gli equivalenti sovietici) contro i siluri del giocatore. Oggi il Nixie non c'è: le navi si difendono solo
  allontanandosi.

## 8. Riferimenti

- Codice: `nucleo/siluro.hpp` e `nucleo/src/siluro.cpp` (`DatiEsca`, `rotta_esca_mobile`, verifica e disturbo),
  `nucleo/evasione.hpp`, `nucleo/src/evasione.cpp`, `missioni/src/missione_esche.cpp`, `missioni/src/missione_asw.cpp`
  (`lancia_esca_ia`, `stimola_evasione`), `missioni/src/missione_difesa.cpp`, `missioni/src/missione.cpp`
  (`passo_siluri`, `sorgente_acustica`, `devia_riferimenti`), `nucleo/src/armi_ia.cpp` (`GuidaFiloIa`).
- Dati: `data/motore/siluri.toml [esche.*]`, `data/motore/acustica.toml [firme.adc_mk2]`,
  `data/motore/interazioni.toml [filo_ia.*]`, `data/motore/carico.toml` (esche nel carico).
- Ricerca: dossier 05 §5 e §7; dossier 14 §2.8 e §2.13; dossier 17 §6.7 e §9; dossier 02 §6.3-6.4.
- Progetto: `docs/00_ROADMAP.md` punto 25; `docs/13_asw_aereo.md` (evasione e taratura A6);
  `docs/18_interazioni_ia.md` (fase 4); `docs/04_vm_missioni.md` (esca dell'originale).
- Capitoli: D1 siluri, D5 carico e ricarica, C4 firme acustiche, C8 audio, F1 registro.
