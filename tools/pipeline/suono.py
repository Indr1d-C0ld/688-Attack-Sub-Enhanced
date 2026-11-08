"""Profili della velocita' del suono per teatro e mese -> data/motore/teatri/<teatro>.suono.toml (fase 2, acustica).
Li legge nucleo/acustica.hpp (carica_clima).

Fonte: data/teatri/<teatro>/ssp_mensile.json (decav 1955-2022) o, con l'epoca '8594', ssp_mensile_8594.json (il
decennio 1985-1994, fetch_woa_8594.py): mediana del teatro per mese, WOA23 0-1500 m, c TEOS-10; vedi fetch_woa.py. Sotto l'ultimo livello valido il profilo prosegue con la mediana della stagione del mese
(woa23_stagionale.nc, fino a 5500 m), raccordata sull'ultimo livello comune; oltre i dati, fino a 6000 m, con il
solo gradiente di pressione (+0,0163 m/s per m, dossier 02 §3.1). Nei teatri di acqua bassa la mediana finisce
dove finisce l'acqua: il motore taglia comunque il profilo al fondale del punto.

Parametri del teatro per l'assorbimento e il rumore ambiente (STIME DI GIOCO, dossier 02 §3.2 e §4.3): pH,
traffico s in [0,1] (rotte mercantili 0,8-1, oceano aperto 0,3-0,5).
Uso: suono.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import json, os, sys, warnings
import numpy as np

QUI = os.path.dirname(os.path.abspath(__file__))
PROFONDITA_MAX_M = 6000.0
GRADIENTE_PRESSIONE = 0.0163     # s^-1, strato isotermo profondo
PASSO_PROFONDO_M = 250.0

# teatro: (pH, traffico, mese predefinito, nota). Le missioni del 1989 non hanno una data: il mese predefinito e' una
# scelta di gioco (inverno nei mari del nord, estate nel Mediterraneo, dove le CZ ci sono solo d'estate)
TEATRI = {
    'baltico':    (8.0, 0.8, 2, 'acqua salmastra (S 7-8 psu nel Baltico propriamente detto); traffico intenso'),
    'barents':    (8.1, 0.3, 3, 'acqua bassa, profilo quasi isotermo d\'inverno; ghiaccio al margine (stagionale)'),
    'celtica':    (8.1, 0.7, 2, 'piattaforma e scarpata; rotte d\'accesso alla Manica'),
    'faroe':      (8.1, 0.4, 2, 'fronte Islanda-Faroe; bacini profondi a nord'),
    'gibilterra': (8.2, 1.0, 8, 'stretto: traffico pesantissimo, acqua mediterranea in uscita a 1000 m'),
    'kunashir':   (8.0, 0.4, 2, 'Curili: Mare di Okhotsk e Pacifico subartico'),
    'otranto':    (8.2, 0.8, 8, 'Mediterraneo: isotermo profondo a 13-14 gradi'),
}
# tipo di fondo per fascia di profondita' (piattaforma sotto 200 m, scarpata fino a 2000, profondo oltre): STIMA DI
# GIOCO dalla geologia generale dei teatri, nessun raster dei sedimenti (dossier 02 §3.4 e: roccia e ghiaia
# riflettono, il fango assorbe; dossier 06 §6). Piattaforme glaciali e stretti spazzati dalle correnti duri, bacini e
# piane abissali fangosi
FONDI = {
    'baltico':    ('fango', 'fango', 'fango'),       # bacini del Baltico: argille e fanghi; sabbie solo sulle secche
    'barents':    ('sabbia', 'fango', 'fango'),      # sedimenti glaciali misti sulla piattaforma
    'celtica':    ('sabbia', 'fango', 'fango'),      # sabbie della piattaforma, scarpata e piana fangose
    'faroe':      ('roccia', 'sabbia', 'fango'),     # piattaforma delle Fær Øer e dorsale: roccia e ghiaia
    'gibilterra': ('roccia', 'sabbia', 'fango'),     # lo stretto spazzato dalle correnti
    'kunashir':   ('roccia', 'sabbia', 'fango'),     # arco vulcanico delle Curili
    'otranto':    ('sabbia', 'fango', 'fango'),      # piattaforma pugliese e albanese, fosse fangose
}
STAGIONE = {12: 0, 1: 0, 2: 0, 3: 1, 4: 1, 5: 1, 6: 2, 7: 2, 8: 2, 9: 3, 10: 3, 11: 3}   # WOA: gen-mar inverno...


# la climatologia dei profili: 'decav' (1955-2022) o '8594' (il decennio 1985-1994, l'epoca del gioco; fetch_woa_8594.py)
EPOCA = '8594'


def epoca(radice, teatro, quale=None):
    """I file del teatro per l'epoca scelta; '8594' ripiega sul decav se i file del decennio mancano."""
    d = f'{radice}/data/teatri/{teatro}'
    if (quale or EPOCA) == '8594' and os.path.exists(f'{d}/ssp_mensile_8594.json') and os.path.exists(f'{d}/woa23_8594_stagionale.nc'):
        return f'{d}/ssp_mensile_8594.json', f'{d}/woa23_8594_stagionale.nc', 'WOA23 1985-1994 stagionale'
    return f'{d}/ssp_mensile.json', f'{d}/woa23_stagionale.nc', 'WOA23 stagionale'


def profondo(radice, teatro, quale=None):
    """Mediana stagionale (4 profili) sotto i 1500 m: profondita', c TEOS-10; None senza file."""
    f = epoca(radice, teatro, quale)[1]
    if not os.path.exists(f):
        return None
    import xarray as xr
    sys.path.insert(0, QUI)
    from fetch_woa import sound_speed, MIN_CELLE
    ds = xr.open_dataset(f, decode_times=False)
    z = ds.depth.values.astype(float)
    out = []
    for i in range(ds.sizes['stagione']):
        t = ds['t_an'].isel(stagione=i); s = ds['s_an'].isel(stagione=i)
        c, _ = sound_speed(t, s)
        n = (~np.isnan(t.values)).sum(axis=(1, 2))
        with warnings.catch_warnings():   # livelli sotto il fondo in tutte le celle
            warnings.simplefilter('ignore', RuntimeWarning)
            m = np.nanmedian(c, axis=(1, 2))
        out.append([float(x) if (not np.isnan(x) and k >= MIN_CELLE) else None for x, k in zip(m, n)])
    return z, out


def profilo(z, c, mese, deep):
    """Livelli validi del mese, prolungati con la stagione e poi con la pressione."""
    # solo il tratto continuo dalla superficie (i buchi a meta' colonna sono artefatti della mediana)
    n = next((i for i, b in enumerate(c) if b is None), len(c))
    zz = [float(a) for a in z[:n]]
    cc = [float(b) for b in c[:n]]
    ultimo = zz[-1]
    if deep is not None:
        zd, cd = deep[0], deep[1][STAGIONE[mese]]
        comune = [(a, b) for a, b in zip(zd, cd) if b is not None and abs(a - ultimo) < 1e-6]
        if comune:
            delta = cc[-1] - comune[0][1]
            for a, b in zip(zd, cd):
                if b is not None and a > ultimo + 1e-6:
                    zz.append(float(a)); cc.append(round(b + delta, 3))
    while zz[-1] + 1e-6 < PROFONDITA_MAX_M and zz[-1] >= 1000.0:   # acqua profonda: prosegue a pressione
        zn = min(PROFONDITA_MAX_M, zz[-1] + PASSO_PROFONDO_M)
        cc.append(round(cc[-1] + GRADIENTE_PRESSIONE * (zn - zz[-1]), 3)); zz.append(zn)
    return zz, cc


def esporta_suono(radice, uscita=None, quale=None):
    uscita = uscita or os.path.join(radice, 'data', 'motore', 'teatri')
    os.makedirs(uscita, exist_ok=True)
    fatti = []
    for teatro, (ph, traffico, mese, nota) in TEATRI.items():
        f, _, sotto = epoca(radice, teatro, quale)
        if not os.path.exists(f):
            continue
        d = json.load(open(f))
        deep = profondo(radice, teatro, quale)
        righe = [f'# Profili della velocita\' del suono del teatro {teatro} (generato da tools/pipeline/suono.py: non '
                 'modificare a mano).', f'# Fonte: {d["fonte"]}; sotto i dati mensili: {sotto}, poi pressione.',
                 '', f'teatro = "{teatro}"', f'ph = {ph}', f'traffico = {traffico}', f'mese_predefinito = {mese}',
                 f'nota = "{nota}"']
        piattaforma, scarpata, abisso = FONDI.get(teatro, ('sabbia', 'sabbia', 'fango'))
        righe += [f'fondo = {{ piattaforma = "{piattaforma}", scarpata = "{scarpata}", profondo = "{abisso}" }}', '']
        for m in range(1, 13):
            mm = d['mesi'][str(m)]
            zz, cc = profilo(d['profondita_m'], mm['c_teos10'], m, deep)
            t0 = next(x for x in mm['T'] if x is not None); s0 = next(x for x in mm['S'] if x is not None)
            righe += [f'[mesi.{m}]', f'temperatura_superficie = {t0}', f'salinita_superficie = {s0}',
                      'z_m = [' + ', '.join(f'{x:g}' for x in zz) + ']',
                      'c_m_s = [' + ', '.join(f'{x:.3f}' for x in cc) + ']', '']
        open(os.path.join(uscita, f'{teatro}.suono.toml'), 'w').write('\n'.join(righe))
        fatti.append(teatro)
    return fatti


if __name__ == '__main__':
    # suono.py <radice> [--uscita CARTELLA] [--epoca decav|8594]: con --uscita i .suono.toml vanno li' (una cartella
    # di prova)
    argv = sys.argv[1:]
    opz = {k: argv[argv.index(k) + 1] for k in ('--uscita', '--epoca') if k in argv}
    argv = [a for i, a in enumerate(argv) if a not in opz and opz.get(argv[i - 1] if i else None) != a]
    print('profili del suono:', ' '.join(esporta_suono(argv[0] if argv else '.', opz.get('--uscita'), opz.get('--epoca'))))
