# F7. Editor e pacchetti

*Autore: trailer-1. Revisione del 31/10/2026 (le condizioni «unità classifica» e «giocatore vicino a», le azioni «quota» e «rumore», le mine in sicurezza e comandate, l'azione «comanda mine», la libreria a 41 missioni, i confronti delle variabili, le azioni «togli unità» e «salvati», il pedinamento ravvicinato) e del 06/11/2026 (le scene M3, M4 e T5-T9, la data delle scene, il tiro dei missili dell'IA). Stato: rivisto.*

## 1. A che serve

L'editor permette di costruire una missione senza scrivere codice: teatro, unità, rotte, zone, obiettivi, regole
«quando… allora…», briefing. Il risultato è un file di missione come quelli delle originali, giocabile nei due
livelli. Il pacchetto `.m688m` serve a scambiare le missioni: chi lo riceve lo importa in sicurezza, perché del
pacchetto entra solo la descrizione della missione, mai il suo codice. Con lo stesso strumento sono fatte la libreria
di 41 missioni pronte (23 per il 688 e 18 per l'Alfa) e le scene di prova delle interazioni fra unità dell'IA.

## 2. Il modello

### 2.1 Lo scenario

Lo **scenario** è la missione come la pensa un autore (`missioni/scenario_missione.hpp`, `ScenarioMissione`). È
comune all'editor, alle missioni rapide (capitolo F6), alla libreria e alle scene di prova.

| Parte | Che cosa contiene |
|---|---|
| intestazione | codice, titolo in italiano e inglese, autore, teatro, battello (688 o Alfa), data e ora, situazione (pace, crisi, guerra), paranoia, difficoltà, durata |
| meteo | semplificato, realistico del teatro, o fissato (vento, mare, visibilità…) |
| carico | dotazione, deposito, armi nei tubi, tipo di missione, ospiti |
| unità | classe, parte, posizione, quota, rotta e velocità, punti di rotta (ciclici o con uscita), comportamento e parametri, riserva, nascosta |
| zone | cerchi con nome, per le regole; una zona può essere anche un'area dichiarata dalla sala radio (d'azione, libera ASW, protetta), all'inizio o con un messaggio |
| obiettivi | primari e secondari, dei tipi del registro (capitolo F1): affondare, colpire, proteggere, raggiungere, scoprire, non essere scoperto, sopravvivere, minare, variabile, contatto, foto, zona inviolata, TLAM arrivati, scatola; per il riconoscimento visivo identificare (l'ultima classificazione visiva giusta), telemetro (le distanze lette entro un errore relativo) e rotta stimata (le rotte dall'AOB entro un errore in gradi; la voce STIMA sceglie AOB AL PERISCOPIO o TMA AL MATE, capitolo C13 §2.3) |
| regole | condizioni e azioni a vocabolario chiuso (§2.3) |
| briefing | i paragrafi degli ordini, nelle due lingue |
| radio | i messaggi della sala radio dell'Enhanced (capitolo C11) |
| altro (Enhanced) | campo minato già posato, danni iniziali del battello, armi da esercitazione, la situazione col luogo, la navigazione, la libreria dei TLAM |

**Le chiavi d'unità dell'Enhanced:**
- `stato`: in avaria, incagliata (ferma finché qualcuno non la rimorchia) o relitto (affondata, ferma sul fondo, muta);
- `batteria`: la carica iniziale di un diesel dell'IA; sotto 0,3 va a snorkel, a 0,05 emerge per forza;
- `sonar_speciale`: un sonar attivo diverso da quello della classe (livello, frequenza, impulso, antenna);
- `armi_esercitazione`: le armi che l'unità lancia sono inerti;
- `emcon`: l'EMCON di partenza (silenzio, navigazione, ricerca, intermittente, ingaggio); vuoto, quello della parte. Gli
  eventi possono solo alzarlo;
- `luci`: le luci di navigazione di notte (docs/27): moto, fonda, pesca, manovra_limitata, non_governa, spente; vuota, le
  sceglie il motore dal tipo, dalla velocità e dalla guerra (le militari e i sottomarini spengono in guerra);
- `cisterne`: lo stato delle cisterne di una petroliera: zavorra, zavorra_inerte (col gas inerte); vuota, cariche (E2);
- `parte = "luogo"`: l'unità è della terza parte armata, il paese del teatro (M7). La sua situazione di partenza con le
  altre due parti è la chiave della missione `situazione_luogo = { usa, urss }`; senza, il luogo è in pace con tutti.

**Le chiavi della missione dell'Enhanced** per i missili (capitolo D3):
- `navigazione = { sigma_iniziale_nm, realistica_di_serie }`: l'errore di partenza della posizione stimata, con la
  navigazione realistica (capitolo C12); con `realistica_di_serie = true` il menu propone la navigazione realistica
  quando si sceglie la missione, e il giocatore la toglie con N;
- `tlam`: la libreria di bordo delle missioni TLAM, ognuna con il bersaglio, il punto di lancio (PLP), il landfall, il
  tempo sul bersaglio e la sua finestra. L'ordine d'esecuzione lo porta un messaggio radio (`esecuzione_tlam`).

### 2.2 I comportamenti

Le missioni dell'editor usano la **libreria 2** dei comportamenti; le rapide la libreria 1, che resta com'era per non
cambiare i loro file.

| Comportamento | Che cosa fa |
|---|---|
| solo rotta | segue i punti di rotta e basta |
| nave (`R_nave`) | segue la rotta; su un contatto nemico subacqueo accosta, fa ping e, se le regole d'ingaggio lo consentono, attacca (le navi nemiche le lascia al tiro del motore e ai missili; nelle rapide dalla versione 2) |
| sottomarino (`R_sottomarino`) | ascolta, si avvicina, lancia se le regole lo consentono, evita i siluri scendendo in profondità |
| aereo (`R_aereo`) | il pattugliamento degli aerei; con l'ASW aereo dell'Enhanced lo guida il controllore aereo (capitolo E5) |
| pedinamento (`R_pedina`) | sul contatto nemico si porta alla distanza scelta e ci resta, alla stessa velocità, senza farsi notare |
| pedinamento ravvicinato (`R_pedina_vicino`) | solo un sottomarino e solo sul giocatore: con una buona traccia si tiene nel suo cono sordo alla distanza scelta, 20 m di quota da lui, e ne segue le manovre con ritardo; un'accostata o un arresto brusco possono portare all'urto. Se il giocatore pinga entro 3 km smette e se ne va (S24) |

**Parametri per unità** (vuoti: vale quello della missione): paranoia (0-3: ping, tracce medie, avvicinamento,
allarme), salve di un sottomarino, portata delle armi in miglia, distanza di pedinamento (predefinita 2 nm).

**Nell'Enhanced** (docs/18, fasi 3 e 4) i comportamenti chiedono al motore:
- le regole d'ingaggio dell'IA (`m688.armi_libere`): in crisi solo dopo un atto ostile fra le parti, mai nell'area
  d'azione di un sottomarino amico;
- l'allerta (`m688.allerta`), che sale con gli eventi e non scende mai sotto la paranoia;
- la soluzione (`m688.soluzione`): sottomarini e navi lanciano solo su una posizione stimata, e finché non c'è il
  motore fa da sé le gambe di TMA;
- i siluri rimasti (`m688.armi_rimaste`);
- se il bersaglio è un non combattente (`m688.bersaglio_non_combattente`: mercantili, pescherecci, navi spia): finché
  le armi non sono libere nessun comportamento lo aggancia, nemmeno di una parte nemica (il peschereccio in pace); in
  guerra un convoglio nemico resta un bersaglio.

Nel classico queste funzioni non rispondono e valgono la paranoia e la situazione della missione.

Le navi cacciano prima di sparare (**STIME**):
- la prima salva antisom arriva dopo 13 minuti sul contatto attuale (`m688.tempo_contatto`; nel classico dopo 330
  passaggi di caccia): è la caccia fino al PROBSUB;
- dopo ogni salva la nave aspetta 20 passaggi prima della successiva;
- dopo 6 salve torna ad ascoltare sulla sua rotta, e riattacca solo dopo 120 passaggi sul contatto riacquisito.

### 2.3 Le regole

Una regola è «quando *tutte* queste condizioni sono vere, allora fai queste azioni». Si controlla ogni **10 s**, sulla
sua unità: quella di una condizione «unità in zona», altrimenti il giocatore. Di norma scatta una volta sola.

**Condizioni** (`CondizioneScenario::Tipo`):

| Condizione | Vera quando |
|---|---|
| tempo | sono passati almeno N secondi |
| unità colpita, unità fuori | l'unità è colpita (o affonda); non è più in gioco (affondata o uscita) |
| giocatore in zona, fuori zona | il giocatore è dentro, o fuori, da una zona |
| unità in zona | l'unità è dentro la zona |
| allarme | un'unità dell'IA ha dato l'allarme |
| giocatore ha lanciato, ostile ha lanciato | il giocatore ha lanciato un'arma; un'unità ostile (o una data) ne ha lanciata una |
| variabile | una variabile dell'autore è =, > o < di un valore (altri confronti, come >=, sono un errore della verifica) |
| regola scattata, obiettivo compiuto | un'altra regola è scattata; un obiettivo è raggiunto |
| giocatore sotto / sopra i nodi | il giocatore va sotto, o sopra, una velocità |
| giocatore a quota, in superficie, alberi alzati | fra due quote; in superficie; con periscopio o antenna alzati |
| contatto su, senza contatto su | il giocatore ha avuto N minuti di contatto su un'unità; non la sente da N minuti |
| ping vicino | il giocatore ha fatto ping entro N metri da un'unità |
| unità vede, unità non vede | un'unità dell'IA ha avuto, o ha perso, il contatto su un'altra per N minuti |
| vicino al cavo | il giocatore è vicino al cavo dell'antenna rimorchiata di una nave (Enhanced) |
| unità in superficie, in avaria, a rimorchio | un diesel emerso; un'unità in avaria o incagliata; a rimorchio |
| giocatore ha trasmesso | il giocatore ha fatto una trasmissione di un tipo (contatto, attacco, situazione, conferma, richiesta, previsione) dopo N secondi |
| evento | nel registro c'è un evento con quel nome (incaglio, collisione, colpo d'esercitazione…) dopo N secondi |
| unità classifica | il sonarista di un'unità dell'IA ha classificato un'altra unità (il giocatore, se non indicata) almeno con una confidenza: NONSUB, POSSUB, PROBSUB, CERTSUB; o il suo contatto è lungo (Enhanced) |
| giocatore vicino a | il giocatore è a meno di N metri da un'unità (la posizione vera: la regola fa da arbitro) |

**Azioni** (`AzioneScenario::Tipo`):

| Azione | Che cosa fa |
|---|---|
| messaggio | un messaggio di bordo: un codice dell'originale o un testo proprio (codici da 1000) |
| fine missione | chiude con un esito proprio e il suo testo, con la categoria (successo, fallimento…) |
| variabile | assegna, somma o sottrae a una variabile dell'autore |
| comportamento, vai a | cambia il comportamento di un'unità; la manda in un punto, con una nuova velocità |
| entra in scena | fa entrare un'unità di riserva |
| cue ASW | dà agli aerei antisommergibile un punto e un raggio dove cercare |
| allarme | accende l'allarme |
| sposta zona | sposta il centro di una zona |
| radio | manda in onda subito un messaggio della sala radio |
| situazione | cambia le armi dell'IA: pace, crisi, guerra; fra tutte le parti, o solo fra due (USA, URSS, luogo) |
| posa mine | un'unità posa N mine di un tipo, una ogni N secondi (Enhanced) |
| avaria, rimorchia, molla il rimorchio | guasta o ripara un'unità; un'unità ne prende a rimorchio un'altra; la lascia |
| esercitazione | rende inerti le armi di un'unità, o le rimette vere |
| attacca unità | un sottomarino dell'IA attacca un bersaglio ignorando parte, regole d'ingaggio e non combattenti; con valore 1 anche senza soluzione, sui dati veri (il tiro pianificato d'esercitazione). Vale solo per i sottomarini: le navi sparano con la situazione di guerra fra le parti |
| lancia missile | un'unità tira N missili antinave del modello (1 se non indicato) su un bersaglio (il giocatore, se non indicato), pianificati sulla posizione che ne ha; nel classico è il MISSILE dello script |
| quota | un sottomarino va a una quota (in metri) col rateo del modello: si immerge o risale (l'SSBN della S14, il Charlie della S15) |
| rumore | un'unità fa un rumore transitorio di 11 secondi: una bomba a mano in acqua, una carica o una granata di segnalazione (S14, S15, S16) |
| comanda mine | le mine comandate da terra di un gruppo (tutte, se non indicato) passano alla stazione, che le fa scoppiare sotto un sottomarino che ci passa sopra (S18) |
| togli unità | l'unità esce dal gioco senza essere distrutta: un battello che affonda fuori dalla vista del giocatore (S23) |
| salvati | il soccorso: quanti uomini sono stati salvati su quanti erano in acqua. Conta per il riconoscimento «tutti salvati» e, in pace, per la carriera; vale l'ultima (S23) |

**Le variabili** dello scenario: la 1 conta i lanci del giocatore (dal kit), la 10 è la fase, la 11 l'allarme, da 100
le salve per unità, da 200 le regole scattate, da 300 le dieci variabili dell'autore.

### 2.4 Lo scrittore e il file

Un solo **scrittore** (`scrivi_missione_lua`) trasforma lo scenario nel file Lua del **formato 1**
(`docs/09_schema_missioni.md`), lo stesso delle 22 originali convertite:
- la tabella della missione con le unità, i modelli del livello classico per classe, gli obiettivi, i testi;
- gli script dei comportamenti, delle rotte e delle regole, con l'API classica `m688.*`;
- i comandi e le armi del giocatore dal kit della sua parte (`data/motore/missioni_rapide/kit_*.lua`);
- per le missioni dell'editor, il blocco dichiarativo `editor = { formato_editor = 1, scenario = { … } }`, che il motore
  ignora.

Lo scrittore è **deterministico**: lo stesso scenario dà lo stesso testo, e uno scenario riletto e riscritto dà lo
stesso file, byte per byte. `leggi_scenario` rilegge il blocco `editor` nell'ambiente isolato (§2.6), senza eseguire gli
script.

**Gli identificativi.** Il motore tratta le unità 0 e 1 come i due giocatori dell'originale: il giocatore è sempre la
0, e le missioni della libreria cominciano le altre unità dalla 2.

### 2.5 La verifica

`verifica()` elenca i problemi prima del salvataggio, ognuno con un codice stabile e l'unità da mostrare.
- **Errori** (la missione non si scrive o non si gioca): senza giocatore o con due, unità sulla terra, identificativi
  doppi, classi o comportamenti sconosciuti, obiettivi senza unità o senza zona, regole che nominano unità, zone,
  messaggi o regole che non ci sono, una regola che guarda unità diverse, regole d'ingaggio con nomi sconosciuti, mine sulla terra,
  danni iniziali con sistemi sconosciuti, stati e antenne sconosciuti, un attacco o un lancio di missili su un bersaglio sconosciuto o sull'unità stessa.
- **Attenzioni** (si gioca, ma forse non come voluto): rotte che passano sulla terra, zone o obiettivi fuori dalla
  carta, unità di riserva che nessuna regola fa entrare, regole vuote, mine su un fondale inadatto, durata strana.

### 2.6 L'ambiente Lua isolato

Le missioni di altri non devono poter fare danni. Due difese:

**In lettura** (scenari e pacchetti, `missioni/src/lua_isolato.hpp`):
- librerie: base senza `dofile`, `loadfile`, `load`, `require`, `collectgarbage`, `print`; `math`, `string`,
  `table`. Niente `io`, `os`, `package`, `debug`;
- solo testo, niente bytecode precompilato;
- un `m688` finto, le cui funzioni non fanno niente;
- un tetto di **64 MB** di memoria e di **50 milioni** di istruzioni al caricamento.

**In partita** (`LimitiScript`, `app/partita.cpp`): le missioni della cartella delle importate girano con un tetto di
**10 milioni di istruzioni per chiamata** di un gestore e **64 MB** di memoria. Superato il tetto, la chiamata finisce
con un errore nel registro e la partita continua. La prova dell'editor (simula 10 minuti) usa gli stessi limiti.

### 2.7 Il pacchetto `.m688m`

Il pacchetto è il file della missione con in più il blocco:

```
pacchetto = { formato_pacchetto, formato_missione, formato_editor, libreria, autore, creato,
              dipendenze = { teatro, impronta_teatro, classi = { … }, impronta_classi, dati_rapide },
              impronta = "sha256:…" }
```

- `impronta`: lo SHA-256 del file con il valore dell'impronta azzerato. Scopre un file rovinato o modificato a metà;
  non è una firma.
- `impronta_teatro`, `impronta_classi`: dicono se chi importa ha lo stesso fondale e gli stessi modelli.

**L'importazione**, nell'ordine:
1. l'impronta;
2. i formati (pacchetto, missione, editor);
3. lo scenario, letto nell'ambiente isolato;
4. le dipendenze: il teatro e le classi;
5. le funzioni `m688` usate dal codice: solo quelle conosciute;
6. la **riscrittura**: il file si rigenera dallo scenario con lo scrittore di questo gioco. Se il codice del pacchetto
   era diverso da quello rigenerato (modificato a mano, o di un'altra versione), lo dice un avvertimento.

Un pacchetto rifiutato lo dice con il perché in italiano. Le cartelle stanno accanto ai fascicoli: `missioni_editor`,
`missioni_importate` (con `da_importare`, `importati`, `rifiutati`) e `missioni_esportate`.

### 2.8 Da una missione originale

`scenario_da_originale` apre nell'editor una delle 22 missioni convertite, nella variante più probabile. Riprende teatro,
parte, unità (posizioni, quote, rotte), dotazione, tubi, briefing e gli obiettivi che non dipendono dagli script. I
comportamenti diventano quelli della libreria 2. La logica degli script originali non si riprende: una nota elenca le
unità e gli obiettivi lasciati fuori.

### 2.9 La libreria

La libreria è fatta con l'editor da uno strumento (`strumenti/libreria_missioni.cpp`): ogni missione è uno scenario
scritto in C++, salvato come file `.lua` e come pacchetto `.m688m` in `data/missioni/libreria`. Due serie: le
**inedite** (`LIB-I…`) e le **storiche**, ispirate a episodi della Guerra fredda (`LIB-S…`, dossier 23). Le schede dei
soggetti sono in `docs/17_libreria_missioni.md`.

<!-- generato: libreria_missioni -->
| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Durata |
|---|---|---|---|---|---|---|
| LIB-I01 | La balena di ferro | 688 | Fær Øer e Mar di Norvegia | pace | 4 | 2 h 30 |
| LIB-I02 | Il pescatore di Teriberka | Alfa | Mare di Barents, approcci a Kola | crisi | 3 | 2 h |
| LIB-I03 | Acque basse, nervi tesi | 688 | Mar Baltico sud-occidentale | pace | 4 | 2 h |
| LIB-I04 | La porta di Ercole | Alfa | Stretto di Gibilterra | guerra | 5 | 3 h |
| LIB-I05 | Il disertore | 688 | Canale d'Otranto | crisi | 3 | 2 h |
| LIB-I06 | Il ponte di Ekaterina | Alfa | Passo di Kunashir e Mare di Okhotsk | guerra | 5 | 3 h |
| LIB-I07 | Lupo ferito | 688 | Margine della Piattaforma Celtica | guerra | 4 | 4 h |
| LIB-I08 | Ombra lunga | Alfa | Fær Øer e Mar di Norvegia | pace | 4 | 3 h |
| LIB-I09 | Sotto il bastione | 688 | Mare di Barents, approcci a Kola | pace | 5 | 3 h |
| LIB-I10 | Corsa al relitto | Alfa | Margine della Piattaforma Celtica | pace | 2 | 2 h 40 |
| LIB-I11 | Il primo eco | 688 | Fær Øer e Mar di Norvegia | guerra | 3 | 2 h |
| LIB-I12 | Finestra di lancio | 688 | Mare di Barents, approcci a Kola | guerra | 4 | 3 h |
| LIB-I13 | Sagome nello Stretto | 688 | Stretto di Gibilterra | pace | 2 | 1 h 30 |
| LIB-I14 | Levante | 688 | Stretto di Gibilterra | pace | 3 | 1 h 30 |
| LIB-I15 | Fanali | 688 | Stretto di Gibilterra | pace | 4 | 1 h 30 |
| LIB-I16 | Il bastione bianco | 688 | Passo di Kunashir e Mare di Okhotsk | pace | 5 | 4 h |
| LIB-I17 | La soglia di Ekaterina | Alfa | Passo di Kunashir e Mare di Okhotsk | pace | 3 | 3 h |
| LIB-S01 | Vestfjord | 688 | Fær Øer e Mar di Norvegia | pace | 1 | 1 h 30 |
| LIB-S02 | Nave mineraria | 688 | Passo di Kunashir e Mare di Okhotsk | pace | 2 | 2 h |
| LIB-S03 | Quarantena | 688 | Margine della Piattaforma Celtica | crisi | 3 | 3 h |
| LIB-S04 | Campane d'edera | 688 | Passo di Kunashir e Mare di Okhotsk | pace | 5 | 4 h 30 |
| LIB-S05 | Il guardiano del cavo | Alfa | Passo di Kunashir e Mare di Okhotsk | pace | 3 | 2 h |
| LIB-S06 | Petroliere | 688 | Stretto di Gibilterra | crisi | 3 | 3 h |
| LIB-S07 | Tappo | 688 | Mare di Barents, approcci a Kola | guerra | 4 | 2 h 30 |
| LIB-S08 | Valona | Alfa | Canale d'Otranto | crisi | 2 | 1 h 30 |
| LIB-S09 | Aport | Alfa | Fær Øer e Mar di Norvegia | pace | 4 | 4 h 30 |
| LIB-S10 | Il cavo della fregata | Alfa | Margine della Piattaforma Celtica | pace | 3 | 2 h |
| LIB-S11 | Tenete il confine | Alfa | Mar Baltico sud-occidentale | crisi | 3 | 2 h |
| LIB-S12 | Il gruppo cacciatore | Alfa | Canale d'Otranto | guerra | 3 | 1 h 30 |
| LIB-S13 | Outlaw Shark | 688 | Canale d'Otranto | pace | 2 | 1 h 30 |
| LIB-S14 | Il pedinatore in attesa | Alfa | Margine della Piattaforma Celtica | pace | 3 | 2 h 30 |
| LIB-S15 | L'ancoraggio | 688 | Canale d'Otranto | crisi | 4 | 3 h |
| LIB-S16 | La scala | Alfa | Fær Øer e Mar di Norvegia | pace | 4 | 2 h |
| LIB-S17 | Sotto la prora | Alfa | Passo di Kunashir e Mare di Okhotsk | pace | 4 | 3 h |
| LIB-S18 | L'arcipelago | 688 | Mar Baltico sud-occidentale | pace | 5 | 2 h 30 |
| LIB-S19 | Pietra santa | 688 | Mare di Barents, approcci a Kola | pace | 4 | 3 h |
| LIB-S20 | Cinquecento iarde | 688 | Margine della Piattaforma Celtica | pace | 3 | 2 h 30 |
| LIB-S22 | La rotta lunga | Alfa | Margine della Piattaforma Celtica | pace | 4 | 4 h |
| LIB-S23 | Acqua a due gradi | Alfa | Mare di Barents, approcci a Kola | pace | 3 | 4 h 30 |
| LIB-S24 | Quota zero | Alfa | Passo di Kunashir e Mare di Okhotsk | pace | 3 | 2 h |
| LIB-S25 | Ombra del boomer | 688 | Margine della Piattaforma Celtica | pace | 3 | 6 h |
<!-- fine: libreria_missioni -->

### 2.10 Le scene di prova dello spettatore

Le sette scene del dossier 24 §8.9, più la S8 della parte del luogo (M7), sono missioni di prova in
`data/missioni/prove`, in dieci file (S1 e S3 hanno anche una variante: S1G in guerra, S3A vista dall'Alfa), scritte
dallo stesso scrittore con `strumenti/scene_interazioni.cpp`. Mostrano le interazioni fra unità dell'IA che il
giocatore può vedere: un KPUG che trova un sottomarino alleato, una fregata che pedina un Victor III, un convoglio
silurato, la crisi che diventa guerra, una Koni del luogo che a 15 minuti dichiara la guerra agli USA. Nella stessa
cartella lo strumento scrive:
- le due scene dei missili, M1 (antinave con scorta e un neutrale) e M2 (TLAM), che servono alla pagina di tiro
  (capitolo D3);
- M3, per l'esperimento della spia (capitolo T12 §10): uno Slava con una Krivak e, a 120 km, due Perry, senza uno script
  che comandi il tiro;
- M4, per il quadro radio ai sottomarini: un Victor III e, a 40 km, un convoglio con una Perry. Il Victor sente il convoglio
  da solo, quindi non misura la caccia sul dato (capitolo E6 §7).

Le scene dei trailer, girate per il periscopio:
- T1: un Victor III a 1500 m, da colpire con `--urto-unita` per vedere l'emersione violenta (capitolo B3);
- T2: uno Slava a 1500 m che tira due P-500 su una Perry, col mare 4 e la pioggia (capitoli D3, E7);
- T3: di notte, una Perry in allarme che spara all'Alfa col periscopio alzato (capitolo E4);
- T4: un Ka-27 che caccia il 688 di giorno, in hover col sonar calato (capitolo E5);
- T5, T6 e T7: sotto il pack del bastione col Delta III a prua, in una polynya, e al margine con l'Il-38 che caccia;
- T8 e T9: la notte della I15 con le navi a portata dell'oculare, e la costa marocchina da vicino.

Non sono nel menu: si giocano con `--cartella-missioni data/missioni/prove`. Avviate da riga di comando o da codice prendono
la data del loro file (`app/data_missione.hpp`), come dal menu, e `--data` e `--ora` la sostituiscono. Il piano è
`docs/18_interazioni_ia.md`.

<!-- generato: scene_prova -->
| File | Scena | Battello | Teatro | Situazione |
|---|---|---|---|---|
| `PROVA_M1_ANTINAVE` | M1 - Antinave con scorta e un neutrale | 688 | Fær Øer e Mar di Norvegia | guerra |
| `PROVA_M2_TLAM` | M2 - TLAM: area di lancio e navigazione incerta | 688 | Mare di Barents, approcci a Kola | guerra |
| `PROVA_M3_ORIZZONTE` | M3 - Oltre l'orizzonte | 688 | Fær Øer e Mar di Norvegia | guerra |
| `PROVA_M4_QUADRO` | M4 - Il sottomarino oltre l'orizzonte | 688 | Fær Øer e Mar di Norvegia | guerra |
| `PROVA_S1G_KPUG_GUERRA` | S1G - KPUG e SSN alleato (guerra) | 688 | Mare di Barents, approcci a Kola | guerra |
| `PROVA_S1_KPUG` | S1 - KPUG e SSN alleato | 688 | Mare di Barents, approcci a Kola | crisi |
| `PROVA_S2_PEDINAMENTO` | S2 - Una Perry pedina un Victor III | 688 | Fær Øer e Mar di Norvegia | pace |
| `PROVA_S3A_BASTIONE_ALFA` | S3A - Il bastione visto dall'Alfa | Alfa | Mare di Barents, approcci a Kola | guerra |
| `PROVA_S3_BASTIONE` | S3 - L'intruso nel bastione | 688 | Mare di Barents, approcci a Kola | guerra |
| `PROVA_S4_DUE_SSN` | S4 - Due SSN amici nella stessa acqua | 688 | Fær Øer e Mar di Norvegia | guerra |
| `PROVA_S5_CONVOGLIO` | S5 - Un mercantile silurato nel convoglio | 688 | Margine della Piattaforma Celtica | guerra |
| `PROVA_S6_USCITA` | S6 - L'uscita del Typhoon | Alfa | Mare di Barents, approcci a Kola | pace |
| `PROVA_S7_PORTAEREI` | S7 - La crisi diventa guerra | 688 | Stretto di Gibilterra | crisi |
| `PROVA_S8_LUOGO` | S8 - La parte del luogo | 688 | Canale d'Otranto | pace |
| `PROVA_T1_EMERSIONE` | T1 - Un sottomarino colpito torna su | 688 | Margine della Piattaforma Celtica | pace |
| `PROVA_T2_MISSILI` | T2 - Missili nel mare grosso | 688 | Fær Øer e Mar di Norvegia | pace |
| `PROVA_T3_FUOCO` | T3 - Il fuoco di notte | Alfa | Margine della Piattaforma Celtica | guerra |
| `PROVA_T4_ELICOTTERO` | T4 - L'elicottero | 688 | Mare di Barents, approcci a Kola | guerra |
| `PROVA_T5_VOLTA` | T5 - La volta | 688 | Passo di Kunashir e Mare di Okhotsk | pace |
| `PROVA_T6_POLYNYA` | T6 - La polynya | 688 | Passo di Kunashir e Mare di Okhotsk | pace |
| `PROVA_T7_MARGINE` | T7 - Il margine | 688 | Passo di Kunashir e Mare di Okhotsk | pace |
| `PROVA_T8_FANALI` | T8 - I fanali da vicino | 688 | Stretto di Gibilterra | pace |
| `PROVA_T9_COSTA` | T9 - La costa da vicino | 688 | Stretto di Gibilterra | pace |
<!-- fine: scene_prova -->

`scene_interazioni --semi N` le gioca nell'Enhanced su N semi, col giocatore fermo, e riassume chi vede chi fra parti
ostili e quando, le armi in acqua, chi è colpito. Lanciato senza opzioni, lo strumento riscrive `data/missioni/prove`: per
una prova va usato `--uscita CARTELLA`. `--apertura` scrive le due scene del video d'apertura in `data/motore/apertura`
(`docs/29_video_apertura.md`), fuori dagli elenchi delle missioni.

## 3. Fonti e confidenza

L'editor e i pacchetti sono strumenti, non modelli fisici: qui le fonti sono i formati e le scelte di sicurezza.

| Voce | Fonte | Confidenza |
|---|---|---|
| Formato 1 della missione | le 22 originali convertite, `docs/09_schema_missioni.md` | alta (verificato sulle originali) |
| Modelli del livello classico per classe | `data/motore/missioni_rapide.toml [modelli]`, dalle originali; analogie per le classi nuove | alta, **STIMA** (analogie) |
| SHA-256 | FIPS 180-4, provato sui vettori ufficiali | esatto |
| Limiti dell'ambiente isolato (64 MB, 50 e 10 milioni di istruzioni) | scelti larghi per le missioni vere, stretti per i cicli senza fine | **STIMA** |
| Controllo delle regole ogni 10 s | scelta di progetto: abbastanza spesso per le zone, poco costoso per gli script | **STIMA** |
| Soggetti della libreria | dossier 23 (episodi), dossier 01-03 (piattaforme, teatri) | media |
| Scene di prova | dossier 24 §8.9 | media |

## 4. Classico ed Enhanced

Il file scritto dall'editor usa l'API classica, quindi la missione si gioca nei due livelli. Nel classico si ignorano le
chiavi dell'Enhanced: radio, mine posate, danni iniziali, stato, batteria, sonar speciale, armi da esercitazione, ASW
aereo, EMCON, luogo, navigazione e TLAM. L'azione «lancia missile» nel classico diventa il MISSILE dello script
(registro e avviso al giocatore), senza il modello del missile. Nel 1989 l'editor non c'era: le missioni erano programmi della VM degli scenari (capitolo A3).

## 5. Come si vede in gioco

- **Scelta della missione, scheda «Editor»** (volume 1, cap. 11.2): l'elenco delle missioni della libreria (LIB.), del
  giocatore e importate (IMP.), «+ Nuova missione», «+ Da una originale». Invio apre l'editor, G gioca, I importa.
- **L'editor**: la consolle «SALA PIANI» (capitolo E9 §2.9.5), con la carta del teatro nella grafica della stazione
  Navigazione e otto schede a linguette (Missione, Unità, Rotte, Zone, Obiettivi, Regole, Briefing, Prova). In fondo a
  ogni scheda la verifica con la spia e i primi due avvisi; il piede comune coi tasti Esc, S, E e G. Nella scheda
  Briefing c'è anche la vista «Radio (Enhanced)».
- **La scheda Prova**: simula 10 minuti nel classico (unità, messaggi, colpi, esito, errori degli script), salva (S),
  esporta (E), prova in mare (G); sotto, gli avvisi della verifica in rosso (errori) e in giallo (attenzioni), e un clic
  mostra l'unità.
- **Il debriefing** e l'albo d'onore segnano l'origine «editor» o «libreria».

## 6. Taratura e prove

- `test/prove_scenario_missione.cpp`: scrittura e rilettura identiche, verifica, regole, chiavi dell'Enhanced
  («scenario: stato, avaria e rimorchio…», «foto, zona inviolata, unita' da raggiungere, danni iniziali e sonar
  speciale», «armi da esercitazione, obiettivo colpire, relitto e antenna del sonar speciale», «l'azione «lancia_missile»
  si scrive, si rilegge e si verifica (I11)»), i file radio delle 22 originali.
- `test/prove_editor_missione.cpp`: «missione nuova, unita' col mouse, testi, salvataggio e rilettura», «una rapida
  aperta nell'editor e tutte le schede disegnate».
- `test/prove_pacchetto_missione.cpp`: «SHA-256 sui vettori di FIPS 180-4», «esportato e importato, la missione e'
  quella rigenerata», «rifiutati i file rovinati, di altre versioni, con dipendenze o funzioni sconosciute», «la
  cartella da importare, importati e rifiutati spostati».
- `test/prove_libreria_missioni.cpp`: i file della libreria aggiornati, i pacchetti che si importano, 30 minuti nei due
  livelli; per ogni missione la vittoria e la sconfitta preparate. Per I11, nell'Enhanced, lo Slava tira i P-500 sul
  688 emerso a 35 km (a 340 s, col contatto radar che lampeggia nel mare 4).
- `test/prove_interazioni_scene.cpp`: le scene di prova si leggono, si verificano, si riscrivono uguali e si giocano su
  due semi; otto attese del dossier «da completare» (solo con `M688_PROVE_LUNGHE=1`).

## 7. Limiti noti

- L'editor non piazza armi: la chiave `esercitazione` su un siluro della riserva si scrive solo a mano.
- Il vocabolario delle regole è chiuso: niente script liberi. È voluto, per i pacchetti sicuri.
- Una regola guarda un'unità sola: per due unità diverse servono due regole.
- La libreria ha 41 missioni (23 per il 688, 18 per l'Alfa): comprese le tre di riconoscimento visivo (volume 1, cap. 11.3), le due sotto il ghiaccio (I16 e I17, Okhotsk), quelle del 30/10/2026, da S14 a S20, e le quattro del 31/10/2026, da S22 a S25.
- Le mine dello scenario possono essere in sicurezza (`sicura = true`: inerti per sempre, ma i sonar da mine le vedono) o comandate da terra (`comandata = true`, con un `gruppo`): in sicurezza finché `m688.comanda_mine` non le passa alla stazione, poi scoppiano sotto un sottomarino che ci passa sopra. L'azione «comanda mine» le passa alla stazione, per gruppo (S18).
- Le chiavi `navigazione` e `tlam` le scrive e le rilegge lo scrittore, ma l'editor non ha ancora le loro voci: oggi le
  usano la libreria (I12) e la scena M2. Nemmeno l'allerta di partenza per parte ha una voce; le zone della radio,
  le regole d'ingaggio della sala radio e l'EMCON per unità invece sì.
- «Lancia missile» tira una salva per regola: una regola «una volta sola» dà una salva sola. Il P-500 non parte sotto i
  20 km; una salva fuori dai limiti del piano va sprecata.
- Il tiro dei missili antinave dell'IA c'è dal 29/10/2026 (capitoli D3 §2.7, E6 §2.13.1): un'unità col missile della sua classe, nell'Enhanced con le forze, tira da sé.
  L'azione «lancia missile» resta per le missioni che vogliono il tiro in un momento preciso.

## 8. Riferimenti

- Codice: `missioni/scenario_missione.hpp`, `missioni/src/scenario_missione.cpp`, `missioni/pacchetto_missione.hpp`,
  `missioni/src/pacchetto_missione.cpp`, `missioni/src/lua_isolato.hpp`, `app/editor_missione.cpp`,
  `app/scelta_missione.cpp`, `app/partita.cpp`, `strumenti/libreria_missioni.cpp`, `strumenti/scene_interazioni.cpp`.
- Dati: `data/missioni/libreria`, `data/missioni/prove`, `data/missioni/API.md`, `data/motore/missioni_rapide.toml`.
- Progetto: `docs/09_schema_missioni.md`, `docs/17_libreria_missioni.md`, `docs/18_interazioni_ia.md`.
- Ricerca: dossier 23 (episodi della Guerra fredda), dossier 24 §8.9 (scene delle interazioni).
- Capitoli collegati: F6 (missioni rapide), F1 (registro e obiettivi), A3 (missioni originali), C11 (radio), E5 (ASW
  aereo), volume 1 cap. 11.
