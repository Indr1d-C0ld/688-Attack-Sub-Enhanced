#!/usr/bin/env python3
"""Riferimenti visivi del dossier 25 da MobyGames, attraverso le copie di archive.org (il sito vivo chiede la verifica
anti-bot). Solo immagini, solo come riferimento interno: mai nel gioco (permesso dell'utente del 28/10/2026, via
principale).

  riferimenti_moby.py elenca              le schermate di ogni titolo (didascalia dal nome del file)
  riferimenti_moby.py scarica SCELTE.toml  scarica le scelte in docs/ricerca/25_riferimenti/ e scrive indice.md
"""
import json
import re
import subprocess
import sys
import time
import tomllib
from pathlib import Path

RADICE = Path(__file__).resolve().parents[2]
CARTELLA = RADICE / 'docs' / 'ricerca' / '25_riferimenti'
AGENTE = 'curl/8.5.0'
PAUSA_S = 6   # fra due richieste

# titolo, editore, anno, piattaforma e nome nella vecchia forma degli indirizzi di MobyGames
TITOLI = {
    'aces-of-the-pacific': ('Aces of the Pacific', 'Dynamix / Sierra On-Line', 1992, 'dos'),
    'aces-of-the-deep': ('Aces of the Deep', 'Dynamix / Sierra On-Line', 1994, 'dos'),
    'red-baron': ('Red Baron', 'Dynamix / Sierra On-Line', 1990, 'dos'),
    'f-15-strike-eagle-iii': ('F-15 Strike Eagle III', 'MicroProse', 1992, 'dos'),
    'us-navy-fighters': ('U.S. Navy Fighters', 'Electronic Arts', 1994, 'dos'),
    'strike-commander': ('Strike Commander', 'Origin Systems', 1993, 'dos'),
    'falcon-30': ('Falcon 3.0', 'Spectrum HoloByte', 1991, 'dos'),
    '688i-hunterkiller': ('688(I) Hunter/Killer', 'Electronic Arts / Sonalysts', 1997, 'windows'),
}


def leggi(url, binario=False):
    # curl: urllib riceve 403 da archive.org
    for prova in range(4):
        time.sleep(PAUSA_S)
        r = subprocess.run(['curl', '-sfL', '--max-time', '90', '-A', AGENTE, url], capture_output=True)
        if r.returncode == 0:
            return r.stdout if binario else r.stdout.decode('utf-8', 'replace')
        if prova == 3:
            raise RuntimeError(f'curl {r.returncode} su {url}')
        time.sleep(30 * (prova + 1))   # rifiutato: archive.org vuole meno fretta


def pagina(slug):
    piatt = TITOLI[slug][3]
    orig = f'https://www.mobygames.com/game/{piatt}/{slug}/screenshots'
    info = leggi(f'https://archive.org/wayback/available?url={orig}&timestamp=2022')
    vicina = json.loads(info).get('archived_snapshots', {}).get('closest')
    if not vicina:
        return orig, None, []
    copia = vicina['url']
    try:
        html = leggi(copia)
    except Exception as e:
        print(f'  {slug}: copia illeggibile ({e})', file=sys.stderr)
        return orig, None, []
    ts = re.search(r'/web/(\d+)', copia).group(1)
    scatti = re.findall(r'images/shots/s/(\d+)-([^"\'):]+?)\.(jpg|png|gif)', html)
    visti, righe = set(), []
    for ident, nome, est in scatti:
        if ident in visti:
            continue
        visti.add(ident)
        didascalia = re.sub(r'^.*?-(dos|windows)-screenshot-', '', nome).replace('-', ' ')
        righe.append((ident, didascalia, ts, f'https://www.mobygames.com/images/shots/l/{ident}-{nome}.{est}'))
    return orig, copia, righe


def elenca():
    for slug in TITOLI:
        orig, copia, righe = pagina(slug)
        print(f'## {slug}  ({copia or "nessuna copia"})')
        for ident, did, _, _ in righe:
            print(f'  {ident}  {did}')


def scarica(scelte_toml):
    scelte = tomllib.loads(Path(scelte_toml).read_text())
    CARTELLA.mkdir(parents=True, exist_ok=True)
    # l'indice e' cumulativo: le righe dei passaggi precedenti restano (archive.org limita le richieste)
    righe_indice = {}
    if (CARTELLA / 'indice.md').exists():
        for riga in (CARTELLA / 'indice.md').read_text().splitlines():
            m = re.match(r'\| \[([^\]]+)\.(?:jpg|png|gif)\]', riga)
            if m:
                righe_indice[m.group(1)] = riga
    for slug, ids in scelte.items():
        titolo, editore, anno, _ = TITOLI[slug]
        orig, copia, righe = pagina(slug)
        per_id = {r[0]: r for r in righe}
        for ident in ids:
            ident = str(ident)
            if ident not in per_id:
                print(f'saltata: {slug} {ident} (pagina del titolo non letta: riprovare)')
                continue
            _, did, ts, url_l = per_id[ident]
            pagina_scatto = f'https://www.mobygames.com/game/{TITOLI[slug][3]}/{slug}/screenshots/gameShotId,{ident}/'
            gia = [f for f in CARTELLA.glob(f'{slug}_{ident}.*') if f.stat().st_size > 8000]
            if gia:   # la versione grande c'e' gia': si ripete solo quello che manca
                righe_indice.setdefault(f'{slug}_{ident}', f'| [{gia[0].name}]({gia[0].name}) | *{titolo}* | {editore} | '
                                        f'{anno} | {did} | [pagina]({pagina_scatto}) (copia: {copia}) |')
                continue
            dati = None
            # la versione grande sta nella pagina della singola schermata (spesso .gif, non .jpg come la miniatura)
            candidati = []
            try:
                info = json.loads(leggi(f'https://archive.org/wayback/available?url={pagina_scatto}'))
            except Exception:
                info = {}
            vicina = info.get('archived_snapshots', {}).get('closest')
            if vicina:
                try:
                    html = leggi(vicina['url'])
                    candidati += [('https://web.archive.org' + u if u.startswith('/') else u) for u in
                                  re.findall(r'(?:https?://web\.archive\.org)?/web/\d+im_/[^"\' )]*images/shots/l/'
                                             + ident + r'-[^"\' )]+', html)]
                except Exception:
                    pass
            candidati += [f'https://web.archive.org/web/{ts}im_/{url_l}',
                          f'https://web.archive.org/web/{ts}im_/{url_l.replace("/shots/l/", "/shots/s/")}']
            for url in candidati:
                try:
                    dati = leggi(url, binario=True)
                    break
                except Exception:
                    continue
            if not dati or not (dati[:3] == b'\xff\xd8\xff' or dati[:4] == b'\x89PNG' or dati[:3] == b'GIF'):
                print(f'saltata: {slug} {ident} (non e\' un\'immagine)')
                continue
            est = 'jpg' if dati[:3] == b'\xff\xd8\xff' else 'png' if dati[:4] == b'\x89PNG' else 'gif'
            nome = f'{slug}_{ident}.{est}'
            for vecchia in CARTELLA.glob(f'{slug}_{ident}.*'):
                vecchia.unlink()
            (CARTELLA / nome).write_bytes(dati)
            grande = '' if len(dati) > 8000 else ' (solo la miniatura)'
            righe_indice[f'{slug}_{ident}'] = (f'| [{nome}]({nome}) | *{titolo}* | {editore} | {anno} | {did}{grande} | '
                                               f'[pagina]({pagina_scatto}) (copia: {copia}) |')
            print(f'scaricata: {nome} ({len(dati)} byte)')
            time.sleep(1)
    testo = [
        '# Riferimenti visivi del dossier 25 (solo uso interno)',
        '',
        'Schermate di MobyGames scaricate il 28/10/2026 col permesso dell\'utente, attraverso le copie di archive.org',
        '(il sito vivo chiede una verifica anti-bot). Servono solo come riferimento per le bozze della passata estetica',
        '(roadmap 33): **non entrano nel gioco** e non si ridistribuiscono. Le immagini sono dei rispettivi editori; le',
        'catture sono dei contributori di MobyGames. Scaricate da `tools/estetica/riferimenti_moby.py`.',
        '',
        '| File | Titolo | Editore | Anno | Didascalia | Fonte |',
        '|---|---|---|---|---|---|',
        *[r for k, r in sorted(righe_indice.items()) if list(CARTELLA.glob(k + '.*'))],
        '',
    ]
    (CARTELLA / 'indice.md').write_text('\n'.join(testo))


if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'elenca':
        elenca()
    elif len(sys.argv) >= 3 and sys.argv[1] == 'scarica':
        scarica(sys.argv[2])
    else:
        print(__doc__)
