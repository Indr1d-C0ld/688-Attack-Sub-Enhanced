# 27 — Missili del 688: impiego reale, dati di tiro e ricerca del missile (1985–1991)

> Dossier per la stazione di tiro dei missili dell'Enhanced (`docs/21_stazione_missili.md`, roadmap punto 25). Colma le
> lacune del `05_missili_e_contromisure.md` §1–§3 senza ripeterlo: lì ci sono già la sequenza di lancio dell'Harpoon,
> RBL e BOL in breve, il profilo, il TASM con OUTLAW SHARK e il bilancio dei 139 tiri, il TLAM con l'ora di preparazione
> e la tabella dei missili sovietici. L'aspetto delle consolle Mk 81 è nel `19_consolle_688_alfa.md`.
> Compilato il 28/10/2026 da trailer-1.
>
> I dettagli, con tutte le tabelle e le fonti, sono in tre appunti:
> - `27_note/tiro_e_ricerca.md`: sistemi di tiro, consolle, interblocchi, ricerca dell'Harpoon e del TASM;
> - `27_note/oth_e_tlam.md`: catena dei dati oltre l'orizzonte, invecchiamento del dato, missioni TLAM, chi decide;
> - `27_note/sovietici.md`: Granat, Vodopad, Veter, Vyuga, designazione esterna, proposte per l'IA.
>
> Fonti: pagine web aperte. In più, con l'autorizzazione dell'utente del 28/10/2026, ho letto quattro documenti scaricati
> in `tools/dl/27_missili/`: l'articolo APL di Hatch, Luber e Walker [APL92], la tesi NPS di Becker [BECK86], l'articolo APL
> di Shade e Lucas [APL81] e la sintesi pubblica del rapporto GAO del 1981 [GAO81]. Il manuale del simulatore DSTO
> (ADA327797) e la prova del seeker DSQ-28 (ADA045182) non si sono potuti scaricare: DTIC era in manutenzione.

---

## 0. Convenzioni

Come nel doc 05 §0:
- **Alta:** documento ufficiale o due fonti serie concordi.
- **Media:** una buona fonte secondaria.
- **Bassa:** fonte singola debole, forum, simulatore commerciale. *(estr.)* = letto solo nell'estratto del motore di
  ricerca, perché la pagina era bloccata.
- **STIMA DI GIOCO:** nessun dato pubblico; valore proposto, motivato, da trattare come parametro di bilanciamento.

### 0.1 Le dieci scoperte che cambiano il design

1. **Lo sparo passa per tre livelli** [PAT5551875] (alta, brevetto USN sugli scafi SSN-751):
   - la consolle di tiro (Mk 81 sul Mk 117), dove si impostano i dati;
   - la **Weapons Launch Console** in centrale, che manda l'ordine di fuoco, la tensione di sparo e la pressione
     differenziale;
   - il **Tube Control Panel** nel locale siluri, che manovra valvole e porte.

   **Una sola slide valve per gruppo di tubi può essere aperta alla volta**: è il motivo fisico dell'intervallo fra i
   lanci di una salva.
2. **Il menu dell'Harpoon è documentato:** RBL con la ricerca **piccola, media o grande**; BOL con l'opzione di ricerca
   fino alla portata massima; LOS senza dati [PAT6763289] (alta). La pianificazione prima del lancio sceglie punto di
   mira, ricerca, attacco terminale ed eventuali waypoint [APL92] (alta; i waypoint però non sul 688 del 1989).
3. **Il seeker prende il primo eco valido.** Il DSQ-28 è un radar a ricerca sequenziale senza classificazione; il
   missile attacca il primo bersaglio trovato oltre la distanza d'accensione [NAVS] *(estr.)* [PAT6763289] [SBIR10]
   *(estr.)* (media). Becker modella l'acquisizione esaminando i bersagli **dal più vicino al più lontano** dentro il
   cancello [BECK86] (media). È questo che fa colpire la scorta o il mercantile.
4. **L'area di ricerca è un rettangolo** (cancello in distanza × fascia del seeker), e il cancello ottimo dipende
   dall'incertezza sul bersaglio. Con 3 miglia di errore e 8 scorte entro 12 miglia, il cancello migliore va da circa
   −5 a +5 miglia attorno al punto atteso, con una fascia di 6 miglia [BECK86] (media; modello generico del 1986).
5. **Nel 1989 un 688 non sceglieva il terminale.** La scelta del Block 1C arriva sui sottomarini col CCS Mk 2 degli anni
   '90 [FAS98RDT]: nel 1989 l'UGM-84A fa il pop-up, l'UGM-84C è radente (deduzione, bassa-media).
6. **Il sistema di tiro del sottomarino calcola la probabilità di successo del TASM.** Il CCS «fornisce un quadro
   correlato oltre l'orizzonte, pianifica l'ingaggio scegliendo gli schemi di ricerca appropriati e calcola la
   probabilità di successo» [APL92] (alta). Un manuale APL d'impiego antinave descriveva i problemi del puntamento in
   termini di precisione e tempestività dei rapporti e di navigazione del missile, e gli schemi di ricerca [APL92].
7. **Il dato oltre l'orizzonte arriva lento.** Catena: sensori → OSIS → Shore Targeting Terminal del comando
   sommergibili, che adatta i dati al singolo battello → SSIXS. Nell'epoca di OUTLAW SHARK servivano **20 minuti o più**,
   con un aggiornamento per contatto **una o due volte l'ora** [NSLBA] (media). Senza designazione esterna il TASM era
   utile solo **sotto le 50 miglia** [NSLC2T] (media). Nel 1978 la capacità OTH era «seriamente limitata», soprattutto
   nell'identificazione delle navi [GAO81] (alta).
8. **TLAM: il battello pianifica solo la tratta sul mare.** Il sistema di tiro calcola la traiettoria fino al primo
   waypoint pianificato a terra, la unisce alla missione terrestre del disco e la carica nel missile; la pianificazione
   a terra fornisce le previsioni (precisione di navigazione, probabilità di penetrazione e d'arrivo) [APL92] (alta).
   Nel 1989 non c'è pianificazione imbarcata (arriva nel 1996) [FASTMPC] (alta).
9. **L'errore di posizione del battello passa uno a uno al missile.** La tratta sul mare è inerziale e il TERCOM non
   funziona sull'acqua [DS109]: la prima mappa TERCOM deve contenere l'errore del battello. Il primo fix sulla prima
   mappa del primo volo di prova fallì [APL92] (alta, aneddoto).
10. **I sovietici:** il Granat (SS-N-21) è **solo strategico**, contro terra, senza versione antinave; Vodopad e Veter
    sono armi **a punto fisso**, col punto d'ammaraggio calcolato prima del lancio dai dati del sonar; gli SSN sovietici
    **non avevano ricevitori per Legenda o Uspekh** e ricevevano la designazione come radiogramma [note sovietici §0]
    (media-alta).

---

## 1. Sistemi di tiro e consolle

| Scafo (1989) | Sistema | Che cosa conta per i missili | Fonte | Conf. |
|---|---|---|---|---|
| 688 Flight I | **Mk 117** su UYK-7, consolle Mk 81 («weapon control consoles») | integra l'Harpoon; il Tomahawk entrò **nel Mk 117 con una consolle operatore in più**, con un processore per lo schermo e il puntamento OTH, all'inizio su cinque battelli | [WP688] [DS84] [FOX01] *(estr.)* | media-alta |
| 688 Flight II (VLS) | **CCS Mk 1**, «All Digital Attack Center», UYK-7 e UYK-44, software C4.x | pianificazione, inizializzazione e lancio del Tomahawk anche dai 12 pozzi | [WP688] [NSLCCS] | media-alta |
| 688I | **AN/BSY-1** | sonar e combattimento integrati; conserva il software del CCS Mk 1 | [FASBSY1] | media-alta |
| Equivalente del TWCS navale | **non c'è**: sui sottomarini fa tutto il Combat Control System | — | [FASTom] | alta |

**Due responsabilità distinte** per il Tomahawk [NSL85] (media): il **controllo del tiro** (lancio e volo), di cui
risponde il comandante, e il **controllo dell'impiego** (chi sceglie il bersaglio), che sta a terra.

**Che cosa vede l'operatore dell'Harpoon** (dati d'inizializzazione, lato aereo e navale; il menu del 688 non è
pubblico): modo, distanza e rilevamento, schema di ricerca, opzione d'attacco, accensione del seeker; nel BOL l'istante
d'inizio della ricerca [PAT6763289] [DSTO97] *(estr.)* [USNI77] *(estr.)*. Un simulatore commerciale dell'AGM-84D ha i
campi MODE, TERM, SEEK (SML/MED/LRG), SRCH, DSTR, BRG e gli avvisi «fuori zona» [VRS] (bassa-media, analogia).

**Non si trova:** la pagina-schermo della Mk 81 per i missili, la disposizione della WLC, i nomi delle lampade. La
stazione del gioco è una ricostruzione dichiarata.

## 2. Sequenza, tempi, interblocchi

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Interblocchi del tubo | la porta esterna non si apre senza la culatta bloccata; si equalizza prima d'aprire; dopo lo sparo la porta resta bloccata finché il pistone non ha finito la corsa | [PAT5551875] | alta |
| Slide valve | una sola aperta per gruppo di tubi | [PAT5551875] | alta |
| Capsula Harpoon | lancio possibile da quota periscopica; la quota non è limitata dal missile; uscita dall'acqua a ≥ 45° | [PAT4033225] | alta |
| Velocità di lancio | tattica corrente per i siluri: 5–10 kn | [USNI97] *(estr.)* | media |
| Riscaldamento del seeker | 25 s (AGM-84D, analogia) | [VRS] | bassa-media |
| Tomahawk a bordo | preparazione del missile 10–20 min (1987); circa un'ora per la piattaforma | [NSLGRU] [GAO95] | media / alta |
| Volo TLAM a 1.000 miglia | 2,5–3 ore | [NSLGRU] | media |

**STIME DI GIOCO** (motivate negli appunti):

| Fase | Harpoon | TASM | TLAM |
|---|---|---|---|
| Accensione e riscaldamento | 30 s | parte dell'allineamento | parte della preparazione |
| Allineamento e dati | 20–40 s, in parallelo ai dati di tiro (30–60 s) | 8–12 min, più 5–15 min per la missione di ricerca | primo missile 15 min, poi 3–5 min ciascuno |
| Totale con la soluzione pronta | **60–90 s** | **15–25 min** | **circa 1 h** con l'ordine da decodificare |
| Fra due tubi della salva | 3–5 s di lati opposti, 15–25 s dello stesso lato (slide valve) | idem | lanci a coppie a ~15 s (nave, 2003) |

## 3. Il missile in ricerca

### 3.1 Harpoon

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Profilo | rotta preprogrammata a bassissima quota con riferimento d'assetto e radioaltimetro; vicino al bersaglio il seeker si accende e cerca; trovato il bersaglio, aggancia e guida | [APL92] | alta |
| RBL | vola al rilevamento comandato fino alla distanza comandata, poi esegue la ricerca e l'attacco comandati | [PAT6763289] | alta |
| Inizio ricerca RBL | SML 10 km, MED 20 km, LRG 30 km prima del punto previsto | [VRS] | bassa-media |
| BOL | settore ±45°; ampiezza in azimut e profondità in distanza **crescono gradualmente** dall'istante impostato; senza bersaglio, autodistruzione | [DS84] [USNI77] *(estr.)* | media |
| Seeker | DSQ-28, radar attivo monopulse agile in frequenza, banda J, ricerca sequenziale; efficace fino a mare forza 5 | [USNI77] *(estr.)* [DS84] [SBIR10] *(estr.)* [WTF] | alta / bassa-media |
| Portata d'acquisizione | **non pubblicata**; limitata dall'orizzonte radar | — | — |
| Regola d'aggancio | il primo eco valido nel cancello, dal più vicino | vedi §0.1 punto 3 | media |
| Variante a quadrifoglio | Block 1D, anni '90: **non** nel 1989 | [DS84] | media |

**Becker, il modello del cancello** [BECK86]:
- il missile non riceve nulla finché non arriva nell'area di ricerca;
- l'area esplorata è un rettangolo: il cancello impostato dall'operatore per la fascia del seeker; navi grandi hanno
  una fascia più larga (una portaerei si vede da più lontano di una scorta);
- i bersagli rilevati si esaminano dal più vicino al più lontano, ciascuno con la sua probabilità d'acquisizione
  (nell'esempio 0,97 per il bersaglio di valore e 0,4 per una scorta, cioè un missile «indulgente» che scarta un po'
  di scorte);
- ogni missile della salva ha lo stesso rilevamento e lo stesso programma di ricerca ma decide da solo;
- con più missili il cancello ottimo è un po' più lungo (circa 1,7 miglia con 4 missili invece di 1).

**STIMA DI GIOCO: la scatola.** Rettangolo centrato sul punto previsto, orientato lungo la rotta. Le tre taglie si legano
all'incertezza della soluzione, come in Becker: la profondità vale circa ±1,7 σ della distanza (piccola), ±2,5 σ (media),
±3,5 σ (grande), con un minimo di 5, 10 e 15 km; la larghezza è la fascia del seeker contro la classe più grande del
gruppo (STIMA: 10–12 km contro un caccia, 15–20 contro un incrociatore o una portaerei). Gli appunti propongono anche
valori fissi (±3, ±6, ±10 km di semilarghezza) se si preferisce non legarli alla TMA.

### 3.2 TASM

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Profilo | crociera bassa; a una distanza programmata sale a quota di ricerca e cerca col radar attivo | [LOC92] *(estr.)* | media |
| Ricerca | serpentina con ESM passivo e radar attivo; schemi a più passate studiati da APL dal 1974-75 | [DS109] [LOC92] *(estr.)* [NSLOS] | media |
| ESM | regolato sulle emissioni caratteristiche del bersaglio voluto | [NG2] | media |
| Pianificazione | la piattaforma sceglie gli schemi di ricerca e calcola la probabilità di successo | [APL92] | alta |
| Portata | 250 miglia operative | [GAO81] [NG2] | alta |
| Bilancio dei 139 tiri simulati | 17 sul designato, 53 su altri nemici, 8 su neutrali, 61 senza esito attribuito | [NG2] | media |

**STIMA DI GIOCO:** salita a ~100 m a 40 km dal punto previsto; gambe trasversali alla linea di tiro lunghe 40 km e
spaziate 25 km; se l'ESM capta un radar della classe programmata, vira sul suo rilevamento e aggancia l'eco più
vicino entro ±5°; senza emissioni prende il primo eco grande, mercantili compresi.

## 4. Oltre l'orizzonte e TLAM

### 4.1 Il dato esterno e il suo invecchiamento

- **Formato.** Lo standard è l'OTH-T Gold (OTG): messaggi di testo a set fissi (`MSGID`, `CTC`, `POS`, `RMKS`,
  `ENDAT`), campi separati da `/`, righe di 69 caratteri, **ora della posizione distinta dall'ora del messaggio**
  [NISPOTG] [CATOTHG] *(estr.)* (media). La codifica dell'ellisse non è pubblica: il messaggio del gioco è una
  ricostruzione (appunti OTH §2.2).
- **Ingresso a bordo.** All'inizio a mano, poi diretto in Mk 117 e CCS Mk 1; il calcolatore correla il dato con i
  contatti propri e prepara gli schemi di ricerca (doc 05 §1.4; [NSLOS]).
- **Mira al punto futuro.** Si mira alla posizione stimata all'arrivo, perché l'errore resti nella spazzata del seeker
  [NSLC2T] (media).
- **Invecchiamento (STIMA, appunti OTH §3.2):** età = ritardo della catena + tempo a bordo + volo.
  - Con rotta e velocità note l'ellisse cresce lenta (esempio: Kirov a 70 minuti, circa 200 mi²).
  - Senza rotta il raggio cresce come Vmax·t (stesso esempio: circa 4.800 mi²); il TASM ne spazza circa 3.500.
  - Regola: **un dato con rotta e velocità vale più di un dato fresco senza**.
- **Navigazione.** Il dato OTH è in coordinate geografiche: l'errore della posizione propria si somma all'ellisse.
  Contro un bersaglio del proprio sonar no (geometria relativa).

### 4.2 TLAM

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Pianificazione | CMSA, 24–80 ore, rotte terrestri «settimane o mesi prima» | [FASCMSA] [NSLGRU] | alta |
| Arrivo a bordo | nastri o dischi caricati poco prima del lancio, o radio; esiste una libreria di missioni | [NSLGRU] [FASCMSA] [USNI90] *(estr.)* | alta / bassa |
| A bordo | il sistema di tiro pianifica la tratta sul mare fino al primo waypoint e la unisce alla missione terrestre | [APL92] [NSLGRU] | alta |
| Area di lancio | punto di partenza fisso; dimensioni **non pubbliche** | [FASCMSA] | alta / — |
| Chi decide | il comandante sceglie percorso, momento nella finestra, rinvio, e dà il fuoco; non sceglie bersaglio, missione o numero di missili | [BH03] [FASCMSA] | media |
| TLAM-N | nessun PAL nella Marina; rilascio con EAM e confronto del SAS a due persone; «procedure simili» per il TLAM-N | [BELL] [USNI88] *(estr.)* [NSLCT] | alta / media |

**STIMA DI GIOCO** (appunti OTH §6.2):
- **Errore al landfall:** σ_LF ≈ √(σ_nave² + deriva del missile² + errore di rotta²); domina l'errore del battello.
- **Presa della prima mappa TERCOM:** P = 1 − exp(−H²/2σ_LF²), con H = 2,5 miglia (σ 1 mi → 0,96; 2 mi → 0,54; 3 mi
  → 0,29).
- **Stato della missione:** VALIDA entro 10 mi dal punto di lancio pianificato; RIPIANIFICA fra 10 e 40 (+10 min, meno
  carburante); NON VALIDA oltre; FUORI FINESTRA a ±10 min dall'ora pianificata.
- **Il pannello calcola sulla posizione stimata, l'esito usa quella vera.**

## 5. I sovietici, per l'IA e la simmetria

| Arma | Chi la porta (1989) | Come designa | Note per il gioco | Conf. |
|---|---|---|---|---|
| **S-10 Granat** (SS-N-21) | 671RTMK (dal 1987), 971; 945 incerto | missione preparata a terra da un centro di calcolo, con carte digitali dai satelliti; a bordo si conserva, si carica e si lancia; lancio da **40 m** | solo strategico: per il gioco una minaccia di scenario (SSN in posizione verso la costa), mai contro navi o contro il 688 | alta (ruolo e quota) |
| **83R Vodopad** (SS-N-16) | 671RTM, 971, 949A | punto d'ammaraggio calcolato dalla BIUS Omnibus sui dati del sonar Skat e inserito prima del lancio; gittata regolata dalla durata del tratto subacqueo; nessuna correzione | fino a 50 km; UMGT-1 41 kn, 8 km, gira a sinistra finché aggancia | media |
| **86R Veter** | 971, 949A, 671RTM (tubi da 650) | come il Vodopad; una fonte dice che accetta dati ritrasmessi da navi, aerei e satelliti | circa 100 km; profondità di lancio discordante (fino a 100 m o il doppio del Vodopad) | media / bassa |
| **81R Vyuga** (SS-N-15) | Pr.705 Alfa e altri | solo dati del sonar; l'Akkord li inserisce per via elettrica; si imposta la gittata | **solo nucleare**, 10–40 km, da ~50 m | alta |
| Granit (Oscar II), dettaglio nuovo | 949A | lancio fino a 50 m e 5 kn, **5 s fra i missili**, 24 missili in 3 salve, canale manuale | colma la lacuna del doc 05 §3.3 | media |

**Designazione esterna.** Il «Korall» (ricevitore di Legenda e Uspekh) stava su 949/949A e 1144, la «Kasatka-B» sul 675.
Victor III e Akula avevano la boa VLF «Paravan» e i complessi «Molnija» e «Cunami»: ricevevano la designazione come
radiogramma, in ritardo (deduzione, media).

**Per l'IA (STIME, appunti sovietici §5):** spara solo con la sua soluzione, mai con la verità; l'83R vuole un errore di
distanza ≤ 1,5 km (≤ 3 km con due missili); il Veter può usare dati esterni vecchi al più 10 minuti; dispersione
inerziale dell'1% della distanza; guasti del 5–10%; un ping dà la distanza al 2–3% ma rivela il battello. Il giocatore
vede la minaccia nei cambi di rotta del contatto (la sua TMA), nel ping o nel transitorio del tubo, e la batte
cambiando rotta e velocità: la stessa cosa che fa l'IA contro i Mk 48.

## 6. Giocabile ma storico

1. **La scelta vera dell'operatore** è fra una scatola stretta (protegge i neutrali e le scorte, ma rischia il nulla)
   e una larga (trova sempre qualcosa, non sempre la nave giusta). Il gioco la mostra sulla carta e non la nasconde.
2. **La probabilità di successo sul pannello è storica** [APL92]: il gioco la calcola sui dati del battello (stima),
   l'esito sulla verità.
3. **Il primo eco** è la regola del seeker, per il giocatore e per l'IA: la nave sbagliata, anche neutrale, è un esito
   normale, e il debriefing la racconta.
4. **La lentezza del dato OTH e l'ora del TLAM** restano; la compressione del tempo le attraversa.
5. **Ricostruzioni dichiarate:** la pagina della consolle, le lampade e le misure della scatola sono ricostruzioni; il
   museo e il manuale lo dicono.

## 7. Fonti nuove di questo dossier

Le altre sigle sono negli appunti (`27_note/*.md`, sezioni «Fonti») e nel doc 05 §8.

| Sigla | Titolo | Dove | Note |
|---|---|---|---|
| [APL92] | R. R. Hatch, J. L. Luber, J. H. Walker, «Fifty Years of Strike Warfare Research at the Applied Physics Laboratory», *Johns Hopkins APL Technical Digest* 13(1), 1992 | https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V13-N01/13-01-Hatch.pdf (copia in `tools/dl/27_missili/apl_hatch/`) | letto; fonte di prima mano |
| [BECK86] | K. J. Becker, *An analytical high value target acquisition model*, tesi NPS, marzo 1986 | https://hdl.handle.net/10945/21667 (copia in `tools/dl/27_missili/becker_nps/`) | letto; modello generico di missile antinave |
| [APL81] | K. E. Shade, M. C. Lucas, «The MK 92 Fire Control System», *Johns Hopkins APL Technical Digest* 2(2), 1981 | https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V02-N02/02-02-Shade.pdf (copia in `tools/dl/27_missili/apl_shade/`) | letto; è la direzione del tiro antiaerea delle Perry: serve alle difese delle navi, non alla stazione |
| [GAO81] | GAO C-MASAD-81-11, *Issues Affecting the Navy's Antiship Cruise Missile Programs*, 28/2/1981, sintesi pubblica di 4 pagine | https://www.gao.gov/products/c-masad-81-11 | letta nel browser (il testo completo è classificato); Harpoon circa 60 miglia, TASM 250 miglia operative, OTH «seriamente limitato», tattiche di salva in sviluppo, carenze logistiche dell'Harpoon |

**Non disponibili il 28/10/2026:** DTIC ADA327797 (manuale del simulatore Harpoon DSTO) e ADA045182 (prova del container
del DSQ-28): DTIC era in manutenzione. Restano utili per le misure della scatola RBL e per il seeker.

## 8. Lacune

- La pagina della Mk 81 per i missili, la disposizione della WLC e le sue lampade.
- Le misure vere della scatola RBL e del cancello BOL; la portata d'acquisizione del DSQ-28.
- I tempi di riscaldamento e allineamento di UGM-84 e TASM; quota e velocità massime di lancio.
- Chiavi o consensi di sparo per le armi tattiche del 688.
- La geometria esatta della serpentina del TASM e la larghezza della sua fascia.
- La codifica dell'ellisse nell'OTG del 1989 e la latenza STT → SSN nel 1989.
- Le dimensioni dell'area di lancio del TLAM, la larghezza della prima mappa TERCOM, la deriva dell'inerziale del missile.
- Per i sovietici: tempi, velocità, distanza minima e salve di Vodopad e Veter; quanti Granat a bordo; l'interfaccia
  di Omnibus con le armi.
