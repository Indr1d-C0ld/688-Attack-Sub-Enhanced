"""Scarica foto da Wikimedia Commons per il kit delle navi, con la licenza controllata foto per foto.

Accetta solo pubblico dominio, CC0, CC BY e CC BY-SA (le altre si saltano e si dice perche'). Scarica l'originale se e'
largo al piu' 3000 px (o --larghezza), altrimenti la miniatura del server a quella larghezza; se il server da' di piu'
(l'originale, o le miniature a scalini fissi come 3840 px) la riduce qui alla larghezza chiesta. Scrive in una cartella nuova o vuota e aggiunge le
voci a crediti.json della cartella (file, autore, licenza, pagina, originale, dimensioni, byte), nel formato di
extracted/navi/<classe>/foto/crediti.json. I file scaricati sono dati: non si eseguono e non si aprono come codice.

Uso: scarica_foto.py [--larghezza px] <cartella> "File:<titolo>" ...   (predefinita 3000)
"""
import json, os, re, sys, time, urllib.error, urllib.parse, urllib.request

API = 'https://commons.wikimedia.org/w/api.php'
UA = {'User-Agent': 'melchior-688-kit/0.1 (foto di riferimento a licenza libera)'}
LARGHEZZA = 3000


def licenza_ammessa(l):
    l = (l or '').strip().lower()
    return l.startswith('public domain') or l in ('pd', 'cc0') or l.startswith('pd-') or \
        re.fullmatch(r'cc[ -]by(-sa)?( \d\.\d)?( [a-z]{2})?', l) is not None   # anche le versioni nazionali (2.0 de)


def pulisci(s):
    return re.sub(r'<[^>]+>', '', s or '').strip()


def leggi(url, tentativi=6):
    for k in range(tentativi):
        try:
            with urllib.request.urlopen(urllib.request.Request(url, headers=UA), timeout=120) as r:
                dati = r.read()
            time.sleep(2.0)   # pausa di cortesia
            return dati
        except urllib.error.HTTPError as e:
            if e.code != 429 or k == tentativi - 1: raise
            time.sleep(60 * (k + 1))   # il server limita: si aspetta sempre di piu'


def riduci(cartella, nome):
    """Porta a LARGHEZZA px la foto piu' larga (JPEG qualita' 90, PNG com'e'), col nome '<larghezza>px-<nome>'.
    Rende (nome, [larghezza, altezza]) o (nome, None) se non serviva."""
    from PIL import Image
    percorso = os.path.join(cartella, nome)
    im = Image.open(percorso)
    if im.width <= LARGHEZZA: return nome, None
    im = im.resize((LARGHEZZA, round(im.height * LARGHEZZA / im.width)), Image.LANCZOS)
    nuovo = f'{LARGHEZZA}px-' + re.sub(r'^\d+px-', '', nome)
    if nuovo.lower().endswith('.png'): im.save(os.path.join(cartella, nuovo))
    else: im.convert('RGB').save(os.path.join(cartella, nuovo), quality=90)
    if nuovo != nome: os.remove(percorso)
    return nuovo, [im.width, im.height]


def main():
    global LARGHEZZA
    argv = sys.argv[1:]
    if argv[0] == '--larghezza':
        LARGHEZZA = int(argv[1]); argv = argv[2:]
    cartella, titoli = argv[0], argv[1:]
    os.makedirs(cartella, exist_ok=True)
    f_crediti = os.path.join(cartella, 'crediti.json')
    crediti = json.load(open(f_crediti)) if os.path.exists(f_crediti) else {}
    for titolo in titoli:
        q = dict(action='query', format='json', titles=titolo, prop='imageinfo', iiprop='url|size|extmetadata',
                 iiurlwidth=LARGHEZZA)
        pagine = json.loads(leggi(API + '?' + urllib.parse.urlencode(q)))['query']['pages']
        p = next(iter(pagine.values()))
        if 'imageinfo' not in p:
            print(f'NON TROVATA: {titolo}'); continue
        ii = p['imageinfo'][0]; m = ii.get('extmetadata', {})
        lic = m.get('LicenseShortName', {}).get('value')
        if not licenza_ammessa(lic):
            print(f'SALTATA (licenza {lic!r}): {titolo}'); continue
        url = ii['url'] if ii['width'] <= LARGHEZZA or not ii.get('thumburl') else ii['thumburl']
        nome = urllib.parse.unquote(url.split('?', 1)[0].rsplit('/', 1)[1])
        dati = leggi(url)
        open(os.path.join(cartella, nome), 'wb').write(dati)
        nome, ridotta = riduci(cartella, nome)
        if ridotta: dati = open(os.path.join(cartella, nome), 'rb').read()
        crediti[p['title']] = dict(file=nome, autore=pulisci(m.get('Artist', {}).get('value'))[:160] or '?', licenza=lic,
                                   url='https://commons.wikimedia.org/wiki/' + urllib.parse.quote(p['title'].replace(' ', '_')),
                                   originale=ii['url'].split('?', 1)[0], dimensioni=[ii['width'], ii['height']], byte=len(dati))
        if ridotta: crediti[p['title']]['ridotta'] = ridotta
        json.dump(crediti, open(f_crediti, 'w'), ensure_ascii=False, indent=1)   # dopo ogni foto: un errore non perde i crediti
        print(f'{lic} | {len(dati) // 1024} KB | {nome}')
        time.sleep(5.0)


if __name__ == '__main__':
    main()
