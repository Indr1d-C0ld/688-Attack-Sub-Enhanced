# aggiorna il rapporto finale e proposta.md con le partite alla durata scritta: ogni sostituzione deve trovare il testo una volta sola
import pathlib
def sost(p, coppie):
    t = p.read_text()
    for a, b in coppie:
        assert t.count(a) == 1, a
        t = t.replace(a, b)
    p.write_text(t)
N = pathlib.Path('/home/randolph/.config/Claude/side-session-notes/local_54579f03-2ae3-4feb-a687-2bc2c6456a76/quadro-radio-rapporto-05-11.md')
sost(N, [
("# Quadro radio: rapporto finale (05/11, circa 19:00)", "# Quadro radio: rapporto finale (05/11, aggiornato alle 20:40)"),
("Sostituisce la versione delle 17:10.", "Sostituisce la versione delle 17:10 e quella delle 19:00: ora la tabella delle missioni arriva alla durata scritta e il catalogo è chiuso."),
("e il dato è stato usato una sola volta in 18 partite.", "e il dato è stato usato due volte in 18 partite, entrambe in I07 seme 1."),
("| I06 | 10 800 | **7200, tagliata** | 4, 5 | 1 | 15, 12 | 0 |", "| I06 | 10 800 | 10 800 | 6, 8 | 1 | 18, 24 | 0 |"),
("| I07 | 14 400 | **7200, tagliata** | 1, 2 | 1 | 8, 3 | **1, 0** |", "| I07 | 14 400 | 14 400 | 6, 4 | 1 | 23, 12 | **2, 0** |"),
("| S03 | 10 800 | **7200, tagliata** | 4, 3 | 1 | 0, 2 | 0 |", "| S03 | 10 800 | 10 800 | 7, 7 | 1 | 1, 3 | 0 |"),
("| S07 | 9000 | **7200, tagliata** nel seme 1; 3952 nel seme 2 (esito 2) | 2, 1 | 1 | 0, 0 | 0 |", "| S07 | 9000 | 9000; 3952 (esito 2) | 3, 1 | 1 | 0, 0 | 0 |"),
("| S15 | 10 800 | **7200, tagliata** | 1, 1 | 1 | 0, 1 | 0 |", "| S15 | 10 800 | 10 800 | 2, 2 | 1 | 0, 1 | 0 |"),
("- **Taglio:** avevo chiesto la durata intera, ma il tetto dei 7200 s le ha fermate. Non l'ho indagato, come chiesto: i numeri di quelle cinque sono parziali, e il taglio non l'ho scelto io.\n",
 "- **Durata:** I06, I07, S03, S07 e S15 sono rigiocate con la durata scritta nel codice sandbox (`durata_min`, il tetto dei 7200 s non era un difetto). Fino a 7199 s le righe QUADRO coincidono con quelle della misura tagliata, in tutte e 10 le partite. S15 seme 1 su `fisso2` puro dà la stessa impronta del binario di misura (`56b698e217202a51`).\n"),
("- **Dove si vede:** S08 e I06 copiano di più, I07 e I05 meno. Il dato è usato una volta su 18, in I07 seme 1 (id 7, a 5736 s: salita a 5451, copie a 5731).",
 "- **Dove si vede:** S08, I06 e I07 copiano di più. Il dato è usato due volte, tutte e due in I07 seme 1 e dallo stesso battello (id 7, a 5736 s e a 13 104 s, circa 5 s dopo la copia)."),
("- **I02:** finisce 50-60 s dopo col quadro acceso, stesso esito.\n",
 "- **I02:** finisce 50-60 s dopo col quadro acceso, stesso esito.\n- **Limite:** il confronto con lo spento arriva solo a 7200 s, e non l'ho rifatto, come chiesto.\n- **Oltre i 7200 s, senza confronto:** nessuna nuova perdita e nessuna partita chiaramente peggiorata. Tutte chiudono a tempo scaduto (esito 21), tranne S07 seme 2, che resta affondata a 3888 s con la stessa impronta. I07 seme 1 passa da 0 a 2 lanci dell'IA, non dal battello 7; I07 seme 2 ne aveva già 2. S07 seme 1 resta a 55 `artiglieria_colpo`.\n"),
("- **Tetto dei 7200 s:** taglia cinque delle nove missioni. Non è indagato, per tua indicazione.\n", ""),
("1. **`vetrini.toml`:** la M4 resta fuori dai vetrini pronti e non l'ho aggiunta. **Attenzione per il kit:** alla prossima rigenerazione con `scene_interazioni`, il generatore aggiunge a `vetrini.toml` la voce `PROVA_M4_QUADRO` con `pronto = true`, `durata_min = 120` e il testo «Da solo non lo trova; e' la scena del quadro radio ai sottomarini», che è falso. Va tolta a mano, o va cambiato il catalogo in `scene_interazioni.cpp`. Costo: 10 minuti, da decidere. Non l'ho toccato perché non era nel via.\n",
 "1. **`vetrini.toml`: chiuso.** Ho tolto la M4 dal catalogo `VETRINI[]` in `scene_interazioni.cpp`. Rigenerando su una radice di prova, i 24 file coincidono con `data/missioni/prove`, `vetrini.toml` compreso, senza voci M4. Il kit può rigenerare.\n"),
("- `engine/strumenti/scene_interazioni.cpp`: solo `m4()`, commento e briefing. Annunciato a trailer-1.", "- `engine/strumenti/scene_interazioni.cpp`: `m4()` (commento e briefing) e la voce M4 del catalogo `VETRINI[]`. Annunciato a trailer-1."),
("- **`vetrini.toml`:** vedi il punto 4.1, prima di rigenerare.", "- **`vetrini.toml`:** nessun passaggio a mano, vedi il punto 4.1."),
("nelle partite misurate il dato è servito una volta su 18.", "nelle partite misurate il dato è servito due volte su 18 partite, tutte e due nella stessa."),
])
sost(pathlib.Path('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/proposta.md'), [
("- **Non indagato:** la partita con `--secondi 28800` si è fermata a 7200 s. Per una scena più lunga di 7200 s va capito prima.",
 "- **Durata:** `--secondi` è solo un tetto: la partita si ferma a `durata_min` delle regolazioni, 120 minuti se non è scritta nel codice sandbox\n  (`app/regolazioni_sandbox.hpp:50`). Per una scena più lunga di 7200 s la durata va scritta nel codice."),
])
