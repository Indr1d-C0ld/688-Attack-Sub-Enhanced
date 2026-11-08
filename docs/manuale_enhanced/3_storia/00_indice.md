# Volume 3 — Storia e unità

*Indice approvato (26/10/2026).* Il volume racconta: il gioco del 1989, il mondo che simula, le unità e
le armi. Quello che è interpretazione (giudizi, intenzioni degli autori, accoglienza) è segnato come tale; i fatti
hanno la fonte.

## Parte A — Il gioco

1. **688 Attack Sub, 1989.** Gli autori (John W. Ratcliff e la squadra), Electronic Arts, l'uscita e le versioni,
   l'accoglienza della stampa, il seguito (*688(I) Hunter/Killer*, 1997). Da ricercare con le fonti dell'epoca.
   Fra le curiosità, gli errori e le incoerenze dell'originale: i cacciatorpediniere *Forrest Sherman* della prima
   missione, già radiati nel 1989 (nell'Enhanced in mare c'è lo *Spruance*, ma il briefing tradotto resta fedele), e
   gli stretti sbagliati delle missioni 7 e 10.
2. **Com'era fatto.** Quello che il reverse engineering ha trovato: i file, gli eseguibili compressi, la VM degli
   scenari, le stazioni, il modello dei sensori, i suoni, la protezione anticopia della sala radio. Dalle analisi in
   `docs/01`, `docs/04` e `docs/originale`.
3. **L'edizione Enhanced.** Perché e come è nata, i requisiti, le scelte (classico identico, simmetria con l'IA,
   estetica VGA), il metodo (dossier di ricerca prima del codice), le sessioni di lavoro.
4. **Il progetto in numeri.** Statistiche generate: righe di codice per parte, prove, dossier, voci
   dell'enciclopedia, classi del kit 3D, missioni, achievement (generate da `tools/pipeline/manuale.py`).

## Parte B — La Guerra fredda sotto il mare

5. **Il confronto sottomarino 1955-1991.** Dalla propulsione nucleare al silenziamento; la corsa fra rumore e
   ascolto; il caso Walker-Toshiba e il salto dei sovietici negli anni '80.
6. **Dottrine.** La caccia americana agli SSBN, i bastioni sovietici, la barriera GIUK e il SOSUS, il pedinamento, la
   gestione dello spazio d'acqua, l'accordo INCSEA del 1972.
7. **La vita a bordo.** Equipaggi, turni, il silenzio, la sala sonar, i periscopi, le comunicazioni, i rischi.
8. **Gli incidenti.** Collisioni, perdite, recuperi: il prezzo della Guerra fredda sotto il mare.

## Parte C — I teatri

9. **I sette teatri del gioco**, con la carta, il fondale, la climatologia, le basi e il loro ruolo nel 1989:
   Fær Øer, Mare di Barents, Stretto di Gibilterra, Canale d'Otranto, Kunashir e le Curili, Mar Celtico, Baltico.

## Parte D — Gli episodi

10. **Episodi della Guerra fredda** (dossier 23): una scheda per episodio, dai Foxtrot di Cuba (1962) al
    *Komsomolets* (1989) e oltre, con cosa accadde, le fonti e come se ne gioca nella libreria di missioni.

## Parte E — Unità e armi

Una scheda per voce, dalle voci dell'enciclopedia (`data/motore/enciclopedia.toml`): la storia e l'impiego, l'uso in
partita, la scheda tecnica con la confidenza, le foto a licenza libera coi crediti e le immagini del gioco (sprite,
modello 3D, sagome, oculari, cascate della firma, spaccati). Le schede si generano dall'enciclopedia, come nel Museo.

11. **Sottomarini** (11 voci)
12. **Navi** (17)
13. **Aerei ed elicotteri** (6 e 5)
14. **Siluri, missili, mine ed esche** (15, 6, 5, 4)
15. **Sensori:** sonar, radar, ESM, periscopi, boe (10, 26, 2, 13, 17)
16. **Sistemi di bordo** (17)
17. **Basi** (31) e **marcatori** (4)

## Parte F — Uomini e onori

18. **Gradi e decorazioni** della US Navy e della VMF nel 1989, con le immagini delle medaglie del gioco.
19. **I comandanti storici** dell'albo d'onore: chi erano e cosa fecero.

## Appendici

- Fonti e bibliografia, per volume e capitolo.
- Crediti delle immagini (da `docs/CREDITI.md`).
- Cronologia 1945-1991.

## Stato

| Capitolo | File | Stato |
|---|---|---|
| 1. 688 Attack Sub, 1989 | `01_gioco_1989.md` | bozza (kit), 26/10/2026 |
| 5. Il confronto sottomarino | `05_confronto_sottomarino.md` | bozza (kit) |
| 6. Dottrine | `06_dottrine.md` | bozza (kit) |
| 7. La vita a bordo | `07_vita_a_bordo.md` | bozza (kit) |
| 8. Gli incidenti | `08_incidenti.md` | bozza (kit) |
| 9. I teatri | `09_teatri.md` | schede generate dall'enciclopedia |
| 10. Episodi | `10_episodi.md` | schede generate dal dossier 23 |
| 11-17. Unità e armi | `11_sottomarini.md` … `17_basi.md` | schede generate dall'enciclopedia |
| 18. Gradi e decorazioni | `18_gradi_e_decorazioni.md` | bozza, tabelle e medaglie generate |
| 19. I comandanti storici | `19_comandanti_storici.md` | schede generate dall'albo storico |
| 2. Com'era fatto | `02_com_era_fatto.md` | bozza (kit) |
| 3. L'edizione Enhanced | `03_edizione_enhanced.md` | bozza (kit) |
| 4. Il progetto in numeri | `04_progetto_in_numeri.md` | generato a ogni esecuzione di manuale.py |

Le schede delle unità le rifà `tools/pipeline/manuale.py` dall'enciclopedia (`data/motore/enciclopedia.toml`), con le
immagini in gioco di `anteprima_enciclopedia --manuale` (`figure/unita/`) e le foto dei crediti ridotte (`figure/foto/`,
con `manuale.py --foto`), le medaglie di `anteprima_cerimonia <dati> manuale <cartella>` (`figure/decorazioni/`). Il testo storico di ogni voce si corregge in `tools/pipeline/enciclopedia_testi/`, non qui.
