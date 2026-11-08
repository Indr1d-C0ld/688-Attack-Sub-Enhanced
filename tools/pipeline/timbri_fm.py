"""Timbri FM a due operatori per la musica dal vivo (roadmap 38, A) -> data/motore/timbri_fm.toml.
Li legge nucleo/musica_fm.hpp (carica_banco_fm): un timbro per programma General MIDI (0-based) e per le note del
ritmo (canale 10), coi registri dell'OPL3 (YMF262). I primi sono quelli dei temi dei trailer (tools/trailer/opl_midi.cpp);
gli altri sono scritti qui per le musiche del remake. Tutti originali del progetto (CC0), STIMA d'ascolto: nessun banco
d'epoca copiato.
Uso: timbri_fm.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

CAMPI = ('mult', 'tl', 'ar', 'dr', 'sl', 'rr', 'onda', 'vib', 'am', 'tenuta', 'ksr', 'ksl')


def op(mult, tl, ar, dr, sl, rr, onda=0, vib=False, am=False, tenuta=True, ksr=False, ksl=0):
    return dict(zip(CAMPI, (mult, tl, ar, dr, sl, rr, onda, vib, am, tenuta, ksr, ksl)))


def T(nome, mod, por, retro, connessione=0, nota_fissa=-1):
    return dict(nome=nome, mod=mod, por=por, retro=retro, connessione=connessione, nota_fissa=nota_fissa)


PROGRAMMI = {
    # dai trailer (opl_midi.cpp)
    52: T('coro', op(1, 30, 5, 2, 2, 5, vib=True), op(1, 0, 5, 1, 1, 5, vib=True), 3),
    48: T('archi', op(1, 24, 7, 1, 1, 6, vib=True), op(1, 0, 6, 1, 1, 6, vib=True), 6),
    43: T('contrabbassi', op(0, 20, 15, 4, 3, 7, ksl=1), op(0, 0, 15, 3, 2, 7), 5),
    61: T('ottoni', op(1, 18, 11, 2, 2, 8, vib=True), op(1, 0, 13, 2, 2, 8), 6),
    60: T('corni', op(1, 26, 10, 2, 2, 7, vib=True), op(1, 0, 11, 2, 2, 7), 4),
    47: T('timpani', op(0, 18, 15, 5, 8, 6, tenuta=False), op(0, 0, 15, 4, 7, 4, tenuta=False), 4),
    14: T('campane tubolari', op(7, 26, 15, 3, 6, 3, tenuta=False), op(2, 0, 15, 2, 5, 3, tenuta=False), 2),
    # nuovi (31/10/2026): il necessario per le musiche delle schermate
    0: T('pianoforte', op(1, 28, 15, 3, 7, 5, tenuta=False, ksl=1), op(1, 0, 15, 2, 6, 5, tenuta=False), 3),
    8: T('celesta', op(4, 30, 15, 4, 6, 4, tenuta=False), op(1, 0, 15, 3, 6, 4, tenuta=False), 1),
    11: T('vibrafono', op(4, 34, 15, 2, 5, 4, am=True, tenuta=False), op(1, 0, 15, 2, 5, 4, am=True, tenuta=False), 0),
    19: T('organo', op(1, 12, 15, 0, 0, 8), op(2, 0, 15, 0, 0, 8), 0, connessione=1),
    33: T('basso elettrico', op(0, 22, 15, 3, 4, 7, ksl=1), op(1, 0, 15, 2, 4, 7), 4),
    38: T('basso sintetico', op(0, 16, 15, 2, 3, 8), op(1, 0, 15, 2, 4, 8), 6),
    56: T('tromba', op(1, 20, 12, 1, 1, 8, vib=True), op(1, 0, 13, 1, 1, 8, vib=True), 5),
    71: T('clarinetto', op(2, 26, 12, 1, 1, 8, vib=True), op(1, 0, 12, 1, 1, 8), 4),
    73: T('flauto', op(1, 40, 10, 1, 0, 7, vib=True), op(1, 0, 9, 1, 0, 7, vib=True), 0),
    80: T('onda quadra', op(1, 22, 15, 0, 0, 7, onda=0), op(1, 0, 15, 0, 0, 7, onda=2), 7),
    88: T('pad caldo', op(1, 32, 4, 1, 1, 4, vib=True), op(1, 0, 3, 1, 1, 3, vib=True), 2),
    89: T('pad freddo', op(3, 34, 3, 1, 2, 3, vib=True), op(1, 0, 3, 1, 1, 3, vib=True), 3),
    95: T('pad scuro', op(0, 30, 3, 1, 1, 3), op(1, 0, 2, 1, 1, 3, vib=True), 5),
}

RITMO = {
    36: T('grancassa', op(0, 8, 15, 7, 15, 8, tenuta=False), op(0, 0, 15, 6, 15, 7, tenuta=False), 7, nota_fissa=28),
    41: T('taiko', op(0, 12, 15, 6, 15, 6, tenuta=False), op(0, 0, 15, 5, 15, 6, tenuta=False), 6, nota_fissa=33),
    38: T('rullante', op(15, 0, 15, 8, 15, 8, tenuta=False), op(1, 4, 15, 8, 15, 8, tenuta=False), 7, nota_fissa=55),
    49: T('piatto', op(15, 0, 15, 3, 15, 4, tenuta=False), op(13, 6, 15, 3, 15, 4, tenuta=False), 7, 1, nota_fissa=84),
    42: T('charleston chiuso', op(15, 0, 15, 10, 15, 10, tenuta=False), op(14, 8, 15, 10, 15, 10, tenuta=False), 7, 1, nota_fissa=90),
    39: T('battimani', op(15, 4, 15, 9, 15, 9, tenuta=False), op(3, 6, 15, 9, 15, 9, tenuta=False), 7, nota_fissa=62),
}

for t in list(PROGRAMMI.values()) + list(RITMO.values()):
    for o in (t['mod'], t['por']):
        assert 0 <= o['mult'] <= 15 and 0 <= o['tl'] <= 63 and 0 <= o['onda'] <= 7, t['nome']
        assert all(0 <= o[k] <= 15 for k in ('ar', 'dr', 'sl', 'rr')) and 0 <= o['ksl'] <= 3, t['nome']
    assert 0 <= t['retro'] <= 7 and t['connessione'] in (0, 1), t['nome']


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, int):
        return str(v)
    return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'


def esporta_timbri_fm(radice):
    r = ['# Timbri FM a due operatori (generato da tools/pipeline/timbri_fm.py: non modificare a mano). Originali del',
         '# progetto (CC0). programmi: programma General MIDI 0-based; ritmo: nota del canale 10.', '',
         'programma_riserva = 48', '']
    for sezione, tab in (('programmi', PROGRAMMI), ('ritmo', RITMO)):
        for n in sorted(tab):
            t = tab[n]
            r.append(f'[{sezione}.{n}]')
            for k in ('nome', 'retro', 'connessione', 'nota_fissa'):
                r.append(f'{k} = {valore(t[k])}')
            for k in ('mod', 'por'):
                r.append(f'{k} = {{ ' + ', '.join(f'{c} = {valore(t[k][c])}' for c in CAMPI) + ' }')
            r.append('')
    open(os.path.join(radice, 'data', 'motore', 'timbri_fm.toml'), 'w').write('\n'.join(r))
    return len(PROGRAMMI), len(RITMO)


if __name__ == '__main__':
    print('timbri FM (programmi, ritmo):', esporta_timbri_fm(sys.argv[1] if len(sys.argv) > 1 else '.'))
