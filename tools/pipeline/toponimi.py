"""Toponimi delle carte dei teatri: mari, golfi, stretti, isole, capi, città e basi, coi punti sul piano del teatro.

Fonti:
- Natural Earth 1:10m (pubblico dominio, naturalearthdata.com), in tools/toponimi/grezzi/naturalearth (LEGGIMI.md):
  aree marine (ne_10m_geography_marine_polys), regioni fisiche (ne_10m_geography_regions_points), località
  (ne_10m_populated_places). Il nome è quello italiano di Natural Earth, o l'inglese, corretto con le voci di
  `correzioni` di toponimi_aggiunte.yaml (glossario della traduzione e nomi del 1989);
- toponimi_aggiunte.yaml: le basi navali e i nomi d'epoca che Natural Earth non ha, a mano, ognuno con la fonte.

Il punto di un'area marina è il punto d'acqua più lontano dalla riva e dal bordo della carta dentro l'area, sul fondale
vero del teatro (la trasformata della distanza su una griglia di 1 km); un'area che nel teatro ha meno di 150 km²
d'acqua resta fuori. Il rango (1 il più importante) decide a che scala il nome compare: la carta li dirada da sola.

Uscita: data/motore/teatri/<teatro>.toponimi.toml, letto da grafica/carta_vga (disegna_toponimi).

Uso: toponimi.py <radice del progetto> [teatro ...]       con --elenco stampa e non scrive
"""
import math, pathlib, struct, sys

import numpy as np, yaml
from PIL import Image, ImageDraw
from scipy import ndimage

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from carta_epoca import Teatro   # noqa: E402  (stessa griglia e proiezione del fondale)

TEATRI = ('baltico', 'barents', 'celtica', 'faroe', 'gibilterra', 'kunashir', 'otranto')
NE = 'tools/toponimi/grezzi/naturalearth'
FONTE_NE = 'Natural Earth 1:10m (pubblico dominio)'
PASSO = 5   # celle da 200 m per la griglia delle aree marine: 1 km
AREA_MIN_KM2 = 150.0
TIPI_MARINI = {'sea': 'mare', 'gulf': 'golfo', 'bay': 'baia', 'strait': 'stretto', 'channel': 'canale',
               'sound': 'stretto', 'fjord': 'fiordo', 'inlet': 'baia', 'lagoon': 'laguna', 'ocean': 'oceano',
               'basin': 'mare', 'reef': 'secca', 'river': 'fiume', 'generic': 'mare'}
TIPI_REGIONI = {'island': 'isola', 'island group': 'isole', 'peninsula': 'penisola', 'cape': 'capo',
                'isthmus': 'istmo', 'coast': 'costa', 'plain': 'regione', 'lowland': 'regione', 'delta': 'regione',
                'range/mtn': 'monti', 'plateau': 'regione', 'pen/cape': 'capo', 'peninsulas': 'penisola'}


# --- lettura di shapefile e dbf (formato ESRI, senza librerie) ---------------------------------------------------

def leggi_dbf(percorso):
    b = pathlib.Path(percorso).read_bytes()
    n, testa, lung = struct.unpack('<4xIHH', b[:12])
    campi, o = [], 32
    while b[o] != 0x0D:
        campi.append((b[o:o + 11].split(b'\0')[0].decode('ascii'), b[o + 16]))
        o += 32
    righe = []
    for i in range(n):
        r, p, d = b[testa + i * lung: testa + (i + 1) * lung], 1, {}
        for nome, l in campi:
            d[nome.lower()] = r[p:p + l].decode('utf-8', 'replace').strip()
            p += l
        righe.append(d)
    return righe


def leggi_poligoni(percorso):
    """Per record, la lista degli anelli [(lon, lat), ...] (tipo 5, poligono)"""
    b = pathlib.Path(percorso).read_bytes()
    o, forme = 100, []
    while o < len(b):
        _, lung = struct.unpack('>ii', b[o:o + 8])
        c = b[o + 8: o + 8 + 2 * lung]
        o += 8 + 2 * lung
        if struct.unpack('<i', c[:4])[0] != 5:
            forme.append([])
            continue
        np_, npt = struct.unpack('<ii', c[36:44])
        parti = list(struct.unpack(f'<{np_}i', c[44:44 + 4 * np_])) + [npt]
        punti = np.frombuffer(c[44 + 4 * np_: 44 + 4 * np_ + 16 * npt], '<f8').reshape(-1, 2)
        forme.append([punti[parti[k]:parti[k + 1]] for k in range(np_)])
    return forme


# --- nomi ------------------------------------------------------------------------------------------------------

def nome_di(r, correzioni):
    it = r.get('name_it') or ''
    en = r.get('name_en') or r.get('name') or ''
    testo = correzioni.get(it) or correzioni.get(en) or it or en
    return testo[:1].upper() + testo[1:], correzioni.get('en:' + en, en)


def dentro(T, x, y, margine=0.0):
    mx, my = T.w * T.cella / 2 - margine, T.hh * T.cella / 2 - margine
    return -mx <= x <= mx and -my <= y <= my


def acqua_grossa(T):
    """Maschera del mare sulla griglia da 1 km (vero se almeno metà delle celle è acqua)"""
    h, w = T.hh // PASSO, T.w // PASSO
    m = (~T.terra[:h * PASSO, :w * PASSO]).reshape(h, PASSO, w, PASSO).mean(axis=(1, 3))
    return m >= 0.5


def riquadro_geografico(T):
    """Longitudini e latitudini minime e massime del teatro, dal suo bordo, con mezzo grado di margine"""
    mx, my = T.w * T.cella / 2, T.hh * T.cella / 2
    s = np.linspace(-1.0, 1.0, 41)
    xs = np.concatenate([s * mx, s * mx, np.full_like(s, -mx), np.full_like(s, mx)])
    ys = np.concatenate([np.full_like(s, -my), np.full_like(s, my), s * my, s * my])
    lon, lat = T.proj(xs, ys, inverse=True)
    return lon.min() - 0.5, lon.max() + 0.5, lat.min() - 0.5, lat.max() + 0.5


def aree_marine(T, mare, correzioni):
    lo0, lo1, la0, la1 = riquadro_geografico(T)
    righe = leggi_dbf(f'{NE}/ne_10m_geography_marine_polys/ne_10m_geography_marine_polys.dbf')
    forme = leggi_poligoni(f'{NE}/ne_10m_geography_marine_polys/ne_10m_geography_marine_polys.shp')
    h, w = mare.shape
    out = []
    for r, anelli in zip(righe, forme):
        if not anelli:
            continue
        im = Image.new('L', (w, h), 0)
        disegna, tocca = ImageDraw.Draw(im), False
        for k, a in enumerate(anelli):
            if a[:, 0].max() < lo0 or a[:, 0].min() > lo1 or a[:, 1].max() < la0 or a[:, 1].min() > la1:
                continue   # lontano dal teatro: la proiezione azimutale lo ripiegherebbe dentro
            c, rr = T.colrig(a[:, 0], a[:, 1])
            if not np.all(np.isfinite(c)):
                continue
            c, rr = c / PASSO, rr / PASSO
            if c.max() < 0 or c.min() > w or rr.max() < 0 or rr.min() > h:
                continue
            tocca = True
            disegna.polygon(list(zip(c.tolist(), rr.tolist())), fill=1 if k == 0 else 0)
        if not tocca:
            continue
        zona = (np.asarray(im) > 0) & mare
        area = zona.sum() * (PASSO * T.cella / 1000.0) ** 2
        if area < AREA_MIN_KM2:
            continue
        bordo = np.pad(zona, 1)   # il bordo della carta conta come riva: il nome non ci finisce sopra
        d = ndimage.distance_transform_edt(bordo)[1:-1, 1:-1]
        i, j = np.unravel_index(np.argmax(d), d.shape)
        x = (j + 0.5) * PASSO * T.cella - T.w * T.cella / 2
        y = T.hh * T.cella / 2 - (i + 0.5) * PASSO * T.cella
        testo, en = nome_di(r, correzioni)
        tipo = TIPI_MARINI.get(r['featurecla'].lower(), 'mare')
        rango = int(float(r['scalerank'] or 5))
        out.append(dict(testo=testo, en=en, tipo=tipo, acqua=True, rango=max(1, min(10, rango)), x=x, y=y,
                        area_km2=round(area), fonte=FONTE_NE))
    return out


def regioni(T, correzioni):
    out = []
    for r in leggi_dbf(f'{NE}/ne_10m_geography_regions_points/ne_10m_geography_regions_points.dbf'):
        x, y = T.proj(float(r['long_x']), float(r['lat_y']))
        if not dentro(T, x, y, 2000.0):
            continue
        testo, en = nome_di(r, correzioni)
        tipo = TIPI_REGIONI.get(r['featurecla'].lower(), 'regione')
        out.append(dict(testo=testo, en=en, tipo=tipo, acqua=False, rango=max(1, min(10, int(float(r['scalerank'] or 5)))),
                        x=x, y=y, fonte=FONTE_NE))
    return out


def localita(T, correzioni):
    out = []
    for r in leggi_dbf(f'{NE}/ne_10m_populated_places/ne_10m_populated_places.dbf'):
        x, y = T.proj(float(r['longitude']), float(r['latitude']))
        if not dentro(T, x, y, 2000.0):
            continue
        testo, en = nome_di(r, correzioni)
        # le città contano meno dei mari e delle basi: dal rango 4 (una capitale) in giù
        out.append(dict(testo=testo, en=en, tipo='citta', acqua=False, rango=max(4, min(10, int(float(r['scalerank'] or 8)) + 1)),
                        x=x, y=y, fonte=FONTE_NE))
    return out


def terra_in(T, x, y, raggio_m=1000.0):
    """Quota del punto: vero se c'è terra entro il raggio (per le coste e le isole piccole)"""
    c, r = (x + T.w * T.cella / 2) / T.cella, (T.hh * T.cella / 2 - y) / T.cella
    k = int(raggio_m / T.cella)
    i0, i1 = max(0, int(r) - k), min(T.hh, int(r) + k + 1)
    j0, j1 = max(0, int(c) - k), min(T.w, int(c) + k + 1)
    return bool(T.terra[i0:i1, j0:j1].any()), bool((~T.terra[i0:i1, j0:j1]).any())


def aggiunte(T, voci):
    out = []
    for v in voci or []:
        x, y = T.proj(v['lon'], v['lat'])
        if not dentro(T, x, y):
            print(f"  avviso: {v['testo']} fuori dal teatro {T.id}", file=sys.stderr)
            continue
        terra, acqua = terra_in(T, x, y)
        if v['tipo'] != 'isole' and (v.get('acqua', False) and not acqua or not v.get('acqua', False) and not terra):
            print(f"  avviso: {v['testo']} ({T.id}) cade {'a terra' if not acqua else 'in mare'}", file=sys.stderr)
        out.append(dict(testo=v['testo'], en=v.get('en', v['testo']), tipo=v['tipo'], acqua=v.get('acqua', False),
                        rango=v.get('rango', 3), x=x, y=y, fonte=v['fonte']))
    return out


def toml(teatro, voci):
    righe = [f'# Toponimi del teatro {teatro}: generato da tools/pipeline/toponimi.py, non modificare a mano.',
             '# x, y in metri sul piano del teatro (come il fondale); rango 1 il più importante; acqua = nome di mare.', '']
    for v in voci:
        righe += ['[[nome]]', f'testo = "{v["testo"]}"', f'en = "{v["en"]}"', f'tipo = "{v["tipo"]}"',
                  f'acqua = {"true" if v["acqua"] else "false"}', f'rango = {v["rango"]}',
                  f'x = {v["x"]:.0f}', f'y = {v["y"]:.0f}', f'fonte = "{v["fonte"]}"', '']
    return '\n'.join(righe)


def main():
    argomenti = [a for a in sys.argv[1:] if not a.startswith('--')]
    radice = pathlib.Path(argomenti[0] if argomenti else '.').resolve()
    teatri = argomenti[1:] or TEATRI
    elenco = '--elenco' in sys.argv
    agg = yaml.safe_load((radice / 'tools/pipeline/toponimi_aggiunte.yaml').read_text(encoding='utf-8'))
    correzioni = agg.get('correzioni', {})
    escludi = set(agg.get('escludi', []))
    global NE
    NE = str(radice / NE)
    for t in teatri:
        T = Teatro(str(radice), t)
        mare = acqua_grossa(T)
        voci = aree_marine(T, mare, correzioni) + regioni(T, correzioni) + localita(T, correzioni) + \
            aggiunte(T, agg.get('teatri', {}).get(t))
        voci = [v for v in voci if v['testo'] and v['testo'] not in escludi and f'{t}:{v["testo"]}' not in escludi]
        visti, uniche = set(), []
        for v in sorted(voci, key=lambda v: (v['rango'], v['testo'])):   # un nome una volta sola, il più importante
            if v['testo'] not in visti:
                visti.add(v['testo'])
                uniche.append(v)
        if elenco:
            print(f'== {t}: {len(uniche)}')
            for v in uniche:
                print(f"  {v['rango']:2d} {v['tipo']:9s} {v['testo']:32s} ({v['en']}) {v['x'] / 1000:7.1f} {v['y'] / 1000:7.1f} km")
            continue
        (radice / f'data/motore/teatri/{t}.toponimi.toml').write_text(toml(t, uniche), encoding='utf-8')
        print(f'{t}: {len(uniche)} toponimi')


if __name__ == '__main__':
    main()
