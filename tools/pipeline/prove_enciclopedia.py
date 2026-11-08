"""Prove del generatore dell'enciclopedia (tools/pipeline/enciclopedia.py): coerenza coi cataloghi del motore, nessuna
voce o riga senza fonte, riferimenti risolti, immagini esistenti, foto con licenza libera, indice completo e tutto scritto.
Uso: prove_enciclopedia.py <radice del progetto>   (esce con errore alla prima prova fallita)"""
import os, re, sys, tomllib

import yaml

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from enciclopedia import LICENZE_LIBERE, MISSILI_CLASSI, esporta_enciclopedia   # noqa: E402

CONF = {'alta', 'media', 'bassa', 'STIMA'}
FATTORI = (1.0, 1 / 1000.0, 1 / 60.0, 1 / 3600.0, 1 / 1852.0, 2.0, 100.0)   # m -> km, s -> min / h, m -> nm, semi-ampiezza, %


def prova(radice):
    esporta_enciclopedia(radice)
    D = os.path.join(radice, 'data', 'motore')
    enc = tomllib.load(open(os.path.join(D, 'enciclopedia.toml'), 'rb'))
    cat = {f: tomllib.load(open(os.path.join(D, f + '.toml'), 'rb')) for f in
           ('classi', 'carico', 'siluri', 'acustica', 'emettitori', 'asw_aereo', 'periscopi', 'meteo')}
    teatri = {t['id'] for t in yaml.safe_load(open(os.path.join(radice, 'tools', 'pipeline', 'teatri.yaml')))['teatri']}
    ind, voci = enc['indice'], enc['voci']
    errori = []

    # l'indice copre i cataloghi
    for c in cat['classi']:
        if c not in MISSILI_CLASSI and not any(k.endswith('.' + c) for k in ind): errori.append(f'classe senza voce: {c}')
    for sez, cats in (('armi', None), ('mine', 'mina')):
        for a in cat['carico'][sez]:
            if not any(k.endswith('.' + a) for k in ind): errori.append(f'carico [{sez}] senza voce: {a}')
    for s in cat['siluri']['siluri']:
        if f'siluro.{s}' not in ind: errori.append(f'siluro senza voce: {s}')
    for b in cat['asw_aereo']['boe']:
        if f'boa.{b}' not in ind: errori.append(f'boa senza voce: {b}')
    for p in cat['periscopi']['periscopi']:
        if f'periscopio.{p}' not in ind: errori.append(f'periscopio senza voce: {p}')
    for t in teatri:
        if f'teatro.{t}' not in ind: errori.append(f'teatro senza voce: {t}')

    # completezza: ogni voce dell'indice e' scritta
    for vid, i in ind.items():
        if not i.get('scritta'): errori.append(f'voce senza testo: {vid}')

    for vid, v in voci.items():
        # testi e fonti
        for k in ('testo_it', 'testo_en', 'fonti'):
            if not v.get(k): errori.append(f'{vid}: manca {k}')
        if v['testo_it'] == v['testo_en']: errori.append(f'{vid}: testo italiano uguale all\'inglese')
        if v['categoria'] in ('siluro', 'mina', 'esca', 'missile') and not (v.get('uso_it') and v.get('uso_en')):
            errori.append(f'{vid}: arma senza l\'uso in gioco')
        if not 2 <= len(re.findall(r'[.!?](\s|$)', v['testo_it'])) <= 7: errori.append(f'{vid}: testo di lunghezza insolita')
        # scheda: fonte, confidenza, dato del catalogo e valore coerente
        if not v.get('scheda'): errori.append(f'{vid}: scheda vuota')
        for r in v.get('scheda', []):
            if not r.get('fonte'): errori.append(f'{vid}/{r["campo"]}: senza fonte')
            if r.get('conf') not in CONF: errori.append(f'{vid}/{r["campo"]}: confidenza {r.get("conf")}')
            if not r.get('testo_it') or not r.get('testo_en'): errori.append(f'{vid}/{r["campo"]}: testo mancante')
            if 'gioco_valore' in r and not r.get('gioco_fonte'): errori.append(f'{vid}/{r["campo"]}: valore del gioco senza fonte')
            m = re.match(r'(\w+)\.toml \[([^\]]+)\] (\w+)$', r.get('dato_gioco', r['dato']))
            if 'gioco_valore' in r: r = dict(r, valore=r['gioco_valore'], dato=r['dato_gioco'])
            if m and m.group(1) in cat and isinstance(r['valore'], (int, float)) and not isinstance(r['valore'], bool):
                nodo = cat[m.group(1)]
                for p in m.group(2).split('.'):
                    nodo = nodo.get(p, {}) if isinstance(nodo, dict) else {}
                v_cat = nodo.get(m.group(3)) if isinstance(nodo, dict) else None
                if isinstance(v_cat, list): v_cat = v_cat[-1] if r['campo'] == 'ingrandimenti' else v_cat[0]
                if not isinstance(v_cat, (int, float)):
                    errori.append(f'{vid}/{r["campo"]}: il dato {r["dato"]} non e\' nel catalogo')
                elif not any(abs(r['valore'] - v_cat * f) < 1e-6 * max(1.0, abs(v_cat)) for f in FATTORI):
                    errori.append(f'{vid}/{r["campo"]}: {r["valore"]} diverso dal catalogo ({v_cat})')
        # riferimenti risolti
        for x in v.get('riferimenti', []):
            if x['voce'] not in ind: errori.append(f'{vid}: riferimento a {x["voce"]} fuori dall\'indice')
        # immagini esistenti, foto con licenza libera e col credito
        for im in v.get('immagini', []):
            if im['tipo'] in ('sprite', 'modello', 'foto', 'carta') and not os.path.exists(os.path.join(D, im['file'])):
                errori.append(f'{vid}: immagine mancante {im["file"]}')
            if im['tipo'] == 'cascata' and (im.get('classe') not in cat['acustica']['firme'] or
                                            cat['acustica']['sensori'].get(im.get('sensore'), {}).get('modo') != 'passivo'):
                errori.append(f'{vid}: cascata senza firma o senza sensore passivo')
            if im['tipo'] == 'spaccato' and not os.path.exists(os.path.join(D, 'scafi', im['classe'] + '.toml')):
                errori.append(f'{vid}: spaccato senza scafo')
            if im['tipo'] == 'foto':
                if not any(l in im.get('licenza', '').lower() for l in LICENZE_LIBERE): errori.append(f'{vid}: foto senza licenza libera')
                if not v.get('crediti'): errori.append(f'{vid}: foto senza crediti')
                d = open(os.path.join(D, im['file']), 'rb').read()
                if d[:4] != b'IMG1' or len(d) != 8 + 320 * 200 or not all(144 <= b <= 158 for b in d[8:]):
                    errori.append(f'{vid}: foto non nei 16 grigi 320x200')
        for c in v.get('crediti', []):
            if not (c.get('autore') or c.get('titolo')) or not c.get('licenza'): errori.append(f'{vid}: credito incompleto')
    return errori, len(ind), len(voci)


if __name__ == '__main__':
    errori, n_ind, n_voci = prova(sys.argv[1] if len(sys.argv) > 1 else '.')
    for e in errori: print('ERRORE', e)
    print(f'indice {n_ind}, voci {n_voci}: ' + ('tutto a posto' if not errori else f'{len(errori)} errori'))
    sys.exit(1 if errori else 0)
