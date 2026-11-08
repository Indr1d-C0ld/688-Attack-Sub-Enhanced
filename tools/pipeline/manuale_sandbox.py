"""Le tabelle del manuale per la sandbox (volume 1, capitolo 12): i vetrini pronti da data/missioni/prove/vetrini.toml.

Il file lo scrive strumenti/scene_interazioni (trailer-1) insieme alle scene; qui si legge e basta, cosi' il manuale
segue i vetrini quando cambiano titoli, testi o stato."""
import pathlib
import re
import tomllib

RADICE = pathlib.Path(__file__).resolve().parents[2]
VETRINI = RADICE / 'data' / 'missioni' / 'prove' / 'vetrini.toml'


def accenti(s):
    """I testi dei dati scrivono gli accenti finali con l'apostrofo (unita', e'): nel manuale le lettere accentate"""
    return re.sub(r"\b(\w*?)([aeiou])'(?=[\s.,;:!?)]|$)",
                  lambda m: m[1] + {'a': 'à', 'e': 'è', 'i': 'ì', 'o': 'ò', 'u': 'ù'}[m[2]], s)


def vetrini_pronti(dati):
    v = tomllib.loads(VETRINI.read_text(encoding='utf-8')).get('vetrino', [])
    out = ['| Vetrino | Che cosa mostra | Durata |', '|---|---|---|']
    for x in v:
        titolo = x['titolo']['it'] + ('' if x.get('pronto', True) else ' *(in ritaratura: non ancora nella libreria)*')
        out.append(f"| {accenti(titolo)} | {accenti(x['mostra']['it'])} | {x.get('durata_min', 0)} min |")
    return '\n'.join(out) + '\n'


TABELLE_SANDBOX = {'vetrini_pronti': vetrini_pronti}
