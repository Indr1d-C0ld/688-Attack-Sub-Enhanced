"""Nimitz: sprite di prova NON tagliati in alto (solo diagnosi, non sostituiscono quelli di produzione).

Il renderer condiviso (costruisci.camera_vista) inquadra un'altezza fissa di 46 m attorno a z = 18,4 m:
sopra circa 41 m tutto viene tagliato. Sulla Nimitz restano fuori l'antenna SPS-48 (fino a 42 m), la
parte alta del traliccio poppiero e l'albero principale (cima a 63 m). Questo script NON modifica i file
condivisi: sostituisce camera_vista solo dentro il proprio processo, con un'altezza inquadrata di 72 m,
e scrive gli sprite in extracted/navi/nimitz/sprite_alti/ (stessa palette, stessa scala, stesse foto).

Uso: nimitz_sprite_alti.py <radice progetto> [passo_gradi]
"""
import os, sys, functools
import numpy as np, yaml
from PIL import Image
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import costruisci
from foto import Foto

ALTO = 72.0

if __name__ == '__main__':
    root = sys.argv[1]; passo = int(sys.argv[2]) if len(sys.argv) > 2 else 30
    fcl = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'nimitz.yaml')
    cl = yaml.safe_load(open(fcl)); out = f"{root}/{cl['uscita']}/sprite_alti"; os.makedirs(out, exist_ok=True)
    mesh = costruisci.costruisci_mesh(cl, os.path.dirname(fcl))
    XB = cl['scafo']['L'] / 2
    foto = []
    for fc in cl['foto']:
        fc = dict(fc, punti=[[costruisci.risolvi(pp, XB), q] for pp, q in fc['punti']])
        f = Foto(fc, root); f.calibra(mesh); foto.append(f)
        print(f"  {f.nome}: errore {f.rms_punti:.1f} px, coincidenza {f.iou:.3f}")
    foto += [f.speculare() for f in list(foto) if f.cfg.get('specchiabile')]
    # sostituzione locale: altezza inquadrata 72 m (mira a 0,40 x 72 = 28,8 m)
    costruisci.camera_vista = functools.partial(costruisci.camera_vista, alto=ALTO)
    pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
    ramp = np.array([pal[3 * i] for i in range(144, 159)], float)
    for aob in range(0, 181, passo):
        for nome, lato in (('dritta', 1), ('sinistra', -1)):
            k = costruisci.sprite(costruisci.rendi(mesh, foto, aob, lato), ramp)
            im = Image.fromarray(k, 'P'); im.putpalette(pal)
            im.save(f'{out}/aob_{aob:03d}_{nome}.png', transparency=159)
            print(f'  aob {aob:3d} {nome}: {k.shape[1]} x {k.shape[0]} px')
