#!/usr/bin/env python3
"""Inventario delle schermate dell'Enhanced (roadmap 33, la rifinitura estetica): un'istantanea senza finestra di ogni
schermata che si raggiunge da riga di comando, ingrandita 2x in PNG, e l'elenco dei comandi in Markdown.

    tools/venv/bin/python tools/schermate/inventario.py [--uscita docs/ricerca/schermate_prima] [--motore BUILD]
                                                        [--lavori N] [--solo id1,id2] [--rifai]

Serve per il «prima» e per il «dopo»: la stessa lista di schermate, gli stessi comandi. Il motore e' M688_MOTORE (o
--motore), predefinito engine/build: melchior688 e serie_sandbox. Prima delle schermate prepara tre partite: una
traccia di SCENE1A col lancio (replay, statistiche, debriefing), e una piccola serie della sandbox su S5 (schermata
delle serie). Nell'uscita: <id>.png per schermata ed elenco.md coi comandi; le partite di servizio in --lavoro. Solo
l'Enhanced (--regola simulazione): il classico non cambia. L'analisi a occhio sta in indice.md, scritto a mano.
"""

import argparse
import concurrent.futures as cf
import os
import subprocess
import sys
import time
from pathlib import Path

from PIL import Image

RADICE = Path(__file__).resolve().parents[2]
DATI = RADICE / "data" / "motore"
PROVE = RADICE / "data" / "missioni" / "prove"
LIB = RADICE / "data" / "missioni" / "libreria"
SIM = ["--regola", "simulazione"]

ORDINI = {
    # SCENE1A: periscopio su, la prima Spruance designata, tubo 1 caricato e lancio a 30 s (come il trailer n. 1)
    "lancio": "1 periscopio 0 0 1\n2 designa_bersaglio 0 0 2\n3 carica_tubo 0 0 1\n30 lancia 0 0 1\n",
    "periscopio": "1 periscopio 0 0 1\n",
}


def mis(missione, *altro, secondi=60):
    return ["--missione", missione, *SIM, "--secondi", str(secondi), *altro]


# Le schermate: (id, gruppo, titolo, argomenti); «@traccia», «@sandbox», «@serie», «@ordini:X» sono le partite di servizio
SCHERMATE = [
    # menu e schede
    ("menu_originali", "Menu", "Scelta della missione: originali", ["--schermata", "scelta"]),
    ("menu_rapide", "Menu", "Scelta della missione: missioni rapide", ["--schermata", "scelta-rapida"]),
    ("menu_editor", "Menu", "Scelta della missione: editor, libreria e vetrini", ["--schermata", "scelta-editor"]),
    ("regolazioni", "Menu", "Regolazioni della sandbox (vetrino)", ["--schermata", "regolazioni", "--missione", "SCENE1A"]),
    # prima e dopo la missione
    ("briefing", "Missione", "Sala briefing (SCENE1A)", mis("SCENE1A", "--schermata", "briefing")),
    ("briefing_libreria", "Missione", "Sala briefing (libreria, LIB_S03)",
     ["--cartella-missioni", str(LIB), *mis("LIB_S03_QUARANTENA", "--schermata", "briefing")]),
    ("siluri_mk48", "Missione", "Sala siluri: Mk 48", mis("SCENE1A", "--schermata", "siluri", "--arma", "mk48")),
    ("siluri_mk67", "Missione", "Sala siluri: mina Mk 67", mis("SCENE1A", "--schermata", "siluri", "--arma", "mk67")),
    ("debriefing_esito", "Dopo", "Debriefing: l'esito (evento 0)",
     mis("SCENE1A", "--ordini", "@ordini:lancio", "--schermata", "debriefing", "--evento", "0", secondi=420)),
    ("debriefing_evento", "Dopo", "Debriefing: un evento con la minicartolina",
     mis("SCENE1A", "--ordini", "@ordini:lancio", "--schermata", "debriefing", "--evento", "2", secondi=420)),
    ("esito_rapporto", "Dopo", "Esito: il rapporto (pagina 1)",
     mis("SCENE1A", "--ordini", "@ordini:lancio", "--schermata", "esito", "--pagina", "1", secondi=420)),
    ("esito_cerimonia", "Dopo", "Esito: la cerimonia (pagina 2)",
     mis("SCENE1A", "--ordini", "@ordini:lancio", "--schermata", "esito", "--pagina", "2", secondi=420)),
    # stazioni del 688 (SCENE1A, 60 s)
    ("688_periscopio", "Stazioni 688", "Periscopio (posto di comando)", mis("SCENE1A", "--ordini", "@ordini:periscopio")),
    ("688_radio", "Stazioni 688", "F2 Sala radio ed ESM", mis("SCENE1A", "--ordini", "@ordini:periscopio", "--stazione", "2")),
    ("688_stato", "Stazioni 688", "F3 Stato del battello", mis("SCENE1A", "--stazione", "3")),
    ("688_manovra", "Stazioni 688", "F4 Manovra", mis("SCENE1A", "--stazione", "4")),
    ("688_carta", "Stazioni 688", "F6 Navigazione (carta VGA)", mis("SCENE1A", "--stazione", "6")),
    ("688_carta_nautica", "Stazioni 688", "F6 Carta, stile nautico", mis("SCENE1A", "--stazione", "6", "--stile-carta", "nautica")),
    ("688_carta_foglio", "Stazioni 688", "F6 Carta, stile foglio", mis("SCENE1A", "--stazione", "6", "--stile-carta", "foglio")),
    ("688_carta_vera", "Stazioni 688", "F6 Carta col fondale vero", mis("SCENE1A", "--stazione", "6", "--carta", "vera")),
    ("688_carta_mate", "Stazioni 688", "F6 Carta col MATE della TMA", mis("SCENE1A", "--stazione", "6", "--mate", secondi=300)),
    ("688_armi", "Stazioni 688", "F7 Armi", mis("SCENE1A", "--stazione", "7")),
    ("688_sonar", "Stazioni 688", "F8 Sonar moderno (cascata, LOFAR)", mis("SCENE1A", "--stazione", "8", secondi=200)),
    ("688_sonar_1989", "Stazioni 688", "F8 Sonar nell'aspetto 1989", mis("SCENE1A", "--stazione", "8", "--sonar", "1989", secondi=200)),
    ("688_centrale", "Stazioni 688", "F9 Centrale", mis("SCENE1A", "--stazione", "9")),
    # stazioni dell'Alfa (SCENE2B)
    ("alfa_periscopio", "Stazioni Alfa", "Periscopio dell'Alfa", mis("SCENE2B", "--ordini", "@ordini:periscopio")),
    ("alfa_radio", "Stazioni Alfa", "F2 Radio dell'Alfa", mis("SCENE2B", "--stazione", "2")),
    ("alfa_stato", "Stazioni Alfa", "F3 Stato dell'Alfa", mis("SCENE2B", "--stazione", "3")),
    ("alfa_manovra", "Stazioni Alfa", "F4 Manovra dell'Alfa (pulti)", mis("SCENE2B", "--stazione", "4")),
    ("alfa_carta", "Stazioni Alfa", "F6 Carta VMF", mis("SCENE2B", "--stazione", "6")),
    ("alfa_armi", "Stazioni Alfa", "F7 Armi dell'Alfa", mis("SCENE2B", "--stazione", "7")),
    ("alfa_sonar", "Stazioni Alfa", "F8 Sonar sovietico", mis("SCENE2B", "--stazione", "8", secondi=200)),
    # dopo la partita: replay e statistiche
    ("replay_quadro", "Replay e statistiche", "Replay, quadro del giocatore", ["--schermata", "replay", "--replay", "@traccia", "--secondi", "300"]),
    ("replay_verita", "Replay e statistiche", "Replay, verita'", ["--schermata", "replay", "--replay", "@traccia", "--secondi", "300", "--pagina", "2"]),
    ("statistiche", "Replay e statistiche", "Statistiche della missione", ["--schermata", "statistiche", "--replay", "@traccia"]),
    ("statistiche_cumulate", "Replay e statistiche", "Statistiche cumulate", ["--schermata", "statistiche-cumulate"]),
    # carriera e consultazione
    ("bacheca", "Carriera", "Bacheca dei riconoscimenti", ["--schermata", "bacheca"]),
    ("albo", "Carriera", "Albo dei comandanti", ["--schermata", "albo"]),
    ("fascicolo", "Carriera", "Fascicolo di un comandante (pagina 1)", ["--schermata", "fascicolo", "--evento", "1", "--pagina", "1"]),
    ("museo", "Consultazione", "Museo: la prima voce", ["--schermata", "museo"]),
    ("museo_alfa", "Consultazione", "Museo: l'Alfa", ["--schermata", "museo", "--voce", "sottomarino.alfa"]),
    ("museo_mk48", "Consultazione", "Museo: il Mk 48, seconda immagine", ["--schermata", "museo", "--voce", "siluro.mk48", "--evento", "2"]),
    # editor, scheda per scheda
    *[(f"editor_{k}", "Editor", f"Editor di missione, scheda {k}", ["--schermata", "editor", "--pagina", str(k)]) for k in range(1, 9)],
    # sandbox
    ("sandbox_vivo", "Sandbox", "Vetrino dal vivo (S5, 600 s)",
     ["--sandbox", "--cartella-missioni", str(PROVE), "--missione", "PROVA_S5_CONVOGLIO", "--seme", "1", "--secondi", "600",
      "--schermata", "vivo"]),
    *[(f"sandbox_statistiche_{k}", "Sandbox", f"Statistiche tecniche della sandbox, pagina {k}",
       ["--sandbox", "--cartella-missioni", str(PROVE), "--missione", "PROVA_S5_CONVOGLIO", "--seme", "1", "--secondi", "600",
        "--schermata", "statistiche-sandbox", "--pagina", str(k)]) for k in range(1, 6)],
    *[(f"serie_{k}", "Sandbox", f"Serie della sandbox, pagina {k}", ["--serie", "@serie", "--schermata", "serie", "--pagina", str(k)])
      for k in range(1, 4)],
]

# Le schermate che senza finestra non si fotografano: per la principale, che vede se aggiungere un'opzione
MANCANTI = [
    ("Titolo e menu iniziale dell'Enhanced", "la sequenza d'avvio con la musica; --immagine mostra solo le immagini del 1989"),
    ("Scelta della missione: scheda libreria e scheda importate", "--schermata ha solo scelta, scelta-rapida e scelta-editor"),
    ("Scelta della missione con una missione selezionata e il dettaglio", "manca un'opzione per la riga scelta"),
    ("Museo: l'indice delle voci e la ricerca", "--schermata museo apre direttamente una voce"),
    ("Albo d'onore storico", "c'e' solo l'anteprima del kit (anteprima_albo_onore), non la schermata del gioco"),
    ("Opzioni e tasti", "nessuna --schermata"),
    ("Pausa, messaggi radio da autenticare, dialoghi di conferma", "compaiono solo in finestra"),
    ("Vista dal vivo della sandbox in lettura parte o unita'", "--schermata vivo disegna solo la verita'"),
    ("Periscopio a 6x e nei modi notte e termico", "servono i tasti: --tasti-a-tempo li da', ma non c'e' un'opzione diretta"),
]


def prepara(lavoro, motore, env):
    """le partite di servizio: la traccia di SCENE1A col lancio e una piccola serie della sandbox su S5"""
    lavoro.mkdir(parents=True, exist_ok=True)
    for nome, testo in ORDINI.items():
        (lavoro / f"{nome}.ord").write_text(testo)
    traccia = lavoro / "scene1a.m688tr"
    if not traccia.exists():
        subprocess.run([str(motore / "melchior688"), "--missione", "SCENE1A", *SIM, "--ordini", str(lavoro / "lancio.ord"),
                        "--secondi", "600", "--salva-traccia", str(traccia), "--istantanea", str(lavoro / "traccia.bmp")],
                       check=True, env=env, capture_output=True)
    serie = lavoro / "serie" / "serie.toml"
    if not serie.exists():
        subprocess.run([str(motore / "serie_sandbox"), "--semi", "3", "--lavori", "3", "--secondi", "900", "--uscita", str(serie.parent),
                        "--motore", str(motore / "melchior688"), "--", "--cartella-missioni", str(PROVE), "--missione",
                        "PROVA_S5_CONVOGLIO"], check=True, env=env, capture_output=True)
    return {"@traccia": str(traccia), "@serie": str(serie)}


def comando(motore, args, servizio, lavoro, uscita_bmp):
    out = []
    for a in args:
        if a.startswith("@ordini:"):
            out.append(str(lavoro / f"{a.split(':')[1]}.ord"))
        else:
            out.append(servizio.get(a, a))
    return [str(motore / "melchior688"), *out, "--istantanea", str(uscita_bmp)]


def mostra(args):   # la riga di comando per l'elenco, coi segnaposto leggibili
    return "melchior688 " + " ".join(a if " " not in a else f'"{a}"' for a in args).replace(str(RADICE) + "/", "") + " --istantanea X.bmp"


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--uscita", type=Path, default=RADICE / "docs" / "ricerca" / "schermate_prima")
    ap.add_argument("--motore", type=Path, default=Path(os.environ.get("M688_MOTORE", RADICE / "engine" / "build")))
    ap.add_argument("--lavori", type=int, default=max(1, min(8, (os.cpu_count() or 2) - 1)))
    ap.add_argument("--lavoro", type=Path, default=Path(os.environ.get("TMPDIR", "/tmp")) / "m688_schermate",
                    help="le partite di servizio e le BMP (si rifanno da sole se mancano)")
    ap.add_argument("--solo")
    ap.add_argument("--rifai", action="store_true")
    a = ap.parse_args()
    usc, lavoro = a.uscita, a.lavoro
    usc.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, TMPDIR=str(lavoro))
    print("partite di servizio...")
    servizio = prepara(lavoro, a.motore, env)
    scelte = [s for s in SCHERMATE if not a.solo or s[0] in a.solo.split(",")]
    risultati = {}

    def fai(s):
        ident, _, _, args = s
        png = usc / f"{ident}.png"
        if png.exists() and not a.rifai:
            return ident, "in cache", 0.0
        bmp = lavoro / f"{ident}.bmp"
        t0 = time.time()
        r = subprocess.run(comando(a.motore, args, servizio, lavoro, bmp), env=env, capture_output=True, text=True)
        if r.returncode or not bmp.exists():
            return ident, "ERRORE: " + ((r.stderr or r.stdout).strip().splitlines() or ["?"])[-1], time.time() - t0
        im = Image.open(bmp).convert("RGB")
        im.resize((im.width * 2, im.height * 2), Image.NEAREST).save(png, optimize=True)
        bmp.unlink()
        return ident, "fatta", time.time() - t0

    with cf.ThreadPoolExecutor(max_workers=a.lavori) as ex:
        for ident, stato, dt in ex.map(fai, scelte):
            risultati[ident] = stato
            print(f"  {ident:28s} {stato} ({dt:.0f} s)")
    # l'elenco dei comandi, per gruppo
    righe = ["# Schermate dell'Enhanced: elenco dei comandi", "",
             "Scritto da `tools/schermate/inventario.py`. Ogni immagine è un'istantanea senza finestra a 640×480, ingrandita "
             "2× senza filtri. Le partite di servizio (la traccia di SCENE1A col lancio, la serie di S5) le rifà lo script. "
             "L'analisi a occhio è in `indice.md`.", ""]
    gruppo = None
    for ident, g, titolo, args in SCHERMATE:
        if g != gruppo:
            righe += ["", f"## {g}", "", "| Schermata | Immagine | Comando |", "|---|---|---|"]
            gruppo = g
        stato = risultati.get(ident, "non rifatta")
        img = f"[{ident}.png]({ident}.png)" if (usc / f"{ident}.png").exists() else stato
        righe.append(f"| {titolo} | {img} | `{mostra(args)}` |")
    righe += ["", "## Da aggiungere (non si fotografano senza finestra)", "", "| Schermata | Perché |", "|---|---|"]
    righe += [f"| {n} | {p} |" for n, p in MANCANTI]
    (usc / "elenco.md").write_text("\n".join(righe) + "\n")
    errori = [i for i, s in risultati.items() if s.startswith("ERRORE")]
    print(f"schermate: {len(risultati)} ({len(errori)} errori){': ' + ', '.join(errori) if errori else ''}")
    return 1 if errori else 0


if __name__ == "__main__":
    sys.exit(main())
