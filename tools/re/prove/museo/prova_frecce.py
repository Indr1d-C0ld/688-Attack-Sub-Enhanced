"""Le frecce nelle schermate di contorno col gioco intero, dal primo avvio al briefing, con tasti X veri.

Uso: prova_frecce.py BINARIO USCITA (rc 0 se tutti i passi vanno). Le immagini vanno in USCITA, da guardare per intero.
"""
import os
import shutil
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from guida import Gioco  # noqa: E402
from prova_navigazione import BREVE, Prova, diff  # noqa: E402


def breve(g, nome, n=1):
    for _ in range(n):
        g.tasto(nome, tieni=BREVE, dopo=0.02)


def percorso(p):
    g = p.g
    p.ferma()
    base = p.salva("0_modo")
    p.cambia("modo: Sinistra breve", lambda: breve(g, "Left"))
    p.uguale("modo: Destra breve torna", lambda: breve(g, "Right"), base)

    p.cambia("Invio: all'albo", lambda: g.tasto("Return", dopo=0), attesa=90)
    p.salva("1_albo")
    p.cambia("albo: Invio apre il modulo", lambda: g.tasto("Return", dopo=0), attesa=30)
    base = p.salva("2_modulo")
    p.cambia("modulo: Giu breve", lambda: breve(g, "Down"))
    p.uguale("modulo: Su breve torna", lambda: breve(g, "Up"), base)
    p.cambia("modulo: Esc chiude", lambda: g.tasto("Escape", dopo=0))
    p.cambia("albo: Esc alla scelta", lambda: g.tasto("Escape", dopo=0), attesa=90)

    base = p.salva("3_scelta")
    p.cambia("scelta: Giu breve", lambda: breve(g, "Down"))
    p.uguale("scelta: Su breve torna", lambda: breve(g, "Up"), base)
    p.cambia("scelta: Destra breve, il battello", lambda: breve(g, "Right"))
    p.salva("4_scelta_alfa")
    p.uguale("scelta: Sinistra breve torna", lambda: breve(g, "Left"), base)
    p.cambia("scelta: 5 Giu brevi di fila", lambda: breve(g, "Down", 5))
    p.salva("5_scelta_quinta")
    p.uguale("scelta: 5 Su brevi di fila tornano", lambda: breve(g, "Up", 5), base)

    p.cambia("Invio: al briefing", lambda: g.tasto("Return", dopo=0), attesa=90)
    uno = p.salva("6_briefing_p1")
    # una pressione normale gira una pagina sola: con tre pagine la seconda e la terza sono diverse
    a = p.cambia("briefing: Giu normale gira", lambda: g.tasto("Down", tieni=0.12, dopo=0))
    a.save(f"{p.uscita}/{p.nome}_7_briefing_p2.png")
    b = p.cambia("briefing: un altro Giu gira ancora", lambda: g.tasto("Down", tieni=0.12, dopo=0))
    b.save(f"{p.uscita}/{p.nome}_8_briefing_p3.png")
    p.esito("briefing: la seconda pagina non e' la terza", diff(a, b) > 0)
    g.tasto("Up", tieni=0.12, dopo=0)
    time.sleep(1)
    g.tasto("Up", tieni=0.12, dopo=0)
    p.esito("briefing: due Su tornano alla prima", diff(p.ferma(), uno) == 0)


def main():
    binario, uscita = sys.argv[1], os.path.abspath(sys.argv[2])
    os.makedirs(uscita, exist_ok=True)
    shutil.rmtree(uscita + "/casa", ignore_errors=True)
    g = Gioco(binario, "--menu", "--senza-apertura", casa=uscita + "/casa")
    p = Prova(g, uscita, "frecce")
    try:
        percorso(p)
    except subprocess.CalledProcessError:
        p.esito("la finestra e' sparita a meta' prova", False)
    finally:
        g.chiudi()
    print(f"passi: {p.passi - p.falliti}/{p.passi}", flush=True)
    return 1 if p.falliti else 0


if __name__ == "__main__":
    sys.exit(main())
