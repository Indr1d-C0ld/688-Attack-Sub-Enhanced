"""Foto di riferimento di una classe: maschera nave/sfondo, cancellatura delle scritte, livelli,
calibrazione della fotocamera e mappa di profondita' per il controllo delle occlusioni."""
import numpy as np
from PIL import Image, ImageFilter
from scipy import ndimage
from camera import (Camera, calibra_punti, calibra_sagoma, q_iniziale, limiti_foto, al_limite, scarto_orizzonte,
                    scarto_verticali)

def scala_verticale_cfg(cfg, k):
    """Copia della configurazione di una foto con tutte le coordinate verticali (v, y) moltiplicate per k:
    serve per `aspetto_pixel` (scansioni stirate), quando la foto viene ricampionata in altezza."""
    c = dict(cfg)
    uv = lambda p: [p[0], p[1] * k]
    if 'punti' in c: c['punti'] = [[P, uv(q)] for P, q in c['punti']]
    for chiave in ('orizzonte_punti',):
        if c.get(chiave): c[chiave] = [uv(p) for p in c[chiave]]
    if c.get('verticali'): c['verticali'] = [[uv(a), uv(b)] for a, b in c['verticali']]
    if c.get('orizzonte'): c['orizzonte'] = [uv(p) for p in c['orizzonte']]
    for chiave in ('orizzonte_y', 'mare_fino_a_y'):
        if chiave in c: c[chiave] = c[chiave] * k
    for chiave in ('cancella', 'escludi'):
        if c.get(chiave): c[chiave] = [[x0, round(y0 * k), x1, round(y1 * k)] for x0, y0, x1, y1 in c[chiave]]
    if c.get('riquadro'):
        x0, y0, x1, y1 = c['riquadro']; c['riquadro'] = [x0, round(y0 * k), x1, round(y1 * k)]
    return c
import raster

class Foto:
    def __init__(self, cfg, root):
        # aspetto_pixel: altezza vera / altezza nella scansione (es. 0,90 per una scansione stirata in verticale
        # dell'11%): la foto e le maschere vengono ricampionate in altezza e le coordinate annotate riscalate
        self.aspetto = float(cfg.get('aspetto_pixel', 1.0))
        if self.aspetto != 1.0: cfg = scala_verticale_cfg(cfg, self.aspetto)
        self.cfg = cfg; self.nome = cfg['nome']; self.root = root
        im = self._apri(cfg['file']).convert('L')
        self.g = np.asarray(im, float); self.h, self.w = self.g.shape
        self.qualita = float(cfg.get('qualita', 1.0))
        # cancellatura di numeri di scafo e nomi (riempimento con la mediana della fascia a sinistra)
        # riquadro allargato di un margine e riempito interpolando, riga per riga, tra le fasce a sinistra
        # e a destra (mediane), con un leggero rumore per non lasciare zone piatte
        self.pulita = self.g.copy(); mg = int(cfg.get('margine_cancella', 4))
        rng = np.random.default_rng(0)
        for x0, y0, x1, y1 in cfg.get('cancella', []):
            x0, y0, x1, y1 = max(x0 - mg, 0), max(y0 - mg, 0), min(x1 + mg, self.w), min(y1 + mg, self.h)
            sx = np.median(self.g[y0:y1, max(x0 - 8, 0):x0], axis=1) if x0 > 0 else None
            dx = np.median(self.g[y0:y1, x1:min(x1 + 8, self.w)], axis=1) if x1 < self.w else None
            if sx is None: sx = dx
            if dx is None: dx = sx
            t = np.linspace(0, 1, x1 - x0)[None, :]
            fill = sx[:, None] * (1 - t) + dx[:, None] * t
            self.pulita[y0:y1, x0:x1] = fill + rng.normal(0, 3, fill.shape)
        self._maschera_affidabile()

    def _apri(self, file, nearest=False):
        im = Image.open(f"{self.root}/{file}")
        if self.aspetto != 1.0:
            im = im.resize((im.width, round(im.height * self.aspetto)), Image.NEAREST if nearest else Image.LANCZOS)
        return im

    def _maschera_affidabile(self):
        """Zona in cui la separazione dallo sfondo e' affidabile e maschera nave in quella zona.
        Con `maschera_file` (maschera ritoccata a mano, tools/navi/maschera_manuale.py) la maschera e'
        affidabile ovunque e sostituisce soglie e sagoma del modello."""
        c = self.cfg
        if 'maschera_file' in c:
            self.manuale = np.asarray(self._apri(c['maschera_file'], nearest=True).convert('L')) > 127
            self.zona = np.ones_like(self.manuale); self.maschera_cielo = self.manuale
            # `escludi` toglie zone dalla calibrazione (es. parti di un'altra variante della classe);
            # `maschera_calibrazione` sostituisce la maschera solo per la calibrazione
            for x0, y0, x1, y1 in c.get('escludi', []): self.zona[y0:y1, x0:x1] = False
            if 'maschera_calibrazione' in c:
                self.maschera_cielo = np.asarray(self._apri(c['maschera_calibrazione'], nearest=True).convert('L')) > 127
            return
        self.manuale = None
        # sulla foto pulita: le scritte cancellate contano come nave (altrimenti lasciano un "fantasma")
        g = np.asarray(Image.fromarray(np.clip(self.pulita, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(0.8)), float)
        yy, xx = np.mgrid[0:self.h, 0:self.w]
        if 'orizzonte' in c:         # retta, anche inclinata: [[x0, y0], [x1, y1]]
            (x0, y0), (x1, y1) = c['orizzonte']
            oy = (y0 + (y1 - y0) * (np.arange(self.w) - x0) / (x1 - x0))[None, :]
        else:
            oy = c['orizzonte_y']
        zona = yy < oy - 2
        if 'scarto_cielo' in c:
            # nave chiara o scura: si scosta dal cielo della propria riga (mediana della riga nella zona)
            cielo = np.array([np.median(g[y, zona[y]]) if zona[y].sum() > 20 else np.nan for y in range(self.h)])
            ok = ~np.isnan(cielo)
            if ok.any():
                cielo = ndimage.uniform_filter1d(np.interp(np.arange(self.h), np.flatnonzero(ok), cielo[ok]), 15)
            m = zona & (np.abs(g - cielo[:, None]) > c['scarto_cielo'])
        else:
            m = zona & (g < c['soglia_cielo'])
        if 'soglia_mare' in c:
            z2 = (yy >= oy - 2) & (yy <= c.get('mare_fino_a_y', self.h))
            zona |= z2; m |= z2 & (g < c['soglia_mare'])
        if 'riquadro' in c:
            x0, y0, x1, y1 = c['riquadro']; r = (xx >= x0) & (xx <= x1) & (yy >= y0) & (yy <= y1)
            zona &= r; m &= r
        for x0, y0, x1, y1 in c.get('escludi', []):
            zona[y0:y1, x0:x1] = False; m[y0:y1, x0:x1] = False
        m = ndimage.binary_opening(m, np.ones((2, 2)))
        lab, n = ndimage.label(m)
        if n:
            dim = ndimage.sum(m, lab, range(1, n + 1)); m = np.isin(lab, 1 + np.flatnonzero(dim >= 30))
        self.zona, self.maschera_cielo = zona, m

    def calibra(self, mesh):
        c = self.cfg; ci = c['fotocamera_iniziale']
        P = [p for p, q in c['punti']]; Q = [q for p, q in c['punti']]
        q0 = q_iniziale(ci['posizione'], ci['bersaglio'], ci['f'])
        lim = limiti_foto(c); oriz = c.get('orizzonte_punti'); prior = c.get('prior_fotocamera')
        vert = c.get('verticali'); ass = c.get('assetto_nave')
        # `affondamento`: la nave in questa foto sta piu' bassa (m) del modello: modello e punti scendono di tanto
        self.dz = -float(c.get('affondamento', 0.0))
        P = [np.asarray(p, float) + np.array([0.0, 0.0, self.dz]) for p in P]
        # parti escluse da calibrazione e proiezione (`escludi_parti`) e materiale retinato (eliche, rotori)
        tri = self._triangoli(mesh)
        # partenze multiple attorno alla stima iniziale (azimut, elevazione, distanza): evita i minimi locali
        partenze_el = {q0[1], 5.0, 15.0, 30.0, 45.0} | ({-15.0, -30.0, -50.0} if c.get('_volante') else set())
        migliore = None
        for daz in (-40, -20, 0, 20, 40):
            for el in sorted(partenze_el):
                for fd in (0.5, 1.0, 2.0):
                    qs = q0.copy(); qs[0] += daz; qs[1] = el; qs[2] += np.log(fd); qs[6] += np.log(fd)
                    qs = np.clip(qs, lim[:, 0], lim[:, 1])   # le partenze multiple restano nei limiti fisici
                    qq, cc, rms = calibra_punti(qs, self.w, self.h, P, Q, orizzonte=oriz, limiti=lim, prior=prior,
                                                verticali=vert, assetto=ass)
                    if migliore is None or rms < migliore[2]:
                        migliore = (qq, cc, rms)
        q1, cam, _ = migliore
        s = 0.5
        hs, ws = int(self.h * s), int(self.w * s)
        zona_s = np.asarray(Image.fromarray(self.zona.astype(np.uint8) * 255).resize((ws, hs), Image.NEAREST)) > 0
        mask_s = np.asarray(Image.fromarray(self.maschera_cielo.astype(np.uint8) * 255).resize((ws, hs), Image.NEAREST)) > 0
        sag = lambda cc: raster.sagoma(cc, tri, 1.0)
        def iou(cam_full):
            cs = Camera(np.r_[cam_full.p[:6], cam_full.p[6] * s], ws, hs)
            a, b = sag(cs) & zona_s, mask_s & zona_s
            return (a & b).sum() / max((a | b).sum(), 1)
        misurabile = bool((mask_s & zona_s).any())
        iou0 = iou(cam)
        if c.get('solo_punti'):    # calibrazione sui soli punti (nessun affinamento sulla sagoma)
            q2, cam2 = q1, cam
        else:
            q2, cam2 = calibra_sagoma(q1, self.w, self.h, sag, mask_s, zona_s, P, Q, orizzonte=oriz, limiti=lim,
                                      prior=prior, verticali=vert, assetto=ass)
        iou1 = iou(cam2)
        if iou1 < iou0:            # l'affinamento non deve peggiorare
            cam2, iou1, q2 = cam, iou0, q1
        # parametri finiti su un limite fisico: calibrazione probabilmente forzata (da controllare)
        self.limiti_raggiunti = al_limite(q2, lim)
        self.cam = cam2
        u, v, _ = cam2.proietta(np.asarray(P, float))
        self.rms_punti = float(np.sqrt(np.mean((u - np.asarray(Q)[:, 0]) ** 2 + (v - np.asarray(Q)[:, 1]) ** 2)))
        # coincidenza None se non misurabile (nessun pixel di nave nella zona affidabile)
        self.iou_iniziale, self.iou = (float(iou0), float(iou1)) if misurabile else (None, None)
        so = scarto_orizzonte(cam2, oriz, ass)
        self.errore_orizzonte = float(np.sqrt(np.mean(so ** 2))) if so.size else None
        sv = scarto_verticali(cam2, vert)
        self.errore_verticali = float(np.sqrt(np.mean(sv ** 2))) if sv.size else None
        self._finalizza(tri)

    def _triangoli(self, mesh):
        """Triangoli usati per calibrazione e proiezione: senza le parti escluse e i dischi retinati, abbassati
        di `affondamento`."""
        c = self.cfg
        self.dz = -float(c.get('affondamento', 0.0))
        escluse = set(c.get('escludi_parti', []))
        tieni = np.array([(pa not in escluse) and (ma != 'retinato') for pa, ma in zip(mesh.parte, mesh.mat)])
        tri = mesh.tri[tieni].copy(); tri[..., 2] += self.dz
        return tri

    def usa_fotocamera(self, mesh, p):
        """Fotocamera gia' calibrata (parametri `fotocamera` di verifica.json) invece della calibrazione: prepara
        maschera, profondita' e livelli come `calibra`, per proiettare la foto su un modello (esportazione)."""
        self.cam = Camera(np.asarray(p, float), self.w, self.h)
        self._finalizza(self._triangoli(mesh))

    def _finalizza(self, tri):
        c = self.cfg; cam2 = self.cam
        # maschera finale: affidabile dove la foto separa bene la nave, sagoma del modello altrove
        sag_full = raster.sagoma(cam2, tri, 1.0)
        # sotto l'orizzonte: sagoma del modello, meno la schiuma (pixel molto chiari) e meno i pixel che
        # sembrano mare. Dove il modello non coincide con la nave reale la foto mostra il mare: senza
        # questo controllo la parte verrebbe "dipinta" di mare e apparirebbe trasparente.
        if self.manuale is not None:
            self.maschera = self.manuale
        else:
            self.mare = self._simile_al_mare(sag_full, float(c.get('soglie_mare_sigma', 2.0)))
            self.maschera = np.where(self.zona, self.maschera_cielo,
                                     sag_full & ~self.mare & (self.pulita < float(c.get('soglia_schiuma', 200))))
        # profondita' vista dalla foto (meta' risoluzione) per le occlusioni
        cam_d = Camera(np.r_[cam2.p[:6], cam2.p[6] * 0.5], self.w // 2, self.h // 2)
        _, _, _, self.zbuf = raster.gbuffer(cam_d, tri)
        self.cam_d = cam_d
        # livelli: media e deviazione dei pixel della nave portati a valori comuni
        v = self.pulita[self.maschera]
        self.livelli = (float(v.mean()), float(v.std()) + 1e-6)

    def speculare(self):
        """Copia della foto usata riflessa rispetto al piano di simmetria (y -> -y): fornisce il lato opposto.
        Usa sempre la versione con le scritte cancellate (che altrimenti apparirebbero rovesciate)."""
        import copy
        c = copy.copy(self); c.specchio = True; c.nome = self.nome + '_speculare'
        c.qualita = self.qualita * float(self.cfg.get('qualita_speculare', 0.9))
        return c

    specchio = False

    def _simile_al_mare(self, sag, k):
        """Statistiche del mare riga per riga (mediana e dispersione robusta) dai pixel fuori dalla sagoma
        dilatata, levigate tra righe vicine; un pixel e' 'mare' se cade entro k dispersioni."""
        fuori = ~ndimage.binary_dilation(sag, iterations=12)
        g = self.pulita; H = self.h
        mu = np.full(H, np.nan); sd = np.full(H, np.nan)
        for y in range(H):
            v = g[y, fuori[y]]
            if v.size >= 40:
                m = np.median(v); mu[y] = m; sd[y] = 1.4826 * np.median(np.abs(v - m))
        ok = ~np.isnan(mu)
        if ok.sum() < 2: return np.zeros_like(sag)
        yy = np.arange(H)
        mu = ndimage.uniform_filter1d(np.interp(yy, yy[ok], mu[ok]), 15)
        sd = np.maximum(ndimage.uniform_filter1d(np.interp(yy, yy[ok], sd[ok]), 15), 6.0)
        return np.abs(g - mu[:, None]) < k * sd[:, None]

    def campiona(self, P, n):
        """Per punti del mondo P (N,3) con normali n (N,3): valore (0..1), peso e flag di nave."""
        if self.specchio:
            P = P * np.array([1.0, -1.0, 1.0]); n = n * np.array([1.0, -1.0, 1.0])
        P = P + np.array([0.0, 0.0, getattr(self, 'dz', 0.0)])
        u, v, z = self.cam.proietta(P)
        ui, vi = np.round(u).astype(int), np.round(v).astype(int)
        dentro = (ui >= 0) & (ui < self.w) & (vi >= 0) & (vi < self.h) & (z > 0)
        ud, vd, zd = self.cam_d.proietta(P)
        udi = np.clip(np.round(ud).astype(int), 0, self.cam_d.w - 1); vdi = np.clip(np.round(vd).astype(int), 0, self.cam_d.h - 1)
        visibile = zd <= self.zbuf[vdi, udi] + 0.5 + 0.01 * zd
        uc, vc = np.clip(ui, 0, self.w - 1), np.clip(vi, 0, self.h - 1)
        nave = self.maschera[vc, uc] & dentro
        m, s = self.livelli
        # livello e contrasto comuni della classe (`tono`, `contrasto`: 0,45 e 0,17 di norma, piu' scuro per gli scafi neri)
        tono, contrasto = float(self.cfg.get('_tono', 0.45)), float(self.cfg.get('_contrasto', 0.17))
        val = np.clip(tono + contrasto * (self.pulita[vc, uc] - m) / s, 0, 1)
        verso = self.cam.C - P; verso /= np.linalg.norm(verso, axis=1, keepdims=True)
        w = np.clip((n * verso).sum(1), 0, 1) ** float(self.cfg.get('esponente_peso', 2.0)) * self.qualita
        w = np.where(dentro & visibile, w, 0.0)
        return val, w, nave
