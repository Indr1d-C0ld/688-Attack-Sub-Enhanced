"""Campioni in partita del classico (GAME.EXE, 1989) -> data/motore/suoni_classici/<nome>.t e suoni_classici.toml.
I sei .T del disco 2 (extracted/raw/DISK2) si copiano senza modifiche: PCM a 4 bit senza segno, due campioni per byte,
prima il nibble alto. La frequenza di riproduzione la decide l'evento (docs/originale/suoni.md), non il file: per
questo restano nibble e non WAV. Il toml dice da dove viene ogni file (disco, nome, byte, sha1) e a cosa serve, con gli
indirizzi di GAME.EXE. Li legge nucleo/suoni_classici.
Uso: suoni_classici.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import hashlib, os, shutil, sys

CAMPIONI = [
    ('EXP1', 'esplosione: 5000 Hz ai messaggi 7 e 50 (1000:8CAE) e al periscopio (1000:2AA8), 4000 Hz ai messaggi 19, 20, 26, 30 (1000:8C54)'),
    ('SONAR1', 'impulso sonar: 5500 Hz al ping proprio (1000:3D4E), 6200 Hz al messaggio 8 (1000:8DFE)'),
    ('TORPRAMP', 'lancio del giocatore da un tubo: 6500 Hz (1000:7A73)'),
    ('PROP2', 'ANALYZE, navi: 2500 + 50 t Hz, 4 volte (1000:4049)'),
    ('PROPV2', 'ANALYZE, armi 5500 + 10 t Hz 5 volte, sottomarini ed esche 2000 + 30 t Hz 4 volte (1000:4049)'),
    ('WHALE2', 'ANALYZE, biologici: 2500-6499 Hz a caso, 2 volte (1000:4049)'),
]


def esporta_suoni_classici(radice):
    sorgente = os.path.join(radice, 'extracted', 'raw', 'DISK2')
    uscita = os.path.join(radice, 'data', 'motore')
    os.makedirs(os.path.join(uscita, 'suoni_classici'), exist_ok=True)
    righe = ['# Campioni in partita del classico, copiati senza modifiche dai .T del disco 2 dell\'originale',
             '# (tools/pipeline/suoni_classici.py; quando suonano: docs/originale/suoni.md). Asset dell\'originale del 1989,',
             '# dal disco dell\'utente: non ridistribuibili.',
             'formato = "PCM a 4 bit senza segno, due campioni per byte, prima il nibble alto"', '']
    fatti = []
    for nome, uso in CAMPIONI:
        f = os.path.join(sorgente, f'{nome}.T')
        if not os.path.exists(f):
            continue
        dati = open(f, 'rb').read()
        rel = f'suoni_classici/{nome.lower()}.t'
        shutil.copyfile(f, os.path.join(uscita, rel))
        righe += [f'[campioni.{nome}]', f'file = "motore/{rel}"', f'origine = "DISK2/{nome}.T"', f'byte = {len(dati)}',
                  f'sha1 = "{hashlib.sha1(dati).hexdigest()}"', f'uso = "{uso}"', '']
        fatti.append(nome)
    open(os.path.join(uscita, 'suoni_classici.toml'), 'w').write('\n'.join(righe))
    return fatti


if __name__ == '__main__':
    print('campioni del classico:', ' '.join(esporta_suoni_classici(sys.argv[1] if len(sys.argv) > 1 else '.')))
