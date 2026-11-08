# La quota e la velocita' del Foxtrot in S03 seme 1, base e P3, ogni 300 s dove si staccano. Solo lettura.
import sys, runpy
m = runpy.run_path('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/occasioni_tracce.py', run_name='lib')
B='/home/randolph/Claude/melchior-688/engine/build_radio/'
a = m['leggi'](B+'base2/S03_1.m688tr')[0]; p = m['leggi'](B+'p3/S03_1.m688tr')[0]
print(a[2][:2], len(a[2]))
for t0 in range(0, 10800, 300):
    def v(s):
        x=[r for r in s if r[0]>=t0]
        return x[0] if x else None
    print(t0, 'base', v(a[2]), 'p3', v(p[2]))
