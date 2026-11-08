"""Il Museo col gioco intero, dal percorso dell'utente: F1 dal menu e F1 in partita, con tasti e clic X veri.

Ogni passo controlla la schermata prima e dopo. Le immagini vanno in USCITA, da guardare per intero.
Uso: prova_navigazione.py BINARIO USCITA (rc 0 se tutti i passi vanno)
"""
import os
import shutil
import subprocess
import sys
import time
import tomllib

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from guida import Gioco  # noqa: E402

RADICE = "/home/randolph/Claude/melchior-688"
SCHEDARIO = (6, 25, 196, 399)
VOCE = (208, 25, 426, 399)
SIN, DES, NOME = (25, 41), (187, 41), (100, 41)
RIGA0, PASSO, X_RIGA = 80, 14, 80
PIEDE_Y, TAB_X, PAG_X = 463, 320, 530
BREVE = 0.005   # premuto e rilasciato dentro un fotogramma

with open(RADICE + "/data/motore/enciclopedia.toml", "rb") as f:
    N_SEZIONI = len({v["gruppo"] for v in tomllib.load(f)["voci"].values() if v.get("gruppo")})


def diff(a, b, zona=None):
    a, b = np.asarray(a), np.asarray(b)
    if zona:
        x, y, w, h = zona
        a, b = a[y:y + h, x:x + w], b[y:y + h, x:x + w]
    return int((a != b).any(axis=2).sum())


class Prova:
    def __init__(self, g, uscita, nome):
        self.g, self.uscita, self.nome = g, uscita, nome
        self.falliti = self.passi = 0

    def esito(self, cosa, ok, dati=""):
        self.passi += 1
        self.falliti += 0 if ok else 1
        print(f"{'OK ' if ok else 'NO '} {self.nome}: {cosa} {dati}", flush=True)

    def salva(self, n):
        im = self.g.tela()
        im.save(f"{self.uscita}/{self.nome}_{n}.png")
        return im

    def ferma(self):
        """La schermata, quando due letture di fila sono uguali: la macchina puo' essere carica"""
        prec = self.g.tela()
        for _ in range(60):
            time.sleep(0.25)
            ora = self.g.tela()
            if diff(prec, ora) == 0:
                return ora
            prec = ora
        return prec

    def cambia(self, cosa, azione, zona=None, attesa=20.0):
        """L'azione deve cambiare la zona"""
        prima = self.g.tela()
        azione()
        fino = time.time() + attesa
        d = 0
        while d == 0 and time.time() < fino:
            time.sleep(0.2)
            d = diff(prima, self.g.tela(), zona)
        dopo = self.ferma()
        d = diff(prima, dopo, zona)
        self.esito(cosa, d > 0, f"(pixel {d})")
        return dopo

    def uguale(self, cosa, azione, rif, attesa=20.0):
        """Dopo l'azione la schermata deve tornare uguale a `rif`"""
        azione()
        fino = time.time() + attesa
        dopo = self.g.tela()
        while diff(rif, dopo) and time.time() < fino:
            time.sleep(0.2)
            dopo = self.g.tela()
        d = diff(rif, dopo)
        self.esito(cosa, d == 0, f"(pixel diversi {d})")
        return dopo


def navigazione(p, riga_base):
    g = p.g
    base = p.salva("0_base")
    p.cambia("Destra breve cambia sezione", lambda: g.tasto("Right", tieni=BREVE, dopo=0), SCHEDARIO)
    p.uguale("Sinistra breve torna", lambda: g.tasto("Left", tieni=BREVE, dopo=0), base)
    p.cambia("Giu breve cambia voce", lambda: g.tasto("Down", tieni=BREVE, dopo=0), VOCE)
    p.uguale("Su breve torna", lambda: g.tasto("Up", tieni=BREVE, dopo=0), base)
    p.cambia("Destra normale cambia sezione", lambda: g.tasto("Right", dopo=0), SCHEDARIO)
    p.uguale("Sinistra normale torna", lambda: g.tasto("Left", dopo=0), base)

    a = p.cambia("clic su > cambia sezione", lambda: g.clic(*DES, dopo=0), SCHEDARIO)
    a.save(f"{p.uscita}/{p.nome}_1_clic_succ.png")
    p.uguale("clic su < torna", lambda: g.clic(*SIN, dopo=0), base)
    p.cambia("clic su < dalla prima: in giro", lambda: (g.clic(*SIN, dopo=0)), SCHEDARIO)
    p.uguale("clic su > torna", lambda: g.clic(*DES, dopo=0), base)

    altra = 2 if riga_base != 2 else 3
    a = p.cambia("clic su una riga cambia voce", lambda: g.clic(X_RIGA, RIGA0 + altra * PASSO, dopo=0), VOCE)
    a.save(f"{p.uscita}/{p.nome}_2_clic_riga.png")
    p.uguale("clic sulla riga di prima torna", lambda: g.clic(X_RIGA, RIGA0 + riga_base * PASSO, dopo=0), base)

    # tante pressioni brevi di fila, fra immagini lente: il giro completo delle sezioni torna al punto di partenza
    def giro_veloce():
        for _ in range(N_SEZIONI):
            g.tasto("Right", tieni=BREVE, dopo=0.01)
    p.uguale(f"{N_SEZIONI} Destra brevi di fila fanno il giro", giro_veloce, base)

    def giro_normale():
        for _ in range(N_SEZIONI):
            g.tasto("Right", tieni=0.06, dopo=0.04)
    p.uguale(f"{N_SEZIONI} Destra normali di fila fanno il giro", giro_normale, base)

    def giro_clic():
        for _ in range(N_SEZIONI):
            g.clic(*DES, tieni=0.01, dopo=0.02)
    p.uguale(f"{N_SEZIONI} clic su > di fila fanno il giro", giro_clic, base)

    p.cambia("clic su Tab del piede", lambda: g.clic(TAB_X, PIEDE_Y, dopo=0), VOCE)
    p.uguale("Su in cima all'elenco torna alla prima immagine", lambda: g.tasto("Up", dopo=0), base)
    # l'indice
    a = p.cambia("Invio apre l'indice", lambda: g.tasto("Return", dopo=0), SCHEDARIO)
    d = diff(base, a, VOCE)
    p.esito("con l'indice la voce resta", d == 0, f"(pixel {d})")
    a.save(f"{p.uscita}/{p.nome}_3_indice.png")

    def giu3():
        for _ in range(3):
            g.tasto("Down", tieni=BREVE, dopo=0.02)
    a = p.cambia("Giu scorre l'indice", giu3, SCHEDARIO)
    a.save(f"{p.uscita}/{p.nome}_4_indice_scorso.png")
    d = diff(base, a, VOCE)
    p.esito("l'indice si scorre senza cambiare scheda", d == 0, f"(pixel {d})")
    a = p.cambia("Invio apre la sezione", lambda: g.tasto("Return", dopo=0), VOCE)
    a.save(f"{p.uscita}/{p.nome}_5_sezione_dall_indice.png")

    # era la terza dopo: tre clic su < riportano alla sezione di partenza, sulla prima voce
    for _ in range(3):
        g.clic(*SIN, tieni=0.01, dopo=0.05)
    p.ferma()

    def fino_a_base():   # dalla prima voce della sezione di partenza alla voce di partenza
        for _ in range(riga_base):
            g.tasto("Down", tieni=BREVE, dopo=0.02)
    p.uguale("alla voce di partenza", fino_a_base, base)

    p.cambia("clic sul nome apre l'indice", lambda: g.clic(*NOME, dopo=0), SCHEDARIO)
    a = p.cambia("clic su una riga dell'indice apre la sezione", lambda: g.clic(X_RIGA, RIGA0 + 5 * PASSO, dopo=0), SCHEDARIO)
    a.save(f"{p.uscita}/{p.nome}_6_clic_indice.png")
    p.cambia("Invio riapre l'indice", lambda: g.tasto("Return", dopo=0), SCHEDARIO)
    p.uguale("Esc chiude l'indice senza uscire", lambda: g.tasto("Escape", dopo=0), a)

    p.cambia("clic su Pag del piede", lambda: g.clic(PAG_X, PIEDE_Y, dopo=0), VOCE)
    p.salva("7_fine")
    p.cambia("Esc esce dal Museo", lambda: g.tasto("Escape", dopo=0))
    p.esito("il gioco e' vivo dopo il Museo", g.vivo())


def guidata(p, riga_base):
    try:
        navigazione(p, riga_base)
    except subprocess.CalledProcessError:   # il gioco si e' chiuso a meta': un Esc di troppo
        p.esito("la finestra e' sparita a meta' prova", False)


def main():
    binario, uscita = sys.argv[1], os.path.abspath(sys.argv[2])
    os.makedirs(uscita, exist_ok=True)
    for casa in ("casa_menu", "casa_partita"):   # ogni volta il primo avvio
        shutil.rmtree(f"{uscita}/{casa}", ignore_errors=True)
    print("sezioni:", N_SEZIONI, flush=True)
    falliti = 0

    g = Gioco(binario, "--menu", "--senza-apertura", casa=uscita + "/casa_menu")
    try:
        p = Prova(g, uscita, "menu")
        p.ferma()
        p.salva("00_primo_avvio")
        p.cambia("Invio al primo avvio: Enhanced, all'albo", lambda: g.tasto("Return", dopo=0), attesa=90)
        p.salva("01_albo")
        p.cambia("Esc: dall'albo alla scelta della missione", lambda: g.tasto("Escape", dopo=0), attesa=90)
        p.salva("02_scelta")
        p.cambia("F1 apre il Museo", lambda: g.tasto("F1", dopo=0), attesa=90)
        guidata(p, 0)
        falliti += p.falliti
        print(f"menu: {p.passi - p.falliti}/{p.passi}", flush=True)
    finally:
        g.chiudi()

    g = Gioco(binario, "--missione", "SCENE1A", "--regola", "simulazione", casa=uscita + "/casa_partita")
    try:
        p = Prova(g, uscita, "partita")
        time.sleep(8)
        p.cambia("F1 apre il Museo", lambda: g.tasto("F1", dopo=0), attesa=90)
        p.salva("00_apertura")   # si apre sulla scheda del proprio battello, la settima dei sottomarini
        for _ in range(6):
            g.tasto("Up", dopo=0.05)
        p.ferma()
        guidata(p, 0)
        falliti += p.falliti
        print(f"partita: {p.passi - p.falliti}/{p.passi}", flush=True)
    finally:
        g.chiudi()
    print("falliti:", falliti, flush=True)
    return 1 if falliti else 0


if __name__ == "__main__":
    sys.exit(main())
