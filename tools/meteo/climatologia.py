"""Climatologia ERA5 1981-1990 dei teatri per il meteo dell'Enhanced (docs/15_meteo_e_mare.md, M0; dossier 21 §3-4).

Legge extracted/meteo/era5/<punto>_atm.json e _mar.json (scarica_era5.py) e scrive tools/meteo/climatologia_era5.json,
che tools/pipeline/meteo.py trasforma in data/motore/meteo.toml. Stessi metodi della nota A (docs/ricerca/21_note/
a_climatologia.md, script in 21_note/era5/), rifatti con numpy e estesi ai punti in piu' di ogni teatro.

- Punto centrale (tutte le variabili): per mese vento (media, quantili, Weibull dai momenti, rosa a 8 settori, Bft 7 e
  8), raffica, onde (Hs, Tp, classi Douglas), ghiaccio (ore senza onda), temperature, nubi, precipitazione e neve,
  indici di nebbia e fumo di mare, spruzzo gelante (Overland), pressione; per stagione persistenze (tau dall'
  autocorrelazione a 24 h), ritardo vento-onda, Markov a 6 h delle classi di vento, burrasche, minimi barici, ciclo
  diurno del vento, Hs mediana per classe di vento.
- Punti in piu': per mese vento, Hs, ghiaccio, nebbia e temperatura, da interpolare nel riquadro.
Uso: climatologia.py <radice del progetto>"""
import json, math, os, sys
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scarica_era5 import PUNTI   # noqa: E402

STAGIONI = {'inverno': (12, 1, 2), 'primavera': (3, 4, 5), 'estate': (6, 7, 8), 'autunno': (9, 10, 11)}
DOUGLAS = [0.0001, 0.1, 0.5, 1.25, 2.5, 4, 6, 9, 14]
TF = -1.7   # congelamento dell'acqua di mare (Overland)


def arr(d, k):
    v = d.get(k)
    return None if v is None else np.array([np.nan if x is None else x for x in v], float)


def r1(x, n=2):
    x = float(x)
    return None if math.isnan(x) else round(x, n)


def weibull(u):
    u = u[~np.isnan(u)]
    m, s = u.mean(), u.std()
    k = (s / m) ** -1.086
    return k, m / math.gamma(1 + 1 / k)


def acf(x, mesi, lag, medie):
    a = x - medie[mesi - 1]
    v0, v1 = a[:-lag], a[lag:]
    ok = ~np.isnan(v0) & ~np.isnan(v1)
    return float(np.corrcoef(v0[ok], v1[ok])[0, 1]) if ok.sum() > 100 else float('nan')


def episodi(x, sel, soglia):
    sopra = (x >= soglia) & sel
    durate, n = [], 0
    for s in sopra:
        if s: n += 1
        elif n:
            durate.append(n); n = 0
    if n: durate.append(n)
    durate = [d for d in durate if d >= 3]
    return len(durate) / 10.0, (float(np.median(durate)) if durate else 0.0)


def carica(cartella, nome):
    a = json.load(open(os.path.join(cartella, f'{nome}_atm.json')))
    m = json.load(open(os.path.join(cartella, f'{nome}_mar.json')))
    return a, m


def centro(a, m):
    h = a['hourly']
    mesi = np.array([int(t[5:7]) for t in h['time']])
    ore = np.array([int(t[11:13]) for t in h['time']])
    U, D, G = arr(h, 'wind_speed_10m'), arr(h, 'wind_direction_10m'), arr(h, 'wind_gusts_10m')
    Ta, RH, P = arr(h, 'temperature_2m'), arr(h, 'relative_humidity_2m'), arr(h, 'pressure_msl')
    CC, CL, PR, SN = arr(h, 'cloud_cover'), arr(h, 'cloud_cover_low'), arr(h, 'precipitation'), arr(h, 'snowfall')
    SST = arr(h, 'soil_temperature_0_to_7cm')
    H, TP = arr(m['hourly'], 'wave_height'), arr(m['hourly'], 'wave_period')
    n = min(len(U), len(H))
    U, D, G, Ta, RH, P, CC, CL, PR, SN, SST, H, TP, mesi, ore = (x[:n] for x in (U, D, G, Ta, RH, P, CC, CL, PR, SN, SST, H, TP, mesi, ore))
    SSTg = np.maximum(SST, TF)
    ppr = np.where(Ta < TF, U * (TF - Ta) / (1 + 0.3 * (SSTg - TF)), 0.0)
    out = {'mesi': [], 'stagioni': {}}
    medie_u, medie_h = np.zeros(12), np.zeros(12)
    for mo in range(1, 13):
        s = mesi == mo
        u, hs, d = U[s], H[s], D[s]
        k, c = weibull(u)
        medie_u[mo - 1] = np.nanmean(u)
        medie_h[mo - 1] = np.nanmean(hs) if np.isfinite(hs).any() else 0.0
        rosa = [100.0 * np.mean(((d[(u >= 0.5)] + 22.5) % 360) // 45 == j) for j in range(8)]
        hv = hs[~np.isnan(hs)]
        douglas = [100.0 * np.mean(np.digitize(hv, DOUGLAS) == j) if len(hv) else 0.0 for j in range(10)]
        pioggia = PR[s] >= 0.1
        out['mesi'].append(dict(
            vento_media_m_s=r1(np.nanmean(u)), vento_p10_m_s=r1(np.nanpercentile(u, 10)),
            vento_p50_m_s=r1(np.nanpercentile(u, 50)), vento_p90_m_s=r1(np.nanpercentile(u, 90)),
            weibull_k=r1(k, 3), weibull_c_m_s=r1(c), rosa_pct=[r1(x, 1) for x in rosa],
            bft7_pct=r1(100 * np.nanmean(u >= 13.9), 2), bft8_pct=r1(100 * np.nanmean(u >= 17.2), 2),
            raffica_fattore=r1(np.nanmean(G[s]) / np.nanmean(u), 3),
            hs_media_m=r1(np.mean(hv) if len(hv) else 0.0), hs_p90_m=r1(np.percentile(hv, 90) if len(hv) else 0.0),
            tp_media_s=r1(np.nanmean(TP[s]) if np.isfinite(TP[s]).any() else 0.0), douglas_pct=[r1(x, 1) for x in douglas],
            ghiaccio_pct=r1(100 * np.mean(np.isnan(hs)), 1),
            t_aria_c=r1(np.nanmean(Ta[s]), 1), t_mare_c=r1(np.nanmean(SSTg[s]), 1),
            nubi_pct=r1(np.nanmean(CC[s]), 1), coperto_pct=r1(100 * np.nanmean(CC[s] >= 75), 1),
            sereno_pct=r1(100 * np.nanmean(CC[s] <= 25), 1), nubi_basse_pct=r1(np.nanmean(CL[s]), 1),
            precipitazione_pct=r1(100 * np.nanmean(pioggia), 1),
            intensita_mm_h=r1(np.nanmean(PR[s][pioggia]) if pioggia.any() else 0.0, 2),
            precipitazione_forte_pct=r1(100 * np.nanmean(PR[s] >= 2.0), 2),
            neve_pct=r1(100 * np.nanmean(pioggia & (SN[s] > 0.05)), 1),
            ur97_pct=r1(100 * np.nanmean(RH[s] >= 97), 1),
            fumo_di_mare_pct=r1(100 * np.nanmean((SSTg[s] - Ta[s] >= 9) & (Ta[s] <= 0)), 1),
            spruzzo_moderato_pct=r1(100 * np.nanmean(ppr[s] >= 22.4), 2),
            spruzzo_forte_pct=r1(100 * np.nanmean(ppr[s] >= 53.3), 2),
            pressione_hpa=r1(np.nanmean(P[s]), 1)))
    # pressione smussata 24 h per i minimi barici (nota A §6.1)
    Pf = np.convolve(np.nan_to_num(P, nan=np.nanmean(P)), np.ones(25) / 25, mode='same')
    for nome, ms in STAGIONI.items():
        sel = np.isin(mesi, ms)
        lag24 = acf(np.where(sel, U, np.nan), mesi, 24, medie_u)
        lag24h = acf(np.where(sel, H, np.nan), mesi, 24, medie_h)
        tau = -24.0 / math.log(min(max(lag24, 0.05), 0.95))
        tau_h = -24.0 / math.log(min(max(lag24h, 0.05), 0.95)) if not math.isnan(lag24h) else tau
        # ritardo vento -> onda: massimo della correlazione incrociata
        migliore = (0, -2.0)
        for L in range(0, 25, 2):
            a, b = U[:len(U) - L][sel[:len(U) - L]], H[L:][sel[:len(U) - L]]
            ok = ~np.isnan(a) & ~np.isnan(b)
            if ok.sum() > 100:
                cc = float(np.corrcoef(a[ok], b[ok])[0, 1])
                if cc > migliore[1]: migliore = (L, cc)
        # Markov a 6 h fra le classi calmo < 5,5, moderato < 10,8, forte < 17,2, burrasca
        cl = np.digitize(U, [5.5, 10.8, 17.2])
        mk = np.zeros((4, 4))
        for i in range(0, len(U) - 6, 6):
            if sel[i] and not np.isnan(U[i]) and not np.isnan(U[i + 6]): mk[cl[i], cl[i + 6]] += 1
        quote = mk.sum(axis=1)
        markov = [[r1(100 * mk[a, b] / quote[a], 1) if quote[a] else None for b in range(4)] for a in range(4)]
        n8, d8 = episodi(U, sel, 17.2)
        n7, d7 = episodi(U, sel, 13.9)
        # minimi barici: minimo locale di +-36 h almeno 3 hPa sotto la media su 5 giorni
        minimi = 0
        for i in range(60, len(Pf) - 60, 1):
            if sel[i] and Pf[i] == Pf[i - 36:i + 37].min() and Pf[i - 60:i + 61].mean() - Pf[i] >= 3: minimi += 1
        giorni = 30.4 * len(ms)
        out['stagioni'][nome] = dict(
            tau_vento_h=r1(tau, 1), tau_onda_h=r1(tau_h, 1), ritardo_onda_h=migliore[0],
            markov6h_pct=markov, quote_classi_pct=[r1(100 * q / quote.sum(), 1) for q in quote],
            burrasche_bft8_per_stagione=r1(n8, 1), durata_bft8_h=r1(d8, 1),
            episodi_bft7_per_stagione=r1(n7, 1), durata_bft7_h=r1(d7, 1),
            intervallo_minimi_giorni=r1(giorni / (minimi / 10.0), 1) if minimi else None,
            hs_per_vento_m={str(lo): r1(np.nanmedian(H[sel & (U >= lo) & (U < lo + 5)]) if (sel & (U >= lo) & (U < lo + 5) & ~np.isnan(H)).sum() > 20 else float('nan'))
                            for lo in (5, 10, 15, 20)})
    return out, ore


def diurno(a, ore, mesi_ok=STAGIONI):
    h = a['hourly']
    U = arr(h, 'wind_speed_10m')
    mesi = np.array([int(t[5:7]) for t in h['time']])
    loc = ((ore + a['longitude'] / 15.0) % 24).astype(int)
    out = {}
    for nome, ms in mesi_ok.items():
        sel = np.isin(mesi[:len(ore)], ms)
        medie = [np.nanmean(U[:len(ore)][sel & (loc == k)]) for k in range(24)]
        out[nome] = dict(ampiezza_m_s=r1(max(medie) - min(medie)), ora_massimo=int(np.argmax(medie)))
    return out


def punto(a, m):
    h = a['hourly']
    mesi = np.array([int(t[5:7]) for t in h['time']])
    U, Ta, RH = arr(h, 'wind_speed_10m'), arr(h, 'temperature_2m'), arr(h, 'relative_humidity_2m')
    H = arr(m['hourly'], 'wave_height')
    n = min(len(U), len(H))
    out = []
    for mo in range(1, 13):
        s = mesi[:n] == mo
        hs = H[:n][s]
        out.append(dict(vento_media_m_s=r1(np.nanmean(U[:n][s])), hs_media_m=r1(np.nanmean(hs) if np.isfinite(hs).any() else 0.0),
                        ghiaccio_pct=r1(100 * np.mean(np.isnan(hs)), 1), ur97_pct=r1(100 * np.nanmean(RH[:n][s] >= 97), 1),
                        t_aria_c=r1(np.nanmean(Ta[:n][s]), 1)))
    return out


def climatologia(radice):
    cartella = os.path.join(radice, 'extracted', 'meteo', 'era5')
    risultato = {'fonte': 'ERA5 (ECMWF/Copernicus, Hersbach et al. 2020) via Open-Meteo, orario 1981-1990',
                 'metodo': 'tools/meteo/climatologia.py (nota A del dossier 21)', 'teatri': {}}
    for teatro, punti in PUNTI.items():
        nome, lat, lon = punti[0]
        a, m = carica(cartella, nome)
        c, ore = centro(a, m)
        c['diurno'] = diurno(a, ore)
        c['centro'] = dict(nome=nome, lat=lat, lon=lon)
        c['punti'] = []
        for nome, lat, lon in punti:
            f = os.path.join(cartella, f'{nome}_atm.json')
            if not os.path.exists(f) or not os.path.exists(f.replace('_atm', '_mar')):
                print(teatro, nome, 'non scaricato: saltato', file=sys.stderr)
                continue
            a, m = carica(cartella, nome)
            mesi = punto(a, m)
            if all(x['ghiaccio_pct'] >= 99.0 for x in mesi):   # terra per il modello d'onda: scartato
                print(teatro, nome, 'senza onde in nessun mese (terra): scartato', file=sys.stderr)
                continue
            c['punti'].append(dict(nome=nome, lat=lat, lon=lon, mesi=mesi))
        risultato['teatri'][teatro] = c
        print(teatro, 'punti', len(c['punti']), file=sys.stderr)
    uscita = os.path.join(radice, 'tools', 'meteo', 'climatologia_era5.json')
    json.dump(risultato, open(uscita, 'w'), ensure_ascii=False, separators=(',', ':'))
    return uscita


if __name__ == '__main__':
    print(climatologia(sys.argv[1] if len(sys.argv) > 1 else '.'))
