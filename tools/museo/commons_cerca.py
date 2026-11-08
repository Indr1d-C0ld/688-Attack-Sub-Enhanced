"""Cerca immagini su Wikimedia Commons e ne riporta licenza, autore, data e descrizione.

Uso: commons_cerca.py "<testo di ricerca>" [max]
Stampa righe JSON con: titolo, licenza, autore/credito, data, dimensioni, url, descrizione.
Solo metadati: nessun file viene scaricato.
"""
import json, sys, urllib.error, urllib.parse, urllib.request, re

API = 'https://commons.wikimedia.org/w/api.php'
UA = {'User-Agent': 'melchior-688-prototipo/0.1 (ricerca immagini di pubblico dominio)'}

def get(params, tentativi=6):
    """Richiesta all'API con pausa di cortesia e nuovi tentativi se il server limita la frequenza (429)."""
    import time
    u = API + '?' + urllib.parse.urlencode(params)
    for k in range(tentativi):
        try:
            with urllib.request.urlopen(urllib.request.Request(u, headers=UA), timeout=60) as r:
                dati = json.load(r)
            time.sleep(2.0)
            return dati
        except urllib.error.HTTPError as e:
            if e.code != 429 or k == tentativi - 1: raise
            time.sleep(20 * (k + 1))

def pulisci(s):
    return re.sub(r'<[^>]+>', '', s or '').strip()

def cerca(q, n=40):
    r = get(dict(action='query', format='json', generator='search', gsrsearch=q, gsrnamespace=6,
                 gsrlimit=n, prop='imageinfo', iiprop='url|size|extmetadata', iiurlwidth=800))
    for p in r.get('query', {}).get('pages', {}).values():
        ii = p['imageinfo'][0]; m = ii.get('extmetadata', {})
        yield dict(titolo=p['title'], licenza=m.get('LicenseShortName', {}).get('value'),
                   autore=pulisci(m.get('Artist', {}).get('value'))[:120],
                   credito=pulisci(m.get('Credit', {}).get('value'))[:160],
                   data=pulisci(m.get('DateTimeOriginal', {}).get('value'))[:40],
                   dim=f"{ii['width']}x{ii['height']}", url=ii['url'], anteprima=ii.get('thumburl'),
                   # versione da 1920 px (miniatura del server, leggera da scaricare) o l'originale se piu' piccolo
                   grande=re.sub(r'/\d+px-', '/1920px-', ii['thumburl']) if ii.get('thumburl') and ii['width'] > 1920 else ii['url'],
                   descrizione=pulisci(m.get('ImageDescription', {}).get('value'))[:300])

if __name__ == '__main__':
    for x in cerca(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 40):
        print(json.dumps(x, ensure_ascii=False))
