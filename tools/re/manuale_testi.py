"""Struttura del manuale originale (docs/originale/manuale_EN.txt, da pdftotext -layout) per la traduzione.

Pulizia: rimuove le intestazioni "Seite N" e "688 Attack Sub", i numeri di pagina allineati a destra,
le righe tratteggiate dei cambi pagina e la firma della BBS; ricompone i paragrafi spezzati dal cambio pagina.
La copia BBS finisce con i comandi da tastiera: Appendice A, Glossario, Indice e crediti sono trascritti dal
manuale stampato in docs/originale/manuale_EN_appendici.txt (testo dopo <<INIZIO>>) e diventano il capitolo 8.
Blocchi: 'capitolo' (numero + titolo sottolineato da ===), 'titolo' (riga sottolineata da ===),
'elenco' (voce numerata o puntata), 'pre' (tabelle e figure in ASCII: righe con '|' o con colonne),
'didascalia' ([...] e "Figure n.n:"), 'sottotitolo' (riga sottolineata da ---), 'testo' (paragrafo).
Uscita: docs/traduzione/manuale/sorgente.json (blocchi in ordine, con capitolo e id) e
docs/traduzione/manuale/memoria_en.json (id -> testo; id = primi 10 caratteri dello SHA-1).
"""
import os, re, sys, json, hashlib

APPENDICI = 8   # Appendice A, Glossario, Indice, crediti

def pid(t): return hashlib.sha1(t.encode()).hexdigest()[:10]

def righe_pulite(path):
    rr = open(path, encoding='utf-8', errors='replace').read().split('\n')
    out = []; i = 0
    while i < len(rr):
        r = rr[i].rstrip()
        if re.fullmatch(r'\s*Seite \d+', r):
            if i + 1 < len(rr) and rr[i + 1].strip() == '688 Attack Sub': i += 1
            out.append('<<PAGINA>>'); i += 1; continue
        if r.startswith('Downloaded From P-80 Systems'):
            i += 1; continue
        if re.fullmatch(r'-{70,}', r.strip()) or re.fullmatch(r' {30,}\d{1,3}', r):
            out.append('<<PAGINA>>'); i += 1; continue
        out.append(r); i += 1
    return out

def e_pre(r):
    s = r.rstrip()
    if '|' in s or re.search(r'-{3,}\+|\+-{3,}', s): return True
    if re.search(r'\S {3,}\S.* {3,}\S', s.strip()) and not re.match(r'\s*\d+\.\s{2,}', s): return True
    if re.fullmatch(r'\s*[-=_*]{3,}[-=_* ]*', s) : return True
    return False

def blocchi(righe, cap=0):
    out = []; i = 0; n = len(righe)
    def aggiungi(tipo, testo, extra=None):
        testo = testo.strip()
        if not testo: return
        b = dict(tipo=tipo, capitolo=cap, en=testo, id=pid(testo))
        if extra: b.update(extra)
        out.append(b)
    par = []
    def chiudi():
        if par:
            aggiungi('testo', re.sub(r'\s+', ' ', ' '.join(par))); par.clear()
    def prossima(k, salta_vuote=False):
        # indice della prossima riga significativa (salta i cambi pagina, e se richiesto le righe vuote)
        k += 1
        while k < n and (righe[k] == '<<PAGINA>>' or (salta_vuote and not righe[k].strip())): k += 1
        return k
    while i < n:
        r = righe[i]; s = r.strip()
        k = prossima(i); nx = righe[k].strip() if k < n else ''
        if r == '<<PAGINA>>':
            i += 1; continue                      # il paragrafo puo' continuare dopo il cambio pagina
        if not s:
            # fine paragrafo solo se la riga successiva non vuota non continua la frase
            j = i + 1
            while j < n and (not righe[j].strip() or righe[j] == '<<PAGINA>>'): j += 1
            cont = par and j < n and righe[j].strip()[:1].islower() and not re.search(r'[.:!?)]$', par[-1])
            if not cont: chiudi()
            i += 1; continue
        if s and re.fullmatch(r'=+', nx) and len(nx) >= 3:
            chiudi()
            if out and out[-1]['tipo'] == 'numero_capitolo':
                num = out.pop()['en']; cap = int(num); aggiungi('capitolo', s, dict(numero=cap))
            else:
                aggiungi('titolo', s)
            i = k + 1; continue
        if s and re.fullmatch(r'-+', nx) and abs(len(nx) - len(s)) <= 2 and len(nx) >= 3:
            chiudi(); aggiungi('sottotitolo', s); i = k + 1; continue
        if re.fullmatch(r'\d{1,2}', s):
            t = prossima(i, True); u = prossima(t, True) if t < n else n
            if t < n and u < n and re.fullmatch(r'=+', righe[u].strip()):
                chiudi(); aggiungi('numero_capitolo', s); i = t; continue
        if re.fullmatch(r'\[.*\]', s) or re.match(r'Figure (\d+\.\d+|[A-Z]:)', s):
            chiudi(); aggiungi('didascalia', s); i += 1; continue
        if e_pre(r):
            chiudi(); blocco = [r]
            i += 1
            while i < n and righe[i] != '<<PAGINA>>' and (e_pre(righe[i]) or (righe[i].strip() and righe[i].startswith('  ') and blocco)):
                blocco.append(righe[i]); i += 1
            testo = '\n'.join(blocco)
            out.append(dict(tipo='pre', capitolo=cap, en=testo, id=pid(testo)))
            continue
        m = re.match(r'(\s*)(\d+\.|[A-C]\)|\*|-)\s+\S', r)
        if m and not par:
            chiudi(); voce = [s]; i += 1
            while i < n and righe[i].strip() and righe[i] != '<<PAGINA>>' and not re.match(r'\s*(\d+\.|[A-C]\)|\*|-)\s+\S', righe[i]) \
                    and righe[i].startswith(' ') and not e_pre(righe[i]):
                voce.append(righe[i].strip()); i += 1
            aggiungi('elenco', re.sub(r'\s+', ' ', ' '.join(voce)))
            continue
        par.append(s); i += 1
    chiudi()
    return out

if __name__ == '__main__':
    root = sys.argv[1]
    dst = f'{root}/docs/traduzione/manuale'; os.makedirs(dst, exist_ok=True)
    bb = blocchi(righe_pulite(f'{root}/docs/originale/manuale_EN.txt'))
    app = open(f'{root}/docs/originale/manuale_EN_appendici.txt', encoding='utf-8').read().split('<<INIZIO>>\n', 1)[1]
    bb += blocchi(app.split('\n'), cap=APPENDICI)
    json.dump(bb, open(f'{dst}/sorgente.json', 'w'), indent=1, ensure_ascii=False)
    mem = {}
    for b in bb:
        mem.setdefault(b['id'], dict(tipo=b['tipo'], capitolo=b['capitolo'], en=b['en']))
    json.dump(mem, open(f'{dst}/memoria_en.json', 'w'), indent=1, ensure_ascii=False)
    from collections import Counter
    c = Counter(b['tipo'] for b in bb)
    parole = Counter()
    for b in mem.values(): parole[b['capitolo']] += len(b['en'].split())
    print('blocchi', len(bb), 'unici', len(mem), dict(c))
    print('parole per capitolo', dict(sorted(parole.items())))
    for b in bb:
        if b['tipo'] in ('capitolo', 'titolo', 'sottotitolo'): print(f"  cap {b['capitolo']} {b['tipo']:11s} {b['en']}")
