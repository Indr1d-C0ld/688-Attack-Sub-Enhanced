"""Le impronte di riferimento delle rapide (test/prove_scenario_missione.cpp) dopo «Ochotsk» -> «Okhotsk» nel file di
dati: per ogni codice, la missione rigenerata col nome italiano vecchio rimesso («di Ochotsk»; l'inglese era gia' Okhotsk) deve avere l'impronta vecchia; allora la nuova
impronta sostituisce la vecchia. Uso: verifica_impronte.py <cartella dei file generati> <file di prova>"""
import pathlib
import re
import sys


def fnv(b):
    h = 1469598103934665603
    for c in b:
        h = ((h ^ c) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


cartella, prova = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
testo = prova.read_text(encoding='utf-8')
cambiate = 0
for codice, vecchia in re.findall(r'\{"([0-9A-Z-]{15})", 0x([0-9a-f]{16})ull\}', testo):
    f = cartella / ('RAPIDA_' + codice.replace('-', '') + '.lua')
    nuovo = f.read_bytes()
    nuova = fnv(nuovo)
    if nuova == int(vecchia, 16):
        continue
    if fnv(nuovo.replace('di Okhotsk'.encode(), 'di Ochotsk'.encode())) != int(vecchia, 16):
        sys.exit(f'{codice}: cambia anche altro oltre al nome')
    testo = testo.replace(f'0x{vecchia}ull', f'0x{nuova:016x}ull')
    cambiate += 1
    print(f'{codice}: solo il nome, impronta aggiornata')
prova.write_text(testo, encoding='utf-8')
print(f'impronte aggiornate: {cambiate}')
