"""Pilota di DOSBox Staging per le riprese del gioco DOS originale (trailer 3).

    python3 pilota.py [--cicli N] [--in NOME] [--servizio] PASSO [PASSO ...]      (catture in catture/NOME, predefinito «prova»)

Passi, in ordine:
    k:NOME[+ctrl][+alt][+shift]   un tasto (keysym X: 4, Return, Escape, space, F1, Up, a ...)
    t:TESTO                       i caratteri del testo, uno per tasto
    w:SECONDI                     attesa
    s                             istantanea grezza dell'emulatore (Ctrl+F5) in catture/
    v                             avvia o ferma la cattura video (Ctrl+F7, ZMBV senza perdita) in catture/
    q                             (solo nel servizio) chiude
Con --servizio DOSBox resta acceso dopo i passi e legge altri passi, uno per riga, da catture/NOME/comandi.txt.
Il gioco parte da c688/ (copia della cartella originale) con 688.BAT; alla fine DOSBox si chiude (solo il suo PID).
"""
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import xtasti  # noqa: E402

QUI = os.path.dirname(os.path.abspath(__file__))
DOSBOX = os.path.join(QUI, "dosbox-staging-linux-x86_64-0.83.0-7b400", "dosbox")
SPECIALI = {" ": "space", ".": "period", ",": "comma", "-": "minus", "/": "slash"}


def esegui(passi, cicli=3000, cartella="prova", servizio=False):
    amb = dict(os.environ, HOME=os.path.join(QUI, "conf_home"), XDG_CONFIG_HOME=os.path.join(QUI, "conf_home"),
               SDL_AUDIODRIVER="dummy")
    uscita = os.path.join(QUI, "catture", cartella)
    os.makedirs(uscita, exist_ok=True)
    p = subprocess.Popen([DOSBOX, "--noprimaryconf", "--conf", os.path.join(QUI, "base.conf"), "--set", f"cpu_cycles={cicli}",
                          "--set", f"capture_dir={uscita}",
                          "-c", "mount c " + os.path.join(QUI, "c688"), "-c", "c:", "-c", "688"],
                         env=amb, stdout=open(os.path.join(QUI, "dosbox.log"), "w"), stderr=subprocess.STDOUT)
    try:
        w = xtasti.finestra()
        time.sleep(1.5)
        comandi = os.path.join(uscita, "comandi.txt")
        letti = 0
        if servizio:
            open(comandi, "w").close()

        def prossimi():   # i passi della riga di comando, poi (nel servizio) quelli che arrivano nel file
            nonlocal letti
            yield from passi
            while servizio:
                righe = open(comandi).read().split("\n")
                nuove = [r.strip() for r in righe[letti:-1] if r.strip()]
                letti = len(righe) - 1
                for r in nuove:
                    for passo in r.split():
                        if passo == "q":
                            return
                        yield passo
                if p.poll() is not None:
                    return
                time.sleep(0.2)

        for passo in prossimi():
            tipo, _, valore = passo.partition(":")
            if tipo == "k":
                nome, *mod = valore.split("+")
                xtasti.tasto(w, nome, ctrl="ctrl" in mod, alt="alt" in mod, maiuscolo="shift" in mod)
                time.sleep(0.25)
            elif tipo == "t":
                for c in valore:
                    xtasti.tasto(w, SPECIALI.get(c, c.lower()), maiuscolo=c.isupper())
                    time.sleep(0.12)
            elif tipo == "w":
                time.sleep(float(valore))
            elif tipo == "s":
                xtasti.tasto(w, "F5", ctrl=True)
                time.sleep(0.4)
            elif tipo == "v":
                xtasti.tasto(w, "F7", ctrl=True)
                time.sleep(0.4)
            else:
                raise ValueError("passo sconosciuto: " + passo)
    finally:
        p.terminate()
        try:
            p.wait(10)
        except subprocess.TimeoutExpired:
            p.kill()


if __name__ == "__main__":
    argomenti = sys.argv[1:]
    cicli, cartella, servizio = 3000, "prova", False
    while argomenti[:1] in (["--cicli"], ["--in"], ["--servizio"]):
        if argomenti[0] == "--servizio":
            servizio, argomenti = True, argomenti[1:]
            continue
        if argomenti[0] == "--cicli":
            cicli = int(argomenti[1])
        else:
            cartella = argomenti[1]
        argomenti = argomenti[2:]
    esegui(argomenti, cicli, cartella, servizio)
