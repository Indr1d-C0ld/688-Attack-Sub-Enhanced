#!/usr/bin/env python3
"""Le anteprime: corse della M4 per configurazione, distanza e seme; scene per coppia, oggi contro griglia e sorgenti."""
import glob, os, re, sys, collections
E = sys.argv[1] if len(sys.argv) > 1 else '.'
righe = collections.defaultdict(dict)
for f in sorted(glob.glob(E + '/corse/*.tsv')):
    m = re.match(r'(\w+?)_(ia|g)_k(\d+)_s(\d)\.tsv', os.path.basename(f))
    if not m: continue
    conf, modo, km, seme = m.groups()
    t = open(f).read().splitlines()
    fine = next((l.split('t_fine=')[1] for l in t if l.startswith('# riassunto')), '?')
    for l in t:
        if l.startswith('# ') and not l.startswith('# bers') and not l.startswith('# riassunto'):
            p = l[2:].split()
            # id classe primo d sens q799 solido d sens pos d err perdite se_max
            nums = [x for x in p[2:]]
            righe[(modo, int(km), conf, p[1])][seme] = (nums[0], nums[6] if len(nums) > 6 else '?', nums[-2], nums[-1])
print('modo km conf bersaglio: per seme primo/solido/perdite/se_max')
for k in sorted(righe):
    print(*k, ' | '.join('%s: %s/%s/%s/%s' % (s, *v) for s, v in sorted(righe[k].items())))
print()
sc = collections.defaultdict(lambda: collections.defaultdict(dict))
for f in sorted(glob.glob(E + '/scene/*.txt')):
    m = re.match(r'(oggi|gs)_(PROVA_\w+)_s(\d)\.txt', os.path.basename(f))
    conf, scena, seme = m.groups()
    for l in open(f):
        if l.startswith('#') or not l.strip(): continue
        p = l.split()
        if p[3] in ('sonoboa',): continue
        sc[scena][tuple(p[:4])][(conf, seme)] = (p[4], p[5], p[7], p[9])
for scena in sorted(sc):
    print('==', scena, '(oss bers: oggi s1 s2 s3 || griglia+sorgenti s1 s2 s3; primo_s@km,solido,pos)')
    for k in sorted(sc[scena], key=lambda x: (int(x[0]), int(x[2]))):
        d = sc[scena][k]
        v = lambda c: ' '.join('%s@%s,%s,%s' % d[(c, s)] if (c, s) in d else '-' for s in '123')
        print(' ', *k, '|', v('oggi'), '||', v('gs'))
