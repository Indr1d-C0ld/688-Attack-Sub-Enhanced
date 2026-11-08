"""HELICOP.ENC: non e' un'animazione, ma la base dati della protezione anticopia (SEND CODE, sala radio).

Il file e' compresso in LZW (stesso formato dei .LZW: u16 codici, u16 lunghezza = 6398 byte) e
decompresso contiene una lista di comandi nel formato .CLS (tools/re/cls_layout.py), con 90 voci.
GAME.EXE (1000:6444 circa) lo carica nello slot 29h (1000:F59F) ed esegue la pagina
contatore % 89 + 1 (1000:EF73). Ogni pagina chiama la macro 89, che stampa a telescrivente (primitiva 29,
1000:606E: finestra a x 17, y 89, interlinea 5) le righe
"ESTABLISHING SATELLITE UPLINK....", "SATELLITE UPLINK ESTABLISHED", "SECURITY ACCESS KEYCODE FOLLOWS:",
poi il nome di un battello della classe 688 e un frammento di frase del manuale che finisce con "???";
infine tre comandi 11 (1000:EE9D) scrivono le tre lettere attese nella tabella puntata da DS:D31B.
La risposta sono le prime tre lettere della parola che segue il frammento nel paragrafo del manuale
marcato con quel battello.

Uso:  protezione.py <radice>  ->  docs/originale/protezione_codici.json e .md (con verifica sul manuale)
"""
import os, re, sys, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cls_layout as CL
from lzw import decompress

def voci(root):
    img = CL.carica_exe(root); fnt = CL.font(img)
    f, n = decompress(open(f'{root}/extracted/raw/DISK2/HELICOP.ENC', 'rb').read())
    assert len(f) == n
    out = []
    for p in range(1, f[0] + 1):
        na, h, _ = CL.voce(f, 0, p - 1); e = CL.Esecutore(img, fnt, f, 'HELICOP'); e.esegui(e.base + h, [0] * na, [p - 1])
        righe = [x['args'][0] for x in e.eventi if x['op'] == 'n29']
        lettere = [x['args'] for x in e.eventi if x['op'] == 'tabella']
        if not lettere: continue                     # voce 90: la macro comune
        testo = ' '.join(righe[3:])
        m = re.match(r"([-A-Z .']+):\s*(.*)$", testo)
        out.append(dict(pagina=p, battello=m.group(1).strip(), frammento=m.group(2).replace('???', '').strip(),
                        risposta=''.join(chr(c) for i, c in sorted(lettere))))
    return [r for r in out], righe[:3]

def testo_normale(t):
    t = re.sub(r'\n\s*Seite \d+\s*\n\s*688 Attack Sub\s*\n', '\n', t)
    t = re.sub(r'\n\s{30,}\d{1,3}\s*\n', '\n', t)
    t = re.sub(r'-{70,}', ' ', t)
    t = re.sub(r'(\w)[-\xad]\s*\n\s*(\w)', r'\1\2', t)
    t = t.replace('\u2019', "'").replace('\u2014', ' ').replace('--', ' ').replace('-', ' ')
    return re.sub(r' +', ' ', re.sub(r"[^A-Z0-9' \[\]]", ' ', t.upper()))

def cerca(testo, v):
    """Il frammento dopo il marcatore del battello (entro 3000 caratteri), altrimenti la prima occorrenza."""
    fr = re.sub(r' +', ' ', re.sub(r"[^A-Z0-9' ]", ' ', v['frammento'].upper().replace('-', ' '))).strip()
    b = re.sub(r'[^A-Z ]', ' ', v['battello'].replace('-', ' ')).strip(); b = re.sub(r' +', ' ', b)
    starts = [m.end() for m in re.finditer(r'\[?\b' + re.escape(b) + r'\b\]?', testo)]
    trovata = None
    for s0 in starts:                                  # il marcatore puo' comparire piu' volte (anche nell'esempio)
        k = testo.find(fr + ' ', s0, s0 + 3000)
        if k >= 0:
            w = testo[k + len(fr) + 1:].split(' ')[0]
            if w[:3] == v['risposta']: return w, 'dopo il marcatore'
            trovata = trovata or w
    if trovata: return trovata, 'dopo il marcatore'
    k = testo.find(fr + ' ')
    return (testo[k + len(fr) + 1:].split(' ')[0], 'altrove') if k >= 0 else (None, None)

def verifica(root, vv):
    """Prima la copia BBS del manuale, poi l'OCR della stampa (extracted/manuale_stampa/ocr_djvu.txt)."""
    bbs = testo_normale(open(f'{root}/docs/originale/manuale_EN.txt', encoding='utf-8', errors='replace').read())
    p = f'{root}/extracted/manuale_stampa/ocr_djvu.txt'
    stampa = testo_normale(open(p, encoding='utf-8', errors='replace').read()) if os.path.exists(p) else ''
    for v in vv:
        for nome, t in (('copia BBS', bbs), ('stampa (OCR)', stampa)):
            parola, dove = cerca(t, v)
            if parola and parola[:3] == v['risposta']:
                v.update(parola_manuale=parola, fonte=nome, dove=dove, verifica='ok'); break
        else:
            v.update(parola_manuale=parola, verifica='non confermata')
    return vv

if __name__ == '__main__':
    root = sys.argv[1]
    vv, intestazione = voci(root); vv = verifica(root, vv)
    json.dump(dict(intestazione=intestazione, voci=vv), open(f'{root}/docs/originale/protezione_codici.json', 'w'),
              indent=1, ensure_ascii=False)
    ok = sum(v['verifica'] == 'ok' for v in vv)
    md = ['# Protezione anticopia: le 89 domande di SEND CODE', '',
          'Estratte da `HELICOP.ENC` con `tools/re/protezione.py`. Il gioco ne sceglie una a caso; la risposta sono le',
          'prime tre lettere della parola che segue il frammento nel paragrafo del manuale marcato con il battello.',
          f'Verifica sulla copia BBS del manuale: {ok} su {len(vv)} risposte coincidono.', '',
          '| Pagina | Battello | Frammento | Risposta | Parola nel manuale | Verifica |', '|---|---|---|---|---|---|']
    md += [f"| {v['pagina']} | {v['battello']} | {v['frammento']} ??? | {v['risposta']} | {v.get('parola_manuale') or '—'} | "
           f"{v['verifica'] + (' (' + v['fonte'] + ')' if v.get('fonte') else '')} |" for v in vv]
    open(f'{root}/docs/originale/protezione_codici.md', 'w').write('\n'.join(md) + '\n')
    print(f'voci {len(vv)}  verificate {ok}')
    for v in vv:
        if v['verifica'] != 'ok': print('  ', v['pagina'], v['battello'], '|', v['frammento'], '|', v['risposta'], '|', v['verifica'])
