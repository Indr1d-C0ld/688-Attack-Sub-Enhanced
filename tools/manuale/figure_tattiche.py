"""Figure di C5 §2.11, E5 §2.6.2 ed E11 del manuale dell'Enhanced: SVG nello stile di figure_acustica.py.

I numeri sono quelli calcolati col motore dal disassemblatore (31/10/2026: righe_emesse e rumore_ambiente del nucleo,
la regola dei passaggi MAD di nucleo/src/asw_aereo.cpp), scritti qui sotto; il quadrifoglio si ricava simulando la
regola. Le figure vanno in docs/manuale_enhanced/figure/acustica e figure/schemi.
    tools/venv/bin/python tools/manuale/figure_tattiche.py
"""
import math
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import figure_acustica as fa   # noqa: E402
from figure_acustica import (ASSE, CARATTERE, GRIGLIA, INCHIOSTRO, SECONDARIO, SERIE, SUPERFICIE, TENUE,  # noqa: E402
                             TRATTI, Grafico, esc, num)

FIGURE = fa.RADICE / "docs/manuale_enhanced/figure"

# C5 §2.11: il falso Victor III a 12 nodi e 150 m (righe_emesse con la fedelta' della falsa flag)
VICTOR = {
    1.0: [(50, 112.0, "rete"), (100, 109.0, "rete"), (150, 106.0, "rete"), (250, 118.0, "rete"), (36, 116.0, "pompa"),
          (88, 114.0, "turbina"), (132, 112.0, "riduttore"), (185, 109.0, "ausiliario"), (30.4, 103.0, "turbina"),
          (105.6, 105.0, "riduttore"), (464, 99.0, "riduttore"), (1.6, 139.3, "elica"), (6.4, 145.3, "elica"),
          (12.8, 139.3, "elica")],
    0.8: [(50, 110.8, "rete"), (150, 104.8, "rete"), (36, 114.8, "pompa"), (88, 112.8, "turbina"),
          (132, 110.8, "riduttore"), (185, 107.8, "ausiliario"), (30.4, 101.8, "turbina"), (464, 97.8, "riduttore"),
          (6.4, 144.1, "elica"), (12.8, 138.1, "elica")],
    0.5: [(150, 103.0, "rete"), (132, 109.0, "riduttore"), (185, 106.0, "ausiliario"), (30.4, 100.0, "turbina"),
          (464, 96.0, "riduttore")],
}

# E11 §2.3: livello spettrale (dB re 1 µPa²/Hz), traffico 0,5
RUMORE = [  # Hz, mare aperto col vento a 5 m/s, pack con le creste a 0,6 e l'aria a -30 °C, lo stesso sopra -10 °C
    (10, 78.2, 78.2, 78.2), (20, 74.9, 74.9, 74.9), (50, 72.2, 73.2, 73.2), (100, 67.8, 71.7, 71.7),
    (200, 64.0, 67.6, 67.6), (500, 62.8, 55.5, 55.4), (1000, 61.0, 51.0, 46.7), (2000, 57.6, 54.0, 42.2),
    (5000, 51.5, 38.6, 35.9), (10000, 46.1, 30.8, 30.5), (20000, 40.4, 25.0, 25.0)]


def svg(nome, cartella, larghezza, altezza, descrizione, pezzi):
    uscita = FIGURE / cartella
    uscita.mkdir(parents=True, exist_ok=True)
    testo = (f"<svg xmlns='http://www.w3.org/2000/svg' width='{larghezza}' height='{altezza}' "
             f"viewBox='0 0 {larghezza} {altezza}' {CARATTERE} role='img' aria-label='{esc(descrizione)}'>\n"
             f"<title>{esc(descrizione)}</title>\n" + "\n".join(pezzi) + "\n</svg>\n")
    (uscita / nome).write_text(testo)
    print(uscita / nome)


def testo(x, y, t, dim=11, colore=INCHIOSTRO, ancora="start", grassetto=False):
    peso = " font-weight='bold'" if grassetto else ""
    return (f"<text x='{x:.1f}' y='{y:.1f}' font-size='{dim}' fill='{colore}' text-anchor='{ancora}'{peso}>"
            f"{esc(t)}</text>")


def falso_victor():
    """Tre spettri a righe uno sotto l'altro: le righe che restano, quelle sparite in grigio tratteggiato."""
    L, H = 760, 560
    SX, DX, SU = 72, 150, 70
    alto, passo = 110, 140   # altezza di un pannello e passo fra i pannelli
    f0, f1, y0, y1 = 1, 1000, 90, 150
    px = lambda f: SX + (math.log10(f) - math.log10(f0)) / (math.log10(f1) - math.log10(f0)) * (L - SX - DX)
    pezzi = [f"<rect width='{L}' height='{H}' fill='{SUPERFICIE}'/>",
             testo(SX, 26, "Il falso Victor III al LOFAR, per fedeltà", 16, grassetto=True),
             testo(SX, 46, "12 nodi, 150 m. Righe dalle macchine e dalle pale; in grigio quelle che la falsa flag non ha più.",
                   12, SECONDARIO)]
    tutte = {(f, tipo): db for f, db, tipo in VICTOR[1.0]}
    for k, fed in enumerate((1.0, 0.8, 0.5)):
        top = SU + k * passo
        py = lambda db: top + alto - (db - y0) / (y1 - y0) * alto
        for f in (1, 10, 100, 1000):
            pezzi.append(f"<line x1='{px(f):.1f}' y1='{top}' x2='{px(f):.1f}' y2='{top + alto}' stroke='{GRIGLIA}'/>")
        for db in (100, 120, 140):
            pezzi.append(f"<line x1='{SX}' y1='{py(db):.1f}' x2='{L - DX}' y2='{py(db):.1f}' stroke='{GRIGLIA}'/>")
            pezzi.append(testo(SX - 8, py(db) + 4, str(db), 11, TENUE, "end"))
        pezzi.append(f"<rect x='{SX}' y='{top}' width='{L - SX - DX}' height='{alto}' fill='none' stroke='{ASSE}'/>")
        presenti = {(f, tipo) for f, _, tipo in VICTOR[fed]}
        for (f, tipo), db in tutte.items():   # le righe sparite: il fantasma della fedelta' piena
            if (f, tipo) not in presenti:
                pezzi.append(f"<line x1='{px(f):.1f}' y1='{top + alto}' x2='{px(f):.1f}' y2='{py(db):.1f}' "
                             f"stroke='{TENUE}' stroke-width='1.5' stroke-dasharray='2 3'/>")
        for f, db, tipo in VICTOR[fed]:
            colore = SERIE[1] if tipo == "elica" else SERIE[0]
            pezzi.append(f"<line x1='{px(f):.1f}' y1='{top + alto}' x2='{px(f):.1f}' y2='{py(db):.1f}' "
                         f"stroke='{colore}' stroke-width='2.5'/>")
            pezzi.append(f"<circle cx='{px(f):.1f}' cy='{py(db):.1f}' r='2.5' fill='{colore}'/>")
        note = {1.0: "tutte le righe", 0.8: "4 righe sparite, le altre\n1,2 dB più deboli",
                0.5: "sotto 0,7 niente pale\nné DEMON; le altre 3 dB\npiù deboli"}[fed]
        pezzi.append(testo(L - DX + 14, top + 16, f"fedeltà {num(fed, 1)}", 12, grassetto=True))
        for r, parte in enumerate(note.split("\n")):
            pezzi.append(testo(L - DX + 14, top + 34 + r * 14, parte, 11, SECONDARIO))
    fondo = SU + 2 * passo + alto
    for f, t in ((1, "1"), (10, "10"), (100, "100"), (1000, "1 k")):
        pezzi.append(testo(px(f), fondo + 18, t, 11, TENUE, "middle"))
    pezzi.append(testo((SX + L - DX) / 2, fondo + 38, "frequenza (Hz)", 12, SECONDARIO, "middle"))
    yc = SU + (fondo - SU) / 2
    pezzi.append(f"<text x='18' y='{yc:.0f}' font-size='12' fill='{SECONDARIO}' text-anchor='middle' "
                 f"transform='rotate(-90 18 {yc:.0f})'>livello della riga (dB re 1 µPa a 1 m)</text>")
    for i, (colore, nome) in enumerate(((SERIE[0], "macchine (rete, pompe, turbine, riduttori)"),
                                        (SERIE[1], "pale dell'elica"), (TENUE, "riga sparita"))):
        x, y = SX + (0, 300, 450)[i], fondo + 62   # la legenda in una riga sotto l'asse
        tratto = " stroke-dasharray='2 3'" if i == 2 else ""
        pezzi.append(f"<line x1='{x}' y1='{y}' x2='{x + 20}' y2='{y}' stroke='{colore}' stroke-width='2.5'{tratto}/>")
        pezzi.append(testo(x + 26, y + 4, nome, 11))
    svg("c5_falso_victor.svg", "acustica", L, H,
        "Spettro a righe di un falso Victor III a fedeltà 1, 0,8 e 0,5: righe che spariscono e si indeboliscono, "
        "senza le pale sotto 0,7", pezzi)


def rumore_pack():
    g = Grafico("Il rumore ambiente sotto il pack e in mare aperto",
                "Livello spettrale, traffico 0,5. Pack a copertura piena con le creste a 0,6.", 10, 20000, 20, 85, logx=True)
    g.assi([10, 100, 1000, 10000], range(20, 81, 10), "frequenza (Hz)", "livello spettrale (dB re 1 µPa²/Hz)",
           xfmt=lambda x: {10: "10", 100: "100", 1000: "1 k", 10000: "10 k"}[x])
    for i in range(3):
        g.linea([(r[0], r[i + 1]) for r in RUMORE], i)
    g.verticale(100, "creste, ~100 Hz")
    g.verticale(2000, "fessure, ~2 kHz")
    g.legenda(["mare aperto,\nvento 5 m/s", "pack, aria −30 °C\n(fessure termiche)", "pack, aria sopra\n−10 °C (niente fessure)"])
    fa.USCITA = FIGURE / "acustica"
    g.salva("e11_rumore_pack.svg", "Rumore ambiente sotto il pack, col freddo e senza, contro il mare aperto col vento a 5 m/s")


def quadrifoglio(passi=4, v=46.3, raggio=900.0, uscita=150.0, corsa=1500.0, dt=0.25):
    """La regola dei passaggi MAD: punta il fix; entro 150 m il passaggio e' fatto, tira dritto fino a 1.500 m, poi
    torna a puntarlo virando al raggio dato. Restituisce la traccia (x, y) in metri e i punti di ogni passaggio."""
    x, y, rotta = -1500.0, -300.0, 0.0   # l'arrivo: da sud-ovest, verso nord-est
    rotta = math.atan2(-x, -y)           # rotta verso il centro (0 = nord, x est)
    omega = v / raggio
    traccia, modo, passaggi = [(x, y)], "punta", 0
    while passaggi < passi:
        d = math.hypot(x, y)
        if modo == "punta":
            voluta = math.atan2(-x, -y)
            diff = (voluta - rotta + math.pi) % (2 * math.pi) - math.pi
            rotta += max(-omega * dt, min(omega * dt, diff))
            if d < uscita:
                modo, passaggi = "dritto", passaggi + 1
        elif d >= corsa:
            modo = "punta"
        x += v * math.sin(rotta) * dt
        y += v * math.cos(rotta) * dt
        traccia.append((x, y))
    # fin che esce dall'ultimo passaggio
    while math.hypot(x, y) < corsa:
        x += v * math.sin(rotta) * dt
        y += v * math.cos(rotta) * dt
        traccia.append((x, y))
    return traccia


def stesura_vecchia(raggio=900.0, corsa=1500.0):
    """La stesura vecchia: capi fissi a 1.500 m su assi ruotati di 45 gradi; si va dal capo all'opposto per il centro,
    poi al capo successivo lungo una corda. Qui la sola geometria ideale dei capi."""
    capi = []
    for k in range(4):
        a = math.radians(45 * k)
        capi += [(corsa * math.sin(a), corsa * math.cos(a)), (-corsa * math.sin(a), -corsa * math.cos(a))]
    return capi


def figura_quadrifoglio():
    L, H = 760, 440
    pezzi = [f"<rect width='{L}' height='{H}' fill='{SUPERFICIE}'/>",
             testo(36, 26, "I passaggi MAD del Lynx sul fix, visti dall'alto", 16, grassetto=True),
             testo(36, 46, "90 nodi a 46 m. A sinistra la regola d'oggi, a destra la stesura vecchia coi capi fissi.", 12, SECONDARIO)]
    scala = 150 / 2700.0   # pixel per metro: 2.700 m in 150 px (le virate a 900 m escono oltre i 2,5 km)
    for cx, titolo in ((200, "oggi: punta, passa, 1.500 m dritto, torna"), (560, "prima: capi fissi a 45°")):
        cy = 245
        for r in (500, 1500, 2500):
            pezzi.append(f"<circle cx='{cx}' cy='{cy}' r='{r * scala:.1f}' fill='none' stroke='{GRIGLIA}'/>")
            pezzi.append(testo(cx + r * scala * 0.707 + 2, cy - r * scala * 0.707 - 2, f"{num(r / 1000, 1)} km", 10, TENUE))
        pezzi.append(f"<circle cx='{cx}' cy='{cy}' r='{150 * scala:.1f}' fill='none' stroke='{SECONDARIO}' stroke-dasharray='2 2'/>")
        pezzi.append(f"<path d='M{cx - 5},{cy} L{cx + 5},{cy} M{cx},{cy - 5} L{cx},{cy + 5}' stroke='{INCHIOSTRO}' stroke-width='2'/>")
        pezzi.append(testo(cx, 432 - 20, titolo, 12, SECONDARIO, "middle"))
    t = quadrifoglio()
    d = " ".join(f"{'M' if k == 0 else 'L'}{200 + x * scala:.1f},{245 - y * scala:.1f}" for k, (x, y) in enumerate(t[::4]))
    pezzi.append(f"<path d='{d}' fill='none' stroke='{SERIE[0]}' stroke-width='2' stroke-linejoin='round'/>")
    x0, y0 = t[0]
    pezzi.append(f"<circle cx='{200 + x0 * scala:.1f}' cy='{245 - y0 * scala:.1f}' r='3.5' fill='{SERIE[0]}'/>")
    pezzi.append(testo(200 + x0 * scala - 6, 245 - y0 * scala + 16, "arrivo", 10, SECONDARIO, "end"))
    capi = stesura_vecchia()
    d = " ".join(f"{'M' if k == 0 else 'L'}{560 + x * scala:.1f},{245 - y * scala:.1f}" for k, (x, y) in enumerate(capi))
    pezzi.append(f"<path d='{d}' fill='none' stroke='{SERIE[1]}' stroke-width='2' stroke-dasharray='{TRATTI[1]}' stroke-linejoin='round'/>")
    for x, y in capi:
        pezzi.append(f"<circle cx='{560 + x * scala:.1f}' cy='{245 - y * scala:.1f}' r='3' fill='{SERIE[1]}'/>")
    pezzi.append(testo(560, 82, "corde a ~574 m dal centro; col raggio di", 10, SECONDARIO, "middle"))
    pezzi.append(testo(560, 95, "virata (~900 m) i passaggi cadevano a 700-800 m", 10, SECONDARIO, "middle"))
    pezzi.append(testo(200, 82, "la virata fa girare l'asse: ogni petalo", 10, SECONDARIO, "middle"))
    pezzi.append(testo(200, 95, "passa entro 150 m (cerchio tratteggiato)", 10, SECONDARIO, "middle"))
    svg("e5_quadrifoglio.svg", "schemi", L, H,
        "Passaggi MAD del Lynx sul fix: la regola d'oggi a petali che passano sul bersaglio, la stesura vecchia a capi fissi "
        "che passava a 700-800 m", pezzi)


def figura_volta():
    """Taglio verticale della volta: ghiaccio livellato di 1 m, chiglie di 4 e 15 m, una polynya, un tratto di nilas;
    sotto il 688 col cono del sondatore verso l'alto e la spazzata del BQS-15 a prua. Quote esagerate."""
    L, H = 760, 440
    SX, DX, SU, GIU = 60, 40, 70, 50
    x0, x1, z0, z1 = 0.0, 1000.0, -4.0, 80.0   # m lungo il taglio, m di quota (positiva in giu')
    px = lambda x: SX + (x - x0) / (x1 - x0) * (L - SX - DX)
    pz = lambda z: SU + (z - z0) / (z1 - z0) * (H - SU - GIU)
    pezzi = [f"<rect width='{L}' height='{H}' fill='{SUPERFICIE}'/>",
             testo(SX, 26, "Sotto la volta: un taglio del ghiaccio", 16, grassetto=True),
             testo(SX, 46, "La scala verticale è esagerata 6 volte. Pescaggi delle chiglie e del ghiaccio in metri.", 12, SECONDARIO)]
    for z in (0, 20, 40, 60, 80):
        pezzi.append(f"<line x1='{SX}' y1='{pz(z):.1f}' x2='{L - DX}' y2='{pz(z):.1f}' stroke='{GRIGLIA}'/>")
        pezzi.append(testo(SX - 8, pz(z) + 4, f"{z} m", 11, TENUE, "end"))
    pezzi.append(f"<rect x='{SX}' y='{pz(z0):.1f}' width='{L - SX - DX}' height='{pz(z1) - pz(z0):.1f}' fill='none' stroke='{ASSE}'/>")
    # il profilo della chiglia del ghiaccio: (x, pescaggio); None = acqua libera
    profilo = [(0, 1.0), (90, 1.0), (130, 4.0), (170, 1.0), (300, 1.0), (330, None), (430, None), (460, 1.0),
               (560, 1.0), (620, 15.0), (690, 1.0), (760, 1.0), (790, 0.1), (920, 0.1), (950, 1.0), (1000, 1.0)]
    ghiaccio = "#a9c7e6"
    tratto = []
    for x, d in profilo + [(None, None)]:
        if d is None or x is None:
            if len(tratto) > 1:
                sopra = " ".join(f"L{px(a):.1f},{pz(-0.6):.1f}" for a, _ in reversed(tratto))
                sotto = " ".join(f"{'M' if k == 0 else 'L'}{px(a):.1f},{pz(b):.1f}" for k, (a, b) in enumerate(tratto))
                pezzi.append(f"<path d='{sotto} {sopra} Z' fill='{ghiaccio}' stroke='{SECONDARIO}' stroke-width='1'/>")
            tratto = []
        else:
            tratto.append((x, d))
    pezzi.append(f"<line x1='{SX}' y1='{pz(0):.1f}' x2='{L - DX}' y2='{pz(0):.1f}' stroke='{SERIE[0]}' stroke-width='1' stroke-dasharray='4 3'/>")
    for x, t in ((130, "chiglia di 4 m"), (380, "polynya (acqua libera)"), (620, "chiglia di 15 m"), (855, "nilas, 10 cm"),
                 (60, "livellato, 1 m")):
        d = dict((a, b) for a, b in profilo).get(x)
        y = pz(d if d else 0) + 16 if x != 380 else pz(0) - 6
        pezzi.append(testo(px(x), y, t, 11, INCHIOSTRO, "middle"))
    # il 688 a 45 m, prua a destra; il sondatore verso l'alto (cono di 12 gradi) e la spazzata del BQS-15 a prua
    zb, xb, lb = 45.0, 420.0, 110.0
    scafo = (f"M{px(xb - lb / 2):.1f},{pz(zb):.1f} Q{px(xb - lb / 2):.1f},{pz(zb - 3.5):.1f} {px(xb - lb / 2 + 15):.1f},{pz(zb - 3.5):.1f} "
             f"L{px(xb + lb / 2 - 12):.1f},{pz(zb - 3.5):.1f} Q{px(xb + lb / 2):.1f},{pz(zb - 3.5):.1f} {px(xb + lb / 2):.1f},{pz(zb):.1f} "
             f"Q{px(xb + lb / 2):.1f},{pz(zb + 3.5):.1f} {px(xb + lb / 2 - 12):.1f},{pz(zb + 3.5):.1f} "
             f"L{px(xb - lb / 2 + 15):.1f},{pz(zb + 3.5):.1f} Q{px(xb - lb / 2):.1f},{pz(zb + 3.5):.1f} {px(xb - lb / 2):.1f},{pz(zb):.1f} Z")
    pezzi.append(f"<path d='{scafo}' fill='#52514e' stroke='{INCHIOSTRO}'/>")
    pezzi.append(f"<rect x='{px(xb + 5):.1f}' y='{pz(zb - 9):.1f}' width='{px(xb + 20) - px(xb + 5):.1f}' height='{pz(zb - 3.5) - pz(zb - 9):.1f}' fill='#52514e' stroke='{INCHIOSTRO}'/>")
    # cono del sondatore: semiapertura 6 gradi, dalla vela alla superficie del ghiaccio
    h = zb - 9
    w = h * math.tan(math.radians(6))
    pezzi.append(f"<path d='M{px(xb + 12):.1f},{pz(zb - 9):.1f} L{px(xb + 12 - w):.1f},{pz(1):.1f} L{px(xb + 12 + w):.1f},{pz(1):.1f} Z' "
                 f"fill='{SERIE[2]}' fill-opacity='0.35' stroke='{SERIE[2]}'/>")
    pezzi.append(testo(px(xb + 12 - w) - 6, pz(28), "sondatore verso l'alto, cono di 12°", 11, INCHIOSTRO, "end"))
    # BQS-15 a prua: un settore in avanti, da 30 gradi sotto a 30 sopra l'orizzonte, fino a 400 m
    xp, r = xb + lb / 2, 400.0
    a0, a1 = math.radians(-30), math.radians(30)
    punti = [(xp + r * math.cos(a), zb - r * math.sin(a) / 6.0) for a in [a0 + (a1 - a0) * k / 20 for k in range(21)]]
    d = f"M{px(xp):.1f},{pz(zb):.1f} " + " ".join(f"L{px(x):.1f},{pz(min(max(z, z0), z1)):.1f}" for x, z in punti) + " Z"
    pezzi.append(f"<path d='{d}' fill='{SERIE[3]}' fill-opacity='0.25' stroke='{SERIE[3]}'/>")
    pezzi.append(testo(px(xp + 200), pz(zb) + 4, "BQS-15 a prua: chiglie e mine", 11, INCHIOSTRO, "middle"))
    pezzi.append(testo(px(xb), pz(zb + 3.5) + 16, "688 a 45 m", 11, SECONDARIO, "middle"))
    pezzi.append(testo((SX + L - DX) / 2, H - 16, "distanza lungo il taglio (1 km in tutto)", 12, SECONDARIO, "middle"))
    svg("e11_volta.svg", "schemi", L, H,
        "Taglio verticale della volta di ghiaccio con chiglie di 4 e 15 m, polynya e nilas; sotto il 688 col sondatore "
        "verso l'alto e la spazzata del BQS-15 a prua", pezzi)


def main():
    falso_victor()
    rumore_pack()
    figura_quadrifoglio()
    figura_volta()


if __name__ == "__main__":
    main()
