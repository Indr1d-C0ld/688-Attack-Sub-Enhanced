"""Esporta il modello di una classe per il motore (sprite generati al volo per le navi danneggiate,
docs/10_danni_affondamento_esplosioni.md §5.1).

Il modello e' quello del kit con l'opera viva (costruisci.modello_intero). Il colore di ogni triangolo viene
"cotto" in una tessitura dalla stessa proiezione delle foto che produce gli sprite: per ogni texel si salvano il
valore delle foto (media pesata) e la quota delle foto rispetto all'ombreggiatura di ripiego. Le due facce di
ogni triangolo hanno tessiture separate, come negli sprite, dove una foto colora solo la faccia rivolta
all'osservatore che la vede anch'essa; la faccia che nessuna foto vede ha un texel solo. Il motore rifa'
l'ombreggiatura con la stessa formula di costruisci._rendi, quindi a nave intatta il disegno coincide con lo
sprite precalcolato. Le fotocamere sono quelle gia' calibrate (verifica.json): niente nuova calibrazione.

Uso: esporta_modello.py <radice progetto> <classe.yaml> [--verifica]
Scrive <uscita della classe>/modello.mdl e, con --verifica, verifica_modello.png e il confronto con gli sprite.

Formato .mdl (little endian):
  "MDL2", u32 triangoli, u32 larghezza e u32 altezza della tessitura, f32 lato del texel (m),
  f32 tono, f32 luce ambiente, f32 lunghezza fuori tutto, f32 quota massima del modello sopra il galleggiamento
  (senza opera viva), u16 parti, per parte u8 lunghezza e nome (UTF-8);
  poi tre blocchi, ciascuno u32 byte compressi e dati DEFLATE grezzi (RFC 1951, senza intestazione zlib):
  1. i record dei triangoli per piani di byte (prima il byte 0 di tutti i record, poi il byte 1...). Record di 91
     byte: 9 f32 (vertici x, y, z del kit: x a prua, y a sinistra, z in alto, galleggiamento a 0), 6 f32
     (coordinate di tessitura in texel della faccia diretta, quella della normale (v1-v0)x(v2-v0)), 6 f32 (faccia
     opposta), f32 albedo, u8 flag (bit 0 retinato), u16 parte;
  2. larghezza*altezza u8 quota delle foto (0..255);
  3. il valore delle foto (u8) dei soli texel con quota > 0, in ordine di riga, come scarto (mod 256) dalla
     predizione MED (`predizione`); gli altri texel valgono 0.
  Senza perdite rispetto alla tessitura quantizzata a 8 bit: la compressione toglie gli spazi vuoti
  dell'impacchettamento e le facce che le foto non vedono (203 MB in MDL1, 39 MB in MDL2 per le 26 classi).
"""
import json, os, struct, sys, time, zlib
import numpy as np, yaml
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import costruisci, raster
from foto import Foto

TEXEL = float(os.environ.get('M688_TEXEL', 0.15))  # m: gli sprite hanno 3,75 px/m (0,27 m), campionati 3x3; con 0,15 m
                                                    # il 98% dei pixel resta entro un livello di grigio dallo sprite
BORDO = 1            # texel attorno a ogni triangolo, per l'interpolazione bilineare
LARGHEZZA = 4096

def impacchetta(tri, con_foto=None):
    """Riquadro di tessitura per ogni triangolo (a scaffali): origine, base locale e coordinate dei vertici.
    I triangoli che nessuna foto vede (`con_foto` falso: quasi tutta l'opera viva) hanno un solo texel."""
    v0 = tri[:, 0]
    e1 = tri[:, 1] - v0
    l1 = np.linalg.norm(e1, axis=1)
    eu = e1 / np.maximum(l1, 1e-9)[:, None]
    e2 = tri[:, 2] - v0
    ev = e2 - (e2 * eu).sum(1)[:, None] * eu
    lv = np.linalg.norm(ev, axis=1)
    ev = ev / np.maximum(lv, 1e-9)[:, None]
    a = np.stack([np.zeros(len(tri)), l1, (e2 * eu).sum(1)], 1)       # coordinate locali dei vertici (m)
    b = np.stack([np.zeros(len(tri)), np.zeros(len(tri)), lv], 1)
    amin, bmin = a.min(1), b.min(1)
    w = np.ceil((a.max(1) - amin) / TEXEL).astype(int) + 2 * BORDO
    h = np.ceil((b.max(1) - bmin) / TEXEL).astype(int) + 2 * BORDO
    passo = np.full(len(tri), TEXEL)
    if con_foto is not None:
        w[~con_foto] = h[~con_foto] = 1
        passo[~con_foto] = 1e9    # tutte le coordinate nel texel unico
    if (w > LARGHEZZA).any(): raise ValueError('triangolo piu\' largo della tessitura')
    ordine = np.argsort(-h, kind='stable')
    ox = np.zeros(len(tri), int); oy = np.zeros(len(tri), int)
    x = y = alto = 0
    for i in ordine:
        if x + w[i] > LARGHEZZA: x, y, alto = 0, y + alto, 0
        ox[i], oy[i] = x, y
        x += w[i]; alto = max(alto, h[i])
    H = y + alto
    bordo = np.where(passo < 1e8, BORDO, 0.5)
    uv = np.stack([ox[:, None] + bordo[:, None] + (a - amin[:, None]) / passo[:, None],
                   oy[:, None] + bordo[:, None] + (b - bmin[:, None]) / passo[:, None]], 2)   # (N, 3, 2)
    return dict(v0=v0, eu=eu, ev=ev, amin=amin, bmin=bmin, w=w, h=h, ox=ox, oy=oy, uv=uv, W=LARGHEZZA, H=H,
                passo=passo)

def facce(mesh):
    """Le due facce di ogni triangolo: triangoli ripetuti, normali (diretta, poi opposta), materiali, parti."""
    T = mesh.tri
    n = np.cross(T[:, 1] - T[:, 0], T[:, 2] - T[:, 0]); n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    return np.concatenate([T, T]), np.concatenate([n, -n]), mesh.mat * 2, mesh.parte * 2

def cuoci(tri, n, mat, parte, foto, pk, peso_ombra):
    """Valore delle foto e loro quota per ogni texel delle facce (stessi pesi di costruisci._rendi: una foto
    colora una faccia solo se la vede dal lato della sua normale)."""
    W, H = pk['W'], pk['H']
    val = np.zeros((H, W), np.float32); mix = np.zeros((H, W), np.float32)
    T = tri
    retinato = np.array([m == 'retinato' for m in mat])
    parte = np.array(parte)
    # i triangoli a blocchi, per contenere la memoria
    idx = np.arange(len(T)); blocco = []
    def svuota(blocco):
        k = np.concatenate([np.full(pk['w'][i] * pk['h'][i], i) for i in blocco])
        ii = np.concatenate([np.tile(np.arange(pk['w'][i]), pk['h'][i]) for i in blocco])
        jj = np.concatenate([np.repeat(np.arange(pk['h'][i]), pk['w'][i]) for i in blocco])
        ps = np.minimum(pk['passo'][k], 1e3)      # texel unico: il punto e' il primo vertice
        bo = np.where(pk['passo'][k] < 1e8, BORDO, 0.5)
        A = pk['amin'][k] + (ii + 0.5 - bo) * ps
        Bq = pk['bmin'][k] + (jj + 0.5 - bo) * ps
        P = pk['v0'][k] + A[:, None] * pk['eu'][k] + Bq[:, None] * pk['ev'][k]
        nn = n[k]
        num = np.zeros(len(P)); den = np.zeros(len(P))
        for f in foto:
            v, w, nave = f.campiona(P, nn)
            w = np.where(nave & ~retinato[k], w, 0.0)
            escluse = f.cfg.get('escludi_parti')
            if escluse: w = np.where(np.isin(parte[k], escluse), 0.0, w)
            num += w * v; den += w
        yy = pk['oy'][k] + jj; xx = pk['ox'][k] + ii
        val[yy, xx] = np.where(den > 0, num / np.maximum(den, 1e-12), 0.0)
        mix[yy, xx] = den / (den + peso_ombra)
    somma = 0
    for i in idx:
        blocco.append(i); somma += pk['w'][i] * pk['h'][i]
        if somma > 400_000:
            svuota(blocco); blocco, somma = [], 0
    if blocco: svuota(blocco)
    return val, mix

def quantizza(val, mix):
    """Tessiture a 8 bit come nel file; il valore e' 0 dove la quota delle foto lo e' (il formato non lo salva)."""
    q_mix = np.clip(np.round(mix * 255), 0, 255).astype(np.uint8)
    q_val = np.where(q_mix > 0, np.clip(np.round(val * 255), 0, 255), 0).astype(np.uint8)
    return q_val, q_mix

def predizione(val, vista):
    """Predizione MED (LOCO-I) di ogni texel dai vicini a sinistra (a), sopra (b) e in diagonale (c), se visti;
    altrimenti dal vicino a sinistra, da quello sopra o 128. La rifa' identica Modello::carica."""
    v = val.astype(np.int16)
    a = np.zeros_like(v); a[:, 1:] = v[:, :-1]
    b = np.zeros_like(v); b[1:] = v[:-1]
    c = np.zeros_like(v); c[1:, 1:] = v[:-1, :-1]
    va = np.zeros_like(vista); va[:, 1:] = vista[:, :-1]
    vb = np.zeros_like(vista); vb[1:] = vista[:-1]
    vc = np.zeros_like(vista); vc[1:, 1:] = vista[:-1, :-1]
    med = np.where(c >= np.maximum(a, b), np.minimum(a, b), np.where(c <= np.minimum(a, b), np.maximum(a, b), a + b - c))
    return np.where(va & vb & vc, med, np.where(va, a, np.where(vb, b, 128)))

def deflate(dati):
    z = zlib.compressobj(9, zlib.DEFLATED, -15, 9)
    b = z.compress(dati) + z.flush()
    return struct.pack('<I', len(b)) + b

def scrivi(file, mesh, pk, q_val, q_mix, cl, L, quota_max):
    N = len(mesh.tri)
    parti = sorted(set(mesh.parte))
    indice = {p: i for i, p in enumerate(parti)}
    tab = dict(costruisci.ALBEDO, **(cl.get('albedo') or {}))
    with open(file, 'wb') as f:
        f.write(b'MDL2' + struct.pack('<IIIfffff', len(mesh.tri), pk['W'], pk['H'], TEXEL, cl.get('tono', 0.45),
                                      cl.get('luce_ambiente', costruisci.LUCE_AMBIENTE), L, quota_max))
        f.write(struct.pack('<H', len(parti)))
        for p in parti:
            b = p.encode('utf-8')[:255]; f.write(struct.pack('<B', len(b)) + b)
        rec = np.zeros(N, dtype=[('v', '<f4', 9), ('uv', '<f4', 6), ('uv_opposta', '<f4', 6), ('alb', '<f4'),
                                 ('flag', 'u1'), ('parte', '<u2')])
        rec['v'] = mesh.tri.reshape(-1, 9)
        rec['uv'] = pk['uv'][:N].reshape(-1, 6); rec['uv_opposta'] = pk['uv'][N:].reshape(-1, 6)
        rec['alb'] = [tab[m] for m in mesh.mat]
        rec['flag'] = [1 if m == 'retinato' else 0 for m in mesh.mat]
        rec['parte'] = [indice[p] for p in mesh.parte]
        f.write(deflate(np.frombuffer(rec.tobytes(), np.uint8).reshape(N, rec.itemsize).T.tobytes()))
        f.write(deflate(q_mix.tobytes()))
        vista = q_mix > 0
        f.write(deflate(((q_val.astype(np.int16) - predizione(q_val, vista)) & 255).astype(np.uint8)[vista].tobytes()))

def rendi_cotto(mesh, pk, val, mix, cl, L, quota_max, aob, lato, ss=3):
    """Disegno dal modello cotto con la camera degli sprite (stessa formula di _rendi): per la verifica.
    L e quota_max sono quelli del modello senza opera viva, come in costruisci.rendi."""
    alto = max(46.0, quota_max * 1.08 + 2.0)
    cam = costruisci.camera_vista(aob, lato, 3.75, ss, L, alto=alto)
    tid, l0, l1, _ = raster.gbuffer(cam, mesh.tri, sopra_acqua=False)
    P = raster.posizioni(mesh.tri, tid, l0, l1, sopra_acqua=False)
    pix = (tid >= 0) & (P[..., 2] >= 0)
    k = tid[pix]; b0, b1 = l0[pix], l1[pix]; b2 = 1 - b0 - b1
    T = mesh.tri; n = np.cross(T[:, 1] - T[:, 0], T[:, 2] - T[:, 0]); n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    Pp = P[pix]; nn = n[k]; verso = cam.C - Pp
    opposta = (nn * verso).sum(1) < 0
    nn = np.where(opposta[:, None], -nn, nn)
    uv = pk['uv'][k + np.where(opposta, len(T), 0)]
    u = uv[:, 0, 0] * b0 + uv[:, 1, 0] * b1 + uv[:, 2, 0] * b2 - 0.5
    v = uv[:, 0, 1] * b0 + uv[:, 1, 1] * b1 + uv[:, 2, 1] * b2 - 0.5
    u0 = np.clip(np.floor(u).astype(int), 0, pk['W'] - 2); v0 = np.clip(np.floor(v).astype(int), 0, pk['H'] - 2)
    fu, fv = np.clip(u - u0, 0, 1), np.clip(v - v0, 0, 1)
    def bil(t):
        return (t[v0, u0] * (1 - fu) * (1 - fv) + t[v0, u0 + 1] * fu * (1 - fv) + t[v0 + 1, u0] * (1 - fu) * fv
                + t[v0 + 1, u0 + 1] * fu * fv)
    vf, mx = bil(val), bil(mix)
    sole = (0.55, 0.35, 0.75)
    s = sole[0] * (-cam.fw) + sole[1] * cam.r + sole[2] * np.array([0, 0, 1.0]); s /= np.linalg.norm(s)
    tab = dict(costruisci.ALBEDO, **(cl.get('albedo') or {}))
    alb = np.array([tab[m] for m in mesh.mat])[k]
    amb = cl.get('luce_ambiente', costruisci.LUCE_AMBIENTE); tono = cl.get('tono', 0.45)
    ombra = np.clip(alb * (amb + (1 - amb) * np.clip(nn @ s, 0, 1)) * (0.85 + 0.15 * np.clip(Pp[:, 2] / 15, 0, 1)) / 0.62 * tono, 0, 1)
    out = np.full(tid.shape, np.nan); out[pix] = (1 - mx) * ombra + mx * vf
    Hh, Ww = tid.shape; h2, w2 = Hh // ss, Ww // ss
    blk = out[:h2 * ss, :w2 * ss].reshape(h2, ss, w2, ss)
    cop = (~np.isnan(blk)).mean(axis=(1, 3))
    with np.errstate(all='ignore'):
        import warnings
        with warnings.catch_warnings():
            warnings.simplefilter('ignore', RuntimeWarning)
            m = np.nanmean(blk, axis=(1, 3))
    return np.where(cop >= 0.4, m, np.nan)

if __name__ == '__main__':
    root, fcl = sys.argv[1], sys.argv[2]; verifica = '--verifica' in sys.argv
    t0 = time.time()
    cl = yaml.safe_load(open(fcl)); base = os.path.dirname(os.path.abspath(fcl))
    out = os.path.join(root, cl['uscita'])
    mesh = costruisci.costruisci_mesh(cl, base)
    ver = json.load(open(f'{out}/verifica.json'))
    foto = []
    for fc in cl['foto']:
        f = Foto(costruisci.cfg_foto(cl, fc), root)
        f.usa_fotocamera(mesh, ver['foto'][f.nome]['fotocamera'])
        foto.append(f)
    foto += [f.speculare() for f in list(foto) if f.cfg.get('specchiabile')]
    intero = costruisci.modello_intero(cl, base)
    tri2, n2, mat2, parte2 = facce(intero)
    # prima passata grossolana: quali facce vede almeno una foto
    T0 = TEXEL; TEXEL = 0.5
    pk0 = impacchetta(tri2)
    _, mix0 = cuoci(tri2, n2, mat2, parte2, foto, pk0, cl.get('peso_ombra', 0.12))
    con_foto = np.array([mix0[pk0['oy'][i]:pk0['oy'][i] + pk0['h'][i], pk0['ox'][i]:pk0['ox'][i] + pk0['w'][i]].max() > 0
                         for i in range(len(tri2))])
    TEXEL = T0
    pk = impacchetta(tri2, con_foto)
    q_val, q_mix = quantizza(*cuoci(tri2, n2, mat2, parte2, foto, pk, cl.get('peso_ombra', 0.12)))
    val, mix = q_val / 255.0, q_mix / 255.0     # la verifica usa le tessiture del file
    L, quota_max = float(np.ptp(mesh.tri[..., 0])), float(mesh.tri[..., 2].max())
    scrivi(f'{out}/modello.mdl', intero, pk, q_val, q_mix, cl, L, quota_max)
    print(f"{cl['nome']}: {len(intero.tri)} triangoli ({len(intero.tri) - len(mesh.tri)} dell'opera viva, "
          f"{con_foto.sum()} facce viste dalle foto su {len(con_foto)}), "
          f"tessitura {pk['W']}x{pk['H']}, {os.path.getsize(f'{out}/modello.mdl') / 1e6:.1f} MB in {time.time() - t0:.0f} s")
    if verifica:
        pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
        ramp = np.array([pal[3 * i] for i in range(144, 159)], float)
        righe = []
        for aob, nome, lato in ((90, 'dritta', 1), (30, 'sinistra', -1), (150, 'dritta', 1)):
            img = rendi_cotto(intero, pk, val, mix, cl, L, quota_max, aob, lato)
            k, anc = costruisci.sprite_ancorato(img, ramp)
            rif = np.asarray(Image.open(f'{out}/sprite/aob_{aob:03d}_{nome}.png'))
            a_rif = json.load(open(f'{out}/sprite/ancore.json'))['ancore'][f'aob_{aob:03d}_{nome}']
            # i due sprite su una tela comune, con le ancore sovrapposte
            Hc, Wc = max(k.shape[0], rif.shape[0]) + 40, max(k.shape[1], rif.shape[1]) + 40
            def tela(sp, an):
                t = np.full((Hc, Wc), 159); y0, x0 = 20 + int(round(max(anc[1], a_rif[1]) - an[1])), 20 + int(round(max(anc[0], a_rif[0]) - an[0]))
                t[y0:y0 + sp.shape[0], x0:x0 + sp.shape[1]] = sp; return t
            a, b = tela(k.astype(int), anc), tela(rif.astype(int), a_rif)
            entrambi = (a != 159) & (b != 159)
            sag = ((a != 159) & (b != 159)).sum() / max(((a != 159) | (b != 159)).sum(), 1)
            print(f"  aob {aob:3d} {nome:8s}: sprite {k.shape} contro {rif.shape}, ancora {anc[0]:.1f},{anc[1]:.1f} "
                  f"contro {a_rif[0]},{a_rif[1]}; sagoma {sag:.3f}, scarto medio {np.abs(a - b)[entrambi].mean():.2f} "
                  f"indici, entro 1: {(np.abs(a - b)[entrambi] <= 1).mean():.1%}")
            righe.append(np.concatenate([np.pad(b, ((0, 0), (0, 10)), constant_values=159), a], 1))
        Wm = max(r.shape[1] for r in righe)
        im = Image.fromarray(np.concatenate([np.pad(r, ((0, 6), (0, Wm - r.shape[1])), constant_values=159) for r in righe]).astype(np.uint8), 'P')
        im.putpalette(pal); im.save(f'{out}/verifica_modello.png', transparency=159)
