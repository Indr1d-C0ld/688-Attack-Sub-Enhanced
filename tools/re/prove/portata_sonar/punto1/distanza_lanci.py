#!/usr/bin/env python3
"""SBAGLIATO, da non usare per la distanza al lancio: a e b dell'evento sono x.stima->pos, quindi stampa stima-bersaglio, non
lanciatore-bersaglio. Le distanze giuste: ../sonda_lanci.cpp (07/11/2026).
Uso: distanza_lanci.py SONDA_TRACCE CARTELLA DURATA"""
import glob, math, os, re, subprocess, sys
sonda, cartella, dur = sys.argv[1:4]
env = dict(os.environ, LD_LIBRARY_PATH='/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu')
def posizioni(f, u):
    r = []
    for l in subprocess.run([sonda, f, str(u), '0', dur], capture_output=True, text=True, env=env).stdout.splitlines():
        m = re.match(r't=\s*([\d.]+) x=\s*(-?\d+) y=\s*(-?\d+)', l)
        if m: r.append(tuple(map(float, m.groups())))
    return r
for f in sorted(glob.glob(cartella + '/partita_*.m688tr'), key=lambda x: int(re.findall(r'(\d+)\.m688tr', x)[0])):
    s = re.findall(r'(\d+)\.m688tr', f)[0]
    t = subprocess.run([sonda, f, '0', '0', dur], capture_output=True, text=True, env=dict(env, TUTTI='1')).stdout
    for tm, al, x, y, arma in re.findall(r'ev t=\s*([\d.]+) u=0 lancio altra=(\d+) a=(-?[\d.]+) b=(-?[\d.]+) s=(\w+)', t):
        p = posizioni(f, al)
        q = min(p, key=lambda z: abs(z[0] - float(tm)))
        print('seme %s t=%7.1f %s su %s: stima-bersaglio %.1f km' % (s, float(tm), arma, al, math.hypot(q[1] - float(x), q[2] - float(y)) / 1000))
