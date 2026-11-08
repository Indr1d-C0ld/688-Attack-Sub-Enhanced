"""Estrae il testo dei briefing e dei messaggi di fine missione dai file SCENE*.LZA decompressi.

Markup del testo (token del dialogo): 1D 00 <testo> 00 = riga; 0C 00 nn = colore; 0B 00 i v = imposta
il byte i del blocco di configurazione missione (14 = indice mappa).
"""
import glob, os, re, sys

MISSIONI = {0: "HIT AND RUN", 1: "TORPEX '89", 2: "SHAKE 'EM", 3: "SUB OPS", 4: "MUMAR CADAVER",
            5: "ESCAPE", 6: "GOULASH", 7: "CAT WALK", 8: "SURPRISE PARTY", 9: "HOMECOMING"}

def lines(d):
    for m in re.finditer(rb'\x1d\x00([\x20-\x7e]*)\x00', d):
        yield m.group(1).decode('ascii').rstrip()

if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    with open(dst, 'w') as out:
        for f in sorted(glob.glob(f'{src}/SCENE*.LZA.bin'), key=lambda p: (int(os.path.basename(p)[5]) or 10, p)):
            sc = os.path.basename(f)[:7]
            n = int(sc[5])
            out.write(f'\n{"=" * 70}\n{sc}  missione {n or 10}: {MISSIONI[n]}  lato {sc[6]}\n{"=" * 70}\n')
            for l in lines(open(f, 'rb').read()):
                out.write(l + '\n')
    print('scritto', dst)
