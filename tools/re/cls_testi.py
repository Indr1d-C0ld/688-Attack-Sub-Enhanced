"""Inventario delle stringhe dell'interfaccia nei file *.CLS (layout delle stazioni e dei dialoghi).

Le stringhe sono testo ASCII stampabile delimitato dai token del layout; si tengono quelle di almeno
3 caratteri con almeno 2 lettere, escludendo i residui binari (sequenze senza vocali o con simboli).
Uscita: docs/traduzione/interfaccia_en.json  {file: [stringhe in ordine, senza duplicati]}
"""
import os, re, sys, glob, json

def pulisci(s):
    s = s.strip()
    s = re.sub(r'^[^A-Za-z0-9(@*\'"]+', '', s)          # byte di layout attaccati in testa
    s = re.sub(r'^[a-z](?=[A-Z]{2})', '', s)               # byte di layout minuscolo prima di una parola maiuscola
    return s

def valida(s):
    lettere = sum(c.isalpha() for c in s)
    if len(s) < 3 or lettere < 2: return False
    if not re.search(r'[AEIOUaeiou]', s): return False
    if re.search(r'[`{}|~\\^]', s): return False
    if re.fullmatch(r'\w+\.(enc|ENC|cls|CLS|lzw|LZW)', s): return False    # nomi di file
    return True

if __name__ == '__main__':
    root = sys.argv[1]; out = {}
    for f in sorted(glob.glob(f'{root}/extracted/raw/DISK*/*.CLS')):
        b = open(f, 'rb').read()
        viste = []
        for m in re.finditer(rb'[\x20-\x7e]{3,}', b):
            s = pulisci(m.group().decode('ascii'))
            if valida(s) and s not in viste: viste.append(s)
        if viste: out[os.path.basename(f)] = viste
    json.dump(out, open(f'{root}/docs/traduzione/interfaccia_en.json', 'w'), indent=1, ensure_ascii=False)
    for k, v in out.items(): print(f'{k:12s} {len(v):4d}')
    print('totale', sum(len(v) for v in out.values()))
