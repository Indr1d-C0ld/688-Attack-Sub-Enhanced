"""Partita sotto X col pacchetto estratto da verifica_pacchetto.sh, nella stessa isola (/home/randolph vuota, casa di
prova) e sotto la stessa traccia dei file aperti (CARTELLA/traccia.log). Tasti e immagini con tools/re/prove/museo/guida.py.

Uso: tools/venv/bin/python partita_pacchetto.py CARTELLA [OPZIONI DEL GIOCO] < COMANDI
Comandi, uno per riga: t TASTO [TENUTO_S] [ATTESA_S], c TASTO (con Ctrl), a SECONDI, s NOME (immagine in CARTELLA/uscite/x_NOME.png), fine.
L'audio va in CARTELLA/uscite/audio_x.raw (driver disk di SDL: float a 32 bit, stereo, 48 kHz). Scritto il 07/11/2026.
"""
import ctypes
import glob
import os
import subprocess
import sys
import time

R = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, R + "/tools/re/prove/museo")
import guida  # noqa: E402
from guida import Gioco  # noqa: E402


class GiocoIsolato(Gioco):
    def __init__(self, cartella, argomenti, attesa=240.0):
        pacchetto = sorted(glob.glob(cartella + "/estratto/*/bin/melchior688"))[0]
        env = dict(os.environ, TRACCIA="1", SDL_AUDIO_DRIVER="disk", SDL_AUDIO_DISK_OUTPUT_FILE=cartella + "/uscite/audio_x.raw")
        self.log = open(cartella + "/uscite/partita_x.log", "wb")
        self.p = subprocess.Popen(["unshare", "-rm", cartella + "/isola.sh", pacchetto, *argomenti], env=env,
                                  stdout=self.log, stderr=self.log, cwd=cartella + "/uscite")
        fino = time.time() + attesa
        self.w = None
        self.rediretta = None
        self.d = guida.xtasti.display()
        vista = None
        while time.time() < fino and self.p.poll() is None:
            vista = (vista or time.time()) if self.mie() else None
            if vista and time.time() - vista > 5.0:
                break
            time.sleep(0.2)
        if not vista:
            self.chiudi()
            raise RuntimeError("finestra del gioco non trovata")
        self.rifinestra()


def con_ctrl(g, nome, dopo=0.6):
    """Ctrl+TASTO: guida.tasto non porta i modificatori; qui Control_L premuto attorno al tasto, con lo stato giusto"""
    X = guida.xtasti.X

    def manda(nome_x, tipo, maschera, stato):
        e = guida.xtasti.XEvent()
        k = e.xkey
        k.type, k.display, k.window, k.root = tipo, g.d, g.w, X.XDefaultRootWindow(g.d)
        k.time, k.same_screen, k.state = int(time.monotonic() * 1000) & 0x7FFFFFFF, 1, stato
        k.keycode = X.XKeysymToKeycode(g.d, X.XStringToKeysym(nome_x.encode()))
        X.XSendEvent(g.d, g.w, 1, maschera, ctypes.byref(e))
        X.XFlush(g.d)
        time.sleep(0.08)

    premi, lascia = (guida.xtasti.KeyPress, guida.xtasti.KeyPressMask), (guida.xtasti.KeyRelease, guida.xtasti.KeyReleaseMask)
    manda("Control_L", *premi, 0)
    manda(nome, *premi, 4)
    manda(nome, *lascia, 4)
    manda("Control_L", *lascia, 4)
    time.sleep(dopo)


cartella = os.path.abspath(sys.argv[1])
g = GiocoIsolato(cartella, sys.argv[2:])


def fuori_schermo():
    """La finestra fuori dallo schermo dell'utente (X Composite la legge lo stesso): sullo schermo la si riduce a icona per
    sbaglio mentre si lavora, e una finestra non mappata non si legge. Il gioco rifa' la finestra fra le schermate"""
    for w, _, _ in g.mie():
        subprocess.run(["xdotool", "windowmap", hex(w), "windowmove", hex(w), "6000", "0"], capture_output=True)


fuori_schermo()
t0 = time.time()
for riga in sys.stdin:
    p = riga.split()
    if not p:
        continue
    if p[0] == "t":
        g.tasto(p[1], tieni=float(p[2]) if len(p) > 2 else 0.08, dopo=float(p[3]) if len(p) > 3 else 0.6)
    elif p[0] == "c":
        con_ctrl(g, p[1])
    elif p[0] == "a":
        time.sleep(float(p[1]))
    elif p[0] == "s":
        fuori_schermo()
        g.tela().save(f"{cartella}/uscite/x_{p[1]}.png")
    elif p[0] == "fine":
        break
    print(f"{time.time() - t0:6.1f} s  {riga.strip()}  vivo={g.vivo()}", flush=True)
g.chiudi()
