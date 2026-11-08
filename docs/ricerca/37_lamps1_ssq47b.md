# 37 — L'SH-2F (LAMPS Mk I) e la localizzazione con le SSQ-47B

Disassemblatore, 31/10/2026. L'ha chiesto la principale. Nel motore l'SH-2F non posava mai le sue SSQ-47B: con le sole
SSQ-41B omnidirezionali la traccia restava ferma sulla prima boa che sentiva (errore di 5,6 km, la portata della boa), e
la SSQ-47B entrava solo dopo un fix, come ripiego della DICASS.

**Le fonti.**
- Pagine aperte, più un PDF scaricato il 31/10/2026 con l'autorizzazione dell'utente: R. A. Holler, «The Evolution of the
  Sonobuoy from World War II to the Cold War», U.S. Navy Journal of Underwater Acoustics, JUA_2014_025_N (gennaio 2014;
  distribuzione libera), dal sito navairdevcen.org, 1,9 MB. È la fonte `[HOLLER]` del dossier 17.
- Il rapporto DTIC ADA152004 era autorizzato anche lui, ma il server DTIC rifiuta le richieste automatiche (HTTP 403),
  e non l'ho aggirato. Resta da leggere a mano, se serve.
- Confidenza: **alta** quando le fonti concordano, **media** con una sola fonte, **bassa** quando è debole o in disaccordo.
- Le scelte di gioco sono **STIMA**.

## 1. Che cosa dicono le fonti

| Dato | Fonte | Confidenza |
|---|---|---|
| L'SH-2F portava un misto di SSQ-41A passive e SSQ-47B attive; l'operatore dei sistemi manovrava il lanciaboe | Plane Encyclopedia, Kaman SH-2F; dossier 17 §2.1b `[ACSH2F]` `[GSSH2]` | media |
| La SSQ-47B è attiva, omnidirezionale, di **sola distanza**: un ping CW ogni 10 s a 13-19 kHz, 60 o 800 ft, circa 30 minuti | dossier 17 §4.1 `[HOLLER]`; prc68.com, tabella delle boe | alta |
| Serve a dare una **distanza accurata nella fase finale** della caccia | GlobalSecurity, AN/SSQ-47 (nella sintesi della ricerca; la pagina non si apre più) | media |
| L'elicottero non elaborava i dati delle boe: li passava alla nave col data link **AN/ASK-22**, la nave li elaborava e rimandava i risultati | Plane Encyclopedia; dossier 17 §2.1b | media |
| Il collegamento era a vista: quota tipica di missione intorno ai 700 ft | DTIC ADA152004 (nella sintesi della ricerca, non aperto) | bassa |
| La caccia: verso il rilevamento del contatto della nave, un campo di boe per trovare il sottomarino; il **MAD ASQ-81** per precisare la posizione prima dell'attacco col **Mk 46** | Plane Encyclopedia | media |
| L'SH-2F era «reattivo»: senza sonar a immersione, dipendeva da altri per la prima scoperta | Plane Encyclopedia | media |
| La boa omnidirezionale passiva sente il sottomarino senza distanza né rilevamento; la localizzazione serviva coppie di boe (CODAR) o sorgenti attive | Holler, pp. 332, 342 | alta |
| Il metodo a distanza è l'incrocio dei cerchi di distanza di più boe: in Julie le boe sentivano l'eco di una carica (Fig. 9) | Holler, pp. 332-333 | alta |
| Il precursore della SSQ-47, la SSQ-15 (1961, formato B), era attiva e di sola distanza, CW a 26-38 kHz, con una portata affidabile di **2.500 iarde** (2,3 km) e il Doppler | Holler, p. 334 | alta |
| La SSQ-47 (formato A) sostituì la SSQ-15 a fine anni '60: un impulso CW libero ogni 10 s, su una di sei frequenze fra 13 e 19 kHz. Con le boe passive meno efficaci contro i sottomarini più silenziosi, l'accento passò alla localizzazione attiva | Holler, p. 339 | alta |
| Uno schema di ricerca a quadrato di 4 boe con una al centro, poi uno di inseguimento spostato nella direzione del bersaglio (Fig. 5, epoca della seconda guerra mondiale) | Holler, p. 328 | media per gli anni '80 |

**Cosa non si è trovato:**
- quante SSQ-47B si posavano e con che geometria nel LAMPS I, a che distanza fra loro, e quando l'operatore passava dalle
  passive alle attive;
- la portata della SSQ-47B: Holler dà solo quella della SSQ-15;
- il ritardo del data link ASK-22 fra l'elicottero e la nave: né Holler né le pagine aperte ne parlano.

## 2. Nel gioco (STIMA)

**Il principio:** con la sola distanza una boa dà un cerchio, due boe due punti possibili, tre boe un punto. Il
controllore lo applica ai mezzi **senza boe direzionali** (né passive direzionali né DICASS nel carico): oggi solo l'SH-2F.

1. **Le SSQ-41B che sentono insieme.** Le omni che sentono lo stesso contatto entro 90 s danno una stima: la media delle
   posizioni pesata col segnale (10^(dB/20)). L'errore è la portata della boa divisa per il numero delle boe, almeno
   1.200 m. Con due boe si scende sotto i 2 nm, la soglia dello schema del fix.
2. **Il box delle SSQ-47B.** Lo schema del fix dei mezzi americani è già quello: 70% box di 4 boe attive a 1,5 kyd di
   lato, 30% passaggio MAD. Senza DICASS il box ripiega sulle SSQ-47B: i vertici a 970 m dal centro, dentro la portata
   di 1.500 m.
3. **La trilaterazione.** Le distanze delle SSQ-47B dello stesso pensiero (ogni 5 s) si combinano coi minimi quadrati
   sulle differenze dei cerchi. Con tre o più boe non allineate l'errore è il doppio del residuo più 100 m; sotto i
   914 m il fix è possibile. Con due boe si sceglie l'incrocio più vicino alla traccia, ma l'errore resta almeno 1.200 m.
4. **I ping.** Ogni SSQ-47B pinga ogni 10 s dalla sua attivazione (missione_asw.cpp), non più una sola per aereo.

**Il data link non è modellato:** il controllore elabora da sé quello che nel 1989 elaborava la nave. Per la caccia vista
dal sottomarino non cambia niente; manca solo il ritardo del passaggio dei dati. Nessuna fonte ne dà il valore, quindi
non l'ho proposto.

**Dopo Holler (31/10/2026) le stime restano quelle sopra.** Il principio dei cerchi di distanza e lo schema a quadrato
sono confermati. La portata della SSQ-47B nel gioco (1.500 m, STIMA) è prudente rispetto alle 2.500 iarde della SSQ-15. Le
due boe non sono confrontabili alla pari: frequenza, potenza e formato sono diversi, quindi non la cambio.

## 3. Le missioni toccate

L'SH-2F vola nella S14 «Pedinatore in attesa» e nella S17 «Sotto la prora». Lì ora può arrivare al fix e, in guerra,
all'attacco col Mk 46. Le altre missioni non cambiano: gli altri mezzi hanno boe direzionali.
