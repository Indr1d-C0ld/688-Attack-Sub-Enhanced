# API m688 usata dalle missioni convertite

Generato da `tools/re/missioni_lua.py`. Livello classico: ogni funzione
corrisponde a un'operazione della VM originale (`docs/04_vm_missioni.md` §3). Il primo argomento è sempre `u`, l'unità
che esegue lo script. Le condizioni restituiscono vero o falso.

| Funzione | Opcode | Descrizione originale | Usi |
|---|---|---|---|
| `m688.accosta_opposto` | `TURN_AWAY` (71h) | rotta opposta al rilevamento del bersaglio (+180 gradi) | 12 |
| `m688.accosta_verso_bersaglio` | `TURN_TO_TGT` (61h) | rotta verso il rilevamento del bersaglio | 94 |
| `m688.annulla_timer` | `CANCEL` (2Bh) | annulla i timer dell'unita' corrente diretti all'etichetta | 401 |
| `m688.annulla_tutti_i_timer` | `CANCEL_ALL` (2Ch) | annulla tutti i timer dell'unita' corrente | 7 |
| `m688.antenna` | `F54_4` (5Ch) | flag +54, bit 2 (04h): antenna alzata (comandi 3Eh/3Fh) | 22 |
| `m688.attacca` | `ATTACK` (54h) | attacco al bersaglio: la portata e' ignorata; riesce se il contatore DS:5100 (18..1) supera il byte (0 = +3D), cioe' con probabilita' (18 - b)/18; danno fisso 30 | 17 |
| `m688.attacco_casuale` | `ATTACK_RND` (6Ah) | colpo di missile sull'unita' corrente (DS:475A): riesce se un byte casuale e' >= +3D, messaggio 50, danno = byte | 15 |
| `m688.azzera_rateo_quota` | `CLR_2C` (70h) | campo +2C = 0 (arresta la variazione di quota) | 13 |
| `m688.bersaglio` | `TARGET_ID` (36h) | seleziona come bersaglio il contatto con l'id indicato (+58) | 22 |
| `m688.bersaglio_arma` | `IF_TGT_F20` (3Eh) | salta se l'unita' bersaglio ha il flag 0x20 | 25 |
| `m688.bersaglio_id` | `IF_TGT_ID` (4Bh) | salta se l'id del bersaglio = valore | 59 |
| `m688.bersaglio_mi_punta` | `IF_TGT_LOCKED` (56h) | salta se il bersaglio punta l'ultima unita' trovata | 28 |
| `m688.bersaglio_nave` | `IF_TGT_F02` (40h) | salta se l'unita' bersaglio ha il flag 0x02 | 16 |
| `m688.bersaglio_nemico` | `IF_TGT_ENEMY` (3Fh) | salta se il bersaglio e' dello schieramento opposto (flag 0x18) | 2 |
| `m688.bersaglio_vicino` | `IF_TGT_NEAR` (53h) | salta se il bersaglio e' entro la distanza indicata in X e in Y (50 unita' = 1 nm) | 131 |
| `m688.caso` | `IF_RND` (44h) | salta se valore >= numero casuale (DS:CFD9): probabilita' | 186 |
| `m688.collega` | `LINK` (33h) | collega l'unita' a quella selezionata (+0C, +62) o scollega | 146 |
| `m688.comportamento` | `BEHAVIOR` (1Eh) | script del bersaglio dell'unita' (+10): eseguito una volta quando un contatto diventa bersaglio (+58), poi azzerato | 267 |
| `m688.devia_riferimenti` | `RETARGET` (5Bh) | trasferisce i riferimenti dall'unita' +62 all'ultima trovata | 22 |
| `m688.dopo` | `AT` (14h) | programma l'esecuzione dell'etichetta dopo/al tempo indicato (timer) | 915 |
| `m688.ferma_navigazione` | `STOP_NAV` (27h) | annulla la navigazione automatica (flag +4E, virata, +5C) | 102 |
| `m688.flag_scena` | `UFLAGS` (46h) | flag di scenario dell'unita' (+5F) = valore | 107 |
| `m688.flag_scena_accendi` | `UFLAG_SET` (6Eh) | +5F OR= maschera | 33 |
| `m688.flag_scena_bit` | `IF_UFLAG_BIT` (6Dh) | salta se +5F AND maschera != 0 (etichetta prima della maschera) | 37 |
| `m688.flag_scena_meno` | `UFLAGS_DEC` (5Fh) | +5F -= 1 | 1 |
| `m688.flag_scena_piu` | `UFLAGS_INC` (49h) | +5F += 1 | 16 |
| `m688.flag_scena_spegni` | `UFLAG_CLR` (6Fh) | +5F AND= NOT maschera | 14 |
| `m688.flag_scena_uguale` | `IF_UFLAGS` (47h) | salta se +5F = valore | 56 |
| `m688.giocatore_secondo` | `IF_PLAYER1` (68h) | salta se il giocatore e' l'unita' 1 (DS:0364 = 1) | 37 |
| `m688.impulso_sonar` | `PING` (2Ah) | impulso del sonar attivo dell'unita' corrente: evento acustico per 300 passi (33 s) nella lista DS:4772; gli altri giocatori ricevono il messaggio 8; se l'unita' e' la 0 incrementa la variabile di scena 0 | 138 |
| `m688.in_rettangolo` | `IF_IN_RECT` (5Dh) | salta se X >= x0, Y <= y0, X <= x1, Y >= y1 | 32 |
| `m688.ingaggia` | `ENGAGE` (3Bh) | aggancia il bersaglio corrente (+60) e ne riarma lo scatto dopo n aggiornamenti del contatto (byte); script del bersaglio (+10) e del bersaglio perso (+56) | 111 |
| `m688.insegui` | `PURSUE` (1Ch) | insegue il bersaglio +58 (sul rilevamento finche' la traccia e' sotto 800, poi sulla posizione stimata); soglia d'arrivo +50 in quarti di unita'; script all'arrivo (+0E) e di reazione (+56) | 22 |
| `m688.lancia` | `LAUNCH` (2Dh) | prepara nel modello corrente l'arma che NEW_UNIT creera' (non crea l'unita'): soglia d'arrivo 0, script all'arrivo (+0E) e di reazione (+56), inseguimento del bersaglio memorizzato (DS:4762) con il contatto del lanciatore copiato a traccia solida (qualita' 1200) | 82 |
| `m688.lancio_missile` | `MISSILE` (5Ah) | lancio di missile dell'unita' corrente: evento acustico per 900 passi (100 s) nella lista DS:4772, messaggio 0x30 agli altri giocatori | 27 |
| `m688.manomesso` | `IF_TAMPERED` (75h) | salta se la memoria DOS non corrisponde ai valori attesi (anti-manomissione) | 16 |
| `m688.marca_contatti` | `CONTACTS_MARK` (57h) | marca tutti i contatti dell'unita' (+16 = 1) | 101 |
| `m688.memorizza_bersaglio` | `SAVE_TARGET` (34h) | memorizza il bersaglio corrente in DS:4762 | 81 |
| `m688.messaggio` | `MSG` (55h) | accoda un messaggio di bordo (coda DS:6045); codici >= 100 = esito di fine missione | 243 |
| `m688.messaggio_al_giocatore` | `MSG_IF_PLAYER` (23h) | accoda il messaggio di bordo se l'unita' e' il giocatore o lo segue | 463 |
| `m688.modello_3c` | `P_3C2` (29h) | campo byte +3C | 1 |
| `m688.modello_4c` | `P_4C` (13h) | periodo +4C dei controlli di quota, velocita' e fondale, in visite dell'unita' (byte copiato in periodo e conto) | 82 |
| `m688.modello_accelerazione` | `P_ACCEL` (0Ah) | rateo di accelerazione (+40) | 49 |
| `m688.modello_avanti_tutta` | `P_VMAX` (0Bh) | velocita' 'avanti tutta' in 1/350 di nodo (+42); sforzo massimo = x 280/256 | 82 |
| `m688.modello_azzera` | `CLEAR` (11h) | azzera lo stato dinamico del modello (contatti, ordini, flag) | 86 |
| `m688.modello_classe` | `UNIT_TYPE` (28h) | tipo d'unita' (+4F): indice della tabella dei nomi DS:019A | 68 |
| `m688.modello_difesa` | `P_SKILL` (0Fh) | soglia di probabilita' +3D (confrontata con il numero casuale) | 54 |
| `m688.modello_flag` | `FLAGS` (05h) | flag dell'unita' (+06): schieramento e proprieta' | 82 |
| `m688.modello_integrita` | `HEALTH` (38h) | integrita' dell'unita' (+07) | 101 |
| `m688.modello_quota_max` | `DEPTH_MAX` (35h) | quota massima in 1/8 di piede (+5A) | 48 |
| `m688.modello_rateo_quota` | `P_DIVE` (08h) | parametro di variazione di quota (+38) | 49 |
| `m688.modello_rumore` | `P_3A` (0Ch) | parametro byte +3A (sensori/abilita') | 120 |
| `m688.modello_sonar` | `P_3B` (0Dh) | parametro byte +3B (sensori/abilita') | 89 |
| `m688.modello_tipo` | `TYPE` (04h) | tipo/classe dell'unita' (+04, byte duplicato) | 85 |
| `m688.modello_virata` | `P_TURN` (09h) | rateo di virata standard (+3E) | 49 |
| `m688.navigazione` | `NAVFLAG` (39h) | flag di navigazione +4E (0 = spegne il bit 0) | 82 |
| `m688.nuova_unita` | `NEW_UNIT` (10h) | crea un'unita' dal modello corrente con un nuovo id (DS:5103) | 96 |
| `m688.nuovo_caso` | `RANDOM` (60h) | nuovo numero casuale in DS:CFD9 | 33 |
| `m688.periscopio` | `F54_2` (2Fh) | flag +54, bit 1 (02h): periscopio alzato (comandi 32h/33h) | 30 |
| `m688.posizione` | `POS` (02h) | posizione X, Y dell'unita' (parte frazionaria azzerata) | 189 |
| `m688.quota` | `DEPTH_TO` (22h) | quota ordinata in 1/8 di piede (limitata da +5A) e rateo in 1/256 di +38 | 143 |
| `m688.quota_ora` | `DEPTH_NOW` (03h) | quota attuale e ordinata in 1/8 di piede (+1E, +30), variazione azzerata | 14 |
| `m688.rateo_virata` | `TURN_RATE` (20h) | rateo di virata corrente (+36); la virata si ferma sulla rotta ordinata +34 (libera con HEAD_TO -1). Nell'originale positivo = sinistra; in Lua gradi/s con il positivo a dritta | 111 |
| `m688.riprendi_navigazione` | `NAV_ON` (62h) | attiva la navigazione verso il punto (flag +4E = 4) | 37 |
| `m688.rotta` | `HEAD_TO` (24h) | rotta ordinata (+34); -1 = rotta libera (la virata non si ferma). In Lua gradi bussola, nil = libera | 67 |
| `m688.rotta_ora` | `HEAD_NOW` (06h) | rotta attuale e ordinata = valore (+20, +34), virata azzerata. Angolo matematico (0 = est, antiorario); in Lua gradi bussola | 163 |
| `m688.rumore_transitorio` | `TRANSIENT` (73h) | rumore transitorio dell'unita' corrente (lancio di siluro, manovra): evento acustico per 100 passi (11 s) nella lista DS:4772 | 42 |
| `m688.salva_modello` | `STORE` (15h) | salva il modello corrente nel record dell'unita' selezionata | 1241 |
| `m688.schiera_trainata` | `F54_1` (30h) | flag +54, bit 0 (01h): schiera trainata filata, ascolto su +/-90 gradi; +3B += 2 (con 0: -= 2 sempre) | 83 |
| `m688.seleziona` | `SELECT_ID` (74h) | unita' corrente per i comandi successivi = id (DS:5CBD) | 57 |
| `m688.seleziona_bersaglio` | `SELECT_TGT` (59h) | seleziona come corrente (DS:475A) l'unita' bersaglio (se non esiste termina) | 69 |
| `m688.seleziona_collegata` | `SELECT_LINKED` (6Bh) | come `seleziona_bersaglio`, ma prima scrive l'unita' corrente nel +62 del bersaglio (2011:1DCD) | 18 |
| `m688.telegrafo` | `TELEGRAPH` (21h) | velocita' ordinata in 1/256 della massima (999 = arresto rapido, >256 = accelerazione doppia) | 362 |
| `m688.teletrasporta_al_bersaglio` | `JUMP_TO_TGT` (72h) | teletrasporta l'unita' vicino al bersaglio (+45, +45) | 16 |
| `m688.togli_unita` | `SELECT_LAST` (37h) | toglie dal gioco l'unita' corrente DS:475A (chi esegue lo script, salvo selezioni successive): fine corsa, esca esaurita, carica esplosa (2011:19CB -> 003C; il nome storico e' sbagliato) | 69 |
| `m688.traccia_lunga` | `IF_TRACK_LONG` (3Ch) | salta se il contatto bersaglio e' tracciato da molto (+14 >= 1400) | 1 |
| `m688.traccia_solida` | `IF_TRACK_GOOD` (3Dh) | salta se la traccia del contatto bersaglio e' solida (+0E >= 800 su 1200) | 108 |
| `m688.uguaglia_quota` | `MATCH_DEPTH` (66h) | quota ordinata = quota del bersaglio (limitata) | 16 |
| `m688.uguaglia_velocita` | `MATCH_SPEED` (67h) | velocita' ordinata = velocita' del bersaglio | 15 |
| `m688.unita_colpita` | `IF_HIT` (52h) | salta se l'unita' con l'id indicato e' colpita o distrutta (+5E, +55) | 3 |
| `m688.unita_esiste` | `IF_EXISTS` (48h) | salta se esiste un'unita' con l'id indicato | 115 |
| `m688.unita_id` | `IF_ID` (45h) | salta se l'id dell'unita' corrente = valore | 32 |
| `m688.vai_a` | `GOTO_XY` (12h) | naviga verso il punto X, Y; soglia d'arrivo +50 in quarti di unita' per asse; script all'arrivo (+0E) | 331 |
| `m688.vai_al_bersaglio` | `GOTO_TGT` (64h) | naviga verso la posizione del bersaglio; soglia d'arrivo +50 in quarti di unita' per asse, script all'arrivo (+0E) | 2 |
| `m688.variabile` | `VAR_SET` (4Ch) | variabile di scenario = valore | 75 |
| `m688.variabile_maggiore` | `IF_VAR_GT` (4Eh) | salta se variabile > valore | 2 |
| `m688.variabile_meno` | `VAR_DEC` (51h) | variabile di scenario -= 1 | 26 |
| `m688.variabile_piu` | `VAR_INC` (4Dh) | variabile di scenario += 1 | 111 |
| `m688.variabile_uguale` | `IF_VAR_EQ` (50h) | salta se variabile = valore | 166 |
| `m688.velocita_ora` | `SPEED_NOW` (07h) | velocita' attuale e ordinata in 1/350 di nodo (+22, +32, +2E) | 36 |
| `m688.virata_relativa` | `TURN_REL` (63h) | virata relativa: rotta + valore (modulo 9000), nel verso piu' breve al rateo standard (1E9E). In Lua gradi con il positivo a dritta | 39 |
| `m688.x_maggiore` | `IF_X_GT` (6Ch) | salta se X dell'unita' > valore | 3 |

## Funzioni dell'Enhanced

Nel livello classico non fanno niente. Le chiavi della missione sono in `docs/09_schema_missioni.md` §5.

| Funzione | Descrizione |
|---|---|
| `m688.cue_asw(u, aereo, x, y [, raggio_m [, ritardo_s [, rotta, velocita_kn]]])` | ASW aereo (§5.4): un datum per l'aereo `aereo` (-1: tutti gli aerei controllati), col fix vecchio di `ritardo_s` secondi (predefiniti 1852 m e 0 s) |
| `m688.soluzione(u)` | (vero/falso) il contatto del bersaglio ha una posizione stimata (eco, vista, radar, la soluzione della TMA dell'IA) e non il solo rilevamento; nel classico vale come `traccia_solida`. Da chiedere prima di un lancio |
| `m688.attacca` (Enhanced) | come `ATTACK`, ma non colpisce un bersaglio oltre la portata (per asse, almeno 2 nm) e non fa niente per un aereo guidato dal controllore ASW, che sgancia da sé con le sue regole |
| `m688.allerta(u)` | (numero 0-3, Enhanced; nil nel classico) l'allerta dell'unità: la maggiore fra quella della sua formazione e quella della sua forza (A0 routine, A1 vigilanza, A2 minaccia, A3 combattimento; docs/18 fase 3, dossier 24 §8.1). Parte dalla `paranoia` dell'editor e sale con gli eventi: contatti classificati, albero visto, ping nemico, armi, alleati affondati |
| `m688.armi_libere(u [, bersaglio])` | (vero/falso, Enhanced; nil nel classico) le regole d'ingaggio dell'IA sul bersaglio (per difetto quello corrente; senza bersaglio, sulla parte ostile): pace mai, crisi solo dopo un atto ostile fra le due parti, guerra sì; i neutrali mai, salvo ad A3 su un contatto non classificato (P 0,3). La parte è quella creduta dal sonarista dell'IA. Con nil la libreria usa le sue costanti (`ARMI`) |
| `m688.situazione(u, "pace"/"crisi"/"guerra" [, "usa", "urss"])` | (azione, Enhanced) dichiara la situazione fra due parti (`usa`, `urss`, `neutrale`), o fra tutte senza le parti; alza i minimi d'allerta (A1 in crisi, A2 in guerra). Le regole della missione che cambiano `ARMI` la chiamano, così motore, artiglieria e ASW aereo sanno della guerra |
| `m688.in_zona_amica(u [, bersaglio])` | (vero/falso, Enhanced) la posizione creduta del bersaglio (per difetto quello corrente) sta nell'area d'azione di un sottomarino della parte di u: l'area mobile di 15 nm avanti, 30 indietro e 5 per lato, o per il giocatore le aree d'azione dichiarate dalla missione (24 §8.3). `m688.armi_libere` la guarda già per navi e aerei contro i sottomarini |
| `m688.armi_rimaste(u [, famiglia])` | (numero, Enhanced; nil senza carico o nel classico) i siluri del sottomarino dell'IA nei tubi e di riserva, di una famiglia (`type53`, `mk48`...) o tutti. Il carico è quello della classe nell'anno; un lancio prende dai tubi il siluro migliore contro il bersaglio, il tubo si ricarica in 5 minuti, senza siluri in tubo il lancio non parte (docs/18 fase 4 B) |
| `m688.bersaglio_non_combattente(u)` | (vero/falso, Enhanced; nil nel classico) il bersaglio corrente è un non combattente (flag 40h: mercantili, pescherecci, navi spia), da non ingaggiare |
| `m688.classificato(u [, b [, confidenza]])` | (vero/falso, Enhanced; nil nel classico) il sonarista dell'unità ha classificato il bersaglio corrente (o l'unità `b`) con almeno la `confidenza` data (0 NONSUB, 1 POSSUB, 2 PROBSUB, 3 CERTSUB; senza, qualunque), oppure il contatto è lungo |
| `m688.tempo_contatto(u [, b])` | (secondi, Enhanced; nil nel classico) da quando è nato il contatto attuale dell'unità sul bersaglio corrente (o su `b`); 0 senza contatto |

## Condizioni delle missioni nuove (rapide, editor, libreria)

Non vengono dall'originale: restituiscono `true` o `false` e valgono in tutti e due i livelli. `u` è l'unità corrente
dello script; posizioni e raggi in metri, come in `posizione`.

| Funzione | Vero se |
|---|---|
| `m688.in_cerchio(u, x, y, raggio_m)` | l'unità è entro `raggio_m` dal punto |
| `m688.velocita_sotto(u, kn)` | la velocità attuale è sotto `kn` nodi |
| `m688.quota_entro(u, da_m, a_m)` | la quota è fra `da_m` e `a_m` metri |
| `m688.in_superficie(u)` | l'unità è in superficie (quota fino a 0,381 m, la soglia della scoperta automatica) |
| `m688.alberi_alzati(u)` | periscopio o antenna alzati |
| `m688.obiettivo_compiuto(u, "id")` | l'obiettivo `id` della missione (docs/09 §5.2) è raggiunto ora |
| `m688.contatto_da(u, id, minuti)` | `u` ha avuto in tutto almeno `minuti` di contatto visibile sull'unità `id` |
| `m688.senza_contatto_da(u, id, minuti)` | `u` non vede l'unità `id` da almeno `minuti` (o non l'ha mai vista per tanto) |
| `m688.ping_giocatore_entro(u, raggio_m [, ultimi_s])` | il giocatore ha fatto un ping entro `raggio_m` dall'unità, negli ultimi `ultimi_s` secondi (senza: in tutta la missione) |
| `m688.ha_pingato(u, id [, ultimi_s])` | l'unità `id` ha fatto un ping attivo negli ultimi `ultimi_s` secondi (senza: in tutta la missione); `id` = -1: un'unità qualunque diversa da `u` |
| `m688.comanda_mine(u [, gruppo])` | le mine comandate da terra (chiave `comandata = true, gruppo = '…'` nel campo della missione) del gruppo passano alla stazione: da lì l'operatore le fa scoppiare quando un sottomarino è sopra di loro entro il raggio, dopo 10-30 s; senza gruppo tutte. Vero se ce n'era almeno una |
| `m688.distanza_da(u [, id])` | i metri veri dall'unità a quella `id` (senza: il bersaglio), per le regole che fanno da arbitro; nil se l'altra non c'è, è distrutta o è fuori dal teatro |
| `m688.ping_entro(u, raggio_m [, ultimi_s])` | un'unità qualunque diversa da `u` (anche il giocatore) ha fatto un ping entro `raggio_m` dall'unità, negli ultimi `ultimi_s` secondi |
| `m688.ha_lanciato(u [, id])` | l'unità ostile `id` ha lanciato un'arma (senza `id`, o -1: una qualunque ostile al giocatore) |

| `m688.bqs15(u [, acceso])` | (vero/falso, Enhanced) accende (o spegne con `acceso` falso) il BQS-15 dell'unità: le mine nel settore di prua e gli impulsi che gli altri sentono; falso se la classe non lo ha (solo `los_angeles`) (S06) |
| `m688.mine_scoperte()` | (numero, Enhanced) quante mine ha visto il BQS-15 del giocatore (S06) |
| `m688.distanza_cavo(u, nave)` | (numero, Enhanced) metri fra lo scafo dell'unità e il cavo della schiera trainata della nave; -1 se la nave non fila (S10) |
| `m688.esercitazione(u [, si])` | (vero/falso) l'arma u diventa da esercitazione, o lo diventano le armi che l'unità u lancerà: il colpo si registra senza danni (schede I03 e S01; docs/09 §4) |
| `m688.in_acque_territoriali(u)` | (vero/falso, Enhanced) l'unità è nelle acque territoriali di un altro paese dichiarate dalla missione (chiave `acque_territoriali`, docs/09 §4) |
| `m688.avaria(u [, si])` | (vero/falso) l'unità dell'IA perde la propulsione (o la ripara con `si` falso); falso per il giocatore (M6, docs/09 §4) |
| `m688.incaglia(u)` | (vero/falso) l'unità dell'IA resta ferma sugli scogli finché non la rimorchiano (M6) |
| `m688.rimorchia(u, id [, velocita_kn, cavo_m])` | (vero/falso) la nave u prende a rimorchio l'unità id (6 nodi e 300 m per difetto); falso se una delle due è già in un rimorchio (M6) |
| `m688.molla_rimorchio(u)` | (vero/falso) finisce il rimorchio di cui u fa parte (M6) |
| `m688.in_avaria(u)` | (vero/falso) l'unità è in avaria o incagliata (M6) |
| `m688.rimorchiata(u)` | (vero/falso) l'unità è a rimorchio (M6) |
| `m688.pedina_vicino(u, id [, distanza_m, sopra_m])` | (vero/falso) il sottomarino dell'IA u pedina da vicino il sottomarino id, di qualunque parte: si tiene nel suo cono sordo (dai sensori in ascolto di id; la schiera filata lo restringe) a `distanza_m` (400 per difetto), alla quota che vede più `sopra_m` (negativo: sotto), e ne segue rotta, velocità e quota con il ritardo con cui le vede (20-60 s). Evita l'urto da sé, sulla geometria vista: un'accostata o un arresto brusco del pedinato può portare all'urto. `id` -1 smette; senza contatto per 5 minuti smette da sé. Registro: reazione `pedina_vicino` e `fine_pedina_vicino`. Solo nell'Enhanced (S24) |
| `m688.salvati(u, n, di)` | (niente) il soccorso della missione: n uomini salvati sui di che erano in acqua (anche `m688.salvati(n, di)`). Vale per l'achievement «tutti salvati» (n >= di > 0) e, in pace, per la decorazione al coraggio della carriera (rapide ed editor). L'ultima chiamata vince |
| `m688.posa_mina(u, "tipo" [, quante, ogni_s])` | (azione, Enhanced) l'unità dell'IA posa mine sotto di sé; vero se la posa è partita (docs/09 §5.1c) |
| `m688.radio(u, "ID")` | (azione, Enhanced) il messaggio radio della missione con quell'id va in onda subito; vero se è partito (docs/09 §5.1, R3/E6) |

I messaggi di bordo propri della missione (`messaggi = { [n] = { it, en } }`, docs/09 §5.1b) si mandano con
`m688.messaggio(u, 1000 + n)`: non sono esiti.

Il lancio di un TLAM-C (Enhanced) esegue il gestore `armi.attacco_terra` della missione, se c'è, mai quello del
`tomahawk`; il suo `m688.lancio_missile(u)` non conta un secondo lancio (docs/09 §5).

I contatti si contano per il giocatore dall'inizio. Per le altre unità si contano dalla prima domanda dello script su
di loro.

Per i file di terzi, l'app crea la missione con `Missione(file, seme, LimitiScript{istruzioni, memoria})`. Una chiamata
che supera il limite finisce con un errore in `errori()`, e la missione continua.
