"""Effetti di bordo registrati (roadmap 38, campioni veri; elenco approvato dall'utente il 31/10/2026, nota condivisa
disassemblatore-audio-campioni-da-approvare.md) -> data/suoni/effetti/<id>.wav e data/motore/effetti.toml.
Allarmi veri di sottomarini e navi della US Navy, sala radio e un sonar: solo pubblico dominio federale (17 U.S.C.
§105), CC0 e CC BY; un CC BY-SA (i segnali sonar) resta file a se'. Fonte, autore e licenza di ogni file nel manifesto
qui sotto e in effetti.toml (i crediti li legge anche docs/CREDITI.md).

I grezzi stanno in tools/campioni/effetti_grezzi/<cartella>/ (dati non fidati: si decodificano soltanto, con PyAV; da
Freesound l'anteprima pubblica HQ in Ogg, la licenza e' la stessa del file). Qui si portano a PCM 16 bit mono a
22.050 Hz (l'audio d'epoca: il Mixer li ricampiona), si taglia il tratto utile, 10 ms di rampa ai capi e il picco a
-1 dBFS. Nessun altro trattamento: il carattere del campione resta il suo.
Uso: effetti_veri.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

FS = 22050
RAMPA_S = 0.01
PICCO = 10 ** (-1 / 20)

COMMONS = 'https://commons.wikimedia.org/wiki/File:'
FREESOUND = 'https://freesound.org/people/'
PI_INTERCEPTOR = ('raccolta di policeinterceptor.com (Authentic Navy Alarm Sounds); il marchio PD-USNavy l\'ha messo chi '
                  'l\'ha caricato su Commons: provenienza buona, non provata')


def E(ident, voce, cartella, file, pagina, autore, licenza, da_s=0.0, durata_s=None, note=''):
    return dict(id=ident, voce=voce, cartella=cartella, file=file, pagina=pagina, autore=autore, licenza=licenza,
                da_s=da_s, durata_s=durata_s, note=note)


MANIFESTO = [
    # --- allarmi di bordo ---
    E('allarme_immersione', 'allarme d\'immersione elettronico di un sottomarino (stack AN/WIC)',
      'commons_Alarm_-_Diving_-_Cow_Fart', 'Alarm_-_Diving_-_Cow_Fart.ogg', COMMONS + 'Alarm_-_Diving_-_Cow_Fart.ogg',
      'U.S. Navy', 'PD-USNavy', note=PI_INTERCEPTOR),
    E('allarme_immersione_federal', 'allarme d\'immersione della USS Woodrow Wilson (Federal Sign and Signal)',
      'freesound_mkjunker_sounds_156672', 'mkjunker_sounds_156672-hq.ogg', FREESOUND + 'mkjunker/sounds/156672/',
      'mkjunker (freesound.org)', 'CC0'),
    E('tromba_immersione_h8', 'tromba d\'immersione meccanica H8 (il klaxon)', 'commons_Alarm_-_Diving_-_H8',
      'Alarm_-_Diving_-_H8.ogg', COMMONS + 'Alarm_-_Diving_-_H8.ogg', 'U.S. Navy', 'PD-USNavy', note=PI_INTERCEPTOR),
    E('allarme_collisione', 'allarme di collisione di un sottomarino (SSN-625, stack AN/WIC)', 'commons_Alarm_-_Collision',
      'Alarm_-_Collision.ogg', COMMONS + 'Alarm_-_Collision.ogg', 'U.S. Navy', 'PD-USNavy', note=PI_INTERCEPTOR),
    E('allarme_collisione_nave', 'allarme di collisione di una nave (AN/SIA-114B, USS San Diego AFS-6)',
      'commons_Surface_ship_-_Collision_Alarm_-_USS_San_Diego_AFS-6_-_AN-SIA-114B',
      'Surface_ship_-_Collision_Alarm_-_USS_San_Diego_AFS-6_-_AN-SIA-114B.ogg',
      COMMONS + 'Surface_ship_-_Collision_Alarm_-_USS_San_Diego_(AFS-6)_-_AN-SIA-114B.ogg', 'U.S. Navy', 'PD-USNavy',
      note=PI_INTERCEPTOR),
    E('posti_combattimento', 'general quarters a bordo di una nave USN (gong e voce)', 'commons_General_quarters',
      'General_quarters.ogg', COMMONS + 'General_quarters.ogg', 'United States Navy (defenselink.mil)', 'PD-USNavy'),
    E('posti_combattimento_gong', 'il segnale del general alarm sul 1MC di una nave USN, senza voce',
      'freesound_lancelindley_sounds_423949', 'lancelindley_sounds_423949-hq.ogg', FREESOUND + 'lancelindley/sounds/423949/',
      'lancelindley (freesound.org)', 'CC BY 4.0', da_s=18.0, durata_s=14.5,
      note='solo il tratto 18-32,5 s: il segnale a 391 Hz senza voce (la voce del file dice «this is a drill»)'),
    E('allarme_propulsione', 'allarme di avaria della propulsione (per lo SCRAM)', 'commons_Alarm_-_Propul_Casualty',
      'Alarm_-_Propul_Casualty.ogg', COMMONS + 'Alarm_-_Propul_Casualty.ogg', 'U.S. Navy', 'PD-USNavy', note=PI_INTERCEPTOR),
    E('fischietto_chiamata', 'fischietto del nostromo, chiamata generale', 'commons_Pipegeneralcall', 'Pipegeneralcall.ogg',
      COMMONS + 'Pipegeneralcall.ogg', 'Seloloving (Wikimedia Commons)', 'CC0'),
    E('fischietto_a_bordo', 'fischietto del nostromo, «piping aboard»', 'commons_Pipeaboard', 'Pipeaboard.ogg',
      COMMONS + 'Pipeaboard.ogg', 'U.S. Department of Defense', 'PD-USGov'),
    # --- sala radio ---
    E('cw_rumoroso', 'CW vero sui 40 m, ricevitore Kenwood R-2000', 'freesound_xperseai_sounds_752469',
      'xperseai_sounds_752469-hq.ogg', FREESOUND + 'xperseai/sounds/752469/', 'xperseai (freesound.org)', 'CC0'),
    E('cw_fading', 'CW vero con fading, WebSDR', 'freesound_klankbeeld_sounds_554164', 'klankbeeld_sounds_554164-hq.ogg',
      FREESOUND + 'klankbeeld/sounds/554164/', 'klankbeeld (freesound.org)', 'CC BY 4.0', da_s=0.0, durata_s=60.0,
      note='il primo minuto'),
    E('fruscio_onde_corte', 'fruscio e toni delle onde corte (Grundig Yacht Boy 207)', 'freesound_nakkivene66_sounds_583364',
      'nakkivene66_sounds_583364-hq.ogg', FREESOUND + 'nakkivene66/sounds/583364/', 'nakkivene66 (freesound.org)', 'CC0'),
    E('telescrivente', 'telescrivente Lorenz Lo 133 che stampa', 'commons_WWS_Teleprinterautomaticprinting',
      'WWS_Teleprinterautomaticprinting.ogg', COMMONS + 'WWS_Teleprinterautomaticprinting.ogg',
      'Work With Sounds / Konrad Gutkowski', 'CC BY 4.0'),
    E('telescrivente_strappo', 'telescrivente con lo strappo della carta', 'freesound_Halleck_sounds_19488',
      'Halleck_sounds_19488-hq.ogg', FREESOUND + 'Halleck/sounds/19488/', 'Halleck (freesound.org)', 'CC BY 4.0'),
    E('rtty', 'RTTY vero ricevuto in HF (DX394)', 'freesound_benj500_sounds_369476', 'benj500_sounds_369476-hq.ogg',
      FREESOUND + 'benj500/sounds/369476/', 'benj500 (freesound.org)', 'CC0'),
    E('radiofax', 'radiofax del National Weather Service', 'commons_Radiofax', 'Radiofax.ogg', COMMONS + 'Radiofax.ogg',
      'National Weather Service, USA', 'PD-USGov-NOAA'),
    E('uvb76', '«The Buzzer» UVB-76, la radio sovietica (dalla fine degli anni \'70)', 'commons_UVB-76', 'UVB-76.ogg',
      COMMONS + 'UVB-76.ogg', 'Mysid (Wikimedia Commons)', 'PD-autore'),
    E('vlf_naturale', 'VLF naturale: sferics e chorus, senza elaborazione', 'freesound_awg_sounds_395575',
      'awg_sounds_395575-hq.ogg', FREESOUND + 'awg/sounds/395575/', 'awg (freesound.org)', 'CC BY 4.0', da_s=60.0,
      durata_s=60.0, note='un minuto, da 1:00 a 2:00 (il file intero e\' di 9:37)'),
    # --- sonar ---
    E('segnali_sonar', 'segnali sonar veri, dal CD «Schall im Meer» del FWG della Bundeswehr (2000)',
      'commons_Sonar_Signals', 'Sonar_Signals.ogg', COMMONS + 'Sonar_Signals.ogg', 'L. Ginzkey (Wikimedia Commons)',
      'CC BY-SA 2.5', note='CC BY-SA: resta un file a se\', non si fonde con altro'),
]

LICENZE = {'PD-USNavy', 'PD-USGov', 'PD-USGov-NOAA', 'PD-autore', 'CC0', 'CC BY 4.0', 'CC BY-SA 2.5'}
assert all(e['licenza'] in LICENZE for e in MANIFESTO)
assert len({e['id'] for e in MANIFESTO}) == len(MANIFESTO)


def leggi(percorso):
    import av
    import numpy as np
    with av.open(percorso) as f:
        flusso = f.streams.audio[0]
        fs = flusso.sample_rate
        pezzi = [fr.to_ndarray().astype(np.float64) for fr in f.decode(flusso)]
    x = np.concatenate([p if p.ndim == 1 else p.mean(axis=0) for p in pezzi])
    if np.abs(x).max() > 1.5:   # interi
        x = x / 32768.0
    return x, fs


def tratta(x, fs, da_s, durata_s):
    import numpy as np
    from math import gcd
    from scipy.signal import resample_poly
    a = int(round(da_s * fs))
    b = len(x) if durata_s is None else min(len(x), a + int(round(durata_s * fs)))
    x = x[a:b] - np.mean(x[a:b])
    g = gcd(int(fs), FS)
    y = resample_poly(x, FS // g, int(fs) // g) if fs != FS else x
    n = int(RAMPA_S * FS)
    if len(y) > 2 * n:
        r = np.linspace(0.0, 1.0, n)
        y[:n] *= r
        y[-n:] *= r[::-1]
    picco = np.abs(y).max()
    return y * (PICCO / picco) if picco > 0 else y


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, (int, float)):
        return repr(float(v)) if isinstance(v, float) else str(v)
    return '"' + str(v).replace('\\', '\\\\').replace('"', '\\"') + '"'


def esporta_effetti_veri(radice):
    import numpy as np
    from scipy.io import wavfile
    grezzi = os.path.join(radice, 'tools', 'campioni', 'effetti_grezzi')
    uscita = os.path.join(radice, 'data', 'suoni', 'effetti')
    if not os.path.isdir(grezzi):
        return 0
    os.makedirs(uscita, exist_ok=True)
    righe = ['# Effetti di bordo registrati (generato da tools/pipeline/effetti_veri.py: non modificare a mano). Fonte,',
             '# autore e licenza di ogni file; PCM 16 bit mono a 22.050 Hz in data/suoni/effetti.', '']
    fatti = 0
    for e in MANIFESTO:
        p = os.path.join(grezzi, e['cartella'], e['file'])
        if not os.path.exists(p):
            print('effetti veri: manca', p)
            continue
        x, fs = leggi(p)
        y = tratta(x, fs, e['da_s'], e['durata_s'])
        wavfile.write(os.path.join(uscita, e['id'] + '.wav'), FS, np.int16(np.clip(y, -1, 1) * 32767))
        righe += [f'[effetti.{e["id"]}]', f'file = "suoni/effetti/{e["id"]}.wav"', f'campionamento_hz = {FS}',
                  f'durata_s = {valore(round(len(y) / FS, 3))}']
        for k in ('voce', 'pagina', 'autore', 'licenza', 'note'):
            righe.append(f'{k} = {valore(e[k])}')
        righe.append('')
        fatti += 1
    # quelli che non hanno una registrazione vera: sintetizzati qui (tools/pipeline/effetti_sintetici.py)
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from effetti_sintetici import sintetizza, FS as FS_SINTESI
    assert FS_SINTESI == FS
    for ident, voce, y in sintetizza():
        wavfile.write(os.path.join(uscita, ident + '.wav'), FS, np.int16(np.clip(y, -1, 1) * 32767))
        righe += [f'[effetti.{ident}]', f'file = "suoni/effetti/{ident}.wav"', f'campionamento_hz = {FS}',
                  f'durata_s = {valore(round(len(y) / FS, 3))}', f'voce = {valore(voce)}',
                  'pagina = ""', 'autore = "melchior-688 (sintesi, tools/pipeline/effetti_sintetici.py)"',
                  'licenza = "CC0"', 'note = "sintetizzato: nessuna registrazione vera con una licenza ammessa"', '']
        fatti += 1
    open(os.path.join(radice, 'data', 'motore', 'effetti.toml'), 'w').write('\n'.join(righe))
    return fatti


if __name__ == '__main__':
    print('effetti veri:', esporta_effetti_veri(sys.argv[1] if len(sys.argv) > 1 else '.'))
