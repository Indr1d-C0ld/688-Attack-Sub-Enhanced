"""Font bitmap delle stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md §9.4), disegnati da zero per il
remake (licenza del progetto, nessuna fonte esterna): data/motore/font/*.fnt nel formato FNT2 (engine/grafica/font.hpp).

Un solo disegno di riferimento a tratto, 5x7, solo maiuscole (MASTER: tratti di un pixel, segmenti dritti e angoli a
45 gradi, alla Hershey). Da qui:
- «Fosforo 688» 5x7 (cella 6x9) e 7x9 (cella 8x12): terminale monospaziato, tratto (2) e alone (1) per la rampa del fosforo;
- «Incisione 688» alta 7 e 5: incisioni alla Gorton (MIL-M-18012), proporzionale, estremita' dei tratti a mezzo tono;
- «Stampino URSS» alto 10 e 7: latino nelle proporzioni del tipo Б del GOST 2.304 (E, F, C, S 5 px; normali 6; A, M, X
  7; W 8), con le varianti inclinata (75 gradi) e a ponticelli dello stampino;
- «Cifre»: 7 segmenti 5x9 e 6x11, coi segmenti spenti a mezzo tono (i LED rossi del 688).
Il tracciato 5x7 si porta alle altre misure come grafo di punti: ogni coppia di pixel vicini e' un tratto, riscalato
e ridisegnato con Bresenham.

Livelli dei pixel: 1 mezzo tono, 2 tratto, 3 nucleo. FNT2: "FNT2", u16 glifi, u8 righe, u8 spazio; per glifo u32
codepoint, u8 larghezza, righe da u32 (2 bit per colonna, colonna 0 nei bit alti).
Uso: font_stazioni.py <radice>   (lo chiama esporta_motore.py)"""
import os, struct, sys

# --- disegno di riferimento 5x7 (righe dall'alto; '#' tratto) ---
MASTER = {
    'A': ['.###.', '#...#', '#...#', '#####', '#...#', '#...#', '#...#'],
    'B': ['####.', '#...#', '#...#', '####.', '#...#', '#...#', '####.'],
    'C': ['.###.', '#...#', '#....', '#....', '#....', '#...#', '.###.'],
    'D': ['####.', '#...#', '#...#', '#...#', '#...#', '#...#', '####.'],
    'E': ['#####', '#....', '#....', '####.', '#....', '#....', '#####'],
    'F': ['#####', '#....', '#....', '####.', '#....', '#....', '#....'],
    'G': ['.###.', '#...#', '#....', '#.###', '#...#', '#...#', '.###.'],
    'H': ['#...#', '#...#', '#...#', '#####', '#...#', '#...#', '#...#'],
    'I': ['###', '.#.', '.#.', '.#.', '.#.', '.#.', '###'],
    'J': ['..###', '...#.', '...#.', '...#.', '...#.', '#..#.', '.##..'],
    'K': ['#...#', '#..#.', '#.#..', '##...', '#.#..', '#..#.', '#...#'],
    'L': ['#....', '#....', '#....', '#....', '#....', '#....', '#####'],
    'M': ['#...#', '##.##', '#.#.#', '#.#.#', '#...#', '#...#', '#...#'],
    'N': ['#...#', '#...#', '##..#', '#.#.#', '#..##', '#...#', '#...#'],
    'O': ['.###.', '#...#', '#...#', '#...#', '#...#', '#...#', '.###.'],
    'P': ['####.', '#...#', '#...#', '####.', '#....', '#....', '#....'],
    'Q': ['.###.', '#...#', '#...#', '#...#', '#.#.#', '#..#.', '.##.#'],
    'R': ['####.', '#...#', '#...#', '####.', '#.#..', '#..#.', '#...#'],
    'S': ['.###.', '#...#', '#....', '.###.', '....#', '#...#', '.###.'],
    'T': ['#####', '..#..', '..#..', '..#..', '..#..', '..#..', '..#..'],
    'U': ['#...#', '#...#', '#...#', '#...#', '#...#', '#...#', '.###.'],
    'V': ['#...#', '#...#', '#...#', '#...#', '#...#', '.#.#.', '..#..'],
    'W': ['#...#', '#...#', '#...#', '#.#.#', '#.#.#', '#.#.#', '.#.#.'],
    'X': ['#...#', '#...#', '.#.#.', '..#..', '.#.#.', '#...#', '#...#'],
    'Y': ['#...#', '#...#', '.#.#.', '..#..', '..#..', '..#..', '..#..'],
    'Z': ['#####', '....#', '...#.', '..#..', '.#...', '#....', '#####'],
    '0': ['.###.', '#...#', '#..##', '#.#.#', '##..#', '#...#', '.###.'],   # zero barrato
    '1': ['.#.', '##.', '.#.', '.#.', '.#.', '.#.', '###'],
    '2': ['.###.', '#...#', '....#', '...#.', '..#..', '.#...', '#####'],
    '3': ['#####', '...#.', '..#..', '...#.', '....#', '#...#', '.###.'],
    '4': ['...#.', '..##.', '.#.#.', '#..#.', '#####', '...#.', '...#.'],
    '5': ['#####', '#....', '####.', '....#', '....#', '#...#', '.###.'],
    '6': ['..##.', '.#...', '#....', '####.', '#...#', '#...#', '.###.'],
    '7': ['#####', '....#', '...#.', '..#..', '.#...', '.#...', '.#...'],
    '8': ['.###.', '#...#', '#...#', '.###.', '#...#', '#...#', '.###.'],
    '9': ['.###.', '#...#', '#...#', '.####', '....#', '...#.', '.##..'],
    ' ': ['...', '...', '...', '...', '...', '...', '...'],
    '.': ['.', '.', '.', '.', '.', '.', '#'],
    ',': ['..', '..', '..', '..', '..', '.#', '#.'],
    ':': ['.', '.', '#', '.', '.', '#', '.'],
    ';': ['..', '..', '.#', '..', '..', '.#', '#.'],
    '-': ['....', '....', '....', '####', '....', '....', '....'],
    '+': ['.....', '..#..', '..#..', '#####', '..#..', '..#..', '.....'],
    '=': ['....', '....', '####', '....', '####', '....', '....'],
    '/': ['....#', '....#', '...#.', '..#..', '.#...', '#....', '#....'],
    '(': ['..#', '.#.', '#..', '#..', '#..', '.#.', '..#'],
    ')': ['#..', '.#.', '..#', '..#', '..#', '.#.', '#..'],
    '[': ['###', '#..', '#..', '#..', '#..', '#..', '###'],
    ']': ['###', '..#', '..#', '..#', '..#', '..#', '###'],
    "'": ['#', '#', '.', '.', '.', '.', '.'],
    '"': ['#.#', '#.#', '...', '...', '...', '...', '...'],
    '!': ['#', '#', '#', '#', '#', '.', '#'],
    '?': ['.###.', '#...#', '....#', '...#.', '..#..', '.....', '..#..'],
    '%': ['##..#', '##..#', '...#.', '..#..', '.#...', '#..##', '#..##'],
    '#': ['.#.#.', '.#.#.', '#####', '.#.#.', '#####', '.#.#.', '.#.#.'],
    '*': ['.....', '#.#.#', '.###.', '#####', '.###.', '#.#.#', '.....'],
    '<': ['...#', '..#.', '.#..', '#...', '.#..', '..#.', '...#'],
    '>': ['#...', '.#..', '..#.', '...#', '..#.', '.#..', '#...'],
    '_': ['....', '....', '....', '....', '....', '....', '####'],
    '°': ['.#.', '#.#', '.#.', '...', '...', '...', '...'],
    '×': ['.....', '#...#', '.#.#.', '..#..', '.#.#.', '#...#', '.....'],
    '§': ['.##.', '#...', '.##.', '#..#', '.##.', '...#', '.##.'],
    '&': ['.##..', '#..#.', '#.#..', '.#...', '#.#.#', '#..#.', '.##.#'],
    '@': ['.###.', '#...#', '#.###', '#.#.#', '#.###', '#....', '.###.'],
    '|': ['#', '#', '#', '#', '#', '#', '#'],
}
# accentate maiuscole: la lettera e l'accento nella riga sopra (le celle hanno due righe libere in alto)
ACCENTI = {'À': ('A', 'grave'), 'È': ('E', 'grave'), 'É': ('E', 'acuto'), 'Ì': ('I', 'grave'), 'Í': ('I', 'acuto'),
           'Ò': ('O', 'grave'), 'Ó': ('O', 'acuto'), 'Ù': ('U', 'grave'), 'Ú': ('U', 'acuto'), 'Á': ('A', 'acuto')}

# larghezze del tipo Б (GOST 2.304) applicate al latino, per altezza 10 (h/10 = 1 px di tratto)
LARGHEZZE_GOST = {**{c: 6 for c in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'},
                  **{c: 5 for c in 'EFCSLJ'}, **{c: 7 for c in 'AMXQ'}, 'W': 8, 'I': 3, '1': 4}
PONTICELLI = set('OABDPR0468Q9')   # lettere con l'occhiello chiuso: lo stampino lo apre con un ponticello


def punti(c):
    """Il tracciato di riferimento come insieme di pixel (x, y)."""
    return {(x, y) for y, r in enumerate(MASTER[c]) for x, ch in enumerate(r) if ch == '#'}


def tratti(p):
    """Coppie di pixel vicini (8-connessi): i tratti del disegno. Le diagonali solo dove manca l'angolo retto."""
    s = set()
    for (x, y) in p:
        for dx, dy in ((1, 0), (0, 1), (1, 1), (-1, 1)):
            q = (x + dx, y + dy)
            if q not in p: continue
            if dx and dy and ((x + dx, y) in p or (x, y + dy) in p): continue   # angolo retto: niente scorciatoia
            s.add(((x, y), q))
    return s


def linea(a, b):
    (x0, y0), (x1, y1) = a, b
    dx, dy, sx, sy = abs(x1 - x0), -abs(y1 - y0), 1 if x0 < x1 else -1, 1 if y0 < y1 else -1
    e, out = dx + dy, []
    while True:
        out.append((x0, y0))
        if (x0, y0) == (x1, y1): return out
        e2 = 2 * e
        if e2 >= dy: e += dy; x0 += sx
        if e2 <= dx: e += dx; y0 += sy


def riscala(c, w, h):
    """Il tracciato 5x7 (o piu' stretto) portato a w x h: punti isolati e tratti riscalati e ridisegnati."""
    p = punti(c)
    w0 = len(MASTER[c][0])
    fx = (w - 1) / max(w0 - 1, 1)
    fy = (h - 1) / 6
    m = lambda q: (round(q[0] * fx), round(q[1] * fy))
    out = {m(q) for q in p}
    for a, b in tratti(p): out.update(linea(m(a), m(b)))
    return out


def accento(pix, w, tipo, alto):
    """Accento di due pixel sopra la lettera, nelle righe alto-2 e alto-1 (coordinate della cella)."""
    c = (w - 1) // 2
    if tipo == 'grave': return pix | {(c - 1, alto - 2), (c, alto - 1)}
    return pix | {(c + 1, alto - 2), (c, alto - 1)}


def glifo(pix, w, h, alone=False, estremita=False):
    """Righe FNT2 (u32): tratto 2; con alone i vicini vuoti a 1 (fosforo); con estremita' i capi dei tratti a 1."""
    lv = {}
    for q in pix: lv[q] = 2
    if estremita:   # capi dei tratti (un solo vicino): l'ultimo pixel a mezzo tono, come lascia la fresa
        for (x, y) in pix:
            n = sum((x + dx, y + dy) in pix for dx in (-1, 0, 1) for dy in (-1, 0, 1) if dx or dy)
            if n == 1: lv[(x, y)] = 1
    if alone:
        for (x, y) in list(pix):
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                q = (x + dx, y + dy)
                if q not in lv and 0 <= q[0] < 16 and 0 <= q[1] < h: lv[q] = 1
    righe = []
    for y in range(h):
        v = 0
        for x in range(16):
            v |= lv.get((x, y), 0) << (30 - 2 * x)
        righe.append(v)
    return righe


def scrivi_fnt(percorso, glifi, righe, spazio):
    """glifi: {codepoint: (larghezza, [righe u32])}"""
    if ord('?') not in glifi: raise ValueError(percorso + ': manca il ?')
    with open(percorso, 'wb') as o:
        o.write(b'FNT2' + struct.pack('<HBB', len(glifi), righe, spazio))
        for cp in sorted(glifi):
            w, r = glifi[cp]
            assert len(r) == righe and 0 < w <= 16, (percorso, chr(cp))
            o.write(struct.pack('<IB', cp, w) + b''.join(struct.pack('<I', x) for x in r))


def famiglia(w_di, h, alto, cella_h, alone=False, estremita=False, monospazio=None, trasforma=None):
    """Tutti i caratteri: w_di(c) larghezza del glifo, h altezza delle maiuscole, alto riga della cima nella cella."""
    out = {}
    for c in list(MASTER) + list(ACCENTI):
        base, acc = ACCENTI.get(c, (c, None))
        w = w_di(base)
        pix = {(x, y + alto) for x, y in riscala(base, w, h)}
        if acc: pix = accento(pix, w, acc, alto)
        if trasforma: pix, w = trasforma(base, pix, w)
        if base == ' ': pix = set()
        if monospazio and w < monospazio:   # terminale: i glifi stretti al centro della cella
            pix = {(x + (monospazio - w) // 2, y) for x, y in pix}
        larg = monospazio or (w if trasforma else max(x for x, _ in pix) + 1 if pix else w)   # stampino: le larghezze del GOST
        out[ord(c)] = (larg, glifo(pix, larg, cella_h, alone, estremita))
    return out


def larghezza_naturale(c, f=1.0):
    return max(1, round((len(MASTER[c][0]) - 1) * f) + 1)


def stampino(h, ponti=False, inclinato=False):
    def w_di(c):
        w = LARGHEZZE_GOST.get(c, len(MASTER[c][0]) + 1)
        w = max(1, round(w * h / 10)) if h != 10 else w
        return max(w, 3) if c in 'I1' else w   # le grazie della I e del 1 vogliono tre colonne
    def tr(c, pix, w):
        if ponti and c in PONTICELLI:   # il ponticello: un pixel tolto in cima e in fondo al centro dell'occhiello
            ys = [y for _, y in pix]
            cx = w // 2
            for y in (min(ys), max(ys)):
                pix = pix - {(cx, y)} if (cx - 1, y) in pix and (cx + 1, y) in pix else pix
        if inclinato:   # 75 gradi: un pixel verso destra ogni 4 righe dal basso
            base = max(y for _, y in pix) if pix else 0
            pix = {(x + (base - y) // 4, y) for x, y in pix}   # l'avanzamento resta quello dritto: sporge come un corsivo
        return pix, w
    return w_di, tr


def sette_segmenti(w, h):
    """Cifre a 7 segmenti w x h: segmenti accesi a 2, spenti a 1 (LED rossi coi segmenti spenti visibili)."""
    m = h // 2
    seg = {'a': [(x, 0) for x in range(1, w - 1)], 'd': [(x, h - 1) for x in range(1, w - 1)],
           'g': [(x, m) for x in range(1, w - 1)],
           'f': [(0, y) for y in range(1, m)], 'b': [(w - 1, y) for y in range(1, m)],
           'e': [(0, y) for y in range(m + 1, h - 1)], 'c': [(w - 1, y) for y in range(m + 1, h - 1)]}
    cifre = {'0': 'abcdef', '1': 'bc', '2': 'abged', '3': 'abgcd', '4': 'fgbc', '5': 'afgcd', '6': 'afgedc', '7': 'abc',
             '8': 'abcdefg', '9': 'abcdfg', '-': 'g', ' ': '', '?': 'abge', 'E': 'adefg', 'r': 'eg', 'o': 'cdeg'}
    out = {}
    for c, acc in cifre.items():
        lv = {}
        for s, px in seg.items():
            for q in px: lv[q] = 2 if s in acc else 1
        righe = []
        for y in range(h):
            v = 0
            for x in range(w): v |= lv.get((x, y), 0) << (30 - 2 * x)
            righe.append(v)
        out[ord(c)] = (w, righe)
    out[ord('.')] = (1, [(2 << 30) if y == h - 1 else 0 for y in range(h)])
    out[ord(':')] = (1, [(2 << 30) if y in (h // 3, 2 * h // 3) else 0 for y in range(h)])
    return out


def esporta_font_stazioni(radice):
    d = os.path.join(radice, 'data', 'motore', 'font')
    os.makedirs(d, exist_ok=True)
    fatti = {}
    def metti(nome, glifi, righe, spazio):
        scrivi_fnt(os.path.join(d, nome + '.fnt'), glifi, righe, spazio)
        fatti[nome] = len(glifi)
    # Fosforo 688: terminale monospaziato; 5x7 in cella 6x9 (2 righe sopra per gli accenti), 7x9 in cella 8x12
    metti('fosforo688_5x7', famiglia(lambda c: 5 if len(MASTER[c][0]) == 5 else len(MASTER[c][0]), 7, 2, 9, alone=True,
                                     monospazio=5), 9, 1)
    metti('fosforo688_7x9', famiglia(lambda c: 7 if len(MASTER[c][0]) == 5 else larghezza_naturale(c, 1.5), 9, 3, 12,
                                     alone=True, monospazio=7), 12, 1)
    # Incisione 688 (Gorton): proporzionale, larghezza 0,6-0,7 h; M e W piu' larghe; estremita' ammorbidite
    metti('incisione688_7', famiglia(lambda c: len(MASTER[c][0]), 7, 2, 9, estremita=True), 9, 1)
    metti('incisione688_5', famiglia(lambda c: max(1, round((len(MASTER[c][0]) - 1) * 0.75) + 1), 5, 2, 7, estremita=True), 7, 1)
    # Stampino URSS: GOST tipo Б, alto 10 (riga ogni 17 nella pagina) e 7 per i pannelli bassi; varianti
    for h, cella, alto in ((10, 13, 3), (7, 10, 3)):
        for var, kw in (('', {}), ('_ponti', {'ponti': True}), ('_inclinato', {'inclinato': True})):
            w_di, tr = stampino(h, **kw)
            metti(f'stampino_urss_{h}{var}', famiglia(w_di, h, alto, cella, trasforma=tr), cella, 2)
    # Cifre a 7 segmenti
    metti('cifre_5x9', sette_segmenti(5, 9), 9, 1)
    metti('cifre_6x11', sette_segmenti(6, 11), 11, 2)
    return fatti


if __name__ == '__main__':
    print(esporta_font_stazioni(sys.argv[1] if len(sys.argv) > 1 else '.'))
