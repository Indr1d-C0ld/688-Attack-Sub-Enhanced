"""Schemi geometrici del volume 2 del manuale (kit, 03/11/2026), nello stile di figure_acustica.py, coi numeri del gioco:
- D1: le fasi del siluro viste dall'alto (corsa fino all'RTE, ricerca in serpentina di ±30°, il cono della testa, l'aggancio);
- C6: la schiera filata, il rilevamento vero e lo specchio, i coni sordi dell'endfire (25° per parte);
- C9: i settori dei fanali del regolamento (albero 225°, fianchi 112,5°, poppa 135°) e la geometria dello stadimetro;
- C13: la TMA a due gambe (i rilevamenti da due rotte proprie che fissano la distanza);
- D4: il BQS-15 su un campo di mine (settore di ±60°, 1.100 m per le ormeggiate e 450 per quelle sul fondo a 5 nodi);
- E6: i quattro livelli dell'IA e gli strati che, in ordine, decidono la rotta voluta (Missione::navigazione);
- E4: la dispersione di 40 colpi del Mk 75 a 3 km (σ 4,5 m in direzione e 35 m in distanza), su un sottomarino in
  superficie visto di traverso e sulla testa di un periscopio;
- E5: gli schemi di posa delle boe, coi punti veri di posa_schema (tools/manuale/schemi_boe.txt, dalla sonda
  tools/manuale/sonda_schemi_boe.cpp);
- D1: la geometria del filo (i due rocchetti e i limiti del battello);
- C13: il regolo di Ekelund (la striscia graduata sulle linee di rilevamento).
Scrivono in docs/manuale_enhanced/figure/schemi.
    tools/venv/bin/python tools/manuale/figure_schemi.py
"""
import math
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from figure_acustica import ASSE, GRIGLIA, INCHIOSTRO, SECONDARIO, SERIE, SUPERFICIE, TENUE   # noqa: E402
from figure_tattiche import svg, testo   # noqa: E402

L, H = 760, 440


def fondo(titolo, sottotitolo):
    return [f"<rect width='{L}' height='{H}' fill='{SUPERFICIE}'/>", testo(36, 26, titolo, 16, grassetto=True),
            testo(36, 46, sottotitolo, 12, SECONDARIO)]


def linea(punti, colore, spessore=2, tratto=""):
    d = " ".join(f"{'M' if k == 0 else 'L'}{x:.1f},{y:.1f}" for k, (x, y) in enumerate(punti))
    t = f" stroke-dasharray='{tratto}'" if tratto else ""
    return f"<path d='{d}' fill='none' stroke='{colore}' stroke-width='{spessore}' stroke-linejoin='round'{t}/>"


def settore(cx, cy, r, da, a, colore, opacita=0.25):
    """settore circolare da `da` ad `a` gradi bussola (0 in alto, orario)"""
    p = lambda g: (cx + r * math.sin(math.radians(g)), cy - r * math.cos(math.radians(g)))
    (x0, y0), (x1, y1) = p(da), p(a)
    grande = 1 if (a - da) % 360 > 180 else 0
    return (f"<path d='M{cx},{cy} L{x0:.1f},{y0:.1f} A{r},{r} 0 {grande} 1 {x1:.1f},{y1:.1f} Z' fill='{colore}' "
            f"fill-opacity='{opacita}' stroke='{colore}'/>")


def battello(cx, cy, rotta, lungo=26, largo=7, colore="#52514e"):
    """il battello visto dall'alto, prua verso `rotta` (gradi bussola)"""
    r = math.radians(rotta)
    u, n = (math.sin(r), -math.cos(r)), (math.cos(r), math.sin(r))
    pt = lambda a, b: (cx + a * u[0] + b * n[0], cy + a * u[1] + b * n[1])
    punti = [pt(lungo / 2, 0), pt(lungo / 4, largo / 2), pt(-lungo / 2, largo / 2), pt(-lungo / 2, -largo / 2), pt(lungo / 4, -largo / 2)]
    d = " ".join(f"{'M' if k == 0 else 'L'}{x:.1f},{y:.1f}" for k, (x, y) in enumerate(punti)) + " Z"
    return f"<path d='{d}' fill='{colore}' stroke='{INCHIOSTRO}'/>"


def fasi_siluro():
    p = fondo("Le fasi del siluro, viste dall'alto", "Mk 48: corsa fino all'RTE, ricerca in serpentina di ±30°, aggancio nel cono della testa, inseguimento.")
    x0, y0, s = 70, 370, 0.07   # il lanciatore; 0,07 px per metro
    p.append(battello(x0, y0, 70))
    p.append(testo(x0 - 4, y0 + 22, "il 688", 11, SECONDARIO, "middle"))
    rte = 2500
    ux, uy = math.sin(math.radians(70)), -math.cos(math.radians(70))
    xr, yr = x0 + rte * s * ux, y0 + rte * s * uy
    p.append(linea([(x0 + 14 * ux, y0 + 14 * uy), (xr, yr)], SERIE[0], 2, "6 4"))
    p.append(testo((x0 + xr) / 2, (y0 + yr) / 2 + 18, "corsa fino all'RTE, testa spenta", 11, INCHIOSTRO, "middle"))
    p.append(f"<circle cx='{xr:.1f}' cy='{yr:.1f}' r='4' fill='{SERIE[0]}'/>")
    p.append(testo(xr, yr - 10, "RTE: la testa si accende", 11, INCHIOSTRO, "middle"))
    # la serpentina di ±30° fino all'aggancio
    pts, x, y, rot, lato = [(xr, yr)], xr, yr, 70.0, 1
    for k in range(4):
        rot = 70 + 30 * lato
        lun = 500 if k == 0 else 1000
        x += lun * s * math.sin(math.radians(rot))
        y -= lun * s * math.cos(math.radians(rot))
        pts.append((x, y))
        lato = -lato
    p.append(linea(pts, SERIE[0], 2))
    p.append(testo(pts[2][0], pts[2][1] - 12, "ricerca a serpentina ±30°", 11, INCHIOSTRO, "middle"))
    # il cono della testa dall'ultimo punto, ±45° e 4 km in passivo, e il bersaglio dentro
    xa, ya = pts[-1]
    p.append(settore(xa, ya, 4000 * s * 0.55, 70 - 45, 70 + 45, SERIE[3], 0.18))
    p.append(testo(xa + 90, ya + 46, "cono della testa: ±45°, 4 km in passivo", 11, INCHIOSTRO, "middle"))
    xb, yb = xa + 120, ya - 30
    p.append(battello(xb, yb, 200, colore=SERIE[1]))
    p.append(testo(xb + 18, yb - 10, "il bersaglio", 11, INCHIOSTRO))
    p.append(linea([(xa, ya), (xb - 20, yb + 10)], SERIE[1], 2.5))
    p.append(testo(xa + 60, ya - 10, "aggancio e inseguimento sul punto futuro", 11, INCHIOSTRO))
    p.append(linea([(600, 420), (600 + 1000 * s, 420)], INCHIOSTRO, 2))
    p.append(testo(600 + 500 * s, 414, "1 km", 11, SECONDARIO, "middle"))
    svg("d1_fasi_siluro.svg", "schemi", L, H,
        "Le fasi del siluro viste dall'alto: corsa fino all'RTE, ricerca in serpentina, cono della testa, aggancio", p)


def schiera_specchio():
    p = fondo("La schiera filata e lo specchio", "La schiera sente il rilevamento rispetto al suo asse: non sa da che lato. I coni dell'endfire (25°) sono sordi.")
    cx, cy = 360, 230
    p.append(battello(cx, cy, 90, 40, 10))
    p.append(testo(cx, cy + 24, "il 688, rotta 090", 11, SECONDARIO, "middle"))
    xa0, xa1 = cx - 60, cx - 190   # la schiera dietro il battello, lungo l'asse
    p.append(linea([(cx - 20, cy), (xa0, cy)], TENUE, 1.5, "3 3"))
    p.append(linea([(xa0, cy), (xa1, cy)], INCHIOSTRO, 4))
    p.append(testo((xa0 + xa1) / 2, cy + 18, "la schiera TB-16", 11, INCHIOSTRO, "middle"))
    xm = (xa0 + xa1) / 2
    # i coni dell'endfire, a prua e a poppa della schiera
    p.append(settore(xm, cy, 170, 90 - 25, 90 + 25, TENUE, 0.15))
    p.append(settore(xm, cy, 170, 270 - 25, 270 + 25, TENUE, 0.15))
    p.append(testo(xm + 180, cy + 4, "endfire: sordo", 11, SECONDARIO))
    p.append(testo(xm - 120, cy - 82, "endfire: sordo", 11, SECONDARIO, "middle"))
    # il contatto vero a 50° dall'asse, a nord-est, e lo specchio simmetrico rispetto all'asse
    ang = 50
    for segno, colore, nome in ((1, SERIE[0], "il contatto vero"), (-1, SERIE[1], "lo specchio")):
        rot = 90 - segno * ang   # rilevamento: a sinistra dell'asse (nord) il vero, a destra lo specchio
        x1, y1 = xm + 150 * math.sin(math.radians(rot)), cy - 150 * math.cos(math.radians(rot))
        p.append(linea([(xm, cy), (x1, y1)], colore, 2, "" if segno == 1 else "6 4"))
        p.append(f"<circle cx='{x1:.1f}' cy='{y1:.1f}' r='5' fill='{colore}'/>")
        p.append(testo(x1 + 8, y1 + (-6 if segno == 1 else 16), f"{nome}, {rot:.0f}°", 11, INCHIOSTRO))
    p.append(testo(36, 425, "Accostando di 30-60° lo specchio salta dall'altra parte dell'asse, il vero resta: così si risolve.", 12, SECONDARIO))
    svg("c6_schiera_specchio.svg", "schemi", L, H,
        "La schiera filata dietro il battello: il rilevamento vero e il suo specchio rispetto all'asse, i coni sordi dell'endfire", p)


def fanali():
    p = fondo("I settori dei fanali", "Dal regolamento internazionale (COLREG): albero 225°, fianchi 112,5° ciascuno, poppa 135°.")
    cx, cy, r = 250, 250, 150
    p.append(settore(cx, cy, r + 30, -112.5, 112.5, "#c3c2b7", 0.35))
    p.append(settore(cx, cy, r, 0, 112.5, "#1baf7a", 0.35))
    p.append(settore(cx, cy, r, -112.5, 0, "#d03b2f", 0.35))
    p.append(settore(cx, cy, r - 20, 112.5, 247.5, "#c3c2b7", 0.6))
    p.append(battello(cx, cy, 0, 60, 16))
    p.append(testo(cx, cy - r - 40, "fanale d'albero, bianco, 225°", 12, INCHIOSTRO, "middle"))
    p.append(testo(cx + 110, cy - 60, "verde, dritta", 12, INCHIOSTRO))
    p.append(testo(cx - 110, cy - 60, "rosso, sinistra", 12, INCHIOSTRO, "end"))
    p.append(testo(cx, cy + r + 4, "fanale di poppa, bianco, 135°", 12, INCHIOSTRO, "middle"))
    righe = ["Che cosa vedi:",
             "- verde e rosso insieme: ti viene incontro;",
             "- solo il verde: ti mostra la dritta;",
             "- solo il rosso: ti mostra la sinistra;",
             "- solo un bianco: la vedi di poppa;",
             "- due bianchi d'albero: l'allineamento",
             "  dà la rotta (il più alto sta a poppavia).",
             "Alla fonda un bianco tutt'intorno; in pesca",
             "verde o rosso sopra bianco; la manovra",
             "limitata rosso, bianco, rosso."]
    for k, t in enumerate(righe):
        p.append(testo(460, 120 + 20 * k, t, 12, INCHIOSTRO if k else SECONDARIO))
    svg("c9_fanali.svg", "schemi", L, H, "I settori dei fanali di navigazione e che cosa si vede da ogni lato", p)


def stadimetro():
    p = fondo("Lo stadimetro", "Dall'altezza d'albero H e dall'angolo α sotto cui la si vede: D = H / tan α. Un errore su H vale lo stesso errore su D.")
    x0, y0, x1 = 90, 330, 620
    p.append(linea([(40, y0), (720, y0)], SERIE[0], 1.5))
    p.append(testo(700, y0 + 18, "il mare", 11, SECONDARIO, "end"))
    p.append(f"<circle cx='{x0}' cy='{y0 - 2}' r='5' fill='{INCHIOSTRO}'/>")
    p.append(testo(x0, y0 + 30, "la testa del periscopio", 11, INCHIOSTRO, "middle"))
    p.append(linea([(x1, y0), (x1, y0 - 170)], INCHIOSTRO, 3))
    p.append(battello(x1 + 2, y0 - 6, 90, 120, 14))
    p.append(testo(x1 + 12, y0 - 90, "H: l'altezza d'albero", 12, INCHIOSTRO))
    p.append(testo(x1 + 12, y0 - 72, "(dal libretto)", 11, SECONDARIO))
    p.append(linea([(x0, y0 - 2), (x1, y0 - 170)], SERIE[1], 1.5, "6 4"))
    p.append(linea([(x0, y0 - 2), (x1, y0 - 2)], SERIE[1], 1.5, "6 4"))
    p.append(f"<path d='M{x0 + 80},{y0 - 2} A80,80 0 0 0 {x0 + 80 * math.cos(math.atan(168 / 530)):.1f},{y0 - 2 - 80 * math.sin(math.atan(168 / 530)):.1f}' fill='none' stroke='{SERIE[1]}'/>")
    p.append(testo(x0 + 90, y0 - 14, "α", 14, SERIE[1]))
    p.append(testo((x0 + x1) / 2, y0 + 30, "D = H / tan α", 14, INCHIOSTRO, "middle", True))
    p.append(testo(36, 400, "Il Type 2 sdoppia l'immagine; il reticolo dell'Alfa conta le divisioni. Errore 2-3% a 6×, di più a 1,5×.", 12, SECONDARIO))
    svg("c9_stadimetro.svg", "schemi", L, H, "La geometria dello stadimetro: la distanza dall'altezza d'albero e dall'angolo", p)


def due_gambe():
    p = fondo("La TMA a due gambe", "Con una sola gamba i rilevamenti vanno bene per infinite soluzioni; con due, una sola le spiega tutte.")
    s = 0.06   # px per metro
    o = (120, 380)
    # il battello: 6 minuti a 090, poi 6 minuti a 030, a 8 nodi; il bersaglio a 270 a 10 nodi
    v, vb, dt = 8 * 0.5144, 10 * 0.5144, 100.0
    nostro, bers = [], []
    x, y = 0.0, 0.0
    bx, by = 8000.0, 4500.0
    for k in range(13):
        rot = 90 if k < 6 else 30
        nostro.append((x, y))
        bers.append((bx, by))
        x += v * dt * math.sin(math.radians(rot)); y += v * dt * math.cos(math.radians(rot))
        bx -= vb * dt
    P = lambda q: (o[0] + q[0] * s, o[1] - q[1] * s)
    for k in range(0, 13, 2):
        p.append(linea([P(nostro[k]), P(bers[k])], TENUE, 1, "3 3"))
    p.append(linea([P(q) for q in nostro], SERIE[0], 3))
    p.append(linea([P(q) for q in bers], SERIE[1], 3))
    xa, ya = P(nostro[0]); xg, yg = P(nostro[6]); xf, yf = P(nostro[-1])
    p.append(testo(xa, ya + 18, "gamba 1: rotta 090", 11, INCHIOSTRO))
    p.append(testo(xg + 12, yg + 4, "accostata", 11, INCHIOSTRO))
    p.append(testo(xf + 8, yf, "gamba 2: rotta 030", 11, INCHIOSTRO))
    xb, yb = P(bers[0])
    p.append(testo(xb, yb - 10, "il bersaglio, rotta 270, 10 nodi", 11, INCHIOSTRO, "end"))
    p.append(testo(470, 380, "In grigio i rilevamenti ogni 3 minuti e mezzo.", 11, SECONDARIO))
    p.append(testo(470, 396, "Il rateo cambia con l'accostata: dalla differenza dei", 11, SECONDARIO))
    p.append(testo(470, 412, "ratei sulle due gambe viene la distanza (Ekelund).", 11, SECONDARIO))
    svg("c13_due_gambe.svg", "schemi", L, H, "La TMA a due gambe: il battello accosta e i rilevamenti fissano la soluzione", p)


def campo_mine():
    p = fondo("Il BQS-15 su un campo di mine", "Settore di ±60° dalla rotta, una spazzata ogni 8 s. A 5 nodi: 1.100 m per le mine ormeggiate, 450 per quelle sul fondo.")
    cx, cy, s = 110, 240, 0.18   # px per metro
    p.append(battello(cx, cy, 90, 46, 10))
    p.append(testo(cx, cy + 24, "il 688, 5 nodi", 11, SECONDARIO, "middle"))
    p.append(settore(cx + 20, cy, 1100 * s, 90 - 60, 90 + 60, SERIE[3], 0.15))
    p.append(settore(cx + 20, cy, 450 * s, 90 - 60, 90 + 60, SERIE[3], 0.25))
    p.append(testo(cx + 20 + 1100 * s * 0.55, cy - 1100 * s * 0.80, "ormeggiate: 1.100 m", 11, INCHIOSTRO))
    p.append(testo(cx + 20 + 450 * s * 0.6, cy - 450 * s * 0.85, "sul fondo: 450 m", 11, INCHIOSTRO))
    # le mine: una barriera ormeggiata e qualche mina sul fondo; quelle viste (nel settore e in portata) piene
    mine = [(500, -300, True), (600, -100, True), (700, 100, True), (800, 300, True), (1300, 0, True), (1250, 400, True),
            (350, 120, False), (300, -80, False), (650, 260, False), (900, -250, False)]
    for mx, my, orm in mine:
        x, y = cx + 20 + mx * s, cy + my * s
        dist, ril = math.hypot(mx, my), math.degrees(math.atan2(my, mx))
        vista = abs(ril) <= 60 and dist <= (1100 if orm else 450)
        if orm:
            p.append(f"<circle cx='{x:.1f}' cy='{y:.1f}' r='5' fill='{SERIE[1] if vista else SUPERFICIE}' stroke='{SERIE[1]}' stroke-width='1.5'/>")
        else:
            p.append(f"<rect x='{x - 4:.1f}' y='{y - 4:.1f}' width='8' height='8' fill='{SERIE[2] if vista else SUPERFICIE}' stroke='{SERIE[2]}' stroke-width='1.5'/>")
    voci = [(SERIE[1], "cerchio", "mina ormeggiata (piena: vista)"), (SERIE[2], "quadrato", "mina sul fondo (piena: vista)")]
    for k, (c, forma, t) in enumerate(voci):
        y = 395 + 18 * k
        if forma == "cerchio":
            p.append(f"<circle cx='{46}' cy='{y - 4}' r='5' fill='{c}'/>")
        else:
            p.append(f"<rect x='42' y='{y - 8}' width='8' height='8' fill='{c}'/>")
        p.append(testo(58, y, t, 11, INCHIOSTRO))
    p.append(testo(400, 395, "A 18 nodi la portata scende a 0,52 volte:", 11, SECONDARIO))
    p.append(testo(400, 411, "570 m per una CAPTOR, 230 m per una Mk 67.", 11, SECONDARIO))
    svg("d4_campo_mine.svg", "schemi", L, H, "Il settore del BQS-15 su un campo di mine ormeggiate e sul fondo, con le mine viste e quelle no", p)


def livelli_ia():
    p = fondo("I livelli dell'IA e chi comanda il timone", "A sinistra chi decide che cosa; a destra, in ordine, gli strati che correggono la rotta voluta a ogni passo.")
    livelli = [("Dottrina", "i numeri per marina, classe ed esperienza", "interazioni.toml, carico.toml"),
               ("Missione", "dove va, che cosa cerca, quando ingaggia", "gli script, la VM, la libreria"),
               ("Tattica", "allerta, ROE, rapporti, reazioni, TMA, armi", "il motore dell'Enhanced"),
               ("Manovra", "rotta, velocità, quota del passo", "manovra_ia, pilota, dinamica")]
    for k, (nome, cosa, dove) in enumerate(livelli):
        y = 74 + k * 86
        p.append(f"<rect x='36' y='{y}' width='300' height='70' rx='6' fill='{SUPERFICIE}' stroke='{SERIE[k % 4]}' stroke-width='2'/>")
        p.append(testo(50, y + 24, nome, 14, INCHIOSTRO, grassetto=True))
        p.append(testo(50, y + 44, cosa, 11, INCHIOSTRO))
        p.append(testo(50, y + 60, dove, 11, SECONDARIO))
        if k < 3:
            p.append(linea([(186, y + 70), (186, y + 86)], TENUE, 2))
    strati = ["lo script: meta, inseguimento, telegrafo", "gli ordini dopo i danni: ritirata, soccorso",
              "manovra_ia: difesa, reazione, gamba di TMA", "il pilota del fondale", "l'evasione del sottomarino",
              "la batteria del diesel", "il Crazy Ivan", "i vincoli del filo: 12 nodi, 2°/s",
              "la dinamica (B1) o il moto classico"]
    for k, t in enumerate(strati):
        y = 82 + k * 36
        p.append(f"<rect x='380' y='{y}' width='344' height='28' rx='4' fill='{GRIGLIA}' stroke='{ASSE}'/>")
        p.append(testo(392, y + 19, f"{k + 1}. {t}", 12, INCHIOSTRO))
    p.append(testo(552, 420, "ognuno può correggere quello di prima", 11, SECONDARIO, "middle"))
    svg("e6_livelli.svg", "schemi", L, H, "I quattro livelli dell'IA e i nove strati che decidono la rotta voluta, in ordine", p)


def dispersione():
    import random
    p = fondo("La dispersione del tiro", "Mk 75 di una Perry a 3 km: σ 4,5 m in direzione e 35 m in distanza (i primi tre colpi 6,4 e 49 m). 40 colpi.")
    cx, cy, sx, sy = 400, 250, 2.2, 3.0   # px per metro: la distanza in orizzontale, la direzione in verticale
    p.append(linea([(60, cy), (740, cy)], GRIGLIA, 1))
    p.append(linea([(cx, 70), (cx, 420)], GRIGLIA, 1))
    for k, (a, b) in enumerate(((35, 4.5), (70, 9.0))):   # le ellissi a 1 e 2 sigma
        p.append(f"<ellipse cx='{cx}' cy='{cy}' rx='{a * sx:.1f}' ry='{b * sy:.1f}' fill='none' stroke='{TENUE}' stroke-dasharray='4 3'/>")
        p.append(testo(cx + a * sx + 4, cy - 4, f"{k + 1}σ", 11, SECONDARIO))
    # il sottomarino in superficie di traverso al tiro: i 104 m di lunghezza stanno in direzione, i 10 di larghezza in distanza
    p.append(f"<rect x='{cx - 5 * sx:.1f}' y='{cy - 52 * sy:.1f}' width='{10 * sx:.1f}' height='{104 * sy:.1f}' fill='{SERIE[0]}' fill-opacity='0.15' stroke='{SERIE[0]}'/>")
    p.append(testo(cx + 8, 82, "sottomarino in superficie, di traverso (104 × 10 m)", 11, INCHIOSTRO))
    p.append(f"<circle cx='{cx}' cy='{cy}' r='4' fill='{INCHIOSTRO}'/>")
    p.append(testo(cx + 8, cy + 16, "la testa del periscopio", 11, INCHIOSTRO))
    rnd = random.Random(688)
    for k in range(40):
        f = 0.5 ** -0.5 if k < 3 else 1.0
        dx, dy = rnd.gauss(0, 35 * f), rnd.gauss(0, 4.5 * f)
        p.append(f"<circle cx='{cx + dx * sx:.1f}' cy='{cy + dy * sy:.1f}' r='3' fill='{SERIE[1]}' fill-opacity='0.8'/>")
    p.append(testo(60, cy - 8, "corto", 11, SECONDARIO))
    p.append(testo(740, cy - 8, "lungo", 11, SECONDARIO, "end"))
    p.append(testo(36, 425, "La caduta è radente (1,8°): metà dei colpi in acqua rimbalza. La prova: la testa colpita nel 6,7% dei colpi a 3 km.", 12, SECONDARIO))
    svg("e4_dispersione.svg", "schemi", L, H, "La dispersione di 40 colpi del Mk 75 a 3 km attorno a un sottomarino in superficie e alla testa di un periscopio", p)


def schemi_boe():
    p = fondo("Gli schemi di posa delle boe", "Datum al centro, rotta stimata 090 a 8 nodi, datum vecchio di 15 minuti, portata della boa 7,4 km (posa_schema).")
    file = pathlib.Path(__file__).resolve().parent / "schemi_boe.txt"
    nomi = {"barriera": "Barriera", "chevron": "Chevron", "cerchio": "Cerchio", "cuneo": "Cuneo", "trappola": "Trappola",
            "barriera_circolare": "Barriera circolare (URSS)"}
    for k, riga in enumerate(file.read_text().splitlines()):
        parti = riga.split()
        punti = [tuple(float(x) for x in q.split(",")) for q in parti[1:]]
        col, rig = k % 3, k // 3
        x0, y0, w, h = 36 + col * 232, 62 + rig * 186, 224, 176
        p.append(f"<rect x='{x0}' y='{y0}' width='{w}' height='{h}' fill='none' stroke='{GRIGLIA}'/>")
        p.append(testo(x0 + 8, y0 + 17, nomi[parti[0]], 12, INCHIOSTRO, grassetto=True))
        ampio = max(max(abs(a), abs(b)) for a, b in punti + [(1000.0, 1000.0)])
        sc = (min(w, h - 26) / 2 - 16) / ampio
        cx, cy = x0 + w / 2, y0 + 26 + (h - 26) / 2
        P = lambda q: (cx + q[0] * sc, cy - q[1] * sc)
        p.append(linea([P(q) for q in punti], TENUE, 1, "3 3"))
        for q in punti:
            x, y = P(q)
            p.append(f"<circle cx='{x:.1f}' cy='{y:.1f}' r='3.5' fill='{SERIE[0]}'/>")
        p.append(f"<path d='M{cx - 5},{cy} L{cx + 5},{cy} M{cx},{cy - 5} L{cx},{cy + 5}' stroke='{SERIE[1]}' stroke-width='2'/>")
        p.append(linea([(cx, cy), (cx + 26, cy)], SERIE[1], 1.5))
        p.append(f"<path d='M{cx + 26},{cy} l-6,-3 l0,6 Z' fill='{SERIE[1]}'/>")
        lato = 2 * ampio / 1000
        p.append(testo(x0 + w - 8, y0 + 17, f"{lato:.0f} km".replace(".", ","), 11, SECONDARIO, "end"))
    svg("e5_schemi_boe.svg", "schemi", L, H, "Sei schemi di posa delle boe, coi punti del gioco: barriera, chevron, cerchio, cuneo, trappola e barriera circolare", p)


def filo():
    p = fondo("Il filo del siluro", "Due rocchetti: il siluro svolge il suo col percorso fatto, il battello il suo col percorso fatto dopo il lancio.")
    xl, yl = 110, 340   # il punto di lancio
    # il siluro, fino a dove e' arrivato; il battello dopo il lancio, piu' lento e in un'altra direzione
    xs, ys = 640, 120
    xb, yb = 215, 230
    p.append(f"<circle cx='{xl}' cy='{yl}' r='5' fill='{INCHIOSTRO}'/>")
    p.append(testo(xl - 8, yl + 20, "il punto di lancio", 11, INCHIOSTRO))
    p.append(linea([(xl, yl), (xs, ys)], SERIE[0], 2))
    p.append(battello(xs + 4, ys - 2, 67, 16, 4, SERIE[0]))
    p.append(testo(xs - 6, ys - 16, "il siluro", 11, INCHIOSTRO, "end"))
    p.append(testo((xl + xs) / 2 + 14, (yl + ys) / 2 + 22, "rocchetto del siluro: 30 km, 18 minuti a 55 nodi", 12, SERIE[0]))
    p.append(linea([(xl, yl), (xb, yb)], SERIE[1], 2))
    p.append(battello(xb + 4, yb - 4, 44, 26, 6))
    p.append(testo(xb - 10, yb - 12, "il battello", 11, INCHIOSTRO, "end"))
    p.append(testo(xb - 62, yb + 40, "rocchetto del", 12, SERIE[1], "end"))
    p.append(testo(xb - 62, yb + 56, "battello: 3 km", 12, SERIE[1], "end"))
    righe = ["Il filo si rompe se:",
             "- il battello va oltre 12 nodi;",
             "- vira oltre 2° al secondo per più di 5 s;",
             "- un rocchetto finisce;",
             "- il battello sparisce.",
             "All'80% di un rocchetto il filo è «a rischio».",
             "Il rocchetto del battello dura 8 minuti a 12 nodi, 16 a 6:",
             "con il filo in acqua il limite vero è la velocità del battello."]
    for k, t in enumerate(righe):
        p.append(testo(350, 282 + 18 * k, t, 12, INCHIOSTRO if k else SECONDARIO))
    svg("d1_filo.svg", "schemi", L, H, "La geometria del filo: i due rocchetti dal punto di lancio e i limiti del battello", p)


def ekelund():
    p = fondo("Il regolo di Ekelund", "La striscia graduata alla velocità stimata, posata sull'ultima linea e ruotata finché le tacche cadono sulle linee.")
    s = 0.042   # px per metro
    ox, oy = 90, 380
    v, vb, dt = 8 * 0.5144, 10 * 0.5144, 240.0
    rb = math.radians(200.0)
    nostro = [(k * v * dt, 0.0) for k in range(4)]
    bers = [(6500.0 + k * vb * dt * math.sin(rb), 7000.0 + k * vb * dt * math.cos(rb)) for k in range(4)]
    P = lambda q: (ox + q[0] * s, oy - q[1] * s)
    for k in range(4):
        a, b = nostro[k], bers[k]
        d = math.hypot(b[0] - a[0], b[1] - a[1])
        oltre = (a[0] + (b[0] - a[0]) * 8700 / d, a[1] + (b[1] - a[1]) * 8700 / d)
        p.append(linea([P(a), P(oltre)], TENUE, 1, "4 3"))
    p.append(linea([P(q) for q in nostro], SERIE[0], 3))
    for k, q in enumerate(nostro):
        x, y = P(q)
        p.append(f"<circle cx='{x:.1f}' cy='{y:.1f}' r='3.5' fill='{SERIE[0]}'/>")
        p.append(testo(x, y + 18, f"O{k + 1}", 11, INCHIOSTRO, "middle"))
    # la striscia: una banda lungo la rotta del bersaglio, coi segni alla velocita' stimata
    a, b = P(bers[0]), P(bers[-1])
    ux, uy = b[0] - a[0], b[1] - a[1]
    n = math.hypot(ux, uy)
    ux, uy = ux / n, uy / n
    pre = (a[0] - 18 * ux, a[1] - 18 * uy)
    post = (b[0] + 18 * ux, b[1] + 18 * uy)
    p.append(linea([pre, post], ASSE, 12))
    for k, q in enumerate(bers):
        x, y = P(q)
        p.append(linea([(x - 8 * uy, y + 8 * ux), (x + 8 * uy, y - 8 * ux)], SERIE[1], 2.5))
        p.append(testo(x + 14, y - 8, f"tacca {k + 1}", 11, INCHIOSTRO))
    righe = ["Una tacca per linea, distanti V · (tₙ − tₖ).",
             "Frecce: la rotta. Su e giù: la distanza.",
             "[ e ]: la velocità. La riga in basso alla",
             "carta dà lo scarto: più è piccolo, più la",
             "soluzione spiega i rilevamenti.",
             "Invio porta la soluzione nel MATE."]
    for k, t in enumerate(righe):
        p.append(testo(470, 310 + 18 * k, t, 12, INCHIOSTRO))
    p.append(testo(ox + 150, oy - 8, "la rotta propria", 11, SECONDARIO))
    svg("c13_ekelund.svg", "schemi", L, H, "Il regolo di Ekelund: una striscia con una tacca per linea di rilevamento lungo la rotta stimata del bersaglio", p)


def main():
    fasi_siluro()
    schiera_specchio()
    fanali()
    stadimetro()
    due_gambe()
    campo_mine()
    livelli_ia()
    dispersione()
    schemi_boe()
    filo()
    ekelund()


if __name__ == "__main__":
    main()
