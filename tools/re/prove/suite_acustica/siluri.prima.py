"""Tabella dei siluri e delle esche dell'Enhanced (fase C6b) -> data/motore/siluri.toml.
La legge nucleo/siluro.hpp (carica_siluri); il significato dei campi e' in DatiSiluro e DatiEsca. Prestazioni dal
dossier 01 §6.1-6.2 (stime della letteratura aperta, non i valori «floor» ufficiali); testa, filo, spoletta e virata
sono STIME DI GIOCO (le fonti aperte non danno numeri). Dove le fonti discordano la nota dice il valore scelto.
Uso: siluri.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

CAMPI = ['nome', 'velocita_alta_kn', 'velocita_bassa_kn', 'portata_alta_m', 'portata_bassa_m', 'quota_max_m',
         'virata_gradi_s', 'testata_kg', 'spoletta_m', 'filoguidato', 'filo_siluro_m', 'filo_battello_m', 'attivo',
         'passivo', 'segui_scia', 'acquisizione_attiva_m', 'acquisizione_passiva_m', 'semiapertura_gradi',
         'in_servizio_dal', 'fonte']

# classe: (valori nell'ordine di CAMPI, nota sulle scelte)
SILURI = {
    'mk48': (('Mk 48 Mod 4', 55.0, 40.0, 38000.0, 50000.0, 760.0, 8.0, 293.0, 8.0, True, 30000.0, 3000.0,
              True, True, False, 1500.0, 4000.0, 45.0, 1980, 'dossier 01 §6.1 (media); testa e filo STIMA'),
             'ufficiale ">28 kn, >5 mi", ">1.200 ft": stime della letteratura (Almanac of Seapower 1987, Jane\'s); '
             'quota stimata 760-800 m, si prende 760'),
    'mk48_adcap': (('Mk 48 ADCAP', 60.0, 40.0, 32000.0, 50000.0, 800.0, 8.0, 295.0, 8.0, True, 30000.0, 3000.0,
                    True, True, False, 2000.0, 5000.0, 50.0, 1988, 'dossier 01 §6.1 (media); marginale nel 1989'),
                   'IOC 1988; 55-65 kn nelle fonti: 60; 27-38 km in alta: 32'),
    'tigerfish': (('Tigerfish Mk 24 Mod 1', 35.0, 24.0, 17000.0, 33000.0, 442.0, 6.0, 134.0, 6.0, True, 25000.0,
                   3000.0, True, True, False, 1000.0, 2500.0, 40.0, 1987, 'dossier 01 §6.1 (media)'),
                  '13-21 km e 27-39 km nelle fonti: i valori medi'),
    'spearfish': (('Spearfish', 60.0, 30.0, 21000.0, 56000.0, 600.0, 9.0, 300.0, 8.0, True, 30000.0, 3000.0,
                   True, True, False, 2000.0, 5000.0, 50.0, 1994, 'dossier 01 §6.1 (media); non in servizio nel 1989'),
                  'in servizio dal 1992-94: solo scenari what-if; bassa velocita\' e quota STIMA'),
    'uset80': (('USET-80', 45.0, 35.0, 18000.0, 24000.0, 1000.0, 7.0, 250.0, 7.0, False, 0.0, 0.0,
                True, True, False, 1200.0, 2500.0, 40.0, 1980, 'dossier 01 §6.1 (media); bassa velocita\' STIMA'),
               '43-50 kn e 18-20 km; 1.000 m (fonte russa) contro ">400 m" (navweaps): si prende 1.000. '
               'E\' il "Type 53" antisom dell\'originale'),
    'test71m': (('TEST-71M', 40.0, 24.0, 15000.0, 20000.0, 400.0, 6.0, 205.0, 6.0, True, 20000.0, 2000.0,
                 True, True, False, 800.0, 1500.0, 40.0, 1977, 'dossier 01 §6.1 (media)'),
                '"35 kn d\'attacco, 24 kn x 15 km" (RU wiki) contro 40 kn x 15 km: si prende 40; acquisizione <= 800 m'),
    'set65': (('SET-65', 40.0, 40.0, 16000.0, 16000.0, 400.0, 6.0, 205.0, 6.0, False, 0.0, 0.0,
               True, True, False, 800.0, 1500.0, 40.0, 1965, 'dossier 14 §2.9 (media); quota e testa STIMA'),
              'antisom dell\'Alfa nel 1989 (dossier 14 §2.9): una sola velocita\' nelle fonti'),
    'saet60m': (('SAET-60M', 40.0, 40.0, 15000.0, 15000.0, 200.0, 6.0, 300.0, 6.0, False, 0.0, 0.0,
                 False, True, False, 0.0, 1500.0, 40.0, 1969, 'dossier 14 §2.9 (bassa); quota e testa STIMA'),
                'testa solo passiva; testata 300 kg (100 in una fonte): si prende 300'),
    'type53_65k': (('53-65K', 45.0, 45.0, 19000.0, 19000.0, 100.0, 5.0, 300.0, 6.0, False, 0.0, 0.0,
                    False, False, True, 0.0, 0.0, 30.0, 1969, 'dossier 01 §6.1 (alta); segui-scia'),
                   'segui-scia antinave: immune alle esche, inutile contro sottomarini immersi'),
    'mk46': (('Mk 46 Mod 5', 45.0, 45.0, 11000.0, 11000.0, 450.0, 10.0, 44.0, 5.0, False, 0.0, 0.0,
              True, True, False, 900.0, 1500.0, 45.0, 1979, 'dossier 01 §6.2 (alta)'),
             'leggero; serpentina o cerchio'),
    'at1': (('AT-1M', 27.0, 27.0, 5000.0, 5000.0, 200.0, 8.0, 70.0, 5.0, False, 0.0, 0.0,
             True, False, False, 300.0, 0.0, 40.0, 1965, 'dossier 01 §5.1 (media); anno di servizio STIMA'),
            'Ka-25: 27 kn x 5 km, acquisizione 500 m nelle fonti (300 m dalla taratura A6), quota 20-200 m'),
    # il Tp 42 svedese (docs/ricerca/35): 400 mm, del 1975 (Jane's); il Tp 422 per gli elicotteri antisom, che lo sganciano
    # senza paracadute (Military Periscope). Per le acque basse del Baltico: quota massima e fondale minimo bassi. Velocita',
    # portata, testata e acquisizione non pubblicate: STIMA. Filoguidato dai battelli; dall'elicottero, come qui, no
    'tp42': (('Tp 42 (Tp 422)', 30.0, 20.0, 10000.0, 20000.0, 200.0, 8.0, 45.0, 4.0, False, 0.0, 0.0,
              True, True, False, 500.0, 800.0, 40.0, 1975, 'docs/ricerca/35 (bassa): 400 mm, 1975; il resto STIMA'),
             'leggero da 400 mm per acque basse; dall\'Hkp 4 senza paracadute'),
    # siluri aviolanciati dell'ASW aereo (docs/13_asw_aereo.md, A3; dossier 17 §6.7)
    'at2': (('AT-2M', 40.0, 40.0, 10000.0, 10000.0, 600.0, 7.0, 100.0, 5.0, False, 0.0, 0.0,
             True, True, False, 800.0, 800.0, 40.0, 1973, 'dossier 17 §6.7 (alta per peso e autoguida); '
             'velocita, portata, testata e anno STIMA'),
            'Il-38 e Tu-142: 534 mm, 1030 kg, autoguida a 600-1000 m (si prende 800); paracadute multiplo'),
    'umgt1': (('UMGT-1 Orlan', 41.0, 41.0, 8000.0, 8000.0, 550.0, 7.0, 60.0, 5.0, False, 0.0, 0.0,
               True, True, False, 600.0, 600.0, 40.0, 1981, 'dossier 17 §6.7 (alta/media); testata e '
               'acquisizione STIMA'),
              '400 mm, 41 kn x 8 km, fino a 550 m; entra in acqua a 60-65 m/s, scende a 40-50 m e gira a sinistra a 7°/s'),
    'apr2': (('APR-2 Yastreb-M', 60.0, 43.0, 3000.0, 3000.0, 600.0, 10.0, 100.0, 6.0, False, 0.0, 0.0,
              True, True, False, 1000.0, 1500.0, 50.0, 1976, 'dossier 17 §6.7 (media); portata, testata e '
              'acquisizione STIMA'),
             'razzo-siluro: plana in spirale a -17° fino a 150 m in passivo, poi motore e attivo; 43-60 kn; '
             'bersagli a 15-600 m; Pk 0,7-0,85 con 300-500 m d\'errore di designazione'),
}

# affidabilita' (testa e spoletta che funzionano) dove non e' 1: classe -> (valore, nota). Taratura A6 (25/10/2026)
AFFIDABILITA = {
    'mk46': (0.8, 'STIMA (dossier 17 §6.7, taratura A6.1 del 25/10/2026): proprieta\' dell\'arma, teste e spolette che '
                  'funzionano; lascia fisica la precisione dello sgancio. Pk 0,5-0,7 contro l\'SSN non allertato'),
    'at1': (0.3, 'taratura A6: con l\'acquisizione a 300 m e il 30% di teste e spolette funzionanti la Pk sul Foxtrot '
                 'torna a 0,15-0,18 delle prove di stato (dossier 17 §9, [ART-Be12]); prima colpiva il 96% dei lanci. '
                 'Misurata sul banco del Barents (A6.5, 25/10/2026): nel Baltico l\'AT-1 non si sgancia'),
    'tp42': (0.8, 'STIMA (docs/ricerca/35): come il Mk 46, nessun dato aperto'),
}

# fondale minimo per l'aviolancio: classe -> (m, nota). In acqua piu' bassa il siluro, nel tuffo d'ingresso, urta il
# fondo e si perde (taratura A6.1, 25/10/2026)
FONDALE_MINIMO = {
    'at1': (60.0, 'dossier 17 §6.7 [ART-Be12]: fondale >= 60 m'),
    'apr2': (80.0, 'dossier 17 §6.7 [RW-APR2]: fondale >= 80 m'),
    'mk46': (40.0, 'STIMA: il Mod 5 e\' la versione per acque basse; sotto 40 m il tuffo d\'ingresso tocca il fondo'),
    'tp42': (15.0, 'STIMA (docs/ricerca/35): fatto per il Baltico, sganciato basso e senza paracadute'),
}

# spoletta misurata dallo scafo (Enhanced, cilindro della classe): classe -> (m, nota). Senza: spoletta_m. spoletta_m
# resta per i bersagli senza dimensioni note, misurata dal centro con un margine per lo scafo. Come i colpi del modello
# dei danni (danni_sottomarini.cpp): i leggeri e i 533 sovietici a contatto, il Mk 48 a influenza sotto lo scafo
CONTATTO = 'STIMA: spoletta a contatto (1 m dallo scafo), come nel modello dei danni'
SPOLETTA_SCAFO = {
    'mk46': (1.0, CONTATTO), 'at1': (1.0, CONTATTO), 'at2': (1.0, CONTATTO), 'umgt1': (1.0, CONTATTO),
    'apr2': (1.0, CONTATTO), 'saet60m': (1.0, CONTATTO), 'set65': (1.0, CONTATTO), 'test71m': (1.0, CONTATTO),
    'uset80': (1.0, CONTATTO), 'tigerfish': (1.0, CONTATTO), 'tp42': (1.0, CONTATTO),
    'mk48': (5.0, 'STIMA: spoletta a influenza sotto lo scafo, 3-8 m nel modello dei danni'),
    'mk48_adcap': (5.0, 'STIMA: spoletta a influenza sotto lo scafo, 3-8 m nel modello dei danni'),
    'spearfish': (5.0, 'STIMA: come il Mk 48'),
    'type53_65k': (3.0, 'STIMA: segui-scia, scoppia sotto la carena'),
}

# esche: classe -> (livello dB, durata s, quota m, nota)
ESCHE = {
    'adc_mk2': (150.0, 600.0, 60.0, 'dossier 05 §5.1 e §5.3; livello STIMA (piu\' rumorosa del battello); '
                                    'batteria termica 6-45 min nelle fonti: STIMA 10 min'),
    'mg74': (140.0, 1200.0, 0.0, 'MG-74 Korund (URSS, 1974), dossier 05 §5.2 e §7: semovente da tubo da 533, 3,9 m, '
                                 'fino a 250 m; imita rumore ed eco del sottomarino. Durata STIMA 20 min (15-30)'),
    'moss_mk70': (140.0, 1200.0, 0.0, 'MOSS Mk 70 (USA, 1976), dossier 05 §5.1: veicolo da 10" senza testata, eco attiva '
                                      'e firma passiva simili al battello; sugli SSBN, non sui 688 del 1989 (what-if). '
                                      'Durata STIMA 20 min'),
}
# esche semoventi (le altre stazionano): velocita' kn, quota massima m, eco dB (TS del ripetitore), imita il rumore del
# lanciatore, manovra programmata (una virata di `virata_gradi` alternata ogni `virata_ogni_s`), anno d'entrata in
# servizio. Quota 0 nelle ESCHE: quella del lanciatore al lancio
MOBILI = {
    'mg74': (10.0, 250.0, 15.0, True, 240.0, 40.0, 1974, 'velocita\' 8-12 kn STIMA (dossier 05 §7); quota massima [KREM]; '
                                                       'eco e manovra STIMA («come un sottomarino»)'),
    'moss_mk70': (10.0, 300.0, 15.0, True, 240.0, 40.0, 1976, 'velocita\', quota massima, eco e manovra STIMA'),
}


def valore(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if isinstance(v, str): return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    return repr(v)


def esporta_siluri(radice):
    righe = ['# Siluri ed esche dell\'Enhanced (generato da tools/pipeline/siluri.py: non modificare a mano).', '']
    for c, (valori, nota) in SILURI.items():
        assert len(valori) == len(CAMPI), c
        extra = [f'affidabilita = {valore(AFFIDABILITA[c][0])}', f'nota_affidabilita = {valore(AFFIDABILITA[c][1])}'] if c in AFFIDABILITA else []
        if c in SPOLETTA_SCAFO:
            extra += [f'spoletta_scafo_m = {valore(SPOLETTA_SCAFO[c][0])}', f'nota_spoletta = {valore(SPOLETTA_SCAFO[c][1])}']
        if c in FONDALE_MINIMO:
            extra += [f'fondale_minimo_m = {valore(FONDALE_MINIMO[c][0])}', f'nota_fondale = {valore(FONDALE_MINIMO[c][1])}']
        righe += [f'[siluri.{c}]', *(f'{k} = {valore(v)}' for k, v in zip(CAMPI, valori)), *extra, f'nota = {valore(nota)}', '']
    for c, (livello, durata, quota, nota) in ESCHE.items():
        extra = []
        if c in MOBILI:
            v, qmax, eco, imita, ogni, gradi, anno, nota_m = MOBILI[c]
            extra = [f'velocita_kn = {v}', f'quota_max_m = {qmax}', f'eco_db = {eco}', f'imita_rumore = {valore(imita)}',
                     f'virata_ogni_s = {ogni}', f'virata_gradi = {gradi}', f'in_servizio_dal = {anno}',
                     f'nota_moto = {valore(nota_m)}']
        righe += [f'[esche.{c}]', f'livello_db = {livello}', f'durata_s = {durata}', f'quota_m = {quota}', *extra,
                  f'nota = {valore(nota)}', '']
    uscita = os.path.join(radice, 'data', 'motore', 'siluri.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(SILURI), len(ESCHE)


if __name__ == '__main__':
    print('siluri, esche:', esporta_siluri(sys.argv[1] if len(sys.argv) > 1 else '.'))
