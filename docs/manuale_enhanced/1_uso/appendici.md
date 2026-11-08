# Appendici del volume 1

## A. Tutti i tasti

Il riferimento completo dei tasti: prima i comandi, poi schermata per schermata e stazione per stazione. Le tabelle
sono generate e rifatte a ogni cambio. I tasti vengono da `data/motore/tasti_predefiniti.toml`, quindi seguono i tasti
cambiati (§2.9); le spiegazioni per schermata da `tools/pipeline/tasti_schermate.toml`. Una prova
(`manuale.py --controlla`) confronta queste tabelle col codice di ogni schermata e segnala i tasti che mancano.

Su tastiera italiana `[` è AltGr+è e `]` è AltGr++; con Maiusc in più, il passo lungo. Il + da solo resta il telegrafo.
Valgono in tutto il gioco: periscopio, carta, regolo di Ekelund, armi, sonar, radio. Le scritte a schermo dicono ancora
`[` e `]`.

### A.1 I comandi

Ogni comando col suo tasto. Molti tasti cambiano significato secondo la schermata o la stazione: lo dicono le tabelle
che seguono.

<!-- generato: tasti -->
| Comando | Tasti | Che cosa fa |
|---|---|---|
| `esci` | `Esc` | esce dal gioco; in una scelta la annulla |
| `conferma` | `Invio`, `Tastierino Invio` | conferma la scelta in corso |
| `zona_avanti` | `Tab` | scelta: zona successiva; Armi dell'Enhanced: campo successivo |
| `zona_indietro` | `Maiusc+Tab` | scelta: zona precedente |
| `cambia_ingrandimento` | `Giu`, `Su` | periscopio: 1,5x o 6x |
| `ingrandisci` | `I` | periscopio: 6x; carta: zoom in avanti |
| `riduci` | `O` | carta: zoom all'indietro |
| `compressione_piu` | `Pag Su` | raddoppia la compressione del tempo |
| `compressione_meno` | `Pag Giu` | dimezza la compressione del tempo |
| `verita` | `V` | diagnosi: classe, distanza e AOB veri |
| `istantanea` | `F12` | salva la schermata in BMP |
| `uscita_schermo` | `F10` | uscita a schermo: nitida, sharp bilinear, sharp bilinear col CRT (E9 §2.1) |
| `stazione_radio` | `F2` | stazione Radio (archivio dei messaggi; nell'Enhanced la sala radio) |
| `stazione_stato` | `F3` | stazione Stato (quadro danni) |
| `stazione_manovra` | `F4` | stazione Manovra |
| `stazione_periscopio` | `F5` | stazione Periscopio (Enhanced: i periscopi veri) |
| `stazione_navigazione` | `F6` | stazione Navigazione (carta) |
| `stazione_armi` | `F7` | stazione Armi |
| `stazione_sonar` | `F8` | stazione Sonar |
| `stazione_centrale` | `F9` | Centrale: stato rapido e menu (nel classico ferma il tempo) |
| `pausa` | `P` | pausa (Enhanced; nel classico ferma il tempo anche la Centrale) |
| `tubo_1` | `R` | tubo 1 |
| `tubo_5` | `A` | tubo 5 (primo ausiliario) |
| `tubo_7` | `M` | tubo 7 (missili) |
| `tubo_2` | (nessuno) | tubo 2 (pannello Armi della Fase 3) |
| `tubo_3` | (nessuno) | tubo 3 |
| `tubo_4` | (nessuno) | tubo 4 |
| `tubo_6` | (nessuno) | tubo 6 |
| `pulsante_1` | `1` | pulsante 1 della stazione |
| `pulsante_2` | `2` | pulsante 2 |
| `pulsante_3` | `3` | pulsante 3 |
| `pulsante_4` | `4` | pulsante 4 |
| `pulsante_5` | `5` | pulsante 5 |
| `pulsante_6` | `6` | pulsante 6 |
| `pulsante_7` | `7` | pulsante 7 |
| `pulsante_8` | `8` | pulsante 8 |
| `pulsante_9` | `9` | pulsante 9 |
| `lancia` | `L` | lancio dal tubo selezionato |
| `bersaglio` | `G` | TARGET: scelta del bersaglio |
| `siluro` | `S` | siluro filoguidato successivo |
| `cerca` | `C` | classico: CERCA; Enhanced: testa attiva |
| `detona` | `D` | classico: DETONATE; Enhanced: spegnimento |
| `punto_di_rotta` | `W` | carta: nuovo punto di rotta |
| `cancella_punto` | `K` | carta: cancella l'ultimo punto |
| `proiezione` | `J` | carta: proiezione a N minuti |
| `sfondo` | `B` | carta: sfondo senza fondale (Enhanced: stile della carta) |
| `mate` | `T` | carta: strumento MATE della TMA |
| `filo_sinistra` | `Z` | siluro via filo: rotta -10 gradi |
| `filo_dritta` | `X` | siluro via filo: rotta +10 gradi |
| `taglia_filo` | `Y` | siluro: taglia il filo |
| `variante_arma` | `Ctrl+V` | Armi dell'Enhanced: arma successiva della rastrelliera nel tubo scelto |
| `enciclopedia` | `F1` | il Museo dal menu; in missione la scheda dell'enciclopedia (Enhanced) |
| `valore_meno` | `[` | carta: minuti -1; Armi dell'Enhanced: valore precedente. Su tastiera italiana AltGr+è |
| `valore_piu` | `]` | carta: minuti +1; Armi dell'Enhanced: valore successivo. Su tastiera italiana AltGr++ |
| `telegrafo_avanti` | `+`, `=`, `Tastierino +` | telegrafo avanti di una posizione |
| `telegrafo_indietro` | `-`, `Tastierino -` | telegrafo indietro di una posizione |
| `timone_sinistra` | `Tastierino 4` | timone uno scatto a sinistra |
| `timone_dritta` | `Tastierino 6` | timone uno scatto a dritta |
| `timone_centro` | `Tastierino 5` | timone al centro |
| `barra_su` | `Tastierino 8` | barra di immersione su (Enhanced: quota -15 m) |
| `barra_giu` | `Tastierino 2` | barra di immersione giù (Enhanced: quota +15 m) |
| `barra_zero` | `Tastierino 0` | barra a zero: tiene la quota |
| `pilota_automatico` | `.`, `Tastierino .` | pilota automatico |
| `periscopio` | `U` | periscopio su o giù |
| `antenna` | `N` | antenna su o giù |
| `stadimetro` | `H` | periscopio: distanza del bersaglio dall'altezza dell'albero |
| `quota_periscopica` | `Ctrl+P` | quota periscopica |
| `quota_150_piedi` | `Ctrl+1` | 150 piedi |
| `emersione_emergenza` | `Ctrl+E` | emersione d'emergenza |
| `posati` | `Ctrl+F` | Enhanced: posarsi sul fondo |
| `immersione_emergenza` | `Ctrl+D` | Enhanced: immersione d'emergenza, avanti tutta alla quota di sicurezza |
| `boksit_rotta` | `Ctrl+B` | Alfa: Boksit, modo della rotta (manuale, automatico, programma) |
| `boksit_quota` | `Ctrl+U` | Alfa: Boksit, modo della quota (manuale, automatico, programma) |
| `boksit_tappa` | `Ctrl+T` | Alfa: Boksit, il programma dai punti di rotta della carta |
| `tan` | `Ctrl+N` | Alfa: Tan, assetto automatico inserito o disinserito |
| `sfoga` | `Ctrl+G` | Alfa: ПОКС, apre gli sfoghi delle casse principali |
| `compensa_piu` | `Ctrl+K` | Alfa: ПОКС, imbarca mezzo metro cubo nelle casse d'assetto |
| `compensa_meno` | `Ctrl+J` | Alfa: ПОКС, espelle mezzo metro cubo dalle casse d'assetto |
| `akkord_designa` | `Ctrl+A` | Alfa: Akkord, designa il bersaglio corrente in uno slot libero |
| `akkord_1` | `Ctrl+5` | Alfa: Akkord, sceglie il bersaglio 1 |
| `akkord_2` | `Ctrl+6` | Alfa: Akkord, sceglie il bersaglio 2 |
| `akkord_3` | `Ctrl+7` | Alfa: Akkord, sceglie il bersaglio 3 |
| `bqs15` | `Ctrl+M` | 688: BQS-15, il sonar per le mine acceso o spento |
| `radar_giocatore` | `Ctrl+R` | Enhanced: il radar del battello, emerso o con l'albero su a quota periscopica |
| `punto_nave` | `Ctrl+O` | carta, navigazione realistica: il menu dei punti nave (Transit, Omega, stelle...) |
| `aiuto_comandi` | `F11` | la schermata dei comandi, da ogni stazione |
| `muto` | `Ctrl+Q` | classico: suoni spenti o accesi, come Ctrl-Q nel 1989 (Ctrl+N: senza campioni) |
| `registro_messaggi` | `Ctrl+L` | Enhanced: il registro dei messaggi di bordo, da ogni stazione |
| `intercettazione` | `Ctrl+W` | Enhanced: la pagina dell'intercettazione del sonar (WLR-9), da ogni stazione |
| `quadro_fondale` | `Ctrl+H` | Enhanced: la pagina del quadro del fondale alla Navigazione (ecoscandaglio, sezione di prua, ritorni), da ogni stazione |
| `libretto` | `Ctrl+Z` | Enhanced: il libretto di riconoscimento accanto alla vista del Periscopio, da ogni stazione (Tab sfoglia, Invio identifica il bersaglio, [ ] l'altezza d'albero a mano, Z X l'AOB) |
| `osserva_aob` | `Spazio` | Enhanced, libretto aperto: l'angolo sulla prora impostato va alla TMA, a periscopio alzato |
| `guida_punto` | `Ctrl+I` | carta: DIRECT, il siluro filoguidato scelto verso un punto |
| `silenzio_a_bordo` | `Ctrl+S` | Enhanced: silenzio a bordo (ricariche più lente e senza rumore) |
| `carta_evento` | `E` | carta dell'Enhanced: evento con l'ora sulla posizione del battello |
| `carta_aggancia` | `F` | carta dell'Enhanced: aggancia l'estremo del righello al battello o al contatto più vicino |
| `carta_ekelund` | `Q` | carta dell'Enhanced: regolo di Ekelund |
| `carta_pulisci` | `Ctrl+X` | carta dell'Enhanced: pulisce il lucido (tratti, linee, eventi, datum) |
<!-- fine: tasti -->

<!-- generato: tasti_schermate -->
### A.2 Classico o Enhanced (solo al primo avvio)

| Tasti | Che cosa fa |
|---|---|
| `Sinistra`, `Destra` | il pannello a sinistra (classico) o a destra (Enhanced) |
| `1`-`2` | 1 classico, 2 Enhanced |
| `Invio`, `Tastierino Invio` | conferma |
| `Esc` | esce dal gioco |
| clic | sceglie il pannello, un secondo clic conferma; i tasti del piede |

### A.3 Scelta della missione

Le opzioni della partita e i tasti di passaggio valgono in tutte e tre le schede (cap. 1, §1.5).

**In tutte le schede**

| Tasti | Che cosa fa |
|---|---|
| `O` | il modo di gioco: Enhanced o classico (anche col clic sul primo tasto in alto); la scelta resta nelle preferenze |
| `R` | la scheda successiva: missioni originali, missione rapida, editor |
| `P` | digita un codice: partita, missione rapida o sandbox (§11.4, §12.7) |
| `V` | la sandbox, il vetrino dell'IA, sulla missione scelta: le regolazioni, poi la partita senza giocatore (§12); nella scheda editor vale come G |
| `Tab` | stazioni 1989 o moderne (§1.4) |
| D | carta d'epoca o fondale vero (anche `[` e `]`, dove la tastiera li ha); ogni riga delle opzioni si può anche cliccare |
| `K` | nemici (ASW aereo e cannoni): facile, normale, realistico, minaccia percepita |
| `S` | il sonarista dell'equipaggio: nessuno, recluta, medio, esperto |
| `N` | navigazione: perfetta, realistica, o realistica col GPS 1990+ (cap. 5) |
| `M` | la musica del menu: il tema dell'Enhanced in FM o con l'MT-32 (i WAV resi dalle ROM dell'utente, dove ci sono), i titoli del 1989 in AdLib, Tandy o Covox, o spenta (§1.6) |
| `A` | il video d'apertura: sempre, la prima volta, mai (§1.5) |
| `F1` | il Museo |
| `C` | l'albo dei comandanti |

**Missioni originali**

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | la missione sopra o sotto |
| `1`-`9` | la missione di quella riga |
| `Sinistra`, `Destra` | il battello: 688 a sinistra, Alfa a destra (se la missione li ha tutti e due) |
| `W` | meteo del teatro e della stagione, o mare 3 e sereno |
| `Invio`, `Tastierino Invio` | la missione scelta: apre il briefing |
| `Esc` | esce dal gioco |
| clic | sceglie la missione nella colonna del battello; sulle linguette la scheda; nel piede conferma o esce |

### A.4 Scelta della missione: missione rapida

Valgono anche i tasti di tutte le schede (sopra).

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | il parametro sopra o sotto |
| `Sinistra`, `Destra` | il valore del parametro; sulla riga del seme, un seme nuovo |
| `W` | meteo del teatro e della stagione, o fisso |
| `E` | apre la missione generata nell'editor |
| `Invio`, `Tastierino Invio` | gioca la missione; sulla riga del codice, digita un codice |
| `Esc` | esce dal gioco |
| clic | sceglie il parametro; nel piede gioca o esce |

### A.5 Scelta della missione: editor

Valgono anche i tasti di tutte le schede (sopra).

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | la riga: missione nuova, da una originale, i vetrini pronti dell'IA (in verde, §12.8), le missioni salvate |
| `Sinistra`, `Destra` | sulla riga «da una originale»: quale missione convertire |
| `Invio`, `Tastierino Invio` | apre l'editor sulla riga scelta; su un vetrino pronto, le sue regolazioni |
| `G` | gioca subito la missione salvata, senza aprire l'editor; su un vetrino pronto, le sue regolazioni |
| `I` | importa un pacchetto |
| `Esc` | esce dal gioco |
| clic | sceglie la riga, un secondo clic la apre; nel piede apre o esce |

### A.6 Scelta della missione: codice

Dopo P, in ogni scheda (§11.4).

| Tasti | Che cosa fa |
|---|---|
| la tastiera (si scrive) | lettere, cifre e trattini del codice |
| `Backspace` | cancella l'ultimo carattere |
| `Invio`, `Tastierino Invio` | legge il codice: una rapida va alla sua scheda, una partita parte subito, un codice sandbox riapre le regolazioni già compilate |
| `Esc` | annulla |

### A.7 Sandbox: regolazioni

Dopo V nella scelta della missione (§12.3).

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | la riga |
| `Sinistra`, `Destra` | il valore della riga |
| `Invio`, `Tastierino Invio` | avvia la partita senza giocatore |
| `S` | una serie di 10 semi senza finestra, poi la sua schermata (§12.8) |
| `Tab`, `Maiusc+Tab` | la seconda pagina, gli interventi programmati del vetrino di Dio (anche col clic sulla scheda in alto a destra); Tab o Esc tornano alle regolazioni |
| `Esc` | torna alla scelta della missione |
| clic | sceglie la riga, un secondo clic cambia il valore; nel piede avvia (a destra), fa la serie (al centro) o torna indietro (a sinistra) |

### A.8 Albo dei comandanti

| Tasti | Che cosa fa |
|---|---|
| `Sinistra`, `Destra` | il cartellino precedente o successivo |
| `Su`, `Giu` | la fila sopra o sotto |
| `Invio`, `Tastierino Invio` | mette in carica il comandante scelto; sul cartellino vuoto, una carriera nuova. Nel modulo: il campo seguente; sull'ultimo apre il fascicolo |

**Il modulo della carriera nuova**

| Tasti | Che cosa fa |
|---|---|
| `Tab` | il campo seguente o precedente (anche su e giù; sinistra e destra cambiano la scelta; ogni casella si clicca) |

| Tasti | Che cosa fa |
|---|---|
| `F` | il fascicolo del comandante |
| `B` | la bacheca dei riconoscimenti (senza un comandante scelto, i globali) |
| `O` | l'albo d'onore |
| `S` | le statistiche cumulate (senza un comandante scelto, le globali) |
| E o Backspace | elimina la carriera del comandante scelto: una prima volta chiede conferma, la seconda è definitiva (anche coi tasti del piede); Esc la annulla |
| `Esc` | annulla l'eliminazione, o torna al menu |
| clic | sceglie un cartellino, un secondo clic come Invio; nel piede: menu, albo d'onore (fascicolo con un comandante), statistiche, Invio |

**Carriera nuova**

| Tasti | Che cosa fa |
|---|---|
| la tastiera (si scrive) | il nome |
| `Backspace` | cancella l'ultima lettera del nome |
| `Su`, `Giu` | il campo: nome, marina, carriera breve o completa, indulgenza |
| `Sinistra`, `Destra` | il valore del campo |
| `Invio`, `Tastierino Invio` | crea la carriera |
| `Esc` | annulla |

### A.9 Fascicolo

| Tasti | Che cosa fa |
|---|---|
| `Sinistra`, `Destra` | la pagina: copertina, stato di servizio, note caratteristiche, decorazioni, provvedimenti |
| `Invio`, `Tastierino Invio` | la pagina successiva |
| `Su`, `Giu` | scorre il testo; nelle note caratteristiche, la missione sopra o sotto |
| `P` | nelle note caratteristiche: il replay della missione scelta, se c'è |
| clic | i tasti del piede: indietro, replay, pagina prima e dopo |
| `Esc` | torna all'albo |

### A.10 Bacheca dei riconoscimenti

| Tasti | Che cosa fa |
|---|---|
| `1`-`7` | la scheda: tattici, stealth, tecnici, storici, carriera, editor, nascosti |
| `Tab`, `Maiusc+Tab` | la scheda successiva o precedente |
| `Sinistra`, `Destra`, `Su`, `Giu` | il medaglione |
| `G` | i riconoscimenti del comandante o quelli globali |
| clic | sceglie la scheda o il medaglione; i tasti del piede: indietro, comandante o globali, scheda successiva |
| `Esc` | torna indietro |

### A.11 Albo d'onore

| Tasti | Che cosa fa |
|---|---|
| `1`-`3` | la scheda: migliori missioni, migliori carriere, comandanti storici |
| `Tab`, `Maiusc+Tab` | la scheda successiva o precedente |
| `Su`, `Giu` | scorre il registro |
| clic | sceglie la scheda dalla linguetta |
| `Esc` | torna indietro |

### A.12 Statistiche cumulate

Due pagine: le statistiche (abitudini, armi, la carta dei teatri) e le imprese (ghiaccio, notte, pedinamento, soccorso, riconoscimento, mine, ASW aereo, armi e navigazione).

| Tasti | Che cosa fa |
|---|---|
| `1`-`2` | la pagina: 1 le statistiche, 2 le imprese |
| `Tab`, `Maiusc+Tab` | il teatro successivo o precedente; dalle imprese, torna alle statistiche |
| clic | sulla carta: il teatro successivo; nel piede: altro teatro (dalle imprese: le statistiche) o indietro |
| `Invio`, `Tastierino Invio`, `Esc` | torna indietro |

### A.13 Museo

| Tasti | Che cosa fa |
|---|---|
| `Sinistra`, `Destra` | la sezione accanto, una per pressione; dopo l'ultima si torna alla prima |
| `Su`, `Giu` | la voce (tenuto, scorre); nell'indice, la sezione |
| `Invio`, `Tastierino Invio` | apre l'indice delle sezioni; dall'indice, apre la sezione scelta |
| `Tab`, `Maiusc+Tab` | l'immagine successiva o precedente della voce |
| `Pag Su`, `Pag Giu` | gira la pagina del testo («(SEGUE) 1/3») |
| `L` | italiano o inglese |
| `1`-`9` | apre la voce del riferimento con quel numero |
| `Esc` | chiude l'indice; se no torna alla voce di prima; dalla prima, esce dal Museo |
| clic | `<` e `>`: la sezione accanto; il nome della sezione: l'indice; una riga: la voce, o nell'indice la sezione; i tasti del piede: indietro, l'immagine, la pagina del testo |

### A.14 Briefing

| Tasti | Che cosa fa |
|---|---|
| `1`-`8` | accende o spegne un livello d'informazione della carta |
| `9` | tutti i livelli, o nessuno |
| `Tab` | apre o chiude gli ordini a telescrivente |
| `Maiusc` tenuto | la telescrivente scrive sei volte più in fretta |
| `Su`, `Giu` | con gli ordini aperti, la pagina precedente o seguente («(SEGUE) n/m», «(FINE)») |
| le frecce | spostano la carta (con gli ordini chiusi) |
| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore; trascinare sposta la carta |
| `F7` | la sala siluri: il carico (cap. 6) |
| `Invio`, `Tastierino Invio` | in mare |
| `Esc` | rifiuta la missione |
| clic | accende o spegne un livello dal pannello dei livelli; nel piede i tre pulsanti |

### A.15 Sala siluri

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | l'arma del deposito |
| `Sinistra`, `Destra` | destra la mette (in rastrelliera, nei pozzi o fra le esche), sinistra la toglie |
| `1`-`6` | la mette in quel tubo, o la toglie se c'è già |
| `C` | apre l'elenco dei carichi per compito (anche col clic sul pulsante del carico): frecce e Invio o il clic sulla riga lo applicano, Esc o un clic fuori chiudono senza cambiare |
| `Tab`, `Maiusc+Tab` | il carico successivo |
| `S` | applica il carico scelto |
| `X` | svuota |
| `D` | la dotazione dell'originale |
| `Invio`, `Tastierino Invio` | in mare; con degli avvisi chiede conferma (Invio parte, Esc resta) |
| `Esc` | torna al briefing |
| clic | sceglie l'arma del deposito; nel piede i pulsanti; nell'elenco, la riga |

### A.16 In mare

I tasti che valgono da ogni stazione, uno per comando, sono nella tabella A.1; qui ci sono quelli che cambiano secondo
la stazione. Le cifre sono i pulsanti della stazione. I capitoli 2-7 spiegano ogni comando.

**Da ogni stazione**

| Tasti | Che cosa fa |
|---|---|
| `F2`-`F9` | la stazione (cap. 2); chiude anche la scelta del bersaglio |
| `P` | pausa |
| `Pag Su`, `Pag Giu` | raddoppia o dimezza la compressione del tempo |
| `Esc` | annulla la scelta in corso; chiude il registro, il menu dei punti nave, la libreria del sonar; toglie la pausa; altrimenti lascia la missione |
| `F11` | la schermata dei comandi, sulla scheda della stazione: F11 o Esc la chiude |
| i tasti di manovra | timone, telegrafo, quota, pilota automatico, periscopio, antenna, emergenze (tabella A.1) |
| `R`, `A`, `M` | sceglie il tubo 1, 5 o 7 (missili) |
| `L` | lancia dal tubo scelto |
| `S` | il siluro filoguidato successivo |
| `G` | TARGET: ferma il tempo e fa scegliere il bersaglio fra i segni della vista (al periscopio) o della carta |
| `Ctrl+S` | silenzio a bordo, con la ricarica realistica (cap. 6) |
| `F1` | Enhanced: la scheda dell'enciclopedia di ciò che si guarda |
| `V` | la diagnosi dei contatti: classe, distanza e AOB veri |
| `F12` | salva la schermata |
| `Ctrl+L` | Enhanced: il registro dei messaggi di bordo |
| `Ctrl+R` | Enhanced: il radar del battello, emerso o con l'albero su |
| `Ctrl+M` | Enhanced, 688: il BQS-15, il sonar per le mine |
| `Ctrl+W` | Enhanced: la pagina dell'intercettazione e dell'allarme siluro, sulla stazione Sonar |
| `Ctrl+H` | Enhanced: la pagina del quadro del fondale, sulla stazione Navigazione (§5.5); di nuovo, o Esc, la chiude |
| `Ctrl+Q` | classico: i suoni spenti o accesi, come Ctrl-Q nel 1989 |
| `Ctrl+N` | classico: i suoni senza i campioni, come Ctrl-N nel 1989; Enhanced, Alfa: il Tan |

**Scelta del bersaglio (dopo TARGET)**

| Tasti | Che cosa fa |
|---|---|
| le lettere | il segno con quella lettera |
| `Tab`, `Maiusc+Tab` | il segno successivo o precedente, anche quelli senza lettera |
| clic | il segno più vicino al clic, e la scelta si chiude |
| `Invio`, `Tastierino Invio` | conferma; Esc senza bersaglio |

**Alfa (Enhanced), da ogni stazione**

| Tasti | Che cosa fa |
|---|---|
| `Ctrl+B`, `Ctrl+U` | Boksit: il modo della rotta o della quota (manuale, automatico, programma) |
| `Ctrl+T` | Boksit: il programma dai punti di rotta della carta |
| `Ctrl+G` | ПОКС: apre gli sfoghi delle casse principali |
| `Ctrl+K`, `Ctrl+J` | ПОКС: imbarca o espelle mezzo metro cubo nelle casse d'assetto |
| `Ctrl+A` | Akkord: designa il bersaglio in uno slot libero |
| `Ctrl+5`, `Ctrl+6`, `Ctrl+7` | Akkord: sceglie il bersaglio 1, 2 o 3 |

**Stato (F3)**

| Tasti | Che cosa fa |
|---|---|
| `Tab`, `Maiusc+Tab` | il compartimento ingrandito, uno dopo l'altro, poi la scelta automatica |
| `1`-`5` | Enhanced: 1 isola il compartimento mostrato, poi chiude subito, a porte chiuse riapre; 5 la pagina delle squadre o di nuovo lo spaccato; nella pagina 2 e 3 scelgono un sistema, 4 lo porta in cima alle riparazioni (§7.3) |

**Manovra (F4)**

| Tasti | Che cosa fa |
|---|---|
| `+`, `=`, `Tastierino +`, `-`, `Tastierino -` | il telegrafo, avanti o indietro di una posizione: è la velocità |
| `Tastierino 4`, `Tastierino 6`, `Tastierino 5` | il timone: a sinistra, a dritta, al centro |
| `Tastierino 8`, `Tastierino 2`, `Tastierino 0` | la quota: più su, più giù, tiene questa (Enhanced: a passi di 15 m) |
| `Ctrl+P` | la quota periscopica |
| `Ctrl+1` | la quota di 150 piedi |
| `.`, `Tastierino .` | il pilota automatico sui punti di rotta della carta |
| `Ctrl+E` | l'emersione d'emergenza |
| `Ctrl+D` | Enhanced: l'immersione d'emergenza, avanti tutta alla quota di sicurezza |
| `Ctrl+F` | Enhanced: posarsi sul fondo |

**Periscopio (F5)**

| Tasti | Che cosa fa |
|---|---|
| `U` | alza o abbassa il periscopio, da ogni stazione |
| `Sinistra`, `Destra` | ruota il periscopio (Maiusc più veloce; ad alto ingrandimento più lento) |
| `Su`, `Giu` | Enhanced: l'elevazione, per cercare gli aerei; si ferma ai fermi a molla |
| `Giu`, `Su` | classico: 1,5× o 6×; Enhanced, dalle altre stazioni: l'ingrandimento successivo |
| `I` | l'ingrandimento (Enhanced: il successivo, come il pulsante 3) |
| `1`-`7` | Enhanced: l'altro periscopio, alza o abbassa, ingrandimento, modo di visione, foto, filtro (§4.1); sull'Alfa la telecamera verticale (§4.4) |
| `H` | lo stadimetro; coi telemetri sdoppiati apre l'immagine fantasma e poi legge la distanza (§4.3) |
| `[`, `]` | stadimetro sdoppiato: sposta l'immagine fantasma del 2% (Maiusc del 10%) |
| `Maiusc` tenuto | rotazione e stadimetro più veloci; negli strumenti della carta, passi più lunghi |

**Periscopio: il libretto di riconoscimento (Enhanced, §4.3)**

| Tasti | Che cosa fa |
|---|---|
| `Ctrl+Z` | apre o chiude il libretto accanto alla vista; da un'altra stazione porta al Periscopio |
| `Tab`, `Maiusc+Tab` | la tavola seguente o precedente |
| `Invio`, `Tastierino Invio` | IDENTIFICA: la classe della tavola al bersaglio designato, la sua altezza d'albero al quadrante |
| `[`, `]` | a stadimetro chiuso: l'altezza sul quadrante a mano, 1 m (Maiusc 5 m) |
| `Z`, `X` | l'angolo sulla prora, 10° a sinistra o a dritta |
| `Spazio` | a periscopio alzato: l'angolo sulla prora va alla TMA |
| `F` | apre o chiude la scheda del filtro (parte, tipo, «cosa vedo»); chiusa, Tab sfoglia solo le tavole che passano |
| `Esc` | chiude la scheda del filtro, poi il libretto |

**Periscopio: la scheda del filtro del libretto (Enhanced, §4.3)**

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | la riga: la parte, il tipo, le etichette (l'elenco scorre) |
| `Sinistra`, `Destra` | la parte o il tipo precedente o seguente, passando da TUTTE |
| `Invio`, `Tastierino Invio`, `Spazio` | il valore seguente, o segna e toglie la casella |
| `Backspace` | la riga torna a TUTTE |
| `F`, `Esc` | chiudono la scheda; le frecce tornano al periscopio |

**Navigazione: la carta (F6; sull'Alfa il Sozh)**

| Tasti | Che cosa fa |
|---|---|
| `I`, `O` | ingrandisce o riduce |
| `W` | nuovo punto di rotta: frecce o clic per il cursore, Invio lo mette, Esc annulla; fino a nove |
| `K` | cancella l'ultimo punto di rotta (anche dalle altre stazioni) |
| `J` | la proiezione del battello e del bersaglio a N minuti |
| `[`, `]` | i minuti della proiezione |
| `B` | classico: lo sfondo uniforme; Enhanced: lo stile della carta |
| `1`-`3` | le scie di tutti, la scia del bersaglio, la proiezione disegnata |
| `4`-`9` | Enhanced: righello, linee di rilevamento, anelli, segni, CPA, pannello tempo-rilevamento (§5.3) |
| `E` | Enhanced: un evento con l'ora sulla posizione del battello |
| `Q` | Enhanced: il regolo di Ekelund |
| `T` | il MATE della TMA: frecce rotta e distanza, Maiusc con su e giù la velocità |
| `Ctrl+X` | Enhanced: pulisce il lucido (tratti, linee, eventi, datum) |
| `Ctrl+O` | navigazione realistica: il menu dei punti nave; la cifra sceglie il punto nave |
| `Ctrl+I` | DIRECT: il siluro filoguidato scelto verso un punto (frecce o clic, Invio) |

**Navigazione: il quadro del fondale (Ctrl+H)**

| Tasti | Che cosa fa |
|---|---|
| `1`-`4` | Enhanced: un ping dell'ecoscandaglio, il periodo successivo, la potenza piena o bassa, le misure sulla carta (§5.5) |
| `Esc` | chiude la pagina, non il gioco |

**Carta: righello (4)**

| Tasti | Che cosa fa |
|---|---|
| le frecce | muovono l'estremo attivo (Maiusc più veloce); anche il clic |
| `Tab`, `Maiusc+Tab` | cambia estremo |
| `F` | aggancia l'estremo al battello o al contatto più vicino |
| `Invio`, `Tastierino Invio` | fissa il tratto sul lucido; K toglie l'ultimo |

**Carta: segni (7)**

| Tasti | Che cosa fa |
|---|---|
| le frecce | muovono il cursore (anche il clic); Maiusc con su e giù, il raggio della zona |
| `Tab`, `Maiusc+Tab` | l'affiliazione |
| `[`, `]` | il tipo: subacqueo, superficie, aereo, nota, croce, zona |
| `Invio`, `Tastierino Invio` | mette il segno e apre la scritta; K toglie l'ultimo |
| la tastiera (si scrive) | la scritta del segno (Invio o Esc la chiudono) |
| `Backspace` | cancella l'ultima lettera della scritta |

**Carta: regolo di Ekelund (Q)**

| Tasti | Che cosa fa |
|---|---|
| le frecce | rotta e distanza (Maiusc più veloce) |
| `[`, `]` | la velocità, mezzo nodo alla volta |
| `Invio`, `Tastierino Invio` | passa la soluzione al MATE |

**Armi (F7)**

| Tasti | Che cosa fa |
|---|---|
| `1`-`7` | sceglie il tubo |
| `8`, `9` | Enhanced: la quota del siluro scelto via filo, 15 m più su o più giù |
| `Tab`, `Maiusc+Tab` | Enhanced: il campo delle impostazioni del tubo |
| `[`, `]` | Enhanced: il valore del campo |
| `Ctrl+V` | Enhanced: nel tubo scelto l'arma successiva della rastrelliera |
| `Z`, `X` | il siluro via filo 10° a sinistra o a dritta (anche dalle altre stazioni) |
| `C` | classico: CERCA; Enhanced: la testa attiva |
| `D` | classico: DETONATE; Enhanced: lo spegnimento |
| `Y` | Enhanced: taglia il filo |

**Armi (F7): la pagina di tiro dei missili (Enhanced)**

| Tasti | Che cosa fa |
|---|---|
| `Su`, `Giu` | la riga della consolle (fonte, modo, scatola, missione...) |
| `Sinistra`, `Destra` | il valore della riga |
| `Maiusc+L` | alza o abbassa il coperchio del FIRE |
| `L` | il lancio, solo col coperchio alzato e il tubo pronto (RDY); se no il motivo del rifiuto |
| `I`, `O` | la scala della carta di tiro |
| `C` | rimette la carta di tiro al centro |
| `Tab`, `Maiusc+Tab` | torna alla pagina dei siluri |
| clic | su una riga la sceglie, sulla riga scelta cambia il valore; sul FIRE lancia |

**Sonar (F8)**

| Tasti | Che cosa fa |
|---|---|
| `1`-`4` | filtro, impulso attivo, filare o recuperare l'antenna rimorchiata, ANALIZZA |
| `5`-`9` | Enhanced: sensore, fascio del LOFAR, integrazione, AUTO, modo del cursore: lettura, differenza, armonico, DEMON, RIGHE, STORIA (§3.3-3.4) |
| le frecce | Enhanced: il cursore, su e giù nel tempo, sinistra e destra in frequenza (Maiusc più veloce); nel modo DEMON l'asse e le pale, nel modo RIGHE la riga, nel modo STORIA il tempo |
| `Z`, `X` | Enhanced: il fascio fissato a mano, 5° a sinistra o a dritta |
| `Tab`, `Maiusc+Tab`, `[`, `]` | Enhanced: il campo e il valore (pavimento, tetto, palette, scala dei tempi, Doppler della libreria, la maschera delle righe proprie; nel modo 1989 la persistenza del fosforo) |
| `I`, `O` | Enhanced: ingrandisce o riduce il LOFAR in frequenza |
| `T` | Enhanced: un tracker nuovo sul fascio puntato |
| `K` | Enhanced: toglie il tracker seguito |
| `F` | Enhanced: aggancia o sgancia il verniero alla riga del cursore; nel modo DEMON la serie trovata, nel modo RIGHE il verniero alla riga scelta, nel modo STORIA «quando è comparsa» |
| `Q` | Enhanced: il modo della cuffia |
| `E` | Enhanced: registra l'ascolto del fascio in un file WAV |
| `W`, `J` | Enhanced: un'annotazione sulla cascata al cursore; toglie l'ultima |
| `S`, `Y`, `C` | Enhanced: salva lo spettro in uno slot; lo slot successivo; il confronto (§3.9) |
| `B` | Enhanced: apre o chiude la libreria di riferimento |
| `Invio`, `Tastierino Invio` | Enhanced: toglie lo spettro della libreria sovrapposto al LOFAR |

**Sonar: libreria di riferimento (B)**

| Tasti | Che cosa fa |
|---|---|
| `Tab`, `Maiusc+Tab` | la scheda |
| `Su`, `Giu` | la voce |
| `Invio`, `Tastierino Invio` | sovrappone la voce al LOFAR |

**Radio (F2)**

| Tasti | Che cosa fa |
|---|---|
| `1`-`4` | classico: periscopio, antenna, messaggio precedente e successivo |
| `1`-`9` | Enhanced: antenna, filo galleggiante, messaggio precedente e successivo, solo gli ordini, tipo di trasmissione, trasmetti, scarico via satellite (688), boa SLOT (§4.7) |
| `[`, `]` | Enhanced: il ritardo della boa SLOT |
| `Invio`, `Tastierino Invio` | Enhanced: verifica l'autenticazione del messaggio scelto |

### A.17 In mare, nella sandbox

La vista dal vivo (§12.5): lo spettatore guarda e non comanda. È la carta del replay mentre la partita va avanti, con a destra il registro dell'IA e la scheda dell'unità scelta. La partita finisce da sola alla durata scelta o all'esito; poi c'è il debriefing con tutti gli eventi e la verità, fuori carriera.

| Tasti | Che cosa fa |
|---|---|
| `P` | pausa |
| `Pag Su`, `Pag Giu`, `+`, `=`, `Tastierino +`, `-`, `Tastierino -` | raddoppia o dimezza la compressione del tempo, fino a 256× |
| `V` | la lettura: la verità, il quadro della NATO, il quadro del Patto, il quadro dell'unità scelta; il filtro del registro segue la parte |
| `C` | nel quadro di una parte, la verità delle altre in grigio sotto, per confronto |
| `Tab`, `Maiusc+Tab` | l'unità successiva o precedente (le armi no) |
| `S` | la carta segue l'unità scelta |
| `Sinistra`, `Destra` | l'evento precedente o successivo del registro filtrato (tenuto, continua); dopo l'ultimo si torna al vivo |
| `Invio`, `Tastierino Invio` | torna al vivo dopo aver guardato indietro |
| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore |
| `G` | il pannello degli interventi del vetrino di Dio (§12.5): il gruppo, l'intervento, l'unità o il punto (un clic sulla carta), i parametri, quando; Tab scorre, le frecce cambiano il valore, Invio sceglie, Esc torna indietro (dal primo passo chiude il pannello) |
| `Backspace` | annulla l'ultimo intervento dato dal pannello: la partita si rigioca dall'inizio senza |
| `Esc` | lascia la partita (col pannello aperto, torna indietro nel pannello) |
| clic | sulla linea del tempo, guarda indietro a quell'istante (ADESSO torna al vivo); sulla riga in alto del registro, il filtro per parte (a sinistra) e per tipo (a destra); ADESSO a destra nella fascia del tempo e i tre tasti del piede (FINE, CONFRONTO, ADESSO); su una riga del registro, la carta va all'evento e sceglie l'unità; sul titolo della scheda, segue l'unità; trascinare sposta la carta |
| `Maiusc` tenuto | tenuto mentre si clicca una riga del registro: anche il tempo va a quell'evento |

### A.18 Debriefing

| Tasti | Che cosa fa |
|---|---|
| `Sinistra`, `Destra` | l'evento precedente o successivo |
| `Invio`, `Tastierino Invio` | l'evento successivo; dall'esito, il rapporto ACINT (se c'è) o chiude |
| `E` | va all'esito |
| `Tab` | tutti gli eventi, o i soli momenti salienti |
| `V` | la verità (le unità com'erano davvero, gli eventi non noti allora) o il quadro di bordo |
| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore |
| `R` | riprende la missione dall'evento scelto (Invio conferma, Esc annulla) |
| `P` | il replay completo |
| `S` | le statistiche della missione |
| `Esc` | chiude il debriefing |
| clic | i quattro tasti del piede (Esc, R, P, E); una riga dell'elenco va a quell'evento; trascinare sposta la carta |

### A.19 Esito

| Tasti | Che cosa fa |
|---|---|
| `Invio`, `Tastierino Invio` | la pagina successiva (rapporto, decorazioni, promozioni); dall'ultima, chiude |
| `Esc` | la pagina precedente; dalla prima, torna al debriefing |
| clic | i due tasti del piede: indietro (o il debriefing) e avanti (o fine) |

### A.20 Statistiche della missione

Due pagine: il report della missione e le sue imprese (ghiaccio, notte, pedinamento, soccorso, riconoscimento, mine, ASW aereo, armi e navigazione).

| Tasti | Che cosa fa |
|---|---|
| `Tab`, `Maiusc+Tab` | il report o le imprese |
| clic | nel piede delle imprese: il report o indietro |
| `Invio`, `Tastierino Invio`, `Esc` | torna al debriefing |

### A.21 Statistiche della sandbox

S dal debriefing dello spettatore (§12.6).

| Tasti | Che cosa fa |
|---|---|
| `Tab` | la scheda successiva o precedente |
| `Su`, `Giu` | scorre la tabella |
| `Invio`, `Tastierino Invio`, `Esc` | torna al debriefing |
| clic | sulle linguette, la scheda; su una riga con l'ora, il replay da quell'istante |

### A.22 Serie della sandbox

Dopo S nelle regolazioni, o con --serie (§12.8).

| Tasti | Che cosa fa |
|---|---|
| `Tab` | la pagina: esiti, per parte, istogrammi |
| `Su`, `Giu` | per parte: scorre la tabella; istogrammi: il numero da mostrare |
| `Invio`, `Tastierino Invio`, `Esc` | torna alle regolazioni |
| clic | sulle linguette, la pagina; nel piede, la pagina seguente o indietro; negli istogrammi, su una barra di partita, il suo replay |

### A.23 Replay

| Tasti | Che cosa fa |
|---|---|
| `P` | ferma o riprende |
| `Pag Su`, `Pag Giu`, `+`, `=`, `Tastierino +`, `-`, `Tastierino -` | raddoppia o dimezza la velocità |
| `Sinistra`, `Destra` | salta di 30 s indietro o avanti (tenuto, continua) |
| `Tab`, `Maiusc+Tab` | l'evento successivo o precedente |
| `G` | l'intervento del vetrino di Dio successivo (i triangoli magenta sopra la linea del tempo) |
| `Maiusc` tenuto | tenuto con G: l'intervento precedente |
| `V` | la lettura: la verità o il quadro del comandante |
| `I`, `O` | ingrandisce o riduce la carta |
| rotella | sopra la carta la ingrandisce o la riduce, attorno al punto sotto il cursore |
| `Invio`, `Tastierino Invio` | riprende la missione da quell'istante |
| `Esc` | chiude il replay |
| clic | sulla linea del tempo, quell'istante; SEGUI torna a seguire il battello; trascinare sposta la carta |

### A.24 Editor di missione

| Tasti | Che cosa fa |
|---|---|
| `1`-`8` | la scheda: missione, unità, rotte, zone, obiettivi, regole, briefing, prova (cap. 11) |
| `S` | salva |
| `E` | esporta il pacchetto da scambiare |
| `G` | salva e prova in mare (prima vanno corretti gli errori) |
| `X` | nella scheda unità toglie l'unità scelta; nella scheda rotte, l'ultimo punto |
| `I`, `O` | ingrandisce o riduce la carta |
| le frecce | spostano la carta (Maiusc più veloce); col puntatore sulle schede, su e giù scorrono |
| `Maiusc` tenuto | la carta si sposta più in fretta |
| clic | sulla carta sceglie (trascinare sposta l'unità, la zona o la carta); nelle schede cambia il campo in avanti; negli avvisi mostra l'unità; nel piede: indietro, salva, esporta, prova in mare |
| clic destro | sulla carta aggiunge (unità, punto di rotta, zona, centro dell'obiettivo); nelle schede cambia il campo all'indietro |
| `Esc` | torna al menu; con modifiche non salvate, la prima volta avvisa |

**Mentre si scrive un testo**

| Tasti | Che cosa fa |
|---|---|
| la tastiera (si scrive) | il testo |
| `Backspace` | cancella l'ultima lettera |
| `Invio`, `Tastierino Invio` | conferma |
| `Esc` | annulla |
<!-- fine: tasti_schermate -->

## B. Glossario e sigle

I termini italiani seguono il glossario della traduzione (`docs/traduzione/glossario.md`). Le sigle più usate in
questo manuale:

| Sigla | Significato |
|---|---|
| ADCAP | Advanced Capability: la versione del Mk 48 entrata in servizio nel 1988 |
| AOB | angolo di presentazione del bersaglio (angle on the bow) |
| ASW | lotta antisommergibile |
| BTR | cascata dei rilevamenti a banda larga (bearing-time record) |
| CPA, TCPA | punto e istante del minimo avvicinamento |
| CZ | zona di convergenza |
| DEMON | analisi dell'inviluppo del rumore di cavitazione (demodulated noise) |
| DICASS, DIFAR | boe sonore: attiva comandata, passiva direzionale |
| DTG, GDO | gruppo data-ora |
| ELF, VLF, HF, UHF | bande radio: estremamente bassa, bassissima, alta, ultra alta frequenza |
| EMCON | silenzio elettromagnetico, disciplina delle emissioni |
| ESM | misure di supporto elettronico: ascoltare i radar |
| KSF | fattore d'urto (keel shock factor) |
| LOFAR | analisi spettrale a bassa frequenza (low frequency analysis and recording) |
| MAD | rilevatore di anomalie magnetiche |
| MATE | strumento manuale della TMA (manual adaptive target estimator) |
| NONSUB, POSSUB, PROBSUB, CERTSUB | passi della classificazione: non sottomarino, possibile, probabile, certo |
| NTDS | simbologia tattica della US Navy: amico, ostile, ignoto, neutrale |
| NWP | pubblicazioni di dottrina della US Navy (naval warfare publication) |
| ROE | regole d'ingaggio |
| RTE | distanza di corsa del siluro prima dell'attivazione della testa |
| SCRAM | spegnimento d'emergenza del reattore |
| SOSUS | rete di idrofoni fissi sul fondo |
| SSIXS | trasmissioni ai sottomarini via satellite |
| SVP | profilo di velocità del suono |
| TMA | analisi del moto del bersaglio |
| VMF | marina militare sovietica (Voenno-morskoj flot) |

## C. Dal manuale del 1989 all'Enhanced: cosa è diverso

Il manuale originale tradotto resta valido per il classico. Nell'Enhanced cambiano queste cose (in ordine di capitolo
del manuale originale).

| Manuale del 1989 | Enhanced | Capitolo |
|---|---|---|
| La Centrale (CONN) ferma il tempo | la Centrale non ferma il tempo; c'è la pausa (P) | 2 |
| Telegrafo a 7 posizioni, timone a ratei | 9 posizioni, barra in gradi fino a 35° con la dinamica vera | 2 |
| Emersione d'emergenza istantanea | esaurimento con l'aria delle bombole, rumoroso, che in profondità rende poco | 2, 7 |
| Il sonar dà un elenco di contatti | cascate, LOFAR, DEMON, tracker, cuffia, classificazione con la confidenza | 3 |
| La schiera si fila in 60 s e si perde oltre una velocità | in più: sorda oltre 17 nodi, cieca in virata, ambigua fra dritta e sinistra | 3 |
| Un periscopio, due ingrandimenti | due periscopi sul 688 (Type 2 e Type 18), modi di visione, esposizione e piuma, onde sulla finestra | 4 |
| ESM: un livello di segnale | intercettazioni con rilevamento, identificazione, tendenza e rischio | 4 |
| Radio: l'archivio dei messaggi | trasmissioni circolari, antenne, satellite, ROE, zone, autenticazione, trasmissioni rilevabili | 4 |
| Carta con lo sfondo del fondale | carte d'epoca NATO e sovietica, strumenti di carteggio, TMA col MATE | 5 |
| Ricarica dei tubi in 20 s | ricarica in minuti, rumorosa, più l'armamento di 20 s | 6 |
| Siluro: SELEZ., DIRECT, CERCA, DETONATE | impostazioni reali, guida via filo, spegnimento; DETONATE non c'è | 6 |
| Danni: percentuali per sistema | compartimenti, falle, incendi, squadre, tredici sistemi graduati | 7 |
| L'elicottero gira sul punto | ASW aereo con boe, sonar a immersione, MAD, siluri | 8 |
| Sempre sereno | meteo del teatro e della stagione, giorno e notte veri | 9 |
| Un rapporto d'esito | debriefing, replay, carriera, decorazioni, riconoscimenti, statistiche | 10 |
| Dieci missioni | più missioni rapide, editor e libreria | 11 |
| La scheda sonora (AdLib, Tandy, Covox) si sceglie all'installazione | la musica si sceglie nel menu (M): FM, MT-32, i titoli del 1989 per AdLib, Tandy o Covox, o spenta | 1 |
| Ctrl+R: scritte russe dell'Alfa sì o no | Ctrl+R: il radar del battello; le scritte dell'Alfa sono in caratteri latini | 2, 4 |
| Forrest Sherman nella prima missione | Spruance (il Forrest Sherman era già radiato nel 1989) | 1 |

## D. Fonti

Le fonti di ogni dato sono nei dossier di ricerca (`docs/ricerca/01`-`24`), citati per paragrafo nelle schede del
Museo e nei capitoli del volume 2. Il volume 3 raccoglie la bibliografia completa.

**Le foto.** Le foto d'epoca del manuale (volume 3, `figure/foto/`) sono tutte in pubblico dominio o CC0, tranne una:
la foto del Be-12 «Chayka» (scheda «Be-12 Mail», capitolo 13 del volume 3), di Alan Wilson, è sotto licenza
[CC BY-SA 2.0](https://creativecommons.org/licenses/by-sa/2.0/). La condivisione allo stesso modo vale per quella foto
e per le sue versioni ridotte, non per il resto del manuale. Originale:
[Beriev Be-12 Chayka 25 yellow](https://commons.wikimedia.org/wiki/File:Beriev_Be-12_Chayka_25_yellow_(10071687574).jpg).

**I nomi sulle carte.** I toponimi vengono in parte da Natural Earth (https://www.naturalearthdata.com), dati in
pubblico dominio, in parte dai dossier di ricerca del progetto.

**I suoni.** Le registrazioni che la cuffia e il battello usano come tessiture (balene, vento, pioggia, navi, aerei,
elicotteri) sono in pubblico dominio, CC0 o CC BY. L'elenco completo, con la licenza e l'attribuzione di ognuna, è
nel volume 2, capitolo C8 («Audio», tabella dei campioni): vale anche come credito per le voci CC BY.
