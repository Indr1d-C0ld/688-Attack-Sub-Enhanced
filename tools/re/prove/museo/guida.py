"""Guida il gioco vero con tasti e clic X alla sola sua finestra (XSendEvent, niente fuoco e niente puntatore rubati).

Serve per riprodurre i difetti di navigazione dal percorso dell'utente: lancia un binario, gli manda i tasti e prende
l'immagine della finestra con xwd. Le partite le scrive in una casa a parte: non tocca albo e preferenze dell'utente.
"""
import ctypes
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "dosbox"))
import xtasti  # noqa: E402

import numpy as np  # noqa: E402
from PIL import Image  # noqa: E402

SYSROOT = "/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu"
TITOLO = "688 Attack Sub"

ButtonPress, ButtonRelease, MotionNotify = 4, 5, 6
ButtonPressMask, ButtonReleaseMask, PointerMotionMask = 4, 8, 64


class XButtonEvent(ctypes.Structure):
    _fields_ = [("type", ctypes.c_int), ("serial", ctypes.c_ulong), ("send_event", ctypes.c_int),
                ("display", ctypes.c_void_p), ("window", ctypes.c_ulong), ("root", ctypes.c_ulong),
                ("subwindow", ctypes.c_ulong), ("time", ctypes.c_ulong), ("x", ctypes.c_int), ("y", ctypes.c_int),
                ("x_root", ctypes.c_int), ("y_root", ctypes.c_int), ("state", ctypes.c_uint), ("button", ctypes.c_uint),
                ("same_screen", ctypes.c_int)]


class XImmagine(ctypes.Structure):
    _fields_ = [("width", ctypes.c_int), ("height", ctypes.c_int), ("xoffset", ctypes.c_int), ("format", ctypes.c_int),
                ("data", ctypes.c_void_p), ("byte_order", ctypes.c_int), ("bitmap_unit", ctypes.c_int),
                ("bitmap_bit_order", ctypes.c_int), ("bitmap_pad", ctypes.c_int), ("depth", ctypes.c_int),
                ("bytes_per_line", ctypes.c_int), ("bits_per_pixel", ctypes.c_int)]


COMPOSITE = ctypes.cdll.LoadLibrary("libXcomposite.so.1")
COMPOSITE.XCompositeRedirectWindow.argtypes = [ctypes.c_void_p, ctypes.c_ulong, ctypes.c_int]
COMPOSITE.XCompositeNameWindowPixmap.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
COMPOSITE.XCompositeNameWindowPixmap.restype = ctypes.c_ulong
xtasti.X.XGetImage.restype = ctypes.c_void_p
xtasti.X.XGetImage.argtypes = [ctypes.c_void_p, ctypes.c_ulong, ctypes.c_int, ctypes.c_int, ctypes.c_uint, ctypes.c_uint, ctypes.c_ulong, ctypes.c_int]
xtasti.X.XSync.argtypes = [ctypes.c_void_p, ctypes.c_int]
xtasti.X.XFree.argtypes = [ctypes.c_void_p]
xtasti.X.XFreePixmap.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
_errore_x = False


class XErrore(ctypes.Structure):
    _fields_ = [("type", ctypes.c_int), ("display", ctypes.c_void_p), ("risorsa", ctypes.c_ulong), ("serial", ctypes.c_ulong),
                ("codice", ctypes.c_ubyte), ("richiesta", ctypes.c_ubyte), ("minore", ctypes.c_ubyte)]


ultimo_errore = None


@ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.POINTER(XErrore))
def _su_errore_x(_d, e):   # una finestra sparita fra due letture non deve chiudere lo script
    global _errore_x, ultimo_errore
    _errore_x = True
    ultimo_errore = (e.contents.codice, e.contents.richiesta, e.contents.minore, hex(e.contents.risorsa))
    return 0


xtasti.X.XSetErrorHandler(_su_errore_x)


class XEvento(ctypes.Union):
    _fields_ = [("xbutton", XButtonEvent), ("pad", ctypes.c_long * 24)]


def finestre():
    albero = subprocess.run(["xwininfo", "-root", "-tree"], capture_output=True, text=True).stdout
    r = []
    for riga in albero.splitlines():
        m = re.match(r'\s*(0x[0-9a-f]+) "([^"]*)".*?\s(\d+)x(\d+)[+-]', riga)
        if m and TITOLO in m.group(2) and int(m.group(3)) >= 640:
            r.append((int(m.group(1), 16), int(m.group(3)), int(m.group(4))))
    return r


class Gioco:
    def __init__(self, binario, *argomenti, casa, attesa=240.0):
        os.makedirs(casa, exist_ok=True)
        env = dict(os.environ, HOME=casa, XDG_CONFIG_HOME=casa + "/.config", XDG_DATA_HOME=casa + "/.local/share",
                   XAUTHORITY=os.environ.get("XAUTHORITY", os.path.expanduser("~/.Xauthority")),
                   LD_LIBRARY_PATH=SYSROOT, SDL_AUDIO_DRIVER="dummy", SDL_VIDEO_X11_XINPUT2="0")
        self.log = open(casa + "/gioco.log", "wb")
        self.p = subprocess.Popen([binario, *argomenti], env=env, stdout=self.log, stderr=self.log, cwd=casa)
        fino = time.time() + attesa
        self.w = None
        self.rediretta = None
        self.d = xtasti.display()
        # il gioco rifa' la finestra una volta all'avvio: si aspetta che ci sia da 5 secondi
        vista = None
        while time.time() < fino and self.p.poll() is None:
            ora = self.mie()
            vista = (vista or time.time()) if ora else None
            if vista and time.time() - vista > 5.0:
                break
            time.sleep(0.2)
        if not vista:
            self.chiudi()
            raise RuntimeError("finestra del gioco non trovata")
        self.rifinestra()

    def mie(self):
        """Le finestre di questo processo: il gioco rifa' la finestra, e gli id si riusano"""
        r = []
        for f in finestre():
            x = subprocess.run(["xprop", "-id", hex(f[0]), "_NET_WM_PID"], capture_output=True, text=True).stdout
            if x.strip().endswith("= " + str(self.p.pid)):
                r.append(f)
        return r

    def rifinestra(self):
        """L'ultima finestra del gioco, con le sue misure"""
        nuove = self.mie()
        if not nuove:
            raise RuntimeError("finestra del gioco sparita")
        self.w, self.lw, self.lh = nuove[-1]
        self.scala = min(self.lw // 640, self.lh // 480)
        self.ox, self.oy = (self.lw - 640 * self.scala) // 2, (self.lh - 480 * self.scala) // 2

    def tasto(self, nome, tieni=0.08, dopo=0.25):
        """Preme e rilascia un tasto. Coi tempi veri: a tempo 0, SDL prende per ripetizione due pressioni accodate"""
        codice = xtasti.X.XKeysymToKeycode(self.d, xtasti.X.XStringToKeysym(nome.encode()))

        def manda(tipo, maschera):
            e = xtasti.XEvent()
            k = e.xkey
            k.type, k.display, k.window, k.root = tipo, self.d, self.w, xtasti.X.XDefaultRootWindow(self.d)
            k.time, k.same_screen, k.keycode = int(time.monotonic() * 1000) & 0x7FFFFFFF, 1, codice
            xtasti.X.XSendEvent(self.d, self.w, 1, maschera, ctypes.byref(e))
            xtasti.X.XFlush(self.d)

        manda(xtasti.KeyPress, xtasti.KeyPressMask)
        time.sleep(tieni)
        manda(xtasti.KeyRelease, xtasti.KeyReleaseMask)
        time.sleep(dopo)

    def _manda(self, tipo, maschera, x, y, tasto=1, stato=0):
        e = XEvento()
        b = e.xbutton
        b.type, b.display, b.window, b.root = tipo, self.d, self.w, xtasti.X.XDefaultRootWindow(self.d)
        b.x, b.y, b.x_root, b.y_root, b.button, b.state, b.same_screen = x, y, x, y, tasto, stato, 1
        xtasti.X.XSendEvent(self.d, self.w, 1, maschera, ctypes.cast(ctypes.byref(e), ctypes.POINTER(xtasti.XEvent)))
        xtasti.X.XFlush(self.d)

    def clic(self, x, y, tieni=0.08, dopo=0.25):
        """Un clic del tasto sinistro in pixel della tela 640x480."""
        fx, fy = self.ox + x * self.scala + self.scala // 2, self.oy + y * self.scala + self.scala // 2
        self._manda(MotionNotify, PointerMotionMask, fx, fy)
        time.sleep(0.03)
        self._manda(ButtonPress, ButtonPressMask, fx, fy)
        time.sleep(tieni)
        self._manda(ButtonRelease, ButtonReleaseMask, fx, fy, stato=256)
        time.sleep(dopo)

    def tela(self):
        """La finestra come immagine 640x480, con X Composite: si legge intera anche coperta da altre finestre o fuori schermo"""
        for _ in range(60):   # il gioco rifa' la finestra fra le schermate
            rgb = self._leggi()
            if rgb is not None:
                break
            if not self.vivo():
                raise subprocess.CalledProcessError(1, "tela")
            time.sleep(0.5)
            if self.mie():
                self.rifinestra()
        if rgb is None:
            stato = subprocess.run(["xwininfo", "-id", hex(self.w)], capture_output=True, text=True).stdout
            print("tela fallita, errore X:", ultimo_errore, [r.strip() for r in stato.splitlines() if "Map State" in r or "geometry" in r], flush=True)
            raise subprocess.CalledProcessError(1, "tela")
        rgb = rgb[self.oy:self.oy + 480 * self.scala, self.ox:self.ox + 640 * self.scala][:: self.scala, :: self.scala]
        return Image.fromarray(rgb.copy())

    def _leggi(self):
        global _errore_x
        _errore_x = False
        if self.w != self.rediretta:
            COMPOSITE.XCompositeRedirectWindow(self.d, self.w, 0)   # automatico: lo schermo non cambia
            self.rediretta = self.w
        pm = COMPOSITE.XCompositeNameWindowPixmap(self.d, self.w)
        xtasti.X.XSync(self.d, 0)
        if _errore_x or not pm:
            self.rediretta = None
            return None
        im = xtasti.X.XGetImage(self.d, pm, 0, 0, self.lw, self.lh, ctypes.c_ulong(-1), 2)
        xtasti.X.XSync(self.d, 0)
        rgb = None
        if im and not _errore_x:
            x = ctypes.cast(im, ctypes.POINTER(XImmagine)).contents
            px = np.frombuffer(ctypes.string_at(x.data, x.bytes_per_line * x.height), dtype=np.uint8).reshape(x.height, x.bytes_per_line // 4, 4)
            rgb = px[:, : x.width, [2, 1, 0]].copy()
            xtasti.X.XFree(x.data)
            xtasti.X.XFree(im)
        xtasti.X.XFreePixmap(self.d, pm)
        return rgb

    def vivo(self):
        return self.p.poll() is None

    def chiudi(self):
        if not self.vivo():
            print("il gioco era gia' uscito, codice", self.p.returncode, flush=True)
        if self.vivo():
            self.p.terminate()
            try:
                self.p.wait(10)
            except subprocess.TimeoutExpired:
                self.p.kill()
                self.p.wait()
        self.log.close()
