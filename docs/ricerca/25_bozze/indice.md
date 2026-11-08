# Le bozze per schermata della rifinitura estetica (roadmap 33)

L'utente ha scelto la combinazione consigliata del dossier 25 §8.6 (28/10/2026, via principale):
- **A «Lamiera di bordo»** per la base;
- **B** per i documenti (esito, cerimonie, fascicolo, bacheca);
- **C «Il quadro di comando»** per gli strumenti.

Qui ci sono le bozze, schermata per schermata, da far approvare all'utente prima del cablaggio. Il cablaggio nell'app è della principale.

L'ordine va dal costo più basso al guadagno più visibile. I riferimenti visivi sono in `../25_riferimenti/` (MobyGames, solo uso interno).

**Vincoli (principale, 28/10):**
- nessun indice della palette usato dal classico o dalla carta si sposta;
- le impronte della regressione restano uguali finché non si cabla;
- le schermate di gioco del classico restano come l'originale: la passata vale per i menu, le schermate del remake e le stazioni dell'Enhanced.

**Le tavole `prima_dopo_*.png`** affiancano l'istantanea di oggi (`../schermate_prima/`) e la bozza, alla risoluzione del gioco. Le fa `tools/estetica/prima_dopo.py`.

**Gruppo 1** (28/10): `sonar/` e `lamiera/`. **Approvato dall'utente il 28/10**, con queste decisioni:
- **sonar:** la disposizione nuova del dossier 09 §4.3 in tutti e due i modi, la scala lunga col Bayer e il fosforo;
- **persistenza:** tre gradi da un tasto della stazione, salvati nelle preferenze (breve 0,80, normale 0,90, lunga 0,95);
- **variante A:** più contrasto per il testo secondario. Ora c'è `lamiera::secondaria`, grigio chiaro inciso con l'ombra nera; la scritta del tasto disabilitato e la riga d'aiuto sono più chiare.

## 1. Le cascate del sonar (`sonar/`)

**Codice:** `engine/grafica/scala_cascata.{hpp,cpp}` (kit), con le prove in `test/prove_scala_cascata.cpp`. Le bozze le fa `engine/strumenti/bozze_sonar.cpp` (`bozze_sonar data/motore <cartella>`) con dati sintetici della forma di quelli veri. Le PNG sono ingrandite 2x.

| Tavola | Che cosa mostra |
|---|---|
| `prima_dopo_moderno.png`, `prima_dopo_1989.png` | Le due stazioni di oggi accanto alle bozze. |
| `livelli_cascata.png` | Lo stesso LOFAR e lo stesso BTR in tre rese: 32 livelli (oggi), 128 livelli, e 128 con il dithering Bayer 4x4. Sotto c'è il fianco del mercantile ingrandito 4 volte, e sotto ancora la rampa intera. |
| `sonar_moderno.png` | La stazione moderna con la disposizione del dossier 09 §4.3 e la scala lunga col dithering (viridis). Le parti sono: BTR a sinistra con i tracker; a destra l'A-scan senza scalini, il LOFAR e il DEMON; in basso i contatti e lo stato. |
| `sonar_1989.png` | Il modo 1989: tutto sul fosforo verde, con la persistenza dell'A-scan (le ultime sei righe che si spengono), il bagliore, la grana ferma e una riga su due più scura. |
| `frange_vere.png` | **Righe vere** del disassemblatore (`tools/re/prove/cascate_esempio/`): un 688 con la TB-16 e un mercantile al CPA a 3 km, sulla Celtica, 20 minuti di LOFAR 10-510 Hz, dB grezzi (S+N)/N da 20 a 45 dB. Senza e con le frange d'interferenza, a 32 livelli e a 128 col Bayer. Qui la differenza è netta: a 32 livelli le frange diventano curve di livello. |
| `frange_fosforo.png` | Le stesse frange nel modo 1989: 32 gradini senza effetti contro il fosforo, e il dettaglio ingrandito 4 volte. |
| `fosforo_fondo.png` | **Proposta per il 1989 scuro** (principale, 28/10): le righe vere normalizzate a finestra divisa come la stazione, fra +2 e +16 dB. Oggi sotto il pavimento è nero; col velo di grana al 7% (`cascate::Fosforo::Effetti::fondo = 0.07`) lo schermo è acceso come un BQQ-5, e le righe e la grana si vedono anche dove non c'è segnale. Il predefinito resta 0. |
| `fosforo_dettaglio.png` | L'A-scan del 1989 ingrandito 4 volte in tre passi: un punto per colonna come oggi, poi persistenza e traccia senza scalini, poi bagliore, grana e righe del CRT. |

**Che cosa si vede, onestamente:**
- **Sui dati rumorosi** (il pavimento del BTR e del LOFAR) la differenza fra 32 e 128 livelli è piccola, perché il rumore copre i gradini.
- **Sulle frange d'interferenza vere** (3-10 dB, lisce) la differenza è netta: a 32 livelli si vedono curve di livello, a 128 col dithering una sfumatura.
- **Sulle rampe lisce** si vede: la barra della scala, i fianchi larghi di un contatto, il pavimento che cala con la frequenza. A 32 livelli le fasce si contano; a 128 col dithering la rampa è continua.
- **Il guadagno più grande** è nel modo 1989, che oggi è una palette verde senza effetti e adesso sembra un altro schermo. Così le due stazioni si distinguono a colpo d'occhio (dossier 25 §7, problema 7).

**Le scelte proposte:**
- **Palette:** 128 indici, 96-223, quando la stazione copre la vista, come fa già la carta; nel riquadro e nel Museo restano i 32 di oggi (200-231).
- **Dithering:** ordinato Bayer 4x4 sulla parte frazionaria del livello. Rende la frazione a 1/16 sulla cella, ed è fermo: non tremola fra un fotogramma e l'altro.
- **Fosforo:**
  - persistenza `v = max(nuovo, vecchio × 0,90)` a fotogramma (STIMA);
  - traccia di Xiaolin Wu su un'intensità per pixel;
  - alone 3×3 dai vicini più accesi, al 35%;
  - grana ferma ±1,75%;
  - righe dispari all'85%.
- **Tracker:** con le convenzioni dei simboli della carta, cioè ottagono giallo per l'ignoto e rombo rosa per l'ostile, ma con gli indici fissi 0-15. La scala lunga copre infatti gli indici dei simboli della carta (212-231).
- **Minuti del BTR:** tacche fuori dai dati, non righe sopra.

**Per il cablaggio (principale), quando la bozza è approvata:**
- **La palette e il pixel.** `SonarEnhanced::applica_palette` passa a `cascate::applica(p, tinta, SCALA_LUNGA)` quando la stazione copre la vista. `SonarEnhanced::colore(db)` passa a `cascate::indice(cascate::frazione(db, pavimento, tetto), x, y)`.
- **Le tinte.** Sono le stesse di oggi, spostate in `grafica/scala_cascata.cpp`. Le copie in `app/sonar_enhanced.cpp` si possono togliere.
- **Il modo 1989.** Serve un `cascate::Fosforo` per riquadro (BTR, LOFAR, A-scan, DEMON), che vive con la stazione: `decadi(0,9)` a ogni fotogramma e `componi` al disegno.
- **La pipeline dB → colore.** Va concordata col disassemblatore (nota sonar-confronto-27-10.md): lui dà i dB per pixel, pavimento e tetto, e questo modulo fa il resto.

**Da decidere con l'utente:**
- se la disposizione nuova (A-scan sopra il LOFAR, DEMON sotto, contatti in basso a sinistra) va bene, o se si tiene quella di oggi (cascata a sinistra, LOFAR a destra) con le sole scale nuove;
- il valore della persistenza (0,85 traccia corta, 0,95 lunga scia).

## 2. I pezzi della variante A e il piede comune (`lamiera/`)

**Codice:** `engine/grafica/lamiera.{hpp,cpp}` (kit), con le prove in `test/prove_lamiera.cpp`. La tavola la fa `engine/strumenti/bozze_estetica.cpp`, che ora usa questi pezzi anche per le tavole A del dossier.

| Tavola | Che cosa mostra |
|---|---|
| `prima_dopo_scelta.png`, `prima_dopo_regolazioni.png` | La scelta della missione e le regolazioni della sandbox di oggi accanto alle stesse coi pezzi di A e il piede comune. |
| `pezzi_A.png` | Ogni pezzo da solo: lamiera con la grana, pannello a viti, targa rivettata, vetro nero; i tasti a spia nei cinque stati (normale, evidenziato, premuto, disabilitato, predefinito col filo d'oro); i tre livelli di testo e i colori dei valori (grigio di serie, ciano cambiato, giallo la riga scelta); le spie per gruppo, la marina nel titolo, le parti NTDS. In basso, il piede comune. |

**Le scelte:**
- **Palette:** gli indici del tema delle stazioni (16-47) e l'oro della cerimonia (53-60), cioè quelli delle consolle già fatte. Il «blocco di 48 posti» del §8.2 c'è già: non servono indici nuovi.
- **Il piede:** 50 pixel. La riga d'aiuto grigia su una fascia, poi al più quattro tasti su tutta la larghezza. Indietro a sinistra, avanti a destra col filo d'oro.

**Per il cablaggio (principale):** le schermate di contorno chiamano `lamiera::piede` al posto dei loro piedi e `lamiera::pannello`/`vetro`/`targa` al posto delle cornici di oggi. Le prossime bozze, una schermata alla volta, le mostrano già montate.

## 3. Gli strumenti nella variante C (`strumenti/`), gruppo 2

**Codice:** `engine/grafica/quadro.{hpp,cpp}` (kit), con le prove in `test/prove_quadro.cpp`. I pezzi sono la consolle con la targa del posto, lo schermo d'analisi senza il cappuccio dei CRT delle stazioni, le linguette, le righe e le intestazioni. Il piede è quello comune di `grafica/lamiera`. Le tavole le fa `engine/strumenti/bozze_strumenti.cpp`; `tools/estetica/bozze_strumenti.py` incolla la carta del replay dall'istantanea di oggi. I numeri sono quelli delle istantanee, così il confronto è sugli stessi dati.

| Tavola | Che cosa mostra |
|---|---|
| `prima_dopo_statistiche_riepilogo.png` | La pagina 1 delle statistiche. Oggi è testo nudo su nero, con metà schermo vuota. Nella bozza ci sono due schermi: a sinistra il riepilogo con le righe alterne velate, a destra gli stessi numeri in figura (scoperte fatte e subite, lanci ed esche, tempi del Patto in barre). |
| `prima_dopo_statistiche_scoperte.png` | La pagina 2: la tabella larga delle scoperte in uno schermo solo, con la riga scelta accesa (il clic apre il replay da lì) e una riga che spiega i tempi. |
| `prima_dopo_replay.png` | Il replay con la carta grande a tutto schermo (scelta dell'utente del 28/10/2026): la legenda coi segni della carta in un riquadro in basso a sinistra, la lettura (eventi, quota nostra e del bersaglio) in un riquadro in alto a destra, la linea del tempo come fascia sottile sotto la carta; sotto, il piede comune. |

**Cosa si perde e cosa si guadagna:**
- **La carta del replay** (decisione dell'utente del 28/10/2026): grande, 618×358 contro 468×360 di oggi, con la linea del tempo come fascia sottile (`quadro::fascia_tempo`) e la legenda e la lettura in due riquadri sopra la carta (`quadro::riquadro`). Nella bozza `bozze_strumenti.py` rimette i riquadri sopra la carta incollata (le righe «SOPRA» del programma); nel gioco si disegnano dopo la carta.
- **I simboli della verità.** L'istantanea della lettura «verità» usa ancora i simboli vecchi (i triangoli degli ostili): il cablaggio di `grafica/simboli_carta` nel replay li allinea.

## 4. Briefing, debriefing e serie, gruppo 3

**Stato (28/10/2026):** agganciato dalla principale (`app/briefing.cpp`, `app/debriefing.cpp`,
`app/schermata_serie.cpp`); istantanee in `engine/build-principale/schermate/` (brief1-2, deb1-3, serie1-3). Rispetto
alle tavole: gli ordini a pagine con «(SEGUE) n/m» o «(FINE)», il timbro dalla classifica del messaggio, i livelli
senza dati spenti; le pagine degli eventi del debriefing sul foglio (della principale); la tabella della serie che
scorre con la sintesi fissa in fondo. Manuale: volume 1 §1, §10.2, §12.8; F1 §2.5-2.6, E9 §2.9.4, F8 §2.6.

**La lista** del kit, in ordine di guadagno visibile:
1. **briefing:** si vede a ogni missione ed è un muro di testo verde su nero;
2. **debriefing:** si vede a ogni missione, oggi è testo nudo accanto alla carta;
3. **serie della sandbox:** tre pagine di testo nudo, la seconda è la più densa.

Seguono, per un gruppo 4: museo (muro di testo), statistiche cumulate e albo vuoto (stati vuoti), editor (otto
schermate, il più lungo).

**Codice:** le tavole sono in `engine/strumenti/bozze_strumenti.cpp` (`briefing`, `debriefing`, `serie`). Lo script
`tools/estetica/bozze_strumenti.py` incolla la carta del debriefing dalla sua istantanea. Briefing e debriefing usano
solo gli indici 0-47 (lamiera, lampade, il bianco e il grigio per la carta): la carta del teatro tiene i suoi colori,
e ai documenti non serve la palette della cerimonia.

| Tavola | Che cosa mostra |
|---|---|
| `documenti/prima_dopo_briefing.png` | Il briefing nella lamiera di A coi documenti di B. A sinistra gli ordini battuti dalla telescrivente su carta a modulo continuo: fasce grigie ogni tre righe, i fori di trascinamento, il timbro CONFIDENTIAL, «(SEGUE) 1/2» per le pagine. A destra i livelli 1-8 come spie verdi accese e spente, e la scheda della missione nei vetri: battello, teatro e radio, ROE in giallo, meteo, codice. Il piede comune con INVIO IN MARE predefinito. |
| `documenti/prima_dopo_debriefing.png` | Il debriefing: la carta incassata nel pannello, più grande di oggi; a destra il rapporto dattiloscritto su un foglio bianco con l'ombra. Il rapporto ha il timbro rosso dell'esito, le voci coi puntini e i valori «a penna» in blu, gli obiettivi con le caselle. La riga d'aiuto ha gli otto comandi di oggi; i tasti sono quattro (ESC CHIUDI, R RIPRENDI spento quando non si può, P REPLAY, E ESITO predefinito). |
| `strumenti/prima_dopo_serie.png` | La serie, pagina 2, nella consolle di C come le statistiche: le righe alterne, per parte la media e lo scarto, la barra (scatola media ± scarto, baffi dal minimo al massimo, media bianca) sulla stessa scala per NATO e Patto, l'intervallo. In fondo la sintesi della pagina 1 (esiti, PK, codice), così lo spazio vuoto di oggi serve. |

**Decisioni dell'utente (28/10/2026):**
- **Briefing:** gli ordini restano sopra la carta, come oggi, e TAB li chiude.
- **Debriefing:** quattro tasti più la riga d'aiuto.
- **Serie:** approvata dall'utente la sintesi della pagina 1 in fondo alla pagina 2.

**I pezzi per il cablaggio** (kit; l'aggancio è della principale):
- in `grafica/lamiera`: `a_capo`, `testata_classificata`, `modulo_continuo`, `foglio`, `timbro`, `voce_modulo`,
  `casella`, `spia_voce`, `campo_vetro` (tutti negli indici 0-47);
- in `grafica/quadro`: `barra_statistica`;
- prove in `test/prove_lamiera.cpp` e `test/prove_quadro.cpp`. Le tavole li usano già (`bozze_strumenti.cpp`).

## 5. Museo, statistiche globali, albo vuoto ed editor, gruppo 4

**Stato (29/10/2026):** bozze da approvare. Come per il gruppo 3: A per la base, B per i documenti, C per gli
strumenti. Le tavole le fa `engine/strumenti/bozze_strumenti.cpp` (`museo`, `statistiche_cumulate`, `albo`, `editor`);
`tools/estetica/bozze_strumenti.py` incolla l'immagine del museo e la carta dell'editor dalle istantanee di oggi, e
`tools/estetica/prima_dopo.py` fa le tavole affiancate. I testi e i numeri sono quelli delle istantanee.

| Tavola | Che cosa mostra |
|---|---|
| `documenti/prima_dopo_museo.png` | Il museo nella lamiera di A coi documenti di B. A sinistra lo schedario: la sezione fra due tasti a freccia, l'indice nel vetro con la voce scelta sulla lampada ambra. A destra il nome inciso, l'immagine nel vetro con TAB e «1/8», la scheda tecnica battuta a macchina su un cartellino (valori a penna), il testo su carta: la storia dattiloscritta, «IN GIOCO» a penna blu col segno a margine, i rimandi numerati in rosso, «(SEGUE) 1/2». Il piede comune con PAG predefinito. |
| `strumenti/prima_dopo_statistiche_cumulate.png` | Le statistiche globali senza missioni, nella consolle di C. A sinistra i gruppi di oggi (totali, medie, rilevamenti, primati) con le righe alterne e un trattino spento al posto degli zeri; in fondo «I NUMERI ARRIVANO DOPO IL PRIMO DEBRIEFING». A destra la griglia spenta delle celle e lo stato vuoto: «NESSUNA MISSIONE ANCORA», come si riempie la carta, TAB per gli altri teatri; sotto la scala spenta e «NESSUN LANCIO». |
| `documenti/prima_dopo_albo.png` | L'albo vuoto, nei colori di cerimonia. Il cartellino della carriera nuova è di carta: il posto della foto tratteggiato con la croce a penna, sotto le quattro scelte del modulo. Gli altri sette posti sono fili tratteggiati numerati 2-8, così si vede quanti comandanti tiene l'albo. Il piede di cerimonia: legno e targhette d'ottone, la stessa geometria del piede comune, INVIO NUOVA CARRIERA predefinito. |
| `strumenti/prima_dopo_editor.png` | L'editor, scheda 1, nella consolle di C («SALA PIANI»). La carta nello schermo grande a sinistra; a destra le otto schede come linguette su due righe, i campi in tre gruppi (la missione, luogo e tempo, regole) con la riga scelta accesa e il cursore, la nota sulla fine della missione, e in fondo la verifica sempre in vista con la sua spia. Il piede comune: ESC, S SALVA, E ESPORTA, G PROVA IN MARE predefinito. |

**I pezzi nuovi** (in `grafica` dal 29/10/2026, dopo l'approvazione):
- `lamiera::elenco_vetro` (A): l'elenco nel vetro con la scelta sulla lampada ambra e la tacca dello scorrimento, che
  si sposta con `primo`. Serve all'indice del museo;
- `lamiera::in_colori_vista` (A): rimappa i pixel del tema (16-47) di un rettangolo sul colore più vicino fra gli EGA
  0-15 e i grigi degli sprite 144-158; le lampade solo sugli EGA colorati, così restano rosse, ambra e verdi. Si
  chiama dopo la cornice e prima dell'immagine; poi la schermata non applica il tema. È la strada scelta per il museo;
- `quadro::stato_vuoto` (C): il riquadro tratteggiato al centro di uno schermo, col titolo e le righe che dicono come
  si riempie;
- `posto_libero` e `piede_cerimonia` (B, in `grafica/cornici`): il posto tratteggiato e il piede di legno e ottone
  delle schermate di cerimonia. Usano solo le rampe di `cerimonia.hpp` (legno 207-214, oro 53-60, bianco 48-52,
  inchiostro 40-47).

**Le prove** sono in `test/prove_lamiera.cpp` («l'elenco nel vetro scorre, e nei colori della vista non resta niente
del tema», «il piede di cerimonia e il posto libero usano solo le rampe di cerimonia») e `test/prove_quadro.cpp` («lo
stato vuoto sta nel suo rettangolo»). Finché il runtime tiene i file delle missioni, `prove` non si ricompila: le due
prove sono girate in un eseguibile a parte (il main di doctest, i due file e `libgrafica`), 9 casi su 9.

**Le tavole in più** (29/10/2026): `strumenti/prima_dopo_editor_unita.png` (scheda 2: la prossima unità, l'elenco
sulla carta con la scelta, i campi dell'unità scelta) e `strumenti/prima_dopo_editor_briefing.png` (scheda 7: le
righe numerate con la scelta e il cursore, i comandi come tasti, la vista che gira fra italiano, inglese e radio).
Le altre cinque schede seguono la 1 (campi) o la 2 (elenco più campi). Il museo ora è nei colori della vista: la
lamiera è grigia neutra e le lenti dei tasti sono gli EGA, un po' più vivi di quelli del tema.

**I vincoli di palette per il cablaggio:**
- **L'albo** (e fascicolo, bacheca, esito) ha i ritratti negli indici 16-47, gli stessi del tema delle stazioni: lì
  la lamiera e il piede comune ridipingerebbero i volti. Per questo il piede di cerimonia: stessa geometria, altre
  rampe.
- **Il museo** mostra sprite, modelli e foto con la palette della vista, che usa anche 16-79. La bozza incolla
  l'immagine in RGB e non lo vede; nel gioco la lamiera di A (16-47) la stingerebbe. Due strade: la lamiera del museo
  nei grigi 144-158 e negli EGA 0-15 (come le cornici in modo «vista» del debriefing con le minicartoline), oppure
  l'immagine disegnata in una rampa propria. Va scelto al cablaggio; la prima non chiede indici nuovi.
- **Le statistiche globali** tengono la carta (96-143, 199-231), il rilievo (48-79) e il calore (232-247): col tema
  16-47 non si toccano.
- **L'editor** tiene carta e rilievo: idem.

**Le domande per l'utente:**
1. **Museo:** va bene lo schedario a sinistra e la voce come un cartellino più un foglio, con «IN GIOCO» a penna blu?
   Il tasto predefinito è PAG (il testo che segue).
2. **Albo:** va bene il piede di legno e ottone al posto di quello grigio comune, per non toccare i ritratti? I
   cartellini scendono da 196 a 182 pixel d'altezza per fargli posto.
3. **Statistiche globali:** i trattini spenti al posto degli zeri, e la spiegazione sulla carta vuota?
4. **Editor:**
   - i campi raggruppati in tre gruppi;
   - una riga DATA al posto delle tre righe GIORNO, MESE e ANNO di oggi (la modifica del valore resta per parti);
   - basta la tavola della scheda 1 come modello per tutte e otto, o servono le altre sette?

**Decisioni dell'utente (29/10/2026):** approvate tutte e quattro, con le raccomandazioni del kit.
- **Museo:** la lamiera nei colori della vista (grigi 144-158 ed EGA 0-15), senza indici nuovi: l'immagine resta
  com'è. PAG è il predefinito solo se il testo continua; se la voce sta in una pagina, il filo d'oro va su ESC.
- **Albo:** il piede di legno e ottone, poi anche per fascicolo, bacheca ed esito. I cartellini a 182 pixel vanno
  provati con un comandante vero; se foto, righe e timbro si accavallano, il timbro sale o si toglie una riga.
- **Statistiche globali:** i trattini solo con zero missioni; con missioni giocate uno zero si scrive.
- **Editor:** i campi in gruppi; DATA in una riga sola se la modifica resta per parti (giorno, mese, anno), se no le
  tre righe nel gruppo «luogo e tempo». Prima del cablaggio, le tavole delle schede 2 (unità) e 7 (briefing).
- **L'ordine di cablaggio suggerito:** statistiche globali, albo, museo, editor.

## 6. Le stazioni dell'Enhanced, gruppo 5

**Stato (29/10/2026):** bozze da approvare. Le stazioni hanno due parti: sopra la vista (640×360), sotto il pannello
delle consolle di `grafica/sale` (360-480). Le consolle grigie del 688 e beige dell'Alfa sono già curate e restano.
Il problema è la parte di sopra, quando non è la vista del periscopio, e il pannello della carta. Ogni tavola
ridisegna una delle due parti e incolla l'altra dall'istantanea di oggi. Le fa `bozze_strumenti.cpp` (`radio`,
`manovra`, `carta_pannello`). Le tavole sono solo del 688; l'Alfa seguirebbe con i suoi materiali.

| Tavola | Che cosa mostra |
|---|---|
| `stazioni/prima_dopo_radio.png` | La sala radio (F2), sopra, in A coi documenti di B. In alto una fascia di stato: albero, filo e bande come spie verdi; le ROE come cinque spie, rosse se trattenute e verdi se libere; la prossima finestra in un vetro con la barra del tempo; la sessione. A sinistra il messaggio scelto sulla telescrivente, col timbro della classifica e «(SEGUE) 1/3». A destra «in onda» e «in programma» nei vetri, e i messaggi ricevuti in un elenco. Il pannello di sotto resta com'è. |
| `stazioni/prima_dopo_manovra.png` | La manovra (F4) col periscopio abbassato, in C. Oggi sopra c'è la vista del periscopio sott'acqua, che sembra un errore. La bozza mette un ripetitore di manovra: a sinistra il profilo di quota degli ultimi 10 minuti, con la quota periscopica, lo strato e il fondo dall'ecoscandaglio; a destra l'ordinato contro l'attuale (rotta, velocità, quota, timone, piani, macchina, zavorra) e l'acqua sotto la chiglia. Col periscopio su torna la vista. I numeri della tavola sono d'esempio. |
| `stazioni/prima_dopo_carta.png` | La carta (F6), il pannello di sotto, in A. Oggi è una riga di stato di solo testo, un campo nero e una riga di comandi lunga e fitta. Nella bozza pilota, proiezione e scie sono spie; punti di rotta e scala stanno in vetri; il bersaglio ha il suo vetro. I comandi 4-9, Q, E, F, B, T sono tasti della tastiera disegnati con la parola accanto, su due file. La riga dell'equipaggio resta in fondo. |

**Pezzi:** nessuno nuovo. Bastano quelli dei gruppi 1-4: `lamiera` (pannello, vetro, spia_voce, campo_vetro,
elenco_vetro, tasto_tastiera, modulo_continuo, timbro) e `quadro` (schermo, intestazione, riga).

**Vincoli per il cablaggio:**
- **La parte di sopra della radio e della manovra** copre la vista solo quando la stazione non la mostra. Il tema
  16-47 è già applicato nelle stazioni, quindi lamiera e fosforo non chiedono indici nuovi. La vista del periscopio
  usa anche 16-79, ma qui non c'è.
- **Il profilo della manovra** vuole la storia della quota (dalla traccia del replay o da un anello in memoria), lo
  strato e il fondo misurato: il quadro del fondale ha già i sondaggi.
- **L'Alfa:** le stesse disposizioni, coi colori beige della sua consolle e i nomi dei suoi pulti. Va visto a parte.

**Le domande per l'utente:**
1. **Radio:** va bene il messaggio sulla telescrivente, con lo stato in spie e vetri al posto del testo fitto?
2. **Manovra:** col periscopio giù, il ripetitore di manovra al posto della vista sott'acqua? Oppure si tiene la vista
   del 1989, con una scritta chiara «PERISCOPIO GIU'» sopra? Lo stesso vale per la stazione Armi (F7): lì il
   ripetitore mostrerebbe i tubi e le armi in acqua.
3. **Carta:** va bene il pannello con le spie e i tasti disegnati, o si preferisce la riga di testo di oggi, più
   corta?
4. **L'Alfa:** si fanno anche le tavole dell'Alfa prima di cablare, o si cabla il 688 e l'Alfa dopo?

**Dopo l'approvazione (kit, 29/10/2026 sera):**
- **Armi:** la bozza `stazioni/prima_dopo_armi.png` (`armi` in `bozze_strumenti.cpp`), col ripetitore a periscopio
  giù: le armi in acqua e l'arma scelta, i tubi, il carico e la ricarica.
- **Manovra:** il profilo ora è il pezzo `quadro::profilo_quota`, con la prova in `test/prove_quadro.cpp`.
- **Le misure per il cablaggio:** nella nota `gruppo5-firme-e-misure.md` delle sessioni.
- **Le tavole dell'Alfa, da approvare:** `stazioni/prima_dopo_<radio|manovra|armi|carta>_alfa.png`, con le stesse
  disposizioni:
  - **i colori:** il tema dell'Alfa (smalto beige, fosforo verde giallino);
  - **le scritte:** il titolo inciso a stampino (`stampino_urss_7`); le scritte sulla lamiera scure e piatte, come le
    targhette delle sue consolle;
  - **i testi:** del battello sovietico (RADIORUBKA, SOV. SEKRETNO, KP SF, SLEZHENIE, il Sozh);
  - **le armi:** quelle del suo carico (`carico.toml`): cinque tubi più il lanciatore delle esche; SET-65 e 53-65K,
    senza filo; l'esca MG-74.

  **Approvate dall'utente (29/10/2026 sera)**, con le etichette scure anche dentro i pezzi: ora i pezzi prendono
  `lamiera::Scritte`.

**Decisioni dell'utente (29/10/2026):** approvate tutte e tre le bozze: radio, manovra e armi col ripetitore a
periscopio giù, pannello della carta. Si cabla prima il 688; le tavole dell'Alfa dopo, da approvare a parte.

## 7. Il piede della cerimonia: fascicolo, bacheca ed esito

**Stato (29/10/2026 sera):** bozze da approvare, proposta del kit (c). L'albo ha già il piede di legno e ottone
(`piede_cerimonia`, gruppo 4). Le altre tre schermate della cerimonia hanno ancora tre piedi diversi:
- **il fascicolo:** una riga d'aiuto in fondo;
- **la bacheca:** una riga d'aiuto centrata;
- **l'esito:** due pulsanti grigi.

La proposta dà a tutte e tre lo stesso piede dell'albo, `piede_cerimonia(t, f, 428, aiuto, tasti, predefinito)`.

| Tavola | Che cosa mostra |
|---|---|
| `cerimonia/prima_dopo_fascicolo_piede.png` | Il fascicolo. Nella riga d'aiuto la pagina («PAGINA 1 DI 5: LA COPERTINA») e SU/GIU' SCORRE. I tasti: ESC INDIETRO, P REPLAY (spento fuori dalle note), SINISTRA PRIMA, DESTRA DOPO (il predefinito). Sopra c'è l'anteprima del kit (`anteprima_cerimonia fascicolo`), perché su questa macchina non ci sono carriere. |
| `cerimonia/prima_dopo_bacheca_piede.png` | La bacheca. Nella riga d'aiuto 1-7, Tab e le frecce. I tasti: ESC INDIETRO, G COMANDANTE (spento senza un comandante in carica), TAB SCHEDA. Nessun predefinito. |
| `cerimonia/prima_dopo_esito_piede.png` | L'esito. Nella riga d'aiuto la pagina. I due pulsanti grigi diventano i tasti d'ottone ESC DEBRIEFING (ESC INDIETRO dalla seconda pagina) e INVIO CONTINUA (INVIO FINE all'ultima), il predefinito. |

**Il vincolo per il cablaggio:** il piede occupa le righe da 428 al fondo dello schermo, quindi quello che sta sopra
deve finire a 424.
- **Fascicolo:** la carta `{32, 8, 576, 438}` diventa `{32, 8, 576, 416}`.
- **Esito:** il rapporto finisce a 424.
- **Bacheca:** la scheda del medaglione sale di 14 px.

Nelle tavole l'istantanea è tagliata a 428, quindi la carta sembra passare sotto il piede: nel gioco va accorciata.

**Pezzi:** nessuno nuovo. **Le domande per l'utente:** vanno bene i tre piedi uguali a quello dell'albo? Nella
bacheca, meglio il tasto G spento senza comandante o niente tasto?

## 8. La carta F6: la carta pulita e la carta grande

**Stato (29/10/2026 sera):** bozze da approvare, proposte A e B del kit scelte dall'utente; la C (più fasce e la terra a
tinte, palette nuova) resta per dopo. La carta VGA/SVGA c'è già (N1, 24/10): queste sono rifiniture di leggibilità.
Le tavole le disegna il codice vero, `strumenti/bozze_carta.cpp` (con `tools/estetica/bozze_carta.py`). È lo stesso
tratto di SCENE1A, col fondale di `grafica/carta_vga`, lo strato meteo, la mobilia e i toponimi. Sotto c'è il
pannello nuovo della carta (gruppo 5, approvato). Contatti, rotte e zone li disegna la stazione sopra: qui mancano in
tutte e tre.

| Tavola | Che cosa mostra |
|---|---|
| `carta/prima_dopo_carta_pulita.png` | **A, la carta pulita.** Il retino delle zone non rilevate passa da 6 a 12 px (`OpzioniCarta::passo_tratteggio`). Lo strato meteo ha una barba ogni 192 px invece che ogni 80, in grigio, senza la cifra del mare. Il vento e il mare del bollettino stanno una volta sola in una legenda in alto a destra, coi colori del cartiglio, e i toponimi la evitano. |
| `carta/prima_dopo_carta_grande.png` | **B, la carta grande.** Come A, senza la striscia chiara in alto (la riga dell'ingrandimento e quella della carta). La carta prende anche quelle 30 righe; il titolo e l'edizione restano nel cartiglio, lo stato nel pannello. |

**Per il cablaggio** (in `app/carta.cpp`, della principale):
- **il retino:** `o.passo_tratteggio = 12` nelle opzioni del fondale;
- **lo strato meteo:** in `disegna_meteo_carta` il passo da 80 a 192, il colore da `MATITA + 5` a `RETICOLO`, senza
  `stato_mare` sulla griglia;
- **la legenda:** la disegna `bozze_carta.cpp` (110 × 34 px circa, in alto a destra dentro la cornice);
- **la striscia (solo B):** non si scrive più; `riserva_alto` della mobilia a 0. L'ingrandimento, la proiezione e
  l'errore di posizione vanno nel pannello: l'ingrandimento c'è già nel vetro della SCALA.

**Restano fuori:** le sovrapposizioni fra gli scandagli e le scritte che la stazione mette sopra la carta, come i
minuti di un arco tratteggiato o «ED (1973)». Le scritte sono della stazione, quindi è da vedere in `app/carta.cpp`.

**Decisioni dell'utente (29/10/2026):** il piede comune per fascicolo, bacheca ed esito, col tasto G della bacheca spento ma visibile senza comandante (§7); per la carta la bozza B, «carta più grande» (§8). Cablaggio della principale.

## 9. La stazione Sonar (F8) in stile cascate SDR

**Stato (29/10/2026):** bozze da approvare, proposta e del kit. Ricerca in `docs/ricerca/30_sonar_grafica_sdr.md`
(SDR moderni, consolle del 1989, studi sulla leggibilità), sopra il `09` §4; dati dalla nota del disassemblatore
`sonar-dati-per-la-grafica.md`.

La stazione di oggi funziona, ma si legge male:
- i fasci ciechi sono un blocco blu pieno;
- la cascata è quasi vuota: il fondo di rumore resta scuro ma sporco;
- le etichette del LOFAR si accavallano;
- la parte di sotto è tutta testo.

Le bozze le disegna `strumenti/bozze_sonar_sdr.cpp` (con `tools/estetica/bozze_sonar_sdr.py`), con dati sintetici
della forma di quelli veri:
- **BTR:** 72 fasci da 5°, tre contatti, un banco biologico, i ciechi della sfera a poppa;
- **LOFAR:** 10-410 Hz, con le righe di rete, pompa, elica e turbina e quelle proprie;
- **DEMON:** asse 4,2 Hz, pale 29,4 Hz.

Il «prima» viene dal gioco di oggi (SCENE1A a 200 s, stazione 8); il pannello di sotto resta quello di oggi.

| Tavola | Che cosa mostra |
|---|---|
| `sonar_sdr/prima_dopo_A_<modo>.png` | **A, il quadro SDR.** Il BTR a sinistra, con la scala a tacche (ogni 5°, lunga ogni 10, numero ogni 30). I tracker sono etichette sopra la scala, a righe se si toccano, con la linea a puntini fino alla traccia. Il fascio puntato ha una barretta sulla scala, i bordi a puntini e un gradino in più di luce. I ciechi sono a tratteggio, l'ora sta a sinistra. A destra l'A-scan con il picco e i cerchietti sulle righe trovate (moderno), il band plan delle righe attese colorate per tipo (moderno; nel 1989 solo le tacche 50, 60 e 400 Hz), il LOFAR, la scala in Hz, la barra dello zoom e il DEMON piccolo. Sotto la tabella dei contatti col designato e la riga di stato. |
| `sonar_sdr/prima_dopo_B_<modo>.png` | **B, la consolle BQQ-5.** Gli stessi pezzi su due CRT incassati (BTR; LOFAR sopra il DEMON) e una colonna di tasti quadrati a luce (BTR, LOF, DEM, INT, AUTO, CURS, TRK, ANAL, TMA). Sotto, i contatti in un vetro. È la proposta più «1989». |
| `sonar_sdr/prima_dopo_C_<modo>.png` | **C, l'analizzatore.** Il LOFAR del fascio a tutta larghezza con lo zoom (×4, 20-120 Hz); le righe della libreria sovrapposte a puntini, colorate per tipo; il cursore e il delta (50 e 100 Hz: armonica 2); il DEMON; gli otto slot con le anteprime. |

**Le regole comuni,** dalla ricerca:
- il pavimento a +2 dB, così il rumore cade sul nero (GQRX);
- 16 livelli di fosforo senza dithering nel 1989, perché contano i livelli, non il contrasto (NSMRL 1078). Nel moderno
  viridis a 128 col dithering;
- sopra la cascata solo sovrapposizioni leggere: puntini, tratteggi, testo con l'ombra. Il colore indica una
  categoria (classificazione, tipo di riga), non un valore;
- la media solo sull'A-scan; la cascata resta grezza;
- il blu solo nella cornice della sala.

**Pezzi:** nessuno nuovo in `grafica`. Le cascate usano `cascate::frazione` e `indice` sulla scala lunga (96-223);
le sovrapposizioni gli EGA 0-15; la consolle B `crt_rettangolare` e `lamiera::tasto`.

**Le domande per l'utente:**
1. Quale disposizione: A, B, o A nel moderno e B nel 1989?
2. L'analizzatore C come pagina a parte (oggi il tasto 4)?
3. Nel moderno, il colore per rilevamento sul LOFAR come aiuto (+1,2 dB nello studio DSTO), da spegnere?

**Decisioni dell'utente (29/10/2026):** A nel modo moderno e B nel 1989; l'analizzatore C come pagina a parte sul tasto 4; nel moderno il colore per rilevamento sul LOFAR come aiuto spegnibile (acceso di serie, conta nel realismo del punteggio). Il cablaggio in app/sonar_enhanced lo fa il kit.

**Cablato il 29/10/2026** (manuale C5 §2.8). Le istantanee del gioco sono `sonar_sdr/gioco_{moderno,1989,analisi,analisi_1989}.png`. Da allora il «prima» delle tavole A, B e C è la stazione cablata.

### 9.1 L'Okean dell'Alfa (bozza D, da approvare)

**Il problema.** Sull'Alfa la stazione è Okean: lo schermo tondo di Enisej e accanto il LOFAR del 688 (`prima_alfa_<modo>.png`, SCENE2B a 200 s). Le regole della ricerca valgono anche qui, e oggi non sono rispettate:
- il ventaglio disegna ogni raggio lungo almeno 0,15 del raggio: il rumore fa un disco al centro invece di cadere sul nero;
- la linea piena del fascio copre i dati, e il bordo non ha la scala;
- il cieco di poppa è solo vuoto;
- il LOFAR ha i titoli e i colori del 688, non il fosforo verde giallino dell'Alfa (dossier 19 §8);
- la legenda RIL tocca i numeri della scala.

**La proposta** (`sonar_sdr/prima_dopo_D_<modo>.png`):

| Elemento | Com'è nella bozza |
|---|---|
| **Enisej** | Una cascata polare: l'angolo è il rilevamento relativo, con la prua in alto; il raggio è il tempo, l'ultima riga sul bordo e le vecchie verso il centro (una riga ogni 2 s, cerchi a puntini ogni minuto). È la storia lunga che l'occhio integra (ricerca 30 §3.3). Il pavimento è a +2 dB. |
| **Il cieco di poppa** | Oltre ±120°, a tratteggio tenue. |
| **La scala** | Sul bordo: una tacca ogni 5°, lunga ogni 10°, il numero ogni 30°, con L a sinistra e P a dritta (levyj e pravyj bort); 180 in basso. |
| **Il fascio** | Due parentesi sul bordo e i lati a puntini fino al centro, con un gradino in più di luce dentro. |
| **I tracker** | Una tacca dentro il bordo e il nome con l'ombra nera, su due raggi se due nomi si toccano. |
| **Lo schermo rettangolare** | Il piccolo schermo sotto la cappa nera (non la cornice col cappuccio del 688). Contiene SPEKTR (A-scan, band plan, cascata, scala) e DEMOD. |
| **Le scritte** | Nello stampino traslitterato (GC per Gc, PEL. per peleng, KLASS). |
| **Sotto** | I contatti nei rilevamenti relativi (L035, P072). |

**I due modi:**
- **1989:** il fosforo verde giallino a 16 livelli, una rampa dal nero a circa 550 nm (STIMA, scritta nello strumento);
- **moderno:** viridis, i nomi colorati per classificazione, i cerchietti sui picchi e il band plan a colori.

**Le domande per l'utente:**
1. Enisej come cascata polare (il tempo nel raggio) o come ventaglio dell'ultima riga, ma senza il disco del rumore?
2. Le scritte degli schermi in latino traslitterato (SPEKTR, DEMOD, GC) o in italiano?
3. Nel moderno, Okean coi colori del moderno o sempre col fosforo dell'Alfa?

**Decisioni (29/10/2026):**
- **dell'utente:** Enisej come cascata polare; nel moderno i colori del moderno, come il 688; nel 1989 il fosforo dell'Alfa;
- **della principale:** le scritte di Okean in russo traslitterato (SPEKTR, DEMOD), come KURS e UGROZA.

Il cablaggio in app/sonar_enhanced lo fa il kit.

## 10. La volta del ghiaccio sulla pagina del fondale e sulla carta

**Stato (29/10/2026):** bozze da approvare, chieste dalla principale. Il ghiaccio (opzione A del sotto-ghiaccio) è nel motore: `QuadroVolta` del giocatore (sondatore verso l'alto, BQS-15 sulle chiglie, polynye candidate con l'età, avvisi 486-489; `nucleo/volta_ghiaccio.hpp`, del disassemblatore). Al giocatore però manca una vista della volta.

Le tavole sono nello stile del gruppo 5 (gli schermi a fosforo del ripetitore di manovra). Le fa `bozze_strumenti.cpp` (`volta_fondale`, `volta_carta`) con `tools/estetica/bozze_volta.py`. Il «prima» è il gioco di oggi: SCENE1A, la pagina con Ctrl+H a 200 s e la carta della Navigazione a 600 s. I dati della volta sono d'esempio: il battello con la chiglia a 60 m e la cima della vela a 44 m, chiglie da 6 a 38 m, una polynya passata 7 minuti fa, il fondo a 260-310 m.

**`stazioni/prima_dopo_volta_fondale.png`**, la pagina del fondale (Ctrl+H) con la volta. Restano i tre schermi di oggi, e ognuno prende la volta:
- **in alto a sinistra, la volta a prua** (al posto della prospettiva dei ritorni):
  - è la pianta del settore del BQS-15 (±45°, anelli a 1 e 2 km, portata 2,5 km);
  - le chiglie viste sono tacche, col pescaggio dai 15 m in su; l'ostacolo, che arriva alla cima della vela meno il margine, è in rosso;
  - dietro il battello, le celle da 200 m misurate dal sondatore alto: più fitte più spesse; la polynya candidata in ciano, con l'età e le misure libere su quelle fatte;
  - in alto gli avvisi: la chiglia a prua e la velocità sicura;
- **in alto a destra, la carta termica:** la stessa dell'ecoscandaglio, ma con il ghiaccio sopra e il fondo sotto, su scala a radice:
  - il ghiaccio, sopra, come lo traccia il sondatore alto;
  - le tacche blu dell'acqua libera;
  - la quota del battello in verde;
  - il fondo del BQN-17 sotto;
- **sotto, la sezione di prua** (da 2 km a poppa a 8 km a prua), su scala a radice: 0, 10, 25, 50, 100, 200, 300 m. La radice serve perché 30 m di ghiaccio e 300 m di fondo stiano nello stesso riquadro. Contiene:
  - in alto il pescaggio: pieno dove l'ha misurato il sondatore (a poppa), a tacche le chiglie del BQS-15 fino a 2,5 km, a puntini oltre, dove non si sa;
  - la cima della vela meno 10 m a tratti gialli;
  - la quota e il battello con la vela;
  - in basso il fondo, misurato a poppa e dalla carta a tratti a prua.

**`stazioni/prima_dopo_volta_carta.png`**, la striscia sulla carta, come le misure del fondale:
- lungo la scia, il nastro del sondatore alto: bianco sottile, grigio spesso, nero le chiglie, ciano l'acqua libera;
- le polynye candidate col cerchio ciano e l'età («LIBERA 7'», «LIBERA 41'»: il pack deriva);
- le chiglie a prua del BQS-15, con la croce rossa e il pescaggio sull'ostacolo;
- una legenda come il bollettino del meteo.

**Le domande per l'utente:**
1. La volta prende i tre schermi della pagina (come nella tavola), o la pagina ha due linguette, FONDO e VOLTA?
2. La scala a radice nella sezione e nella carta termica, o due fasce, il ghiaccio in alto ingrandito e il fondo sotto?
3. Sulla carta, la striscia accesa con le misure del fondale (tasto 4 della pagina), o sempre sotto la banchisa?

**Decisioni (29/10/2026):**
- **dell'utente:** il ghiaccio nei tre schermi della pagina del fondale, con due fasce (una ingrandita per il ghiaccio e una per il fondo), non la scala a radice;
- **della principale:** la striscia della volta sulla carta sempre visibile sotto la banchisa, perché è un'informazione di sicurezza.

Il cablaggio in app/vista_fondale e nella striscia della carta lo fa il kit.
