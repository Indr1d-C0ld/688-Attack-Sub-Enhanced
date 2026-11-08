"""Figure dei capitoli C2, C3 e C8 del manuale dell'Enhanced: grafici SVG dai numeri del motore.

I dati li scrive tools/manuale/figure_acustica.cpp (come compilarlo è in testa al file) in figure_acustica.dat;
questo script disegna le figure in docs/manuale_enhanced/figure/acustica, senza librerie esterne.
    tools/venv/bin/python tools/manuale/figure_acustica.py
"""
import math
import pathlib

RADICE = pathlib.Path(__file__).resolve().parents[2]
DATI = RADICE / "tools/manuale/figure_acustica.dat"
USCITA = RADICE / "docs/manuale_enhanced/figure/acustica"

# palette di riferimento (superficie chiara): serie nell'ordine fisso, inchiostri, griglia
SERIE = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100"]
TRATTI = ["", "7 4", "2 3", "10 3 2 3"]   # seconda codifica, oltre al colore
SUPERFICIE, INCHIOSTRO, SECONDARIO, TENUE = "#fcfcfb", "#0b0b0b", "#52514e", "#898781"
GRIGLIA, ASSE = "#e1e0d9", "#c3c2b7"
CARATTERE = "font-family='DejaVu Sans, Helvetica, Arial, sans-serif'"

L, H = 760, 440
SX, DX, SU, GIU = 72, 190, 64, 56   # margini dell'area del grafico


def leggi():
    blocchi, nome = {}, None
    for riga in DATI.read_text().splitlines():
        if riga.startswith("#"):
            parti = riga[1:].split()
            nome = parti[0]
            blocchi[nome] = {"intestazione": parti[1:], "righe": []}
        elif riga.strip():
            blocchi[nome]["righe"].append([float(x) for x in riga.split()])
    return blocchi


def num(x, cifre=0):
    return f"{x:.{cifre}f}".replace(".", ",")


def esc(t):
    return t.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("'", "&#39;")


class Grafico:
    def __init__(self, titolo, sottotitolo, xmin, xmax, ymin, ymax, logx=False, y_giu=False):
        self.x0, self.x1, self.y0, self.y1 = xmin, xmax, ymin, ymax
        self.logx, self.y_giu = logx, y_giu
        self.pezzi = [f"<rect width='{L}' height='{H}' fill='{SUPERFICIE}'/>",
                      f"<text x='{SX}' y='26' font-size='16' font-weight='bold' fill='{INCHIOSTRO}'>{esc(titolo)}</text>",
                      f"<text x='{SX}' y='46' font-size='12' fill='{SECONDARIO}'>{esc(sottotitolo)}</text>"]

    def px(self, x):
        if self.logx:
            t = (math.log10(x) - math.log10(self.x0)) / (math.log10(self.x1) - math.log10(self.x0))
        else:
            t = (x - self.x0) / (self.x1 - self.x0)
        return SX + t * (L - SX - DX)

    def py(self, y):
        t = (y - self.y0) / (self.y1 - self.y0)
        return SU + t * (H - SU - GIU) if self.y_giu else H - GIU - t * (H - SU - GIU)

    def assi(self, xt, yt, xnome, ynome, xfmt=num, yfmt=num):
        for x in xt:
            p = self.px(x)
            self.pezzi.append(f"<line x1='{p:.1f}' y1='{SU}' x2='{p:.1f}' y2='{H - GIU}' stroke='{GRIGLIA}'/>")
            self.pezzi.append(f"<text x='{p:.1f}' y='{H - GIU + 18}' font-size='11' fill='{TENUE}' "
                              f"text-anchor='middle'>{xfmt(x)}</text>")
        for y in yt:
            p = self.py(y)
            self.pezzi.append(f"<line x1='{SX}' y1='{p:.1f}' x2='{L - DX}' y2='{p:.1f}' stroke='{GRIGLIA}'/>")
            self.pezzi.append(f"<text x='{SX - 8}' y='{p + 4:.1f}' font-size='11' fill='{TENUE}' "
                              f"text-anchor='end'>{yfmt(y)}</text>")
        self.pezzi.append(f"<rect x='{SX}' y='{SU}' width='{L - SX - DX}' height='{H - SU - GIU}' fill='none' "
                          f"stroke='{ASSE}'/>")
        self.pezzi.append(f"<text x='{(SX + L - DX) / 2:.0f}' y='{H - 14}' font-size='12' fill='{SECONDARIO}' "
                          f"text-anchor='middle'>{esc(xnome)}</text>")
        yc = (SU + H - GIU) / 2
        self.pezzi.append(f"<text x='18' y='{yc:.0f}' font-size='12' fill='{SECONDARIO}' text-anchor='middle' "
                          f"transform='rotate(-90 18 {yc:.0f})'>{esc(ynome)}</text>")

    def linea(self, punti, i, etichetta=None):
        punti = [(x, y) for x, y in punti if self.x0 <= x <= self.x1 and not math.isnan(y)]
        d = " ".join(f"{'M' if k == 0 else 'L'}{self.px(x):.1f},{self.py(min(max(y, min(self.y0, self.y1)), max(self.y0, self.y1))):.1f}"
                     for k, (x, y) in enumerate(punti))
        tratto = f" stroke-dasharray='{TRATTI[i]}'" if TRATTI[i] else ""
        self.pezzi.append(f"<path d='{d}' fill='none' stroke='{SERIE[i]}' stroke-width='2' "
                          f"stroke-linejoin='round'{tratto}/>")
        if etichetta:
            x, y = punti[-1]
            self.pezzi.append(f"<text x='{self.px(x) + 6:.1f}' y='{self.py(y) + 4:.1f}' font-size='11' "
                              f"fill='{INCHIOSTRO}'>{esc(etichetta)}</text>")

    def legenda(self, voci, x=None, y=None):
        x = L - DX + 14 if x is None else x
        y = SU + 6 if y is None else y
        for i, voce in enumerate(voci):
            tratto = f" stroke-dasharray='{TRATTI[i]}'" if TRATTI[i] else ""
            yy = y + i * 34
            self.pezzi.append(f"<line x1='{x}' y1='{yy}' x2='{x + 24}' y2='{yy}' stroke='{SERIE[i]}' "
                              f"stroke-width='2'{tratto}/>")
            for k, parte in enumerate(voce.split("\n")):
                self.pezzi.append(f"<text x='{x + 30}' y='{yy + 4 + k * 13}' font-size='11' "
                                  f"fill='{INCHIOSTRO}'>{esc(parte)}</text>")

    def verticale(self, x, testo):
        p = self.px(x)
        self.pezzi.append(f"<line x1='{p:.1f}' y1='{SU}' x2='{p:.1f}' y2='{H - GIU}' stroke='{SECONDARIO}' "
                          f"stroke-dasharray='3 3'/>")
        self.pezzi.append(f"<text x='{p - 4:.1f}' y='{SU + 14}' font-size='11' fill='{SECONDARIO}' "
                          f"text-anchor='end'>{esc(testo)}</text>")

    def orizzontale(self, y, testo):
        p = self.py(y)
        self.pezzi.append(f"<line x1='{SX}' y1='{p:.1f}' x2='{L - DX}' y2='{p:.1f}' stroke='{SECONDARIO}' "
                          f"stroke-dasharray='3 3'/>")
        self.pezzi.append(f"<text x='{L - DX + 6}' y='{p + 4:.1f}' font-size='11' fill='{SECONDARIO}'>{esc(testo)}</text>")

    def salva(self, nome, descrizione):
        USCITA.mkdir(parents=True, exist_ok=True)
        svg = (f"<svg xmlns='http://www.w3.org/2000/svg' width='{L}' height='{H}' viewBox='0 0 {L} {H}' "
               f"{CARATTERE} role='img' aria-label='{esc(descrizione)}'>\n<title>{esc(descrizione)}</title>\n"
               + "\n".join(self.pezzi) + "\n</svg>\n")
        (USCITA / nome).write_text(svg)
        print(USCITA / nome)


def perdite(blocco, titolo, sottotitolo, nomi, xmax, xt, nome, cz_km=None):
    righe = blocco["righe"]
    tutte = [v for r in righe for v in r[1:]]
    ymin = 10 * math.floor(min(tutte) / 10)
    ymax = 10 * math.ceil(min(max(tutte), 130) / 10)
    g = Grafico(titolo, sottotitolo, 0, xmax, ymin, ymax, y_giu=True)
    g.assi(xt, range(int(ymin), int(ymax) + 1, 10), "distanza (km)", "perdita di trasmissione (dB), in basso si perde di più")
    if cz_km:
        g.verticale(cz_km, f"prima CZ, {num(cz_km, 1)} km")
    for i in range(len(nomi)):
        g.linea([(r[0], r[i + 1]) for r in righe], i)
    g.legenda(nomi)
    g.salva(nome, f"{titolo}. {sottotitolo}")


def main():
    b = leggi()
    perdite(b["tl_faroe"], "Perdita di trasmissione alle Fær Øer, febbraio",
            "1 kHz; strato a 250 m, asse a 650 m. Sorgente a 150 m.",
            ["livello A,\nricevitore a 150 m", "livello A,\nricevitore a 400 m", "raggi,\nricevitore a 400 m"],
            60, range(0, 61, 10), "c2_perdite_faroe.svg", cz_km=51.0)
    perdite(b["tl_otranto"], "Perdita di trasmissione nel Canale d'Otranto, agosto",
            "1 kHz; nessuno strato, asse del canale a 95 m.",
            ["livello A,\n100 m → 100 m", "raggi,\n100 m → 100 m", "raggi,\n20 m → 150 m"],
            45, range(0, 46, 5), "c2_perdite_otranto.svg", cz_km=39.5)

    p = b["profilo_faroe"]
    meta = dict(zip(p["intestazione"][::2], map(float, p["intestazione"][1::2])))
    righe = [r for r in p["righe"] if r[0] <= 3000]
    cmin = 5 * math.floor(min(r[1] for r in righe) / 5)
    cmax = 5 * math.ceil(max(r[1] for r in righe) / 5)
    g = Grafico("Velocità del suono alle Fær Øer, febbraio", "Profilo mensile del teatro (WOA23), fino a 3000 m.",
                cmin, cmax, 0, 3000, y_giu=True)
    g.assi(range(int(cmin), int(cmax) + 1, 5), range(0, 3001, 500), "velocità del suono (m/s)", "quota (m)")
    vertice = next(r[1] for r in righe if r[0] >= meta["strato"])
    g.pezzi.append(f"<line x1='{g.px(vertice):.1f}' y1='{SU}' x2='{g.px(vertice):.1f}' y2='{H - GIU}' "
                   f"stroke='{TENUE}' stroke-dasharray='1 3'/>")
    g.orizzontale(meta["strato"], f"strato, {num(meta['strato'])} m")
    g.orizzontale(meta["asse"], f"asse, {num(meta['asse'])} m")
    g.orizzontale(meta["critica"], f"profondità critica, {num(meta['critica'])} m")
    g.linea([(r[1], r[0]) for r in righe], 0)
    g.pezzi.append(f"<text x='{g.px(vertice) + 4:.1f}' y='{SU + 14}' font-size='11' fill='{SECONDARIO}'>"
                   f"velocità del vertice, {num(vertice, 1)} m/s</text>")
    g.salva("c2_profilo_faroe.svg", "Velocità del suono alle Fær Øer in febbraio, con strato, asse e profondità critica")

    w = b["wenz"]["righe"]
    g = Grafico("Rumore ambiente del modello (curve di Wenz)", "Livello spettrale, traffico 0,5, senza pioggia né ghiaccio.",
                1, 100000, 10, 110, logx=True)
    g.assi([1, 10, 100, 1000, 10000, 100000], range(10, 111, 10), "frequenza (Hz)", "livello spettrale (dB re 1 µPa²/Hz)",
           xfmt=lambda x: {1: "1", 10: "10", 100: "100", 1000: "1 k", 10000: "10 k", 100000: "100 k"}[x])
    nomi = ["vento 0 m/s", "vento 5 m/s", "vento 10 m/s", "vento 15 m/s"]
    for i in range(4):
        g.linea([(r[0], r[i + 1]) for r in w], i)
    g.legenda(nomi)
    g.salva("c3_wenz.svg", "Curve di Wenz del modello: rumore ambiente per frequenza col vento a 0, 5, 10 e 15 m/s")


    s = b["scafo"]["righe"]
    frequenze = [1e1 * 2, 100, 1000, 10000]
    etichette = lambda x: {20: "20", 100: "100", 1000: "1 k", 10000: "10 k"}[int(x)]
    nomi = ["centrale", "sonar", "siluri"]
    g = Grafico("Lo scafo come filtro: K(f)", "Da dB re 1 µPa fuori a dB re 20 µPa dentro, col riverbero del locale.",
                20, 10000, 70, 120, logx=True)
    g.assi(frequenze, range(70, 121, 10), "frequenza (Hz)", "attenuazione dello scafo (dB)", xfmt=etichette)
    for i in range(3):
        g.linea([(r[0], r[i + 1]) for r in s], i)
    g.legenda(nomi)
    g.salva("c8_scafo_k.svg", "Trasferimento dello scafo K(f) per centrale, sonar e locale siluri")

    g = Grafico("Che cosa si sente a bordo", "Livello minimo fuori dallo scafo per superare la soglia d'udibilità dentro (senza il fondo del locale).",
                20, 10000, 80, 180, logx=True)
    g.assi(frequenze, range(80, 181, 10), "frequenza (Hz)", "livello allo scafo (dB re 1 µPa)", xfmt=etichette)
    for i in range(3):
        g.linea([(r[0], r[i + 1] + r[4]) for r in s], i)
    g.legenda(nomi)
    g.salva("c8_udibile.svg", "Livello minimo fuori dallo scafo che si sente in centrale, in sala sonar e nel locale siluri")

if __name__ == "__main__":
    main()
