"""Armi, locali siluri e carichi standard della Sala siluri (Fase 4a, M2) -> data/motore/carico.toml.
Lo legge nucleo/carico.hpp (carica_carico); il significato dei campi e' in SchedaArma, LocaleSiluri e CaricoStandard.
Dati dal dossier docs/ricerca/14_carico_armi.md (§1-2 per armi e locali, §5.1 per i carichi); per i siluri le
prestazioni vengono da siluri.py, cosi' la scheda e il modello dell'Enhanced non si separano.
Uso: carico.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import SILURI, valore   # noqa: E402

# Prestazioni dei siluri dal modello (siluri.py): velocita' e portata alte, testata, anno
def _dal_modello(siluro):
    v = dict(zip(['nome', 'velocita_alta_kn', 'velocita_bassa_kn', 'portata_alta_m', 'portata_bassa_m'],
                 SILURI[siluro][0][:5]))
    campi = SILURI[siluro][0]
    return {'velocita_kn': v['velocita_alta_kn'], 'portata_km': v['portata_alta_m'] / 1000.0,
            'testata_kg': campi[7], 'in_servizio_dal': campi[18]}


# Campi di ogni arma (i mancanti prendono il valore di BASE)
BASE = {'nome': '', 'nazione': 'usa', 'tipo': 'siluro', 'famiglia': '', 'siluro': '', 'guida': '',
        'velocita_kn': 0.0, 'portata_km': 0.0, 'testata_kg': 0.0, 'peso_kg': 0.0, 'lunghezza_m': 0.0,
        'diametro_mm': 533, 'posti': 1.0, 'pozzo_verticale': False, 'in_servizio_dal': 0, 'ritirata_nel': 0,
        'massimo_primo_anno': 0, 'da_famiglia': True, 'solo_ia': False, 'what_if': False, 'moderno': False,
        'nucleare': False,
        'sagoma': 'icona:siluro_533', 'fonte': '', 'confidenza': 'media', 'nota': ''}

# id: campi. famiglia = l'arma della stazione Armi e del gestore di lancio delle missioni (mk48, type53, sea_lance,
# tomahawk, harpoon, esca, mina, attacco_terra, esca_mobile). da_famiglia: puo' prendere dal deposito della famiglia
# (le missioni originali dichiarano solo le famiglie); altrimenti la missione deve dichiarare la variante.
# moderno: un'arma what-if che il modo moderno della partita consente (e il modo 1989 no, anche col what-if del
# giocatore): la MOSS sul 688. Le esche mobili da tubo (famiglia esca_mobile) sono di tipo «siluro» per il carico
# (un posto in rastrelliera e un tubo da siluri, come un siluro): il loro modello e' [esche.<id>] di siluri.toml.
ARMI = {
    # --- USA ---
    'mk48': dict(nome='Mk 48 Mod 4', famiglia='mk48', siluro='mk48', **_dal_modello('mk48'),
                 guida='filoguidato, poi testa attiva o passiva', peso_kg=1558.0, lunghezza_m=5.8,
                 fonte='dossier 14 §2.1', confidenza='alta'),
    'mk48_adcap': dict(nome='Mk 48 ADCAP', famiglia='mk48', siluro='mk48_adcap', **_dal_modello('mk48_adcap'),
                       guida='filoguidato, testa attiva o passiva a scansione elettronica', peso_kg=1676.0,
                       lunghezza_m=5.8, massimo_primo_anno=6, fonte='dossier 14 §2.1 e §5.1',
                       nota='in servizio dal 1988; al piu\' 6 per carico nei primi due anni (PROPOSTA)'),
    'harpoon': dict(nome='UGM-84 Harpoon', tipo='missile', famiglia='harpoon', velocita_kn=560.0, portata_km=140.0,
                    testata_kg=221.0, peso_kg=1300.0, lunghezza_m=4.63, in_servizio_dal=1981,
                    guida='inerziale, radar attivo in banda J; capsula dal tubo', sagoma='sprite:navi/harpoon.spr',
                    fonte='dossier 14 §2.2', nota='Mach 0,85; peso di capsula e missile STIMA (690 kg il missile)'),
    'tasm': dict(nome='UGM-109B Tomahawk TASM', tipo='missile', famiglia='tomahawk', velocita_kn=475.0,
                 portata_km=460.0, testata_kg=450.0, peso_kg=1590.0, lunghezza_m=6.25, in_servizio_dal=1983,
                 ritirata_nel=1994, pozzo_verticale=True, guida='inerziale, radar attivo, volo radente',
                 sagoma='sprite:navi/tomahawk.spr', fonte='dossier 14 §2.3', confidenza='alta',
                 nota='antinave: il «tomahawk» delle missioni originali; ritirato all\'inizio degli anni \'90'),
    'tlam_c': dict(nome='UGM-109C Tomahawk TLAM-C', tipo='missile', famiglia='attacco_terra', velocita_kn=475.0,
                   portata_km=1250.0, testata_kg=450.0, peso_kg=1490.0, lunghezza_m=6.25, in_servizio_dal=1986,
                   pozzo_verticale=True, da_famiglia=False, guida='inerziale, TERCOM e DSMAC',
                   sagoma='sprite:navi/tomahawk.spr', fonte='dossier 14 §2.3', confidenza='alta',
                   nota='attacco a terra: solo se la missione lo dichiara'),
    'sea_lance': dict(nome='UUM-125 Sea Lance', tipo='missile', famiglia='sea_lance', velocita_kn=860.0,
                      portata_km=185.0, testata_kg=45.0, peso_kg=1400.0, lunghezza_m=6.25, what_if=True,
                      guida='inerziale; capsula, poi siluro leggero Mk 50 a paracadute', sagoma='icona:razzo',
                      fonte='dossier 14 §2.5', confidenza='bassa',
                      nota='cancellato nel 1990: arma «what-if» dell\'originale; prestazioni e peso STIMA'),
    'mk67': dict(nome='Mk 67 SLMM', tipo='mina', famiglia='mina', velocita_kn=20.0, portata_km=13.0, testata_kg=150.0, peso_kg=754.0,
                 lunghezza_m=4.1, diametro_mm=485, in_servizio_dal=1983, sagoma='icona:mina_mobile',
                 guida='mina mobile: corre fino a 13 km, poi si posa sul fondo', fonte='dossier 14 §2.6',
                 nota='1 per posto STIMA; 754-800 kg nelle fonti'),
    'mk60': dict(nome='Mk 60 CAPTOR', tipo='mina', famiglia='mina', testata_kg=44.0, peso_kg=935.0, lunghezza_m=3.35,
                 in_servizio_dal=1979, sagoma='icona:mina_captor',
                 guida='ancorata: libera un siluro Mk 46 al passaggio di un sottomarino', fonte='dossier 14 §2.7',
                 confidenza='alta', nota='1 per posto STIMA; testata quella del Mk 46'),
    'moss_mk70': dict(nome='MOSS Mk 70', famiglia='esca_mobile', diametro_mm=254, lunghezza_m=4.0, peso_kg=230.0,
                      velocita_kn=10.0, in_servizio_dal=1976, what_if=True, moderno=True, sagoma='icona:esca_moss',
                      guida='esca semovente da tubo: imita rumore ed eco del battello e si allontana',
                      fonte='dossier 05 §5.1', confidenza='bassa',
                      nota='Mobile Submarine Simulator, 10 pollici in un manicotto per il tubo da 533; negli anni '
                           '\'80 solo sugli SSBN: sul 688 what-if, consentita nel modo moderno; lunghezza e peso STIMA'),
    'adc_mk2': dict(nome='ADC Mk 2', tipo='esca', famiglia='esca', diametro_mm=76, lunghezza_m=1.0, posti=0.0,
                    in_servizio_dal=1980,
                    guida='esca acustica a quota fissa, elica intubata', sagoma='icona:esca',
                    fonte='dossier 14 §2.8, dossier 05 §5.1',
                    nota='lanciatori propri, scorta a parte; 3 pollici (fonti USN); lunghezza STIMA: classificata'),
    # --- URSS ---
    # --- Regno Unito (per l'IA: il Trafalgar; la Royal Navy non e' una marina del giocatore) ---
    'tigerfish': dict(nome='Tigerfish Mk 24 Mod 1', nazione='uk', famiglia='mk48', siluro='tigerfish',
                      **_dal_modello('tigerfish'), guida='filoguidato, testa attiva o passiva', peso_kg=1550.0,
                      lunghezza_m=6.46, fonte='dossier 01 §6.1, §2.3', confidenza='media',
                      nota='il siluro dei Trafalgar nel 1989 (lo Spearfish entra nel 1994)'),
    'set65': dict(nome='SET-65', nazione='urss', famiglia='type53', siluro='set65', **_dal_modello('set65'),
                  guida='testa attiva o passiva', peso_kg=1900.0, lunghezza_m=7.8, fonte='dossier 14 §2.9',
                  nota='antisom dell\'Alfa nel 1989; peso e lunghezza STIMA'),
    'type53_65k': dict(nome='53-65K', nazione='urss', famiglia='type53', siluro='type53_65k',
                       **_dal_modello('type53_65k'), guida='segui-scia, immune alle esche', peso_kg=2070.0,
                       lunghezza_m=7.2, fonte='dossier 14 §2.9', nota='antinave; peso STIMA'),
    'saet60m': dict(nome='SAET-60M', nazione='urss', famiglia='type53', siluro='saet60m', **_dal_modello('saet60m'),
                    guida='testa passiva', peso_kg=1800.0, lunghezza_m=7.5, fonte='dossier 14 §2.9',
                    confidenza='bassa', nota='peso e lunghezza STIMA'),
    'uset80': dict(nome='USET-80', nazione='urss', famiglia='type53', siluro='uset80', **_dal_modello('uset80'),
                   guida='testa attiva o passiva', peso_kg=2000.0, lunghezza_m=7.9, da_famiglia=False,
                   fonte='dossier 14 §2.9', confidenza='bassa',
                   nota='sull\'Alfa non trovato: solo se la missione lo dichiara; peso e lunghezza STIMA'),
    'test71m': dict(nome='TEST-71M', nazione='urss', famiglia='type53', siluro='test71m', **_dal_modello('test71m'),
                    guida='filoguidato, testa attiva o passiva', peso_kg=1800.0, lunghezza_m=7.9, da_famiglia=False,
                    fonte='dossier 14 §2.9', confidenza='bassa',
                    nota='sull\'Alfa non confermato: solo se la missione lo dichiara; peso e lunghezza STIMA'),
    'shkval': dict(nome='VA-111 Shkval', nazione='urss', famiglia='type53', velocita_kn=200.0, portata_km=11.0,
                   testata_kg=210.0, peso_kg=2700.0, lunghezza_m=8.2, in_servizio_dal=1977, solo_ia=True,
                   guida='supercavitante a razzo, non guidato', sagoma='icona:razzo', fonte='dossier 14 §2.10',
                   confidenza='bassa', nota='arma segreta nel 1989: solo per l\'IA'),
    'vyuga': dict(nome='RPK-2 Vyuga (SS-N-15)', nazione='urss', tipo='missile', famiglia='sea_lance',
                  portata_km=45.0, peso_kg=1800.0, lunghezza_m=8.2, in_servizio_dal=1969, solo_ia=True,
                  guida='razzo antisom dal tubo, carica di profondita\'', sagoma='icona:razzo',
                  fonte='dossier 14 §2.11', confidenza='bassa',
                  nota='sull\'Alfa incerto: arma dell\'IA; peso e lunghezza STIMA'),
    'pmr1': dict(nome='PMR-1 Kal\'mar', nazione='urss', tipo='mina', famiglia='mina', testata_kg=200.0,
                 peso_kg=1700.0, lunghezza_m=7.8, in_servizio_dal=1970, sagoma='icona:mina_razzo',
                 guida='mina-razzo antisom ormeggiata: lancia un razzo subacqueo contro un sottomarino sopra di lei',
                 fonte='dossier 14 §2.12 [RuPMR1] [AMPMR2]', confidenza='media',
                 nota='lunga come un siluro: 1 posto; solo sottomarini in immersione (Naval Encyclopedia la dava '
                      'per mina-siluro da 400 mm anche antinave: corretto)'),
    'pmr2': dict(nome='PMR-2', nazione='urss', tipo='mina', famiglia='mina', testata_kg=175.0, peso_kg=1850.0,
                 lunghezza_m=7.83, in_servizio_dal=1973, sagoma='icona:mina_razzo',
                 guida='mina-razzo antisom ormeggiata a 200-400 m: razzo a 80-100 m/s, attacco in 7 s al piu\'',
                 fonte='dossier 14 §2.12 [AMPMR2] [WFPMR2]', confidenza='media',
                 nota='raggio doppio della PMR-1; 1972 o 1973 nelle fonti; carica 175 kg (export 300-350)'),
    'mdt': dict(nome='MDT', nazione='urss', tipo='mina', famiglia='mina', testata_kg=600.0, peso_kg=1050.0,
                lunghezza_m=2.575, diametro_mm=533, posti=0.5, in_servizio_dal=1953,
                sagoma='icona:mina_fondo', guida='mina da fondo acustico-induttiva, fino a 50 m di fondale',
                fonte='dossier 14 §2.12 [WPMDT] [AMMDT]', confidenza='media',
                nota='due per tubo, unite da un aggancio: 0,5 posti (36 sull\'Alfa al posto di 18 armi, come una fonte)'),
    'mg74': dict(nome='MG-74 Korund', nazione='urss', famiglia='esca_mobile', diametro_mm=533, lunghezza_m=3.9,
                 peso_kg=800.0, velocita_kn=10.0, in_servizio_dal=1974, sagoma='icona:esca_mg74',
                 guida='esca semovente da tubo: imita rumore ed eco del sottomarino fino a 250 m',
                 fonte='dossier 05 §5.2 e §7 [KREM]', confidenza='bassa',
                 nota='al posto di un siluro; sull\'Alfa STIMA del dossier; peso STIMA'),
    'mg84': dict(nome='MG-84 Korund', nazione='urss', tipo='esca', famiglia='esca', diametro_mm=400, lunghezza_m=3.0,
                 posti=0.0,
                 in_servizio_dal=1974, guida='esca che disturba o imita la firma del battello', sagoma='icona:esca',
                 fonte='dossier 14 §2.13', confidenza='bassa',
                 nota='nel sesto tubo come nell\'originale; lunghezza STIMA (nessuna fonte aperta)'),
}

# Efficacia per bersaglio (tabella della Sala siluri, dossier 15 §7.5 alla F-117A): 0-3 stelle contro un sottomarino
# in immersione, una nave di superficie, un bersaglio a terra e come mina (posa). STIMA del gioco dalla guida e dalla
# testata di ogni arma (dossier 14 §2): non e' un dato delle fonti.
EFFICACIA = {   # id: (sottomarino, superficie, terra, mine, nota)
    'mk48': (3, 3, 0, 0, 'antisom e antinave, filoguidato'),
    'mk48_adcap': (3, 3, 0, 0, 'come il Mod 4, testa migliore e piu\' veloce'),
    'tigerfish': (2, 2, 0, 0, 'filoguidato ma lento (35 kn) e poco affidabile nei primi modelli'),
    'harpoon': (0, 3, 0, 0, 'solo navi (radar attivo); Block 1C senza attacco a terra'),
    'tasm': (0, 3, 0, 0, 'antinave a lunga distanza'),
    'tlam_c': (0, 0, 3, 0, 'solo bersagli a terra'),
    'sea_lance': (3, 0, 0, 0, 'antisom a distanza (siluro leggero Mk 50)'),
    'mk67': (1, 2, 0, 3, 'mina mobile da fondo, a influenza: acque basse'),
    'mk60': (2, 0, 0, 3, 'CAPTOR: libera un Mk 46 solo contro i sottomarini; acque profonde'),
    'adc_mk2': (0, 0, 0, 0, 'esca: nessun effetto sui bersagli'),
    'moss_mk70': (0, 0, 0, 0, 'esca mobile: allontana i siluri dal battello, nessun effetto sui bersagli'),
    'set65': (3, 2, 0, 0, 'antisom, testa attiva o passiva'),
    'type53_65k': (0, 3, 0, 0, 'segui-scia: inutile contro un sottomarino in immersione'),
    'saet60m': (2, 2, 0, 0, 'testa solo passiva'),
    'uset80': (3, 2, 0, 0, 'antisom'),
    'test71m': (3, 2, 0, 0, 'filoguidato, antisom'),
    'shkval': (2, 2, 0, 0, 'non guidato: solo a distanza ravvicinata'),
    'vyuga': (2, 0, 0, 0, 'razzo antisom con carica di profondita\''),
    'pmr1': (2, 0, 0, 3, 'mina-razzo antisom: solo sottomarini in immersione, acque profonde'),
    'pmr2': (3, 0, 0, 3, 'mina-razzo antisom, raggio doppio della PMR-1'),
    'mdt': (1, 2, 0, 3, 'mina da fondo a influenza: acque basse (fino a 50 m)'),
    'mg74': (0, 0, 0, 0, 'esca mobile: allontana i siluri dal battello, nessun effetto sui bersagli'),
    'mg84': (0, 0, 0, 0, 'esca: nessun effetto sui bersagli'),
}
BERSAGLI = ['sottomarino', 'superficie', 'terra', 'mine']

# Comportamento delle mine dell'Enhanced (M2b, nucleo/mine.hpp; dossier 14 §2.6-2.7 e §2.12). tipo: "influenza"
# (da fondo, scoppia quando un bersaglio passa entro il raggio), "captor" (ormeggiata: rileva un sottomarino e libera
# un siluro leggero), "mina_siluro" (da fondo: rileva navi e sottomarini e lancia un siluro), "mina_razzo" (a quota
# fissa: lancia un razzo subacqueo contro un sottomarino sopra di lei). Raggi, profondita',
# armamento e cariche dove le fonti tacciono: STIMA del gioco.
MINE = {
    'mk67': dict(tipo='influenza', carica_kg=150.0, raggio_m=60.0, profondita_min_m=10.0, profondita_max_m=100.0,
                 corsa_max_m=13000.0, velocita_corsa_kn=20.0, ormeggio_m=0.0, armamento_s=600.0, siluro='',
                 navi=True, sottomarini=True, quota_min_bersaglio_m=0.0,
                 fonte='dossier 14 §2.6 (corsa 13 km, testata 150 kg); raggio, profondita\' e velocita\' STIMA',
                 nota='Mk 37 modificato: corre dal tubo lungo la rotta impostata, poi si posa; per acque basse'),
    'mk60': dict(tipo='captor', carica_kg=44.0, raggio_m=1000.0, profondita_min_m=200.0, profondita_max_m=900.0,
                 corsa_max_m=0.0, velocita_corsa_kn=0.0, ormeggio_m=150.0, armamento_s=600.0, siluro='mk46',
                 navi=False, sottomarini=True, quota_min_bersaglio_m=30.0,
                 fonte='dossier 14 §2.7 (Mk 46 in un contenitore ormeggiato); raggio e profondita\' STIMA',
                 nota='ignora le navi di superficie: libera il Mk 46 contro un sottomarino in immersione'),
    'pmr1': dict(tipo='mina_razzo', carica_kg=200.0, raggio_m=100.0, profondita_min_m=200.0,
                 profondita_max_m=1000.0, corsa_max_m=0.0, velocita_corsa_kn=0.0, ormeggio_m=0.0, quota_mina_m=300.0,
                 errore_razzo_m=10.0, solo_sopra=True, armamento_s=600.0, siluro='', navi=False, sottomarini=True,
                 quota_min_bersaglio_m=30.0,
                 fonte='dossier 14 §2.12 [RuPMR1] [AMPMR2]: carica 200 kg, bersagli 30-400 m; raggio meta\' di '
                       'quello della PMR-2, fondali come la PMR-2, quota ed errore del razzo STIMA',
                 nota='razzo subacqueo: lo scoppio e\' vicino al bersaglio; due file per coprire le quote'),
    'pmr2': dict(tipo='mina_razzo', carica_kg=175.0, raggio_m=200.0, profondita_min_m=200.0,
                 profondita_max_m=1000.0, corsa_max_m=0.0, velocita_corsa_kn=0.0, ormeggio_m=0.0, quota_mina_m=300.0,
                 errore_razzo_m=10.0, solo_sopra=True, armamento_s=600.0, siluro='', navi=False, sottomarini=True,
                 quota_min_bersaglio_m=30.0,
                 fonte='dossier 14 §2.12 [AMPMR2] [WFPMR2]: raggio 200 m in un settore di 160 gradi verso l\'alto, '
                       'fondali 200-1000 m, mina a 200-400 m; quota ed errore del razzo STIMA',
                 nota='il settore verso l\'alto e\' reso con «solo bersagli sopra la mina»'),
    'mdt': dict(tipo='influenza', carica_kg=600.0, raggio_m=50.0, profondita_min_m=8.0, profondita_max_m=50.0,
                corsa_max_m=0.0, velocita_corsa_kn=0.0, ormeggio_m=0.0, armamento_s=600.0, siluro='', navi=True,
                sottomarini=True, quota_min_bersaglio_m=0.0,
                fonte='dossier 14 §2.12 [WPMDT] [AMMDT]: 600 kg, raggio 50 m, fondali fino a 50 m; minimo STIMA',
                nota='si posa sotto il battello (nessuna corsa)'),
}

# Locali siluri per classe (o classe_variante). posizioni = le 7 (6 sull'Alfa) della stazione Armi dell'originale:
# "tubo" = tubo da siluri (armi da tubo), "esca" = lanciatore delle esche, "pozzi" = i pozzi verticali come una
# posizione sola, "" = assente (il tubo 7 «missili» del 1989 su un Flight I: i missili vanno nei tubi da siluri).
LOCALI = {
    'los_angeles': dict(nome='Los Angeles (Flight I)', nazione='usa', tubi=4, calibro_mm=533, rastrelliera=21,
                        pozzi=0, lanciatori_esche=2, esche=6, limite_in_acqua=4,
                        posizioni=['tubo'] * 4 + ['esca'] * 2 + [''], fonte='dossier 14 §1.2'),
    'los_angeles_flight2': dict(nome='Los Angeles (Flight II)', nazione='usa', tubi=4, calibro_mm=533,
                                rastrelliera=21, pozzi=12, lanciatori_esche=2, esche=6, limite_in_acqua=4,
                                posizioni=['tubo'] * 4 + ['esca'] * 2 + ['pozzi'], fonte='dossier 14 §1.2'),
    'los_angeles_688i': dict(nome='Los Angeles (688I)', nazione='usa', tubi=4, calibro_mm=533, rastrelliera=21,
                             pozzi=12, lanciatori_esche=2, esche=6, limite_in_acqua=4,
                             posizioni=['tubo'] * 4 + ['esca'] * 2 + ['pozzi'], fonte='dossier 14 §1.2'),
    # sei tubi veri; il sesto tiene le esche come nell'originale, quindi 13 in rastrelliera per le 18 armi
    'alfa': dict(nome='Alfa (progetto 705)', nazione='urss', tubi=5, calibro_mm=533, rastrelliera=13, pozzi=0,
                 lanciatori_esche=1, esche=4, limite_in_acqua=6, posizioni=['tubo'] * 5 + ['esca'],
                 fonte='dossier 14 §1.3 e §4.2'),
}

# Carichi standard per tipo di missione (dossier 14 §5.1): tubi alla partenza, rastrelliera, pozzi
_LA = {
    'antisom': dict(tubi=['mk48'] * 4, rastrelliera={'mk48': 19, 'harpoon': 2}),
    'antinave': dict(tubi=['mk48'] * 3 + ['harpoon'], rastrelliera={'mk48': 13, 'harpoon': 4, 'tasm': 4}),
    'attacco_terra': dict(tubi=['mk48'] * 2 + ['tlam_c'] * 2, rastrelliera={'mk48': 13, 'tlam_c': 6, 'harpoon': 2}),
    'minamento': dict(tubi=['mk48'] * 2 + ['mk67'] * 2, rastrelliera={'mk48': 9, 'mk67': 10, 'harpoon': 2}),
    'scorta': dict(tubi=['mk48'] * 4, rastrelliera={'mk48': 17, 'harpoon': 4}),
}
_LA2 = {   # Flight II e 688I: Tomahawk nei pozzi, i 25 posti per siluri, Harpoon e mine
    'antisom': dict(tubi=['mk48'] * 4, rastrelliera={'mk48': 19, 'harpoon': 2}, pozzi={'tasm': 4}),
    'antinave': dict(tubi=['mk48'] * 3 + ['harpoon'], rastrelliera={'mk48': 17, 'harpoon': 4}, pozzi={'tasm': 8}),
    'attacco_terra': dict(tubi=['mk48'] * 4, rastrelliera={'mk48': 19, 'harpoon': 2}, pozzi={'tlam_c': 12}),
    'minamento': dict(tubi=['mk48'] * 2 + ['mk67'] * 2, rastrelliera={'mk48': 9, 'mk67': 10, 'harpoon': 2},
                      pozzi={'tasm': 4}),
    'scorta': dict(tubi=['mk48'] * 4, rastrelliera={'mk48': 17, 'harpoon': 4}, pozzi={'tasm': 4}),
}
CARICHI = {
    'los_angeles': _LA,
    'los_angeles_flight2': _LA2,
    'los_angeles_688i': _LA2,
    'alfa': {
        'antisom': dict(tubi=['set65'] * 3 + ['type53_65k'] * 2, rastrelliera={'set65': 6, 'type53_65k': 5, 'mg74': 2}),
        'antinave': dict(tubi=['set65'] * 2 + ['type53_65k'] * 3, rastrelliera={'set65': 6, 'type53_65k': 7}),
        'minamento': dict(tubi=['set65'] * 3 + ['type53_65k'] * 2, rastrelliera={'set65': 2, 'pmr2': 6, 'mdt': 10}),
    },
}
# I carichi per compito della schermata (06/11/2026), oltre a quelli del dossier 14 §5.1: mix STIMA, le fonti dicono solo
# che cosa si puo' imbarcare. Le chiavi non devono coincidere con un tipo_missione delle missioni o dell'editor: il
# carico dell'IA lo prende per tipo di missione (armi_ia.cpp). Per questo l'Alfa non ha `scorta`
_LA_COMPITI = {
    'caccia': dict(tubi=['mk48_adcap'] * 4, rastrelliera={'mk48_adcap': 2, 'mk48': 19}),
    'saturazione': dict(tubi=['mk48'] * 2 + ['harpoon'] * 2, rastrelliera={'mk48': 9, 'harpoon': 8, 'tasm': 4}),
    'costiero': dict(tubi=['mk48'] * 3 + ['harpoon'], rastrelliera={'mk48': 11, 'harpoon': 4, 'mk67': 6}),
    'barriera': dict(tubi=['mk48'] * 2 + ['mk60'] * 2, rastrelliera={'mk48': 9, 'mk60': 10, 'harpoon': 2}),
}
_LA2_COMPITI = {
    'caccia': dict(tubi=['mk48_adcap'] * 4, rastrelliera={'mk48_adcap': 2, 'mk48': 19}, pozzi={'tasm': 4}),
    'saturazione': dict(tubi=['mk48'] * 2 + ['harpoon'] * 2, rastrelliera={'mk48': 13, 'harpoon': 8}, pozzi={'tasm': 12}),
    'costiero': dict(tubi=['mk48'] * 3 + ['harpoon'], rastrelliera={'mk48': 11, 'harpoon': 4, 'mk67': 6},
                     pozzi={'tasm': 6, 'tlam_c': 6}),
    'barriera': dict(tubi=['mk48'] * 2 + ['mk60'] * 2, rastrelliera={'mk48': 9, 'mk60': 10, 'harpoon': 2}, pozzi={'tasm': 4}),
}
CARICHI['los_angeles'] = {**_LA, **_LA_COMPITI}
CARICHI['los_angeles_flight2'] = {**_LA2, **_LA2_COMPITI}
CARICHI['los_angeles_688i'] = {**_LA2, **_LA2_COMPITI}
CARICHI['alfa'].update({
    'caccia': dict(tubi=['set65'] * 5, rastrelliera={'set65': 11, 'mg74': 2}),
    'agguato': dict(tubi=['saet60m'] * 3 + ['type53_65k'] * 2, rastrelliera={'saet60m': 8, 'type53_65k': 3, 'mg74': 2}),
    'barriera': dict(tubi=['set65'] * 3 + ['type53_65k'] * 2, rastrelliera={'set65': 3, 'pmr2': 10}),
    'costiero': dict(tubi=['set65'] * 3 + ['type53_65k'] * 2, rastrelliera={'set65': 2, 'type53_65k': 1, 'mdt': 20}),
})
# Nome, nota e fonte per l'elenco della schermata, nell'ordine in cui si mostrano
COMPITI = {
    'antisom': dict(nome='Caccia antisom', nota='Il carico di partenza piu\' comune: quasi tutto contro i sottomarini.',
                    fonte='dossier 14 §5.1', confidenza='PROPOSTA'),
    'caccia': dict(nome='Caccia pura', nota='Solo siluri: tutti i posti contro i sottomarini.',
                   fonte='dossier 14 §1.2, §1.3, §5.1', confidenza='STIMA'),
    'agguato': dict(nome='Agguato passivo', nota='Teste solo passive contro i sottomarini, segui-scia contro le navi.',
                    fonte='dossier 14 §1.3 (SET-65A o SAET-60A)', confidenza='STIMA, fonte bassa'),
    'barriera': dict(nome='Barriera antisom', nota='Mine contro i sottomarini in acque profonde: da 200 m di fondale in su.',
                     fonte='dossier 14 §2.7, §2.12', confidenza='STIMA'),
    'antinave': dict(nome='Antinave', nota='Contro le navi, tenendo qualche arma per i sottomarini.', fonte='dossier 14 §5.1', confidenza='PROPOSTA'),
    'saturazione': dict(nome='Antinave a distanza', nota='Piu\' missili possibile, a scapito dei siluri.',
                        fonte='dossier 14 §3.6', confidenza='STIMA'),
    'scorta': dict(nome='Scorta e sorveglianza', nota='Come l\'originale: siluri e qualche missile.',
                   fonte='dossier 14 §5.1', confidenza='PROPOSTA'),
    'costiero': dict(nome='Attacco costiero', nota='Acque basse: mine da fondo fino a 50-100 m, siluri, missili se ci sono.',
                     fonte='dossier 14 §2.6, §2.12, §3.6', confidenza='STIMA'),
    'minamento': dict(nome='Minamento', nota='Molte mine, pochi siluri.', fonte='dossier 14 §5.1', confidenza='PROPOSTA'),
    'attacco_terra': dict(nome='Attacco a terra', nota='TLAM-C: solo se la missione li dichiara.',
                          fonte='dossier 14 §5.1, §3.6', confidenza='PROPOSTA'),
}
# variante predefinita di ogni famiglia per nazione: quella che vale per le chiavi di famiglia del deposito (le
# missioni originali) e che prende il posto di un'arma mancante nei carichi standard
PREDEFINITE = {
    'usa': {'mk48': 'mk48', 'harpoon': 'harpoon', 'tomahawk': 'tasm', 'sea_lance': 'sea_lance', 'mina': 'mk67',
            'attacco_terra': 'tlam_c', 'esca': 'adc_mk2', 'esca_mobile': 'moss_mk70'},
    'urss': {'type53': 'set65', 'mina': 'pmr2', 'esca': 'mg84', 'esca_mobile': 'mg74'},
    'uk': {'mk48': 'tigerfish'},
}
# senza tipo di missione: il carico del caso piu' comune
PREDEFINITO = {'los_angeles': 'antisom', 'los_angeles_flight2': 'antisom', 'los_angeles_688i': 'antisom',
               'alfa': 'antisom'}


def _tabella(d):
    return '{ ' + ', '.join(f'{k} = {valore(v)}' for k, v in d.items()) + ' }'


def _lista(v):
    return '[' + ', '.join(valore(x) for x in v) + ']'


def esporta_carico(radice):
    righe = ['# Armi, locali siluri e carichi standard della Sala siluri (generato da tools/pipeline/carico.py: '
             'non modificare a mano).', '']
    assert set(EFFICACIA) == set(ARMI), set(EFFICACIA) ^ set(ARMI)
    for a, campi in ARMI.items():
        ignoti = set(campi) - set(BASE)
        assert not ignoti, (a, ignoti)
        scheda = {**BASE, **campi}
        assert scheda['famiglia'], a
        assert not scheda['siluro'] or scheda['siluro'] in SILURI, a
        righe.append(f'[armi.{a}]')
        righe += [f'{k} = {valore(float(v) if k in ("posti",) else v)}' for k, v in scheda.items()]
        *stelle, nota = EFFICACIA[a]
        assert all(0 <= x <= 3 for x in stelle), a
        righe.append('efficacia = { ' + ', '.join(f'{b} = {x}' for b, x in zip(BERSAGLI, stelle)) + ' }')
        righe += [f'efficacia_nota = {valore(nota)}', 'efficacia_fonte = "dossier 14 §2, dossier 15 §7.5"',
                  'efficacia_confidenza = "STIMA"']
        righe.append('')
    for m, campi in MINE.items():
        assert ARMI[m].get('tipo') == 'mina' and (not campi['siluro'] or campi['siluro'] in SILURI), m
        righe += [f'[mine.{m}]', *(f'{k} = {valore(v)}' for k, v in campi.items()), '']
    assert {a for a, c in ARMI.items() if c.get('tipo') == 'mina'} == set(MINE)
    for c, locale in LOCALI.items():
        righe.append(f'[locali.{c}]')
        for k, v in locale.items():
            righe.append(f'{k} = {_lista(v) if isinstance(v, list) else valore(v)}')
        righe.append('')
    for n, fam in PREDEFINITE.items():
        assert all(ARMI[a]['famiglia'] == f and ARMI[a].get('nazione', 'usa') == n for f, a in fam.items()), n
        righe += [f'[predefinite.{n}]', *(f'{f} = {valore(a)}' for f, a in fam.items()), '']
    for c, tipi in CARICHI.items():
        righe += [f'[carichi.{c}]', f'predefinito = {valore(PREDEFINITO[c])}', '']
        posti = LOCALI[c]['tubi'] + LOCALI[c]['rastrelliera']
        for t, car in tipi.items():
            assert all(x in ARMI for x in car['tubi'] + list(car['rastrelliera']) + list(car.get('pozzi', {}))), (c, t)
            usati = len(car['tubi']) + sum(n * ARMI[x].get('posti', 1.0) for x, n in car['rastrelliera'].items())
            assert len(car['tubi']) == LOCALI[c]['tubi'] and usati <= posti, (c, t, usati)
            assert sum(car.get('pozzi', {}).values()) <= LOCALI[c]['pozzi'], (c, t)
            righe += [f'[carichi.{c}.{t}]', f'tubi = {_lista(car["tubi"])}',
                      f'rastrelliera = {_tabella(car["rastrelliera"])}', f'pozzi = {_tabella(car.get("pozzi", {}))}',
                      '']
    assert {x for tipi in CARICHI.values() for x in tipi} == set(COMPITI)
    for k, (tipo, campi) in enumerate(COMPITI.items()):
        righe += [f'[compiti.{tipo}]', f'ordine = {k}', *(f'{c} = {valore(v)}' for c, v in campi.items()), '']
    uscita = os.path.join(radice, 'data', 'motore', 'carico.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(ARMI), len(LOCALI), sum(len(t) for t in CARICHI.values())


if __name__ == '__main__':
    print('armi, locali, carichi:', esporta_carico(sys.argv[1] if len(sys.argv) > 1 else '.'))
