# Schermate dell'Enhanced: elenco dei comandi

Scritto da `tools/schermate/inventario.py`. Ogni immagine è un'istantanea senza finestra a 640×480, ingrandita 2× senza filtri. Le partite di servizio (la traccia di SCENE1A col lancio, la serie di S5) le rifà lo script. L'analisi a occhio è in `indice.md`.


## Menu

| Schermata | Immagine | Comando |
|---|---|---|
| Scelta della missione: originali | [menu_originali.png](menu_originali.png) | `melchior688 --schermata scelta --istantanea X.bmp` |
| Scelta della missione: missioni rapide | [menu_rapide.png](menu_rapide.png) | `melchior688 --schermata scelta-rapida --istantanea X.bmp` |
| Scelta della missione: editor, libreria e vetrini | [menu_editor.png](menu_editor.png) | `melchior688 --schermata scelta-editor --istantanea X.bmp` |
| Regolazioni della sandbox (vetrino) | [regolazioni.png](regolazioni.png) | `melchior688 --schermata regolazioni --missione SCENE1A --istantanea X.bmp` |

## Missione

| Schermata | Immagine | Comando |
|---|---|---|
| Sala briefing (SCENE1A) | [briefing.png](briefing.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --schermata briefing --istantanea X.bmp` |
| Sala briefing (libreria, LIB_S03) | [briefing_libreria.png](briefing_libreria.png) | `melchior688 --cartella-missioni data/missioni/libreria --missione LIB_S03_QUARANTENA --regola simulazione --secondi 60 --schermata briefing --istantanea X.bmp` |
| Sala siluri: Mk 48 | [siluri_mk48.png](siluri_mk48.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --schermata siluri --arma mk48 --istantanea X.bmp` |
| Sala siluri: mina Mk 67 | [siluri_mk67.png](siluri_mk67.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --schermata siluri --arma mk67 --istantanea X.bmp` |

## Dopo

| Schermata | Immagine | Comando |
|---|---|---|
| Debriefing: l'esito (evento 0) | [debriefing_esito.png](debriefing_esito.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 420 --ordini @ordini:lancio --schermata debriefing --evento 0 --istantanea X.bmp` |
| Debriefing: un evento con la minicartolina | [debriefing_evento.png](debriefing_evento.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 420 --ordini @ordini:lancio --schermata debriefing --evento 2 --istantanea X.bmp` |
| Esito: il rapporto (pagina 1) | [esito_rapporto.png](esito_rapporto.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 420 --ordini @ordini:lancio --schermata esito --pagina 1 --istantanea X.bmp` |
| Esito: la cerimonia (pagina 2) | [esito_cerimonia.png](esito_cerimonia.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 420 --ordini @ordini:lancio --schermata esito --pagina 2 --istantanea X.bmp` |

## Stazioni 688

| Schermata | Immagine | Comando |
|---|---|---|
| Periscopio (posto di comando) | [688_periscopio.png](688_periscopio.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --ordini @ordini:periscopio --istantanea X.bmp` |
| F2 Sala radio ed ESM | [688_radio.png](688_radio.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --ordini @ordini:periscopio --stazione 2 --istantanea X.bmp` |
| F3 Stato del battello | [688_stato.png](688_stato.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 3 --istantanea X.bmp` |
| F4 Manovra | [688_manovra.png](688_manovra.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 4 --istantanea X.bmp` |
| F6 Navigazione (carta VGA) | [688_carta.png](688_carta.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 6 --istantanea X.bmp` |
| F6 Carta, stile nautico | [688_carta_nautica.png](688_carta_nautica.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 6 --stile-carta nautica --istantanea X.bmp` |
| F6 Carta, stile foglio | [688_carta_foglio.png](688_carta_foglio.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 6 --stile-carta foglio --istantanea X.bmp` |
| F6 Carta col fondale vero | [688_carta_vera.png](688_carta_vera.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 6 --carta vera --istantanea X.bmp` |
| F6 Carta col MATE della TMA | [688_carta_mate.png](688_carta_mate.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 300 --stazione 6 --mate --istantanea X.bmp` |
| F7 Armi | [688_armi.png](688_armi.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 7 --istantanea X.bmp` |
| F8 Sonar moderno (cascata, LOFAR) | [688_sonar.png](688_sonar.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 200 --stazione 8 --istantanea X.bmp` |
| F8 Sonar nell'aspetto 1989 | [688_sonar_1989.png](688_sonar_1989.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 200 --stazione 8 --sonar 1989 --istantanea X.bmp` |
| F9 Centrale | [688_centrale.png](688_centrale.png) | `melchior688 --missione SCENE1A --regola simulazione --secondi 60 --stazione 9 --istantanea X.bmp` |

## Stazioni Alfa

| Schermata | Immagine | Comando |
|---|---|---|
| Periscopio dell'Alfa | [alfa_periscopio.png](alfa_periscopio.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --ordini @ordini:periscopio --istantanea X.bmp` |
| F2 Radio dell'Alfa | [alfa_radio.png](alfa_radio.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --stazione 2 --istantanea X.bmp` |
| F3 Stato dell'Alfa | [alfa_stato.png](alfa_stato.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --stazione 3 --istantanea X.bmp` |
| F4 Manovra dell'Alfa (pulti) | [alfa_manovra.png](alfa_manovra.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --stazione 4 --istantanea X.bmp` |
| F6 Carta VMF | [alfa_carta.png](alfa_carta.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --stazione 6 --istantanea X.bmp` |
| F7 Armi dell'Alfa | [alfa_armi.png](alfa_armi.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 60 --stazione 7 --istantanea X.bmp` |
| F8 Sonar sovietico | [alfa_sonar.png](alfa_sonar.png) | `melchior688 --missione SCENE2B --regola simulazione --secondi 200 --stazione 8 --istantanea X.bmp` |

## Replay e statistiche

| Schermata | Immagine | Comando |
|---|---|---|
| Replay, quadro del giocatore | [replay_quadro.png](replay_quadro.png) | `melchior688 --schermata replay --replay @traccia --secondi 300 --istantanea X.bmp` |
| Replay, verita' | [replay_verita.png](replay_verita.png) | `melchior688 --schermata replay --replay @traccia --secondi 300 --pagina 2 --istantanea X.bmp` |
| Statistiche della missione | [statistiche.png](statistiche.png) | `melchior688 --schermata statistiche --replay @traccia --istantanea X.bmp` |
| Statistiche cumulate | [statistiche_cumulate.png](statistiche_cumulate.png) | `melchior688 --schermata statistiche-cumulate --istantanea X.bmp` |

## Carriera

| Schermata | Immagine | Comando |
|---|---|---|
| Bacheca dei riconoscimenti | [bacheca.png](bacheca.png) | `melchior688 --schermata bacheca --istantanea X.bmp` |
| Albo dei comandanti | [albo.png](albo.png) | `melchior688 --schermata albo --istantanea X.bmp` |
| Fascicolo di un comandante (pagina 1) | [fascicolo.png](fascicolo.png) | `melchior688 --schermata fascicolo --evento 1 --pagina 1 --istantanea X.bmp` |

## Consultazione

| Schermata | Immagine | Comando |
|---|---|---|
| Museo: la prima voce | [museo.png](museo.png) | `melchior688 --schermata museo --istantanea X.bmp` |
| Museo: l'Alfa | [museo_alfa.png](museo_alfa.png) | `melchior688 --schermata museo --voce sottomarino.alfa --istantanea X.bmp` |
| Museo: il Mk 48, seconda immagine | [museo_mk48.png](museo_mk48.png) | `melchior688 --schermata museo --voce siluro.mk48 --evento 2 --istantanea X.bmp` |

## Editor

| Schermata | Immagine | Comando |
|---|---|---|
| Editor di missione, scheda 1 | [editor_1.png](editor_1.png) | `melchior688 --schermata editor --pagina 1 --istantanea X.bmp` |
| Editor di missione, scheda 2 | [editor_2.png](editor_2.png) | `melchior688 --schermata editor --pagina 2 --istantanea X.bmp` |
| Editor di missione, scheda 3 | [editor_3.png](editor_3.png) | `melchior688 --schermata editor --pagina 3 --istantanea X.bmp` |
| Editor di missione, scheda 4 | [editor_4.png](editor_4.png) | `melchior688 --schermata editor --pagina 4 --istantanea X.bmp` |
| Editor di missione, scheda 5 | [editor_5.png](editor_5.png) | `melchior688 --schermata editor --pagina 5 --istantanea X.bmp` |
| Editor di missione, scheda 6 | [editor_6.png](editor_6.png) | `melchior688 --schermata editor --pagina 6 --istantanea X.bmp` |
| Editor di missione, scheda 7 | [editor_7.png](editor_7.png) | `melchior688 --schermata editor --pagina 7 --istantanea X.bmp` |
| Editor di missione, scheda 8 | [editor_8.png](editor_8.png) | `melchior688 --schermata editor --pagina 8 --istantanea X.bmp` |

## Sandbox

| Schermata | Immagine | Comando |
|---|---|---|
| Vetrino dal vivo (S5, 600 s) | [sandbox_vivo.png](sandbox_vivo.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata vivo --istantanea X.bmp` |
| Statistiche tecniche della sandbox, pagina 1 | [sandbox_statistiche_1.png](sandbox_statistiche_1.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata statistiche-sandbox --pagina 1 --istantanea X.bmp` |
| Statistiche tecniche della sandbox, pagina 2 | [sandbox_statistiche_2.png](sandbox_statistiche_2.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata statistiche-sandbox --pagina 2 --istantanea X.bmp` |
| Statistiche tecniche della sandbox, pagina 3 | [sandbox_statistiche_3.png](sandbox_statistiche_3.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata statistiche-sandbox --pagina 3 --istantanea X.bmp` |
| Statistiche tecniche della sandbox, pagina 4 | [sandbox_statistiche_4.png](sandbox_statistiche_4.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata statistiche-sandbox --pagina 4 --istantanea X.bmp` |
| Statistiche tecniche della sandbox, pagina 5 | [sandbox_statistiche_5.png](sandbox_statistiche_5.png) | `melchior688 --sandbox --cartella-missioni data/missioni/prove --missione PROVA_S5_CONVOGLIO --seme 1 --secondi 600 --schermata statistiche-sandbox --pagina 5 --istantanea X.bmp` |
| Serie della sandbox, pagina 1 | [serie_1.png](serie_1.png) | `melchior688 --serie @serie --schermata serie --pagina 1 --istantanea X.bmp` |
| Serie della sandbox, pagina 2 | [serie_2.png](serie_2.png) | `melchior688 --serie @serie --schermata serie --pagina 2 --istantanea X.bmp` |
| Serie della sandbox, pagina 3 | [serie_3.png](serie_3.png) | `melchior688 --serie @serie --schermata serie --pagina 3 --istantanea X.bmp` |

## Da aggiungere (non si fotografano senza finestra)

| Schermata | Perché |
|---|---|
| Titolo e menu iniziale dell'Enhanced | la sequenza d'avvio con la musica; --immagine mostra solo le immagini del 1989 |
| Scelta della missione: scheda libreria e scheda importate | --schermata ha solo scelta, scelta-rapida e scelta-editor |
| Scelta della missione con una missione selezionata e il dettaglio | manca un'opzione per la riga scelta |
| Museo: l'indice delle voci e la ricerca | --schermata museo apre direttamente una voce |
| Albo d'onore storico | c'e' solo l'anteprima del kit (anteprima_albo_onore), non la schermata del gioco |
| Opzioni e tasti | nessuna --schermata |
| Pausa, messaggi radio da autenticare, dialoghi di conferma | compaiono solo in finestra |
| Vista dal vivo della sandbox in lettura parte o unita' | --schermata vivo disegna solo la verita' |
| Periscopio a 6x e nei modi notte e termico | servono i tasti: --tasti-a-tempo li da', ma non c'e' un'opzione diretta |
