"""Unisce la traduzione italiana del manuale originale.

Entrate: docs/traduzione/manuale/sorgente.json (da manuale_testi.py) e docs/traduzione/manuale/lotti/m_*.json
(traduzione a mano, per id di blocco). Regole dei lotti:
- "" = blocco assorbito dal precedente (frammenti spezzati dal cambio pagina, righe di tabella che la
  traduzione ha ricomposto in una tabella unica nel primo blocco);
- le didascalie [NOME BATTELLO] (marcatori della protezione anticopia) non si traducono: passano invariate;
- un blocco 'pre' tradotto con una tabella Markdown (prima riga che comincia con '|') esce così com'è,
  gli altri 'pre' (figure ASCII) escono in un blocco di codice.
Le figure ritagliate dalla scansione del manuale stampato (figure.json, da manuale_figure.py) entrano nei
Markdown al posto delle figure ASCII della copia BBS, che restano richiudibili sotto l'immagine.
Uscite: docs/traduzione/manuale/memoria_it.json (id -> testo italiano), capitolo_NN.md (uno per capitolo,
0 = frontespizio e sommario, 8 = appendici) e manuale_IT.md (tutto il manuale).
Esce con errore se un blocco resta senza traduzione; segnala i termini vietati dal glossario.
"""
import os, re, sys, glob, json

# termini che il glossario (docs/traduzione/glossario.md) esclude dalla traduzione
VIETATI = [r'(?<!anti)sommergibil', r'\bcapitan[oi]\b', r'\bwaypoint', r'\bnoisemaker', r'\barray\b', r'\bhelicopt',
           r'\b\d+(,\d+)? ?kt\b', r'\bnm\b', r'\bprofondità periscopica', r'\bsonar target marker']

APPENDICI = 8   # capitolo 8: Appendice A, Glossario, Indice, crediti (trascritti dal manuale stampato)

def passa(b):
    return b['tipo'] == 'didascalia' and b['en'].startswith('[')

def paragrafi(t):
    # paragrafi separati da riga vuota; dentro un paragrafo le righe restano righe (a capo forzato),
    # tranne le righe di tabella
    out = []
    for p in t.split('\n\n'):
        rr = p.split('\n')
        if any(r.startswith('|') for r in rr): out.append(p)
        else: out.append('  \n'.join(r.strip() for r in rr))
    return '\n\n'.join(out)

def md(b, it):
    t = b['en'] if passa(b) else it
    if not t: return None
    tipo = b['tipo']
    if tipo == 'capitolo': return f"# {b['numero']}. {t}"
    if tipo == 'titolo': return f"{'#' if b['capitolo'] == APPENDICI else '##'} {t}"
    if tipo == 'sottotitolo': return f'### {t}'
    if tipo == 'didascalia': return t if passa(b) else f'*{t}*'
    if tipo == 'pre': return t if t.startswith('|') else f'```text\n{t}\n```'
    return paragrafi(t)

if __name__ == '__main__':
    root = sys.argv[1]; base = f'{root}/docs/traduzione/manuale'
    bb = json.load(open(f'{base}/sorgente.json'))
    tr = {}
    for f in sorted(glob.glob(f'{base}/lotti/m_*.json')):
        for k, v in json.load(open(f)).items():
            if k in tr and tr[k] != v: sys.exit(f'{os.path.basename(f)}: {k} tradotto in due modi')
            tr[k] = v
    mancano = [b for b in bb if not passa(b) and b['id'] not in tr]
    if mancano:
        for b in mancano: print('MANCA', b['id'], b['tipo'], b['en'][:70])
        sys.exit(f'{len(mancano)} blocchi senza traduzione')
    estranei = set(tr) - {b['id'] for b in bb}
    if estranei: sys.exit(f'id non presenti nel sorgente: {sorted(estranei)}')
    it = {b['id']: (b['en'] if passa(b) else tr[b['id']]) for b in bb}
    json.dump(it, open(f'{base}/memoria_it.json', 'w'), indent=1, ensure_ascii=False)
    # figure della scansione del manuale stampato (manuale_figure.py): l'immagine prende il posto della figura
    # ASCII della copia BBS, che resta in un riquadro richiudibile con le etichette tradotte
    fig = {}
    if os.path.exists(f'{base}/figure.json'):
        for f in json.load(open(f'{base}/figure.json'))['figure']: fig.setdefault(f['ancora'], []).append(f)
    def immagine(f, didascalia=None):
        r = f"![{f['didascalia_en']}](../../../{f['file']})"
        d = didascalia or f['didascalia_it']
        return r + (f'\n\n*{d}*' if d else '')
    capitoli = {}; ascii_ = []; usate = set()
    for i, b in enumerate(bb):
        out = capitoli.setdefault(b['capitolo'], [])
        ff = [] if b['id'] in usate else fig.get(b['id'], [])
        usate.add(b['id'])
        if b['tipo'] == 'pre' and not tr.get(b['id'], '').startswith('|'):
            j = i + 1
            while j < len(bb) and bb[j]['tipo'] == 'pre': j += 1
            if j < len(bb) and any(f['dove'] == 'didascalia' for f in fig.get(bb[j]['id'], [])):
                ascii_.append(md(b, tr.get(b['id']))); continue
        out += [immagine(f) for f in ff if f['dove'] == 'prima']
        r = md(b, tr.get(b['id']))
        if any(f['dove'] == 'didascalia' for f in ff):
            out += [immagine(f, tr.get(b['id'])) for f in ff if f['dove'] == 'didascalia']
            if ascii_:
                out.append('<details><summary>Versione testuale con le etichette tradotte</summary>\n\n'
                           + '\n\n'.join(ascii_) + '\n\n</details>')
            ascii_ = []; continue
        if r: out.append(r)
        out += [immagine(f) for f in ff if f['dove'] == 'dopo']
    tutto = []
    for c, rr in sorted(capitoli.items()):
        testo = '\n\n'.join(rr) + '\n'
        open(f'{base}/capitolo_{c:02d}.md', 'w').write(testo)
        tutto.append(testo)
    open(f'{base}/manuale_IT.md', 'w').write('\n---\n\n'.join(tutto))
    visti = set(); en = itw = 0
    for b in bb:
        if b['id'] in visti or passa(b): continue
        visti.add(b['id']); en += len(b['en'].split()); itw += len(it[b['id']].split())
    print(f'capitoli: {len(capitoli)}  blocchi: {len(bb)}  unici: {len(visti)}  parole EN {en} -> IT {itw} ({itw / en:.0%})')
    for b in bb:
        t = re.sub(r'\([^()]*\)', '', it[b['id']])     # i termini originali tra parentesi sono voluti
        for v in VIETATI:
            for m in re.finditer(v, t, re.I):
                print(f"  glossario: cap {b['capitolo']} {b['id']} «{t[max(0, m.start() - 30):m.end() + 30]}»")
