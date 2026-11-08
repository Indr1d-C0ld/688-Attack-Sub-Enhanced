"""Fotocamera a foro stenopeico (coordinate destrorse del kit) e calibrazione sulle foto.

Parametri: posizione (x, y, z), imbardata (direzione di vista nel piano orizzontale, gradi da +x
verso +y), beccheggio (gradi, positivo verso l'alto), rollio (gradi), focale f (pixel).
Il punto principale e' il centro dell'immagine.
"""
import numpy as np
from scipy.optimize import least_squares, minimize

class Camera:
    def __init__(self, p, w, h):
        self.p = np.asarray(p, float); self.w, self.h = w, h
        C = self.p[:3]; yaw, pitch, roll = np.radians(self.p[3:6]); self.f = self.p[6]
        fw = np.array([np.cos(pitch) * np.cos(yaw), np.cos(pitch) * np.sin(yaw), np.sin(pitch)])
        r0 = np.cross(fw, [0, 0, 1.0]); r0 /= np.linalg.norm(r0); u0 = np.cross(r0, fw)
        self.C, self.fw = C, fw
        self.r = np.cos(roll) * r0 + np.sin(roll) * u0
        self.u = -np.sin(roll) * r0 + np.cos(roll) * u0

    def proietta(self, P):
        """P (..., 3) -> u, v (pixel), profondita' lungo l'asse ottico."""
        v = np.asarray(P, float) - self.C
        z = v @ self.fw
        return self.w / 2 + self.f * (v @ self.r) / z, self.h / 2 - self.f * (v @ self.u) / z, z

    @staticmethod
    def da_bersaglio(pos, bersaglio, f, w, h, rollio=0.0):
        d = np.asarray(bersaglio, float) - np.asarray(pos, float)
        yaw = np.degrees(np.arctan2(d[1], d[0])); pitch = np.degrees(np.arctan2(d[2], np.hypot(d[0], d[1])))
        return Camera([*pos, yaw, pitch, rollio, f], w, h)

# ---------------------------------------------------------------- calibrazione
# Parametri di calibrazione q = [azimut, elevazione, log distanza, x mirato, z mirato, rollio, log f]
# (punto mirato entro +-200 m in lunghezza e tra -10 e 100 m di quota, per le navi fino a ~400 m):
# la fotocamera guarda il punto mirato (x, 0, z) da distanza D, azimut (gradi da +x verso +y) ed
# elevazione. I limiti escludono soluzioni fisicamente assurde (fotocamera dentro la nave, focale minima),
# che con punti quasi complanari (tutti sul piano di simmetria) sarebbero altrimenti possibili.
LIMITI = np.array([[-360, 360], [-5, 60], [np.log(80), np.log(60000)], [-200, 200], [-10, 100], [-30, 30],
                   [np.log(300), np.log(250000)]])
NOMI_Q = ['azimut', 'elevazione', 'distanza', 'mira_x', 'mira_z', 'rollio', 'focale']

def limiti_foto(cfg):
    """Limiti fisici per una foto: quelli generali, ristretti o allargati da `limiti_fotocamera`
    (elevazione in gradi, distanza in m, focale in pixel), es. {elevazione: [5, 30]} per una foto aerea
    senza orizzonte."""
    lim = LIMITI.copy(); lc = (cfg or {}).get('limiti_fotocamera') or {}
    if (cfg or {}).get('_volante'):   # oggetti in volo: foto anche dal basso e con forti inclinazioni
        lim[NOMI_Q.index('elevazione')] = (-80, 80); lim[NOMI_Q.index('rollio')] = (-60, 60)
    for k, (a, b) in lc.items():
        i = NOMI_Q.index(k)
        lim[i] = np.log([a, b]) if k in ('distanza', 'focale') else (a, b)
    return lim

def al_limite(q, lim, tolleranza=1e-3):
    """Nomi dei parametri della fotocamera finiti su un limite (calibrazione probabilmente forzata)."""
    out = []
    for i, n in enumerate(NOMI_Q):
        a, b = lim[i]
        if n == 'azimut' and b - a >= 720 - 1e-6: continue   # azimut libero (periodico): nessun limite vero
        t = tolleranza * max(b - a, 1e-9)
        if q[i] <= a + t: out.append(f'{n} al minimo')
        elif q[i] >= b - t: out.append(f'{n} al massimo')
    return out

def scarto_prior(q, prior):
    """Vincolo debole su elevazione e distanza (in pixel equivalenti: 5 px per sigma).
    `prior`: {elevazione: gradi, sigma_el: gradi, distanza: m, sigma_logD: log naturale}; le voci assenti
    non vincolano."""
    if not prior: return np.zeros(0)
    r = []
    if 'elevazione' in prior: r.append(5.0 * (q[1] - prior['elevazione']) / prior.get('sigma_el', 5.0))
    if 'distanza' in prior: r.append(5.0 * (q[2] - np.log(prior['distanza'])) / prior.get('sigma_logD', 0.5))
    return np.array(r)

def da_q(q, w, h):
    az, el, lD, xt, zt, roll, lf = q
    D = np.exp(lD); a, e = np.radians(az), np.radians(el)
    t = np.array([xt, 0.0, zt])
    pos = t + D * np.array([np.cos(e) * np.cos(a), np.cos(e) * np.sin(a), np.sin(e)])
    return Camera.da_bersaglio(pos, t, np.exp(lf), w, h, roll)

def q_iniziale(posizione, bersaglio, f):
    d = np.asarray(posizione, float) - np.asarray(bersaglio, float)
    D = np.linalg.norm(d)
    return np.array([np.degrees(np.arctan2(d[1], d[0])), np.degrees(np.arcsin(d[2] / D)), np.log(D),
                     bersaglio[0], bersaglio[2], 0.0, np.log(f)])

R_TERRA = 6371000.0

def verticale_nave(assetto):
    """Verticale vera nelle coordinate della nave con l'assetto {beccheggio, sbandamento} in gradi (beccheggio
    positivo = prua in alto, sbandamento positivo = sinistra in alto, cioe' nave sbandata sulla dritta)."""
    if not assetto: return np.array([0.0, 0.0, 1.0])
    b, s = np.radians(assetto.get('beccheggio', 0.0)), np.radians(assetto.get('sbandamento', 0.0))
    v = np.array([np.sin(b), np.sin(s), np.cos(b) * np.cos(s)])
    return v / np.linalg.norm(v)

def scarto_orizzonte(cam, orizzonte, assetto=None):
    """Scarto in pixel tra i punti annotati sulla linea dell'orizzonte marino e l'orizzonte vero: il raggio
    di ciascun pixel deve stare all'angolo di depressione sqrt(2h/R) sotto l'orizzontale (h = quota della
    fotocamera, R = raggio terrestre). Vincola elevazione e rollio anche quando i punti della nave sono
    tutti sul piano di simmetria. `assetto`: beccheggio e sbandamento della nave nella foto (verticale_nave)."""
    if not orizzonte: return np.zeros(0)
    O = np.asarray(orizzonte, float)
    d = cam.fw[None, :] + ((O[:, 0] - cam.w / 2) / cam.f)[:, None] * cam.r[None, :] \
        - ((O[:, 1] - cam.h / 2) / cam.f)[:, None] * cam.u[None, :]
    su = verticale_nave(assetto)
    elev = np.arcsin(np.clip(d @ su / np.linalg.norm(d, axis=1), -1, 1))
    # quota del mare nelle coordinate del modello (0 per le navi; per gli oggetti in volo, meno la loro quota)
    quota_mare = float((assetto or {}).get('quota_mare', 0.0))
    depr = np.sqrt(2 * max(float(cam.C @ su) - quota_mare, 0.5) / R_TERRA)
    return cam.f * (elev + depr)

def scarto_verticali(cam, verticali):
    """Segmenti [[u1, v1], [u2, v2]] tracciati lungo spigoli verticali della nave (alberi, spigoli delle
    sovrastrutture): devono puntare al punto di fuga delle verticali della nave. Residuo in pixel: distanza dei
    due estremi dalla retta che unisce il punto medio al punto di fuga. Vincola il rollio della fotocamera
    quando la foto e' quasi orizzontale."""
    if not verticali: return np.zeros(0)
    d = np.array([0.0, 0.0, 1.0])
    W = d @ cam.fw                                   # punto di fuga in coordinate omogenee (anche all'infinito)
    X = cam.w / 2 * W + cam.f * (d @ cam.r); Y = cam.h / 2 * W - cam.f * (d @ cam.u)
    out = []
    for a, b in verticali:
        a, b = np.asarray(a, float), np.asarray(b, float); m = (a + b) / 2
        dv = np.array([X - m[0] * W, Y - m[1] * W]); n = np.linalg.norm(dv)
        if n < 1e-12: out += [0.0, 0.0]; continue
        nor = np.array([-dv[1], dv[0]]) / n
        out += [float((a - m) @ nor), float((b - m) @ nor)]
    return np.array(out)

def calibra_punti(q0, w, h, punti3d, punti2d, peso_prior=3.0, orizzonte=None, limiti=None, prior=None,
                  verticali=None, assetto=None):
    """Minimi quadrati sull'errore di riproiezione, con un vincolo debole verso distanza e focale iniziali.
    `orizzonte`: pixel (u, v) sulla linea dell'orizzonte marino (facoltativo, vedi scarto_orizzonte).
    `limiti`: limiti fisici (predefiniti LIMITI, vedi limiti_foto); `prior`: vedi scarto_prior;
    `verticali`: vedi scarto_verticali; `assetto`: assetto della nave per il vincolo d'orizzonte."""
    lim = LIMITI if limiti is None else limiti
    q0 = np.clip(q0, lim[:, 0], lim[:, 1])
    P = np.asarray(punti3d, float); Q = np.asarray(punti2d, float)
    def res(q):
        c = da_q(q, w, h); u, v, z = c.proietta(P)
        return np.concatenate([u - Q[:, 0], v - Q[:, 1], scarto_orizzonte(c, orizzonte, assetto), scarto_prior(q, prior),
                               scarto_verticali(c, verticali),
                               [peso_prior * (q[2] - q0[2]), peso_prior * (q[6] - q0[6])]])
    sol = least_squares(res, q0, bounds=(lim[:, 0], lim[:, 1]), x_scale=[5, 2, 0.2, 5, 2, 1, 0.2],
                        loss='soft_l1', f_scale=5.0)
    c = da_q(sol.x, w, h); u, v, _ = c.proietta(P)
    err = np.hypot(u - Q[:, 0], v - Q[:, 1])
    return sol.x, c, float(np.sqrt((err ** 2).mean()))

def calibra_sagoma(q0, w, h, sagoma_fn, maschera, zona, punti3d, punti2d, peso_punti=0.002, iter=500, orizzonte=None,
                   limiti=None, prior=None, verticali=None, assetto=None):
    """Affina massimizzando la coincidenza (IoU) tra sagoma del modello e maschera della foto nella zona
    affidabile; i punti di riferimento restano come vincolo debole. `sagoma_fn(camera)` riceve una
    fotocamera alla risoluzione della maschera."""
    lim = LIMITI if limiti is None else limiti
    P = np.asarray(punti3d, float); Q = np.asarray(punti2d, float)
    hm, wm = maschera.shape; s = wm / w
    scala = np.array([1.0, 0.5, 0.05, 1.0, 0.5, 0.5, 0.05])
    def costo(d):
        q = np.clip(q0 + d * scala, lim[:, 0], lim[:, 1])
        c = da_q(q, w, h)
        cs = Camera(np.r_[c.p[:6], c.p[6] * s], wm, hm)
        sg = sagoma_fn(cs)
        a, b = sg & zona, maschera & zona
        iou = (a & b).sum() / max((a | b).sum(), 1)
        u, v, _ = c.proietta(P)
        e2 = np.concatenate([(u - Q[:, 0]) ** 2 + (v - Q[:, 1]) ** 2, scarto_orizzonte(c, orizzonte, assetto) ** 2,
                             scarto_prior(q, prior) ** 2, scarto_verticali(c, verticali) ** 2])
        return -iou + peso_punti * float(np.mean(e2))
    r = minimize(costo, np.zeros(7), method='Nelder-Mead',
                 options=dict(maxiter=iter, xatol=1e-3, fatol=1e-5, initial_simplex=np.vstack([np.zeros(7), np.eye(7)])))
    q = np.clip(q0 + r.x * scala, lim[:, 0], lim[:, 1])
    return q, da_q(q, w, h)
