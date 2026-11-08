# che cosa rifarebbe manuale.py in F7, senza scrivere nulla
import sys, pathlib, difflib
R = pathlib.Path('/home/randolph/Claude/melchior-688')
sys.path.insert(0, str(R / 'tools/pipeline'))
import manuale
manuale.MANUALE = R / 'docs/manuale_enhanced'
f = R / 'docs/manuale_enhanced/2_riferimento/F7_editor_e_pacchetti.md'
t = f.read_text(encoding='utf-8')
n = manuale.rifai(t, R / 'data/motore', True)
print('\n'.join(difflib.unified_diff(t.splitlines(), n.splitlines(), lineterm='', n=0)))
