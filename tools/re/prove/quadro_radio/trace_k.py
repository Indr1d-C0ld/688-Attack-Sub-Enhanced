# Le scoperte radar e vista sui battelli in una traccia: tempo, osservatore, bersaglio, sensore. Solo lettura.
import sys, runpy
m = runpy.run_path('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/occasioni_tracce.py', run_name='lib')
serie, K, I, poss = m['leggi'](sys.argv[1])
for r in sorted(set((round(t), o, b, sn) for (t, o, b, sn) in poss if sn in ('radar', 'esm', 'vista', 'periscopio') and b in serie)): print('possibile', *r)
