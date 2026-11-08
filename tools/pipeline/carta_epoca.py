"""Carte d'epoca dei teatri: la carta nautica di ciascuna parte, derivata dal fondale vero e degradata
(docs/08 §5, dossier 06 §3.3 e §7.4; parametri in tools/pipeline/carte_epoca.yaml, tutte STIME DI GIOCO).

Per teatro e per parte (nato, urss), con un seme fisso ricavato da (teatro, parte):
 1. campagne di rilievo: tessellazione di Voronoi di ogni zona in blocchi di 20-50 nm; ogni campagna ha anno, ente,
    orientazione delle linee, spostamento orizzontale e scarto della velocita' del suono; una parte resta non rilevata
    (copertura) e la fascia costiera avversaria (fascia_negata_km) non si rileva mai;
 2. rilievo sintetico: linee parallele con la spaziatura della zona (fitta sopra `profondita_soglia_m`, rada oltre il
    doppio, linee annidate), campionate ogni cella con un ecoscandaglio a cono (quota massima nell'impronta);
    nelle zone non rilevate solo sondaggi sparsi;
 3. errori: spostamento della campagna (tutti i sondaggi insieme), velocita' del suono +-1-3% per campagna, rumore
    dello 0,5% per sondaggio;
 4. ricostruzione: TIN (interpolazione lineare su triangolazione di Delaunay) dei sondaggi nelle posizioni sbagliate,
    con la linea di costa vera a quota 0: i rilievi piu' stretti della spaziatura spariscono o si spostano;
 5. generalizzazione: passa-basso gaussiano (sigma = 1/4 della spaziatura) che tiene i bassi fondali dei sondaggi
    (si prende il meno profondo fra liscio e TIN, come una carta prudente);
 6. arrotondamento: profondita' in metri interi, terra a passi di 10 m (la carta non e' la topografia);
 7. annotazioni: dove un rilievo vero e' sparito (risale piu' di 20 m e del 20% sopra la carta), con la probabilita'
    della zona, una nota ED, PA, «profondita' minore riportata» o «acqua scolorita» in posizione approssimata.
La terra resta quella vera e il mare non diventa mai terra.

Uscita: data/motore/teatri/<teatro>_carta_<parte>.fdl (FDL2 con la stessa griglia del fondale vero, piu' i blocchi
GEO1, RIL1 e ANN1, formato in scrivi_fdl) e l'anteprima data/teatri/<teatro>/carta_<parte>_anteprima.png
(vero | carta con le zone non rilevate e le annotazioni | differenza).

Uso: carta_epoca.py <radice del progetto> [teatro ...]
"""
import hashlib, json, math, os, struct, sys, zlib
import numpy as np, yaml
from PIL import Image, ImageDraw
from pyproj import Proj
from scipy import ndimage
from scipy.interpolate import LinearNDInterpolator
from scipy.spatial import cKDTree

NM = 1852.0
PARTI = ('nato', 'urss')
TIPI_ANNOTAZIONE = {'ed': 0, 'pa': 1, 'acqua_scolorita': 2, 'profondita_minore': 3}


def scrivi_fdl(percorso, valori, cella, scala, offset, blocchi=()):
    """Scrive un .fdl: "FDL2", u16 larghezza, u16 altezza, f32 cella, f32 scala, f32 offset, u32 byte compressi, i
    valori u16 compressi (scarto dalla predizione a piano sinistra + sopra - diagonale, byte bassi poi alti,
    DEFLATE grezzo), poi i blocchi facoltativi, ciascuno "TAG4" + u32 lunghezza + contenuto (little endian):
      GEO1  f64 latitudine, f64 longitudine del centro della proiezione azimutale equidistante (WGS84, gradi)
      RIL1  u16 anno dell'edizione, u16 campagne; per campagna u16 anno, f32 spaziatura sulle soglie (m),
            f32 spaziatura nei bacini (m), f32 spostamento (m), u8 byte + ente in UTF-8; poi u32 byte compressi
            e larghezza*altezza u8 compressi con DEFLATE grezzo: campagna della cella (0 = non rilevata o terra,
            k = campagna k-1)
      ANN1  u16 annotazioni; per annotazione u8 tipo (0 ED, 1 PA, 2 acqua scolorita, 3 profondita' minore),
            f32 x, f32 y (m, come le riporta la carta), f32 profondita' (m), u16 anno
    Un lettore che non conosce un blocco lo salta."""
    h, w = valori.shape
    v = valori.astype(np.int64)
    a = np.zeros_like(v); a[:, 1:] = v[:, :-1]                     # sinistra
    b = np.zeros_like(v); b[1:] = v[:-1]                           # sopra
    d = np.zeros_like(v); d[1:, 1:] = v[:-1, :-1]                  # diagonale
    r = ((v - (a + b - d)) & 0xffff).astype('<u2')
    dati = deflate(np.stack([r & 255, r >> 8]).astype(np.uint8).tobytes())
    with open(percorso, 'wb') as o:
        o.write(b'FDL2' + struct.pack('<HHfffI', w, h, cella, scala, offset, len(dati)) + dati)
        for tag, contenuto in blocchi:
            o.write(tag + struct.pack('<I', len(contenuto)) + contenuto)


def deflate(byte):
    zc = zlib.compressobj(9, zlib.DEFLATED, -15, 9)
    return zc.compress(byte) + zc.flush()


def blocco_geo(heightmap):
    p = heightmap['proiezione']
    return b'GEO1', struct.pack('<dd', p['centro_lat'], p['centro_lon'])


def seme(teatro, parte):
    return int.from_bytes(hashlib.sha256(f'carta:{teatro}:{parte}'.encode()).digest()[:8], 'little')


class Teatro:
    def __init__(self, radice, t):
        d = f'{radice}/data/teatri/{t}'
        self.id, self.dir = t, d
        self.h = json.load(open(f'{d}/heightmap.json'))
        g, c = self.h['griglia'], self.h['codifica']
        self.w, self.hh, self.cella = g['larghezza'], g['altezza'], g['cella_m']
        self.scala, self.offset = c['scala'], c['offset']
        self.v = np.asarray(Image.open(f'{d}/heightmap_u16.png'))
        assert self.v.shape == (self.hh, self.w)
        self.z = self.v.astype(np.float64) * self.scala + self.offset
        self.terra = self.z >= 0
        p = self.h['proiezione']
        self.proj = Proj(proj='aeqd', lat_0=p['centro_lat'], lon_0=p['centro_lon'], datum='WGS84', units='m')
        self.xs = (np.arange(self.w) + 0.5) * self.cella - self.w * self.cella / 2
        self.ys = self.hh * self.cella / 2 - (np.arange(self.hh) + 0.5) * self.cella

    def colrig(self, lon, lat):
        x, y = self.proj(lon, lat)
        return (np.asarray(x) + self.w * self.cella / 2) / self.cella, (self.hh * self.cella / 2 - np.asarray(y)) / self.cella


def mappa_zone(T, zone):
    """Indice di zona per cella: vince la prima zona il cui poligono contiene la cella; l'ultima e' il resto."""
    zona = np.full((T.hh, T.w), len(zone) - 1, np.int16)
    for i in reversed(range(len(zone))):
        pol = zone[i].get('poligono')
        if not pol: continue
        c, r = T.colrig([q[0] for q in pol], [q[1] for q in pol])
        im = Image.new('L', (T.w, T.hh), 0)
        ImageDraw.Draw(im).polygon(list(zip(c.tolist(), r.tolist())), fill=1)
        zona[np.asarray(im) > 0] = i
    return zona


def massimo_nel_cono(z, raggio_celle):
    """Quota massima nell'impronta quadrata di semilato `raggio_celle` (0..7) attorno a ogni cella."""
    out = z.copy()
    for R in range(1, 8):
        sel = raggio_celle >= R
        if not sel.any(): break
        out[sel] = ndimage.maximum_filter(z, size=2 * R + 1, mode='nearest')[sel]
    return out


def genera_carta(T, conf, pred, parte):
    rng = np.random.default_rng(seme(T.id, parte))
    zone = conf['zone']
    P = [zn[parte] for zn in zone]
    zona = mappa_zone(T, zone)
    mare = ~T.terra
    prof = np.maximum(-T.z, 0.0)
    X, Y = np.meshgrid(T.xs, T.ys)

    # spaziatura locale delle linee: fitta fino alla soglia, rada oltre il doppio (interpolazione geometrica)
    fine = np.array([p['spaziatura_km'][0] * 1000 for p in P])[zona]
    rada = np.array([p['spaziatura_km'][1] * 1000 for p in P])[zona]
    soglia = conf['profondita_soglia_m']
    f = np.clip(np.log2(np.maximum(prof, 1.0) / soglia), 0.0, 1.0)
    S = fine * (rada / fine) ** f

    # fascia costiera negata
    dist_terra = ndimage.distance_transform_edt(~T.terra) * T.cella
    negata = np.zeros_like(mare)
    for i, p in enumerate(P):
        if p.get('fascia_negata_km'):
            negata |= (zona == i) & (dist_terra < p['fascia_negata_km'] * 1000)

    # campagne: Voronoi di semi casuali per zona, con blocchi di 20-50 nm di lato
    b0, b1 = pred['blocco_nm']
    blocco = np.full((T.hh, T.w), -1, np.int32)
    campagne = []
    for i, p in enumerate(P):
        celle = np.flatnonzero(zona == i)
        if celle.size == 0: continue
        lato = rng.uniform(b0, b1) * NM
        n = max(1, int(round(celle.size * T.cella ** 2 / lato ** 2)))
        semi = rng.choice(celle, size=n, replace=False)
        _, k = cKDTree(np.c_[X.flat[semi], Y.flat[semi]]).query(np.c_[X.flat[celle], Y.flat[celle]])
        blocco.flat[celle] = len(campagne) + k
        for _ in range(n):
            s0, s1 = p['spostamento_nm']
            mod, ang = rng.uniform(s0, s1) * NM, rng.uniform(0, 2 * math.pi)
            v0, v1 = pred['sigma_velocita_suono']
            campagne.append(dict(
                zona=i, anno=int(rng.integers(p['anni'][0], p['anni'][1] + 1)), ente=p['ente'],
                fine=p['spaziatura_km'][0] * 1000, rada=p['spaziatura_km'][1] * 1000, spostamento=mod,
                dx=mod * math.sin(ang), dy=mod * math.cos(ang),
                bias=rng.choice([-1, 1]) * rng.uniform(v0, v1),
                teta=rng.uniform(0, math.pi), fase=rng.uniform(0, 1),
                rilevata=bool(rng.random() < p['copertura'])))
    C = campagne
    dx = np.array([c['dx'] for c in C])[blocco]; dy = np.array([c['dy'] for c in C])[blocco]
    bias = np.array([c['bias'] for c in C])[blocco]
    teta = np.array([c['teta'] for c in C])[blocco]; fase = np.array([c['fase'] for c in C])[blocco]
    rilevata = np.array([c['rilevata'] for c in C])[blocco] & ~negata

    # linee di rilievo: u = coordinata trasversale; linea k ogni `fine` metri, tenuta se k e' multiplo di m
    u = X * np.cos(teta) + Y * np.sin(teta)
    q = u / fine + fase
    k = np.floor(q + 0.5)
    sulla_linea = np.abs(q - k) * fine < T.cella / 2
    m = 2 ** np.floor(np.log2(np.maximum(S / fine, 1.0)) + 0.5)
    linea = mare & rilevata & sulla_linea & (np.mod(k, m) == 0)

    # ecoscandaglio a cono: quota massima nell'impronta (minima profondita')
    raggio = np.clip(np.round(prof * math.tan(math.radians(pred['cono_gradi'])) / T.cella), 0, 7)
    zmax = massimo_nel_cono(T.z, raggio)
    sel = np.flatnonzero(linea)
    misura = np.maximum(-zmax.flat[sel], 0.5) * (1 + bias.flat[sel]) * (1 + rng.normal(0, pred['rumore_profondita'], sel.size))
    px, py = X.flat[sel] + dx.flat[sel], Y.flat[sel] + dy.flat[sel]

    # sondaggi sparsi nelle zone non rilevate (carte vecchie, passaggi isolati), posizione incerta di ~0,5 nm
    lontano = np.flatnonzero(mare & ~rilevata)
    n_sparsi = int(lontano.size * T.cella ** 2 / (pred['sondaggi_sparsi_km'] * 1000) ** 2)
    sp = rng.choice(lontano, size=min(n_sparsi, lontano.size), replace=False) if lontano.size else np.array([], int)
    misura_sp = prof.flat[sp] * (1 + rng.normal(0, 0.02, sp.size))
    spx = X.flat[sp] + rng.normal(0, 0.5 * NM, sp.size); spy = Y.flat[sp] + rng.normal(0, 0.5 * NM, sp.size)

    # costa vera a quota 0 (celle di terra che toccano il mare) e bordo della griglia dal fondale vero liscio, perche'
    # la triangolazione copra tutto
    costa = T.terra & ndimage.binary_dilation(mare)
    ic = np.flatnonzero(costa)
    bordo = np.zeros_like(mare); bordo[::max(1, T.hh - 1), ::8] = True; bordo[::8, ::max(1, T.w - 1)] = True
    liscio = ndimage.gaussian_filter(np.minimum(T.z, 0), 10)
    ib = np.flatnonzero(bordo & mare)
    pts = np.r_[np.c_[px, py], np.c_[spx, spy], np.c_[X.flat[ic], Y.flat[ic]], np.c_[X.flat[ib], Y.flat[ib]]]
    val = np.r_[-misura, -misura_sp, np.zeros(ic.size), liscio.flat[ib]]
    tin = LinearNDInterpolator(pts, val)(X[mare], Y[mare])
    if np.isnan(tin).any():   # fuori dall'inviluppo: il punto piu' vicino
        buchi = np.isnan(tin)
        _, j = cKDTree(pts).query(np.c_[X[mare][buchi], Y[mare][buchi]])
        tin[buchi] = val[j]
    zt = np.zeros_like(T.z); zt[mare] = tin

    # generalizzazione: liscio con sigma = S/4 (in celle), poi il meno profondo fra liscio e TIN
    sig = S / 4 / T.cella
    livelli = [0.5, 1, 2, 4, 8, 16]
    lisci = [ndimage.gaussian_filter(zt, s, mode='nearest') for s in livelli]
    ls = np.clip(np.log2(np.maximum(sig, 0.5)) + 1, 0, len(livelli) - 1)
    i0 = np.floor(ls).astype(int); i1 = np.minimum(i0 + 1, len(livelli) - 1); fr = ls - i0
    L = np.stack(lisci)
    g = np.take_along_axis(L, i0[None], 0)[0] * (1 - fr) + np.take_along_axis(L, i1[None], 0)[0] * fr
    carta = np.where(mare, np.minimum(np.maximum(g, zt), -0.5), T.z)

    # campagna per cella: quella del sondaggio di linea piu' vicino, se entro 1,5 volte la spaziatura
    camp = np.zeros((T.hh, T.w), np.uint8)
    usate = sorted({int(b) for b in np.unique(blocco.flat[sel])})
    assert len(usate) < 255
    nuovo = np.zeros(len(C), np.uint8)
    nuovo[usate] = np.arange(1, len(usate) + 1)
    if sel.size:
        im = np.flatnonzero(mare)
        dist, j = cKDTree(np.c_[px, py]).query(np.c_[X.flat[im], Y.flat[im]])
        ok = dist <= 1.5 * S.flat[im]
        camp.flat[im[ok]] = nuovo[blocco.flat[sel][j[ok]]]
    tabella = [C[b] for b in usate]

    annotazioni = genera_annotazioni(T, carta, zona, P, C, blocco, prof, pred, rng)
    return carta, camp, tabella, annotazioni, dict(linee=int(sel.size), sparsi=int(sp.size), campagne=len(C))


def genera_annotazioni(T, carta, zona, P, C, blocco, prof, pred, rng):
    diff = T.z - carta   # positivo dove il fondo vero e' piu' alto della carta
    mare = ~T.terra
    cand = mare & (diff > np.maximum(20.0, 0.2 * prof))
    lab, n = ndimage.label(cand, structure=np.ones((3, 3)))
    if n == 0: return []
    idx = ndimage.maximum_position(np.where(cand, T.z, -1e9), lab, index=np.arange(1, n + 1))
    cands = []
    for (r, c) in idx:
        pr = prof[r, c]
        cands.append(((diff[r, c] / max(pr, 10.0)) * (1.0 if pr < 600 else 0.3), r, c))
    cands.sort(reverse=True)
    out = []
    for _, r, c in cands:
        if len(out) >= pred['massimo_annotazioni']: break
        p = P[zona[r, c]]
        if rng.random() >= p['annotazioni']: continue
        pr = prof[r, c]
        tipo = rng.choice(['ed', 'pa', 'profondita_minore'])
        if pr < 20 and rng.random() < 0.4: tipo = 'acqua_scolorita'
        x, y = T.xs[c], T.ys[r]
        cb = C[blocco[r, c]]
        if tipo == 'pa': s = rng.uniform(0.5, 2.0) * NM
        elif tipo == 'ed': s = rng.uniform(0.2, 1.0) * NM
        else: s = rng.uniform(0.0, 0.3) * NM
        a = rng.uniform(0, 2 * math.pi)
        x += cb['dx'] + s * math.sin(a); y += cb['dy'] + s * math.cos(a)
        if abs(x) > T.w * T.cella / 2 or abs(y) > T.hh * T.cella / 2: continue   # nota fuori dal foglio
        profondita = 0.0 if tipo == 'acqua_scolorita' else float(round(pr * (1 + rng.normal(0, 0.05))))
        anno = int(rng.integers(p['anni'][0], p['anni'][1] + 1))
        out.append(dict(tipo=tipo, x=float(x), y=float(y), profondita=profondita, anno=anno, vera=(float(T.xs[c]), float(T.ys[r]), float(pr))))
    return out


def blocchi_carta(T, edizione, camp, tabella, annotazioni):
    ril = struct.pack('<HH', edizione, len(tabella))
    for c in tabella:
        ente = c['ente'].encode('utf-8')[:255]
        ril += struct.pack('<HfffB', c['anno'], c['fine'], c['rada'], c['spostamento'], len(ente)) + ente
    zc = deflate(camp.tobytes())
    ril += struct.pack('<I', len(zc)) + zc
    ann = struct.pack('<H', len(annotazioni))
    for a in annotazioni:
        ann += struct.pack('<BfffH', TIPI_ANNOTAZIONE[a['tipo']], a['x'], a['y'], a['profondita'], a['anno'])
    return [blocco_geo(T.h), (b'RIL1', ril), (b'ANN1', ann)]


def colori(z):
    """Tinte batimetriche per l'anteprima: azzurri piu' scuri con la profondita', terra in ocra."""
    d = np.clip(-z, 0, 3000) / 3000
    d = np.sqrt(d)
    img = np.zeros(z.shape + (3,), np.uint8)
    img[..., 0] = (40 * (1 - d)).astype(np.uint8)
    img[..., 1] = (210 * (1 - d) + 30).astype(np.uint8)
    img[..., 2] = (255 * (1 - 0.6 * d)).astype(np.uint8)
    img[z >= 0] = (200, 170, 100)
    # isobate ogni 100 m (ogni 20 m sotto i 200 m)
    for passo, lim in ((20, 200), (100, 1e9)):
        q = np.floor(np.minimum(-z, lim) / passo)
        bordo = (np.diff(q, axis=0, prepend=q[:1]) != 0) | (np.diff(q, axis=1, prepend=q[:, :1]) != 0)
        img[bordo & (z < 0)] = (0, 40, 90)
    return img


def anteprima(T, carta, camp, annotazioni, percorso, rid=2):
    vero = colori(T.z)
    c = colori(carta)
    nr = (camp == 0) & (carta < 0)
    tratteggio = ((np.add.outer(np.arange(T.hh), np.arange(T.w)) // 6) % 2 == 0)
    c[nr & tratteggio] = (c[nr & tratteggio] * 0.55 + np.array([120, 120, 120]) * 0.45).astype(np.uint8)
    diff = np.where(T.terra, 0.0, carta - T.z)
    s = np.clip(diff / 100.0, -1, 1)
    dimg = np.full(T.z.shape + (3,), 255, np.uint8)
    dimg[..., 0] = (255 * (1 - np.maximum(-s, 0))).astype(np.uint8)   # blu: carta piu' profonda del vero
    dimg[..., 2] = (255 * (1 - np.maximum(s, 0))).astype(np.uint8)    # rosso: carta meno profonda
    dimg[..., 1] = (255 * (1 - np.abs(s))).astype(np.uint8)
    dimg[T.terra] = (200, 170, 100)
    pannelli = [Image.fromarray(a).resize((T.w // rid, T.hh // rid), Image.BILINEAR) for a in (vero, c, dimg)]
    out = Image.new('RGB', (3 * (T.w // rid) + 20, T.hh // rid + 24), (255, 255, 255))
    dr = ImageDraw.Draw(out)
    for i, p in enumerate(pannelli):
        out.paste(p, (i * (T.w // rid + 10), 24))
    dr.text((4, 6), f'{T.id}: fondale vero', fill=(0, 0, 0))
    dr.text((T.w // rid + 14, 6), 'carta d\'epoca (tratteggio: non rilevato; magenta: annotazioni)', fill=(0, 0, 0))
    dr.text((2 * (T.w // rid + 10) + 4, 6), 'carta - vero (rosso: carta meno profonda, blu: piu\' profonda, +-100 m)', fill=(0, 0, 0))
    x0 = T.w // rid + 10
    for a in annotazioni:
        cx = (a['x'] + T.w * T.cella / 2) / T.cella / rid + x0
        cy = (T.hh * T.cella / 2 - a['y']) / T.cella / rid + 24
        dr.ellipse((cx - 3, cy - 3, cx + 3, cy + 3), outline=(255, 0, 255), width=2)
        vx = (a['vera'][0] + T.w * T.cella / 2) / T.cella / rid + x0
        vy = (T.hh * T.cella / 2 - a['vera'][1]) / T.cella / rid + 24
        dr.line((cx, cy, vx, vy), fill=(255, 0, 255))
    out.save(percorso)


def esporta_carte(radice, teatri=None, anteprime=True):
    conf = yaml.safe_load(open(f'{radice}/tools/pipeline/carte_epoca.yaml'))
    pred = conf['predefiniti']
    os.makedirs(f'{radice}/data/motore/teatri', exist_ok=True)
    fatte = []
    for t in sorted(conf['teatri']):
        if teatri and t not in teatri: continue
        if not os.path.exists(f'{radice}/data/teatri/{t}/heightmap.json'): continue
        T = Teatro(radice, t)
        for parte in PARTI:
            carta, camp, tabella, ann, stat = genera_carta(T, conf['teatri'][t], pred, parte)
            # profondita' in metri interi come sulle carte, terra a passi di 10 m: la carta pesa un terzo di meno
            carta = np.where(T.terra, np.maximum(np.round(T.z / 10) * 10, 0), np.minimum(np.round(carta), -0.5))
            v = np.clip(np.round((carta - T.offset) / T.scala), 0, 65535).astype(np.uint16)
            percorso = f'{radice}/data/motore/teatri/{t}_carta_{parte}.fdl'
            scrivi_fdl(percorso, v, T.cella, T.scala, T.offset, blocchi_carta(T, pred['edizione'][parte], camp, tabella, ann))
            if anteprime:
                anteprima(T, v.astype(np.float64) * T.scala + T.offset, camp, ann, f'{T.dir}/carta_{parte}_anteprima.png')
            mare = ~T.terra
            err = np.abs(carta - T.z)[mare]
            print(f'{t} {parte}: {stat["linee"]} sondaggi di linea, {stat["sparsi"]} sparsi, {len(tabella)}/{stat["campagne"]} '
                  f'campagne rilevate, non rilevato {np.mean(camp[mare] == 0):.0%}, errore medio {err.mean():.1f} m '
                  f'(95% {np.percentile(err, 95):.0f} m), {len(ann)} annotazioni, {os.path.getsize(percorso) // 1024} KB')
            fatte.append(f'{t}_carta_{parte}')
    return fatte


if __name__ == '__main__':
    radice = sys.argv[1] if len(sys.argv) > 1 else '.'
    esporta_carte(radice, sys.argv[2:] or None)
