"""Analisi a monte dei campioni della libreria (pezzo R4; dossier docs/ricerca/18_audio_reale.md §2.2 punto 1) e
conversione nel formato del gioco. Legge i grezzi scaricati da campioni.py (tools/campioni/grezzi/, dati non fidati:
si decodificano soltanto) e scrive:

- data/suoni/campioni/<id>.wav: la tessitura (sbianca(): spettro piatto, senza le righe stabili, rms 0,25), PCM a 16
  bit mono, al piu' 22.050 Hz, al piu' 60 s (il tratto piu' forte); il colore lo rimette il modello (nucleo/campioni.hpp);
- data/motore/campioni.toml: per ogni campione la voce del catalogo, il fattore d'accelerazione della fonte, il
  tempo vero, e quello che contiene, misurato in frequenze vere (divise per il fattore):
  - l'inviluppo medio in terzi d'ottava (dB per Hz rispetto al fondo scala: la forma, non il livello assoluto);
  - le righe stabili (presenti in almeno il 60% dei tratti di 4 s, 8 dB sopra il fondo locale);
  - la modulazione d'inviluppo del DEMON (frequenza e profondita');
  - il ritmo degli eventi (al minuto vero, intervallo mediano);
  - l'inviluppo nel tempo a 0,25 s in tre bande (per il mescolamento R5: cascata e cuffia seguono il campione);
  - il clipping, la licenza, lo stato e l'attribuzione;
- tools/campioni/crediti.json: le voci per i crediti del gioco (gruppo «suoni e musica», tools/pipeline/crediti.py).

Uso:  tools/venv/bin/python -I tools/pipeline/campioni_analisi.py <radice>
"""
import glob, json, os, sys
import numpy as np
from scipy import signal
from scipy.io import wavfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import campioni as M

FS_MAX = 22050
DURATA_MAX_S = 60.0
TRATTO_RIGHE_S = 4.0
PRESENZA_RIGHE = 0.6
PROMINENZA_DB = 8.0
PASSO_INVILUPPO_S = 0.25
BANDE_TEMPO = [(10.0, 300.0), (300.0, 2000.0), (2000.0, 10000.0)]   # Hz veri: basse, medie, alte


def leggi(percorso):
    """(campioni float mono in [-1, 1], frequenza di campionamento)"""
    testa = open(percorso, 'rb').read(4)   # dal contenuto: i nomi lunghi di Commons sono tagliati senza estensione
    if testa in (b'\x1aE\xdf\xa3', b'OggS'):   # WebM/Matroska e Ogg (R8): solo l'audio, con PyAV
        import av
        with av.open(percorso) as f:
            flusso = next(st for st in f.streams if st.type == 'audio')
            fs = flusso.rate
            pezzi = [fr.to_ndarray().astype(np.float64) for fr in f.decode(flusso)]
        x = np.concatenate([p.mean(axis=0) if p.ndim == 2 else p for p in pezzi])
        return np.clip(x, -1.0, 1.0), fs
    if percorso.lower().endswith('.mp3') or testa[:3] == b'ID3' or testa[:2] in (b'\xff\xfb', b'\xff\xf3'):
        import miniaudio
        d = miniaudio.decode_file(percorso, output_format=miniaudio.SampleFormat.SIGNED16, nchannels=1)
        return np.asarray(d.samples, dtype=np.float64) / 32768.0, d.sample_rate
    fs, x = wavfile.read(percorso)
    # prima la scala, poi la media dei canali: la media fa i float e la scala degli interi non si applicava piu' (il
    # primo WAV stereo, il boing di M, risultava saturo al 99%)
    if x.dtype == np.int16: x = x / 32768.0
    elif x.dtype == np.int32: x = x / 2147483648.0
    elif x.dtype == np.uint8: x = (x.astype(np.float64) - 128.0) / 128.0
    if x.ndim > 1: x = x.mean(axis=1)
    return np.asarray(x, dtype=np.float64), fs


DURATA_AEREI_S = 10.0


def aereo(voce):
    return voce.startswith(('aereo_', 'elicottero_', 'hover_'))


def tratto_del_motore(x, fs, durata):
    """R8: il tratto di `durata` dove il motore (20-400 Hz: eliche, rotori, getti) domina di piu' su voce e musica
    (500-4000 Hz), con un peso al livello del motore: i video hanno telecronache, voci, musica e folla"""
    if len(x) <= durata * fs: return x, 0.0
    basso = signal.sosfilt(signal.butter(4, [20.0, 400.0], btype='band', fs=fs, output='sos'), x)
    medio = signal.sosfilt(signal.butter(4, [500.0, min(4000.0, 0.45 * fs)], btype='band', fs=fs, output='sos'), x)
    q = int(0.5 * fs)
    eb = np.array([np.mean(basso[i:i + q] ** 2) for i in range(0, len(x) - q + 1, q)]) + 1e-20
    em = np.array([np.mean(medio[i:i + q] ** 2) for i in range(0, len(x) - q + 1, q)]) + 1e-20
    pezzi = int(durata / 0.5)
    punteggi = [np.mean(10 * np.log10(eb[k:k + pezzi] / em[k:k + pezzi])) + 0.3 * np.mean(10 * np.log10(eb[k:k + pezzi]))
                for k in range(0, len(eb) - pezzi + 1)]
    k = int(np.argmax(punteggi))
    return x[k * q:k * q + int(durata * fs)], k * 0.5


def tratto_piu_forte_di(misura, x, fs, durata):
    if len(x) <= durata * fs: return x, 0.0
    passo, n = int(fs), int(durata * fs)
    cumulata = np.concatenate([[0.0], np.cumsum(misura * misura)])
    inizi = np.arange(0, len(x) - n + 1, passo)
    i = int(inizi[np.argmax(cumulata[inizi + n] - cumulata[inizi])])
    return x[i:i + n], i / fs


def spettrogramma(x, fs, percorso):
    """Un'immagine per il controllo a occhio (0-5 kHz, scala logaritmica): musica, voci e spari si riconoscono"""
    from PIL import Image
    f, t, S = signal.spectrogram(x, fs, nperseg=1024, noverlap=512)
    S = 10 * np.log10(S[f <= 5000] + 1e-20)
    S = np.clip((S - S.max() + 70) / 70, 0, 1)
    Image.fromarray(np.uint8(255 * S[::-1])).resize((min(1200, S.shape[1]), 256)).save(percorso)


LICENZE = {   # codice del manifesto -> (testo, stato, attribuzione obbligatoria)
    'PD-§105': ('pubblico dominio (17 U.S.C. §105)', 'ok', False),
    'PD-§105?': ('pubblico dominio (17 U.S.C. §105) da verificare', 'da_verificare', False),
    'PD-§105 presunto': ('pubblico dominio presunto (sito federale NOAA senza credito per il clip, 17 U.S.C. §105)', 'ok', False),
    'CC0': ('CC0', 'ok', False),
    'PD-autore': ('pubblico dominio (rilasciato dall\'autore)', 'ok', False),
    'CC BY 3.0': ('CC BY 3.0', 'attribuzione', True),
    'CC BY 4.0': ('CC BY 4.0', 'attribuzione', True),
    'CC BY 4.0?': ('CC BY 4.0 da verificare', 'da_verificare', True),
}


def tratto_piu_forte(x, fs, durata):
    if len(x) <= durata * fs: return x, 0.0
    passo, n = int(fs), int(durata * fs)
    cumulata = np.concatenate([[0.0], np.cumsum(x * x)])
    inizi = np.arange(0, len(x) - n + 1, passo)   # al secondo
    i = int(inizi[np.argmax(cumulata[inizi + n] - cumulata[inizi])])
    return x[i:i + n], i / fs


def terzi(f_max):
    centri = 10.0 * 2.0 ** (np.arange(0, 60) / 3.0)
    return centri[centri * 2 ** (1 / 6) <= f_max]


def inviluppo_terzi(x, fs, fattore):
    f, p = signal.welch(x, fs, nperseg=min(len(x), 8192))
    out = []
    for c in terzi(fs / 2 / fattore):
        lo, hi = c * 2 ** (-1 / 6) * fattore, c * 2 ** (1 / 6) * fattore
        m = (f >= lo) & (f < hi)
        if m.sum() == 0: continue
        out.append([round(float(c), 2), round(float(10 * np.log10(np.mean(p[m]) * fattore + 1e-30)), 1)])
    return out


def righe_stabili(x, fs, fattore):
    n = int(TRATTO_RIGHE_S * fs)
    if len(x) < 2 * n: n = len(x) // 2
    tratti = [x[i:i + n] for i in range(0, len(x) - n + 1, n)]
    if len(tratti) < 2: return []
    df = fs / n
    presenze, forza = None, None
    for t in tratti:
        f, p = signal.periodogram(t * np.hanning(len(t)), fs)
        db = 10 * np.log10(p + 1e-30)
        sopra = db - signal.medfilt(db, 31)
        picchi, _ = signal.find_peaks(sopra, height=PROMINENZA_DB, distance=3)
        picchi = picchi[f[picchi] > 5.0 * fattore]
        segno = np.zeros(len(f), dtype=bool)
        valore = np.zeros(len(f))
        for d in (-1, 0, 1):   # +-1 bin: la riga che oscilla di poco conta lo stesso
            k = np.clip(picchi + d, 0, len(f) - 1)
            segno[k] = True
            valore[k] = np.maximum(valore[k], sopra[picchi])
        presenze = segno.astype(int) if presenze is None else presenze + segno
        forza = valore if forza is None else forza + valore
    stabili = np.nonzero(presenze >= PRESENZA_RIGHE * len(tratti))[0]
    righe = sorted(((k * df / fattore, forza[k] / presenze[k]) for k in stabili), key=lambda r: -r[1])
    unite = []   # bin vicini: una riga sola
    for hz, snr in righe:
        if all(abs(hz - u[0]) > 2.5 * df / fattore for u in unite): unite.append((hz, snr))
    return [[round(float(hz), 2), round(float(snr), 1)] for hz, snr in sorted(unite[:12])]


def demon(x, fs, fattore):
    lo, hi = (1000.0, 5000.0) if fs >= 12000 else (200.0, 0.4 * fs)
    sos = signal.butter(4, [lo, hi], btype='band', fs=fs, output='sos')
    e = np.abs(signal.hilbert(signal.sosfilt(sos, x)))
    q = max(1, int(fs // 200))
    e = signal.decimate(e, q, ftype='fir') if q > 1 else e
    fe = fs / q
    f, p = signal.welch(e - e.mean(), fe, nperseg=min(len(e), int(fe * 16)))
    m = (f >= 0.5) & (f <= 50.0)
    if m.sum() < 5: return None, None
    k = int(np.argmax(p[m]))
    profondita = float(10 * np.log10(p[m][k] / np.median(p[m])))
    return round(float(f[m][k] / fattore), 3), round(profondita, 1)


def ritmo(x, fs, fattore):
    passo = int(0.05 * fs)
    livelli = np.array([10 * np.log10(np.mean(x[i:i + passo] ** 2) + 1e-20) for i in range(0, len(x) - passo, passo)])
    if len(livelli) < 10: return 0.0, None
    fondo = signal.medfilt(livelli, 41)
    picchi, _ = signal.find_peaks(livelli - fondo, height=8.0, distance=4)
    durata_vera = len(x) / fs * fattore
    eventi = len(picchi) * 60.0 / durata_vera
    intervallo = float(np.median(np.diff(picchi)) * 0.05 * fattore) if len(picchi) > 2 else None
    return round(eventi, 2), (round(intervallo, 3) if intervallo else None)


def inviluppo_tempo(x, fs, fattore):
    passo = int(PASSO_INVILUPPO_S * fs)
    out = []
    for lo, hi in BANDE_TEMPO:
        lo_f, hi_f = lo * fattore, min(hi * fattore, 0.45 * fs)
        if lo_f >= hi_f: out.append([]); continue
        sos = signal.butter(4, [lo_f, hi_f], btype='band', fs=fs, output='sos')
        y = signal.sosfilt(sos, x)
        out.append([round(float(10 * np.log10(np.mean(y[i:i + passo] ** 2) + 1e-20)), 1) for i in range(0, len(y) - passo + 1, passo)])
    return out


def sbianca(x, fs, fattore, righe):
    """La tessitura del campione (dossier 18 §2.2 punti 2 e 4): lo spettro diviso per il suo inviluppo levigato in terzi
    d'ottava, le righe stabili tolte (il modello mette le sue), niente fuori dalla banda valida (dove il campione ha
    almeno -40 dB del massimo dell'inviluppo; gli MP3 tagliano gli acuti). Restituisce la tessitura con rms 0,25 e la
    banda valida in Hz veri."""
    n = len(x)
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(n, 1.0 / fs)
    p = np.abs(X) ** 2
    # levigatura robusta in log f: la mediana su un terzo d'ottava, su una griglia di 12 punti per ottava, interpolata.
    # La mediana non si fa gonfiare dalle righe (le armoniche delle eliche): con la media fra una riga e l'altra lo
    # spettro sbiancato restava basso e il colore del modello non tornava
    utile = (f > 5.0 * fattore) & (f < 0.45 * fs)
    nodi = 2.0 ** np.arange(np.log2(5.0 * fattore), np.log2(0.45 * fs), 1 / 12)
    mediane = []
    for c in nodi:
        m = (f >= c * 2 ** (-1 / 6)) & (f < c * 2 ** (1 / 6))
        mediane.append(np.median(p[m]) / np.log(2.0) if m.sum() >= 3 else np.nan)   # mediana -> media dell'esponenziale
    mediane = np.array(mediane)
    buoni = ~np.isnan(mediane)
    liscio = np.exp(np.interp(np.log(np.maximum(f, 1e-3)), np.log(nodi[buoni]), np.log(np.maximum(mediane[buoni], 1e-30))))
    massimo = np.max(liscio[utile]) if utile.any() else 1.0
    valida = utile & (liscio > massimo * 1e-4)
    Y = np.where(valida, X / np.sqrt(np.maximum(liscio, massimo * 1e-4)), 0.0)
    # ogni bin oltre 10 dB sopra la mediana (le righe che restano, anche oltre le 12 stabili) si riporta a quel livello
    modulo = np.abs(Y)
    Y = np.where(modulo > np.sqrt(10.0), Y / np.maximum(modulo, 1e-30) * np.sqrt(10.0), Y)
    # secondo passaggio: il taglio lascia una pendenza residua (~1 dB); la media in terzi d'ottava della tessitura la toglie
    q = np.abs(Y) ** 2
    medie = []
    for c in nodi:
        m = valida & (f >= c * 2 ** (-1 / 6)) & (f < c * 2 ** (1 / 6))
        medie.append(np.mean(q[m]) if m.sum() >= 3 else np.nan)
    medie = np.array(medie)
    buoni = ~np.isnan(medie)
    if buoni.sum() >= 2:
        residuo = np.exp(np.interp(np.log(np.maximum(f, 1e-3)), np.log(nodi[buoni]), np.log(np.maximum(medie[buoni], 1e-30))))
        Y = np.where(valida, Y / np.sqrt(residuo), 0.0)
    # le righe stabili: modulo ricondotto a quello dei vicini (la fase resta)
    df = fs / n
    for hz, _ in righe:
        k = int(round(hz * fattore / df))
        lar = max(3, int(round(0.3 / df * fattore)))   # +-0,3 Hz veri, almeno 3 bin
        a0, a1 = max(k - lar, 1), min(k + lar + 1, len(Y))
        vicini = np.abs(np.concatenate([Y[max(a0 - 4 * lar, 1):a0], Y[a1:a1 + 4 * lar]]))
        livello = np.median(vicini) if len(vicini) else 0.0
        modulo = np.abs(Y[a0:a1])
        Y[a0:a1] = np.where(modulo > livello, Y[a0:a1] / np.maximum(modulo, 1e-30) * livello, Y[a0:a1])
    y = np.fft.irfft(Y, n)
    y *= 0.25 / max(float(np.sqrt(np.mean(y * y))), 1e-12)
    fv = f[valida]
    return y, (float(fv.min() / fattore) if len(fv) else 0.0, float(fv.max() / fattore) if len(fv) else 0.0)


def toml_valore(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if isinstance(v, str): return json.dumps(v, ensure_ascii=False)
    if isinstance(v, (list, tuple)): return '[' + ', '.join(toml_valore(a) for a in v) + ']'
    if v is None: return '""'
    return repr(v)


def analizza(radice):
    _, grezzi, manifesto = M.cartelle(radice)
    registro = json.load(open(manifesto))
    uscita = os.path.join(radice, 'data', 'suoni', 'campioni')
    os.makedirs(uscita, exist_ok=True)
    per_voce = {}
    voci, crediti = [], []
    esclusi = {c['url']: c['escluso'] for c in M.MANIFESTO + M.MANIFESTO_R8 + M.MANIFESTO_M if c.get('escluso')}
    for f in glob.glob(os.path.join(uscita, '*.wav')) + glob.glob(os.path.join(radice, 'tools', 'campioni', 'spettri', '*.png')):
        os.remove(f)   # si rifa' tutto: niente resti di campioni esclusi o rinominati
    for nome, c in sorted(registro.items(), key=lambda kv: kv[1]['n']):
        if c['url'] in esclusi:
            print(f"{c['voce']:24s} escluso: {esclusi[c['url']]}")
            continue
        x, fs = leggi(os.path.join(grezzi, nome))
        clip = float(np.mean(np.abs(x) >= 0.999))
        x, inizio = tratto_del_motore(x, fs, DURATA_AEREI_S) if aereo(c['voce']) else tratto_piu_forte(x, fs, DURATA_MAX_S)
        if fs > FS_MAX:
            g = np.gcd(int(fs), FS_MAX)
            x = signal.resample_poly(x, FS_MAX // g, int(fs) // g)
            fs = FS_MAX
        fattore = c['fattore']
        k = per_voce[c['voce']] = per_voce.get(c['voce'], 0) + 1
        ident = c['voce'] if k == 1 else f"{c['voce']}_{k}"
        picco = float(np.max(np.abs(x))) or 1.0
        righe = righe_stabili(x, fs, fattore)
        tessitura, (f_min, f_max) = sbianca(x, fs, fattore, righe)
        wavfile.write(os.path.join(uscita, ident + '.wav'), int(fs), np.int16(np.clip(tessitura, -1, 1) * 32767))
        dm_hz, dm_db = demon(x, fs, fattore)
        eventi, intervallo = ritmo(x, fs, fattore)
        testo_licenza, stato, obbligo = LICENZE[c['licenza']]
        if aereo(c['voce']):
            os.makedirs(os.path.join(radice, 'tools', 'campioni', 'spettri'), exist_ok=True)
            spettrogramma(x, fs, os.path.join(radice, 'tools', 'campioni', 'spettri', ident + '.png'))
        v = dict(id=ident, voce=c['voce'], file=f'suoni/campioni/{ident}.wav', campionamento_hz=int(fs),
                 fattore=float(fattore), durata_s=round(len(x) / fs, 2), durata_vera_s=round(len(x) / fs * fattore, 1),
                 inizio_nel_grezzo_s=round(inizio, 1), normalizzato_db=round(float(20 * np.log10(0.89 / picco)), 1),
                 clipping=round(clip, 5), inviluppo=inviluppo_terzi(x, fs, fattore), righe=righe,
                 demon_hz=dm_hz or 0.0, demon_db=dm_db or 0.0, eventi_al_minuto=eventi, intervallo_s=intervallo or 0.0,
                 inviluppo_tempo=inviluppo_tempo(x, fs, fattore), passo_inviluppo_s=PASSO_INVILUPPO_S,
                 tessitura_rms=0.25, f_min_valida_hz=round(f_min, 2), f_max_valida_hz=round(f_max, 1),
                 bande_tempo_hz=[list(b) for b in BANDE_TEMPO],
                 licenza=testo_licenza, stato=stato, attribuzione=c['attribuzione'], url=c['url'],
                 sha256=c['sha256'], note=c['note'])
        voci.append(v)
        crediti.append(dict(gruppo='suoni e musica', titolo=f"Campione «{ident}» ({c['voce']})", file=f"tools/campioni/grezzi/{nome}",
                            url=c['url'], autore=c['attribuzione'], licenza=v['licenza'], attribuzione=obbligo, condivisione=False,
                            uso='tessitura della libreria di campioni (R4/R5), sbiancata e ricolorata dal modello',
                            derivati=[f"data/suoni/campioni/{ident}.wav"], stato=v['stato'],
                            nota=(c['note'] + '; ' if c['note'] else '') + ('opera federale USA; si cita la fonte come chiede'
                                  if c['licenza'].startswith('PD-§105') else 'credito come chiede la licenza')))
        print(f"{ident:24s} {fs:6d} Hz x{fattore:<4g} {v['durata_s']:5.1f} s  righe {len(righe):2d}  demon {dm_hz or 0:6.2f} Hz {dm_db or 0:5.1f} dB  "
              f"eventi/min {eventi:6.1f}  clip {clip:.4f}", flush=True)
    righe_toml = ['# Libreria di campioni registrati (R4): generato da tools/pipeline/campioni_analisi.py, non modificare a mano.',
                  '# Fonti e licenze in docs/ricerca/18_note/campioni_r4.md e campioni_r8.md; frequenze vere (divise per il fattore).', '']
    for v in voci:
        righe_toml.append(f"[campioni.{v['id']}]")
        righe_toml += [f'{k} = {toml_valore(x)}' for k, x in v.items() if k != 'id']
        righe_toml.append('')
    open(os.path.join(radice, 'data', 'motore', 'campioni.toml'), 'w').write('\n'.join(righe_toml))
    json.dump(crediti, open(os.path.join(radice, 'tools', 'campioni', 'crediti.json'), 'w'), ensure_ascii=False, indent=1)
    print(len(voci), 'campioni;', sum(os.path.getsize(os.path.join(uscita, v['id'] + '.wav')) for v in voci), 'byte in data/suoni/campioni')


if __name__ == '__main__':
    analizza(sys.argv[1] if len(sys.argv) > 1 else '.')
