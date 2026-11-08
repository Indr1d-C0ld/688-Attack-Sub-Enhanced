"""ASW aereo dell'Enhanced (docs/13_asw_aereo.md, pezzo A0) -> data/motore/asw_aereo.toml.
Lo legge nucleo/asw_aereo.hpp (carica_asw_aereo); il significato dei campi e' in DatiBoa, DatiMarcatore,
DatiSonarImmersione, DatiMezzo, DatiEquipaggio, DatiDifficolta, ProbabilitaAsw e SceltaSchema.
Dati dal dossier docs/ricerca/17_asw_aereo.md, citato per paragrafo; le sigle delle fonti sono quelle del §11.

Ogni voce porta:
- `fonte`: le sigle delle fonti della voce (il paragrafo del dossier e le sigle del §11);
- `conf`: alta, media o bassa (dossier §0);
- `stime`: i campi che sono STIMA, cioe' valori proposti da tarare (pezzo A6), non storici;
- `fonti` (facoltativo): la fonte dei campi che non vengono dalla fonte della voce.
Unita': metri, secondi, nodi, ore per i TOS; quote in metri anche dove la fonte usa i piedi (il commento li riporta).
Uso: asw_aereo.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import valore   # noqa: E402

FT = 0.3048
NM = 1852.0
KMH = 1 / 1.852   # km/h -> kn


def ft(x):
    return round(x * FT, 1)


# --- boe (§4.1 USA, §4.2 URSS) ---------------------------------------------------------------------------------------
# Campi di ogni boa (i mancanti prendono il valore di BASE_BOA); funzione: vedi FunzioneBoa
BASE_BOA = {'nome': '', 'parte': 'usa', 'funzione': 'passiva_omni', 'sensore': '', 'quote_m': [], 'vite_s': [],
            'caduta_m_s': 30.0, 'attivazione_s': 0.0, 'guasto': 0.05, 'sigma_rilevamento_deg': 0.0,
            'portata_m': 0.0, 'soglia': False, 'soglia_s': 0.0, 'periodo_ping_s': 0.0, 'ping_secondi': 0.0,
            'mare_max': 9.0, 'falsi_allarmi_h': 0.0, 'frequenze_hz': [], 'sl_db': 0.0,
            'fonte': '', 'conf': 'media', 'stime': [], 'fonti': {}}

# attivazione_s = 0: la stabilizzazione si calcola dalla quota dell'idrofono (§4.1: 100 s a 30 m ... 240 s a 300 m).
# portata_m: R50 indicativa contro un SSN medio a 5-8 kn, per la spaziatura degli schemi (§6.1: S = 1,5-2 R50);
# la rivelazione vera la fa il motore acustico col sensore `sensore` (A2).
# falsi_allarmi_h: contatti spontanei per ora di boa attiva, oltre a quelli veri dei biologici e dei mercantili del
# paesaggio sonoro; tarati sul §7.2 (K -> R: 0,2 USA, 0,35-0,5 URSS) e sul 50% del Korshun nel 1985 (§9).
BOE = {
    # --- USA ---
    'ssq41b': dict(nome='AN/SSQ-41B (LOFAR)', funzione='passiva_omni', sensore='ssq41b',
                   quote_m=[ft(60), ft(1000)], vite_s=[3600.0, 10800.0, 28800.0], portata_m=5556.0,
                   falsi_allarmi_h=0.03, frequenze_hz=[10.0, 10000.0],
                   fonte='§4.1 [FASSSQ41] [HOLLER]', conf='media',
                   stime=['guasto', 'portata_m', 'falsi_allarmi_h'], fonti={'caduta_m_s': '§4.1 [U53D]'}),
    'ssq53b': dict(nome='AN/SSQ-53B (DIFAR)', funzione='passiva_dir', sensore='ssq53b',
                   quote_m=[ft(90), ft(1000)], vite_s=[3600.0, 10800.0, 28800.0], sigma_rilevamento_deg=7.0,
                   portata_m=7408.0, falsi_allarmi_h=0.03, frequenze_hz=[10.0, 2400.0], mare_max=6.0,
                   fonte='§4.1 [HOLLER] [AAD-DIFAR]', conf='alta',
                   stime=['quote_m', 'guasto', 'portata_m', 'falsi_allarmi_h', 'frequenze_hz', 'mare_max'],
                   fonti={'mare_max': 'dossier 21 §8 punto 4 (SSQ-53D fino a SS6; nessun dato per la B)'}),
    'ssq77a': dict(nome='AN/SSQ-77A (VLAD)', funzione='passiva_dir', sensore='ssq77a',
                   quote_m=[ft(500), ft(1000)], vite_s=[3600.0, 14400.0, 28800.0], sigma_rilevamento_deg=7.0,
                   portata_m=9260.0, falsi_allarmi_h=0.02, frequenze_hz=[10.0, 2400.0],
                   fonte='§4.1 [HOLLER] [FASSSQ77]', conf='alta',
                   stime=['guasto', 'portata_m', 'falsi_allarmi_h', 'sigma_rilevamento_deg', 'frequenze_hz']),
    'ssq57a': dict(nome='AN/SSQ-57A (tarata)', funzione='tarata', sensore='ssq57a',
                   quote_m=[ft(90), ft(400)], vite_s=[3600.0, 10800.0], fonte='§4.1 [HOLLER]', conf='alta',
                   stime=['vite_s', 'guasto']),
    'ssq47b': dict(nome='AN/SSQ-47B (attiva, distanza)', funzione='attiva_distanza', sensore='ssq47b',
                   quote_m=[ft(60), ft(800)], vite_s=[1800.0], periodo_ping_s=10.0, portata_m=1500.0,
                   frequenze_hz=[13000.0, 14200.0, 15400.0, 16600.0, 17800.0, 19000.0], sl_db=195.0,
                   fonte='§4.1 [HOLLER]', conf='alta', stime=['portata_m', 'sl_db', 'guasto', 'frequenze_hz']),
    'ssq62b': dict(nome='AN/SSQ-62B (DICASS)', funzione='attiva_dir', sensore='ssq62b',
                   quote_m=[ft(90), ft(400), ft(1500)], vite_s=[1800.0], ping_secondi=100.0, portata_m=1800.0,
                   sigma_rilevamento_deg=5.0, frequenze_hz=[6500.0, 7500.0, 8500.0, 9500.0], sl_db=200.0,
                   fonte='§4.1 [FASSSQ62] [U62G]', conf='alta',
                   stime=['vite_s', 'portata_m', 'sigma_rilevamento_deg', 'guasto'],
                   fonti={'sl_db': '§8.3 [CALC]', 'portata_m': '§8.3 [CALC]'}),
    'ssq36': dict(nome='AN/SSQ-36 (batitermografo)', funzione='batitermografo', sensore='',
                  quote_m=[ft(1000), ft(2625)], vite_s=[600.0], fonte='§4.1 [FASSSQ36]', conf='media',
                  stime=['vite_s', 'guasto']),
    # --- URSS ---
    'rgb_nm': dict(nome='RGB-NM «Chinara»', parte='urss', funzione='passiva_omni', sensore='rgb_nm',
                   quote_m=[20.0], vite_s=[21600.0], attivazione_s=90.0, guasto=0.25, portata_m=2700.0,
                   soglia=True, soglia_s=12.0, mare_max=3.0, falsi_allarmi_h=0.25, frequenze_hz=[7000.0, 10000.0],
                   fonte='§4.2 [ART-Be12] [CIA72]', conf='alta', stime=['falsi_allarmi_h'],
                   fonti={'guasto': '§4.2 (affidabilita\' 0,7-0,8) e §7.4'}),
    'rgb_nm1': dict(nome='RGB-NM-1 «Zheton»', parte='urss', funzione='passiva_omni', sensore='rgb_nm',
                    quote_m=[20.0, 40.0, 100.0], vite_s=[21600.0], attivazione_s=90.0, guasto=0.25, portata_m=2700.0,
                    soglia=True, soglia_s=12.0, mare_max=3.0, falsi_allarmi_h=0.25, frequenze_hz=[7000.0, 10000.0],
                    fonte='§4.2 [ART-Be12] [CIA72]', conf='alta', stime=['falsi_allarmi_h']),
    'rgb1': dict(nome='RGB-1', parte='urss', funzione='passiva_omni', sensore='rgb1', quote_m=[35.0, 70.0],
                 vite_s=[10800.0], caduta_m_s=34.0, guasto=0.25, portata_m=2800.0, soglia=True, soglia_s=10.0,
                 falsi_allarmi_h=0.25, frequenze_hz=[100.0, 10000.0],
                 fonte='§4.2 [ART-Il38] [RW-Il38]', conf='alta',
                 stime=['vite_s', 'falsi_allarmi_h', 'frequenze_hz', 'soglia_s'],
                 fonti={'guasto': '§3.4 e §7.4 (fino al 30% guaste a terra)'}),
    'rgb2': dict(nome='RGB-2', parte='urss', funzione='passiva_dir', sensore='rgb2', quote_m=[20.0],
                 vite_s=[2700.0], guasto=0.2, sigma_rilevamento_deg=6.0, portata_m=1750.0, falsi_allarmi_h=0.15,
                 frequenze_hz=[300.0, 5000.0], fonte='§4.2 [ART-Il38] [ART-Be12N]', conf='alta',
                 stime=['guasto', 'falsi_allarmi_h', 'frequenze_hz']),
    'rgb75': dict(nome='RGB-75 (infrasonora)', parte='urss', funzione='passiva_omni', sensore='rgb75',
                  quote_m=[150.0], vite_s=[18000.0], guasto=0.2, portata_m=1000.0, mare_max=2.0,
                  falsi_allarmi_h=0.5, frequenze_hz=[2.0, 60.0], fonte='§4.2 [ART-Tu142]', conf='alta',
                  stime=['quote_m', 'guasto', 'falsi_allarmi_h'],
                  fonti={'falsi_allarmi_h': '§9 (Korshun 1985: 50% di falsi allarmi)'}),
    'rgb15': dict(nome='RGB-15', parte='urss', funzione='passiva_omni', sensore='rgb15', quote_m=[150.0],
                  vite_s=[7200.0], guasto=0.2, portata_m=750.0, falsi_allarmi_h=0.4, frequenze_hz=[2.0, 5000.0],
                  fonte='§4.2 [ART-Tu142]', conf='alta', stime=['quote_m', 'guasto', 'falsi_allarmi_h']),
    'rgb25': dict(nome='RGB-25', parte='urss', funzione='passiva_dir', sensore='rgb25', quote_m=[150.0, 50.0],
                  vite_s=[3600.0], guasto=0.2, sigma_rilevamento_deg=6.0, portata_m=2000.0, mare_max=2.0,
                  falsi_allarmi_h=0.3, frequenze_hz=[100.0, 5000.0], fonte='§4.2 [ART-Tu142]', conf='alta',
                  stime=['quote_m', 'vite_s', 'guasto', 'sigma_rilevamento_deg', 'portata_m', 'falsi_allarmi_h',
                         'frequenze_hz']),
    'rgb55a': dict(nome='RGB-55A (attiva)', parte='urss', funzione='attiva_omni', sensore='rgb55a',
                   quote_m=[150.0], vite_s=[3600.0], guasto=0.2, portata_m=5000.0,
                   frequenze_hz=[7000.0, 8000.0, 9000.0, 10000.0], sl_db=200.0, fonte='§4.2 [ART-Tu142]',
                   conf='alta', stime=['quote_m', 'guasto', 'frequenze_hz', 'sl_db']),
    'mgab': dict(nome='MGAB (sorgente esplosiva)', parte='urss', funzione='esplosiva', sensore='',
                 quote_m=[25.0, 150.0, 400.0], vite_s=[0.0], guasto=0.05, sl_db=215.0,
                 fonte='§4.2 [ART-Tu142]', conf='alta', stime=['guasto', 'sl_db']),
    'rlb': dict(nome='RLB «Poplavok-1A» (radar-faro)', parte='urss', funzione='radar_faro', sensore='',
                quote_m=[0.0], vite_s=[1200.0], guasto=0.1, fonte='§4.2 [RW-Ka25]', conf='alta', stime=['guasto']),
}

# --- marcatori (§4.3) ------------------------------------------------------------------------------------------------
MARCATORI = {
    'mk25': dict(nome='Mk 25 (fumogeno e fiamma)', parte='usa', durata_s=900.0, visibile_m=7400.0,
                 fonte='§4.3 [AUS-MK25]', conf='alta', stime=['visibile_m']),
    'mk58': dict(nome='Mk 58 (due candele)', parte='usa', durata_s=3000.0, visibile_m=4700.0,
                 fonte='§4.3 [AUS-MK58]', conf='alta', stime=[]),
    'mk84': dict(nome='Mk 84 SUS (segnalatore acustico)', parte='usa', durata_s=80.0, acustico=True,
                 frequenza_hz=3000.0, fonte='§4.3 [PRC68SUS]', conf='media', stime=[]),
    'omab': dict(nome='OMAB-25-12D/-8N', parte='urss', durata_s=1800.0, visibile_m=5000.0,
                 fonte='§4.3 [RW-Ka25] [ART-Il38]', conf='alta', stime=['durata_s', 'visibile_m']),
}

# --- sonar a immersione (§6.4-6.5, §2.2-2.3, §3.1-3.2) ---------------------------------------------------------------
# Una stazione di ricerca dura dip_ricerca_s (hover, calata, ascolto e ping, recupero); in inseguimento: ascolto
# fra ascolto_min_s e ascolto_max_s e ping_min-ping_max ping. Poi il salto a salto_velocita_kn.
SONAR = {
    'aqs13b': dict(nome='AN/AQS-13B', sensore='aqs13b', hover_m=ft(40), cavo_max_m=ft(450), avvicinamento_s=45.0,
                   calata_m_s=2.0, recupero_m_s=2.0, ascolto_min_s=30.0, ascolto_max_s=60.0, ping_min=3, ping_max=4,
                   periodo_ping_s=8.0, p_attivo=1.0, dip_ricerca_s=900.0, salto_ricerca_m=10 * NM,
                   salto_inseguimento_m=5 * NM, salto_velocita_kn=114.0, frequenza_hz=10000.0, sl_db=211.0,
                   notte=True, mare_max=5.0, hs_max_m=3.0, vento_max_kn=35.0,
                   fonte='§6.4 [RUDZIS] [MILAQS13B]', conf='alta',
                   stime=['avvicinamento_s', 'calata_m_s', 'recupero_m_s', 'ascolto_min_s', 'ascolto_max_s',
                          'periodo_ping_s', 'mare_max'],
                   fonti={'mare_max': 'dossier 21 §8 punto 6 (N fino a 3, D 4, D/X 5 con onde oltre 10 ft, X da 6)',
                          'vento_max_kn': 'dossier 02 §7.7; dossier 21 §8 punto 6'}),
    'aqs13f': dict(nome='AN/AQS-13F', sensore='aqs13f', hover_m=ft(60), cavo_max_m=ft(1450), avvicinamento_s=45.0,
                   calata_m_s=2.9, recupero_m_s=2.9, ascolto_min_s=30.0, ascolto_max_s=60.0, ping_min=3, ping_max=4,
                   periodo_ping_s=8.0, p_attivo=1.0, dip_ricerca_s=900.0, salto_ricerca_m=10 * NM,
                   salto_inseguimento_m=5 * NM, salto_velocita_kn=130.0, frequenza_hz=10000.0, sl_db=215.0,
                   notte=True, mare_max=5.0, hs_max_m=3.0, vento_max_kn=35.0,
                   fonte='§2.3 [WPAQS13] [SIKS70B]', conf='media',
                   stime=['cavo_max_m', 'calata_m_s', 'recupero_m_s', 'sl_db', 'avvicinamento_s', 'ascolto_min_s',
                          'ascolto_max_s', 'salto_velocita_kn', 'mare_max']),
    'vgs3': dict(nome='VGS-3 «Ros\'-V»', sensore='vgs3', hover_m=30.0, cavo_max_m=150.0, avvicinamento_s=45.0,
                 calata_m_s=2.0, recupero_m_s=2.0, ascolto_min_s=180.0, ascolto_max_s=240.0, ping_min=2, ping_max=3,
                 periodo_ping_s=15.0, p_attivo=0.1, dip_ricerca_s=420.0, salto_ricerca_m=5000.0,
                 salto_inseguimento_m=5000.0, salto_velocita_kn=32.0, frequenza_hz=8500.0, sl_db=205.0,
                 notte=False, mare_max=5.0, vento_max_kn=35.0,
                 fonte='§3.2 §6.5 [ART-Ka27]', conf='media',
                 stime=['avvicinamento_s', 'calata_m_s', 'recupero_m_s', 'p_attivo', 'frequenza_hz', 'sl_db',
                        'mare_max'],
                 fonti={'mare_max': 'dossier 21 §7.1 e §8 punto 6 (Ka-27 fino a 5 баллов)',
                        'ascolto_min_s': '§6.5 (3-10 min, stretto sul ciclo di 10-12 min)',
                        'salto_velocita_kn': '§6.5 (ricerca a 50-60 km/h)'}),
    'vgs2': dict(nome='VGS-2 «Oka»', sensore='vgs2', hover_m=25.0, cavo_max_m=125.0, avvicinamento_s=80.0,
                 calata_m_s=1.5, recupero_m_s=1.5, ascolto_min_s=240.0, ascolto_max_s=330.0, ping_min=2, ping_max=4,
                 periodo_ping_s=15.0, p_attivo=0.3, dip_ricerca_s=540.0, salto_ricerca_m=5000.0,
                 salto_inseguimento_m=5000.0, salto_velocita_kn=24.0, frequenza_hz=8000.0, sl_db=200.0,
                 notte=False, mare_max=5.0, vento_max_kn=30.0,
                 fonte='§3.1 [RW-Ka25] [CIA72] [ART-Ka27]', conf='media',
                 stime=['hover_m', 'avvicinamento_s', 'calata_m_s', 'recupero_m_s', 'ascolto_min_s', 'ascolto_max_s',
                        'ping_min', 'ping_max', 'periodo_ping_s', 'p_attivo', 'dip_ricerca_s', 'frequenza_hz',
                        'sl_db', 'mare_max', 'vento_max_kn'],
                 fonti={'dip_ricerca_s': '§3.1 (ciclo 13-15 min, STIMA)',
                        'mare_max': 'dossier 21 §7.1 (Ka-25/Ka-27: D a 5, X da 6)'}),
}

INTERVALLO_POSA_BE12 = 120.0

# --- mezzi (§2-3, §6, §6.8) -------------------------------------------------------------------------------------------
# id = classe di classi.toml. TOS(d) = tos_base_h - d_nm / tos_nm_per_h (§6.8). Velocita' in kn, quote in m.
# carichi: il primo e' il predefinito; armi per voce di siluri.toml (at2, apr2, umgt1 da aggiungere in A3) o bomba.
BASE_MEZZO = {'nome': '', 'parte': 'usa', 'elicottero': False, 'dal_anno': 0, 'radar': '', 'mad': None, 'sonar': '',
              'carichi': {}, 'canali_boe': 16, 'velocita_transito_kn': 300.0, 'velocita_ricerca_kn': 200.0,
              'velocita_posa_kn': 200.0, 'quota_transito_m': 7600.0, 'quota_ricerca_m': 460.0,
              'quota_posa_m': 460.0, 'quota_attacco_m': 150.0, 'tos_base_h': 8.0, 'tos_nm_per_h': 165.0,
              'riserva_h': 0.5, 'equipaggio': 'medio', 'gruppo_tipico': 1, 'conferma': 1.0, 'falsi_allarmi': 1.0,
              'intervallo_posa_s': 0.0,
              'fonte': '', 'conf': 'media',
              'stime': [], 'fonti': {}}


def mad(quota_giorno_m, velocita_kn, fattore=1.0, quota_notte_m=None, cavo_m=0.0, fonte='§6.3 [NPS-MAD84]',
        conf='media'):
    return dict(fattore=fattore, quota_giorno_m=quota_giorno_m,
                quota_notte_m=quota_giorno_m if quota_notte_m is None else quota_notte_m, velocita_kn=velocita_kn,
                cavo_m=cavo_m, fonte=fonte, conf=conf)


MEZZI = {
    # --- USA ---
    'sh60b': dict(nome='SH-60B Seahawk (LAMPS III)', elicottero=True, radar='aps124',
                  mad=mad(ft(150), 90.0, cavo_m=ft(180), fonte='§2.1 §6.3 [SBIRMAD]'),
                  carichi={'asw': dict(boe={'ssq53b': 15, 'ssq62b': 5, 'ssq41b': 4, 'ssq36': 1}, armi={'mk46': 2},
                                       marcatori={'mk25': 6})},
                  canali_boe=8, velocita_transito_kn=125.0, velocita_ricerca_kn=100.0, velocita_posa_kn=100.0,
                  quota_transito_m=ft(1000), quota_ricerca_m=ft(1000), quota_posa_m=ft(500), quota_attacco_m=ft(150),
                  tos_base_h=4.0, tos_nm_per_h=50.0, fonte='§2.1 [GAOALFS] [SIKS70B] [FASSH60] [NHHCSH60B]',
                  conf='media',
                  stime=['carichi', 'canali_boe', 'velocita_ricerca_kn', 'velocita_posa_kn', 'quota_transito_m',
                         'quota_ricerca_m', 'quota_posa_m'],
                  fonti={'tos_base_h': '§6.8 [NHHCSH60B] [CALC]', 'quota_attacco_m': '§6.7 [JPROC]'}),
    # LAMPS Mk I delle Knox (e di Kidd, Spruance e Perry prima del LAMPS III): niente sonar a immersione; il radar di
    # superficie LN-66HP non e' nel catalogo dell'elettronica (dossier 11), quindi radar vuoto come l'SH-3H
    'sh2f': dict(nome='SH-2F Seasprite (LAMPS I)', elicottero=True, radar='',
                 mad=mad(ft(150), 90.0, cavo_m=ft(180), fonte='§2.1b §6.3 [FASSH2] [GSSH2]'),
                 carichi={'asw': dict(boe={'ssq41b': 9, 'ssq47b': 5, 'ssq36': 1}, armi={'mk46': 2}, marcatori={'mk25': 6})},
                 canali_boe=4, velocita_transito_kn=120.0, velocita_ricerca_kn=100.0, velocita_posa_kn=100.0,
                 quota_transito_m=ft(1000), quota_ricerca_m=ft(1000), quota_posa_m=ft(500), quota_attacco_m=ft(150),
                 tos_base_h=2.5, tos_nm_per_h=50.0, fonte='§2.1b [FASSH2] [GSSH2] [ACSH2F]', conf='media',
                 stime=['carichi', 'canali_boe', 'velocita_ricerca_kn', 'velocita_posa_kn', 'quota_transito_m',
                        'quota_ricerca_m', 'quota_posa_m', 'quota_attacco_m', 'tos_base_h', 'tos_nm_per_h'],
                 fonti={'tos_base_h': '§2.1b (autonomia 3,5-4,5 h nelle fonti, STIMA)'}),
    'sh3h': dict(nome='SH-3H Sea King', elicottero=True, radar='', sonar='aqs13b',
                 mad=mad(ft(150), 90.0, cavo_m=ft(180), fonte='§2.2 §6.3 [RUDZIS]'),
                 carichi={'asw': dict(boe={'ssq53b': 6, 'ssq62b': 2, 'ssq41b': 2}, armi={'mk46': 2},
                                      marcatori={'mk25': 12})},
                 canali_boe=4, velocita_transito_kn=114.0, velocita_ricerca_kn=100.0, velocita_posa_kn=100.0,
                 quota_transito_m=ft(150), quota_ricerca_m=ft(150), quota_posa_m=ft(150), quota_attacco_m=ft(150),
                 tos_base_h=3.0, tos_nm_per_h=57.0, gruppo_tipico=2, fonte='§2.2 §6.4 [RUDZIS] [MILAQS13B]',
                 conf='alta', stime=['carichi', 'canali_boe', 'velocita_ricerca_kn', 'velocita_posa_kn'],
                 fonti={'tos_base_h': '§6.8 [CALC] da [RUDZIS]', 'gruppo_tipico': '§5.3 (2 in zona interna)'}),
    'sh60f': dict(nome='SH-60F Ocean Hawk', elicottero=True, dal_anno=1990, radar='', sonar='aqs13f',
                  carichi={'asw': dict(boe={'ssq53b': 4, 'ssq62b': 2}, armi={'mk46': 2}, marcatori={'mk25': 6})},
                  canali_boe=4, velocita_transito_kn=130.0, velocita_ricerca_kn=110.0, velocita_posa_kn=110.0,
                  quota_transito_m=ft(150), quota_ricerca_m=ft(150), quota_posa_m=ft(150), quota_attacco_m=ft(150),
                  tos_base_h=3.5, tos_nm_per_h=57.0, gruppo_tipico=2, fonte='§2.3 [WPAQS13] [SIKS70B] [WPSH60]',
                  conf='media',
                  stime=['carichi', 'canali_boe', 'velocita_transito_kn', 'velocita_ricerca_kn', 'velocita_posa_kn',
                         'tos_base_h', 'tos_nm_per_h']),
    'p3c': dict(nome='P-3C Orion', radar='aps115', mad=mad(ft(250), 200.0, fonte='§2.4 §6.3 [FASP3] [DMNP3]'),
                carichi={'sosus': dict(boe={'ssq53b': 45, 'ssq41b': 10, 'ssq77a': 5, 'ssq62b': 9, 'ssq36': 3,
                                            'ssq57a': 3}, armi={'mk46': 4}, marcatori={'mk25': 12, 'mk58': 6, 'mk84': 6}),
                         'barriera': dict(boe={'ssq53b': 40, 'ssq41b': 20, 'ssq77a': 10, 'ssq62b': 4, 'ssq36': 2,
                                               'ssq57a': 2}, armi={'mk46': 4}, marcatori={'mk25': 12, 'mk58': 6, 'mk84': 6}),
                         'guerra': dict(boe={'ssq53b': 45, 'ssq41b': 8, 'ssq77a': 4, 'ssq62b': 10, 'ssq36': 2},
                                        armi={'mk46': 8}, marcatori={'mk25': 12, 'mk58': 6})},
                canali_boe=16, velocita_transito_kn=330.0, velocita_ricerca_kn=206.0, velocita_posa_kn=225.0,
                quota_transito_m=ft(25000), quota_ricerca_m=ft(1500), quota_posa_m=ft(1500), quota_attacco_m=ft(400),
                tos_base_h=8.1, tos_nm_per_h=165.0, equipaggio='esperto', fonte='§2.4 §6.2 [FASP3] [HARP3] [HOLLER]',
                conf='media', stime=['carichi', 'canali_boe', 'velocita_posa_kn', 'quota_attacco_m'],
                fonti={'tos_base_h': '§6.8 [CALC] su [FASP3]', 'equipaggio': '§7.4 (Update III)',
                       'quota_attacco_m': '§6.7 [HARPHAAWC] (300-1000 ft, di norma sotto i 500)'}),
    's3a': dict(nome='S-3A Viking', radar='aps116', mad=mad(ft(300), 200.0, fonte='§2.5 §6.3 [WPS3] [TWZS3]'),
                carichi={'asw': dict(boe={'ssq53b': 40, 'ssq41b': 6, 'ssq62b': 8, 'ssq36': 2, 'ssq57a': 3},
                                     armi={'mk46': 4}, marcatori={'mk25': 8, 'mk58': 4, 'mk84': 4})},
                canali_boe=16, velocita_transito_kn=345.0, velocita_ricerca_kn=180.0, velocita_posa_kn=200.0,
                quota_transito_m=ft(36000), quota_ricerca_m=ft(1500), quota_posa_m=ft(1500), quota_attacco_m=ft(400),
                tos_base_h=5.0, tos_nm_per_h=170.0, fonte='§2.5 [WPS3] [TWZS3] [USNIS3]', conf='media',
                stime=['carichi', 'velocita_ricerca_kn', 'velocita_posa_kn', 'quota_ricerca_m', 'quota_posa_m',
                       'quota_attacco_m', 'tos_base_h', 'tos_nm_per_h']),
    's3b': dict(nome='S-3B Viking', radar='aps137', mad=mad(ft(300), 200.0, fonte='§2.5 §6.3 [WPS3] [TWZS3]'),
                carichi={'asw': dict(boe={'ssq53b': 38, 'ssq41b': 4, 'ssq77a': 4, 'ssq62b': 8, 'ssq36': 2,
                                          'ssq57a': 3}, armi={'mk46': 4}, marcatori={'mk25': 8, 'mk58': 4, 'mk84': 4})},
                canali_boe=16, velocita_transito_kn=345.0, velocita_ricerca_kn=180.0, velocita_posa_kn=200.0,
                quota_transito_m=ft(36000), quota_ricerca_m=ft(1500), quota_posa_m=ft(1500), quota_attacco_m=ft(400),
                tos_base_h=5.0, tos_nm_per_h=170.0, equipaggio='esperto', fonte='§2.5 [FASS3] [TWZS3]', conf='media',
                stime=['carichi', 'velocita_ricerca_kn', 'velocita_posa_kn', 'quota_ricerca_m', 'quota_posa_m',
                       'quota_attacco_m', 'tos_base_h', 'tos_nm_per_h'], fonti={'equipaggio': '§7.4'}),
    # --- URSS ---
    'ka25': dict(nome='Ka-25PL Hormone-A', parte='urss', elicottero=True, radar='initsiativa2k', sonar='vgs2',
                 carichi={'ricerca': dict(boe={'rgb_nm': 36, 'rlb': 3}, armi={}, marcatori={'omab': 4}),
                          'attacco': dict(boe={'rlb': 3}, armi={'at1': 1}, marcatori={'omab': 4})},
                 canali_boe=18, velocita_transito_kn=round(195 * KMH, 1), velocita_ricerca_kn=round(40 * KMH, 1),
                 velocita_posa_kn=80.0, quota_transito_m=300.0, quota_ricerca_m=150.0, quota_posa_m=150.0,
                 quota_attacco_m=150.0, tos_base_h=1.33, tos_nm_per_h=81.0, equipaggio='scarso', gruppo_tipico=2,
                 fonte='§3.1 [RW-Ka25] [AW-Ka25] [ART-Ka27]', conf='media',
                 stime=['velocita_posa_kn', 'quota_transito_m', 'quota_ricerca_m', 'quota_posa_m', 'quota_attacco_m',
                        'tos_base_h', 'tos_nm_per_h'],
                 fonti={'equipaggio': '§7.4', 'gruppo_tipico': '§3.1 (coppie ricerca-attacco)',
                        'quota_attacco_m': '§6.7 (elicotteri sovietici a 100-300 m, STIMA)'}),
    'ka27': dict(nome='Ka-27PL Helix-A', parte='urss', elicottero=True, radar='initsiativa2km', sonar='vgs3',
                 mad=mad(70.0, 60.0, fattore=0.9, cavo_m=85.0, fonte='§3.2 §6.3 [ART-Ka27]'),
                 carichi={'ricerca': dict(boe={'rgb_nm1': 36}, armi={}, marcatori={'omab': 4}),
                          'attacco': dict(boe={'rgb_nm1': 8}, armi={'apr2': 1}, marcatori={'omab': 4}),
                          'kiev_1987': dict(boe={'rgb_nm1': 18}, armi={}, marcatori={'omab': 4})},
                 canali_boe=18, velocita_transito_kn=120.0, velocita_ricerca_kn=round(55 * KMH, 1),
                 velocita_posa_kn=90.0, quota_transito_m=300.0, quota_ricerca_m=100.0, quota_posa_m=100.0,
                 quota_attacco_m=150.0, tos_base_h=2.0, tos_nm_per_h=108.0, gruppo_tipico=4,
                 fonte='§3.2 §6.5 [ART-Ka27] [RW-Ka27]', conf='media-alta',
                 stime=['velocita_transito_kn', 'velocita_posa_kn', 'quota_transito_m', 'quota_ricerca_m',
                        'quota_posa_m', 'quota_attacco_m', 'carichi'],
                 fonti={'tos_base_h': '§6.8 (1,5 h a 100 km, 1 h a 200 km)', 'gruppo_tipico': '§6.5 (4-6 dal Kiev)'}),
    'be12': dict(nome='Be-12 Mail', parte='urss', radar='initsiativa2b',
                 mad=mad(85.0, round(310 * KMH, 1), fattore=0.7, fonte='§3.3 §6.3 [ART-Be12]'),
                 # le boe: il campo storico di 60-70 RGB lo posa la coppia (§9 [ART-Be12], fonte alta; §3.3), quindi
                 # 32 per aereo con l'AT-1, 40 in sola ricerca (STIMA, taratura del 27/10/2026: con 60 la coppia ne
                 # posava 113 e la scoperta saliva a 0,73, sopra lo 0,5-0,6 del campo)
                 carichi={'ricerca_attacco': dict(boe={'rgb_nm': 32}, armi={'at1': 1}, marcatori={'omab': 6}),
                          'ricerca': dict(boe={'rgb_nm': 40}, armi={}, marcatori={'omab': 6})},
                 canali_boe=18, velocita_transito_kn=round(450 * KMH, 1), velocita_ricerca_kn=round(310 * KMH, 1),
                 velocita_posa_kn=round(375 * KMH, 1), quota_transito_m=2000.0, quota_ricerca_m=85.0,
                 quota_posa_m=500.0, quota_attacco_m=500.0, tos_base_h=5.5, tos_nm_per_h=108.0, gruppo_tipico=2,
                 # ritmo di posa: una boa ogni INTERVALLO_POSA_BE12 s al piu', a mano dal vano dell'anfibio. STIMA della
                 # taratura A6.5 del 25/10/2026: la coppia (§3.3) nel campo di boe del §9, p 0,5-0,6
                 intervallo_posa_s=INTERVALLO_POSA_BE12,
                 fonte='§3.3 §6.6 [ART-Be12] [ART-Be12N]', conf='alta',
                 stime=['carichi', 'velocita_posa_kn', 'quota_attacco_m', 'tos_base_h', 'tos_nm_per_h'],
                 fonti={'quota_transito_m': '§3.3 (controllo radar da 2000 m)', 'gruppo_tipico': '§3.3 (sempre in coppia)'}),
    'il38': dict(nome='Il-38 May', parte='urss', radar='wet_eye',
                 mad=mad(100.0, round(350 * KMH, 1), fattore=0.9, quota_notte_m=200.0, fonte='§3.4 §6.3 [ART-Il38]'),
                 carichi={'ricerca_attacco': dict(boe={'rgb1': 144, 'rgb2': 10}, armi={'at2': 2, 'plab250': 10},
                                                  marcatori={'omab': 8})},
                 canali_boe=24, velocita_transito_kn=round(456 * KMH, 1), velocita_ricerca_kn=round(350 * KMH, 1),
                 velocita_posa_kn=round(375 * KMH, 1), quota_transito_m=8000.0, quota_ricerca_m=1000.0,
                 quota_posa_m=1000.0, quota_attacco_m=500.0, tos_base_h=10.3, tos_nm_per_h=162.0,
                 equipaggio='scarso', fonte='§3.4 §6.6 [ART-Il38] [RW-Il38]', conf='alta',
                 # il «Berkut» elabora peggio (§9, «8-10 volte peggio» del P-3C, fonte media): conferma piu' lenta e piu'
                 # falsi allarmi delle boe, solo per l'Il-38. Il campo di boe resta quello del Be-12 (§9, fonte alta).
                 # STIMA, taratura A6.2 del 25/10/2026, con l'evasione dell'IA
                 conferma=0.08, falsi_allarmi=3.0,
                 stime=['quota_transito_m', 'quota_attacco_m', 'tos_base_h', 'tos_nm_per_h', 'carichi', 'conferma',
                        'falsi_allarmi'],
                 fonti={'equipaggio': '§7.4 (Berkut)', 'tos_base_h': '§6.8 (7 h a 1000 km, 3 h a 2200 km)',
                        'conferma': '§9 e taratura A6.2', 'falsi_allarmi': '§9 e taratura A6.2'}),
    'tu142': dict(nome='Tu-142MK Bear-F', parte='urss', radar='korshun',
                  mad=mad(200.0, round(450 * KMH, 1), fattore=0.75, fonte='§3.5 §6.3 [ART-Tu142]'),
                  carichi={'ricerca_attacco': dict(boe={'rgb75': 66, 'rgb15': 44, 'rgb25': 10, 'rgb55a': 10},
                                                   armi={'at2': 3}, marcatori={'omab': 8}),
                           'eco': dict(boe={'rgb15': 44, 'rgb25': 10, 'mgab': 200}, armi={'at2': 1},
                                       marcatori={'omab': 8})},
                  canali_boe=24, velocita_transito_kn=round(735 * KMH, 1), velocita_ricerca_kn=round(450 * KMH, 1),
                  velocita_posa_kn=round(450 * KMH, 1), quota_transito_m=7750.0, quota_ricerca_m=600.0,
                  quota_posa_m=600.0, quota_attacco_m=600.0, tos_base_h=14.0, tos_nm_per_h=180.0,
                  fonte='§3.5 §6.6 [ART-Tu142] [RW-Tu142]', conf='alta',
                  stime=['canali_boe', 'quota_ricerca_m', 'quota_posa_m', 'quota_attacco_m', 'tos_base_h',
                         'tos_nm_per_h', 'mad'],
                  fonti={'tos_base_h': '§6.8 (8 h a 2000 km, 2 h a 4000 km)'}),
}

# --- equipaggi (§7.4) e difficolta' (§10.3) ---------------------------------------------------------------------------
EQUIPAGGI = {
    'esperto': dict(errore_posa_m=91.0, falsi_allarmi=0.7, ritardo=0.7, perdita_contatto=0.7),
    'medio': dict(errore_posa_m=183.0, falsi_allarmi=1.0, ritardo=1.0, perdita_contatto=1.0),
    'scarso': dict(errore_posa_m=366.0, falsi_allarmi=1.5, ritardo=1.5, perdita_contatto=1.5),
}
DIFFICOLTA = {
    'facile': dict(scoperta=0.5, scoperta_urss=1.0, equipaggio='scarso', notte=False, guasti=False, deriva=False),
    # docs/13, decisione 2: equipaggi medi
    'normale': dict(scoperta=1.0, scoperta_urss=1.0, equipaggio='medio', notte=True, guasti=True, deriva=False),
    'realistico': dict(scoperta=1.0, scoperta_urss=1.0, equipaggio='', notte=True, guasti=True, deriva=True),
    'minaccia_percepita': dict(scoperta=1.0, scoperta_urss=5.0, equipaggio='', notte=True, guasti=True, deriva=True),
}

# --- probabilita' delle transizioni (§7.2) e della manovra degli elicotteri (§7.3), tutte STIMA -----------------------
PROBABILITA = {
    'usa': dict(falso_allarme=0.2, conferma=0.7, finestra_conferma_s=600.0, fix=0.7, finestra_fix_s=600.0,
                mad_bassa=0.6, mad_alta=0.3, passaggio_contatto=0.5, riacquisizione=0.4,
                finestra_riacquisizione_s=1800.0, perdita_s=600.0, rinuncia_s=3600.0, scarta_vero=0.05,
                crede_falso=0.03, perso_spirale=0.5, attacco_hover=0.4, attacco_altro=0.0),
    'urss': dict(falso_allarme=0.42, conferma=0.5, finestra_conferma_s=600.0, fix=0.5, finestra_fix_s=600.0,
                 mad_bassa=0.48, mad_alta=0.24, passaggio_contatto=0.16, riacquisizione=0.3,
                 finestra_riacquisizione_s=1800.0, perdita_s=600.0, rinuncia_s=3600.0, scarta_vero=0.1,
                 crede_falso=0.15, perso_spirale=0.6, attacco_hover=0.0, attacco_altro=0.7),
}

# --- scelta degli schemi (§7.3): situazione -> [(schema, p, funzione della boa, numero di boe)] ------------------------
SCHEMI = {
    'usa': {
        'datum_fresco_rotta_ignota': [('cerchio', 0.6, 'passiva_dir', 8), ('cuneo', 0.4, 'passiva_dir', 6)],
        'datum_fresco_rotta_nota': [('chevron', 0.6, 'passiva_dir', 6), ('trappola', 0.4, 'passiva_dir', 4)],
        'datum_medio': [('cerchio', 0.3, 'passiva_dir', 8), ('trappola', 0.5, 'passiva_dir', 5),
                        ('barriera', 0.2, 'passiva_dir', 6)],
        'datum_vecchio': [('barriera', 0.7, 'passiva_dir', 8), ('campo', 0.3, 'passiva_omni', 12)],
        'difar_singolo': [('cuneo', 0.5, 'passiva_dir', 5), ('incrocio', 0.5, 'passiva_dir', 2)],
        # un rilevamento solo da lontano (oltre 2 portate d'incertezza): DIFAR lungo il rilevamento, sfalsate di lato,
        # finche' una piu' vicina incrocia (STIMA, taratura A6.4 del 25/10/2026: Foxtrot sentito a 30 km)
        'difar_lontano': [('lungo_rilevamento', 1.0, 'passiva_dir', 6)],
        'fix': [('box_dicass', 0.7, 'attiva_dir', 4), ('passaggio_mad', 0.3, 'passiva_dir', 0)],
        'perso': [('cerchio', 0.5, 'passiva_dir', 6), ('trappola', 0.5, 'passiva_dir', 4)],
        'ricerca': [('barriera', 0.6, 'passiva_dir', 8), ('campo', 0.4, 'passiva_omni', 12)],
    },
    'urss': {
        'arrivo': [('barriera', 0.7, 'passiva_omni', 10), ('scacchiera', 0.3, 'passiva_omni', 16)],
        'arrivo_campo': [('campo', 1.0, 'passiva_omni', 12)],   # Tu-142M: RGB-75 a 15-20 km
        'immersione': [('spirale', 1.0, 'passiva_omni', 10)],
        'boa_reagisce': [('barriera_circolare', 0.6, 'passiva_omni', 10), ('barriera_stretta', 0.4, 'passiva_dir', 4)],
        'rotta_stimata': [('intercettazione', 1.0, 'passiva_omni', 10)],
        'silenzioso': [('eco_esplosiva', 0.6, 'esplosiva', 3), ('attiva', 0.4, 'attiva_omni', 2)],
        'perso': [('barriera_circolare', 0.6, 'passiva_omni', 10), ('intercettazione', 0.4, 'passiva_omni', 10)],
        'datum': [('barriera_circolare', 1.0, 'passiva_omni', 10)],
    },
}

SCHEMI_NOTI = {'barriera', 'barriera_stretta', 'chevron', 'cerchio', 'cuneo', 'trappola', 'box_dicass', 'cloverleaf',
               'scacchiera', 'barriera_circolare', 'spirale', 'intercettazione', 'campo', 'incrocio', 'passaggio_mad',
               'eco_esplosiva', 'attiva', 'lungo_rilevamento'}
FUNZIONI = {'passiva_omni', 'passiva_dir', 'tarata', 'attiva_distanza', 'attiva_dir', 'attiva_omni', 'batitermografo',
            'esplosiva', 'radar_faro'}
ARMI_NOTE = {'mk46', 'at1', 'at2', 'apr2', 'umgt1', 'plab250', 'plab50'}

PENSIERO_S = 5.0   # il controllore pensa ogni 5 s (docs/13, «Ritmo»)

# sottomarini per il MAD (§6.3): dislocamento in immersione (t) e fattore magnetico; classi di classi.toml
DISLOCAMENTI = {
    'los_angeles': (6900.0, 1.0, '§6.3'), 'victor3': (7000.0, 1.0, '§6.3'), 'akula': (13000.0, 1.0, '§6.3'),
    'typhoon': (25000.0, 1.0, '§6.3'), 'kilo': (3000.0, 1.0, '§6.3 (2-3,2 kt)'),
    'foxtrot': (2500.0, 1.0, '§6.3 (2-3,2 kt)'),
    'alfa': (3200.0, 0.65, '§6.3: scafo in titanio, restano macchinari e reattore (0,6-0,7 STIMA)'),
    'oscar': (24000.0, 1.0, 'STIMA (schede pubbliche)'), 'charlie2': (5500.0, 1.0, 'STIMA (schede pubbliche)'),
    'sturgeon': (4800.0, 1.0, 'STIMA (schede pubbliche)'), 'trafalgar': (5200.0, 1.0, 'STIMA (schede pubbliche)'),
}


def _tabella(d):
    return '{ ' + ', '.join(f'{k} = {valore(v)}' for k, v in d.items()) + ' }'


def _campo(k, v):
    if isinstance(v, list): return f'{k} = [' + ', '.join(valore(x) for x in v) + ']'
    if isinstance(v, dict): return f'{k} = {_tabella(v)}'
    return f'{k} = {valore(v)}'


def _voce(sezione, chiave, campi, base=None):
    if base is not None:
        ignoti = set(campi) - set(base)
        assert not ignoti, (chiave, ignoti)
        campi = {**base, **campi}
    noti = set(campi)
    assert all(s in noti for s in campi.get('stime', [])), (chiave, campi.get('stime'))
    assert all(f in noti for f in campi.get('fonti', {})), (chiave, campi.get('fonti'))
    righe = [f'[{sezione}.{chiave}]']
    sotto = []
    for k, v in campi.items():
        if isinstance(v, dict) and k in ('mad', 'carichi'): sotto.append((k, v)); continue
        if v is None: continue
        righe.append(_campo(k, v))
    for k, v in sotto:
        if k == 'mad':
            righe += ['', f'[{sezione}.{chiave}.mad]', *(_campo(a, b) for a, b in v.items())]
        else:
            for nome, car in v.items():
                righe += ['', f'[{sezione}.{chiave}.carichi.{nome}]', *(_campo(a, b) for a, b in car.items())]
    return righe + ['']


def esporta_asw_aereo(radice):
    righe = ['# ASW aereo dell\'Enhanced (generato da tools/pipeline/asw_aereo.py: non modificare a mano).',
             '# Dati dal dossier docs/ricerca/17_asw_aereo.md (paragrafi e sigle delle fonti del §11).',
             '# conf: alta, media, bassa; stime: i campi STIMA da tarare (pezzo A6); fonti: le fonti per campo.', '',
             f'pensiero_s = {PENSIERO_S}', '']
    for c, (t, f, fonte) in DISLOCAMENTI.items():
        righe += [f'[dislocamenti.{c}]', f'dislocamento_t = {t}', f'fattore = {f}', f'fonte = {valore(fonte)}', '']
    for b, campi in BOE.items():
        assert campi['funzione'] in FUNZIONI and campi['quote_m'] and campi['vite_s'], b
        righe += _voce('boe', b, campi, BASE_BOA)
    for m, campi in MARCATORI.items():
        righe += _voce('marcatori', m, campi)
    for s, campi in SONAR.items():
        righe += _voce('sonar_immersione', s, campi)
    for m, campi in MEZZI.items():
        car = campi['carichi']
        assert car, m
        for nome, c in car.items():
            assert all(b in BOE for b in c['boe']), (m, nome)
            assert all(a in ARMI_NOTE for a in c['armi']), (m, nome)
            assert all(x in MARCATORI for x in c['marcatori']), (m, nome)
        assert not campi.get('sonar') or campi['sonar'] in SONAR, m
        voce = {**campi, 'carico_predefinito': next(iter(car))}
        righe += _voce('mezzi', m, voce, {**BASE_MEZZO, 'carico_predefinito': ''})
    for e, campi in EQUIPAGGI.items():
        righe += [f'[equipaggi.{e}]', *(_campo(k, v) for k, v in campi.items()), '']
    for d, campi in DIFFICOLTA.items():
        righe += [f'[difficolta.{d}]', *(_campo(k, v) for k, v in campi.items()), '']
    for p, campi in PROBABILITA.items():
        righe += [f'[probabilita.{p}]', *(_campo(k, v) for k, v in campi.items()),
                  'fonte = "§7.2 §7.3 [NPS-FD00] [ART-Be12] [ART-Tu142]"', 'conf = "bassa"', 'stime = ["tutti"]', '']
    for p, situazioni in SCHEMI.items():
        for s, scelte in situazioni.items():
            assert abs(sum(x[1] for x in scelte) - 1.0) < 1e-9, (p, s)
            assert all(x[0] in SCHEMI_NOTI and x[2] in FUNZIONI for x in scelte), (p, s)
            righe += [f'[schemi.{p}.{s}]',
                      'scelte = [' + ', '.join(_tabella(dict(schema=x[0], p=x[1], funzione=x[2], boe=x[3]))
                                               for x in scelte) + ']',
                      'fonte = "§6.1 §6.6 §7.3"', 'conf = "bassa"', '']
    uscita = os.path.join(radice, 'data', 'motore', 'asw_aereo.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(BOE), len(MEZZI), len(SONAR)


if __name__ == '__main__':
    print('boe, mezzi, sonar a immersione:', esporta_asw_aereo(sys.argv[1] if len(sys.argv) > 1 else '.'))
