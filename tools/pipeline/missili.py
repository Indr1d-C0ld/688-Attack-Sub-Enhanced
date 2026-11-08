"""Classi di missile dell'Enhanced per la stazione di tiro (docs/21_stazione_missili.md §11) -> data/motore/missili.toml.
La legge nucleo/missili.hpp (carica_missili); il significato dei campi e' in DatiMissile. Numeri dal dossier
docs/ricerca/27_missili_impiego.md (con gli appunti in 27_note/) e dal 05 §1-§3; dove le fonti aperte non danno numeri
(portata del seeker, tempi di preparazione, quote e velocita' di lancio) i valori sono STIME DI GIOCO, segnate nella
nota. In fondo, quale classe vale per ogni arma del carico (data/motore/carico.toml) secondo l'anno, e quale missile antinave
tira ciascuna classe dell'IA (classi.toml).
Uso: missili.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

CAMPI = ['nome', 'ruolo', 'uscita', 'velocita_m_s', 'quota_crociera_m', 'portata_m', 'distanza_min_m',
         'quota_lancio_max_m', 'velocita_lancio_max_kn', 'quota_lancio_min_m', 'salita_s', 'boost_s', 'discesa_s', 'terminale', 'pop_up_m',
         'acquisizione_m', 'semiapertura_gradi', 'mare_max', 'p_acquisizione_scorta', 'esm', 'esm_radar',
         'radar_seeker', 'quota_ricerca_m', 'testata_kg', 'affidabilita', 'accensione_s', 'allineamento_s',
         'preparazione_primo_s', 'preparazione_altri_s', 'anno_dal', 'anno_al', 'fonte', 'confidenza']

# seeker DSQ-28 (Harpoon e TASM): portata d'acquisizione per dimensione del bersaglio (piccola, media, grande), STIMA dal
# calcolo dell'orizzonte radar con un margine di potenza (dossier 27 §3.1)
DSQ28_RADENTE = [12000.0, 18000.0, 23000.0]
DSQ28_TASM = [15000.0, 25000.0, 35000.0]   # dai 100 m della quota di ricerca del TASM

# classe: (valori nell'ordine di CAMPI, nota sulle scelte)
MISSILI = {
    'harpoon_84a': (('UGM-84A Harpoon (Block 1)', 'antinave', 'capsula', 240.0, 15.0, 111000.0, 5556.0, 60.0, 10.0,
                     20.0, 3.0, 15.0, 'pop_up', 1800.0, DSQ28_RADENTE, 45.0, 5, 0.4, False, [], 'dsq28', 0.0, 221.0,
                     0.95, 30.0, 30.0, 0.0, 0.0, 1981, 1997,
                     'dossier 05 §1 e 27 §1-§3; portata GAO C-MASAD-81-11 (circa 60 nmi)', 'media'),
                    'pop-up a 1.800 m (Block 1); il terminale non si sceglie sui sottomarini nel 1989 (dossier 27 §0.1 '
                    'punto 5). Quota e velocita\' di lancio, distanza minima, tempi di salita, accensione e allineamento '
                    'STIMA (dossier 05 §1.2-§1.3, 27 §2); acquisizione del seeker STIMA; affidabilita\' come il Tomahawk '
                    '(GAO 1995), STIMA'),
    'harpoon_84c': (('UGM-84C Harpoon (Block 1B)', 'antinave', 'capsula', 240.0, 5.0, 111000.0, 5556.0, 60.0, 10.0,
                     20.0, 3.0, 15.0, 'radente', 0.0, DSQ28_RADENTE, 45.0, 5, 0.4, False, [], 'dsq28', 0.0, 221.0,
                     0.95, 30.0, 30.0, 0.0, 0.0, 1982, 1997,
                     'dossier 05 §1 e 27 §1-§3; portata GAO C-MASAD-81-11 (circa 60 nmi)', 'media'),
                    'consegne dal giugno 1982, solo radente e piu\' basso del Block 1 (designation-systems); il resto '
                    'come l\'84A'),
    'tasm': (('UGM-109B Tomahawk antinave (TASM)', 'antinave', 'booster_subacqueo', 245.0, 15.0, 460000.0, 9260.0,
              60.0, 10.0, 0.0, 12.0, 20.0, 'radente', 0.0, DSQ28_TASM, 45.0, 5, 0.4, True, ['top_sail', 'head_net_c'],
              'dsq28', 100.0, 454.0, 0.95, 600.0, 600.0, 0.0, 0.0, 1983, 1994,
              'dossier 05 §2.3 e 27 §3.2; portata operativa 250 nmi (GAO C-MASAD-81-11); affidabilita\' GAO 1995',
              'media'),
             'booster Mk 106 per 12 s sott\'acqua; sale a ~100 m per cercare (forum, bassa) con l\'ESM sulle emissioni '
             'del bersaglio voluto (Naval Gazing): i radar dell\'ESM sono una STIMA da concordare col disassemblatore; '
             'accensione e allineamento 8-12 min STIMA (si prende 10, divisi in due); testata 454 kg come nel danno '
             '(manuale D3 §2.3); distanza minima 5 miglia come nell\'originale'),
    'tlam_c': (('UGM-109C Tomahawk d\'attacco a terra (TLAM-C)', 'attacco_terra', 'booster_subacqueo', 245.0, 40.0,
                875000.0, 0.0, 60.0, 10.0, 0.0, 12.0, 20.0, 'radente', 0.0, [0.0, 0.0, 0.0], 0.0, 9, 0.0, False, [],
                '', 0.0, 450.0, 0.95, 0.0, 0.0, 900.0, 240.0, 1986, 9999,
                'dossier 05 §2.4, 27 §4.2 e appunti OTH §4; affidabilita\' GAO 1995 (95% in crociera)', 'media'),
               'portata da sottomarino ~875 km (1.250 km meno il 30% del booster, GAO 1995); quota 30-50 m sul terreno; '
               'preparazione del primo missile 15 min e poi 3-5 min ciascuno, STIMA su Gruner 1987 (10-20 min) e GAO '
               '1995 (circa un\'ora per la piattaforma)'),
    'p700': (('P-700 Granit (SS-N-19)', 'antinave', 'contenitore', 500.0, 25.0, 550000.0, 20000.0, 50.0, 5.0,
              0.0, 5.0, 10.0, 'radente', 0.0, [25000.0, 35000.0, 45000.0], 45.0, 6, 0.2, False, [], 'granit', 0.0,
              750.0, 0.9, 60.0, 120.0, 0.0, 0.0, 1983, 9999,
              'dossier 05 §3.1 e §3.3; lancio fino a 50 m e 5 kn, 5 s fra i missili (Pavlov, appunti sovietici §0)',
              'media'),
             'il profilo alto (14-17 km per la gran parte del volo) qui e\' semplificato in uno basso a Mach 1,5; la '
             'salva in rete che riconosce i bersagli per dimensione e\' resa con una p_acquisizione_scorta bassa (0,2), '
             'STIMA; portata del seeker, tempi e affidabilita\' STIMA'),
    'p500': (('P-500 Bazalt (SS-N-12)', 'antinave', 'contenitore', 450.0, 25.0, 550000.0, 20000.0, 0.0, 30.0,
              0.0, 5.0, 10.0, 'radente', 0.0, [25000.0, 35000.0, 45000.0], 45.0, 6, 0.3, False, [], '', 0.0,
              1000.0, 0.9, 60.0, 120.0, 0.0, 0.0, 1977, 9999,
              'manuale D3 §2.3 (Slava, 1.000 kg); profilo come il Granit', 'bassa'),
             'imbarcato su navi (Slava) e sugli Echo II: lancio di superficie; numeri del seeker, tempi e affidabilita\' '
             'STIMA; manca un emettitore della testa in emettitori.toml'),
    'p120': (('P-120 Malakhit (SS-N-9)', 'antinave', 'contenitore', 300.0, 60.0, 120000.0, 10000.0, 50.0, 5.0,
              0.0, 7.0, 10.0, 'radente', 0.0, [15000.0, 20000.0, 25000.0], 45.0, 5, 0.5, False, [], '', 0.0,
              840.0, 0.85, 60.0, 120.0, 0.0, 0.0, 1972, 9999,
              'dossier 05 §3.1 e §3.4 (lancio umido fino a 50 m, dati dal sonar Rubikon)', 'media'),
             'testata 840 kg (Wikipedia); il danno dell\'IA usa 500 kg per classe (manuale D3 §2.3): da allineare col '
             'runtime; 10-15% di guasto al lancio (prove del 1974), si prende 0,85; seeker e tempi STIMA'),
    'p70': (('P-70 Ametist (SS-N-7)', 'antinave', 'contenitore', 320.0, 60.0, 65000.0, 8000.0, 30.0, 5.0,
             0.0, 7.0, 10.0, 'radente', 0.0, [12000.0, 18000.0, 22000.0], 45.0, 5, 0.5, False, [], '', 0.0,
             530.0, 0.85, 60.0, 120.0, 0.0, 0.0, 1968, 9999,
             'dossier 05 §3.1 (lancio fino a 30 m, boost di 7 s)', 'media'),
            'testata 530 kg HE (Wikipedia; 1.000 kg nella versione speciale): da allineare col danno dell\'IA come il '
            'Malakhit; seeker e tempi STIMA'),
}

# quale classe vale per un'arma del carico (data/motore/carico.toml [armi.*]) nell'anno della missione
ARMI = [
    ('harpoon', 'harpoon_84a', 1981, 1981),
    ('harpoon', 'harpoon_84c', 1982, 9999),
    ('tasm', 'tasm', 1983, 9999),
    ('tlam_c', 'tlam_c', 1986, 9999),
]

# il missile antinave delle classi dell'IA (chiavi di data/motore/classi.toml) nell'anno della missione, per il MISSILE
# degli script col modello (docs/21 §7; dossier 05 §3 e 27 §5). Fuori: Kara e Krivak (SS-N-14, un missile ASW che il
# catalogo non ha), Victor III e Akula (il Granat e' solo strategico, Vodopad e Veter sono antisom), Alfa (Vyuga, solo
# nucleare), Koni (niente missili antinave nella versione sovietica). Le navi americane lanciano l'RGM-84 di superficie:
# qui la stessa classe dell'UGM-84 (stesso missile dopo la capsula), STIMA
LANCIATORI = [
    ('oscar', 'p700', 1980, 9999),        # Pr. 949 (1980) e 949A: 24 Granit
    ('kirov', 'p700', 1980, 9999),        # Pr. 1144: 20 Granit
    ('slava', 'p500', 1982, 9999),        # Pr. 1164: 16 Bazalt
    ('charlie2', 'p120', 1973, 9999),     # Pr. 670M: 8 Malakhit, lancio subacqueo
    ('nanuchka', 'p120', 1969, 9999),     # Pr. 1234: 6 Malakhit
    ('los_angeles', 'harpoon_84a', 1981, 1981),
    ('los_angeles', 'harpoon_84c', 1982, 9999),
    ('sturgeon', 'harpoon_84a', 1981, 1981),
    ('sturgeon', 'harpoon_84c', 1982, 9999),
    ('trafalgar', 'harpoon_84c', 1983, 9999),   # Sub-Harpoon della Royal Navy, STIMA
    ('perry', 'harpoon_84a', 1977, 1981),
    ('perry', 'harpoon_84c', 1982, 9999),
    ('spruance', 'harpoon_84a', 1979, 1981),
    ('spruance', 'harpoon_84c', 1982, 9999),
    ('kidd', 'harpoon_84a', 1981, 1981),
    ('kidd', 'harpoon_84c', 1982, 9999),
    ('iowa', 'harpoon_84c', 1982, 9999),      # rientrate in servizio dal 1982 con Harpoon e Tomahawk
]

# il tiro dell'IA senza script (engine/missioni/src/missione_missili.cpp, passo_missili_ia): la salva su una nave (su un
# sottomarino emerso sempre 1) e la riserva a bordo; 0 per i sottomarini occidentali, che li prendono dal carico
# (carico.toml). STIMA di gioco: il dossier 27 non da' la dottrina del numero per classe; i sovietici tirano a 4 contro
# i gruppi navali, le occidentali a coppie
SALVE = {'oscar': (4, 24), 'kirov': (4, 20), 'slava': (4, 16), 'charlie2': (2, 8), 'nanuchka': (3, 6), 'los_angeles': (2, 0),
         'sturgeon': (2, 0), 'trafalgar': (2, 0), 'perry': (2, 4), 'spruance': (2, 8), 'kidd': (2, 8), 'iowa': (4, 16)}


# le difese delle navi contro i missili del modello (nucleo/difese_navi.hpp; dossier 05 §4; manuale D3 §2.6): affusti Phalanx,
# canali di guida, anno del Block 1 (0 = mai), fonte, confidenza. Canali: uno per lanciatore di missili antiaerei, la regola del
# dossier per l'Osa (§4.2), con almeno 1 anche senza missili: il byte di una nave senza missili sono i cannoni e gli inganni.
# Canali tutti STIMA, tranne il Kinzhal. Una classe che non e' qui resta com'era: solo il byte, senza limiti. L'AK-630 resta
# fuori per decisione dell'utente (04/11/2026): il byte 90 dei sovietici lo riassume
DIFESE_NAVE = [
    ('perry', 1, 1, 0, 'Phalanx: dossier 01 §4.1 e 22; canali: Mk 13', 'media'),
    ('knox', 1, 1, 0, 'Phalanx: enciclopedia (il modello del kit non lo disegna); canali: BPDMS', 'media'),
    ('spruance', 2, 1, 0, 'Phalanx: solo il modello del kit, i dossier non la elencano; canali: Sea Sparrow Mk 29', 'STIMA'),
    ('kidd', 2, 2, 0, 'Phalanx: dossier 01 §4.1 e 22; canali: 2 Mk 26', 'media'),
    ('ticonderoga', 2, 2, 0, 'Phalanx: enciclopedia e kit; canali: 2 Mk 26 o lancio verticale', 'media'),
    ('nimitz', 3, 3, 0, 'Phalanx: dossier 01 (3-4) e kit (3); canali: 3 Mk 29', 'media'),
    ('kitty_hawk', 3, 3, 0, 'Phalanx: enciclopedia senza numero, kit (3); canali come la Nimitz', 'STIMA'),
    ('iowa', 4, 1, 1988, 'Phalanx: dossier 01 §4.1, enciclopedia, kit; Block 1 dal 1988 (dossier 05 §4.1); nessun missile antiaereo', 'media'),
    ('sacramento', 2, 1, 0, 'Phalanx: dossier 01 §4.1 e kit; canali: Sea Sparrow', 'media'),
    ('krivak2', 0, 2, 0, 'canali: 2 Osa-M (dossier 05 §4.2)', 'STIMA'),
    ('kara', 0, 4, 0, 'canali: 2 Shtorm, 2 Osa-M', 'STIMA'),
    ('kirov', 0, 3, 0, 'canali: S-300F, 2 Osa-M', 'STIMA'),
    ('slava', 0, 3, 0, 'canali: S-300F, 2 Osa-M', 'STIMA'),
    ('udaloy', 0, 4, 0, 'canali: Kinzhal, 4 bersagli (dossier 05 §4.2)', 'media'),
    ('koni', 0, 1, 0, 'canali: 1 Osa-M', 'STIMA'),
    ('nanuchka', 0, 1, 0, 'canali: 1 Osa-M', 'STIMA'),
    ('grisha3', 0, 1, 0, 'canali: 1 Osa-M', 'STIMA'),
    ('type22', 0, 2, 0, 'canali: 2 Sea Wolf', 'STIMA'),
    ('type22b2', 0, 2, 0, 'canali: 2 Sea Wolf', 'STIMA'),
    ('oslo', 0, 1, 0, 'canali: Sea Sparrow', 'STIMA'),
    ('hauk', 0, 1, 0, 'nessun missile antiaereo: i cannoni', 'STIMA'),
    ('stockholm', 0, 1, 0, 'nessun missile antiaereo: i cannoni', 'STIMA'),
]


def valore(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if isinstance(v, str): return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list): return '[' + ', '.join(valore(x) for x in v) + ']'
    return repr(v)


QUOTA_MIN = {'harpoon_84a': 20.0, 'harpoon_84c': 20.0}   # sotto i 20 m la capsula affiora vicino alla vela (dossier 05 §1.3, STIMA)


def con_minimo(c, valori):   # il campo quota_lancio_min_m dopo velocita_lancio_max_kn
    i = CAMPI.index('quota_lancio_min_m')
    return valori[:i] + (QUOTA_MIN.get(c, 0.0),) + valori[i:]


# l'esplosivo della testata (kg), per il danno del colpo (75 x esplosivo / 100, l'Harpoon di riferimento) e sulla stessa
# scala dei depositi dei danni critici (il kit, kit-depositi-carburante-dati.md §0.1): (kg, fonte)
ESPLOSIVO = {
    'harpoon_84a': (100.0, 'testata 221 kg (dossier 01 §6.4), esplosivo 0,45 STIMA (il kit, depositi)'),
    'harpoon_84c': (100.0, 'come il Block 1'),
    'tasm': (205.0, 'testata 454 kg (dossier 01 §6.4), esplosivo 0,45 STIMA (il kit)'),
    'tlam_c': (205.0, 'testata 450 kg, come il TASM (il kit)'),
    'p700': (518.0, 'dossier 05 §3.1: 518-750 kg HE'),
    'p500': (600.0, 'testata 1.000 kg (manuale D3 §2.3), esplosivo 0,6 STIMA (il kit)'),
    'p120': (500.0, 'manuale D3 §2.3 (500 kg nel danno, scelta della principale 31/10/2026); 0,6 della testata di 840 kg'),
    'p70': (320.0, 'testata 530 kg (Wikipedia), esplosivo 0,6 STIMA come il Malakhit'),
}


def esporta_missili(radice):
    righe = ['# Classi di missile dell\'Enhanced (generato da tools/pipeline/missili.py: non modificare a mano).', '']
    for c, (valori, nota) in MISSILI.items():
        valori = con_minimo(c, valori)
        assert len(valori) == len(CAMPI), c
        kg, fonte = ESPLOSIVO[c]
        righe += [f'[missili.{c}]', *(f'{k} = {valore(v)}' for k, v in zip(CAMPI, valori)), f'esplosivo_kg = {valore(kg)}',
                  f'fonte_esplosivo = {valore(fonte)}', f'nota = {valore(nota)}', '']
    for arma, missile, dal, al in ARMI:
        assert missile in MISSILI, missile
        righe += ['[[arma]]', f'arma = {valore(arma)}', f'missile = {valore(missile)}', f'dal = {dal}', f'al = {al}', '']
    for classe, missile, dal, al in LANCIATORI:
        assert missile in MISSILI, missile
        salva, riserva = SALVE.get(classe, (0, 0))
        righe += ['[[lanciatore]]', f'classe = {valore(classe)}', f'missile = {valore(missile)}', f'dal = {dal}', f'al = {al}',
                  f'salva = {salva}', f'riserva = {riserva}', '']
    for classe, phalanx, canali, block1, fonte, confidenza in DIFESE_NAVE:
        righe += ['[[difesa_nave]]', f'classe = {valore(classe)}', f'phalanx = {phalanx}', f'canali = {canali}', f'block1_dal = {block1}',
                  f'fonte = {valore(fonte)}', f'confidenza = {valore(confidenza)}', '']
    uscita = os.path.join(radice, 'data', 'motore', 'missili.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(MISSILI), len(ARMI), len(LANCIATORI)


if __name__ == '__main__':
    print('missili, armi, lanciatori:', esporta_missili(sys.argv[1] if len(sys.argv) > 1 else '.'))
