# 15 — Meteo e mare nell'Enhanced: piano d'implementazione

Richieste dell'utente del 25/10/2026:
- lo stato del mare in diverse forze, «con tutto quello che ne comporta per tutti, anche a livello di animazioni oltre
  che capacità e prestazioni»;
- un meteo realistico e dinamico per ogni teatro, che cambia durante la missione con il tempo e la stagione, riportato
  nel pre-briefing, e che influisce su capacità, prestazioni e sensori di tutti i mezzi: foschia e nebbia, temporali
  che rendono difficili gli elicotteri;
- eventi estremi rari ma possibili, coi fulmini resi in modo realistico;
- gli avvisi meteo via radio dalla centrale di comando, e la previsione chiesta via radio;
- la superficie del mare dinamica.

Si appoggia a:
- requisito 7 di `docs/00_ROADMAP.md`;
- `docs/ricerca/21_meteo_e_mare.md` (il «dossier 21»): contratto al §11, pezzi al §11.1, soglie raccolte e corrette
  al §8;
- note complete e dati ERA5 1981-1990 in `docs/ricerca/21_note/`.

Tutto vale **solo con la regola di simulazione**. Il classico resta identico.

## Che cosa c'è già

- Vista dell'Enhanced (kit, nei periscopi P3): mare dinamico (Pierson-Moskowitz), acqua e gocce sulla finestra, pioggia
  sull'oculare, onde della simulazione in `nucleo/onde.hpp`.
- L'acustica ricava il rumore ambiente dal vento (Wenz).
- Nella missione lo stato del mare è fisso a 3 (`STATO_MARE`); la stazione dei periscopi usa lo stesso valore.

## Pezzi e chi li fa (dossier 21 §11.1, confermati)

| Pezzo | Contenuto | Chi |
|---|---|---|
| **M0 Dati** | `tools/pipeline/meteo.py` → `data/motore/meteo.toml`, per teatro e mese; ERA5 esteso a 3-5 punti per teatro; fonte, conf e STIME su ogni valore | kit |
| **M1 Modello** | `nucleo/meteo.{hpp,cpp}` col contratto del §11: regimi, vento AR(1) e Weibull, mare che rilassa verso Pierson-Moskowitz, swell, eventi rari (Poisson), lampi, previsione con l'errore del 1989, `in(pos)`. Prove con cento anni sintetici contro le tabelle (±20%) | runtime |
| **M2 Effetti** | Le correzioni del §8 (una sola tabella Douglas, SSQ-53, sonar a immersione, MAD, testa del periscopio sott'acqua) e il meteo per passo al posto di `STATO_MARE` in acustica, radar, vista, ASW aereo, navi, sottomarino (errore di quota, affioramento, snorkel, superficie, ghiaccio), armi e mine, danni e soccorso | runtime |
| **M3 Grafica** | Cielo e nubi, foschia e nebbia, fumo di mare, pioggia e neve, lampi e fulmini (drammatici di notte), moto delle navi col periodo d'incontro, spruzzi a prua, elicotteri in hover col vento; carta con gli strati meteo | kit |
| **M4 Briefing e radio** | Pannello METEO E MARE nel pre-briefing e meteo nel debriefing; scelta nell'editor e nella missione rapida (principale). WEAX e GALE WARNING sul broadcast, richiesta di previsione col suo costo di scoperta, nomi sovietici (runtime, sala radio) | principale e runtime |
| **M5 Audio** | Pioggia e vento sul rumore ambiente e a bordo, tuono in superficie, fulmini al sonar, scariche HF in sala radio | disassemblatore |
| **M6 Taratura** | Missioni di prova per teatro e stagione; equilibrio fra la copertura del maltempo e la perdita della quota periscopica | runtime col principale |

## Ordine di lavoro

1. Il runtime pubblica per prima l'intestazione `nucleo/meteo.hpp` col contratto del §11, così kit, disassemblatore e
   principale lavorano in parallelo.
2. In parallelo M0 (kit) e M1 (runtime), poi M3 (kit) e M5 (disassemblatore) sull'intestazione.
3. M2 (runtime) e M4 (principale e runtime) sul modello.
4. M6, poi la documentazione qui e nella roadmap.

## Verifiche

- Il classico resta **IDENTICO**.
- Nell'Enhanced con il meteo «fisso: mare 3, sereno» le missioni di prova restano vicine a oggi. Col meteo dinamico le
  differenze si guardano una per una.
- Le prove nuove sono deterministiche. Le immagini di controllo coprono tutti i teatri, di giorno, di notte e col
  temporale.

## Stato

- 25/10/2026: dossier 21 fatto, piano scritto, pezzi avviati.
- M0 e M3 fatti (kit):
  - `meteo.toml` da ERA5 1981-1990 (33 punti in mare, Open-Meteo), con ERA5 e Open-Meteo nei crediti;
  - nella vista: nubi col plafond e i cumulonembi, foschia, nebbia, fumo di mare, pioggia e neve, fulmini (3-4 colpi a
    57 ms), moto delle navi col periodo d'incontro e spruzzi a prua, elicotteri in hover col vento;
  - sulla carta: barbe del vento, stato del mare, celle, ghiaccio e nebbia (livello `meteo`, spento nel classico);
  - con il meteo spento la vista è identica pixel per pixel, e il classico resta IDENTICO.
  - Manca la chiamata degli strati nella carta, che aspetta `Missione::meteo()` dal runtime.
- TV dei periscopi in bianco e nero: collegate le due chiamate di palette in main (principale).
- M5 audio fatto (disassemblatore; `nucleo/audio_meteo`, cuffia, bordo; note `21_note/e_audio_meteo.md` §8):
  - fulmini al sonar come transitori, con lo schiocco a campione esatto in cuffia;
  - a bordo il tuono (solo in superficie), i colpi di mare da mare 4 e l'acqua sul ponte;
  - in sala radio le scariche HF tarate sul CCIR, gli schianti dei lampi, l'AGC e i tweek in VLF.
  - Collegamenti: il bordo al principale (aspetta `Missione::meteo()`); la radio e i fulmini della scena al runtime.
- Audio di bordo collegato al meteo (principale): a ogni rilettura della scena `AmbienteBordo::meteo(Meteo::in(pos))`;
  per ogni lampo entro 20 km il tuono (`AmbienteBordo::tuono`), col ritardo e la superficie decisi dal bordo.
- Enciclopedia: il kit ha fissato il formato su 14 voci campione e procede con tutte le 145.
- M1, M2 e la radio di M4 fatti (runtime). Il predefinito dell'Enhanced è il meteo **semplificato** (mare 3, sereno,
  5 m/s); il dinamico si sceglie con `Missione::usa_meteo(ModoMeteo::realistico)` prima del primo passo.
- Collegamenti del principale:
  - opzione `--meteo semplificato|realistico` (e `--asw facile|normale|realistico|minaccia` per l'ASW aereo);
  - il meteo nasce già prima del briefing (`meteo_enhanced` è idempotente);
  - vista: `applica_meteo` dallo stato nel punto del giocatore, coi lampi dell'ultimo mezzo minuto; la luce sotto le
    nubi (`cielo::luce_sotto_le_nubi`) e il lampo in palette (`cielo::lampo`); al periscopio l'onda vera
    (`meteo_qui().hs_m()`);
  - briefing: nel riquadro delle informazioni il meteo all'inizio e la previsione a 12 ore (WEAX per il 688,
    PROGNOZ per l'Alfa), con l'errore del 1989.
  - Verificato: nel briefing della 7A (Kunashir) vento da 286° forza 7, mare 3, e la previsione; al periscopio cielo
    coperto e foschia nelle missioni 1B e 4A.
- 26/10/2026, superficie del mare dinamica al periscopio (kit, `grafica/oculare`):
  - il mare vicino è un campo d'altezze percorso colonna per colonna: le creste coprono il mare dietro e, sopra l'occhio,
    l'orizzonte e il cielo (orizzonte frastagliato da mare 5, muri d'acqua con la testa bassa);
  - navi e colonne d'acqua dietro le creste più vicine spariscono (`ProfiloMare`, dentro `Periscopio::disegna`);
  - schiuma per forza Beaufort (dossier 21 §9): creste di Monahan, strisce lungo il vento da forza 7, spruzzaglia
    da forza 8, mare bianco da forza 10;
  - tavole in `extracted/meteo/mare_dinamico/`; il classico resta IDENTICO.
- Da fare (principale): la scelta del meteo nel menu e nella missione rapida, il meteo nel debriefing, il livello
  meteo della carta acceso nell'Enhanced.
