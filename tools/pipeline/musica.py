"""Musica dei titoli dell'originale (CALLS.EXE, 1989) -> data/motore/musica/titolo_<versione>.wav.
Le tre versioni estratte da tools/re (extracted/musica, catalogo.json): AdLib (OPL2, resa con ymfm), Tandy 1000 e
Covox. L'app le suona nel titolo e nel menu (app/musica_titoli). Qui si portano tutte a PCM a 16 bit mono, che e' quello
che legge nucleo/campioni (leggi_wav): il Covox nasce a 8 bit senza segno.
Uso: musica.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, struct, sys, wave

VERSIONI = ('adlib', 'tandy', 'covox')


def a_16_bit(dati, larghezza):
    if larghezza == 2:
        return dati
    if larghezza == 1:   # 8 bit senza segno -> 16 bit con segno
        return b''.join(struct.pack('<h', (b - 128) << 8) for b in dati)
    raise ValueError(f'campioni a {8 * larghezza} bit non previsti')


def esporta_musica(radice):
    uscita = os.path.join(radice, 'data', 'motore', 'musica')
    os.makedirs(uscita, exist_ok=True)
    fatte = []
    for v in VERSIONI:
        sorgente = os.path.join(radice, 'extracted', 'musica', f'titolo_{v}.wav')
        if not os.path.exists(sorgente):
            continue
        with wave.open(sorgente, 'rb') as w:
            if w.getnchannels() != 1:
                raise ValueError(f'{sorgente}: attesa una traccia mono')
            fs, larghezza = w.getframerate(), w.getsampwidth()
            dati = a_16_bit(w.readframes(w.getnframes()), larghezza)
        with wave.open(os.path.join(uscita, f'titolo_{v}.wav'), 'wb') as w:
            w.setnchannels(1)
            w.setsampwidth(2)
            w.setframerate(fs)
            w.writeframes(dati)
        fatte.append(v)
    return fatte


if __name__ == '__main__':
    print('musica dei titoli:', ' '.join(esporta_musica(sys.argv[1] if len(sys.argv) > 1 else '.')))
