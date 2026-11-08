"""Le musiche FM del remake (roadmap 38, B) -> data/motore/musica/fm/<id>.mid.
Brani brevi in stile AdLib/Sound Blaster per le schermate fuori dalla pattuglia (in mare niente musica, come
nell'originale). Li suona nucleo/musica_fm coi timbri di data/motore/timbri_fm.toml; le stesse partiture valgono per
l'MT-32 dell'utente (tools/mt32, facoltativo). Composte per il progetto (CC0): nessun tema d'epoca copiato; il classico
tiene la musica dei titoli di Rob Hubbard.

Una partitura e' un elenco di voci (canale MIDI, programma GM, volume, note); una nota e' (battuta in quarti, durata in
quarti, altezza, velocita'). I brani in loop hanno i marcatori «loop_inizio» e «loop_fine».
Uso: musiche_fm.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, struct, sys

DIVISIONE = 96   # tick per quarto

NOMI = {'C': 0, 'D': 2, 'E': 4, 'F': 5, 'G': 7, 'A': 9, 'B': 11}


def n(s):
    """'C4', 'F#3', 'Bb2' -> numero MIDI (C4 = 60)."""
    base = NOMI[s[0]]
    i = 1
    while i < len(s) and s[i] in '#b':
        base += 1 if s[i] == '#' else -1
        i += 1
    return base + 12 * (int(s[i:]) + 1)


def melodia(testo, inizio=0.0, vel=96):
    """'D5:1 F5:0.5 -:0.5 A5:2' (- = pausa) -> note, dal quarto `inizio`."""
    out, t = [], inizio
    for pezzo in testo.split():
        nota, dur = pezzo.split(':')
        d = float(dur)
        if nota != '-':
            out.append((t, d * 0.95, n(nota), vel))
        t += d
    return out


def accordi(sequenza, quarti, inizio=0.0, vel=70, legato=0.98):
    """['D3 F3 A3', ...] un accordo ogni `quarti` quarti."""
    out = []
    for k, a in enumerate(sequenza):
        for x in a.split():
            out.append((inizio + k * quarti, quarti * legato, n(x), vel))
    return out


def arpeggio(sequenza, quarti, passo, inizio=0.0, vel=64):
    """Ogni accordo di `quarti` quarti suonato a note di `passo` quarti, su e giu'."""
    out = []
    for k, a in enumerate(sequenza):
        note = [n(x) for x in a.split()]
        giro = note + note[-2:0:-1]
        t, i = inizio + k * quarti, 0
        while t < inizio + (k + 1) * quarti - 1e-9:
            out.append((t, passo * 0.9, giro[i % len(giro)], vel))
            t += passo
            i += 1
    return out


def bassi(sequenza, quarti, schema, inizio=0.0, vel=90):
    """Un basso per accordo: `schema` = [(quarto nella battuta, durata, intervallo in semitoni dalla fondamentale)]."""
    out = []
    for k, a in enumerate(sequenza):
        fond = n(a.split()[0])
        for q, d, intervallo in schema:
            if q < quarti:
                out.append((inizio + k * quarti + q, d, fond + intervallo, vel))
    return out


def ripeti(note, volte, periodo):
    return [(t + k * periodo, d, h, v) for k in range(volte) for (t, d, h, v) in note]


def V(canale, programma, note, volume=100):
    return dict(canale=canale, programma=programma, note=note, volume=volume)


def brano(bpm, voci, loop=None, coda_quarti=4.0):
    return dict(bpm=bpm, voci=voci, loop=loop, coda=coda_quarti)


# --- le partiture ---

# 1. Menu dell'Enhanced: re minore, notturno e in moto; il tema sul flauto e poi sugli ottoni
GIRO_MENU = ['D3 F3 A3', 'Bb2 D3 F3', 'F2 A2 C3', 'C3 E3 G3'] * 2 + ['G2 Bb2 D3', 'D3 F3 A3', 'Bb2 D3 F3', 'A2 C#3 E3']
TEMA_MENU = ('A4:1.5 D5:0.5 F5:1 E5:1 D5:2 C5:1 A4:1 Bb4:1.5 C5:0.5 D5:1 F5:1 E5:3 -:1 '
             'A4:1.5 D5:0.5 F5:1 G5:1 A5:2 G5:1 F5:1 E5:1.5 D5:0.5 C5:1 E5:1 D5:4 '
             'G4:1 Bb4:1 D5:1 G5:1 F5:2 E5:1 D5:1 D5:1 F5:1 A5:2 G5:1 F5:1 E5:2 C#5:2')
MENU = brano(92, [
    V(0, 89, accordi([' '.join(x[:-1] + '4' for x in a.split()) for a in GIRO_MENU], 4, vel=58), 90),      # pad freddo
    V(1, 38, bassi(GIRO_MENU, 4, [(0, 1.4, -12), (1.5, 0.4, -12), (2, 1.4, -5), (3.5, 0.4, -12)]), 105),   # basso sintetico
    V(2, 73, melodia(TEMA_MENU, 0, 92), 100),                                                               # flauto
    V(3, 61, melodia(TEMA_MENU, 48, 84), 70),                                                               # ottoni dalla meta'
    V(9, -1, ripeti([(0, 0.2, 36, 100), (2, 0.2, 38, 80), (2.5, 0.2, 36, 70), (3.5, 0.1, 42, 50)], 12, 4), 85),
], loop=(0, 48))

# 2. Briefing: do minore, ostinato degli archi gravi, timpani radi, un corno lontano. Attesa, non battaglia
GIRO_BRIEF = ['C3 Eb3 G3', 'C3 Eb3 G3', 'Ab2 C3 Eb3', 'G2 B2 D3'] * 2
OSTINATO = [(q * 0.5, 0.45, h, 72) for q, h in enumerate([48, 51, 55, 51] * 2)]
BRIEFING = brano(76, [
    V(0, 48, ripeti(OSTINATO, 2, 4) + [(t + 8, d, h - (4 if 4 <= t % 16 < 8 else 0), v) for t, d, h, v in ripeti(OSTINATO, 2, 4)]
      + [(t + 16, d, h, v) for t, d, h, v in ripeti(OSTINATO, 4, 4)], 95),
    V(1, 43, bassi(GIRO_BRIEF, 4, [(0, 3.9, -12)], vel=85), 100),
    V(2, 60, melodia('-:8 G4:2 Ab4:1 G4:1 F4:2 Eb4:2 -:4 D4:2 Eb4:1 F4:1 G4:4', 0, 70), 80),
    V(3, 47, [(t, 0.5, 36, 90) for t in (0, 6, 8, 14, 16, 22, 24, 28, 30)], 90),
    V(4, 95, accordi([' '.join(x[:-1] + '4' for x in a.split()) for a in GIRO_BRIEF], 4, vel=46), 70),
], loop=(0, 32))

# 3. Debriefing riuscito: la risoluzione in maggiore, un corale d'ottoni; senza loop
DEB_OK = brano(72, [
    V(0, 61, accordi(['Bb3 D4 F4', 'Eb3 G3 Bb3', 'C4 Eb4 G4', 'F3 A3 C4', 'Bb3 D4 F4', 'G3 Bb3 D4', 'Eb3 G3 C4', 'F3 A3 C4',
                      'Bb3 D4 F4'], 2, vel=80), 95),
    V(1, 43, bassi(['Bb2', 'Eb2', 'C3', 'F2', 'Bb2', 'G2', 'Eb2', 'F2', 'Bb1'], 2, [(0, 1.9, 0)], vel=88), 95),
    V(2, 56, melodia('F5:2 G5:2 G5:1 F5:1 F5:2 -:2 D5:1 F5:1 G5:2 Eb5:2 F5:2 D5:4', 0, 86), 90),
    V(3, 47, [(0, 0.6, 34, 95), (8, 0.6, 34, 85), (16, 0.9, 34, 100)], 85),
])

# 4. Debriefing fallito: re minore, archi gravi e il corno; senza loop
DEB_KO = brano(58, [
    V(0, 48, accordi(['D3 F3 A3', 'Bb2 D3 G3', 'G2 Bb2 D3', 'A2 C#3 E3', 'D3 F3 A3', 'Bb2 D3 F3', 'G2 C3 E3', 'D3 F3 A3'], 2,
                     vel=72), 95),
    V(1, 43, bassi(['D2', 'G1', 'G2', 'A1', 'D2', 'Bb1', 'C2', 'D2'], 2, [(0, 1.95, 0)], vel=84), 95),
    V(2, 60, melodia('A4:2 G4:1 F4:1 E4:2 -:2 D4:1 F4:1 E4:1 D4:1 C#4:2 D4:6', 0, 70), 85),
])

# 5. Cerimonia e promozione: una fanfara breve e sobria in si bemolle; senza loop
CERIMONIA = brano(84, [
    V(0, 56, melodia('F4:0.5 F4:0.5 Bb4:1.5 D5:0.5 F5:2 D5:1 Bb4:1 C5:1.5 D5:0.5 Eb5:1 C5:1 Bb4:4', 0, 96), 100),
    V(1, 60, accordi(['Bb3 D4', 'Bb3 D4', 'Eb3 G3', 'F3 A3', 'Bb3 D4'], 2.5, vel=78) , 85),
    V(2, 61, accordi(['Bb2 F3', 'Bb2 F3', 'Eb2 Bb2', 'F2 C3', 'Bb2 F3'], 2.5, vel=78), 85),
    V(3, 47, [(0, 0.3, 34, 100), (0.5, 0.3, 34, 80), (1, 0.6, 34, 100), (7.5, 0.3, 29, 90), (8, 0.3, 34, 100), (12, 1.0, 34, 110)], 90),
    V(9, -1, [(0, 0.2, 38, 70), (0.5, 0.2, 38, 70), (1, 0.2, 38, 90), (12, 0.6, 49, 90)], 80),
])

# 6. Albo e statistiche: un loop calmo, pad caldo e celesta in arpeggio, fa maggiore
GIRO_ALBO = ['F3 A3 C4', 'D3 F3 A3', 'Bb2 D3 F3', 'C3 E3 G3']
ALBO = brano(70, [
    V(0, 88, accordi(GIRO_ALBO * 2, 4, vel=56), 85),
    V(1, 8, arpeggio([' '.join(x[:-1] + str(int(x[-1]) + 2) for x in a.split()) for a in GIRO_ALBO * 2], 4, 0.5, vel=58), 75),
    V(2, 33, bassi(GIRO_ALBO * 2, 4, [(0, 1.8, -12), (2, 1.8, -5)], vel=78), 90),
    V(3, 73, melodia('-:16 C5:2 A4:1 F4:1 G4:2 A4:2 F4:3 -:1 D4:1 F4:1 A4:2 G4:4', 0, 70), 75),
], loop=(0, 32))

# 7. Museo: ambiente, pad scuro e vibrafono radi in mi minore, lento
MUSEO = brano(54, [
    V(0, 95, accordi(['E2 B2 G3', 'C3 G3 E4', 'A2 E3 C4', 'B2 F#3 D4'], 8, vel=54), 85),
    V(1, 11, melodia('B4:3 -:1 G4:2 E4:2 -:4 C5:3 -:1 B4:2 A4:2 -:4 E5:2 D5:2 B4:4 -:2 A4:2 F#4:6 -:2', 0, 64), 75),
    V(2, 89, accordi(['B3 E4', 'G3 C4', 'E3 A3', 'F#3 B3'], 8, vel=40), 60),
], loop=(0, 32))

# 8. Editor: un loop leggero, basso elettrico e onda quadra, la maggiore, 108 bpm
GIRO_ED = ['A2 C#3 E3', 'F#2 A2 C#3', 'D2 F#2 A2', 'E2 G#2 B2']
EDITOR = brano(108, [
    V(0, 33, bassi(GIRO_ED * 4, 4, [(0, 0.45, 0), (0.5, 0.45, 12), (1.5, 0.45, 0), (2, 0.45, 7), (3, 0.45, 12), (3.5, 0.45, 7)],
                   vel=88), 95),
    V(1, 80, melodia('E5:0.5 C#5:0.5 A4:1 B4:0.5 C#5:0.5 E5:1 F#5:0.5 E5:0.5 C#5:1 A4:2 '
                     'D5:0.5 C#5:0.5 B4:1 A4:0.5 B4:0.5 C#5:1 B4:3 -:1', 16, 80), 75),
    V(2, 11, arpeggio([' '.join(x[:-1] + str(int(x[-1]) + 2) for x in a.split()) for a in GIRO_ED * 4], 4, 0.5, vel=50), 65),
    V(9, -1, ripeti([(0, 0.2, 36, 90), (1, 0.1, 42, 50), (2, 0.2, 39, 70), (3, 0.1, 42, 50), (3.5, 0.1, 42, 40)], 16, 4), 75),
], loop=(0, 64))

# 9. Il video d'apertura (docs/29_video_apertura.md): «Lo stesso mare», re minore, 72 bpm, 31 battute sul montaggio. Il
# motivo e il suo specchio (gli stessi intervalli capovolti attorno al re) passano da un battello all'altro. Il silenzio, la
# traccia, la caccia, il culmine; alla battuta 29 tutto si ferma di colpo, e dal nero esce il titolo sul motivo solo.
def _q(b):   # il quarto d'inizio della battuta b (da 1)
    return (b - 1) * 4.0


def _sale(sequenza, battuta, velocita):   # una battuta a accordo, ciascuna piu' forte: il culmine cresce fino al nero
    return [x for k, (a, v) in enumerate(zip(sequenza, velocita)) for x in accordi([a], 4, _q(battuta + k), vel=v)]


MOTIVO, SPECCHIO = 'D4:1.5 F4:0.5 E4:1 A3:1 -:4', 'D4:1.5 B3:0.5 C4:1 G4:1 -:4'
GIRO_CACCIA = ['D2', 'D2', 'Bb1', 'C2', 'D2', 'F2', 'Bb1', 'A1']
APERTURA = brano(72, [
    V(0, 95, accordi(['D2 A2', 'D2 A2', 'Bb1 F2'], 8, _q(3), vel=46) +                       # il fondo: quinte vuote
             accordi(['D2 A2 F3', 'D2 A2 F3', 'Bb1 F2 D3', 'A1 E2 C#3'], 8, _q(9), vel=52) +
             accordi(['D2 A2'], 8, _q(30), vel=40), 90),
    V(1, 14, [(_q(5), 3.0, n('A5'), 44), (_q(7) + 2.0, 3.0, n('D6'), 40), (_q(31), 4.0, n('D6'), 52)], 70),   # una campana lontana
    V(2, 73, melodia(MOTIVO, _q(9), 70) + melodia(MOTIVO, _q(30), 76), 95),                 # il 688: il motivo, e il titolo
    V(3, 71, melodia(SPECCHIO, _q(11), 68), 95),                                            # l'Alfa: lo specchio
    V(4, 60, melodia(MOTIVO.replace('4:', '3:').replace('A3', 'A2'), _q(13), 68) +           # il motivo scende ai corni
             melodia(MOTIVO, _q(21), 86) + melodia(MOTIVO, _q(23), 92), 95),
    V(5, 52, melodia(SPECCHIO, _q(15), 62) + melodia(SPECCHIO, _q(22), 84) +                 # lo specchio al coro, poi in canone
             _sale(['D4 A4', 'Eb4 Bb4', 'E4 B4', 'F4 C5'], 25, [84, 88, 92, 96]), 90),
    V(6, 38, [(_q(17) + k * 0.5, 0.42, n(GIRO_CACCIA[k // 8]), 84 + k // 4) for k in range(64)] +   # la caccia: il battito
             [(_q(25) + k * 0.25, 0.22, n(x), 88 + 4 * j) for j, x in enumerate(['D2', 'Eb2', 'E2', 'F2']) for k in range(j * 16, j * 16 + 16)], 100),
    V(7, 48, accordi(['D3 A3 D4', 'D3 A3 D4', 'Bb2 F3 D4', 'C3 G3 E4', 'D3 A3 F4', 'F3 C4 A4', 'Bb2 F3 D4', 'A2 E3 C#4'], 4, _q(17), vel=70) +
             _sale(['D3 A3 F4', 'Eb3 Bb3 G4', 'E3 B3 G4', 'F3 C4 A4'], 25, [90, 96, 102, 108]), 90),
    V(8, 61, _sale(['D4 F4 A4', 'Eb4 G4 Bb4', 'E4 G4 B4', 'F4 Ab4 C5'], 25, [92, 102, 108, 114]), 100),   # il culmine: sale di semitono
    V(10, 47, [(_q(17) + k * 2.0, 0.8, n('D2'), 72 + k * 2) for k in range(8)] +             # i timpani
              [(_q(25) + k * 0.25, 0.22, n('D2'), min(120, 70 + k)) for k in range(64)], 100),
], coda_quarti=6.0)

BRANI = {'menu': MENU, 'briefing': BRIEFING, 'debriefing_riuscito': DEB_OK, 'debriefing_fallito': DEB_KO,
         'cerimonia': CERIMONIA, 'albo': ALBO, 'museo': MUSEO, 'editor': EDITOR, 'apertura': APERTURA}


# --- il file MIDI ---

def vlq(v):
    b = [v & 0x7F]
    v >>= 7
    while v:
        b.append(0x80 | (v & 0x7F))
        v >>= 7
    return bytes(reversed(b))


def traccia(eventi):
    eventi.sort(key=lambda e: (e[0], e[1]))
    out, prima = bytearray(), 0
    for tick, _, dati in eventi:
        out += vlq(tick - prima) + dati
        prima = tick
    out += vlq(0) + b'\xff\x2f\x00'
    return b'MTrk' + struct.pack('>I', len(out)) + bytes(out)


def smf(b):
    tick = lambda q: int(round(q * DIVISIONE))
    us = int(round(60_000_000 / b['bpm']))
    conduttore = [(0, 0, b'\xff\x51\x03' + us.to_bytes(3, 'big'))]
    fine = 0.0
    for v in b['voci']:
        for t, d, _, _ in v['note']:
            fine = max(fine, t + d)
    if b['loop']:
        for q, testo in ((b['loop'][0], b'loop_inizio'), (b['loop'][1], b'loop_fine')):
            conduttore.append((tick(q), 1, b'\xff\x06' + vlq(len(testo)) + testo))
    else:
        conduttore.append((tick(fine + b['coda']), 1, b'\xff\x06' + vlq(4) + b'fine'))
    tracce = [traccia(conduttore)]
    for v in b['voci']:
        c = v['canale']
        ev = [(0, 0, bytes([0xB0 | c, 7, v['volume']]))]
        if v['programma'] >= 0:
            ev.append((0, 0, bytes([0xC0 | c, v['programma']])))
        for t, d, h, vel in v['note']:
            assert 0 <= h <= 127 and 1 <= vel <= 127, (v, h, vel)
            ev.append((tick(t), 2, bytes([0x90 | c, h, vel])))
            ev.append((tick(t + d), 1, bytes([0x80 | c, h, 0])))   # a pari tick gli spegnimenti prima
        tracce.append(traccia(ev))
    return b'MThd' + struct.pack('>IHHH', 6, 1, len(tracce), DIVISIONE) + b''.join(tracce)


def esporta_musiche_fm(radice):
    uscita = os.path.join(radice, 'data', 'motore', 'musica', 'fm')
    os.makedirs(uscita, exist_ok=True)
    for nome, b in BRANI.items():
        open(os.path.join(uscita, nome + '.mid'), 'wb').write(smf(b))
    return len(BRANI)


if __name__ == '__main__':
    print('musiche FM:', esporta_musiche_fm(sys.argv[1] if len(sys.argv) > 1 else '.'))
