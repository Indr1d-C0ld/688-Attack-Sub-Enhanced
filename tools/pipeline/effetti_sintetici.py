"""Effetti di bordo sintetizzati (roadmap 38, C): quelli per cui non c'e' una registrazione vera con una licenza
ammessa (nota condivisa disassemblatore-audio-campioni-da-approvare.md, «Senza un campione vero»). Li scrive
effetti_veri.py in data/suoni/effetti/ e in effetti.toml, con la licenza del progetto (CC0).
Sintesi semplice e deterministica (rumore filtrato, modi risonanti, motori con le armoniche), all'orecchio d'epoca:
il Mixer li passa poi dalla resa a 8 bit. Ogni effetto e' una STIMA sul suono vero (le fonti sono nelle note).
Si importa da effetti_veri.py; da solo scrive i WAV in una cartella di prova:  effetti_sintetici.py <cartella>"""
import os, sys

import numpy as np

FS = 22050


def _t(durata):
    return np.arange(int(durata * FS)) / FS


def _rumore(n, seme):
    return np.random.default_rng(seme).standard_normal(n)


def _passa_basso(x, fc):
    a = np.exp(-2 * np.pi * fc / FS)
    y = np.empty_like(x)
    s = 0.0
    for i, v in enumerate(x):
        s = (1 - a) * v + a * s
        y[i] = s
    return y


def _passa_banda(x, fc, q):
    # biquad a guadagno costante sul picco
    w = 2 * np.pi * fc / FS
    alfa = np.sin(w) / (2 * q)
    b0, b2 = alfa, -alfa
    a0, a1, a2 = 1 + alfa, -2 * np.cos(w), 1 - alfa
    y = np.zeros_like(x)
    x1 = x2 = y1 = y2 = 0.0
    for i, v in enumerate(x):
        o = (b0 * v + b2 * x2 - a1 * y1 - a2 * y2) / a0
        x2, x1, y2, y1 = x1, v, y1, o
        y[i] = o
    return y


def _inviluppo(n, attacco, rilascio):
    e = np.ones(n)
    a, r = int(attacco * FS), int(rilascio * FS)
    if a:
        e[:a] = np.linspace(0, 1, a)
    if r:
        e[-r:] *= np.linspace(1, 0, r)
    return e


def _modi(durata, modi, seme=0):
    """Un colpo su una struttura: modi (frequenza Hz, decadimento s, ampiezza)."""
    t = _t(durata)
    x = np.zeros_like(t)
    fasi = np.random.default_rng(seme).uniform(0, 2 * np.pi, len(modi))
    for (f, tau, a), ph in zip(modi, fasi):
        x += a * np.exp(-t / tau) * np.sin(2 * np.pi * f * t + ph)
    return x


def _motore(durata, f0, armoniche, seme, salita=0.0):
    """Un motore idraulico: la fondamentale (che puo' salire del fattore `salita` all'avvio) e le armoniche."""
    t = _t(durata)
    f = f0 * (1 + salita * (1 - np.exp(-t / 0.4)))
    fase = 2 * np.pi * np.cumsum(f) / FS
    x = sum(a * np.sin(k * fase) for k, a in armoniche)
    return x + 0.15 * _passa_banda(_rumore(len(t), seme), 2 * f0 * 4, 2.0)


def norma(x, picco=0.89):
    m = np.abs(x).max()
    return x * (picco / m) if m > 0 else x


def periscopio(su):
    # il motore idraulico del sollevamento (whine che sale e si assesta), il sibilo dell'olio, il colpo d'arresto
    d = 4.5
    x = _motore(d, 95, [(1, 0.5), (2, 0.35), (3, 0.2), (5, 0.1)], 11 if su else 12, salita=0.25 if su else 0.15)
    x *= _inviluppo(len(x), 0.3, 0.5)
    x += 0.25 * _passa_banda(_rumore(len(x), 13), 3200, 1.0) * _inviluppo(len(x), 0.5, 0.8)
    colpo = _modi(0.6, [(180, 0.08, 1.0), (410, 0.05, 0.6), (1230, 0.03, 0.3)], 14)
    k = int((d - 0.7) * FS)
    x[k:k + len(colpo)] += 1.4 * colpo[:len(x) - k]
    return norma(x)


def antenna():
    x = _motore(2.8, 120, [(1, 0.5), (2, 0.4), (4, 0.15)], 21, salita=0.3) * _inviluppo(int(2.8 * FS), 0.2, 0.4)
    colpo = _modi(0.5, [(240, 0.06, 1.0), (620, 0.04, 0.5)], 22)
    k = int(2.2 * FS)
    x[k:k + len(colpo)] += colpo[:len(x) - k]
    return norma(x)


def tubo_allaga():
    # l'acqua che entra nel tubo: un rombo che cresce, gorgoglii, l'aria che esce dallo sfogo
    d = 6.0
    n = int(d * FS)
    r = _passa_basso(_rumore(n, 31), 400) * np.linspace(0.3, 1.0, n)
    g = np.zeros(n)
    rng = np.random.default_rng(32)
    for k in rng.uniform(0.2, d - 0.5, 40):
        b = _modi(0.15, [(rng.uniform(150, 600), 0.03, 1.0)], int(k * 1000))
        i = int(k * FS)
        g[i:i + len(b)] += b[:n - i]
    s = 0.3 * _passa_banda(_rumore(n, 33), 2500, 0.8) * _inviluppo(n, 0.5, 1.5)
    return norma((r + 0.4 * g + s) * _inviluppo(n, 0.4, 1.0))


def tubo_lancio():
    # l'impulso: il colpo sordo dello stantuffo ad acqua, il soffio d'aria, l'acqua che torna nel tubo
    d = 3.0
    n = int(d * FS)
    t = _t(d)
    colpo = np.exp(-t / 0.18) * np.sin(2 * np.pi * (45 + 25 * np.exp(-t / 0.1)) * t)
    soffio = _passa_basso(_rumore(n, 41), 900) * np.exp(-t / 0.6)
    ritorno = _passa_basso(_rumore(n, 42), 250) * np.clip((t - 0.6) / 0.4, 0, 1) * np.exp(-(t - 0.6).clip(0) / 1.0)
    return norma(1.2 * colpo + 0.6 * soffio + 0.5 * ritorno)


def ricarica_tubo():
    # la ricarica: il carrello idraulico (whine), le catene e i colpi metallici dello stivaggio
    d = 8.0
    n = int(d * FS)
    x = 0.5 * _motore(d, 70, [(1, 0.5), (2, 0.4), (3, 0.3)], 51, salita=0.2) * _inviluppo(n, 0.5, 1.0)
    rng = np.random.default_rng(52)
    for k in [0.8, 2.1, 2.4, 4.0, 5.6, 6.9, 7.3]:
        b = _modi(0.7, [(rng.uniform(120, 260), 0.15, 1.0), (rng.uniform(500, 900), 0.08, 0.5),
                        (rng.uniform(1800, 2600), 0.04, 0.25)], int(k * 10))
        i = int(k * FS)
        x[i:i + len(b)] += 0.9 * b[:n - i]
    return norma(x)


def soffiata_emergenza():
    # la soffiata d'emergenza delle casse: l'aria ad alta pressione, un rombo larghissimo che cala piano
    d = 9.0
    n = int(d * FS)
    t = _t(d)
    x = _passa_basso(_rumore(n, 61), 1800) + 0.5 * _passa_banda(_rumore(n, 62), 300, 0.7)
    return norma(x * np.minimum(1, t / 0.15) * np.exp(-t / 5.0) * _inviluppo(n, 0, 1.0))


def sfogo_casse():
    d = 4.5
    n = int(d * FS)
    x = 0.6 * _passa_banda(_rumore(n, 71), 1500, 0.9) * _inviluppo(n, 0.2, 1.5)
    rng = np.random.default_rng(72)
    for k in rng.uniform(0.5, d - 0.5, 60):   # le bolle
        b = _modi(0.08, [(rng.uniform(300, 1200), 0.02, 1.0)], int(k * 997))
        i = int(k * FS)
        x[i:i + len(b)] += 0.5 * b[:n - i]
    return norma(x)


def scafo_scricchiola():
    # lo scafo che si assesta con la quota: gemiti lenti (50-200 Hz) e qualche schiocco
    d = 4.0
    n = int(d * FS)
    t = _t(d)
    f = 70 + 40 * np.sin(2 * np.pi * 0.35 * t)
    x = np.sin(2 * np.pi * np.cumsum(f) / FS) * (0.5 + 0.5 * np.sin(2 * np.pi * 0.8 * t)) ** 2
    x += 0.3 * _passa_banda(_rumore(n, 81), 140, 4.0)
    for k, s in ((1.2, 82), (2.9, 83)):
        b = _modi(0.4, [(95, 0.12, 1.0), (260, 0.06, 0.6), (880, 0.02, 0.3)], s)
        i = int(k * FS)
        x[i:i + len(b)] += 1.2 * b[:n - i]
    return norma(x * _inviluppo(n, 0.4, 0.8))


def portello_sbatte():
    return norma(_modi(1.6, [(118, 0.35, 1.0), (247, 0.25, 0.7), (521, 0.15, 0.5), (1170, 0.07, 0.35),
                             (2380, 0.03, 0.2)], 91) + 0.5 * _passa_basso(_rumore(int(1.6 * FS), 92), 3000)
                * np.exp(-_t(1.6) / 0.01))


def campane_combattimento():
    # «колокола громкого боя»: le campane elettriche dell'allarme di combattimento sovietico, una lunga suonata
    d = 6.0
    t = _t(d)
    x = np.zeros_like(t)
    for k in np.arange(0, d - 0.3, 1 / 22.0):   # il battaglio a ~22 colpi al secondo
        b = _modi(0.25, [(980, 0.12, 1.0), (2650, 0.05, 0.5), (4900, 0.02, 0.25)], int(k * 100))
        i = int(k * FS)
        x[i:i + len(b)] += b[:len(x) - i]
    return norma(x * _inviluppo(len(x), 0.01, 0.4))


def revun():
    # il «ревун»: la sirena elettrica a vibratore, un ronzio aspro sui 500 Hz, a colpi lunghi
    d = 6.0
    t = _t(d)
    f = 480 + 15 * np.sin(2 * np.pi * 6 * t)
    fase = 2 * np.pi * np.cumsum(f) / FS
    x = np.sign(np.sin(fase)) * 0.6 + 0.4 * np.sign(np.sin(2 * fase + 0.3))
    x = _passa_basso(x, 3500)
    porta = ((t % 2.0) < 1.5).astype(float)
    porta = _passa_basso(porta, 30)
    return norma(x * porta)


def allarme_chimico():
    # USN: un allarme a tono che sale e scende (STIMA: il chemical alarm degli stack AN/WIC non ha una registrazione libera)
    d = 6.0
    t = _t(d)
    f = 600 + 400 * (0.5 - 0.5 * np.cos(2 * np.pi * t / 1.5))
    x = np.sin(2 * np.pi * np.cumsum(f) / FS)
    x += 0.3 * np.sin(2 * 2 * np.pi * np.cumsum(f) / FS)
    return norma(x * _inviluppo(len(t), 0.02, 0.2))


def campanello_flash():
    # il campanello della telescrivente: i messaggi FLASH lo fanno suonare piu' volte (qui cinque)
    d = 2.6
    x = np.zeros(int(d * FS))
    for k in range(5):
        b = _modi(1.2, [(1870, 0.45, 1.0), (4650, 0.18, 0.4), (7200, 0.08, 0.15)], 100 + k)
        i = int(k * 0.32 * FS)
        x[i:i + len(b)] += b[:len(x) - i]
    return norma(x)


def tono_sonar_mine():
    # il contatto del sonar da mine e da ghiaccio: un tono a campana in FM (STIMA; «Hell's Bells» era il soprannome del
    # tono del sonar FM per le mine del 1945, Navy Times: per la BQS-15 nessuna fonte lo usa)
    d = 1.8
    t = _t(d)
    portante, modulante = 1320.0, 1320.0 * 1.41
    indice = 3.0 * np.exp(-t / 0.4)
    x = np.sin(2 * np.pi * portante * t + indice * np.sin(2 * np.pi * modulante * t)) * np.exp(-t / 0.55)
    return norma(x * _inviluppo(len(t), 0.005, 0.1))


def clic_interfaccia():
    return norma(_modi(0.05, [(3200, 0.004, 1.0), (7400, 0.002, 0.5)], 111), 0.6)


def bip_messaggio():
    t = _t(0.09)
    return norma(np.sin(2 * np.pi * 1046.5 * t) * _inviluppo(len(t), 0.004, 0.03), 0.5)


EFFETTI = [
    ('periscopio_su', 'idraulica del periscopio che sale, col colpo d\'arresto', lambda: periscopio(True)),
    ('periscopio_giu', 'idraulica del periscopio che scende', lambda: periscopio(False)),
    ('antenna_su', 'un albero o l\'antenna che sale', antenna),
    ('tubo_allaga', 'l\'allagamento del tubo lanciasiluri', tubo_allaga),
    ('tubo_lancio', 'il lancio sentito a bordo: colpo dello stantuffo, soffio, ritorno dell\'acqua', tubo_lancio),
    ('ricarica_tubo', 'la ricarica di un tubo: carrello idraulico e colpi metallici', ricarica_tubo),
    ('soffiata_emergenza', 'la soffiata d\'emergenza delle casse zavorra', soffiata_emergenza),
    ('sfogo_casse', 'lo sfogo delle casse, aria e bolle', sfogo_casse),
    ('scafo_scricchiola', 'lo scafo che si assesta con la quota', scafo_scricchiola),
    ('portello_sbatte', 'un portello stagno che sbatte', portello_sbatte),
    ('campane_combattimento', 'l\'allarme di combattimento sovietico: le campane «громкого боя»', campane_combattimento),
    ('revun', 'il «ревун» sovietico, la sirena a vibratore', revun),
    ('allarme_chimico', 'allarme chimico o antincendio USN (STIMA)', allarme_chimico),
    ('campanello_flash', 'i cinque colpi di campanello della telescrivente per un FLASH', campanello_flash),
    ('tono_sonar_mine', 'il tono di contatto del sonar da mine e da ghiaccio (STIMA)', tono_sonar_mine),
    ('clic_interfaccia', 'il clic dei comandi dell\'interfaccia', clic_interfaccia),
    ('bip_messaggio', 'il bip leggero di un messaggio nella barra', bip_messaggio),
]


def sintetizza():
    """[(id, voce, campioni float -1..1 a FS)] in ordine."""
    return [(i, voce, np.asarray(f(), dtype=np.float64)) for i, voce, f in EFFETTI]


if __name__ == '__main__':
    from scipy.io import wavfile
    cartella = sys.argv[1] if len(sys.argv) > 1 else '.'
    os.makedirs(cartella, exist_ok=True)
    for i, voce, x in sintetizza():
        wavfile.write(os.path.join(cartella, i + '.wav'), FS, np.int16(np.clip(x, -1, 1) * 32767))
        print(f'{i:24} {len(x) / FS:5.2f} s  {voce}')
