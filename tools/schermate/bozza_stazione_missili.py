#!/usr/bin/env python3
"""Bozza della stazione di tiro dei missili dell'Enhanced (docs/21_stazione_missili.md): due pagine di F7 a 640x480,
disegnate con i font del progetto (fosforo e incisione del 688) e i colori della stazione Armi di oggi. E' solo una
figura da mostrare all'utente: il motore non la usa.

  tools/venv/bin/python tools/schermate/bozza_stazione_missili.py [cartella]

Scrive f7_missili_antinave.png (Harpoon/TASM, modo RBL sulla carta di tiro) e f7_missili_tlam.png (missione TLAM e
area di lancio) in docs/ricerca/27_note/ o nella cartella data.
"""
import math
import struct
import sys
from pathlib import Path

from PIL import Image, ImageDraw

RADICE = Path(__file__).resolve().parents[2]
FONT = RADICE / "data" / "motore" / "font"

# colori presi dalla stazione Armi di oggi (docs/manuale_enhanced/figure/armi_688.png)
NERO = (0, 0, 0)
GRIGIO = (109, 114, 115)
GRIGIO_SCURO = (85, 85, 85)
FERRO = (30, 33, 35)
VETRO = (6, 12, 8)
FOSFORO = [(6, 12, 8), (16, 74, 26), (61, 200, 80), (85, 255, 85)]
AMBRA = (255, 176, 40)
ROSSO = (220, 40, 30)
BIANCO = (230, 232, 230)
MARE = [(0, 40, 80), (0, 57, 110), (0, 63, 125), (0, 93, 151)]
CIANO = (85, 255, 255)
GIALLO = (255, 255, 85)
MAGENTA = (255, 85, 255)


class Font:
    """FNT2 del progetto (engine/grafica/src/font.cpp): 2 bit per colonna, colonna 0 nei bit alti."""

    def __init__(self, nome):
        d = (FONT / nome).read_bytes()
        assert d[:4] == b"FNT2", nome
        n, self.righe, self.spazio = struct.unpack_from("<HBB", d, 4)
        p = 8
        self.glifi = {}
        for _ in range(n):
            cp, w = struct.unpack_from("<IB", d, p)
            p += 5
            righe = struct.unpack_from("<%dI" % self.righe, d, p)
            p += 4 * self.righe
            self.glifi[chr(cp)] = (w, righe)

    def larghezza(self, s):
        return sum(self.glifi.get(c, self.glifi["?"])[0] + self.spazio for c in s)

    def scrivi(self, im, x, y, s, colore, livelli=None):
        """livelli: i colori per i toni 1-3 (alone, mezzo, nucleo); senza, il tono 3 e' `colore` e gli altri sfumano"""
        px = im.load()
        for c in s.upper() if not livelli else s:
            w, righe = self.glifi.get(c, self.glifi["?"])
            for r, v in enumerate(righe):
                for k in range(w):
                    tono = (v >> (30 - 2 * k)) & 3
                    if not tono:
                        continue
                    if livelli:
                        col = livelli[tono]
                    else:
                        f = (0.45, 0.75, 1.0)[tono - 1]
                        col = tuple(int(a * f) for a in colore)
                    if 0 <= x + k < im.width and 0 <= y + r < im.height:
                        px[x + k, y + r] = col
            x += w + self.spazio
        return x


def cornice(d, r, titolo, f_targa, im):
    x, y, w, h = r
    d.rectangle([x, y, x + w - 1, y + h - 1], fill=GRIGIO)
    d.rectangle([x + 4, y + 12, x + w - 5, y + h - 5], fill=VETRO, outline=FERRO)
    tw = f_targa.larghezza(titolo) + 6
    d.rectangle([x + 4, y + 2, x + 4 + tw, y + 11], fill=NERO)
    f_targa.scrivi(im, x + 7, y + 3, titolo, BIANCO)


def lampada(d, x, y, accesa, colore):
    spenta = tuple(c // 3 for c in colore)
    d.rectangle([x, y, x + 15, y + 6], fill=colore if accesa else spenta, outline=FERRO)


def croce(d, x, y, r, col):
    d.line([x - r, y, x + r, y], fill=col)
    d.line([x, y - r, x, y + r], fill=col)


def mare(im, d, area):
    x0, y0, x1, y1 = area
    for y in range(y0, y1):
        for x in range(x0, x1):
            t = MARE[((x * 7 + y * 3) // 11 + (x // 37) + (y // 29)) % 4 if (x + y) % 3 else 1]
            im.putpixel((x, y), t)
    d.rectangle(area, outline=GRIGIO_SCURO)


def tratteggio_cerchio(d, c, r, col, passo=8):
    for a in range(0, 360, passo):
        a0, a1 = math.radians(a), math.radians(a + passo / 2)
        d.line([c[0] + r * math.sin(a0), c[1] - r * math.cos(a0), c[0] + r * math.sin(a1), c[1] - r * math.cos(a1)],
               fill=col)


def polare(c, rilevamento, dist):
    a = math.radians(rilevamento)
    return (c[0] + dist * math.sin(a), c[1] - dist * math.cos(a))


def barra_bassa(im, d, f, f_targa, stazione, messaggio):
    d.rectangle([0, 360, 639, 479], fill=GRIGIO)
    d.rectangle([0, 360, 639, 371], fill=GRIGIO_SCURO)
    f.scrivi(im, 6, 362, stazione, CIANO)
    f.scrivi(im, 470, 362, "ORA 03:41:12Z  TEMPO X1", CIANO)
    d.rectangle([0, 466, 639, 479], fill=GRIGIO_SCURO)
    f.scrivi(im, 6, 469, messaggio, (85, 255, 85))


def antinave(f, fg, f_targa):
    im = Image.new("RGB", (640, 480), NERO)
    d = ImageDraw.Draw(im)
    # carta di tiro: il battello in basso a sinistra, scala 1 px = 200 m
    M = 200.0
    mare(im, d, (0, 0, 640, 358))
    f.scrivi(im, 6, 4, "CARTA DI TIRO  SCALA 64 KM  TUBO 3 HARPOON  RBL", CIANO)
    f.scrivi(im, 6, 14, "PREVISTO A T+165 S: BERSAGLIO SPOSTATO DI 1.5 KM", GIALLO)
    io = (110, 330)
    croce(d, *io, 5, (85, 255, 85))
    d.ellipse([io[0] - 3, io[1] - 3, io[0] + 3, io[1] + 3], outline=(85, 255, 85))
    f.scrivi(im, io[0] + 8, io[1] + 2, "688", (85, 255, 85))
    # bersaglio stimato dalla TMA (ellisse) e punto previsto
    stima = polare(io, 47.5, 38200 / M)
    previsto = polare(stima, 210, 1500 / M)
    d.ellipse([stima[0] - 22, stima[1] - 10, stima[0] + 22, stima[1] + 10], outline=AMBRA)
    f.scrivi(im, int(stima[0]) + 24, int(stima[1]) - 4, "S4 KRIVAK II (TMA)", AMBRA)
    d.line([stima, previsto], fill=AMBRA)
    croce(d, int(previsto[0]), int(previsto[1]), 4, GIALLO)
    # rotta del missile e accensione del seeker
    d.line([io, previsto], fill=BIANCO)
    accensione = polare(io, 45.0, (38200 - 20000) / M)
    d.ellipse([accensione[0] - 3, accensione[1] - 3, accensione[0] + 3, accensione[1] + 3], fill=GIALLO)
    f.scrivi(im, int(accensione[0]) - 116, int(accensione[1]) - 3, "SEEKER ON 18.2 KM", GIALLO)
    # area di ricerca RBL: un rettangolo centrato sul previsto, lungo la rotta
    a = math.radians(45.0)
    ux, uy = math.sin(a), -math.cos(a)
    vx, vy = math.cos(a), math.sin(a)
    L, W = 30000 / M, 12000 / M
    angoli = [(previsto[0] + s * L / 2 * ux + t * W / 2 * vx, previsto[1] + s * L / 2 * uy + t * W / 2 * vy)
              for s, t in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
    for a0, a1 in zip(angoli, angoli[1:] + angoli[:1]):
        d.line([a0, a1], fill=MAGENTA)
    f.scrivi(im, int(angoli[1][0]) + 6, int(angoli[1][1]) - 6, "SCATOLA RBL MEDIA", MAGENTA)
    # un neutrale e un'altra nave nemica vicini all'area: il seeker puo' agganciare il primo che vede
    neutrale = polare(previsto, 300, 3300 / M)
    d.rectangle([neutrale[0] - 3, neutrale[1] - 3, neutrale[0] + 3, neutrale[1] + 3], outline=BIANCO)
    f.scrivi(im, int(neutrale[0]) - 110, int(neutrale[1]) - 3, "S7 MERCANTILE?", BIANCO)
    altra = polare(previsto, 150, 4200 / M)
    d.polygon([(altra[0], altra[1] - 4), (altra[0] + 4, altra[1]), (altra[0], altra[1] + 4), (altra[0] - 4, altra[1])],
              outline=ROSSO)
    f.scrivi(im, int(altra[0]) + 7, int(altra[1]) - 3, "S5 KARA", ROSSO)
    # il datum che lascera' il lancio e l'orizzonte radar del bersaglio
    tratteggio_cerchio(d, io, 3 * 1852 / M, (85, 255, 85))
    f.scrivi(im, io[0] - 40, io[1] - 52, "DATUM 3 NM", (85, 255, 85))
    tratteggio_cerchio(d, previsto, 28000 / M, (120, 120, 160), 6)
    f.scrivi(im, 420, 340, "ORIZZONTE RADAR DEL BERSAGLIO 28 KM", (150, 150, 190))

    barra_bassa(im, d, f, f_targa, "ARMI TORPEX '89 1.5X", "TIRO: HARPOON TUBO 3 PRONTO. CONSENSO DEL COMANDANTE?")
    cornice(d, (4, 374, 352, 90), "MK 117 FIRE CONTROL  MISSILE", f_targa, im)
    righe = [
        "TUBO 3 UGM-84C  ALLINEATO      RDY",
        "BERS S4 KRIVAK II  FONTE TMA Q 0.82",
        "RIL 047.5 DIST 38.2 KM  R 210 V 18",
        "MODO RBL  RICERCA MED (SKR 20 KM)",
        "SCATOLA 30 X 12 KM  TERM RADENTE",
        "VOLO 165 S  NEUTRALI IN SCATOLA 1",
        "P SUCC STIM 0.58  QUOTA VEL OK",
    ]
    for k, s in enumerate(righe):
        f.scrivi(im, 12, 388 + k * 10, s, None, FOSFORO)
    cornice(d, (360, 374, 276, 90), "WEAPON CONTROL PANEL", f_targa, im)
    etichette = ["PWR", "ALN", "RDY", "DOOR", "SLIDE"]
    for r, e in enumerate(etichette):
        f_targa.scrivi(im, 366, 390 + r * 11, e, BIANCO)
    stati = {1: (1, 1, 0, 0, 0), 2: (0, 0, 0, 0, 0), 3: (1, 1, 1, 1, 1), 4: (1, 0, 0, 0, 0)}
    for k in range(4):
        x = 404 + k * 34
        f_targa.scrivi(im, x + 2, 381, "%d" % (k + 1), BIANCO)
        for r in range(5):
            lampada(d, x, 390 + r * 11, stati[k + 1][r], (85, 255, 85) if r < 3 else AMBRA)
    d.rectangle([548, 386, 628, 456], fill=FERRO)
    coperchio = Image.new("RGB", (65, 39), (140, 20, 20))   # il tratteggio ritagliato sul coperchio
    dc = ImageDraw.Draw(coperchio)
    for k in range(-6, 10):
        dc.line([k * 8, 38, k * 8 + 38, 0], fill=(110, 10, 10))
    im.paste(coperchio, (556, 392))
    d.rectangle([556, 392, 620, 430], outline=ROSSO)
    f_targa.scrivi(im, 572, 436, "FIRE", BIANCO)
    f_targa.scrivi(im, 556, 448, "CHIAVE CO", AMBRA)
    return im


def tlam(f, fg, f_targa):
    im = Image.new("RGB", (640, 480), NERO)
    d = ImageDraw.Draw(im)
    mare(im, d, (0, 0, 640, 358))
    # costa a destra (la tratta di mare fino al landfall)
    d.polygon([(520, 0), (640, 0), (640, 358), (560, 358), (548, 300), (575, 240), (540, 170), (556, 90)],
              fill=(120, 96, 60), outline=(160, 130, 80))
    f.scrivi(im, 6, 4, "CARTA DI TIRO  SCALA 200 KM  MISSIONE TLAM-C 0417", CIANO)
    f.scrivi(im, 6, 14, "PLP: VALIDA ENTRO 10 NM  RIPIANIFICA FINO A 40", GIALLO)
    centro = (250, 210)
    d.ellipse([centro[0] - 40, centro[1] - 40, centro[0] + 40, centro[1] + 40], outline=GIALLO)
    f.scrivi(im, centro[0] - 46, centro[1] + 46, "AREA DI LANCIO", GIALLO)
    # la stima del battello con l'ellisse di navigazione, dentro l'area
    io = (236, 222)
    d.ellipse([io[0] - 9, io[1] - 6, io[0] + 9, io[1] + 6], outline=(85, 255, 85))
    croce(d, *io, 4, (85, 255, 85))
    f.scrivi(im, io[0] - 92, io[1] - 3, "688 STIMA", (85, 255, 85))
    # la rotta preprogrammata: tratta sul mare (inerziale), landfall, TERCOM
    landfall = (548, 160)
    tratto = [centro, (380, 190), landfall, (600, 120), (636, 80)]
    for a, b in zip(tratto, tratto[1:]):
        d.line([a, b], fill=BIANCO)
    for p in tratto[1:-1]:
        croce(d, int(p[0]), int(p[1]), 3, BIANCO)
    f.scrivi(im, 330, 172, "INERZIALE", BIANCO)
    f.scrivi(im, landfall[0] - 84, landfall[1] + 8, "LANDFALL", BIANCO)
    f.scrivi(im, 560, 100, "TERCOM", BIANCO)
    barra_bassa(im, d, f, f_targa, "ARMI TORPEX '89 1.5X", "RADIO: ORDINE DI ESECUZIONE 0417 RICEVUTO. TOT 0412Z +-10 MIN.")
    cornice(d, (4, 374, 352, 90), "CCS MK 1  TOMAHAWK", f_targa, im)
    righe = [
        "VLS 5 UGM-109C  MISSIONE 0417",
        "STATO VALIDA  2.1 NM DAL PLP",
        "PREP 1/4 PRONTO  2/4 TRA 4 MIN",
        "NAV 0.6 NM  P LANDFALL STIM 96%",
        "TOT 0412Z +-10  ESEC INDIGO SI",
        "QUOTA 25 M  VEL 5 KN OK",
        "PUNTO NAVE: TRANSIT 03:12Z",
    ]
    for k, s in enumerate(righe):
        f.scrivi(im, 12, 388 + k * 10, s, None, FOSFORO)
    cornice(d, (360, 374, 276, 90), "LAUNCH CONTROL  VLS", f_targa, im)
    for k in range(12):
        x, y = 372 + (k % 6) * 28, 390 + (k // 6) * 30
        stato = 2 if k == 4 else (1 if k < 8 else 0)
        col = (85, 255, 85) if stato == 2 else (AMBRA if stato == 1 else GRIGIO_SCURO)
        d.ellipse([x, y, x + 20, y + 20], fill=FERRO, outline=col)
        f_targa.scrivi(im, x + 5 if k < 9 else x + 3, y + 7, "%d" % (k + 1), col if stato else GRIGIO)
    d.rectangle([548, 386, 628, 456], fill=FERRO)
    d.rectangle([556, 392, 620, 430], fill=(140, 20, 20), outline=ROSSO)
    f_targa.scrivi(im, 568, 436, "LAUNCH", BIANCO)
    f_targa.scrivi(im, 556, 448, "CHIAVE CO", AMBRA)
    return im


def main():
    uscita = Path(sys.argv[1]) if len(sys.argv) > 1 else RADICE / "docs" / "ricerca" / "27_note"
    uscita.mkdir(parents=True, exist_ok=True)
    f = Font("fosforo688_5x7.fnt")
    fg = Font("fosforo688_7x9.fnt")
    f_targa = Font("incisione688_5.fnt")
    for nome, fn in (("f7_missili_antinave.png", antinave), ("f7_missili_tlam.png", tlam)):
        im = fn(f, fg, f_targa)
        im.save(uscita / nome)
        print(uscita / nome)


if __name__ == "__main__":
    main()
