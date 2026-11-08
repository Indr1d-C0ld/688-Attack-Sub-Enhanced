"""Stato della traduzione: elenca i paragrafi di prosa non ancora tradotti nei lotti.

Uso:  trad_stato.py <radice> [quanti]   (stampa id|testo dei prossimi paragrafi da tradurre)
"""
import sys, json, glob

root = sys.argv[1]; quanti = int(sys.argv[2]) if len(sys.argv) > 2 else 40
mem = json.load(open(f'{root}/docs/traduzione/memoria_en.json'))
fatti = {}
for f in sorted(glob.glob(f'{root}/docs/traduzione/lotti/lotto_*.json')):
    fatti.update(json.load(open(f)))
prosa = [k for k, v in mem.items() if v['tipo'] == 'testo']
mancano = [k for k in prosa if k not in fatti]
print(f'# prosa: {len(prosa)}  tradotti: {len(prosa) - len(mancano)}  mancanti: {len(mancano)}')
for k in mancano[:quanti]:
    print(f"{k}|{','.join(mem[k]['scene'])}|{mem[k]['en']}")
