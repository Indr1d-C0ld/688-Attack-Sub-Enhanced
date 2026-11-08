"""Spettrogrammi di controllo a occhio (0-8 kHz, log in frequenza simulata con due pannelli: 0-1 kHz e 0-8 kHz)"""
import sys, os
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/pipeline')
import numpy as np
from scipy import signal
from PIL import Image
import campioni_analisi as A
out = sys.argv[1]
for p in sys.argv[2:]:
    x, fs = A.leggi(p)
    x = x[:int(60 * fs)]
    pann = []
    for fmax, nper in ((1000, 8192), (8000, 1024)):
        f, t, S = signal.spectrogram(x, fs, nperseg=nper, noverlap=nper // 2)
        S = 10 * np.log10(S[f <= fmax] + 1e-20)
        S = S - np.median(S, axis=1, keepdims=True)   # sbiancato per riga: si vedono le strutture
        S = np.clip(S / 25.0, 0, 1)
        pann.append(Image.fromarray(np.uint8(255 * S[::-1])).resize((900, 200)))
    img = Image.new('L', (900, 410), 128)
    img.paste(pann[1], (0, 0)); img.paste(pann[0], (0, 210))
    img.save(os.path.join(out, os.path.basename(p) + '.png'))
