"""Tasti alla sola finestra di DOSBox Staging (XSendEvent via ctypes, senza xdotool e senza rubare il fuoco).

Serve per i tasti rapidi dell'emulatore (cattura video Ctrl+F7, istantanea Ctrl+F5) durante le riprese del gioco
DOS originale per il trailer 3; i tasti del gioco li manda AUTOTYPE dentro l'emulatore.
"""
import ctypes
import ctypes.util
import re
import subprocess
import time

X = ctypes.cdll.LoadLibrary(ctypes.util.find_library("X11"))

KeyPress, KeyRelease = 2, 3
KeyPressMask, KeyReleaseMask = 1, 2
ShiftMask, ControlMask, Mod1Mask = 1, 4, 8


class XKeyEvent(ctypes.Structure):
    _fields_ = [("type", ctypes.c_int), ("serial", ctypes.c_ulong), ("send_event", ctypes.c_int),
                ("display", ctypes.c_void_p), ("window", ctypes.c_ulong), ("root", ctypes.c_ulong),
                ("subwindow", ctypes.c_ulong), ("time", ctypes.c_ulong), ("x", ctypes.c_int), ("y", ctypes.c_int),
                ("x_root", ctypes.c_int), ("y_root", ctypes.c_int), ("state", ctypes.c_uint), ("keycode", ctypes.c_uint),
                ("same_screen", ctypes.c_int)]


class XEvent(ctypes.Union):
    _fields_ = [("xkey", XKeyEvent), ("pad", ctypes.c_long * 24)]


X.XOpenDisplay.restype = ctypes.c_void_p
X.XOpenDisplay.argtypes = [ctypes.c_char_p]
X.XDefaultRootWindow.restype = ctypes.c_ulong
X.XDefaultRootWindow.argtypes = [ctypes.c_void_p]
X.XStringToKeysym.restype = ctypes.c_ulong
X.XStringToKeysym.argtypes = [ctypes.c_char_p]
X.XKeysymToKeycode.restype = ctypes.c_ubyte
X.XKeysymToKeycode.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
X.XSendEvent.argtypes = [ctypes.c_void_p, ctypes.c_ulong, ctypes.c_int, ctypes.c_long, ctypes.POINTER(XEvent)]
X.XFlush.argtypes = [ctypes.c_void_p]

_dpy = None


def display():
    global _dpy
    if _dpy is None:
        _dpy = X.XOpenDisplay(None)
        if not _dpy:
            raise RuntimeError("nessun display X")
    return _dpy


def finestra(nome="dosbox_staging", attesa=20.0):
    """L'id della finestra di primo livello col titolo o la classe che contiene `nome` (xwininfo -root -tree)."""
    fino = time.time() + attesa
    while time.time() < fino:
        albero = subprocess.run(["xwininfo", "-root", "-tree"], capture_output=True, text=True).stdout
        for riga in albero.splitlines():
            m = re.match(r'\s*(0x[0-9a-f]+) "([^"]*)".*?\s(\d+)x(\d+)[+-]', riga)
            if m and nome.lower() in riga.lower() and int(m.group(3)) > 100 and int(m.group(4)) > 100:
                return int(m.group(1), 16)
        time.sleep(0.5)
    raise RuntimeError("finestra di DOSBox non trovata")


def tasto(finestra_id, nome, ctrl=False, alt=False, maiuscolo=False, tieni=0.08):
    """Preme e rilascia il tasto `nome` (keysym X: "F5", "Return", "a") con i modificatori."""
    d = display()
    codice = X.XKeysymToKeycode(d, X.XStringToKeysym(nome.encode()))
    stato = (ControlMask if ctrl else 0) | (Mod1Mask if alt else 0) | (ShiftMask if maiuscolo else 0)
    modificatori = [n for n, s in (("Control_L", ctrl), ("Alt_L", alt), ("Shift_L", maiuscolo)) if s]

    def manda(tipo, codice_x, st):
        e = XEvent()
        k = e.xkey
        k.type, k.display, k.window, k.root = tipo, d, finestra_id, X.XDefaultRootWindow(d)
        k.subwindow, k.time, k.same_screen, k.state, k.keycode = 0, 0, 1, st, codice_x
        X.XSendEvent(d, finestra_id, 1, KeyPressMask if tipo == KeyPress else KeyReleaseMask, ctypes.byref(e))
        X.XFlush(d)

    st = 0
    for m in modificatori:
        manda(KeyPress, X.XKeysymToKeycode(d, X.XStringToKeysym(m.encode())), st)
    manda(KeyPress, codice, stato)
    time.sleep(tieni)
    manda(KeyRelease, codice, stato)
    for m in reversed(modificatori):
        manda(KeyRelease, X.XKeysymToKeycode(d, X.XStringToKeysym(m.encode())), stato)
    time.sleep(0.05)


if __name__ == "__main__":
    import sys
    w = finestra()
    print(hex(w))
    if len(sys.argv) > 1:
        tasto(w, sys.argv[1], ctrl="ctrl" in sys.argv, alt="alt" in sys.argv)
