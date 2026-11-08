"""Tabelle generate dei capitoli F6 (missioni rapide) e F7 (editor e pacchetti) del volume 2 del manuale.

Le importa tools/pipeline/manuale.py (TABELLE_MISSIONI: nome -> funzione(cartella dei dati) -> markdown). Le prime due
vengono da data/motore/missioni_rapide.toml; le altre dai blocchi `editor` dei file di data/missioni/libreria e
data/missioni/prove (lo scenario che lo scrittore dell'editor mette in ogni missione).
"""
import re
import tomllib

TIPI = ['pattuglia', 'caccia', 'antinave', 'scorta', 'ricognizione', 'minamento', 'inserimento']
RUOLI = [('pregio', 'Unità di pregio'), ('scorte', 'Scorte'), ('sottomarini', 'Sottomarini'), ('aerei', 'Aerei'),
         ('elicotteri', 'Elicotteri')]
BATTELLO = {'usa': '688', 'urss': 'Alfa'}


def _rapide(dati):
    return tomllib.loads((dati / 'missioni_rapide.toml').read_text(encoding='utf-8'))


def _teatri(r):
    """I teatri nell'ordine del file (lo stesso del codice missione)"""
    return list(r['teatri'].items())


def rapide_tipi_teatri(dati):
    """I pesi dei sette tipi per teatro e battello: la probabilità relativa del tipo a caso; «—» = non si gioca"""
    from manuale import glossario
    r = _rapide(dati)
    righe = ['| Teatro | Battello | ' + ' | '.join(t.capitalize() for t in TIPI) + ' |',
             '|---|---|' + '---|' * len(TIPI)]
    for _, t in _teatri(r):
        for parte in ('usa', 'urss'):
            pesi = t['pesi'][parte]
            celle = [str(pesi.get(k, 0)) if pesi.get(k, 0) else '—' for k in TIPI]
            righe.append(f"| {glossario(t['nome']['it'])} | {BATTELLO[parte]} | " + ' | '.join(celle) + ' |')
    return '\n'.join(righe) + '\n'


def rapide_forze(dati):
    """Le classi per ruolo che il generatore sceglie, per teatro e per parte delle unità (non del giocatore)"""
    from manuale import glossario
    r = _rapide(dati)
    nomi = r.get('nomi_classi', {})

    def nome(c):
        return nomi[c]['it'][0] if c in nomi else c
    righe = ['| Teatro | Parte | ' + ' | '.join(n for _, n in RUOLI) + ' |', '|---|---|' + '---|' * len(RUOLI)]
    for _, t in _teatri(r):
        for parte, etichetta in (('usa', 'NATO'), ('urss', 'URSS')):
            f = t['forze'][parte]
            celle = [', '.join(nome(c) for c in f.get(k, [])) or '—' for k, _ in RUOLI]
            righe.append(f"| {glossario(t['nome']['it'])} | {etichetta} | " + ' | '.join(celle) + ' |')
    return '\n'.join(righe) + '\n'


def _scenario(testo):
    """I campi dell'intestazione dal blocco `editor` di un file di missione"""
    i = testo.find('editor = { formato_editor')
    if i < 0:
        return None
    blocco = testo[i:i + 4000]

    def campo(chiave, modello=r'"([^"]*)"'):
        m = re.search(r'\b' + chiave + r' = ' + modello, blocco)
        return m[1] if m else ''
    return {
        'id': campo('id'),
        'titolo': campo('titolo', r'\{ it = "([^"]*)"'),
        'teatro': campo('teatro'),
        'alfa': campo('alfa', r'(true|false)') == 'true',
        'situazione': campo('situazione'),
        'difficolta': campo('difficolta', r'(\d+)'),
        'durata_s': float(campo('durata_s', r'([0-9.]+)') or 0),
    }


def _ore(s):
    h, m = int(s // 3600), int(round(s % 3600 / 60))
    return f'{h} h' + (f' {m:02d}' if m else '')


def _missioni(cartella, dati):
    from manuale import glossario
    r = _rapide(dati)
    teatri = {k: glossario(t['nome']['it']) for k, t in r['teatri'].items()}
    out = []
    for f in sorted(cartella.glob('*.lua')):
        s = _scenario(f.read_text(encoding='utf-8'))
        if not s:
            continue
        s["titolo"] = glossario(s["titolo"])
        s['teatro'] = teatri.get(s['teatro'], s['teatro'])
        out.append(s)
    return out


def libreria_missioni(dati):
    """Le missioni della libreria (data/missioni/libreria), in ordine di codice"""
    from manuale import RADICE
    righe = ['| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Durata |', '|---|---|---|---|---|---|---|']
    for s in _missioni(RADICE / 'data' / 'missioni' / 'libreria', dati):
        m = re.match(r'LIB_([IS]\d+)_', s['id'])
        codice = f'LIB-{m[1]}' if m else s['id']
        righe.append(f"| {codice} | {s['titolo']} | {'Alfa' if s['alfa'] else '688'} | {s['teatro']} | {s['situazione']} | "
                     f"{s['difficolta']} | {_ore(s['durata_s'])} |")
    return '\n'.join(righe) + '\n'


def scene_prova(dati):
    """Le scene di prova delle interazioni fra unità dell'IA (data/missioni/prove), in ordine di file"""
    from manuale import RADICE
    righe = ['| File | Scena | Battello | Teatro | Situazione |', '|---|---|---|---|---|']
    for s in _missioni(RADICE / 'data' / 'missioni' / 'prove', dati):
        righe.append(f"| `{s['id']}` | {s['titolo']} | {'Alfa' if s['alfa'] else '688'} | {s['teatro']} | {s['situazione']} |")
    return '\n'.join(righe) + '\n'


TABELLE_MISSIONI = {'rapide_tipi_teatri': rapide_tipi_teatri, 'rapide_forze': rapide_forze,
                    'libreria_missioni': libreria_missioni, 'scene_prova': scene_prova}
