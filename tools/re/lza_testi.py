"""Testi dei dialoghi di missione (SCENE*.LZA decompressi) per la traduzione.

Struttura del dialogo: u8 n pagine + offset; ogni pagina = [flag][u16 puntatore] relativo all'ultimo
segmento. Prima della pagina 0 c'e' il preambolo con gli ordini di missione; le pagine 0..18 sono gli
esiti 1..19 (pagina k = esito k+1); la pagina 19 e' la configurazione; l'ultima e' l'intestazione del
rapporto finale. Nel testo: 1D 00 <riga> 00 = riga, una riga di soli spazi = fine paragrafo.

Le righe vengono ricomposte in paragrafi (il nuovo motore andra' a capo da solo); le righe di
intestazione (FM:, TO:, INFO:, SUBJ:, CODE:, classifiche, firma) restano righe singole.

Uso:  lza_testi.py <radice>   ->  docs/traduzione/sorgenti/SCENE*.json e docs/traduzione/memoria_en.json
(memoria: paragrafi unici con id stabile = primi 10 caratteri dello SHA-1 del testo inglese)
"""
import os, re, sys, glob, json, struct, hashlib

# righe d'intestazione: campi del messaggio, classifica, codice d'esito, firma (grado + iniziali)
INTEST = re.compile(r"^\s*(FM:|TO:|INFO:|SUBJ:|CODE:|\([A-Z ]+\)$|\*{3,}|MISSION ORDERS|"
                    r"(RADM|VADM|ADM|VITSE ADMIRAL|KONTR ADMIRAL|ADMIRAL) [A-Z]\.)")

def pid(testo):
    return hashlib.sha1(testo.encode()).hexdigest()[:10]

def righe(seg):
    return [m.group(1).decode('ascii').rstrip() for m in re.finditer(rb'\x1d\x00([\x20-\x7e]*)\x00', seg)]

def paragrafi(rr):
    out, cur = [], []
    def chiudi():
        if cur: out.append(dict(tipo='testo', en=' '.join(cur))); cur.clear()
    for r in rr:
        s = r.strip()
        if not s:
            chiudi(); continue
        if INTEST.match(s):
            chiudi(); out.append(dict(tipo='intestazione', en=re.sub(r'\s+', ' ', s))); continue
        cur.append(re.sub(r'\s+', ' ', s))
    chiudi()
    for p in out:
        p['id'] = pid(p['en'])
    return out

def scena(path):
    d = open(path, 'rb').read(); n = d[0]
    offs = [struct.unpack_from('<H', d, 1 + 2 * i)[0] for i in range(n)]
    base = offs[-1]
    ptr = [struct.unpack_from('<H', d, o + 1)[0] for o in offs[:-1]]
    pagine = [dict(ruolo='ordini', esito=None, paragrafi=paragrafi(righe(d[base:base + ptr[0]])))]
    for k, p in enumerate(ptr):
        fine = base + ptr[k + 1] if k + 1 < len(ptr) else len(d)
        pp = paragrafi(righe(d[base + p:fine]))
        if not pp: continue
        if k <= 18:
            pagine.append(dict(ruolo='esito', esito=k + 1, paragrafi=pp))
        else:
            pagine.append(dict(ruolo='rapporto', esito=None, paragrafi=pp))
    return pagine

if __name__ == '__main__':
    root = sys.argv[1]
    dst = f'{root}/docs/traduzione/sorgenti'; os.makedirs(dst, exist_ok=True)
    memoria = {}; parole = 0
    for f in sorted(glob.glob(f'{root}/extracted/raw_lzw/SCENE*.LZA.bin')):
        nome = os.path.basename(f)[:7]
        pagine = scena(f)
        json.dump(dict(scena=nome, pagine=pagine), open(f'{dst}/{nome}.json', 'w'), indent=1, ensure_ascii=False)
        for pg in pagine:
            for p in pg['paragrafi']:
                if p['id'] not in memoria:
                    memoria[p['id']] = dict(en=p['en'], tipo=p['tipo'], scene=[])
                    parole += len(p['en'].split())
                if nome not in memoria[p['id']]['scene']:
                    memoria[p['id']]['scene'].append(nome)
    json.dump(memoria, open(f'{root}/docs/traduzione/memoria_en.json', 'w'), indent=1, ensure_ascii=False)
    print(f'paragrafi unici: {len(memoria)}  parole uniche: {parole}')
