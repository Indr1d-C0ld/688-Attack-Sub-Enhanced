#!/usr/bin/env python3
# S07 per il controllo dei colpi d'artiglieria: rifa' una partita e scrive la memoria massima. Solo lettura sul gioco.
# uso: artiglieria_s07.py ETICHETTA BINARIO SEME
import os, resource, subprocess, sys, time
R = '/home/randolph/Claude/melchior-688'
U = R + '/engine/build_difese/tmp/artiglieria'
etichetta, binario, seme = sys.argv[1:4]
os.makedirs(U + '/tmp', exist_ok=True)
amb = dict(os.environ, TMPDIR=U + '/tmp', LD_LIBRARY_PATH=R + '/tools/deps/sysroot/usr/lib/x86_64-linux-gnu')
b = f'{U}/{etichetta}'
inizio = time.time()
with open(b + '.txt', 'wb') as out:
    rc = subprocess.call(['nice', '-n', '19', binario, '--sandbox', '--missione', 'libreria/LIB_S07_TAPPO', '--seme', seme, '--secondi', '9000',
                          '--salva-traccia', b + '.m688tr', '--istantanea', b + '.bmp'], cwd=R + '/engine', env=amb, stdout=out, stderr=out)
rss = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
if os.path.exists(b + '.bmp'): os.remove(b + '.bmp')
with open(f'{U}/fatti.txt', 'a') as f:
    f.write(f'{etichetta} seme {seme} rc {rc} {time.time() - inizio:.0f} s memoria massima {rss / 1024:.0f} MB\n')
