"""Emettitori radar, ricevitori ESM ed EMCON dell'Enhanced (punto 23 della roadmap) -> data/motore/emettitori.toml.
La legge nucleo/elettronica.hpp (carica_elettronica). Valori dal dossier docs/ricerca/11_emettitori_radar_esm.md §6
(tabella 6.1, macchina EMCON 6.2, ricevitore 6.3, scoperta per giro 6.4): STIMA salvo dove il dossier cita una fonte.
Portate in miglia nautiche nella tabella, in metri nel file. Uso: emettitori.py <radice del progetto>"""
import os, sys

NM = 1852.0

# banda NATO -> intervallo in GHz (dossier 11 §0.1)
BANDE = {'B': (0.25, 0.5), 'C': (0.5, 1.0), 'D': (1.0, 2.0), 'E': (2.0, 3.0), 'F': (3.0, 4.0), 'G': (4.0, 6.0),
         'H': (6.0, 8.0), 'I': (8.0, 10.0), 'J': (10.0, 20.0), 'K': (20.0, 40.0)}

# id: (nome, ruolo, banda principale, periodo s, altezza m (0: la quota della piattaforma), R50 periscopio mare 2 nm,
#      R50 periscopio mare 4 nm, R50 snorkel mare 2 nm, contro una nave grande nm (0: l'orizzonte), fattore ESM
#      sull'orizzonte, puntato, firma unica, gruppo d'ambiguita')
RADAR = {
    'aps115': ('AN/APS-115', 'asw_aereo', 'I', 6.0, 0.0, 6.0, 2.0, 15.0, 100.0, 1.1, False, False, 'asw_x'),
    'aps124': ('AN/APS-124', 'asw_aereo', 'I', 0.75, 0.0, 5.0, 2.0, 12.0, 60.0, 1.1, False, True, 'asw_x'),
    # il Seaspray del Lynx HAS.3 (dossier 38 §2): radar nel muso per i bersagli di superficie piccoli, in banda I/J; il
    # periodo come l'APS-124, le portate STIMA (un poco sotto l'APS-124: antenna piu' piccola)
    'seaspray': ('Seaspray Mk 1', 'asw_aereo', 'I', 1.0, 0.0, 4.0, 1.5, 10.0, 50.0, 1.1, False, True, 'asw_x'),
    'wet_eye': ('Wet Eye', 'asw_aereo', 'I', 6.0, 0.0, 4.0, 1.5, 12.0, 80.0, 1.1, False, False, 'asw_x'),
    'initsiativa2k': ('Initsiativa-2K «Short Horn»', 'asw_aereo', 'I', 4.0, 0.0, 4.0, 1.5, 10.0, 60.0, 1.1, False, False, 'asw_x'),
    # ASW aereo dell'Enhanced (dossier 17 §8.2 e §10.1): S-3A/B col modo periscopio a 300 giri/min (firma unica),
    # Korshun del Tu-142M con l'agilita' di frequenza, Initsiativa-2B del Be-12 e 2KM del Ka-27. R50 STIMA
    'aps116': ('AN/APS-116', 'asw_aereo', 'I', 0.2, 0.0, 8.0, 3.0, 18.0, 150.0, 1.1, False, True, 'asw_x'),
    'aps137': ('AN/APS-137 ISAR', 'asw_aereo', 'I', 0.2, 0.0, 10.0, 4.0, 20.0, 150.0, 1.1, False, True, 'asw_x'),
    'korshun': ('Korshun', 'asw_aereo', 'I', 6.0, 0.0, 5.0, 2.0, 14.0, 100.0, 1.1, False, False, 'asw_x'),
    'initsiativa2b': ('Initsiativa-2B', 'asw_aereo', 'I', 4.0, 0.0, 3.5, 1.2, 9.0, 50.0, 1.1, False, False, 'asw_x'),
    'initsiativa2km': ('Initsiativa-2KM', 'asw_aereo', 'I', 4.0, 0.0, 4.0, 1.5, 10.0, 60.0, 1.1, False, False, 'asw_x'),
    'sps55': ('AN/SPS-55', 'superficie', 'I', 3.75, 25.0, 5.0, 2.0, 10.0, 0.0, 1.2, False, False, 'nav_i'),
    'sps10_67': ('AN/SPS-10/67', 'superficie', 'G', 4.0, 35.0, 4.0, 1.5, 9.0, 0.0, 1.2, False, True, 'sup_g'),
    'mk92_cas': ('Mk 92 CAS', 'tiro', 'I', 1.0, 25.0, 4.0, 1.5, 9.0, 25.0, 1.2, False, True, 'tiro_i'),
    'spq9': ('AN/SPQ-9', 'tiro', 'I', 1.0, 25.0, 4.0, 1.5, 9.0, 20.0, 1.2, False, True, 'tiro_i'),
    'sps49': ('AN/SPS-49', 'ricerca_aerea', 'C', 5.0, 35.0, 1.0, 0.0, 4.0, 0.0, 1.3, False, True, 'aerea_c'),
    'sps48': ('AN/SPS-48', 'ricerca_aerea', 'E', 6.0, 40.0, 1.5, 0.5, 5.0, 0.0, 1.3, False, False, 'aerea_ef'),
    'sps40': ('AN/SPS-40', 'ricerca_aerea', 'B', 6.0, 30.0, 0.0, 0.0, 2.0, 0.0, 1.3, False, True, 'aerea_b'),
    'top_sail': ('Top Sail / Top Pair / Top Steer', 'ricerca_aerea', 'C', 8.0, 45.0, 1.0, 0.0, 4.0, 0.0, 1.3, False, True, 'aerea_c'),
    'head_net_c': ('Head Net-C / Strut Curve', 'ricerca_aerea', 'E', 6.0, 28.0, 1.5, 0.5, 5.0, 0.0, 1.3, False, False, 'aerea_ef'),
    'nav_sov': ('Don-2 / Don Kay / Palm Frond', 'navigazione', 'I', 3.0, 25.0, 4.0, 1.5, 9.0, 0.0, 1.2, False, False, 'nav_i'),
    'peel_pair': ('Peel Pair / Plank Shave', 'superficie', 'I', 3.0, 20.0, 4.0, 1.5, 9.0, 0.0, 1.2, False, False, 'nav_i'),
    'nav_civ': ('radar di navigazione civile (X + S)', 'navigazione', 'I', 2.5, 30.0, 2.0, 0.5, 6.0, 0.0, 1.2, False, False, 'nav_i'),
    'fc': ('radar di tiro', 'tiro', 'J', 1.0, 20.0, 0.0, 0.0, 0.0, 30.0, 1.0, True, False, 'tiro_j'),
    'bps15': ('AN/BPS-15', 'navigazione', 'I', 3.0, 8.0, 0.0, 0.0, 0.0, 11.0, 1.2, False, False, 'nav_i'),
    'snoop': ('Snoop Tray / Snoop Pair', 'navigazione', 'I', 3.0, 8.0, 0.0, 0.0, 0.0, 11.0, 1.2, False, False, 'nav_i'),
    # le classi nuove dei lotti 1 e 2 (30/10/2026): le dotazioni da Wikipedia EN, Navsource, Seaforces; portate e
    # periodi STIMA come i radar dello stesso ruolo
    'spy1a': ('AN/SPY-1A', 'ricerca_aerea', 'E', 1.0, 20.0, 2.0, 0.5, 6.0, 0.0, 1.3, False, True, 'aerea_ef'),
    'tipo967': ('Type 967/968', 'ricerca_aerea', 'D', 2.0, 22.0, 1.5, 0.5, 5.0, 0.0, 1.3, False, True, 'aerea_d'),
    'tipo1006': ('Type 1006', 'navigazione', 'I', 2.5, 20.0, 4.0, 1.5, 9.0, 0.0, 1.2, False, False, 'nav_i'),
    'top_plate': ('Top Plate (Fregat-M)', 'ricerca_aerea', 'D', 5.0, 35.0, 1.0, 0.0, 4.0, 0.0, 1.3, False, True, 'aerea_d'),
    'sea_giraffe': ('Sea Giraffe 50HC', 'ricerca_aerea', 'G', 1.5, 22.0, 2.0, 0.5, 6.0, 0.0, 1.3, False, True, 'aerea_g'),
    'sps43': ('AN/SPS-43A', 'ricerca_aerea', 'B', 6.0, 30.0, 0.0, 0.0, 2.0, 0.0, 1.3, False, True, 'aerea_b'),
    'terma': ('Terma (radar di navigazione)', 'navigazione', 'I', 3.0, 7.0, 0.0, 0.0, 0.0, 10.0, 1.2, False, False, 'nav_i'),
    'drbv22': ('DRBV 22', 'ricerca_aerea', 'D', 4.0, 30.0, 1.0, 0.0, 4.0, 0.0, 1.3, False, True, 'aerea_d'),
    'calypso': ('Calypso II (DRUA 33)', 'navigazione', 'I', 3.0, 7.0, 0.0, 0.0, 0.0, 10.0, 1.2, False, False, 'nav_i'),
    'dsq28': ('AN/DSQ-28 (Harpoon, TASM)', 'testa_missile', 'J', 0.5, 10.0, 0.0, 0.0, 0.0, 15.0, 1.0, True, True, 'testa_j'),
    'granit': ('testa del Granit', 'testa_missile', 'J', 0.5, 10.0, 0.0, 0.0, 0.0, 15.0, 1.0, True, True, 'testa_j'),
}

# salti di frequenza impulso per impulso: l'ESM lo identifica a fatica (dossier 17 §1 e §8.2)
AGILI = {'korshun'}

# classe di classi.toml -> radar di bordo (dossier 11 §1 e tabella 6.1)
SUITE = {
    'perry': ['sps55', 'mk92_cas', 'sps49', 'fc'],
    'kidd': ['sps55', 'spq9', 'sps48', 'fc'],
    'spruance': ['sps55', 'spq9', 'sps40', 'fc'],
    'iowa': ['sps10_67', 'sps49', 'fc'],
    'nimitz': ['sps10_67', 'sps49', 'sps48'],
    'sacramento': ['sps10_67', 'sps40'],
    'kara': ['nav_sov', 'top_sail', 'head_net_c', 'fc'],
    'kirov': ['nav_sov', 'top_sail', 'fc'],
    'slava': ['nav_sov', 'top_sail', 'fc'],
    'krivak2': ['nav_sov', 'head_net_c', 'fc'],
    'koni': ['nav_sov', 'head_net_c', 'fc'],
    'nanuchka': ['peel_pair', 'fc'], 'hauk': ['nav_civ', 'fc'],
    'oslo': ['drbv22', 'nav_civ', 'fc'],
    'stockholm': ['sea_giraffe', 'nav_civ', 'fc'],
    'nacken': ['terma'],
    'kitty_hawk': ['sps10_67', 'sps43', 'sps48'],   # la Kitty Hawk nel 1984: SPS-43A, SPS-48C, SPS-10 (STIMA le portate)   # la Näcken: il radar di navigazione in cima all'albero (STIMA la portata)   # la Stockholm: Sea Giraffe, radar di navigazione, 9LV200 (STIMA le portate)   # la Oslo: DRBV 22 di scoperta aerea, Decca di navigazione, Mk 91 (STIMA le portate)   # la Hauk: radar di navigazione (Decca, banda I come i civili) e direttore di tiro (STIMA)
    'mercantile': ['nav_civ'], 'cargo': ['nav_civ'], 'petroliera': ['nav_civ'],
    # le classi civili del traffico (docs/20 §3): nel 1989 il radar di navigazione acceso anche sui pescherecci
    'peschereccio': ['nav_civ'], 'portarinfuse': ['nav_civ'], 'petroliera_costiera': ['nav_civ'], 'traghetto': ['nav_civ'],
    'rimorchiatore': ['nav_civ'], 'rompighiaccio': ['nav_civ'], 'nave_ricerca': ['nav_civ'], 'bmrt': ['nav_sov'],
    'peschereccio_costiero': ['nav_civ'], 'cianciolo': ['nav_civ'],
    'p3c': ['aps115'], 'il38': ['wet_eye'], 'tu142': ['korshun'], 'sh60b': ['aps124'], 'lynx': ['seaspray'], 'ka25': ['initsiativa2k'],
    's3a': ['aps116'], 's3b': ['aps137'], 'be12': ['initsiativa2b'], 'ka27': ['initsiativa2km'],
    'los_angeles': ['bps15'], 'sturgeon': ['bps15'], 'trafalgar': ['bps15'],
    'victor3': ['snoop'], 'akula': ['snoop'], 'oscar': ['snoop'], 'typhoon': ['snoop'], 'kilo': ['snoop'],
    'alfa': ['snoop'], 'charlie2': ['snoop'], 'foxtrot': ['snoop'],
    'harpoon': ['dsq28'], 'tomahawk': ['dsq28'], 'p700': ['granit'],
    # i lotti 1 e 2 delle firme nuove (30/10/2026): la Knox del 1989 con l'SPS-40 aereo, l'SPS-10 (o l'SPS-67 secondo
    # lo scafo) di superficie e l'SPG-53 del cannone; la Ticonderoga con l'SPY-1A, l'SPS-49, l'SPS-55 e l'SPQ-9A; la
    # Type 22 (le due Batch) col 967/968 e il 1006; l'Udaloy con lo Strut Pair, il Top Plate e i Palm Frond; il
    # Grisha III con lo Strut Curve e il Don-2; il Type 206A col Calypso II; i sottomarini sovietici con lo Snoop
    'knox': ['sps10_67', 'sps40', 'fc'], 'ticonderoga': ['sps55', 'spq9', 'sps49', 'spy1a', 'fc'],
    'type22': ['tipo1006', 'tipo967', 'fc'], 'type22b2': ['tipo1006', 'tipo967', 'fc'],
    'udaloy': ['nav_sov', 'head_net_c', 'top_plate', 'fc'],
    'grisha3': ['nav_sov', 'head_net_c', 'fc'], 'ugra': ['nav_sov', 'head_net_c', 'fc'],   # l'Ugra: Strut Curve, Don-2, Muff Cob
    'ohio': ['bps15'], 'type206': ['calypso'], 'delta3': ['snoop'], 'sierra': ['snoop'], 'victor1': ['snoop'],
}

# ricevitori ESM dei sottomarini (dossier 11 §4, §6.3): GHz coperti, probabilita' d'intercetto per passaggio in E-J
# e in B-D, errore del rilevamento (gradi, 1 sigma) in I/J, E-H, B-D, altezza dell'antenna
ESM = {
    'wlr8': ('AN/WLR-8(V)2', 0.05, 20.0, 0.95, 0.8, 5.0, 8.0, 12.0, 1.5),
    'bukhta': ('Bukhta', 2.0, 18.0, 0.7, 0.7, 7.5, 12.0, 18.0, 1.5),
    # Type 206A: Thomson-CSF DR-2000U col SARIE (Seaforces, Los Barcos de Eugenio); prestazioni STIMA fra WLR-8 e Bukhta
    'dr2000': ('DR-2000U', 2.0, 18.0, 0.8, 0.6, 6.0, 10.0, 15.0, 1.5),
}
ESM_DI = {c: 'wlr8' for c in ('los_angeles', 'sturgeon', 'trafalgar')}
ESM_DI.update({c: 'bukhta' for c in ('victor3', 'akula', 'oscar', 'typhoon', 'kilo', 'alfa', 'charlie2', 'foxtrot',
                                      'delta3', 'sierra', 'victor1')})
ESM_DI.update({'ohio': 'wlr8', 'type206': 'dr2000'})

# cicli intermittenti degli aerei in ricerca (dossier 11 §6.2): radar acceso t_on, spento t_off (s, intervalli)
INTERMITTENTE = {
    'p3c': (6.0, 18.0, 120.0, 480.0), 'il38': (120.0, 300.0, 120.0, 300.0), 'tu142': (120.0, 300.0, 120.0, 300.0),
    'sh60b': (30.0, 60.0, 180.0, 300.0), 'lynx': (30.0, 60.0, 180.0, 300.0), 'ka25': (1e9, 1e9, 0.0, 0.0),
    # S-3: «uno o due giri», poi silenzio (dossier 17 §2.5); Be-12 come l'Il-38; Ka-27 come il Ka-25 (STIMA)
    's3a': (1.0, 3.0, 180.0, 600.0), 's3b': (1.0, 3.0, 180.0, 600.0), 'be12': (120.0, 300.0, 120.0, 300.0),
    'ka27': (1e9, 1e9, 0.0, 0.0),
}


# Le antenne sui modelli del kit (31/10/2026): per ogni radar delle navi le parti dei modelli che lo portano, in ordine
# di preferenza (la prima che il modello ha). La quota della cima della parte sul galleggiamento va in [antenne.<classe>]
# e l'altezza del radar (altezza_m) diventa la mediana delle sue navi, arrotondata al metro: l'orizzonte radar viene da
# quello che si vede. Sottomarini, aerei e teste dei missili restano come sono (la loro quota cambia in missione).
PARTI_ANTENNA = {
    'sps55': ['sps55', 'pennone_prodiero', 'pennone', 'albero_prodiero'], 'mk92_cas': ['radome_mk92'],
    'spq9': ['spq9', 'radome_albero_prodiero', 'piattaforma_radome'], 'sps49': ['sps49', 'pennone_alto'], 'sps48': ['sps48'],
    'sps40': ['sps40', 'radar_prodiero'], 'sps10_67': ['sps10', 'radar_poppiero', 'pennone_trinchetto', 'pennone_prodiero'],
    'sps43': ['pennone_alto'], 'spy1a': ['spy1_prua_dritta'],
    'top_sail': ['top_sail', 'top_pair_top_sail', 'top_pair', 'top_steer'], 'top_plate': ['top_plate'],
    'head_net_c': ['head_net', 'strut_pair_maestro', 'strut_curve'],
    'nav_sov': ['radar', 'radar_plancia', 'albero_prodiero', 'albero_principale', 'albero_maestro', 'pennone', 'albero',
                'albero_dritta'],
    'peel_pair': ['radar_nav'], 'drbv22': ['radar_aereo'], 'sea_giraffe': ['sea_giraffe'], 'tipo967': ['type967_968'],
    'tipo1006': ['pennone_prodiero'],
    'nav_civ': ['radar', 'radar_navigazione', 'albero_radar', 'radome_plancia', 'albero_plancia', 'torre_albero', 'albero',
                'albero_poppiero'],
    'fc': ['direttore', 'direttore_prodiero', 'pop_group', 'bass_tilt', 'radome_mk92', 'type910_prodiero', 'mk38_poppiero',
           'mk91_poppiero', 'head_light_prodiero'],
}


PIU_ALTA = {'top_sail'}


def parti_modello(file):
    """{parte: quota della cima sul galleggiamento (m)} di un modello del motore (formato MDL2 di esporta_modello)."""
    import struct, zlib
    import numpy as np
    b = open(file, 'rb').read()
    n_tri = struct.unpack('<I', b[4:8])[0]
    o = 36
    n = struct.unpack('<H', b[o:o + 2])[0]; o += 2
    nomi = []
    for _ in range(n):
        k = b[o]; nomi.append(b[o + 1:o + 1 + k].decode()); o += 1 + k
    lz = struct.unpack('<I', b[o:o + 4])[0]; o += 4
    dt = np.dtype([('v', '<f4', 9), ('uv', '<f4', 6), ('uvo', '<f4', 6), ('alb', '<f4'), ('flag', 'u1'), ('parte', '<u2')])
    a = np.frombuffer(zlib.decompress(b[o:o + lz], -15), np.uint8).reshape(dt.itemsize, n_tri).T.copy().view(dt).reshape(n_tri)
    z = a['v'][:, 2::3].max(axis=1)
    return {nomi[i]: float(z[a['parte'] == i].max()) for i in range(n) if (a['parte'] == i).any()}


def antenne(radice):
    """{classe: {radar: (parte, quota)}} dai modelli in data/motore/modelli (quelli che ci sono)."""
    out = {}
    for c, radar in SUITE.items():
        f = os.path.join(radice, 'data', 'motore', 'modelli', f'{c}.mdl')
        if not os.path.exists(f):
            continue
        parti = parti_modello(f)
        for rd in radar:
            cand = [p for p in PARTI_ANTENNA.get(rd, []) if p in parti]
            # il radar combinato Top Sail / Top Pair / Top Steer: la piu' alta delle sue antenne (lo Slava ha il Top Steer
            # a 38 m sopra il Top Pair); gli altri la prima della lista
            p = max(cand, key=lambda q: parti[q]) if rd in PIU_ALTA and cand else (cand[0] if cand else None)
            if p:
                out.setdefault(c, {})[rd] = (p, round(parti[p], 1))
    return out


def toml(radice='.'):
    ant = antenne(radice)
    quote = {}
    for c, v in ant.items():
        for rd, (_, q) in v.items():
            quote.setdefault(rd, []).append(q)
    mediana = {rd: float(round(sorted(q)[len(q) // 2] if len(q) % 2 else (sorted(q)[len(q) // 2 - 1] + sorted(q)[len(q) // 2]) / 2))
               for rd, q in quote.items()}
    righe = ['# Emettitori radar, ESM ed EMCON (generato da tools/pipeline/emettitori.py: non modificare a mano).',
             '# Dossier docs/ricerca/11_emettitori_radar_esm.md §6: valori STIMA salvo dove il dossier cita una fonte.', '']
    for k, (nome, ruolo, banda, periodo, h, p2, p4, s2, nave, kesm, puntato, unica, gruppo) in RADAR.items():
        g0, g1 = BANDE[banda]
        righe += [f'[radar.{k}]', f'nome = "{nome}"', f'ruolo = "{ruolo}"', f'banda = "{banda}"',
                  f'ghz = {round((g0 + g1) / 2, 3)}', f'periodo_s = {periodo}', f'altezza_m = {mediana.get(k, h)}',
                  f'r50_periscopio_mare2_m = {round(p2 * NM)}', f'r50_periscopio_mare4_m = {round(p4 * NM)}',
                  f'r50_snorkel_mare2_m = {round(s2 * NM)}', f'contro_nave_m = {round(nave * NM)}',
                  f'fattore_esm = {kesm}', f'puntato = {str(puntato).lower()}', f'firma_unica = {str(unica).lower()}',
                  f'gruppo = "{gruppo}"', *(['agilita_frequenza = true'] if k in AGILI else []), '']
    for k, (nome, g0, g1, p_ej, p_bd, s_ij, s_eh, s_bd, h) in ESM.items():
        righe += [f'[esm.{k}]', f'nome = "{nome}"', f'ghz_min = {g0}', f'ghz_max = {g1}', f'p_intercetto = {p_ej}',
                  f'p_intercetto_basse = {p_bd}', f'errore_ij_gradi = {s_ij}', f'errore_eh_gradi = {s_eh}',
                  f'errore_bd_gradi = {s_bd}', f'altezza_m = {h}', '']
    righe.append('[suite]')
    righe += [f'{c} = [{", ".join(chr(34) + r + chr(34) for r in v)}]' for c, v in SUITE.items()]
    righe += ['', '[esm_di]'] + [f'{c} = "{e}"' for c, e in ESM_DI.items()]
    righe += ['', '[intermittente]']
    righe += [f'{c} = [{a}, {b}, {c2}, {d}]' for c, (a, b, c2, d) in INTERMITTENTE.items()]
    # le antenne misurate sui modelli: la parte e la quota per radar (per la prova e per l'altezza per classe)
    for c, v in ant.items():
        righe += ['', f'[antenne.{c}]'] + [f'{rd} = {{ parte = "{p}", quota_m = {q} }}' for rd, (p, q) in v.items()]
    return '\n'.join(righe) + '\n'


if __name__ == '__main__':
    radice = sys.argv[1] if len(sys.argv) > 1 else '.'
    uscita = os.path.join(radice, 'data', 'motore', 'emettitori.toml')
    with open(uscita, 'w') as f:
        f.write(toml(radice))
    print('emettitori:', len(RADAR), 'radar,', len(SUITE), 'classi')
