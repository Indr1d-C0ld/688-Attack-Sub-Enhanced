"""Misura le parti dei modelli 3D del kit (data/motore/modelli/<classe>.mdl, formato MDL2 di esporta_modello.py):
per ogni parte gli intervalli x (verso prua), y (verso sinistra) e z (in su, galleggiamento a 0), in metri del modello.

Uso: misura_parti.py <cartella dei modelli> <classe> [<classe> ...] [--parti vela,scafo,...]

Serve a ricavare dai modelli le forme usate altrove (collisioni: altezza e posizione della vela, duomo sonar sotto la
chiglia, VELA e DUOMO di tools/pipeline/esporta_motore.py; artiglieria: sagome in superficie di
tools/pipeline/artiglieria.py). Le x vanno riportate alla lunghezza della classe (classi.toml) col rapporto fra la
lunghezza e l'estensione della parte «scafo».
"""
import struct
import sys
import zlib

import numpy as np


def leggi(percorso):
    """(lunghezza, quota massima, nomi delle parti, vertici N x 3 x 3, parte di ogni triangolo)"""
    b = open(percorso, 'rb').read()
    assert b[:4] == b'MDL2', percorso
    n, _w, _h, _texel, _tono, _amb, lung, qmax = struct.unpack_from('<IIIfffff', b, 4)
    o = 36
    (np_,) = struct.unpack_from('<H', b, o)
    o += 2
    parti = []
    for _ in range(np_):
        k = b[o]
        o += 1
        parti.append(b[o:o + k].decode())
        o += k
    (lb,) = struct.unpack_from('<I', b, o)
    o += 4
    dati = zlib.decompress(b[o:o + lb], -15)
    rec = np.frombuffer(dati, np.uint8).reshape(91, n).T.copy()   # i record sono per piani di byte
    vertici = rec[:, :36].view('<f4').reshape(n, 3, 3)
    parte = rec[:, 89:91].view('<u2').reshape(n)
    return lung, qmax, parti, vertici, parte


def intervalli(vertici, parte, parti, nome):
    """(x0, x1, y0, y1, z0, z1) della parte, o None se il modello non l'ha"""
    if nome not in parti:
        return None
    s = vertici[parte == parti.index(nome)]
    return tuple(float(f(s[:, :, k])) for k in range(3) for f in (np.min, np.max))


if __name__ == '__main__':
    argomenti = [a for a in sys.argv[1:] if not a.startswith('--parti')]
    filtro = None
    for a in sys.argv[1:]:
        if a.startswith('--parti'):
            filtro = a.split('=', 1)[1].split(',') if '=' in a else None
    if len(argomenti) < 2:
        sys.exit(__doc__)
    cartella, classi = argomenti[0], argomenti[1:]
    for c in classi:
        lung, qmax, parti, v, p = leggi(f'{cartella}/{c}.mdl')
        print(f'{c}: lunghezza {lung:.1f} m, quota massima {qmax:.1f} m')
        for nome in parti:
            if filtro and nome not in filtro:
                continue
            x0, x1, y0, y1, z0, z1 = intervalli(v, p, parti, nome)
            print(f'  {nome:24s} x [{x0:7.1f}, {x1:7.1f}]  y [{y0:6.1f}, {y1:6.1f}]  z [{z0:6.1f}, {z1:6.1f}]')
