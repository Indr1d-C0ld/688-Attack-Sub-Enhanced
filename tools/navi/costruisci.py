"""Costruisce una classe di nave dal suo file YAML: modello dal kit, calibrazione delle foto,
sprite da periscopio a ogni angolo sulla prua (entrambi i lati), verifiche e fogli di controllo.

Uso: costruisci.py <radice progetto> <classe.yaml> [passo_gradi] [--uscita <cartella>]

L'uscita e' `uscita` del file della classe, relativa alla radice (o assoluta). Con --uscita (per le prove: una
calibrazione nuova senza toccare gli sprite della classe) va nella cartella data, relativa alla cartella corrente o
assoluta; la classe resta quella del file (l'opera viva, i nomi).
"""
import inspect, json, os, sys, time, warnings
import numpy as np, yaml
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kit, raster
from camera import Camera
from foto import Foto

ALBEDO = dict(scafo=0.50, bagnasciuga=0.12, sovrastruttura=0.62, ponte=0.42, traliccio=0.35, scuro=0.30,
              bianco=0.92, nero=0.08, retinato=0.60, carena=0.30, elica=0.40)
# 'carena' ed 'elica': opera viva (tools/navi/opera_viva.py), solo nel modello esportato per il motore
# 'retinato': dischi di eliche e rotori in movimento. Niente tessitura dalle foto (solo ombreggiatura), esclusi dalla
# calibrazione, e disegnati a scacchiera sopra cio' che sta dietro (semitrasparenti, come i rotori degli sprite 1989)
SOTTILI = {'traliccio', 'scuro'}

def risolvi(v, XB):
    """Valori di parametro {prua: d} -> coordinata x = L/2 - d (anche dentro liste)."""
    if isinstance(v, dict) and set(v) == {'prua'}: return XB - v['prua']
    if isinstance(v, list): return [risolvi(a, XB) for a in v]
    return v

def parti_disponibili(cl, base_dir):
    """Parti del kit piu' quelle dei moduli dichiarati dalla classe (`moduli_parti`: file .py, percorsi
    relativi alla cartella del file della classe, che definiscono un dizionario PARTI). Cosi' una classe
    puo' aggiungere parti proprie senza modificare kit.py."""
    import importlib.util
    parti = dict(kit.PARTI)
    for f in cl.get('moduli_parti', []):
        path = os.path.join(base_dir, f)
        spec = importlib.util.spec_from_file_location(os.path.splitext(os.path.basename(path))[0], path)
        mod = importlib.util.module_from_spec(spec); spec.loader.exec_module(mod)
        parti.update(mod.PARTI)
    return parti

def costruisci_mesh(cl, base_dir='.'):
    s = dict(cl['scafo']); XB = s['L'] / 2
    PARTI = parti_disponibili(cl, base_dir)
    # `generatore`: parte che sostituisce kit.scafo (es. scafo_composto), con gli stessi parametri della sezione
    gen = s.pop('generatore', None)
    m = (PARTI[gen](**s) if gen else kit.scafo(**s)).nome('scafo')
    for p in cl['parti']:
        p = dict(p); tipo = p.pop('tipo'); nome = p.pop('nome', tipo)
        args = {k: risolvi(v, XB) for k, v in p.items()}
        # `materiali: {da: a}` ricolora la parte; `mat` su una parte che non lo accetta = materiali {sovrastruttura: mat}
        rimappa = dict(args.pop('materiali', {}) or {})
        if tipo == 'asta':
            parte = kit.asta(args['da'], args['a'], args.get('spessore', 0.25), args.get('mat', 'traliccio'))
        else:
            f = PARTI[tipo]
            if 'mat' in args and 'mat' not in inspect.signature(f).parameters:
                rimappa.setdefault('sovrastruttura', args.pop('mat'))
            parte = f(**args)
        if rimappa:
            parte = kit.Mesh(parte.tri, [rimappa.get(x, x) for x in parte.mat], parte.parte)
        m = m + parte.nome(nome)
    return m

def cfg_foto(cl, fc):
    """Configurazione di una foto pronta per Foto: punti con le coordinate {prua: d} risolte e parametri della
    classe che servono alla proiezione."""
    XB = cl['scafo']['L'] / 2
    return dict(fc, punti=[[risolvi(pp, XB), q] for pp, q in fc['punti']],
                _volante=bool((cl.get('vista') or {}).get('volante')), _tono=cl.get('tono', 0.45),
                _contrasto=cl.get('contrasto', 0.17))

def modello_intero(cl, base_dir='.'):
    """Modello con l'opera viva (tools/navi/opera_viva.py) per l'esportazione verso il motore: navi di superficie
    con carena e appendici, sottomarini senza il taglio al galleggiamento piu' le eliche e le appendici di poppa
    sommerse. Le classi senza dati (aerei, missili) restano come sono."""
    import opera_viva, kit_sottomarini
    d = opera_viva.OPERA_VIVA.get(os.path.basename(os.path.normpath(cl['uscita'])))
    if d and d.get('sottomarino'):
        taglio = kit_sottomarini.taglia_galleggiamento
        kit_sottomarini.taglia_galleggiamento = lambda m, *a, **k: m
        try:
            m = costruisci_mesh(cl, base_dir)
        finally:
            kit_sottomarini.taglia_galleggiamento = taglio
        m = m + opera_viva.eliche_sottomarino(m, d)
        return m + opera_viva.coda_sottomarino(m, d, cl) if d.get('coda') else m
    m = costruisci_mesh(cl, base_dir)
    if d:
        m = m + opera_viva.carena(m, d['T'], d['tipo'], d.get('n_mezzo'), D=d.get('D')) + opera_viva.appendici(m, d)
    return m

def camera_vista_volante(aob, lato, elevazione, scala, ss, estensione, centro, distanza=4000.0):
    """Vista di un oggetto in volo (aereo, elicottero, missile) da un osservatore che lo vede `elevazione` gradi
    sopra l'orizzonte (dal basso), mirata al `centro` del modello; tela quadrata che contiene il modello."""
    a, e = np.radians(aob), np.radians(elevazione)
    c = np.asarray(centro, float)
    pos = c + distanza * np.array([np.cos(e) * np.cos(a), -lato * np.cos(e) * np.sin(a), -np.sin(e)])
    W = int(estensione * 1.25 * scala * ss) // 2 * 2
    return Camera.da_bersaglio(tuple(pos), tuple(c), scala * ss * distanza, W, W)

def camera_vista(aob, lato, scala, ss, L, distanza=4000.0, occhio=2.0, alto=46.0):
    a = np.radians(aob)
    pos = (distanza * np.cos(a), -lato * distanza * np.sin(a), occhio)   # lato +1 = dritta (y < 0)
    W = int(L * 1.12 * scala * ss) // 2 * 2; H = int(alto * scala * ss)
    return Camera.da_bersaglio(pos, (0, 0, alto * 0.40), scala * ss * distanza, W, H)

# luce ambiente dell'ombreggiatura di ripiego (facce che nessuna foto vede): con 0,30 le facce in ombra
# uscivano quasi nere (~0,14 contro il livello medio 0,45 delle foto); 0,55 le porta a ~0,25
LUCE_AMBIENTE = 0.55

def rendi(mesh, foto, aob, lato, scala=3.75, ss=3, sole=(0.55, 0.35, 0.75), peso_ombra=0.12, diagnosi=False,
          ambiente=LUCE_AMBIENTE, albedo=None, vista=None, elevazione=0.0, tono=0.45):
    L = mesh.tri[..., 0].max() - mesh.tri[..., 0].min()
    if vista and vista.get('volante'):
        # oggetto in volo: il modello sta tutto sopra z = 0 (niente taglio al galleggiamento), centro dato
        estensione = float(np.ptp(mesh.tri.reshape(-1, 3), axis=0).max())
        cam = camera_vista_volante(aob, lato, elevazione, scala, ss, estensione, vista.get('centro', (0, 0, 0)))
    else:
        # altezza della vista adattata alla nave (minimo 46 m): la scala in pixel per metro resta la stessa
        # per tutte le classi, quindi le dimensioni relative sono corrette; cambia solo la tela
        alto = max(46.0, float(mesh.tri[..., 2].max()) * 1.08 + 2.0)
        cam = camera_vista(aob, lato, scala, ss, L, alto=alto)
    opz = dict(ss=ss, sole=sole, peso_ombra=peso_ombra, ambiente=ambiente, albedo=albedo, tono=tono)
    retinati = np.array([m == 'retinato' for m in mesh.mat])
    if not retinati.any():
        return _rendi(mesh, foto, cam, diagnosi=diagnosi, **opz)
    # semitrasparenza dei dischi (eliche, rotori): stessa inquadratura con e senza i dischi, alternate a
    # scacchiera sui pixel dello sprite, cosi' dietro il disco si vede la nave (o lo sfondo)
    senza = kit.Mesh(mesh.tri[~retinati], [m for m, r in zip(mesh.mat, retinati) if not r],
                     [p for p, r in zip(mesh.parte, retinati) if not r] if mesh.parte else None)
    con = _rendi(mesh, foto, cam, diagnosi=diagnosi, **opz)
    img_con, dom = con if diagnosi else (con, None)
    img_senza = _rendi(senza, foto, cam, **opz)
    scacchi = (np.add.outer(np.arange(img_con.shape[0]), np.arange(img_con.shape[1])) % 2) == 1
    img = np.where(scacchi, img_senza, img_con)
    return (img, dom) if diagnosi else img

def _rendi(mesh, foto, cam, ss=3, sole=(0.55, 0.35, 0.75), peso_ombra=0.12, diagnosi=False,
           ambiente=LUCE_AMBIENTE, albedo=None, tono=0.45):
    tid, l0, l1, _ = raster.gbuffer(cam, mesh.tri)
    pix = tid >= 0
    P = raster.posizioni(mesh.tri, tid, l0, l1)[pix]
    k = tid[pix]
    T = mesh.tri; n = np.cross(T[:, 1] - T[:, 0], T[:, 2] - T[:, 0]); n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    n = n[k]; verso = cam.C - P; n = np.where(((n * verso).sum(1) < 0)[:, None], -n, n)
    mat = np.array(mesh.mat)[k]
    s = sole[0] * (-cam.fw) + sole[1] * cam.r + sole[2] * np.array([0, 0, 1.0]); s /= np.linalg.norm(s)
    tab = dict(ALBEDO, **(albedo or {}))
    alb = np.array([tab[x] for x in mat])
    ombra = np.clip(alb * (ambiente + (1 - ambiente) * np.clip(n @ s, 0, 1)) * (0.85 + 0.15 * np.clip(P[:, 2] / 15, 0, 1)) / 0.62 * tono, 0, 1)
    num = peso_ombra * ombra; den = np.full(len(P), peso_ombra)
    fonte = np.zeros((len(P), len(foto) + 1)); fonte[:, 0] = peso_ombra
    parte = np.array(mesh.parte)[k] if mesh.parte else np.array([''] * len(k))
    senza_foto = mat == 'retinato'                       # dischi di eliche e rotori: solo ombreggiatura
    for i, f in enumerate(foto):
        val, w, nave = f.campiona(P, n)
        w = np.where(nave & ~senza_foto, w, 0.0)         # la foto contribuisce solo dove mostra la nave
        escluse = f.cfg.get('escludi_parti')
        if escluse: w = np.where(np.isin(parte, escluse), 0.0, w)
        num += w * val; den += w; fonte[:, i + 1] = w
    out = np.full(tid.shape, np.nan); out[pix] = num / den
    H, W = tid.shape; h2, w2 = H // ss, W // ss
    blk = out[:h2 * ss, :w2 * ss].reshape(h2, ss, w2, ss)
    cop = (~np.isnan(blk)).mean(axis=(1, 3))
    with np.errstate(all='ignore'), warnings.catch_warnings():
        warnings.simplefilter('ignore', RuntimeWarning)       # blocchi vuoti: nanmean di soli NaN
        val = np.nanmean(blk, axis=(1, 3))
    img = np.where(cop >= 0.4, val, np.nan)
    if not diagnosi: return img
    dom = np.full(tid.shape, -1); dom[pix] = fonte.argmax(1)
    return img, dom[::ss, ::ss][:h2, :w2]

BAYER4 = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) / 16.0 - 0.47

def sprite(img, ramp):
    return sprite_ancorato(img, ramp)[0]

def sprite_ancorato(img, ramp, centrato=False):
    """Sprite a 16 grigi ritagliato sulla nave, piu' l'ancora: colonna e riga, nello sprite, del centro della
    nave al galleggiamento (punto (0, 0, 0), che la vista proietta sulla colonna centrale dell'immagine).
    Serve al motore per collocare la nave esattamente sul suo rilevamento."""
    h, w = img.shape; g = np.nan_to_num(img) * 255
    step = (ramp[-1] - ramp[0]) / (len(ramp) - 1)
    t = BAYER4[np.arange(h)[:, None] % 4, np.arange(w)[None, :] % 4] * 0.7
    k = np.clip(np.round((g - ramp[0]) / step + t), 0, len(ramp) - 1).astype(np.uint8) + 144
    k[np.isnan(img)] = 159
    ys, xs = np.nonzero(k != 159)
    # centrato (oggetti in volo): l'ancora e' il centro del modello, proiettato al centro della tela
    ay = h / 2 - ys.min() if centrato else float(ys.max() + 1 - ys.min())
    return k[ys.min():ys.max() + 1, xs.min():xs.max() + 1], (w / 2 - xs.min(), ay)

if __name__ == '__main__':
    argv = sys.argv[1:]
    uscita = None
    if '--uscita' in argv:
        k = argv.index('--uscita'); uscita = os.path.abspath(argv[k + 1]); del argv[k:k + 2]
    root, fcl = argv[0], argv[1]; passo = int(argv[2]) if len(argv) > 2 else 15
    cl = yaml.safe_load(open(fcl)); out = uscita or os.path.join(root, cl['uscita']); os.makedirs(out, exist_ok=True)
    t0 = time.time()
    mesh = costruisci_mesh(cl, os.path.dirname(os.path.abspath(fcl)))
    print(f"{cl['nome']}: {len(mesh.tri)} triangoli, {len(set(mesh.parte))} parti")
    foto = []
    ver = dict(classe=cl['nome'], triangoli=len(mesh.tri), foto={})
    XB = cl['scafo']['L'] / 2
    for fc in cl['foto']:
        f = Foto(cfg_foto(cl, fc), root); f.calibra(mesh); foto.append(f)
        arr = lambda x, n: None if x is None else round(x, n)
        ver['foto'][f.nome] = dict(errore_punti_px=round(f.rms_punti, 1), iou_dopo_punti=arr(f.iou_iniziale, 3),
                                   iou_finale=arr(f.iou, 3), errore_orizzonte_px=arr(f.errore_orizzonte, 1),
                                   errore_verticali_px=arr(f.errore_verticali, 1),
                                   limiti_raggiunti=f.limiti_raggiunti, fotocamera=[round(float(x), 5) for x in f.cam.p])
        sg = 'non misurabile' if f.iou is None else f'{f.iou_iniziale:.3f} -> {f.iou:.3f}'
        print(f"  foto {f.nome:18s} errore punti {f.rms_punti:5.1f} px  coincidenza sagoma {sg}")
        if f.limiti_raggiunti: print(f"    AVVISO: {', '.join(f.limiti_raggiunti)} (calibrazione forzata? vedi limiti_fotocamera)")
        # controllo visivo: sagoma del modello (rosso) sulla foto
        sag = raster.sagoma(f.cam, mesh.tri, 1.0)
        rgb = np.stack([f.g] * 3, -1).astype(np.uint8); rgb[sag & ~f.maschera_cielo & f.zona] = [230, 40, 40]
        rgb[~sag & f.maschera_cielo & f.zona] = [40, 210, 40]
        bordo = sag ^ np.roll(sag, 1, 0) | sag ^ np.roll(sag, 1, 1); rgb[bordo] = [255, 220, 0]
        Image.fromarray(rgb).save(f"{out}/calibrazione_{f.nome}.png")
    foto += [f.speculare() for f in list(foto) if f.cfg.get('specchiabile')]
    pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
    ramp = np.array([pal[3 * i] for i in range(144, 159)], float)
    os.makedirs(f'{out}/sprite', exist_ok=True)
    # un colore distinto per ogni sorgente (grigio = ombreggiatura del modello)
    import colorsys
    colori = np.array([[110, 110, 110]] + [[int(255 * c) for c in colorsys.hsv_to_rgb(k / max(len(foto), 1), 0.75, 0.95)]
                                           for k in range(len(foto))], np.uint8)
    diag = []; ancore = {}
    # `vista` (facoltativa): {volante: true, elevazioni: [0, 10, 25], centro: [x, y, z]} per aerei, elicotteri e
    # missili: sprite da piu' elevazioni (osservatore sotto l'oggetto), nome aob_XXX_lato_eNN se l'elevazione non e' 0
    vista = cl.get('vista') or {}
    elevazioni = vista.get('elevazioni', [0]) if vista.get('volante') else [0]
    for el in elevazioni:
      for aob in range(0, 181, passo):
        for nome, lato in (('dritta', 1), ('sinistra', -1)):
            # per classe: luce ambiente, peso dell'ombreggiatura di ripiego rispetto alle foto, albedo dei materiali
            img, dom = rendi(mesh, foto, aob, lato, diagnosi=True, ambiente=cl.get('luce_ambiente', LUCE_AMBIENTE),
                             peso_ombra=cl.get('peso_ombra', 0.12), albedo=cl.get('albedo'), vista=vista, elevazione=el,
                             tono=cl.get('tono', 0.45))
            k, anc = sprite_ancorato(img, ramp, centrato=bool(vista.get('volante')))
            chiave = f'aob_{aob:03d}_{nome}' + (f'_e{int(el):02d}' if el else '')
            ancore[chiave] = [round(anc[0], 2), round(anc[1], 2)]
            im = Image.fromarray(k, 'P'); im.putpalette(pal)
            im.save(f'{out}/sprite/{chiave}.png', transparency=159)
            if nome == 'dritta' and aob % 45 == 0 and el == elevazioni[0]:
                c = np.zeros(dom.shape + (3,), np.uint8); c[dom >= 0] = colori[dom[dom >= 0] % len(colori)]
                diag.append(Image.fromarray(c))
    nota = ('ancora = [colonna, riga] del centro del modello (`vista.centro`) nello sprite' if vista.get('volante')
            else 'ancora = [colonna, riga] del centro nave al galleggiamento nello sprite')
    json.dump(dict(scala_px_m=3.75, nota=nota,
                   ancore=ancore), open(f'{out}/sprite/ancore.json', 'w'), indent=1)
    # foglio delle fonti dominanti (grigio = ombreggiatura del modello, colori = foto in ordine)
    Wd = max(sum(d.width for d in diag) + 10 * len(diag), 260 + sum(20 + 7 * len(f.nome) for f in foto)); Hd = max(d.height for d in diag)
    sheet = Image.new('RGB', (Wd, Hd + 20), (20, 20, 20)); x = 0
    for d in diag: sheet.paste(d, (x, 0)); x += d.width + 10
    dr = ImageDraw.Draw(sheet); dr.text((4, Hd + 4), 'fonte dominante: grigio = modello;', fill=(230, 230, 230)); x = 230
    for k, f in enumerate(foto):     # legenda con un campione di colore per ogni foto
        dr.rectangle((x, Hd + 5, x + 10, Hd + 15), fill=tuple(int(v) for v in colori[k + 1])); dr.text((x + 14, Hd + 4), f.nome, fill=(230, 230, 230))
        x += 20 + 7 * len(f.nome)
    sheet.save(f'{out}/fonti_dominanti.png')
    ver['durata_s'] = round(time.time() - t0, 1)
    json.dump(ver, open(f'{out}/verifica.json', 'w'), indent=2, ensure_ascii=False)
    print(f"fatto in {ver['durata_s']} s -> {out}")
