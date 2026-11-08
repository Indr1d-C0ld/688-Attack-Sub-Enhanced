"""Verifica che le stringhe italiane dell'interfaccia entrino negli spazi delle stazioni originali.

Entrate: re/cls/eventi.json (da cls_layout.py: ogni testo disegnato dai file .CLS, con posizione, finestra
e contesto) e docs/traduzione/interfaccia_it.json. Per ogni riga di testo lo spazio disponibile e' il primo
limite a destra fra:
- il riquadro (rettangolo o cornice) che contiene l'inizio del testo: se e' disegnato nella stessa macro
  (un pulsante) il margine destro e' uguale a quello sinistro, altrimenti (pannello) 2 pixel dal bordo;
- un altro testo, una linea verticale, un riquadro, una casella o un'immagine che inizia piu' a destra
  sulla stessa fascia (con 3 pixel di distacco);
- il bordo destro della finestra di testo (oltre il quale l'originale va a capo a meta' parola);
- il bordo dello schermo (319).
Le misure sono in pixel della schermata originale 320x200, con il font di GAME.EXE (le lettere accentate
contano come la lettera base). Nel remake a 640x480 lo spazio raddoppia in orizzontale, ma resta lo stesso
in proporzione, se il nuovo font mantiene le larghezze.

Uscite: docs/traduzione/verifica_spazi.md (rapporto), docs/traduzione/verifica_spazi.json e, per le
stazioni, re/cls/<FILE>_it.png (ridisegno con i testi italiani, in rosso quelli che non entrano).
"""
import os, sys, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cls_layout as CL
from PIL import Image, ImageDraw

DISTACCO = 3

def altezza(t):
    return 7 if any('a' <= c <= 'z' for c in t) else 5

def geometria(eventi):
    g = []   # (tipo, x1, y1, x2, y2, pila)
    for e in eventi:
        a = e['args']; o = e['op']
        if o in ('rettangolo', 'cornice'):
            g.append(('riquadro', min(a[0], a[2]), min(a[1], a[3]), max(a[0], a[2]), max(a[1], a[3]), e['pila']))
        elif o == 'linea_v': g.append(('linea', a[0], min(a[1], a[2]), a[0], max(a[1], a[2]), e['pila']))
        elif o == 'casella_numero': g.append(('casella', a[1], a[2], a[1] + 10, a[2] + 10, e['pila']))
        elif o == 'immagine' and isinstance(a[0], str):
            p = None
            for d in ('png',):
                q = f"{ROOT}/extracted/{d}/{a[0].upper().replace('.ENC', '.png')}"
                if os.path.exists(q): p = Image.open(q).size
            if p: g.append(('immagine', a[1], a[2], a[1] + p[0] - 1, a[2] + p[1] - 1, e['pila']))
        elif 'righe' in e:
            for r in e['righe']:
                if r['testo'].strip(): g.append(('testo', r['x'], r['y'], r['x'] + r['w'], r['y'] + altezza(r['testo']), e['pila']))
    return g

def spazio(e, r, g):
    x, y = r['x'], r['y']; h = altezza(r['testo']); y2 = y + h - 1
    rel = lambda v: v > CL.PAR // 2
    fx2 = e['finestra'][2]
    if rel(x) == rel(fx2): lim, motivo = fx2, 'finestra' if fx2 < 319 else 'schermo'
    else: lim, motivo = (x + 10 ** 6, 'nessuno') if rel(x) else (319, 'schermo')
    cont = [q for q in g if q[0] == 'riquadro' and q[1] <= x < q[3] and q[2] <= y and y2 <= q[4]]
    if cont:
        q = min(cont, key=lambda q: (q[3] - q[1]) * (q[4] - q[2]))
        stessa = q[5] == e['pila'] and x - q[1] <= 12     # pulsante: testo vicino al bordo sinistro, stessa macro
        l = q[3] - (x - q[1]) if stessa else q[3] - 2
        if l < lim: lim, motivo = l, 'pulsante' if stessa else 'riquadro'
    for q in g:
        if q[1] > x and q[2] <= y2 and q[4] >= y and not (q[0] == 'testo' and q[1] == x and q[2] == y):
            l = q[1] - DISTACCO
            if q[0] == 'riquadro' and cont and q == min(cont, key=lambda q: (q[3] - q[1]) * (q[4] - q[2])): continue
            if l < lim: lim, motivo = l, q[0]
    return lim - x + 1, motivo

def traduzioni(it, nome):
    voci = it.get(nome, [])
    def cerca(t):
        s = t.strip()
        for v in voci:
            if v['en'].strip() == s: return v
        for v in voci:
            if s and v['en'].strip().endswith(s) and len(v['en'].strip()) - len(s) <= 1: return v
        return None
    return cerca

def cerca_paragrafi(ev, it, fnt, usate, senza):
    """Messaggi su piu' righe in una finestra di testo (dialoghi, aiuto): il remake va a capo da solo, quindi
    si controlla il paragrafo intero, con le parole italiane ridistribuite nella larghezza e nell'altezza
    della finestra."""
    out = []; dentro = {}
    for nome, pagine in ev['file'].items():
        cerca = traduzioni(it, nome)
        for p, eventi in pagine.items():
            k = 0
            while k < len(eventi):
                gruppo = []
                while k < len(eventi) and eventi[k]['op'] == 'testo' and 'righe' in eventi[k] and eventi[k]['args'][0].endswith('\n'):
                    gruppo.append(k); k += 1
                if gruppo and k < len(eventi) and eventi[k]['op'] == 'testo' and 'righe' in eventi[k]: gruppo.append(k)
                k += 1
                if len(gruppo) < 2: continue
                ee = [eventi[i] for i in gruppo]; fin = ee[0]['finestra']
                righe = [r for e in ee for r in e['righe'] if r['testo'].strip()]
                voci = [cerca(r['testo']) for r in righe]
                if not all(voci):
                    senza.extend((nome, p, r['testo']) for r, v in zip(righe, voci) if not v); continue
                for v in voci: usate.add((nome, v['en']))
                passo = (righe[1]['y'] - righe[0]['y']) if len(righe) > 1 and righe[1]['y'] > righe[0]['y'] else 9
                x0, y0 = righe[0]['x'], righe[0]['y']
                largh = fin[2] - x0 + 1
                n_max = (fin[3] - passo - y0) // passo + 1
                parole = ' '.join(v.get('it_breve', v['it']).strip() for v in voci).split()
                linee = ['']
                for w in parole:
                    prova = (linee[-1] + ' ' + w).strip()
                    if CL.larghezza(prova, fnt, ee[0]['spazio']) <= largh or not linee[-1]: linee[-1] = prova
                    else: linee.append(w)
                troppo_larga = [l for l in linee if CL.larghezza(l, fnt, ee[0]['spazio']) > largh]
                out.append(dict(file=nome, pagina=int(p), en=' / '.join(r['testo'].strip() for r in righe),
                                righe_en=len(righe), righe_it=len(linee), righe_max=n_max, larghezza=largh,
                                it=linee, esito='ok' if len(linee) <= max(n_max, len(righe)) and not troppo_larga else 'non entra'))
                dentro.setdefault((nome, p), set()).update(gruppo)
    return out, dentro

if __name__ == '__main__':
    ROOT = sys.argv[1]
    ev = json.load(open(f'{ROOT}/re/cls/eventi.json'))
    it = json.load(open(f'{ROOT}/docs/traduzione/interfaccia_it.json'))
    img = CL.carica_exe(ROOT); fnt = CL.font(img)
    risultati = []; usate = set(); senza = []; numeri = set()
    paragrafi, nei_paragrafi = cerca_paragrafi(ev, it, fnt, usate, senza)
    for nome, pagine in ev['file'].items():
        cerca = traduzioni(it, nome)
        for p, eventi in pagine.items():
            g = geometria(eventi)
            for k, e in enumerate(eventi):
                for r in e.get('righe', []):
                    t = r['testo']
                    if not t.strip() or k in nei_paragrafi.get((nome, p), ()): continue
                    v = cerca(t)
                    if not v:
                        if any('A' <= c <= 'Z' or 'a' <= c <= 'z' for c in t): senza.append((nome, p, t))
                        else: numeri.add(t.strip())        # cifre, unita' di scala e icone: non si traducono
                        continue
                    usate.add((nome, v['en']))
                    disp, motivo = spazio(e, r, g)
                    forma = v.get('it_breve', v['it'])
                    w_it = CL.larghezza(forma.strip('\n'), fnt, e['spazio'], False, e['rilievo'])
                    risultati.append(dict(file=nome, pagina=int(p), en=t.strip('\n'), it=v['it'], it_breve=v.get('it_breve'),
                                          x=r['x'] % CL.PAR, y=r['y'] % CL.PAR, relativo=r['x'] > CL.PAR // 2, w_en=r['w'],
                                          w_it=w_it, disponibile=disp, limite=motivo, evento=k,
                                          esito='ok' if w_it <= disp else 'non entra'))
    # una riga per testo (lo stesso testo puo' essere disegnato da piu' pagine: si tiene il caso peggiore)
    peggiori = {}
    for r in risultati:
        k = (r['file'], r['en'])
        if k not in peggiori or r['w_it'] - r['disponibile'] > peggiori[k]['w_it'] - peggiori[k]['disponibile']: peggiori[k] = r
    righe = sorted(peggiori.values(), key=lambda r: (r['file'], r['esito'] == 'ok', r['y'], r['x']))
    mai = [(f, v['en']) for f, vv in it.items() if f != '_info' for v in vv if (f, v['en']) not in usate]
    json.dump(dict(testi=righe, paragrafi=paragrafi, non_trovati=senza, non_disegnati=mai), open(f'{ROOT}/docs/traduzione/verifica_spazi.json', 'w'),
              indent=1, ensure_ascii=False)
    # ridisegno delle stazioni con i testi italiani
    for nome, pagine in ev['file'].items():
        eventi = pagine['0']; cerca = traduzioni(it, nome); mappa = {}
        for e in eventi:
            for r in e.get('righe', []):
                v = cerca(r['testo']) if r['testo'].strip() else None
                if v: r['testo'] = v.get('it_breve', v['it']).strip('\n')
        CL.disegna(ROOT, eventi, fnt, f'{ROOT}/re/cls/{nome}_it.png')
        im = Image.open(f'{ROOT}/re/cls/{nome}_it.png'); d = ImageDraw.Draw(im)
        for r in righe:
            if r['file'] == nome and r['pagina'] == 0 and r['esito'] != 'ok' and not r['relativo']:
                d.rectangle([2 * r['x'] - 1, 2 * r['y'] - 1, 2 * (r['x'] + r['w_it']) + 1, 2 * (r['y'] + altezza(r['it'])) + 1], outline=(255, 0, 0))
                d.line([2 * (r['x'] + r['disponibile']), 2 * r['y'] - 3, 2 * (r['x'] + r['disponibile']), 2 * (r['y'] + 8)], fill=(255, 255, 0))
        im.save(f'{ROOT}/re/cls/{nome}_it.png')
    ko = [r for r in righe if r['esito'] != 'ok']
    md = ['# Verifica degli spazi: interfaccia italiana nelle schermate originali', '',
          'Generato da `tools/re/trad_spazi.py` sui layout decodificati da `tools/re/cls_layout.py`. Misure in pixel della',
          'schermata originale 320×200, font di GAME.EXE; *disponibile* è lo spazio fino al primo limite a destra.',
          'Ridisegni con i testi italiani in `re/cls/<FILE>_it.png` (riquadro rosso: non entra; linea gialla: limite).',
          'Dove c\'è, si misura la forma breve (`it_breve` in `interfaccia_it.json`); la forma piena resta per i layout più ampi.', '',
          f'Testi controllati: {len(righe)}. Non entrano: **{len(ko)}**. Righe disegnate senza traduzione trovata: {len(senza)}. '
          f'Voci tradotte mai disegnate dai `.CLS`: {len(mai)}. Testi senza lettere (cifre delle scale, numeri dei battelli, icone del font), che non si traducono: {len(numeri)}.', '']
    for nome in sorted({r['file'] for r in righe}):
        rr = [r for r in righe if r['file'] == nome and r['esito'] != 'ok']
        n = sum(1 for r in righe if r['file'] == nome)
        md += [f'## {nome}: {len(rr)} su {n} non entrano', '']
        if rr:
            md += ['| EN | IT | px EN | px IT | disponibili | eccesso | limite | posizione |', '|---|---|---|---|---|---|---|---|']
            for r in rr:
                pos = f"{'rel. ' if r['relativo'] else ''}{r['x']},{r['y']} (p. {r['pagina']})"
                md.append(f"| {r['en']} | {r['it_breve'] or r['it']} | {r['w_en']} | {r['w_it']} | {r['disponibile']} | {r['w_it'] - r['disponibile']} | {r['limite']} | {pos} |")
            md.append('')
    # paragrafi: uno per testo (lo stesso messaggio puo' stare su piu' pagine)
    visti = {}
    for q in paragrafi: visti.setdefault((q['file'], q['en']), q)
    pp = list(visti.values()); pko = [q for q in pp if q['esito'] != 'ok']
    md += [f'## Messaggi nelle finestre (dialoghi e aiuto): {len(pko)} su {len(pp)} non entrano', '',
           'Il remake va a capo da solo: le parole italiane si ridistribuiscono nella larghezza della finestra e devono',
           'stare nelle righe disponibili.', '']
    if pko:
        md += ['| File | Inizio del testo EN | righe EN | righe IT | righe max |', '|---|---|---|---|---|']
        md += [f"| {q['file']} | {q['en'][:60]}… | {q['righe_en']} | {q['righe_it']} | {q['righe_max']} |" for q in pko] + ['']
    if senza:
        md += ['## Righe disegnate senza traduzione trovata', '', '| File | Pagina | Testo |', '|---|---|---|']
        md += [f'| {f} | {p} | `{t.strip()}` |' for f, p, t in sorted(set(senza))] + ['']
    if mai:
        md += ['## Voci tradotte che i `.CLS` non disegnano', '',
               'Sono stringhe composte dal codice del gioco o residui dell\'inventario; si verificano nel motore.', '']
        md += [f'- {f}: `{e}`' for f, e in mai] + ['']
    open(f'{ROOT}/docs/traduzione/verifica_spazi.md', 'w').write('\n'.join(md))
    print(f'paragrafi {len(pp)}  non entrano {len(pko)}')
    print(f'testi {len(righe)}  non entrano {len(ko)}  senza traduzione {len(senza)}  mai disegnati {len(mai)}')
