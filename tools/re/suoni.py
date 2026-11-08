"""Effetti sonori in partita (.T) alle frequenze usate da GAME.EXE, e firme acustiche di ANALYZE.

Formato .T: PCM a 4 bit, due campioni per byte, prima il nibble alto. GAME.EXE li suona a ciclo d'attesa
(1000:D461): ogni nibble passa da una tabella di conversione (DS:BC52) e va all'altoparlante (porta 61h),
al chip del Tandy (porta C0h) o al Covox (LPT1/LPT2), poi si aspetta un ritardo d = s x 3200 / p
(1000:407E e 1000:A0E6), dove s e' la velocita' della CPU misurata all'avvio (1000:A008: blocchi da 100
giri di un ciclo d'attesa in un periodo del timer di 54,9 ms) e p un parametro della chiamata. Il tempo per
nibble non dipende quindi dalla CPU: p e' la frequenza di campionamento voluta, in Hz (i valori sono tondi;
la stima dai cicli del ciclo di ritardo da' circa il 90% di p).

Chiamate (1000:2AA8, 3D4E, 7A73, 8C54, 8CAE, 8DFE): EXP1 a 5000 e 4000 Hz, sonar1 a 5500 e 6200 Hz,
TORPRAMP a 6500 Hz. ANALYZE (sala sonar, 1000:3F9C-4049) ascolta il contatto selezionato ricampionando un
suono con una frequenza che dipende dal tipo d'unita' t (+4F) e dagli indicatori del contatto di 990C (non sono i bit
di +06: 08h = nave o aereo, cioe' +06 & 6; 10h = arma, +06 & 20h; docs/originale/sensori_e_navigazione.md §10.5):
  08h, nave                   prop2.t   a 2500 + 50 t Hz, 4 volte
  10h, arma                   propv2.t  a 5500 + 10 t Hz, 5 volte
  nessuno dei due, t = 19h    whale2.t  a 2500-6499 Hz casuali, 2 volte
  nessuno dei due             propv2.t  a 2000 + 30 t Hz, 4 volte (sottomarini, esche)
Gli aerei (+06 & 3 = 3) sono rifiutati prima. Ogni classe ha cosi' una sua "firma" ottenuta cambiando l'altezza dello stesso campione.

Quando suona ogni campione (chiamate, messaggi, bip) e' scritto a mano in docs/originale/suoni.md; qui si rigenerano
soltanto le tabelle fra i segnaposto «tabelle» e «fine tabelle» di quel file.

Uso:  suoni.py <radice>  ->  extracted/suoni/*.wav e le tabelle di docs/originale/suoni.md
"""
import os, sys, wave, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tok_schede as T

BASE = [('EXP1', 5000, 'esplosione'), ('EXP1', 4000, 'esplosione, variante grave'),
        ('SONAR1', 5500, 'impulso sonar'), ('SONAR1', 6200, 'impulso sonar, variante acuta'),
        ('TORPRAMP', 6500, 'siluro in corsa')]
# categoria di ogni tipo (decide il suono di ANALYZE): +06 tipico nelle scene (bit 0-1 categoria, 20h arma); le armi
# nascono al lancio (Mk 48 29h, 533 mm 31h); aerei e missili in volo hanno la categoria 3, che ANALYZE rifiuta
ARMI = {0x16, 0x24, 0x25, 0x26, 0x27, 0x2c, 0x2d}
AEREI = {0x11, 0x12, 0x18, 0x23, 0x28, 0x29}
NAVI = {0, 1, 2, 5, 6, 7, 8, 9, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x13}

def nibble(path):
    d = open(path, 'rb').read()
    return bytes(v for b in d for v in ((b >> 4) * 17, (b & 15) * 17))

def scrivi(path, pcm, hz, volte=1):
    with wave.open(path, 'wb') as w:
        w.setnchannels(1); w.setsampwidth(1); w.setframerate(hz); w.writeframes(pcm * volte)

def firma(t):
    if t in AEREI: return None, None, 0, 'aereo o missile: rifiutato'
    if t in NAVI: return 'PROP2', 2500 + 50 * t, 4, 'nave'
    if t in ARMI: return 'PROPV2', 5500 + 10 * t, 5, 'arma'
    if t == 0x19: return 'WHALE2', None, 2, 'biologico'
    return 'PROPV2', 2000 + 30 * t, 4, 'sottomarino o esca'

if __name__ == '__main__':
    root = sys.argv[1]; raw = f'{root}/extracted/raw/DISK2'
    dst = f'{root}/extracted/suoni'; os.makedirs(dst, exist_ok=True)
    md = ['## Tabelle generate', '', 'Da `tools/re/suoni.py`; i WAV sono in `extracted/suoni/`.', '',
          '### Effetti', '', '| File | Frequenza | Uso | WAV |', '|---|---|---|---|']
    for nome, hz, uso in BASE:
        out = f'{nome.lower()}_{hz}hz.wav'; scrivi(f'{dst}/{out}', nibble(f'{raw}/{nome}.T'), hz)
        md.append(f'| {nome}.T | {hz} Hz | {uso} | `{out}` |')
    md += ['', '### Firme acustiche di ANALYZE (sala sonar)', '',
           'Lo stesso campione ricampionato per classe: la frequenza dipende dal tipo d\'unità e dalla categoria del',
           'contatto (qui quella del tipo). Il WAV contiene le ripetizioni che il gioco suona.', '',
           '| Tipo | Classe | Categoria | Suono | Frequenza | Ripetizioni | WAV |', '|---|---|---|---|---|---|---|']
    for t, nomeclasse in sorted(T.TIPI.items()):
        if 'RESERVED' in nomeclasse: continue
        suono, hz, volte, lato = firma(t)
        if suono is None:
            md.append(f"| {t:#04x} | {nomeclasse.title()} | {lato} | — | — | — | — |"); continue
        out = f'analyze_{t:02x}_{suono.lower()}.wav'
        scrivi(f'{dst}/{out}', nibble(f'{raw}/{suono}.T'), hz or 4500, volte)
        md.append(f"| {t:#04x} | {nomeclasse.title()} | {lato} | {suono.lower()}.t | {f'{hz} Hz' if hz else '2500–6499 Hz (caso)'} | {volte} | `{out}` |")
    doc = f'{root}/docs/originale/suoni.md'
    testo = open(doc).read()
    a, b = '<!-- tabelle: tools/re/suoni.py -->', '<!-- fine tabelle -->'
    if a not in testo or b not in testo: sys.exit(f'{doc}: mancano i segnaposto delle tabelle')
    testo = testo[:testo.index(a) + len(a)] + '\n' + '\n'.join(md) + '\n' + testo[testo.index(b):]
    open(doc, 'w').write(testo)
    print(len(os.listdir(dst)), 'wav')
