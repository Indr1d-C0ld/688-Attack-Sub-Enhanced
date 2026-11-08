"""Interazioni fra unita' dell'IA (roadmap 31, docs/18_interazioni_ia.md; dossier docs/ricerca/24_interazioni_fra_unita.md)
-> data/motore/interazioni.toml, il file unico dei dati del piano. Ogni valore ha fonte e confidenza (24 §8.10).

Sezioni di questo generatore (fase 4, parte del nucleo):
- [tma.<marina>] e [tma.classi.<classe>]: le manovre di TMA dell'IA (nucleo/manovre_tma.hpp; dossier 02 §5.1-5.2);
- [armi_ia.<classe>]: il carico delle classi dell'IA che non hanno un locale in carico.toml (nucleo/armi_ia.hpp;
  dossier 01 §2, 14 §1);
- [filo_ia.<esperienza>]: la guida a filo dell'IA per livello del sonarista (nucleo/armi_ia.hpp; dossier 02 §6.2,
  §6.4).
Le altre sezioni del piano (allerta, reazioni, pace) le aggiunge il runtime in questo stesso file, con funzioni sue.
Uso: interazioni.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os
import sys

# --- manovre di TMA (dossier 02 §5.1: gambe di 3-6 minuti, piu' lunghe a grande distanza; manovra di 30-90 gradi per
# mettere il bersaglio a prua 30-60 gradi dall'altro lato; non manovrare verso il bersaglio vicino; la schiera si
# raddrizza in 2-5 minuti. 24 §5.2: TMA di 10-30 minuti con manovre di tratta)
# margine_fondo_m: una gamba che porta dove il fondo sta a meno di tanto sotto la quota si rifiuta (STIMA); tenuta_s:
# senza rilevamenti da tanto si rinuncia; gambe_max: senza soluzione dopo tante gambe si rinuncia; gambe_max_specchio:
# lo stesso col solo rilevamento a specchio irrisolto (02 §5.2: una virata di 20-40 gradi lo risolve); variazione_gamba:
# la durata della gamba varia a caso di +-meta' di tanto fra il minimo e il massimo (prevedibile no, ma ripetibile dal seme)
_T = dict(gamba_min_s=180.0, gamba_max_s=360.0, distanza_gamba_lunga_m=20000.0, mascone_deg=45.0, mascone_vicino_deg=100.0, virata_min_deg=30.0, virata_max_deg=90.0,
          vicino_m=4000.0, velocita_kn=6.0, velocita_max_kn=10.0, sigma_relativa=0.15, raddrizzamento_s=120.0,
          tolleranza_rotta_deg=5.0, margine_fondo_m=30.0, tenuta_s=600.0, gambe_max=8, gambe_max_specchio=3,
          variazione_gamba=0.3, fonte='', confidenza='')
TMA = {
    # il mascone a 45 gradi fa virate di 90 gradi fra una gamba e l'altra (30-90 del dossier); a meno di 4 km il
    # bersaglio va messo oltre il traverso, cosi' la gamba nuova apre invece di chiudere
    'nato': dict(_T, fonte='02 §5.1 [WMS] [Hervey]; distanze e velocita\' STIMA', confidenza='media'),
    # la TMA sovietica: meno sistematica e piu' lenta (sonar meno precisi, niente schiera sui piu' vecchi), soluzione
    # accettata con piu' incertezza
    'vmf': dict(_T, gamba_min_s=240.0, gamba_max_s=480.0, mascone_deg=60.0, velocita_kn=8.0, velocita_max_kn=12.0,
                sigma_relativa=0.2, raddrizzamento_s=180.0,
                fonte='02 §5.1; 24 §3.3 (coordinazione sovietica); STIMA DI GIOCO', confidenza='bassa'),
}
# per classe, sopra la marina: solo le differenze
TMA_CLASSI = {
    'alfa': dict(velocita_kn=12.0, velocita_max_kn=20.0, fonte='19 §3: l\'Alfa si fida della velocita\'; STIMA',
                 confidenza='bassa'),
    'kilo': dict(velocita_kn=4.0, velocita_max_kn=6.0, fonte='diesel in immersione: batteria; STIMA', confidenza='bassa'),
    'foxtrot': dict(velocita_kn=4.0, velocita_max_kn=6.0, fonte='diesel in immersione: batteria; STIMA',
                    confidenza='bassa'),
}
MARINE = {'usa': 'nato', 'uk': 'nato', 'nato': 'nato', 'urss': 'vmf', 'vmf': 'vmf'}

# --- carico delle classi dell'IA (le classi con un locale in carico.toml, 688 e Alfa, usano i suoi carichi standard).
# tubi_533 e tubi_650 dai dossier; armi: le varianti di carico.toml per i soli tubi da 533 (il 65-76 da 650 mm non e'
# ancora nel catalogo: i tubi da 650 restano vuoti); tubi: cosa c'e' nei tubi alla partenza. Composizione STIMA.
ARMI_IA = {
    'sturgeon': dict(nazione='usa', tubi_533=4, tubi_650=0, armi={'mk48': 19, 'harpoon': 4},
                     tubi=['mk48', 'mk48', 'mk48', 'mk48'],
                     fonte='01 §2.2 e 14 §1.4 [WPSturgeon]: 4 tubi, 23 armi, tipicamente 4 Harpoon', confidenza='media'),
    'trafalgar': dict(nazione='uk', tubi_533=5, tubi_650=0, armi={'tigerfish': 20, 'harpoon': 5},
                      tubi=['tigerfish'] * 5,
                      fonte='01 §2.3 e 14 §1.4 [WPTraf]: 5 tubi, 25-30 armi; composizione STIMA', confidenza='media'),
    'victor3': dict(nazione='urss', tubi_533=4, tubi_650=2,
                    armi={'uset80': 6, 'test71m': 4, 'set65': 4, 'type53_65k': 4},
                    tubi=['uset80', 'uset80', 'test71m', 'type53_65k'],
                    fonte='01 §2.5 e 14 §1.4 [WPVictor]: 4 x 533 + 2 x 650, 24 armi; i 6 da 650 non ci sono',
                    confidenza='media'),
    'akula': dict(nazione='urss', tubi_533=4, tubi_650=4,
                  armi={'uset80': 10, 'test71m': 6, 'set65': 6, 'type53_65k': 6},
                  tubi=['uset80', 'uset80', 'test71m', 'type53_65k'],
                  fonte='01 §2.6 [WPAkula]: 4 x 533 + 4 x 650, 40 armi; i 12 da 650 non ci sono', confidenza='media'),
    'oscar': dict(nazione='urss', tubi_533=4, tubi_650=2, armi={'uset80': 6, 'set65': 6, 'type53_65k': 8},
                  tubi=['uset80', 'set65', 'type53_65k', 'type53_65k'],
                  fonte='01 §2.7 [WPOscar]: 4 x 533 + 2 x 650, 28 armi (i P-700 a parte)', confidenza='media'),
    'typhoon': dict(nazione='urss', tubi_533=6, tubi_650=0, armi={'set65': 8, 'uset80': 6, 'type53_65k': 4},
                    tubi=['set65', 'set65', 'uset80', 'uset80', 'type53_65k', 'set65'],
                    fonte='01 §2.9 [WPTyphoon]: 6 x 533 (le fonti divergono); composizione STIMA', confidenza='bassa'),
    'charlie2': dict(nazione='urss', tubi_533=6, tubi_650=0, armi={'set65': 6, 'type53_65k': 6},
                     tubi=['set65', 'set65', 'set65', 'type53_65k', 'type53_65k', 'type53_65k'],
                     fonte='01 §2.8 [WPCharlie]: 6 x 533, 12 armi (le fonti russe 4 x 533 + 2 x 400)', confidenza='media'),
    'kilo': dict(nazione='urss', tubi_533=6, tubi_650=0, armi={'set65': 8, 'type53_65k': 6, 'test71m': 4},
                 tubi=['set65', 'set65', 'test71m', 'test71m', 'type53_65k', 'type53_65k'],
                 fonte='01 §2.10 [WPKilo]: 6 x 533, 18 siluri; composizione STIMA', confidenza='media'),
    'foxtrot': dict(nazione='urss', tubi_533=10, tubi_650=0, armi={'saet60m': 6, 'set65': 8, 'type53_65k': 8},
                    tubi=['set65', 'set65', 'saet60m', 'saet60m', 'type53_65k', 'type53_65k',
                          'set65', 'set65', 'type53_65k', 'type53_65k'],
                    fonte='01 §2.11 [WPFoxtrot]: 10 x 533 (6 a prua, 4 a poppa), 22 siluri; composizione STIMA',
                    confidenza='media'),
}

# --- guida a filo dell'IA per esperienza (02 §6.2 punto 4: il filo serve a correggere la rotta, rifiutare un'esca,
# riassegnare il bersaglio; si taglia all'acquisizione finale o quando il sottomarino deve manovrare. §6.4: il bersaglio
# evade con virate e cambi di quota, e l'esca resta dove il bersaglio «era»). Tempi e soglie: STIMA DI GIOCO
_F = dict(correzione_s=20.0, soglia_deg=8.0, taglio_dopo_aggancio_s=30.0, distanza_taglio_m=1500.0,
          rifiuta_esche=True, scarto_esca_deg=15.0, attivo_su_perdita=True, fonte='', confidenza='bassa')
FILO_IA = {
    'recluta': dict(_F, correzione_s=60.0, soglia_deg=15.0, taglio_dopo_aggancio_s=10.0, rifiuta_esche=False,
                    attivo_su_perdita=False, fonte='02 §6.2; STIMA: corregge di rado e si fida della testa'),
    'medio': dict(_F, fonte='02 §6.2, §6.4; STIMA'),
    'esperto': dict(_F, correzione_s=10.0, soglia_deg=5.0, taglio_dopo_aggancio_s=60.0, distanza_taglio_m=1000.0,
                    scarto_esca_deg=10.0, fonte='02 §6.2, §6.4 [Mk48]: guida fino all\'ultimo e rifiuta le esche; STIMA'),
}


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, (int, float)):
        return repr(float(v)) if isinstance(v, float) else str(v)
    if isinstance(v, str):
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list):
        return '[' + ', '.join(valore(x) for x in v) + ']'
    if isinstance(v, dict):
        return '{ ' + ', '.join(f'{k} = {valore(x)}' for k, x in v.items()) + ' }'
    raise TypeError(v)


def esporta_interazioni(radice):
    import tomllib
    carico = tomllib.load(open(os.path.join(radice, 'data', 'motore', 'carico.toml'), 'rb'))
    righe = ['# Interazioni fra unita\' dell\'IA (generato da tools/pipeline/interazioni.py: non modificare a mano).',
             '# Fase 4, parte del nucleo: manovre di TMA, carico e guida a filo dell\'IA (docs/18_interazioni_ia.md).', '']
    righe += ['[tma.marine]', *(f'{m} = {valore(d)}' for m, d in MARINE.items()), '']
    for m, d in TMA.items():
        assert set(d) == set(_T), m
        righe += [f'[tma.{m}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    for c, d in TMA_CLASSI.items():
        assert set(d) <= set(_T), c
        righe += [f'[tma.classi.{c}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    for c, d in ARMI_IA.items():
        for a in list(d['armi']) + d['tubi']:
            assert a in carico['armi'], (c, a)
        assert len(d['tubi']) == d['tubi_533'], c
        righe += [f'[armi_ia.{c}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    for e, d in FILO_IA.items():
        assert set(d) == set(_F), e
        righe += [f'[filo_ia.{e}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from interazioni_forze import righe_forze   # le sezioni del runtime (fase 3: allerta, messaggi, zone, ROE)
    righe += righe_forze(valore)
    open(os.path.join(radice, 'data', 'motore', 'interazioni.toml'), 'w').write('\n'.join(righe))
    return len(TMA) + len(TMA_CLASSI), len(ARMI_IA), len(FILO_IA)


if __name__ == '__main__':
    print('dottrine di TMA, carichi dell\'IA, guide a filo:', esporta_interazioni(sys.argv[1] if len(sys.argv) > 1 else '.'))
