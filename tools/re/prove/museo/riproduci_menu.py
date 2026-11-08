"""Il Museo dal menu con tasti e clic veri: quali comandi cambiano davvero la schermata.

Uso: riproduci_menu.py BINARIO USCITA [ARGOMENTI DEL GIOCO] < COMANDI

Comandi, uno per riga: t TASTO [TENUTO_S] [ATTESA_S], c X Y (clic nella tela 640x480), s NOME (immagine), fine.
"""
import os
import sys
import time

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from guida import Gioco  # noqa: E402

binario, uscita = sys.argv[1], sys.argv[2]
os.makedirs(uscita, exist_ok=True)
argomenti = sys.argv[3:] or ["--menu", "--senza-apertura"]
g = Gioco(binario, *argomenti, casa=os.path.abspath(uscita) + "/casa")
time.sleep(4)
g.tela().save(uscita + "/00_avvio.png")


def diversa(a, b, zona=None):
    a, b = np.asarray(a), np.asarray(b)
    if zona:
        x, y, w, h = zona
        a, b = a[y:y + h, x:x + w], b[y:y + h, x:x + w]
    return int((a != b).any(axis=2).sum())


# dall'albo del primo avvio alla scelta: Esc non serve, si guarda cosa c'e'
print("avvio guardato:", uscita + "/00_avvio.png", flush=True)
for riga in sys.stdin:
    riga = riga.strip()
    if not riga:
        continue
    prima = g.tela()
    p = riga.split()
    if p[0] == "t":
        g.tasto(p[1], tieni=float(p[2]) if len(p) > 2 else 0.08, dopo=float(p[3]) if len(p) > 3 else 0.6)
    elif p[0] == "c":
        g.clic(int(p[1]), int(p[2]), dopo=0.6)
    elif p[0] == "s":
        g.tela().save(uscita + "/" + p[1] + ".png")
        print("salvata", p[1], flush=True)
        continue
    elif p[0] == "fine":
        break
    dopo = g.tela()
    print(riga, "-> pixel cambiati", diversa(prima, dopo), "elenco", diversa(prima, dopo, (6, 25, 196, 399)),
          "voce", diversa(prima, dopo, (208, 25, 426, 399)), "vivo", g.vivo(), flush=True)
g.chiudi()
