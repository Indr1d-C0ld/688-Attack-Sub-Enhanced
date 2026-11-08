"""Artiglieria navale contro il sottomarino (docs/16_artiglieria.md, pezzo G0) -> data/motore/artiglieria.toml.
La legge nucleo/artiglieria.hpp (carica_artiglieria); i campi sono quelli di DatiPezzo e SagomaClasse. Numeri dal
dossier docs/ricerca/22_artiglieria_contro_sottomarini.md (citato «22 §»): dati dei pezzi da NavWeaps (§3.1), distanza
minima dalla depressione (§3.3), tavole di volo (§3.4, calcolate), precisione e correzioni (§3.6, STIMA), reazione
(§4.2, STIMA), schegge efficaci sulla testa (§5.3, STIMA), portate contro il periscopio (§9.2, STIMA). Il Phalanx del
1989 non ha un modo di superficie: non c'e'. Prima versione del runtime; la rivede il kit con i danni (G3).
Uso: artiglieria.py <radice del progetto>"""
import os, sys

CAMPI = ['nome', 'calibro_mm', 'canne', 'cadenza_min', 'colpi_pronti', 'ricarica_s', 'carica_kg', 'v0_m_s',
         'altezza_m', 'depressione_gradi', 'portata_minima_m', 'portata_superficie_m', 'portata_periscopio_m',
         'reazione_combattimento_s', 'reazione_condizione3_s', 'sigma_direzione_mrad', 'sigma_distanza_m',
         'sigma_distanza_frazione', 'r_eff_m', 'spoletta', 'raffica_colpi', 'raffica_s', 'fonte', 'confidenza']

# tavole di volo: distanza m, tempo di volo s, angolo di caduta gradi, velocita' d'arrivo m/s (22 §3.4: calcolate per
# Mk 75, Mk 45, AK-130 e AK-725; le altre per analogia, STIMA)
VOLO = {
    'mk75': [[1000, 1.1, 0.8, 820], [2000, 2.4, 1.1, 720], [3000, 3.9, 1.8, 640], [5000, 7.5, 4.0, 500],
             [8000, 15.0, 12.0, 320], [10000, 22.0, 21.0, 280]],
    'mk45': [[1000, 1.3, 0.9, 750], [2000, 2.7, 1.3, 700], [3000, 4.1, 1.9, 655], [5000, 7.4, 3.6, 570],
             [8000, 13.0, 7.6, 455], [10000, 18.0, 12.0, 390]],
    'mk12_5_38': [[1000, 1.4, 1.0, 700], [2000, 3.0, 1.5, 640], [3000, 4.8, 2.4, 580], [5000, 8.8, 4.8, 480],
                  [8000, 15.5, 10.0, 380], [10000, 21.0, 15.0, 330]],
    'mk7_16': [[2000, 2.7, 0.6, 750], [5000, 7.0, 1.8, 700], [10000, 15.0, 5.0, 640], [15000, 24.0, 9.0, 590],
               [20000, 33.0, 14.0, 550], [30000, 55.0, 25.0, 480]],
    'ak100': [[1000, 1.2, 0.9, 790], [2000, 2.5, 1.2, 740], [3000, 3.9, 1.7, 690], [5000, 7.0, 3.2, 605],
              [8000, 12.6, 6.7, 490], [10000, 17.0, 10.0, 420]],
    'ak130': [[1000, 1.2, 0.9, 790], [2000, 2.5, 1.2, 740], [3000, 3.9, 1.7, 690], [5000, 7.0, 3.2, 605],
              [8000, 12.6, 6.7, 490], [10000, 17.0, 10.0, 420]],
    'ak726': [[1000, 1.1, 0.8, 820], [2000, 2.4, 1.1, 720], [3000, 3.9, 1.8, 640], [5000, 7.5, 4.0, 500],
              [8000, 15.0, 12.0, 320]],
    'ak725': [[1000, 1.1, 0.8, 860], [2000, 2.3, 1.1, 740], [3000, 3.8, 1.7, 630], [5000, 7.5, 4.3, 460],
              [8000, 16.0, 15.0, 290]],
    'ak630': [[500, 0.6, 0.6, 780], [1000, 1.3, 1.0, 650], [2000, 3.0, 1.8, 480], [3000, 5.2, 3.5, 360],
              [4000, 8.0, 6.0, 300]],
    'ak230': [[500, 0.6, 0.6, 780], [1000, 1.3, 1.0, 650], [2000, 3.0, 1.8, 480], [3000, 5.2, 3.5, 360],
              [4000, 8.0, 6.0, 300]],
    # i pezzi della Knox e delle nordiche (ricerca 36, principale 31/10/2026): per analogia, STIMA. Il Mk 42 spara il
    # proietto del Mk 45 alla stessa v0; il 3"/50 il 76 piu' lento del Mk 75 (823 contro 925 m/s: tempi x1,12, velocita'
    # x0,89); il Bofors 57 come l'AK-725; il Bofors 40 come il 30 mm, un po' piu' veloce e piu' lontano
    'mk42': [[1000, 1.3, 0.9, 750], [2000, 2.7, 1.3, 700], [3000, 4.1, 1.9, 655], [5000, 7.4, 3.6, 570],
             [8000, 13.0, 7.6, 455], [10000, 18.0, 12.0, 390]],
    'mk33': [[1000, 1.2, 0.9, 730], [2000, 2.7, 1.3, 640], [3000, 4.4, 2.1, 570], [5000, 8.4, 4.6, 445],
             [8000, 16.8, 13.5, 285]],
    'bofors57': [[1000, 1.1, 0.8, 860], [2000, 2.3, 1.1, 740], [3000, 3.8, 1.7, 630], [5000, 7.5, 4.3, 460],
                 [8000, 16.0, 15.0, 290]],
    'bofors40': [[500, 0.5, 0.5, 860], [1000, 1.2, 0.9, 720], [2000, 2.7, 1.6, 540], [3000, 4.6, 3.0, 410],
                 [4000, 7.0, 5.0, 330]],
}

# pezzo: valori nell'ordine di CAMPI. Distanza minima = altezza / tan(depressione) (22 §3.3). Reazione al primo colpo
# (22 §4.2): 15-40 s al posto di combattimento, 1-3 min in condizione III. Precisione (22 §3.6): 1-2 mrad, 15-30 m +
# 0,5% R; le raffiche 2-3 mrad. Schegge efficaci sulla testa (22 §5.3): 2 m col 76 mm, 4 m col 5"/130, 1 m col 30 mm
PEZZI = {
    'mk75': ('Mk 75 76/62 (OTO Compact)', 76.0, 1, 80.0, 80, 180.0, 0.75, 925.0, 8.0, -15.0, 30.0, 8000.0, 4000.0,
             15.0, 90.0, 1.5, 20.0, 0.005, 2.0, 'pd', 0, 0.0,
             'NW-Mk75 (22 §3.1); tiro STIMA (22 §3.6); il tamburo da 70 si rifa\' a mano, STIMA 3 min (36 §2)',
             'alta (dati), STIMA (tiro)'),
    'mk45': ('Mk 45 5"/54', 127.0, 1, 20.0, 20, 0.0, 3.5, 808.0, 7.0, -15.0, 30.0, 10000.0, 5000.0, 20.0, 120.0,
             1.5, 20.0, 0.005, 4.0, 'pd', 0, 0.0,
             'NW-Mk45 (22 §3.1, 36 §2): 20 colpi nel tamburo, rifornito di continuo durante il tiro da 6 serventi',
             'alta (dati), STIMA (tiro)'),
    'mk12_5_38': ('5"/38 Mk 12 (Iowa, binato)', 127.0, 2, 15.0, 40, 20.0, 3.3, 762.0, 10.0, -15.0, 40.0, 10000.0,
                  5000.0, 20.0, 120.0, 2.0, 25.0, 0.005, 4.0, 'pd', 0, 0.0,
                  'NW-538 (22 §3.1); direzione del tiro Mk 37 piu\' vecchia: sigma 2 mrad (STIMA)', 'alta (dati), STIMA (tiro)'),
    'mk7_16': ('16"/50 Mk 7 (Iowa, torre trinata)', 406.0, 3, 2.0, 9, 0.0, 69.7, 762.0, 11.0, -2.0, 2000.0,
               20000.0, 0.0, 60.0, 300.0, 1.0, 100.0, 0.005, 8.0, 'pd', 0, 0.0,
               'NW-1650 (22 §3.1, §9.2): mai al periscopio; al battello in superficie solo oltre 2 km, evento raro',
               'alta (dati), STIMA (tiro)'),
    'ak100': ('AK-100 100/70', 100.0, 1, 50.0, 60, 0.0, 1.53, 880.0, 7.0, -10.0, 40.0, 10000.0, 5000.0, 20.0, 120.0,
              1.5, 20.0, 0.005, 3.0, 'pd', 0, 0.0,
              'NW-AK100 (22 §3.1); portata efficace n.v., STIMA 10 km; contro il periscopio la percussione (22 §3.5)',
              'media (dati), STIMA (tiro)'),
    'ak130': ('AK-130 130/70 (binato)', 130.0, 2, 30.0, 180, 0.0, 3.56, 850.0, 8.0, -12.0, 40.0, 10000.0, 5000.0,
              15.0, 90.0, 1.5, 20.0, 0.005, 4.0, 'pd', 0, 0.0,
              'NW-AK130 (22 §3.1): 10-40 colpi/min per canna, 150-180 pronti; portata ridotta a 10 km (22 §10)',
              'media (dati), STIMA (tiro)'),
    'ak726': ('AK-726 76/59 (binato)', 76.0, 2, 22.5, 45, 180.0, 0.4, 980.0, 8.0, -2.0, 230.0, 8200.0, 4000.0, 20.0,
              120.0, 1.5, 20.0, 0.005, 2.0, 'pd', 0, 0.0,
              'NW-726 (22 §3.1, §3.3): 45 colpi/min per affusto; raffiche di 40-45 colpi per canna, poi 3 min di '
              'raffreddamento ad acqua di mare (RU-726, 36 §2); '
              'depressione -2 con l\'idraulica: 230 m di distanza minima', 'media (dati), STIMA (tiro)'),
    'ak725': ('AK-725 57/75 (binato)', 57.0, 2, 35.0, 100, 120.0, 0.154, 1000.0, 6.0, -4.0, 85.0, 6000.0, 3000.0, 15.0,
              90.0, 1.5, 20.0, 0.005, 1.5, 'pd', 0, 0.0,
              'WP-AK725 (22 §3.1): 70 colpi/min sostenuti per affusto; 6 km col radar; raffica massima 100 colpi per '
              'canna, poi STIMA 2 min (RU-725, 36 §4)', 'media-bassa (dati), STIMA (tiro)'),
    'ak630': ('AK-630 30 mm', 30.0, 1, 12.0, 20, 30.0, 0.0485, 900.0, 8.0, -12.0, 40.0, 4000.0, 2000.0, 15.0, 60.0,
              2.5, 15.0, 0.005, 1.0, 'raffica', 100, 1.5,
              'RU-AK630, NW-AK630 (22 §3.1, §3.2): raffiche di 1-3 s a 4000-5000 colpi/min, una ogni 5 s (STIMA); '
              'al periscopio entro 2 km col radar MR-123 agganciato (22 §9.2)', 'media (dati), STIMA (tiro)'),
    'ak230': ('AK-230 30 mm (binato)', 30.0, 2, 12.0, 20, 30.0, 0.0485, 900.0, 7.0, -12.0, 40.0, 4000.0, 2000.0, 15.0,
              60.0, 2.5, 15.0, 0.005, 1.0, 'raffica', 50, 1.5,
              'dossier 01 (22 §3.1): come l\'AK-630, raffiche piu\' corte (STIMA)', 'media-bassa'),
    # la Knox e le nordiche (ricerca 36 §2-§4): dati dei pezzi da NavWeaps; tiro, portate e schegge per analogia (STIMA)
    'mk42': ('Mk 42 5"/54 (Knox, Mod 9)', 127.0, 1, 28.0, 40, 0.0, 3.5, 808.0, 7.0, -15.0, 30.0, 10000.0, 5000.0,
             20.0, 120.0, 2.0, 20.0, 0.005, 4.0, 'pd', 0, 0.0,
             'NW-Mk42 (36 §2): 40 colpi nei due tamburi, riforniti a mano durante il tiro; direzione del tiro Mk 68 '
             'piu\' vecchia: sigma 2 mrad (STIMA)', 'alta (dati), STIMA (tiro)'),
    'mk33': ('3"/50 Mk 33 (Oslo, binato)', 76.0, 2, 45.0, 250, 300.0, 0.34, 823.0, 5.0, -15.0, 20.0, 8000.0, 4000.0,
             20.0, 120.0, 2.0, 20.0, 0.005, 2.0, 'pd', 0, 0.0,
             'NW-Mk33 (36 §2): 45-50 colpi/min per canna, 200-300 pronti per canna, poi il rifornimento lento dal '
             'deposito (STIMA 5 min); dati USA, la dotazione norvegese non si trova', 'alta (dati USA), STIMA (tiro)'),
    'bofors57': ('Bofors 57/70 Mk 2 (Stockholm)', 57.0, 1, 220.0, 120, 120.0, 0.2, 1025.0, 6.0, -10.0, 35.0, 8000.0,
                 4000.0, 15.0, 90.0, 1.5, 20.0, 0.005, 1.5, 'pd', 0, 0.0,
                 'NW-57 (36 §2): 120 pronti, ricaricati in 2 min dai montacarichi; direzione del tiro 9LV: sigma 1,5 mrad '
                 '(STIMA)', 'alta (dati), STIMA (tiro)'),
    'bofors40': ('Bofors 40/70 SAK 350 (Oslo, Hauk, Stockholm)', 40.0, 1, 240.0, 96, 60.0, 0.12, 1005.0, 5.0, -10.0,
                 30.0, 4000.0, 2000.0, 15.0, 60.0, 2.0, 15.0, 0.005, 1.0, 'pd', 0, 0.0,
                 'NW-40 (36 §2): 96 colpi sull\'affusto, piastrine da 4 caricate a mano (STIMA 1 min per rifarli)',
                 'alta (dati), STIMA (tiro)'),
}

# il deposito del pezzo: i colpi a bordo per l'affusto, caricatore compreso (docs/ricerca/36 §2-§4); 0 senza limite
# (n.v.: Iowa e AK-130). Finito il deposito il pezzo tace
DEPOSITI = {
    'mk75': 600,        # n.v. per la Perry: STIMA come i 127 mm USA (36 §3)
    'mk45': 500,        # 475-500 su Spruance e Kidd, 600 sulle Ticonderoga (NW-Mk45)
    'mk12_5_38': 0,
    'mk7_16': 0,
    'ak100': 350,       # NW-AK100, fregata di 4000 t
    'ak130': 0,
    'ak726': 1000,      # STIMA per affusto (36 §3)
    'ak725': 1000,      # RU-725, per affusto
    'ak630': 2000,      # il nastro dell'affusto (RU-630); non si rifa' in combattimento (STIMA)
    'ak230': 1000,      # 500 per canna (RU-230)
    'mk42': 600,        # NW-Mk42 (Knox)
    'mk33': 2400,       # 1200 per canna (NW-Mk33)
    'bofors57': 1000,   # STIMA
    'bofors40': 800,    # STIMA
}

# classe: pezzi (ripetuti per affusto: i binati sono un affusto con 2 canne). Dossier 01 §6 per la dotazione 1989
CLASSI = {
    'perry': ['mk75'],
    'spruance': ['mk45', 'mk45'],
    'kidd': ['mk45', 'mk45'],
    'iowa': ['mk12_5_38'] * 6 + ['mk7_16'] * 3,
    'krivak2': ['ak100', 'ak100'],
    'kirov': ['ak100', 'ak100'] + ['ak630'] * 8,
    'slava': ['ak130'] + ['ak630'] * 6,
    'kara': ['ak726', 'ak726'] + ['ak630'] * 4,
    'koni': ['ak726', 'ak726', 'ak230', 'ak230'],
    'nanuchka': ['ak725'],
    'grisha3': ['ak725', 'ak630'],   # il binato AK-725 a poppa e l'AK-630 (ricerca 36 §3; la principale, 31/10/2026)
    'knox': ['mk42'],
    # Oslo: nel 1989 tre su cinque hanno ancora i due binati da 76 (Bergen, Oslo, Stavanger fino al 6/1989); Trondheim e
    # Narvik un binato e il Bofors 40 (NP-Oslo, 36 §3): la classe tiene la dotazione dei piu'
    'oslo': ['mk33', 'mk33'],
    'hauk': ['bofors40'],
    'stockholm': ['bofors57', 'bofors40'],
}

# la parte del sottomarino fuori dall'acqua in superficie (22 §3.6, §5.4; STIMA): bordo libero, falsatorre (lunghezza,
# larghezza, altezza sull'acqua, centro a prora del centro dello scafo)
# sottomarini in superficie: bordo libero, falsatorre (lunghezza, larghezza, altezza sull'acqua, centro a proravia del
# mezzo) misurati sui modelli 3D del kit (data/motore/modelli, parti «scafo» e «vela»; x riportate alla lunghezza della
# classe; revisione del kit, G3). Il modello e' nell'assetto in superficie delle foto.
SAGOME = {
    'los_angeles': (1.9, 8.3, 1.5, 7.6, 23.1, 'modello 3D del kit (falsatorre a prora, 6-8 m sull\'acqua: 22 §3.6)'),
    'sturgeon': (2.0, 10.9, 2.7, 9.0, 20.6, 'modello 3D del kit'),
    'trafalgar': (2.4, 9.3, 2.7, 7.0, 10.8, 'modello 3D del kit'),
    'alfa': (4.0, 22.9, 5.2, 7.0, 10.7, 'modello 3D del kit: vela lunga e carenata'),
    'victor3': (3.7, 18.5, 4.0, 8.8, 9.8, 'modello 3D del kit'),
    'akula': (3.5, 30.2, 5.2, 7.4, 8.1, 'modello 3D del kit: vela lunga raccordata allo scafo'),
    'oscar': (4.5, 29.6, 6.0, 8.8, 32.2, 'modello 3D del kit'),
    'charlie2': (4.1, 19.1, 4.0, 7.0, 4.3, 'modello 3D del kit'),
    'typhoon': (5.1, 28.3, 5.4, 14.3, -9.2, 'modello 3D del kit: vela a poppavia del mezzo, sopra i due scafi'),
    'kilo': (3.7, 16.1, 2.7, 9.4, 8.8, 'modello 3D del kit'),
    'foxtrot': (3.3, 13.6, 1.8, 9.3, 3.1, 'modello 3D del kit'),
    # i lotti 1 e 2 e la Nacken (31/10/2026): prima avevano la sagoma predefinita (vela a 6 m sull'acqua)
    'victor1': (3.7, 18.7, 3.8, 8.6, 13.4, 'modello 3D del kit'),
    'delta3': (2.4, 20.9, 3.4, 10.3, 40.0, 'modello 3D del kit: dietro la vela la gobba dei silos'),
    'sierra': (3.5, 21.8, 4.8, 7.5, 9.1, 'modello 3D del kit: vela bassa e lunga'),
    'ohio': (3.9, 12.8, 1.7, 10.0, 49.8, 'modello 3D del kit: vela a prora'),
    'type206': (1.5, 16.3, 1.9, 6.1, 3.3, 'modello 3D del kit: vela a due piani'),
    'nacken': (1.8, 8.2, 4.6, 7.0, 4.2, 'modello 3D del kit (larghezza coi timoni sulla vela)'),
}
SAGOMA_PREDEFINITA = (2.5, 10.0, 2.8, 6.0, 0.0, 'senza dati della classe (STIMA)')


def valore(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if isinstance(v, str): return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list): return '[' + ', '.join(valore(x) for x in v) + ']'
    return repr(v)


def esporta_artiglieria(radice):
    righe = ['# Artiglieria navale contro il sottomarino (generato da tools/pipeline/artiglieria.py: non modificare a mano).',
             '# Numeri e fonti: docs/ricerca/22_artiglieria_contro_sottomarini.md; piano: docs/16_artiglieria.md.', '']
    for p, valori in PEZZI.items():
        assert len(valori) == len(CAMPI), p
        righe += [f'[pezzi.{p}]', *(f'{k} = {valore(v)}' for k, v in zip(CAMPI, valori)), f'deposito = {DEPOSITI[p]}',
                  f'volo = {valore(VOLO[p])}', '']
    for c, pezzi in CLASSI.items():
        assert all(p in PEZZI for p in pezzi), c
        righe += [f'[classi.{c}]', f'pezzi = {valore(pezzi)}', '']
    for c, (bordo, fl, fw, fh, avanti, nota) in {**SAGOME, '_predefinita': SAGOMA_PREDEFINITA}.items():
        righe += [f'[sagome.{c}]', f'bordo_libero_m = {bordo}', f'falsatorre_lunghezza_m = {fl}',
                  f'falsatorre_larghezza_m = {fw}', f'falsatorre_altezza_m = {fh}', f'falsatorre_avanti_m = {avanti}',
                  f'nota = {valore(nota)}', '']
    uscita = os.path.join(radice, 'data', 'motore', 'artiglieria.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(PEZZI), len(CLASSI), len(SAGOME)


if __name__ == '__main__':
    print('pezzi, classi, sagome:', esporta_artiglieria(sys.argv[1] if len(sys.argv) > 1 else '.'))
