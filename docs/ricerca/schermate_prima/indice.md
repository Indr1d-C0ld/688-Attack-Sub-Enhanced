# Le schermate dell'Enhanced prima della rifinitura (roadmap 33): inventario

Il «prima» del punto 33, chiesto dalla principale il 27/10/2026: tutte le schermate dell'Enhanced che si fotografano
senza finestra, con gli stati e quello che le rende povere o incoerenti. Il dossier estetico è del kit; questo è solo
l'inventario. Le immagini sono istantanee a 640×480 ingrandite 2× senza filtri. Comandi in `elenco.md`, script in
`tools/schermate/inventario.py` (lo stesso per il «dopo»: `--uscita docs/ricerca/schermate_dopo`).

Le partite di servizio sono:
- una traccia di SCENE1A col lancio a 30 s, giocata 600 s, per replay, statistiche, debriefing ed esito;
- una serie della sandbox su S5, 3 semi da 900 s.

Questa macchina non ha carriere: albo, fascicolo, bacheca e statistiche cumulate sono nello **stato vuoto**.

## Problemi trasversali

1. **Due famiglie di schermate che non si somigliano.**
   - **Con le cornici del kit:** stazioni, menu, briefing, sala siluri, esito, bacheca e albo. Hanno le consolle grigie
     (o beige per l'Alfa), i pulsanti in basso, i riquadri incavati, la carta da fascicolo.
   - **Testo nudo su nero:** replay, statistiche, statistiche cumulate, editor, regolazioni della sandbox, statistiche
     della sandbox, serie e debriefing. Testata gialla, filetto grigio, nessun riquadro.
   - Le seconde sembrano strumenti di sviluppo accanto alle prime.
2. **Muri di testo** nel font del 1989 a scala 1, senza gerarchia (stesso corpo per titoli e testo):
   - briefing;
   - museo;
   - sala radio (gli ordini);
   - regolazioni della sandbox (31 righe uguali);
   - tabelle delle statistiche;
   - colonna di destra dell'editor.

   Solo bacheca e albo usano un titolo grande.
3. **Spazio vuoto non progettato:**
   - pagine delle statistiche della sandbox (due terzi della pagina neri sotto la tabella);
   - pagina 1 delle serie;
   - parte alta della sala radio (688 e Alfa);
   - colonna di destra dell'editor nelle schede 3-8;
   - menu originali sotto l'elenco;
   - albo vuoto (un solo riquadro su un tavolo di legno).
4. **Colori delle testate e dei titoli** senza regola:
   - giallo nelle schermate del replay e delle statistiche;
   - arancio nella scelta della missione («COMSUBLANT / FLOTTA DEL NORD»);
   - bianco grande in bacheca e albo;
   - ciano per i sottotitoli ovunque, ma anche verde nel briefing.
   - Le parti hanno una regola sola (simboli del kit, amici azzurri), le testate no.
5. **Righe d'aiuto:** tutte in grigio piccolo in fondo, ma in tre forme:
   - pulsanti veri (menu, briefing, esito);
   - testo semplice (replay, statistiche, sandbox);
   - pulsanti a metà (debriefing).
6. **Stati vuoti senza disegno.** Senza carriere:
   - albo e fascicolo mostrano solo «NUOVA CARRIERA»;
   - le statistiche cumulate hanno una carta blu vuota e zeri;
   - la bacheca è tutta «non ancora presa».

   Senza dati, la serie mostra una sola barra gialla.
7. **Coppie che sembrano la stessa schermata:**
   - sonar moderno e 1989 a 200 s, perché la cascata è ancora vuota e cambiano solo due parole;
   - carta VGA e carta nautica;
   - esito pagina 1 e 2, perché questo esito non ha cerimonia.

   Per il «dopo» conviene fotografarle in un momento in cui differiscono.

## Dati tecnici per il kit

Dal codice dell'app (27/10/2026). La palette di partenza è sempre `giorno.pal`. Il font è `data/motore/font/originale.fnt`
a scala 1, salvo dove è scritto: la scala 2 solo per pochi titoli. Le consolle (sale) sono disegnate da
`grafica/src/sale.cpp` coi font del kit.

| Schermate | File | Palette in più | Cornici e pannelli (`grafica/cornici`) | Font |
|---|---|---|---|---|
| Scelta della missione | `app/scelta_missione.cpp` | `applica_palette_carta` | nessuno: pulsanti e schede disegnati a mano | originale ×1 |
| Regolazioni della sandbox | `app/regolazioni_sandbox.cpp` | `applica_palette_carta` | nessuno | originale ×1 |
| Briefing | `app/briefing.cpp` | — | nessuno (riquadri a linee) | originale ×1 |
| Sala siluri | `app/schermata_siluri.cpp` | — | consolle di `grafica/sale` | originale ×1; sagome di `grafica/sagome_armi` |
| Debriefing | `app/debriefing.cpp` | carta e rilievo (`applica_palette_rilievo`) | nessuno | originale ×1 |
| Esito e cerimonie | `app/esito.cpp` | — | `cornice(legno)`, `pulsante` ×3, carta da fascicolo (`grafica/cerimonia`) | originale ×1 e ×2 (titoli) |
| Stazioni, pannello in basso | `app/interfaccia.cpp` | carta, rilievo, `tema_pannello` (consolle grigie del 688, crema dell'Alfa) | `pannello`, consolle di `grafica/sale` | originale ×1; nelle consolle `incisione688_7` e `fosforo688` (688), `stampino_urss_10` (Alfa), `cifre_5x9` |
| Carta (F6) | `app/carta.cpp` | carta | nessuno | originale ×1; isobate `incisione688_5` (NATO), `stampino_urss_7` (VMF) |
| Sonar (F8) | `app/sonar_enhanced.cpp` | — (rampa della cascata del sonar) | `pannello` | originale ×1 |
| Sala radio (F2) | `app/stazione_radio.cpp` | — | nessuno | originale ×1 |
| Replay e vista dal vivo | `app/replay.cpp` | carta e rilievo | nessuno; simboli e legenda da `grafica/simboli_carta` | originale ×1 |
| Statistiche, cumulate, sandbox | `app/statistiche.cpp` | carta, rilievo, rampa del calore (`palette_statistiche`) | nessuno | originale ×1 |
| Serie | `app/schermata_serie.cpp` | carta (colori delle parti) | nessuno; grafici da `grafica/grafici_serie` | originale ×1 |
| Bacheca | `app/bacheca.cpp` | — | `pulsante`; tavolo di legno e medaglioni (`grafica/cerimonia`) | originale ×1 e ×2 |
| Albo e fascicolo | `app/albo.cpp` | — | tavolo di legno, carta da fascicolo | originale ×1 e ×2 |
| Albo d'onore storico | `app/albo_onore.cpp` | — | — | originale ×1 e ×2 |
| Museo | `app/museo.cpp` | carta, `applica_palette_cascata` | nessuno | originale ×1 |
| Editor | `app/editor_missione.cpp` | carta e rilievo | `pannello`, `pulsante` | originale ×1 |

In breve: le cornici del kit le usano davvero solo l'esito, la bacheca, l'editor (un pannello) e le consolle delle
stazioni. Tutte le altre disegnano righe e rettangoli a mano. I font del kit (incisione, stampino, cifre, fosforo)
stanno solo nelle consolle (`grafica/sale` carica anche `fosforo688` per i loro schermi) e sulle isobate della carta.

## Le schermate

### Menu

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Scelta: originali | ![](menu_originali.png) | schede carriera, originali, rapide, editor; la missione scelta col riquadro della carta | metà bassa vuota sotto le dieci righe; testata arancio diversa da tutte le altre; le opzioni della partita in un riquadro piccolo e fitto |
| Scelta: rapide | ![](menu_rapide.png) | parametri a caso o scelti, la carta del teatro | tabella dei parametri senza riquadri; la descrizione a destra è un blocco unico |
| Scelta: editor | ![](menu_editor.png) | nuova, da un'originale, i nove vetrini, la libreria | i vetrini e la libreria nella stessa lista senza separatore; «VETRINO DELL'IA» ripetuto in verde |
| Regolazioni della sandbox | ![](regolazioni.png) | 31 righe, per parte e globali | elenco nudo senza riquadri né gruppi; «DELLA MISSIONE» ripetuto 15 volte; nessun segno della riga scelta oltre il colore |

### Prima e dopo la missione

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Briefing | ![](briefing.png) | livelli 1-8 (ordini, zone, intelligence, basi, ASW, SOSUS, fondale, eventi) | ordini in un muro di testo verde a tutta altezza; la colonna di destra è una lista di caselle; nessuna immagine |
| Briefing della libreria | ![](briefing_libreria.png) | come sopra | stesso impianto: il testo è il protagonista |
| Sala siluri | ![](siluri_mk48.png) ![](siluri_mk67.png) | deposito, rastrelliera, tubi, scheda dell'arma con la sagoma | la più ricca; le sagome nella scheda sono piccole, la rastrelliera è una griglia uniforme |
| Debriefing, esito | ![](debriefing_esito.png) | evento 0 e gli eventi uno per uno | carta senza cornice, colonna di destra di solo testo; pulsanti in basso a metà fra quelli dei menu e il testo |
| Debriefing, un evento | ![](debriefing_evento.png) | la minicartolina dell'evento | come sopra; la minicartolina è piccola rispetto alla carta |
| Esito | ![](esito_rapporto.png) ![](esito_cerimonia.png) | rapporto, cerimonie | il rapporto sulla carta da fascicolo è curato; la pagina 2 qui è uguale alla 1 (nessuna cerimonia per questo esito) |

### Stazioni del 688 (SCENE1A)

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Periscopio | ![](688_periscopio.png) | 1,5x e 6x, giorno, notte, termico (non da riga di comando) | coerente; la barra ESM vuota sembra un segnaposto |
| F2 Radio ed ESM | ![](688_radio.png) | messaggi, ROE, finestre, ESM col WLR-8 | metà alta testo fitto su nero, poi vuoto; il riquadro dell'ordine è un rettangolo di linee |
| F3 Stato | ![](688_stato.png) | spaccato, danni, compartimenti | coerente e ricco |
| F4 Manovra | ![](688_manovra.png) | il periscopio sopra, i quadranti sotto | sopra c'è la vista del periscopio sott'acqua: chi non lo sa la prende per un errore |
| F6 Navigazione | ![](688_carta.png) ![](688_carta_nautica.png) ![](688_carta_foglio.png) ![](688_carta_vera.png) ![](688_carta_mate.png) | stili VGA, nautica, foglio; fondale vero; MATE | la nautica è quasi identica alla VGA; il foglio ha una griglia rada con segni a L che sembrano un difetto; la riga dei tasti in fondo è lunga e fitta |
| F7 Armi | ![](688_armi.png) | il periscopio sopra, il pannello dei tubi sotto | come la manovra: la metà alta non è della stazione |
| F8 Sonar | ![](688_sonar.png) ![](688_sonar_1989.png) | moderno, 1989; cascata, LOFAR, analizzatore | a 200 s la cascata è vuota; la differenza fra moderno e 1989 non si vede; la banda blu del cono sordo domina |
| F9 Centrale | ![](688_centrale.png) | la sala con le consolle | la più «illustrata»; stile diverso da tutte le altre stazioni |

### Stazioni dell'Alfa (SCENE2B)

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Periscopio | ![](alfa_periscopio.png) | come il 688 | la vista in grigio sembra un'altra palette |
| F2 Radio | ![](alfa_radio.png) | come il 688, consolle beige | metà alta quasi tutta vuota |
| F3 Stato | ![](alfa_stato.png) | spaccato dell'Alfa | coerente |
| F4 Manovra | ![](alfa_manovra.png) | pulti BOKSIT, TELEGRAF, POKS | consolle beige curate; sopra la vista sott'acqua come il 688 |
| F6 Carta VMF | ![](alfa_carta.png) | la carta sovietica | coerente |
| F7 Armi | ![](alfa_armi.png) | AKKORD, SARGAN | coerente |
| F8 Sonar | ![](alfa_sonar.png) | lo schermo circolare del MGK, LOFAR | il messaggio grande in verde in fondo copre la riga d'aiuto |

### Replay e statistiche

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Replay | ![](replay_quadro.png) ![](replay_verita.png) | quadro e verità; dal vivo nella sandbox | colonna di destra senza riquadri; il profilo verticale è una scatola nera con una barra arancio |
| Statistiche della missione | ![](statistiche.png) | una pagina | sei blocchi di testo e grafici senza cornici; i grafici vuoti (quota, livello) sembrano rotti |
| Statistiche cumulate | ![](statistiche_cumulate.png) | globali o per comandante, una carta per teatro | stato vuoto: zeri e una carta blu senza niente |

### Carriera e consultazione

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Bacheca | ![](bacheca.png) | sette schede di riconoscimenti, il medaglione scelto | otto icone su un tavolo di legno vuoto; il riquadro di carta è curato, il resto no |
| Albo | ![](albo.png) | le carriere | stato vuoto: un riquadro «NUOVA CARRIERA» su tutto lo schermo di legno |
| Fascicolo | ![](fascicolo.png) | pagine del fascicolo | senza carriere mostra l'albo (qui non fotografabile) |
| Museo | ![](museo.png) ![](museo_alfa.png) ![](museo_mk48.png) | voci per sezione, immagini, testo it/en | elenco a sinistra senza riquadro; immagine piccola su fondo blu pieno; testo in tre colori senza ragione chiara |

### Editor

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Editor, schede 1-8 | ![](editor_1.png) ![](editor_2.png) ![](editor_3.png) ![](editor_4.png) ![](editor_5.png) ![](editor_6.png) ![](editor_7.png) ![](editor_8.png) | missione, unità, rotte, zone, obiettivi, regole, briefing, prova | le linguette in alto a destra sono minuscole; nelle schede 3-8 la colonna di destra è quasi vuota; nessun riquadro attorno ai campi; la riga dei tasti è tagliata a destra |

### Sandbox

| Schermata | Immagine | Stati | Povera o incoerente |
|---|---|---|---|
| Vetrino dal vivo | ![](sandbox_vivo.png) | letture verità, parte, unità; indietro e al vivo | registro e scheda senza riquadri, come il replay |
| Statistiche della sandbox | ![](sandbox_statistiche_1.png) ![](sandbox_statistiche_2.png) ![](sandbox_statistiche_3.png) ![](sandbox_statistiche_4.png) ![](sandbox_statistiche_5.png) | cinque pagine | tabelle di testo piccolo e due terzi della pagina vuoti; nessun grafico (le serie ce li hanno); le intestazioni troncate |
| Serie | ![](serie_1.png) ![](serie_2.png) ![](serie_3.png) | esiti, per parte, istogrammi | pagina 1 quasi vuota con una serie corta; buona la pagina per parte |

## Da aggiungere (non si fotografano senza finestra)

| Schermata | Perché |
|---|---|
| Titolo e menu iniziale dell'Enhanced | la sequenza d'avvio con la musica; `--immagine` mostra solo le immagini del 1989 |
| Scelta della missione: carriera, importate, una missione scelta col dettaglio | `--schermata` ha solo scelta, scelta-rapida e scelta-editor; nessuna opzione per la riga scelta |
| Museo: l'indice delle voci e la ricerca | `--schermata museo` apre direttamente una voce |
| Albo d'onore storico | c'è l'anteprima del kit (`anteprima_albo_onore`), non la schermata del gioco |
| Albo, fascicolo, bacheca e cumulate con dati | servono carriere: un'opzione per una cartella utente di prova (per esempio `--dati-utente`) le renderebbe fotografabili piene |
| Opzioni e tasti | nessuna `--schermata` |
| Pausa, messaggi radio da autenticare, dialoghi di conferma | compaiono solo in finestra |
| Vista dal vivo della sandbox in lettura parte o unità | `--schermata vivo` disegna solo la verità |
| Periscopio a 6x e nei modi notte e termico | servono i tasti (`--tasti-a-tempo`), nessuna opzione diretta |
| Esito con una cerimonia | serve un esito che la dia: un'opzione per scegliere la pagina con dati finti, o una missione di prova che finisca con una decorazione |
