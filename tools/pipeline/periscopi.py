"""Periscopi dell'Enhanced (docs/14_periscopi.md, pezzo P0) -> data/motore/periscopi.toml.
Lo legge nucleo/periscopi.hpp (DatiPeriscopio, e per classe l'elenco dei periscopi); la vista e' grafica/periscopio.
Dati dal dossier docs/ricerca/20_periscopi.md, citato per paragrafo; le sigle delle fonti sono quelle del §9.

Ogni voce porta:
- `fonte`: le sigle delle fonti della voce (il paragrafo del dossier e le sigle del §9);
- `conf`: alta, media o bassa (dossier §0);
- `stime`: i campi che sono STIMA, cioe' valori proposti da tarare (pezzo P5), non storici;
- `fonti` (facoltativo): la fonte dei campi che non vengono dalla fonte della voce.
Unita': metri, secondi, nodi, gradi; m^2 per la sezione radar (testa esposta per 1 m, banda X).

Le velocita' sono soglie col periscopio alzato: sotto `v_libera_kn` niente tremolio; il tremolio sale linearmente
fino a 0,5 a `v_tremolio_kn` (la piuma si vede) e a 1 a `v_inservibile_kn` (immagine inservibile); oltre
`v_danno_kn` il sistema «periscopio» rischia il danno. `stabilizzato` dimezza il tremolio della vista.
`reticolo_gradi` e `reticolo_fine_gradi` sono i passi delle divisioni grandi e piccole, uno per ingrandimento.
Uso: periscopi.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import valore   # noqa: E402

FT = 0.3048
HYD = 91.44   # centinaia di yarde -> m
IN = 0.0254

MODI = ('giorno', 'bassa_luminosita', 'intensificatore', 'tv', 'termico')
TELEMETRI = ('sdoppiato', 'reticolo')
RETICOLI = ('americano', 'sovietico', 'britannico')

# Campi di ogni periscopio (i mancanti prendono il valore di BASE); lo stesso ordine del contratto di docs/14
BASE = {'nome': '', 'ruolo': 'attacco', 'nazione': 'usa', 'ingrandimenti': [1.5, 6.0], 'campi_gradi': [32.0, 8.0],
        'elevazione_min': -10.0, 'elevazione_max': 60.0, 'telemetro': 'sdoppiato',
        'telemetro_altezza_m': [0.0, 0.0], 'telemetro_distanza_m': [0.0, 0.0], 'telemetro_angolo_max_gradi': 0.0,
        'modi': ['giorno'], 'tv': False, 'foto': False, 'esm': False, 'stabilizzato': False,
        'testa_m': 0.06, 'sezione_m2': 0.2, 'piuma': 0.5, 'alzata_s': 6.0, 'discesa_s': 5.0,
        'v_libera_kn': 6.0, 'v_tremolio_kn': 8.0, 'v_inservibile_kn': 10.0, 'v_danno_kn': 15.0,
        'reticolo': 'americano', 'reticolo_gradi': [4.0, 1.0], 'reticolo_fine_gradi': [1.0, 0.25],
        'fermi_gradi': [], 'filtri': [],
        # l'ottica per i punti luminosi di notte (nucleo/portata_luci, luci::guadagno_ottico; 29/10/2026): l'apertura
        # utile dell'obiettivo e la trasmissione dell'ottica lunga. STIMA per tutti: a 6x guadagno 5,5, a 1,5x 0,68
        'obiettivo_mm': 30.0, 'trasmissione': 0.3,
        'fonte': '', 'conf': 'media', 'stime': [], 'fonti': {}}

# Stadimetro del Type 2 (§2.1, [PSM]): altezza 15-130 ft, distanza 2,2-110 centinaia di yarde, angolo massimo 2°
STADIMETRO = dict(telemetro='sdoppiato', telemetro_altezza_m=[round(15 * FT, 2), round(130 * FT, 2)],
                  telemetro_distanza_m=[round(2.2 * HYD), round(110 * HYD)], telemetro_angolo_max_gradi=2.0)
FILTRI_USA = ['polaroid', 'rosso', 'verde', 'giallo']
FILTRI_URSS = ['affumicato', 'rosso']   # disco a tre settori davanti all'oculare (§4.3 [RAN]): chiaro, fumo, rosso
# STIME comuni ai periscopi sovietici del dopoguerra (§4.2): ottica come i ПЗ di guerra, reticolo a griglia
# Il telemetro a reticolo (kit, 31/10/2026): l'altezza del bersaglio si sceglie a mano (niente quadrante: i limiti sono
# solo quelli di una nave vera) e si contano le divisioni fra il galleggiamento sulla linea di mezzo e la testa d'albero,
# fino alla meta' della scala piccola (la mezza scala grande del 1,5x); STIMA
SOVIETICO = dict(nazione='urss', ingrandimenti=[1.5, 6.0], campi_gradi=[40.0, 10.0], telemetro='reticolo',
                 reticolo='sovietico', reticolo_gradi=[5.0, 1.0], reticolo_fine_gradi=[1.0, 0.2], filtri=FILTRI_URSS,
                 telemetro_altezza_m=[3.0, 80.0], telemetro_distanza_m=[100, 15000], telemetro_angolo_max_gradi=5.0)
STIME_SOVIETICHE = ['ingrandimenti', 'campi_gradi', 'elevazione_min', 'elevazione_max', 'reticolo_gradi',
                    'reticolo_fine_gradi', 'testa_m', 'sezione_m2', 'piuma', 'alzata_s', 'discesa_s', 'v_libera_kn',
                    'v_tremolio_kn', 'v_inservibile_kn', 'v_danno_kn', 'obiettivo_mm', 'trasmissione',
                    'telemetro_altezza_m', 'telemetro_distanza_m', 'telemetro_angolo_max_gradi']
STIME_GIOCO = ['sezione_m2', 'piuma', 'alzata_s', 'discesa_s', 'v_libera_kn', 'v_tremolio_kn', 'v_inservibile_kn',
               'v_danno_kn', 'obiettivo_mm', 'trasmissione']

PERISCOPI = {
    # --- USA (§2) ---
    'type2': dict(nome='Type 2 (attacco)', **STADIMETRO, ingrandimenti=[1.5, 6.0], campi_gradi=[32.0, 8.0],
                  elevazione_max=74.5, testa_m=round(2.4 * IN, 3), fermi_gradi=[14.0, 44.0], filtri=FILTRI_USA,
                  fonte='§2.1 [PSM] [NSL2] [NOVA-P]', conf='alta', stime=STIME_GIOCO,
                  fonti={'testa_m': '§2.1 [NSL2] (Type 2D, 1959)', 'sezione_m2': '§5 §8.1 (calcolo)',
                         'filtri': '§2.1 [PSM] [JTM]'}),
    'type18': dict(nome='Type 18 (ricerca)', ruolo='ricerca', **STADIMETRO, ingrandimenti=[1.5, 6.0, 12.0],
                   campi_gradi=[32.0, 8.0, 4.0], elevazione_max=60.0,
                   modi=['giorno', 'bassa_luminosita', 'tv'], tv=True, foto=True, esm=True, stabilizzato=True,
                   testa_m=round(7.5 * IN, 3), sezione_m2=1.5, piuma=1.0, alzata_s=8.0, discesa_s=6.0,
                   reticolo_gradi=[4.0, 1.0, 0.5], reticolo_fine_gradi=[1.0, 0.25, 0.125], filtri=FILTRI_USA,
                   fonte='§2.1 [PAT] [K18] [NSL2] [NOVA-P]', conf='media',
                   stime=STIME_GIOCO + ['testa_m', 'reticolo_gradi', 'reticolo_fine_gradi', 'filtri'],
                   fonti={'ingrandimenti': '§2.1 [PAT] (alta)', 'esm': '§2.1 [NSL2] [FY98] (alta)',
                          'modi': '§2.1 [NSL2]: forma del modo notte nel 1989 n.v.; TV in bianco e nero'}),
    # --- Regno Unito (§3) ---
    'ch84': dict(nome='CH84 (attacco)', nazione='uk', **STADIMETRO,
                 modi=['giorno', 'intensificatore', 'tv', 'termico'], tv=True, foto=True, stabilizzato=True,
                 testa_m=0.08, sezione_m2=0.3, piuma=0.6, elevazione_max=60.0, reticolo='britannico',
                 filtri=FILTRI_USA, fonte='§3 [RNS]', conf='media',
                 stime=STIME_GIOCO + ['ingrandimenti', 'campi_gradi', 'elevazione_max', 'testa_m', 'foto',
                                      'stabilizzato', 'reticolo_gradi', 'reticolo_fine_gradi', 'filtri',
                                      'telemetro_altezza_m', 'telemetro_distanza_m', 'telemetro_angolo_max_gradi']),
    'ck34': dict(nome='CK34 (ricerca)', nazione='uk', ruolo='ricerca', **STADIMETRO,
                 modi=['giorno', 'bassa_luminosita', 'tv'], tv=True, foto=True, esm=True, stabilizzato=True,
                 testa_m=0.19, sezione_m2=1.5, piuma=1.0, alzata_s=8.0, discesa_s=6.0, reticolo='britannico',
                 filtri=FILTRI_USA, fonte='§3 [RNS] (Jane\'s)', conf='bassa',
                 stime=STIME_GIOCO + ['ingrandimenti', 'campi_gradi', 'elevazione_max', 'testa_m', 'stabilizzato',
                                      'reticolo_gradi', 'reticolo_fine_gradi', 'filtri', 'telemetro_altezza_m',
                                      'telemetro_distanza_m', 'telemetro_angolo_max_gradi']),
    # --- URSS (§4): ruoli e dotazione da [APA]; ottica e prestazioni n.v. (STIME sui ПЗ di guerra, §4.2) ---
    'signal': dict(SOVIETICO, nome='«Сигнал» (universale, sistema ТВ-1)', ruolo='universale',
                   elevazione_min=-15.0, modi=['giorno', 'intensificatore', 'tv'], tv=True, foto=True,
                   testa_m=0.18, sezione_m2=1.0, piuma=1.0, alzata_s=8.0, discesa_s=6.0,
                   fonte='§4.1 §4.4 [APA] [DS705] [SOB] [KOL705]', conf='media',
                   stime=STIME_SOVIETICHE + ['modi', 'foto'],
                   fonti={'modi': '§4.2 [MM]: canale notturno raffreddato dal 1965 (plausibile, STIMA)'}),
    'signal3': dict(SOVIETICO, nome='«Сигнал-3» (universale TV)', ruolo='universale', elevazione_min=-15.0,
                    modi=['giorno', 'intensificatore', 'tv'], tv=True, foto=True, testa_m=0.18, sezione_m2=1.0,
                    piuma=1.0, alzata_s=8.0, discesa_s=6.0, fonte='§4.1 [APA] [RW-971] [RW-949]', conf='media',
                    stime=STIME_SOVIETICHE + ['modi', 'foto']),
    'lebed': dict(SOVIETICO, nome='«Лебедь-11/21» (del comandante)', elevazione_max=45.0,
                  modi=['giorno', 'intensificatore'], testa_m=0.07, sezione_m2=0.3, piuma=0.6,
                  fonte='§4.1 [APA] [RW-971]', conf='media', stime=STIME_SOVIETICHE + ['modi']),
    'pzns10': dict(SOVIETICO, nome='ПЗНС-10 (universale, zenitale)', ruolo='universale', elevazione_min=-15.0,
                   elevazione_max=90.0, modi=['giorno', 'intensificatore'], foto=True, testa_m=0.15,
                   sezione_m2=1.0, piuma=1.0, alzata_s=8.0, discesa_s=6.0, fonte='§4.1 §4.2 [APA] [MM]',
                   conf='media', stime=STIME_SOVIETICHE + ['foto'],
                   fonti={'modi': '§4.2 [MM]: ПЗНС del 1965 con intensificatore raffreddato (media-bassa)'}),
    'pzng10m': dict(SOVIETICO, nome='ПЗНГ-10М (zenitale e di navigazione)', ruolo='universale',
                    elevazione_min=-15.0, elevazione_max=90.0, modi=['giorno', 'intensificatore'], foto=True,
                    testa_m=0.15, sezione_m2=1.0, piuma=1.0, alzata_s=8.0, discesa_s=6.0, fonte='§4.1 [APA]',
                    conf='media', stime=STIME_SOVIETICHE + ['modi', 'foto']),
    'mt70': dict(SOVIETICO, nome='МТ-70 (complesso TV)', ruolo='tv', ingrandimenti=[1.0], campi_gradi=[30.0],
                 elevazione_max=30.0, modi=['tv'], tv=True, reticolo_gradi=[5.0],
                 reticolo_fine_gradi=[1.0], testa_m=0.12, sezione_m2=0.5, piuma=0.8, alzata_s=8.0, discesa_s=6.0,
                 fonte='§4.1 §4.2 [APA] [PAV]', conf='media', stime=STIME_SOVIETICHE + ['modi'],
                 fonti={'modi': '§4.2 [PAV]: TV della superficie e dei ghiacci, fino a ~50 m di quota'}),
    'pk85': dict(SOVIETICO, nome='ПК-8,5 (d\'attacco)', elevazione_max=45.0, testa_m=0.06, sezione_m2=0.2,
                 piuma=0.5, fonte='§4.1 [APA] [KP]', conf='media', stime=STIME_SOVIETICHE),
    'pzng8': dict(SOVIETICO, nome='ПЗНГ-8 (zenitale e di navigazione)', ruolo='universale',
                  elevazione_min=-15.0, elevazione_max=90.0, testa_m=0.15, sezione_m2=1.0, piuma=1.0, alzata_s=8.0,
                  discesa_s=6.0, fonte='§4.1 §4.2 [APA]', conf='media', stime=STIME_SOVIETICHE),
    'pzng8m': dict(SOVIETICO, nome='ПЗНГ-8М (zenitale e di navigazione)', ruolo='universale',
                   elevazione_min=-15.0, elevazione_max=90.0, modi=['giorno', 'intensificatore'], testa_m=0.15,
                   sezione_m2=1.0, piuma=1.0, alzata_s=8.0, discesa_s=6.0, fonte='§4.1 §4.2 [APA]', conf='media',
                   stime=STIME_SOVIETICHE + ['modi']),
}

# Per classe di classi.toml (§2-4, §8.3). quota_m: quota periscopica alla chiglia; esposto_m: altezza della finestra
# sull'acqua calma. Esposizioni dell'IA (§7, §8.3): `esposizione_s` a guardata, una ogni `intervallo_s` (min, max);
# `avvisatore_prima`: l'antenna dell'avvisatore radar sale prima del periscopio, a `avvisatore_quota_m` ([SPR74]).
# tv: il complesso TV con la quota massima a cui guarda la superficie; telecamera_verticale: quella della vela (Alfa).
BASE_CLASSE = {'periscopi': [], 'quota_m': 18.3, 'esposto_m': 1.5, 'esposizione_s': 10.0,
               'intervallo_s': [180.0, 720.0], 'avvisatore_prima': False, 'avvisatore_quota_m': 0.0,
               'tv': '', 'tv_quota_max_m': 0.0, 'telecamera_verticale': False,
               'fonte': '', 'conf': 'media', 'stime': [], 'fonti': {}}
URSS = dict(avvisatore_prima=True, avvisatore_quota_m=17.0, quota_m=13.0, esposto_m=1.2,
            intervallo_s=[180.0, 600.0])
STIME_IA = ['esposizione_s', 'intervallo_s']
CLASSI = {
    'los_angeles': dict(periscopi=['type2', 'type18'], quota_m=round(60 * FT, 1), fonte='§2.1 [NSL2] [NTSB] [GRIF]',
                        conf='alta', stime=STIME_IA, fonti={'esposto_m': '§2.1 [NTSB] [GRIF] (1,4-2 m)'}),
    'sturgeon': dict(periscopi=['type2', 'type18'], quota_m=round(60 * FT, 1), fonte='§2.2 [WIKI-637] [NMVM]',
                     conf='media', stime=STIME_IA + ['quota_m', 'esposto_m']),
    'trafalgar': dict(periscopi=['ch84', 'ck34'], quota_m=18.0, fonte='§3 [RNS] [SD]', conf='media',
                      stime=STIME_IA + ['esposto_m'], intervallo_s=[180.0, 720.0],
                      fonti={'intervallo_s': '§7 [SD] (Perisher: 12 minuti)'}),
    'alfa': dict(periscopi=['signal'], **URSS, telecamera_verticale=True, fonte='§4.1 §4.4 [APA] [DS705] [KOL705]',
                 conf='media', stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'victor3': dict(periscopi=['pzng10m', 'mt70'], **URSS, tv='mt70', tv_quota_max_m=50.0, fonte='§4.1 [APA] [PAV]',
                    conf='media', stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'charlie2': dict(periscopi=['pzns10', 'mt70'], **URSS, tv='mt70', tv_quota_max_m=50.0, fonte='§4.1 [APA] [PAV]',
                     conf='media', stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'akula': dict(periscopi=['lebed', 'signal3'], **URSS, fonte='§4.1 [APA] [RW-971]', conf='media',
                  stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'oscar': dict(periscopi=['pzns10', 'signal3'], **URSS, fonte='§4.1 [RW-949]', conf='media',
                  stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'typhoon': dict(periscopi=['lebed', 'signal3'], **URSS, fonte='§4.1 [APA]', conf='media',
                    stime=STIME_IA + ['quota_m', 'esposto_m', 'avvisatore_quota_m']),
    'kilo': dict(periscopi=['pk85', 'pzng8m'], **URSS, fonte='§4.1 §7 [APA] [SPR74]', conf='media',
                 stime=STIME_IA + ['esposto_m'], fonti={'quota_m': '§7 [SPR74] (periscopio a 12-14 m)'}),
    'foxtrot': dict(periscopi=['pk85', 'pzng8'], **URSS, fonte='§4.1 §7 [APA] [SPR74]', conf='media',
                    stime=STIME_IA + ['esposto_m'], fonti={'quota_m': '§7 [SPR74] (periscopio a 12-14 m)'}),
}


def _campo(k, v):
    if isinstance(v, list): return f'{k} = [' + ', '.join(valore(x) for x in v) + ']'
    if isinstance(v, dict): return f'{k} = {{ ' + ', '.join(f'{a} = {valore(b)}' for a, b in v.items()) + ' }'
    return f'{k} = {valore(v)}'


def _voce(sezione, chiave, campi, base):
    ignoti = set(campi) - set(base)
    assert not ignoti, (chiave, ignoti)
    campi = {**base, **campi}
    assert all(s in base for s in campi['stime']), (chiave, campi['stime'])
    assert all(f in base for f in campi['fonti']), (chiave, campi['fonti'])
    assert campi['fonte'] and campi['conf'] in ('alta', 'media', 'bassa'), chiave
    return [f'[{sezione}.{chiave}]', *(_campo(k, v) for k, v in campi.items()), '']


def _controlla(p, c):
    n = len(c['ingrandimenti'])
    assert n and len(c['campi_gradi']) == n == len(c['reticolo_gradi']) == len(c['reticolo_fine_gradi']), p
    assert c['ingrandimenti'] == sorted(c['ingrandimenti']), p
    assert all(m in MODI for m in c['modi']) and c['modi'], p
    assert c['telemetro'] in TELEMETRI and c['reticolo'] in RETICOLI, p
    assert c['elevazione_min'] < 0 < c['elevazione_max'] <= 90, p
    assert c['v_libera_kn'] <= c['v_tremolio_kn'] <= c['v_inservibile_kn'] <= c['v_danno_kn'], p
    assert c['tv'] == ('tv' in c['modi']), p
    assert all(c['elevazione_min'] < f < c['elevazione_max'] for f in c['fermi_gradi']), p


def esporta_periscopi(radice):
    righe = ['# Periscopi dell\'Enhanced (generato da tools/pipeline/periscopi.py: non modificare a mano).',
             '# Dati dal dossier docs/ricerca/20_periscopi.md (paragrafi e sigle delle fonti del §9).',
             '# conf: alta, media, bassa; stime: i campi STIMA da tarare (pezzo P5); fonti: le fonti per campo.',
             '# Velocita\': sotto v_libera niente tremolio; 0,5 a v_tremolio; 1 a v_inservibile; danno oltre v_danno.',
             '# reticolo_gradi / reticolo_fine_gradi: passi delle divisioni grandi e piccole, uno per ingrandimento.', '']
    for p, campi in PERISCOPI.items():
        _controlla(p, {**BASE, **campi})
        righe += _voce('periscopi', p, campi, BASE)
    classi = open(os.path.join(radice, 'data', 'motore', 'classi.toml')).read()
    for c, campi in CLASSI.items():
        assert f'\n[{c}]\n' in classi, c
        assert campi['periscopi'] and all(p in PERISCOPI for p in campi['periscopi']), c
        assert not campi.get('tv') or campi['tv'] in campi['periscopi'], c
        righe += _voce('classi', c, campi, BASE_CLASSE)
    uscita = os.path.join(radice, 'data', 'motore', 'periscopi.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(PERISCOPI), len(CLASSI)


if __name__ == '__main__':
    print('periscopi, classi:', esporta_periscopi(sys.argv[1] if len(sys.argv) > 1 else '.'))
