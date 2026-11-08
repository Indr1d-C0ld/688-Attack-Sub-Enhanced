"""Meteo e mare dell'Enhanced (docs/15_meteo_e_mare.md, pezzo M0) -> data/motore/meteo.toml.
Lo legge nucleo/meteo.hpp (M1, il generatore del runtime). Dati dal dossier docs/ricerca/21_meteo_e_mare.md (il
«dossier 21», citato per paragrafo) e dalle sue note (A climatologia, B sensori, C mezzi, D briefing e grafica in
docs/ricerca/21_note/); la climatologia per mese e punto viene da tools/meteo/climatologia_era5.json (ERA5 1981-1990,
tools/meteo/climatologia.py).

Ogni voce porta:
- `fonte`: il paragrafo del dossier o della nota e la fonte;
- `conf`: alta, media o bassa (dossier §0);
- `stime`: i campi che sono STIMA, cioe' valori proposti da tarare (pezzo M6), non misurati.
Unita': m/s per il vento (nodi solo dove detto), metri, ore, gradi veri (direzione DA cui viene il vento o il mare),
°C, mm/h, km per le visibilita' e i fetch.

Struttura:
- [generale]: scale (Douglas, Beaufort), relazioni fisiche (Pierson-Moskowitz, Koschmieder, Monahan, Overland),
  previsione del 1989, lampi;
- [teatri.<id>]: correzioni, fetch, foschia di fondo, ciclo diurno, swell, ghiaccio, temporali;
  - [teatri.<id>.mesi.<1-12>]: climatologia ERA5 del punto centrale;
  - [teatri.<id>.stagioni.<stagione>]: persistenze, Markov, burrasche, minimi barici, ciclo diurno;
  - [teatri.<id>.regimi.<regime>]: i regimi sinottici del teatro (dossier 21 §4) con frequenza per stagione;
  - [teatri.<id>.eventi.<evento>]: gli eventi rari (dossier 21 §3.3);
  - [[teatri.<id>.punti]]: i punti ERA5 del riquadro, per mese, da interpolare in `Meteo::in(pos)`.
Uso: meteo.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import json, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import valore   # noqa: E402

STAGIONI = ('inverno', 'primavera', 'estate', 'autunno')
SETTORI = ('N', 'NE', 'E', 'SE', 'S', 'SW', 'W', 'NW')
ERA5 = 'ERA5 1981-1990 (tools/meteo/climatologia_era5.json; nota A §2)'


def _campo(k, v):
    if isinstance(v, dict): return f'{k} = {{ ' + ', '.join(f'{a} = {valore(b)}' for a, b in v.items()) + ' }'
    if isinstance(v, list):
        if v and isinstance(v[0], list): return f'{k} = [' + ', '.join('[' + ', '.join(_v(x) for x in r) + ']' for r in v) + ']'
        return f'{k} = [' + ', '.join(_v(x) for x in v) + ']'
    return f'{k} = {_v(v)}'


def _v(x):
    return 'nan' if x is None else valore(x)


def _voce(sezione, campi, obbligo=True):
    if obbligo:
        assert campi.get('fonte') and campi.get('conf') in ('alta', 'media', 'bassa'), sezione
        assert all(s in campi for s in campi.get('stime', [])), (sezione, campi.get('stime'))
    return [f'[{sezione}]', *(_campo(k, v) for k, v in campi.items()), '']


# --- generale ---------------------------------------------------------------------------------------------------------
GENERALE = dict(
    # Douglas/WMO 3700: limiti superiori di Hs per le classi 0-8 (oltre: 9) e centri delle classi (una sola tabella per
    # acustica e periscopi, dossier 21 §8 punto 2)
    douglas_limiti_m=[0.0001, 0.1, 0.5, 1.25, 2.5, 4.0, 6.0, 9.0, 14.0],
    douglas_centri_m=[0.0, 0.05, 0.3, 0.875, 1.875, 3.25, 5.0, 7.5, 11.5, 14.0],
    beaufort_limiti_m_s=[0.3, 1.6, 3.4, 5.5, 8.0, 10.8, 13.9, 17.2, 20.8, 24.5, 28.5, 32.7],
    # mare: Hs_eq = min(H_CEM(durata, fetch), coeff U^2/g); rilassamento; Tp = coeff sqrt(Hs)
    hs_pm_coeff=0.22, tp_pm_coeff=5.0, tau_crescita_h=[4.0, 8.0], tau_decadimento_h=[12.0, 24.0],
    cem_cd=[0.001, 1.1, 0.035], cem_fetch=[0.0413, 0.751], cem_durata=[0.00523, 1.5],
    raffica_fattore=[1.3, 1.5],
    # visibilita': V = 3,912 / sigma (km); sigma = a C^b con C (g/m3); pioggia C = 0,072 R^0,88
    koschmieder=3.912, estinzione_nebbia=[144.7, 0.88], estinzione_pioggia=[2.24, 0.75], estinzione_neve=[10.36, 0.7776],
    acqua_pioggia=[0.072, 0.88], spruzzi_da_beaufort=9,
    # schiuma (Monahan e O'Muircheartaigh 1980): W = a U^b
    schiuma_monahan=[3.84e-6, 3.41],
    # spruzzo gelante (Overland 1990): PPR = V (Tf - Ta) / (1 + 0,3 (Tw - Tf)); classi in PPR e in cm/h
    overland_tf_c=-1.7, overland_classi_ppr=[22.4, 53.3, 83.0], overland_classi_cm_h=[0.7, 2.0, 4.0],
    # previsione del 1989 (dossier 21 §5.1): orizzonte (h), errore d'ora dei fronti (h), sigma vento (m/s), sigma Hs (m)
    previsione_orizzonti_h=[12.0, 24.0, 48.0, 72.0], previsione_errore_fronti_h=[2.0, 4.0, 10.0, 18.0],
    previsione_sigma_vento_m_s=[1.5, 2.5, 3.5, 4.5], previsione_sigma_hs_m=[0.3, 0.5, 0.7, 0.9],
    previsione_burrasche_in_ritardo=0.25, previsione_mai=['polar_low', 'getto', 'temporale', 'tromba_marina'],
    bollettini_ore_utc=[0.0, 6.0, 12.0, 18.0],
    # lampi (dossier 21 §6.2, nota D B-bis): colpi per scarica, colpo singolo, intervallo, scariche al minuto in una cella
    lampi_colpi_media=3.67, lampi_singolo=0.3, lampi_intervallo_ms=57.0, lampi_al_minuto=[1.0, 4.0],
    lampi_canale_km=15.0, lampi_bagliore_km=150.0, tuono_km=16.0, tuono_ritardo_s_km=3.0,
    # celle temporalesche: raggio, velocita', vita (STIMA)
    celle_raggio_km=[5.0, 15.0], celle_vita_h=[1.0, 3.0], celle_radar_nm=20.0,
    # foschia per massa d'aria (km) e nebbia (dossier 21 §4 punto 6, nota A §6.8)
    nebbia_vento_max_m_s=10.0, nebbia_avvezione_vento_m_s=[2.0, 8.0], fumo_di_mare_delta_c=9.0,
    fumo_di_mare_strato_m=[5.0, 50.0], nebbia_visibilita_km=[0.1, 0.4], nebbia_notte=1.2, nebbia_pomeriggio=0.8,
    neve_sotto_c=1.0, costa_nm=[5.0, 25.0],
    fonte='dossier 21 §2, §4, §5.1, §6.2; nota A §6.5-6.8, §8.2; nota D B-bis', conf='media',
    stime=['hs_pm_coeff', 'tau_crescita_h', 'tau_decadimento_h', 'raffica_fattore', 'previsione_errore_fronti_h',
           'previsione_sigma_vento_m_s', 'previsione_sigma_hs_m', 'previsione_burrasche_in_ritardo', 'lampi_al_minuto',
           'celle_raggio_km', 'celle_vita_h', 'nebbia_visibilita_km', 'fumo_di_mare_strato_m', 'nebbia_notte',
           'nebbia_pomeriggio', 'neve_sotto_c', 'costa_nm', 'tuono_km'])

# --- regimi sinottici (dossier 21 §4, nota A §7.2): famiglia di regimi per teatro, STIMA ---------------------------------
# Ogni regime: vento come fattore della media del mese, direzione da cui viene (None: dalla rosa del mese), nubi (%),
# probabilita' di precipitazione per ora e intensita' (mm/h), visibilita' di fondo (km), anomalia di temperatura
# dell'aria (°C, inverno/estate), durata mediana (h), probabilita' di temporale per ora, nebbia possibile.
def R(vento, da, nubi, pioggia, mm_h, vis_km, dt_c, durata_h, temporale=0.0, nebbia='', prossimi=(), **altro):
    return dict(vento_fattore=vento, da_gradi=-1.0 if da is None else float(da), nubi_pct=nubi, pioggia_ora=pioggia,
                intensita_mm_h=mm_h, visibilita_km=vis_km, anomalia_t_c=dt_c, durata_h=durata_h,
                temporale_ora=temporale, nebbia=nebbia, prossimi=list(prossimi), **altro)


ATLANTICO = {
    'anticiclonico': R(0.6, None, 25, 0.03, 0.3, 40.0, [-1.0, 1.0], 36, prossimi=['prefrontale', 'bloccato']),
    'prefrontale': R(1.1, 170, 75, 0.30, 0.6, 15.0, [0.0, 0.0], 18, nebbia='avvezione', prossimi=['settore_caldo']),
    'settore_caldo': R(1.2, 225, 90, 0.40, 0.8, 8.0, [2.0, 1.0], 12, nebbia='avvezione', prossimi=['fronte_freddo']),
    'fronte_freddo': R(1.6, 260, 100, 0.80, 3.0, 5.0, [-1.0, 0.0], 5, temporale=0.15, prossimi=['postfrontale']),
    'postfrontale': R(1.3, 300, 55, 0.25, 1.5, 40.0, [-4.0, -1.0], 24, temporale=0.02, prossimi=['anticiclonico', 'prefrontale']),
    'bloccato': R(0.7, None, 50, 0.10, 0.5, 20.0, [-2.0, 2.0], 72, prossimi=['prefrontale', 'anticiclonico']),
}
# frequenza dei regimi per stagione (quota del tempo, somma 1), comune all'Atlantico (STIMA da ciclo di 5-6 giorni)
QUOTE_ATLANTICO = {'anticiclonico': 0.27, 'prefrontale': 0.15, 'settore_caldo': 0.10, 'fronte_freddo': 0.05,
                   'postfrontale': 0.23, 'bloccato': 0.20}

REGIMI = {
    'faroe': (ATLANTICO, {s: QUOTE_ATLANTICO for s in STAGIONI}),
    'celtica': (ATLANTICO, {s: QUOTE_ATLANTICO for s in STAGIONI}),
    'barents': (ATLANTICO, {s: QUOTE_ATLANTICO for s in STAGIONI}),
    'baltico': (ATLANTICO, {s: QUOTE_ATLANTICO for s in STAGIONI}),
    'gibilterra': ({
        'levante': R(1.2, 90, 45, 0.08, 1.0, 10.0, [0.0, 0.0], 48, nebbia='avvezione', incanalamento=1.5,
                     prossimi=['poniente', 'calma']),
        'poniente': R(1.1, 275, 25, 0.03, 0.5, 40.0, [-1.0, 0.0], 48, incanalamento=1.4, prossimi=['levante', 'calma', 'vendaval']),
        'vendaval': R(1.3, 225, 85, 0.45, 2.0, 6.0, [1.0, 0.0], 12, temporale=0.08, prossimi=['poniente']),
        'calma': R(0.5, None, 20, 0.01, 0.2, 20.0, [0.0, 1.0], 24, nebbia='radiazione', prossimi=['levante', 'poniente']),
        'cut_off': R(1.0, None, 80, 0.35, 2.0, 8.0, [0.0, 0.0], 200, temporale=0.05, prossimi=['poniente', 'levante']),
    }, {'inverno': {'levante': 0.32, 'poniente': 0.38, 'vendaval': 0.10, 'calma': 0.12, 'cut_off': 0.08},
        'primavera': {'levante': 0.33, 'poniente': 0.42, 'vendaval': 0.07, 'calma': 0.13, 'cut_off': 0.05},
        'estate': {'levante': 0.40, 'poniente': 0.42, 'vendaval': 0.0, 'calma': 0.18, 'cut_off': 0.0},
        'autunno': {'levante': 0.42, 'poniente': 0.30, 'vendaval': 0.10, 'calma': 0.12, 'cut_off': 0.06}}),
    'otranto': ({
        'bora_chiara': R(1.8, 20, 20, 0.02, 0.3, 50.0, [-4.0, 0.0], 12, esordio_h=1.5, prossimi=['calma', 'maestrale']),
        'bora_scura': R(1.6, 40, 90, 0.50, 2.0, 4.0, [-3.0, 0.0], 18, esordio_h=1.5, nebbia='precipitazione', prossimi=['calma', 'bora_chiara']),
        'scirocco': R(1.4, 140, 85, 0.35, 1.5, 6.0, [3.0, 2.0], 24, esordio_h=36.0, nebbia='precipitazione', polvere=True,
                      prossimi=['bora_scura', 'calma']),
        'maestrale': R(1.1, 335, 15, 0.01, 0.2, 30.0, [0.0, 0.0], 48, prossimi=['calma', 'scirocco']),
        'calma': R(0.6, None, 25, 0.03, 0.3, 15.0, [0.0, 0.0], 36, nebbia='radiazione', prossimi=['scirocco', 'maestrale', 'bora_chiara']),
        'temporale_autunnale': R(1.2, 170, 90, 0.60, 6.0, 3.0, [1.0, 0.0], 8, temporale=0.4, prossimi=['calma', 'bora_scura']),
    }, {'inverno': {'bora_chiara': 0.18, 'bora_scura': 0.12, 'scirocco': 0.25, 'maestrale': 0.10, 'calma': 0.32, 'temporale_autunnale': 0.03},
        'primavera': {'bora_chiara': 0.12, 'bora_scura': 0.06, 'scirocco': 0.28, 'maestrale': 0.16, 'calma': 0.36, 'temporale_autunnale': 0.02},
        'estate': {'bora_chiara': 0.02, 'bora_scura': 0.0, 'scirocco': 0.15, 'maestrale': 0.40, 'calma': 0.41, 'temporale_autunnale': 0.02},
        'autunno': {'bora_chiara': 0.12, 'bora_scura': 0.08, 'scirocco': 0.24, 'maestrale': 0.18, 'calma': 0.30, 'temporale_autunnale': 0.08}}),
    'kunashir': ({
        'monsone': R(1.2, 315, 60, 0.30, 0.8, 8.0, [-3.0, 0.0], 48, prossimi=['ciclone_esplosivo', 'calma']),
        'ciclone_esplosivo': R(1.9, 135, 100, 0.70, 2.5, 3.0, [2.0, 0.0], 18, sequenza_da_gradi=[135.0, 315.0],
                               prossimi=['monsone']),
        'flusso_nebbioso': R(0.7, 180, 95, 0.15, 0.3, 0.5, [0.0, 1.0], 36, nebbia='avvezione', prossimi=['calma', 'flusso_nebbioso']),
        'calma': R(0.6, None, 40, 0.05, 0.3, 25.0, [0.0, 0.0], 36, prossimi=['monsone', 'flusso_nebbioso', 'ciclone_esplosivo']),
    }, {'inverno': {'monsone': 0.62, 'ciclone_esplosivo': 0.18, 'flusso_nebbioso': 0.0, 'calma': 0.20},
        'primavera': {'monsone': 0.30, 'ciclone_esplosivo': 0.12, 'flusso_nebbioso': 0.25, 'calma': 0.33},
        'estate': {'monsone': 0.0, 'ciclone_esplosivo': 0.03, 'flusso_nebbioso': 0.60, 'calma': 0.37},
        'autunno': {'monsone': 0.38, 'ciclone_esplosivo': 0.15, 'flusso_nebbioso': 0.10, 'calma': 0.37}}),
}

# --- teatri: correzioni, fetch, foschia, swell, temporali, ghiaccio (dossier 21 §3-4; nota A §2, §5, §6.5, §7.6) ----
# fetch_km per settore da cui viene il vento (N, NE, E, SE, S, SW, W, NW); 0 = illimitato (oceano)
TEATRI = {
    'faroe': dict(correzione_burrasche=1.5, incanalamento=1.0, fetch_km=[800, 600, 300, 400, 0, 0, 0, 0],
                  foschia_km=[40.0, 30.0, 20.0, 30.0], swell=dict(hs_m=[1.0, 4.0], tp_s=[10.0, 16.0], da_gradi=[250.0, 320.0]),
                  temporali_giorni=[[1, 3], [0, 1], [0, 1], [1, 2]], brezza_m_s=[0.0, 0.0, 0.0, 0.0],
                  ghiaccio='nessuno in mare aperto'),
    'barents': dict(correzione_burrasche=2.0, incanalamento=1.0, fetch_km=[1000, 800, 600, 300, 200, 400, 800, 1000],
                    foschia_km=[30.0, 30.0, 20.0, 30.0], swell=dict(hs_m=[0.5, 2.0], tp_s=[8.0, 12.0], da_gradi=[270.0, 340.0]),
                    temporali_giorni=[[0, 1], [0, 0], [0, 1], [0, 1]], brezza_m_s=[0.0, 0.0, 0.0, 0.0],
                    ghiaccio='pack fuori dal riquadro; solo baie e fiordi (Kola, Teriberka)'),
    'gibilterra': dict(correzione_burrasche=3.0, incanalamento=1.5, fetch_km=[100, 300, 600, 300, 100, 300, 800, 300],
                       foschia_km=[25.0, 20.0, 12.0, 20.0], swell=dict(hs_m=[0.5, 2.5], tp_s=[8.0, 12.0], da_gradi=[260.0, 320.0]),
                       temporali_giorni=[[1, 3], [1, 3], [0, 1], [2, 4]], brezza_m_s=[0.5, 1.5, 2.5, 1.5],
                       ghiaccio='mai'),
    'otranto': dict(correzione_burrasche=2.0, incanalamento=1.2, fetch_km=[800, 400, 80, 300, 500, 150, 80, 400],
                    foschia_km=[25.0, 20.0, 15.0, 20.0], swell=dict(hs_m=[0.0, 1.0], tp_s=[6.0, 9.0], da_gradi=[150.0, 200.0]),
                    temporali_giorni=[[1, 2], [1, 2], [2, 4], [4, 7]], brezza_m_s=[0.5, 1.5, 2.5, 1.5],
                    ghiaccio='mai'),
    'kunashir': dict(correzione_burrasche=2.0, incanalamento=1.3, fetch_km=[800, 600, 300, 0, 0, 400, 300, 600],
                     foschia_km=[20.0, 15.0, 10.0, 25.0], swell=dict(hs_m=[0.5, 2.5], tp_s=[8.0, 13.0], da_gradi=[90.0, 180.0]),
                     temporali_giorni=[[0, 0], [0, 0], [0, 2], [0, 3]], brezza_m_s=[0.0, 0.5, 1.0, 0.5],
                     ghiaccio='pack sul lato Okhotsk da inizio febbraio a meta\' aprile, lingue attraverso i passi'),
    'celtica': dict(correzione_burrasche=1.5, incanalamento=1.0, fetch_km=[400, 600, 400, 300, 0, 0, 0, 0],
                    foschia_km=[35.0, 30.0, 20.0, 30.0], swell=dict(hs_m=[1.0, 4.0], tp_s=[10.0, 16.0], da_gradi=[240.0, 310.0]),
                    temporali_giorni=[[1, 3], [0, 2], [0, 2], [1, 3]], brezza_m_s=[0.0, 0.0, 0.0, 0.0],
                    ghiaccio='mai'),
    'baltico': dict(correzione_burrasche=2.0, incanalamento=1.3, fetch_km=[150, 200, 250, 150, 50, 100, 150, 120],
                    foschia_km=[20.0, 25.0, 20.0, 15.0], swell=dict(hs_m=[0.0, 0.5], tp_s=[5.0, 7.0], da_gradi=[240.0, 300.0]),
                    temporali_giorni=[[0, 1], [1, 2], [6, 12], [1, 3]], brezza_m_s=[0.0, 1.0, 2.5, 1.0],
                    ghiaccio='solo negli inverni rigidi (1985, 1986, 1987): banchi e ghiaccio costiero in febbraio-marzo',
                    inverni_rigidi=[1985, 1986, 1987]),
}
STIME_TEATRO = ['correzione_burrasche', 'incanalamento', 'fetch_km', 'foschia_km', 'swell', 'temporali_giorni', 'brezza_m_s']

# --- eventi rari (dossier 21 §3.3, nota A §5.9): tasso per mese nel riquadro, mesi, durata, firma (STIMA salvo citati) --
def E(mesi, tasso_al_mese, durata_h, fonte, conf, **altro):
    return dict(mesi=mesi, tasso_al_mese=tasso_al_mese, durata_h=durata_h, fonte=fonte, conf=conf,
                stime=['tasso_al_mese', 'durata_h'], **altro)


EVENTI = {
    'faroe': {
        'tempesta_bft10': E([12, 1, 2], 0.1, [4.0, 7.0], 'dossier 21 §3.3; nota A §5.1 (ERA5)', 'media', vento_m_s=[24.5, 31.0]),
        'polar_low': E([10, 11, 12, 1, 2, 3, 4], 0.3, [9.0, 25.0], 'dossier 21 §3.3; Smirnova et al. 2015', 'media',
                       vento_m_s=[20.0, 4.0], raggio_km=150.0, traslazione_m_s=8.0, esordio_h=[1.0, 3.0], prevista=False),
    },
    'barents': {
        'polar_low': E([10, 11, 12, 1, 2, 3, 4], 0.75, [9.0, 25.0], 'dossier 21 §3.3; Smirnova et al. 2015 (alta per la '
                       'climatologia, STIMA la ripartizione sul riquadro)', 'media', vento_m_s=[20.0, 4.0], raggio_km=150.0,
                       traslazione_m_s=8.0, esordio_h=[1.0, 3.0], prevista=False, mese_massimo=3),
        'fumo_di_mare_denso': E([12, 1, 2], 1.5, [6.0, 48.0], 'dossier 21 §3.3; nota A §2.2 (potenziale ERA5)', 'media',
                                aria_max_c=-10.0),
    },
    'gibilterra': {
        'cut_off': E([10, 11, 12, 1, 2, 3, 4], 0.15, [168.0, 240.0], 'nota A §2.3, §5.3 (atlante US Navy)', 'media'),
        'tromba_marina': E([3, 4, 10, 11], 0.05, [0.1, 0.3], 'nota A §5.5', 'bassa'),
    },
    'otranto': {
        'medicane': E([9, 10, 11, 12, 1], 0.02, [24.0, 48.0], 'dossier 21 §3.3; Romero e Emanuel 2013', 'media',
                      vento_m_s=[20.0, 30.0], hs_m=[4.0, 6.0]),
        'tromba_marina': E([8, 9, 10], 0.3, [0.1, 0.3], 'dossier 21 §3.3; Renko et al. 2016', 'media',
                           per_giorno_di_temporale=0.075),
    },
    'kunashir': {
        'nebbia_persistente': E([6, 7, 8], 0.5, [72.0, 216.0], 'dossier 21 §3.3; nota A §2.5 (ERA5, atlante US Navy)', 'media'),
        'ciclone_esplosivo': E([11, 12, 1, 2, 3], 0.4, [12.0, 36.0], 'nota A §5.6', 'bassa'),
        'spruzzo_gelante_forte': E([1, 2], 2.0, [6.0, 24.0], 'nota A §2.5 (Overland su ERA5)', 'media', aria_max_c=-10.0),
    },
    'celtica': {
        'tempesta_bft10': E([12, 1, 2], 0.15, [4.0, 7.0], 'dossier 21 §3.3; nota A §5.1', 'media', vento_m_s=[24.5, 30.0]),
    },
    'baltico': {
        'temporale_estivo': E([6, 7, 8], 3.0, [1.0, 4.0], 'nota A §2.7 (atlante US Navy: 2-4 giorni al mese)', 'media'),
    },
}

# --- climatologia ERA5: i campi del punto centrale da esportare per mese -----------------------------------------------
CAMPI_MESE = ['vento_media_m_s', 'vento_p10_m_s', 'vento_p50_m_s', 'vento_p90_m_s', 'weibull_k', 'weibull_c_m_s',
              'rosa_pct', 'bft7_pct', 'bft8_pct', 'raffica_fattore', 'hs_media_m', 'hs_p90_m', 'tp_media_s',
              'douglas_pct', 'ghiaccio_pct', 't_aria_c', 't_mare_c', 'nubi_pct', 'coperto_pct', 'sereno_pct',
              'nubi_basse_pct', 'precipitazione_pct', 'intensita_mm_h', 'precipitazione_forte_pct', 'neve_pct',
              'ur97_pct', 'fumo_di_mare_pct', 'spruzzo_moderato_pct', 'spruzzo_forte_pct', 'pressione_hpa']
CAMPI_STAGIONE = ['tau_vento_h', 'tau_onda_h', 'ritardo_onda_h', 'markov6h_pct', 'quote_classi_pct',
                  'burrasche_bft8_per_stagione', 'durata_bft8_h', 'episodi_bft7_per_stagione', 'durata_bft7_h',
                  'intervallo_minimi_giorni']


def esporta_meteo(radice):
    clima = json.load(open(os.path.join(radice, 'tools', 'meteo', 'climatologia_era5.json')))
    teatri_yaml = open(os.path.join(radice, 'tools', 'pipeline', 'teatri.yaml')).read()
    righe = ['# Meteo e mare dell\'Enhanced (generato da tools/pipeline/meteo.py: non modificare a mano).',
             '# Dati dal dossier docs/ricerca/21_meteo_e_mare.md e dalle note in docs/ricerca/21_note/; climatologia ERA5',
             '# 1981-1990 da tools/meteo/climatologia_era5.json. conf: alta, media, bassa; stime: i campi STIMA da tarare',
             '# (pezzo M6). Direzioni: da cui viene il vento o il mare. «nan»: dato assente (classe senza casi).', '']
    righe += _voce('generale', GENERALE)
    n_punti = 0
    for t, par in TEATRI.items():
        assert f'id: {t}\n' in teatri_yaml, t
        c = clima['teatri'][t]
        regimi, quote = REGIMI[t]
        for s, q in quote.items():
            assert abs(sum(q.values()) - 1.0) < 1e-6 and set(q) == set(regimi), (t, s)
        for nome, r in regimi.items():
            assert all(p in regimi for p in r['prossimi']), (t, nome)
        righe += _voce(f'teatri.{t}', {**par, 'centro': dict(lat=c['centro']['lat'], lon=c['centro']['lon']),
                                       'fonte': 'dossier 21 §3-4; nota A §2, §5.8, §6.5, §7.6', 'conf': 'media',
                                       'stime': STIME_TEATRO})
        for mo, m in enumerate(c['mesi'], 1):
            righe += _voce(f'teatri.{t}.mesi.{mo}', {**{k: m[k] for k in CAMPI_MESE}, 'fonte': ERA5, 'conf': 'media'})
        for s in STAGIONI:
            st = c['stagioni'][s]
            righe += _voce(f'teatri.{t}.stagioni.{s}', {**{k: st[k] for k in CAMPI_STAGIONE},
                                                        'diurno_ampiezza_m_s': c['diurno'][s]['ampiezza_m_s'],
                                                        'diurno_ora_massimo': c['diurno'][s]['ora_massimo'],
                                                        'regimi': quote[s], 'fonte': ERA5 + '; regimi: dossier 21 §4',
                                                        'conf': 'media', 'stime': ['regimi']})
        for nome, r in regimi.items():
            righe += _voce(f'teatri.{t}.regimi.{nome}', {**r, 'fonte': 'dossier 21 §4; nota A §5.4, §7.2', 'conf': 'bassa',
                                                         'stime': ['vento_fattore', 'da_gradi', 'nubi_pct', 'pioggia_ora',
                                                                   'intensita_mm_h', 'visibilita_km', 'anomalia_t_c',
                                                                   'durata_h']})
        for nome, e in EVENTI.get(t, {}).items():
            righe += _voce(f'teatri.{t}.eventi.{nome}', e)
        for p in c['punti']:
            righe += [f'[[teatri.{t}.punti]]', _campo('nome', p['nome']), _campo('lat', p['lat']), _campo('lon', p['lon'])]
            for k in ('vento_media_m_s', 'hs_media_m', 'ghiaccio_pct', 'ur97_pct', 't_aria_c'):
                righe.append(_campo(k, [m[k] for m in p['mesi']]))
            righe += [_campo('fonte', ERA5 + ', punto del riquadro'), _campo('conf', 'media'), '']
            n_punti += 1
    uscita = os.path.join(radice, 'data', 'motore', 'meteo.toml')
    open(uscita, 'w').write('\n'.join(righe))
    return len(TEATRI), n_punti


if __name__ == '__main__':
    print('teatri, punti ERA5:', esporta_meteo(sys.argv[1] if len(sys.argv) > 1 else '.'))
