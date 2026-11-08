#!/usr/bin/env python3
"""Confronta i serie.toml: esiti, e per parte lanci, a segno, perse, scoperte (media, min, max), armi."""
import re, sys
for c in sys.argv[1:]:
    t = open(c + '/serie.toml').read()
    print('==', c)
    print('  esiti', re.findall(r'^"(\d+)" = \{ volte = (\d+)', t, re.M))
    for parte, corpo in re.findall(r'^\[parti\.(\w+)\]\n(.*?)(?=^\[)', t, re.M | re.S):
        v = {k: m for k, m in re.findall(r'^(lanci|a_segno|perse|scoperte) = \{ media = ([\d.e+-]+)', corpo, re.M)}
        r = {k: (a, b) for k, a, b in re.findall(r'^(lanci|a_segno|perse|scoperte) = \{ media = [\d.e+-]+, dev = [\d.e+-]+, min = ([\d.e+-]+), max = ([\d.e+-]+)', corpo, re.M)}
        print('  %-9s' % parte, ' '.join('%s %s[%s-%s]' % (k, v[k], *r[k]) for k in ('lanci', 'a_segno', 'perse', 'scoperte') if k in v))
    a = re.findall(r'^"(\w+)" = \{ a_segno = (\d+), lanciate = (\d+)', t, re.M)
    if a: print('  armi', ' '.join('%s %s/%s' % (n, s, l) for n, s, l in a))
