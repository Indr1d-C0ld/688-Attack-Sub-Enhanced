#!/usr/bin/env python3
"""Kit del giocatore e delle armi per le missioni rapide (engine/missioni/src/missione_rapida.cpp).

Dalle missioni convertite prende i gestori dei comandi del giocatore (`gestori_giocatore`), i gestori di lancio
delle armi (`armi`) e tutti gli script che questi richiamano, e li riscrive con un prefisso (L_041a -> KU_041a), cosi'
i kit delle due parti stanno insieme nella stessa missione. L'IA usa gli stessi gestori di lancio delle armi.

    python3 tools/rapide/estrai_kit.py data/missioni data/motore/missioni_rapide

Scrive kit_usa.lua (688: SCENE1A) e kit_urss.lua (Alfa: SCENE3B). Il Tomahawk delle rapide e' il TASM antinave: usa
il gestore dell'Harpoon (quello di SCENE0A e' l'attacco a terra della missione 10, con le sue variabili). Ogni file ha tre
sezioni, separate da righe `--@ nome`: gestori_giocatore, armi (campi della tabella della missione) e script.
"""
import re
import sys
from pathlib import Path

ETICHETTA = re.compile(r"\bL_([0-9a-f]{4})\b")
INIZIO = re.compile(r"^script\.(L_[0-9a-f]{4}) = function")


def leggi(file):
    testo = Path(file).read_text(encoding="utf-8")
    funzioni, corrente, righe = {}, None, []
    for r in testo.splitlines():
        m = INIZIO.match(r)
        if m:
            corrente, righe = m.group(1), [r]
            continue
        if corrente:
            righe.append(r)
            if r == "end":
                funzioni[corrente] = "\n".join(righe)
                corrente = None
    def blocco(nome):
        m = re.search(r"^  %s = \{\n(.*?)\n  \},$" % nome, testo, re.S | re.M)
        return m.group(1).splitlines() if m else []
    return funzioni, blocco("gestori_giocatore"), blocco("armi")


def chiusura(funzioni, radici):
    visti, coda = set(), list(radici)
    while coda:
        n = coda.pop()
        if n in visti or n not in funzioni:
            continue
        visti.add(n)
        coda += ["L_" + e for e in ETICHETTA.findall(funzioni[n])]
    return sorted(visti)


def kit(cartella, fonti, prefisso, uscita, nota, alias=()):
    alias = list(alias)
    gestori, armi, script = [], [], []
    for scena, tipi_armi, con_gestori in fonti:
        funzioni, g, a = leggi(Path(cartella) / f"{scena}.lua")
        radici = []
        if con_gestori:
            gestori += g
            radici += ["L_" + e for r in g for e in ETICHETTA.findall(r)]
        for r in a:
            tipo = r.strip().split(" ")[0]
            if tipo in tipi_armi:
                armi.append(r)
                radici += ["L_" + e for e in ETICHETTA.findall(r)]
        pref = prefisso + scena[5:] + "_"
        script += [ETICHETTA.sub(lambda m: pref + m.group(1), funzioni[n]) for n in chiusura(funzioni, radici)]
        gestori = [ETICHETTA.sub(lambda m: pref + m.group(1), r) for r in gestori] if con_gestori else gestori
        armi = [ETICHETTA.sub(lambda m: pref + m.group(1), r) if "L_" in r else r for r in armi]
    armi += alias
    testo = [f"-- {nota}", "-- Generato da tools/rapide/estrai_kit.py dalle missioni convertite: non modificare a mano.",
             "--@ gestori_giocatore", *gestori, "--@ armi", *armi, "--@ script", *script, ""]
    Path(uscita).write_text("\n".join(testo) + "\n", encoding="utf-8")
    print(uscita, len(script), "script")


def main():
    cartella, uscita = sys.argv[1], Path(sys.argv[2])
    uscita.mkdir(parents=True, exist_ok=True)
    kit(cartella, [("SCENE1A", {"mk48", "harpoon", "esca"}, True)], "KU", uscita / "kit_usa.lua",
        "Kit del 688 (comandi del giocatore, Mk 48, Harpoon, esca da SCENE1A; il Tomahawk antinave come l'Harpoon)",
        ['    tomahawk = { gestore = "KU1A_03cc" },   -- TASM antinave: il gestore dell\'Harpoon'])
    kit(cartella, [("SCENE3B", {"type53", "esca"}, True)], "KR",
        uscita / "kit_urss.lua", "Kit dell'Alfa (comandi del giocatore, siluri da 533 mm, esca da SCENE3B)")


if __name__ == "__main__":
    main()
