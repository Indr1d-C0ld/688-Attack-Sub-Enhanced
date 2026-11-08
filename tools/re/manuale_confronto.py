"""Confronta il testo BBS del manuale (sorgente.json, capitoli 0-7) con l'OCR della scansione stampata.

Fonte OCR: https://archive.org/download/688attacksub_201909/688%20Attack%20Sub_djvu.txt (testo semplice).
Confronto parola per parola (minuscole, punteggiatura tolta, a capo con trattino ricomposti). Si scartano le
differenze dovute all'OCR: parole fuse o spezzate (stesse lettere), numeri di pagina, puntini e marchi della
BBS. Le sostituzioni brevi e simili sono marcate 'refuso' (di solito errori di battitura della copia BBS);
il resto e' 'testo' e va controllato a mano sulla pagina. Le figure ASCII (blocchi 'pre') non si confrontano.

Uso:  manuale_confronto.py <radice> <ocr.txt>   ->  stampa le differenze, una per riga
"""
import re, sys, json, difflib

def parole(t):
    t = t.replace('\xad ', '').replace('\xad', '').replace('’', "'").replace('‘', "'").replace('“', '"').replace('”', '"')
    t = t.replace('—', ' ').replace('–', ' ').replace('--', ' ')
    t = re.sub(r'(\w)- (\w)', r'\1\2', t)
    return re.findall(r"[a-z0-9]+(?:'[a-z]+)?", t.lower())

if __name__ == '__main__':
    root, ocr = sys.argv[1], sys.argv[2]
    bb = json.load(open(f'{root}/docs/traduzione/manuale/sorgente.json'))
    A, Aid = [], []
    for b in bb:
        if b['capitolo'] > 7 or b['tipo'] == 'pre': continue
        w = parole(b['en']); A += w; Aid += [b['id']] * len(w)
    B = parole(re.sub(r'-\s*\n\s*', '', open(ocr, encoding='utf-8').read()))
    for t, i1, i2, j1, j2 in difflib.SequenceMatcher(None, A, B, autojunk=False).get_opcodes():
        if t == 'equal': continue
        a, b = A[i1:i2], B[j1:j2]; ja, jb = ''.join(a), ''.join(b)
        if ja == jb: continue
        if t == 'insert' and all(w.isdigit() for w in b): continue
        if t == 'delete' and ja in ('o', 'v', 'tm', 'r'): continue
        tipo = 'refuso' if (t == 'replace' and len(ja) <= 12 and abs(len(ja) - len(jb)) <= 2
                            and difflib.SequenceMatcher(None, ja, jb).ratio() >= 0.75) else 'testo'
        print(f"{tipo} {t[:3]} {Aid[min(i1, len(Aid) - 1)]} [{' '.join(A[max(0, i1 - 5):i1])}] "
              f"BBS«{' '.join(a)}» STAMPA«{' '.join(b[:60])}»")
