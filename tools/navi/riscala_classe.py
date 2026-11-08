"""Una classe nuova dal modello e dagli sprite di una parente, riscalati alle misure vere: il passo 2 delle classi del
primo lotto delle firme nuove (docs/ricerca/31_firme_lotto1.md; approvato dalla principale il 29/10/2026). La forma
resta quella della parente, le misure diventano quelle vere; i modelli propri sono il passo 3.

Scrive in extracted/navi/<nuova>/:
- modello.mdl: i vertici della parente scalati per asse (x a prua per sx, y a sinistra per sy, z in alto per sz); le
  tessiture cotte restano quelle della parente (i blocchi 2 e 3 del formato MDL2 si copiano tali e quali: le coordinate
  di tessitura non cambiano, la tessitura si stira col triangolo). L'intestazione prende la lunghezza e la quota
  massima nuove;
- sprite/: per ogni angolo sulla prora lo sprite della parente ricampionato (vicino piu' prossimo, gli indici restano
  della palette): in orizzonte per (sx L |sen a| + sy B |cos a|) / (L |sen a| + B |cos a|), in verticale per sz; le
  ancore con gli stessi fattori. La scala in pixel per metro resta;
- scafo.toml (se la parente l'ha): stazioni, chiglia e semilarghezze, scafo resistente e celle scalati per asse
  (volumi per sx sy sz); lunghezza, larghezza e dislocamenti nuovi; il resto della parente (STIMA);
- RAPPORTO.md: da dove viene, coi fattori.

Uso: riscala_classe.py <radice> <nuova> <parente> <sx> <sy> <sz> <lunghezza_m> <larghezza_m> [disl_superficie_t disl_immersione_t]
"""
import json, os, re, struct, sys, zlib
import numpy as np
from PIL import Image

CARTELLE = {'oscar': 'oscar2'}   # come esporta_motore.CARTELLE
RECORD = 91                      # byte di un triangolo nel blocco 1 (esporta_modello.scrivi)


def deflate(dati):
    z = zlib.compressobj(9, zlib.DEFLATED, -15, 9)
    return z.compress(dati) + z.flush()


def riscala_modello(da, a, sx, sy, sz):
    b = open(da, 'rb').read()
    assert b[:4] == b'MDL2', da
    n, W, H, texel, tono, luce, L, quota = struct.unpack_from('<IIIfffff', b, 4)
    p = 4 + struct.calcsize('<IIIfffff')
    (parti,) = struct.unpack_from('<H', b, p)
    p += 2
    for _ in range(parti):
        p += 1 + b[p]
    testa_parti = b[4 + struct.calcsize('<IIIfffff'):p]
    (lung,) = struct.unpack_from('<I', b, p)
    tri = np.frombuffer(zlib.decompress(b[p + 4:p + 4 + lung], -15), np.uint8).reshape(RECORD, n).T.copy()
    resto = b[p + 4 + lung:]   # i blocchi 2 e 3, le tessiture: invariati
    v = tri[:, :36].copy().view('<f4').reshape(n, 9)
    v[:, 0::3] *= sx
    v[:, 1::3] *= sy
    v[:, 2::3] *= sz
    tri[:, :36] = v.reshape(n, 9).view(np.uint8).reshape(n, 36)
    blocco = deflate(np.ascontiguousarray(tri.T).tobytes())
    with open(a, 'wb') as f:
        f.write(b'MDL2' + struct.pack('<IIIfffff', n, W, H, texel, tono, luce, L * sx, quota * sz))
        f.write(testa_parti + struct.pack('<I', len(blocco)) + blocco + resto)
    return L * sx, quota * sz


def riscala_sprite(da, a, sx, sy, sz, L, B):
    os.makedirs(a, exist_ok=True)
    anc = json.load(open(f'{da}/ancore.json'))
    nuove = {}
    for nome, (ax, ay) in anc['ancore'].items():
        aob = np.radians(int(nome.split('_')[1]))
        s, c = abs(np.sin(aob)), abs(np.cos(aob))
        fh = (sx * L * s + sy * B * c) / (L * s + B * c)
        im = Image.open(f'{da}/{nome}.png')
        w, h = im.size
        nw, nh = max(1, round(w * fh)), max(1, round(h * sz))
        im.resize((nw, nh), Image.NEAREST).save(f'{a}/{nome}.png')
        nuove[nome] = [round(ax * nw / w, 2), round(ay * nh / h, 2)]
    anc = dict(anc, ancore=nuove, nota=f"{anc.get('nota', '')} (riscalati da tools/navi/riscala_classe.py)".strip())
    json.dump(anc, open(f'{a}/ancore.json', 'w'), indent=1)
    return len(nuove)


def riscala_scafo(da, a, sx, sy, sz, L, B, disl):
    """Il testo della parente coi numeri scalati, chiave per chiave"""
    sr = (sy * sz) ** 0.5
    per_chiave = {'passo_m': sx, 'x': sx, 'x0': sx, 'x1': sx, 'dz_m': sz, 'z_chiglia': sz, 'z_resistente': sz, 'z0': sz,
                  'z1': sz, 'y_resistente': sy, 'y0': sy, 'y1': sy, 'raggio_resistente': sr, 'intercapedine_m': sr,
                  'volume_resistente_m3': sx * sy * sz, 'volume_esterno_m3': sx * sy * sz, 'zavorra_iniziale_m3': sx * sy * sz}
    numero = re.compile(r'-?\d+(?:\.\d+)?')
    righe, in_semi = [], False
    for r in open(da).read().split('\n'):
        m = re.match(r'^(\w+) = (.*)$', r)
        if in_semi:   # le righe delle semilarghezze, finche' la lista non si chiude
            righe.append(numero.sub(lambda x: f'{float(x.group()) * sy:.3f}', r))
            in_semi = r.strip() != ']'
            continue
        if m and m.group(1) == 'semilarghezze':
            righe.append(r)
            in_semi = True
            continue
        if m and m.group(1) in per_chiave:
            f, v = per_chiave[m.group(1)], m.group(2)
            cifre = lambda x: len(x.group().split('.')[1]) if '.' in x.group() else 0
            v = numero.sub(lambda x: f'{float(x.group()) * f:.{max(cifre(x), 3 if "." in x.group() else 0)}f}'
                           if '.' in x.group() else str(round(float(x.group()) * f)), v)
            righe.append(f'{m.group(1)} = {v}')
        elif m and m.group(1) == 'lunghezza_m':
            righe.append(f'lunghezza_m = {L}')
        elif m and m.group(1) == 'larghezza_m':
            righe.append(f'larghezza_m = {B:.2f}')
        elif m and disl and m.group(1) in ('dislocamento_pubblicato_t', 'dislocamento_emersione_t'):
            righe.append(f'{m.group(1)} = {disl[0]}')
        elif m and disl and m.group(1) == 'dislocamento_immersione_t':
            righe.append(f'{m.group(1)} = {disl[1]}')
        else:
            righe.append(r)
    testo = '\n'.join(righe)
    testo = testo.replace('# Scafo per la fisica dei danni (tools/navi/compartimenti.py): non modificare a mano.',
                          '# Scafo per la fisica dei danni: quello della parente riscalato (tools/navi/riscala_classe.py).', 1)
    open(a, 'w').write(testo)


def main():
    radice, nuova, parente = sys.argv[1], sys.argv[2], sys.argv[3]
    sx, sy, sz, L, B = (float(x) for x in sys.argv[4:9])
    disl = (int(sys.argv[9]), int(sys.argv[10])) if len(sys.argv) > 10 else None
    da = f'{radice}/extracted/navi/{CARTELLE.get(parente, parente)}'
    a = f'{radice}/extracted/navi/{nuova}'
    os.makedirs(a, exist_ok=True)
    Lm, quota = riscala_modello(f'{da}/modello.mdl', f'{a}/modello.mdl', sx, sy, sz)
    Lp = L / sx
    n = riscala_sprite(f'{da}/sprite', f'{a}/sprite', sx, sy, sz, Lp, B / sy)
    con_scafo = os.path.exists(f'{da}/scafo.toml')
    if con_scafo:
        riscala_scafo(f'{da}/scafo.toml', f'{a}/scafo.toml', sx, sy, sz, L, B, disl)
    open(f'{a}/RAPPORTO.md', 'w').write(
        f'# {nuova}: la parente {parente} riscalata alle misure vere\n\n'
        f'Passo 2 del primo lotto delle firme nuove (docs/ricerca/31_firme_lotto1.md). Fatto da tools/navi/riscala_classe.py:\n'
        f'- fattori: x (lunghezza) {sx:.4f}, y (larghezza) {sy:.4f}, z (altezze) {sz:.4f};\n'
        f'- misure: {L} x {B} m; il modello misura {Lm:.1f} m, alto {quota:.1f} m sul galleggiamento;\n'
        f'- {n} sprite ricampionati da quelli della parente; tessiture del modello della parente, stirate;\n'
        f'- scafo per i danni: {"quello della parente riscalato" if con_scafo else "nessuno (la parente non lo ha)"}'
        f'{", dislocamenti " + str(disl[0]) + "/" + str(disl[1]) + " t" if disl else ""}.\n\n'
        f'La forma e i dettagli sono della parente: **STIMA** finche\' la classe non ha il suo modello (passo 3).\n')
    print(f'{nuova}: modello {Lm:.1f} m, {n} sprite, scafo {"si" if con_scafo else "no"}')


if __name__ == '__main__':
    main()
