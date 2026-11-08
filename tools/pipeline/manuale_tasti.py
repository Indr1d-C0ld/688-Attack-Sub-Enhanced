"""I tasti per schermata del manuale (volume 1, appendice A): tabella scritta a mano, tasti e controllo dal codice.

La spiegazione di ogni tasto sta in tools/pipeline/tasti_schermate.toml, perche' il codice delle schermate non la dice.
I tasti invece vengono dalla fonte unica: un comando di data/motore/tasti_predefiniti.toml si scrive coi suoi tasti, e
un campo di Comandi (engine/piattaforma/include/piattaforma/finestra.hpp) coi tasti dei comandi che lo accendono in
applica() (engine/piattaforma/src/finestra.cpp). Se un tasto si rimappa, la tabella si rifà da sola.

Il controllo (manuale.py --controlla) legge le funzioni dei comandi di ogni schermata (le `funzioni` del file) e ne
raccoglie l'uso: i campi di Comandi letti dal parametro, le lettere fra apici, i comandi per nome confrontati con ==.
Ogni uso deve stare in una riga della schermata (`uso`), e ogni `uso` scritto deve esserci ancora nel codice.

Le voci di `uso` e `tasti`:
  campo:NOME     un campo di Comandi (cifre, tab, minuti, ruota...); nei tasti, i tasti che lo accendono
  lettera:X      la lettera X letta da Comandi::lettere
  comando:NOME   un comando di tasti_predefiniti.toml (confrontato per nome nel codice); nei tasti, i suoi tasti
  pulsanti:A-B   solo nei tasti: i tasti dei pulsanti della stazione da A a B (pulsante_A ... pulsante_B)
  fisso:TESTO    solo nei tasti: il testo com'e' (Markdown)
Una riga senza `tasti` scrive i tasti del suo `uso`.

Uso: manuale_tasti.py [--elenco | --schermata]   --elenco stampa l'uso trovato nel codice per ogni schermata (per scrivere il
file); --schermata scrive data/motore/comandi_schermata.toml, la schermata dei comandi in partita (F11)
"""
import pathlib
import re
import sys
import tomllib

RADICE = pathlib.Path(__file__).resolve().parents[2]
TABELLA = RADICE / 'tools' / 'pipeline' / 'tasti_schermate.toml'
FINESTRA_HPP = RADICE / 'engine' / 'piattaforma' / 'include' / 'piattaforma' / 'finestra.hpp'
FINESTRA_CPP = RADICE / 'engine' / 'piattaforma' / 'src' / 'finestra.cpp'
APP = RADICE / 'engine' / 'app'

# i campi che non sono tasti: la finestra chiusa, il puntatore e il trascinamento (si scrivono col clic)
NON_TASTI = {'esci', 'mouse', 'premuto', 'rilascio'}
# i campi che la finestra riempie da sé, senza un comando
CAMPI_FISSI = {
    'su_giu': '`Su`, `Giu`', 'ruota': '`Sinistra`, `Destra`', 'veloce': '`Maiusc` tenuto', 'cancella': '`Backspace`',
    'testo': 'la tastiera (si scrive)', 'clic': 'clic', 'clic_destro': 'clic destro', 'lettere': 'le lettere',
}


def campi_comandi():
    """I campi di struct Comandi"""
    corpo = FINESTRA_HPP.read_text(encoding='utf-8').split('struct Comandi {', 1)[1].split('\n};', 1)[0]
    campi = set()
    for riga in corpo.splitlines():
        riga = riga.split('//', 1)[0].strip()
        m = re.match(r'^(?:bool|int|std::string|std::vector<.*?>|std::pair<int, int>)\s+(.+);$', riga)
        if not m:
            continue
        for pezzo in re.sub(r'\{[^}]*\}', '', m[1]).split(','):
            campi.add(pezzo.split('=')[0].strip())
    return campi


def comandi_dei_campi():
    """campo di Comandi -> i comandi che lo accendono, da applica() in finestra.cpp"""
    testo = FINESTRA_CPP.read_text(encoding='utf-8').split('void applica(', 1)[1].split('\n}\n', 1)[0]
    mappa = {}
    for nome, campo in re.findall(r'k == "([a-z0-9_]+)"\) c\.([a-z_]+)', testo):
        mappa.setdefault(campo, []).append(nome)
    for nome in re.findall(r'\{"([a-z0-9_]+)", Azione::', testo):
        mappa.setdefault('azioni', []).append(nome)
    # i comandi numerati: k.rfind("tubo_", 0) == 0 ... c.tubi, k.rfind("pulsante_", 0) == 0 ... c.cifre
    for prefisso, campo in re.findall(r'k\.rfind\("([a-z_]+)", 0\) == 0\)\s*\{?\s*c\.([a-z_]+)', testo):
        mappa.setdefault(campo, []).extend(k for k in tasti_predefiniti() if k.startswith(prefisso))
    return mappa


def tasti_predefiniti():
    valori = tomllib.loads((RADICE / 'data' / 'motore' / 'tasti_predefiniti.toml').read_text(encoding='utf-8'))['tasti']
    return {k: [t for t in ([v] if isinstance(v, str) else v) if t] for k, v in valori.items()}


def corpo_funzione(file, nome):
    """Il corpo della definizione di Classe::funzione nel file (dalla firma alla graffa che chiude)"""
    righe = file.read_text(encoding='utf-8').splitlines()
    for i, riga in enumerate(righe):
        if re.match(rf'^\S.*\b{re.escape(nome)}\(', riga):
            profondita, aperta = 0, False
            for j in range(i, len(righe)):
                profondita += righe[j].count('{') - righe[j].count('}')
                aperta = aperta or '{' in righe[j]
                if aperta and profondita == 0:
                    return '\n'.join(righe[i:j + 1])
    raise ValueError(f'{file.name}: funzione {nome} non trovata')


def corpo_blocco(file, testo):
    """Il blocco fra graffe che comincia alla riga col testo (un ramo dentro una funzione lunga, come in main.cpp)"""
    righe = file.read_text(encoding='utf-8').splitlines()
    for i, riga in enumerate(righe):
        if testo in riga:
            profondita = 0
            for j in range(i, len(righe)):
                profondita += righe[j].count('{') - righe[j].count('}')
                if profondita <= 0 and '{' in ''.join(righe[i:j + 1]):
                    return '\n'.join(righe[i:j + 1])
    raise ValueError(f'{file.name}: blocco «{testo}» non trovato')


def uso_nel_codice(schermata, campi, comandi):
    """L'uso dei tasti nelle funzioni della schermata: {'campo:tab', 'lettera:P', 'comando:muto', ...}

    Una voce di `funzioni` e' «file:Classe::funzione» (i Comandi sono il parametro const Comandi&) oppure
    «file@testo#var» (il blocco che comincia alla riga col testo, coi Comandi nella variabile var)."""
    uso = set()
    for f in schermata['funzioni']:
        if '@' in f:
            file, resto = f.split('@', 1)
            testo, var = resto.rsplit('#', 1)
            corpo, variabili = corpo_blocco(APP / file, testo), [var]
        else:
            file, nome = f.split(':', 1)
            corpo = corpo_funzione(APP / file, nome)
            variabili = re.findall(r'const Comandi& (\w+)', corpo.split('{', 1)[0])
            variabili += re.findall(r'\bComandi (\w+) = ', corpo)   # una copia locale, che la funzione ritocca prima di usarla
        corpo = re.sub(r'//[^\n]*', '', corpo)   # i commenti non sono codice
        lettere = {f'lettera:{x}' for x in re.findall(r"'([A-Z])'", corpo)}
        for var in variabili:
            for campo in re.findall(rf'\b{var}\.(\w+)', corpo):
                campo = {'passi_su_giu': 'su_giu', 'passi_ruota': 'ruota'}.get(campo, campo)   # le stesse frecce, contate
                # le lettere e i comandi per nome contano uno per uno (lettera:X, comando:NOME); le lettere senza
                # una lettera precisa (TARGET: la lettera del segno) contano come campo
                if campo in campi and campo not in NON_TASTI and campo != 'nominati' and not (campo == 'lettere' and lettere):
                    uso.add(f'campo:{campo}')
        uso |= lettere
        uso |= {f'comando:{x}' for x in re.findall(r'== "([a-z0-9_]+)"', corpo) if x in comandi}
        # anche cercato fra i nominati: std::find(c.nominati.begin(), c.nominati.end(), "nome") != c.nominati.end()
        uso |= {f'comando:{x}' for x in re.findall(r'nominati\.end\(\), "([a-z0-9_]+)"\)', corpo) if x in comandi}
    return uso - {f'lettera:{x}' for x in schermata.get('ignora_lettere', '')}


def scrivi_tasti(voci, dei_campi, predefiniti):
    pezzi = []
    for v in voci:
        tipo, nome = v.split(':', 1)
        if tipo == 'fisso':
            pezzi.append(nome)
        elif tipo == 'lettera':
            pezzi.append(f'`{nome}`')
        elif tipo == 'pulsanti':   # pulsanti:1-8, i tasti dei pulsanti della stazione dal primo all'ultimo
            a, b = (predefiniti.get(f'pulsante_{n}', ['?'])[0] for n in nome.split('-'))
            pezzi.append(f'`{a}`-`{b}`')
        elif tipo == 'comando':
            pezzi.append(', '.join(f'`{t}`' for t in predefiniti.get(nome, [])) or '(nessun tasto)')
        elif tipo == 'campo' and nome in CAMPI_FISSI:
            pezzi.append(CAMPI_FISSI[nome])
        elif tipo == 'campo':
            tasti = [t for c in dei_campi.get(nome, []) for t in predefiniti.get(c, [])]
            pezzi.append(', '.join(f'`{t}`' for t in tasti) or '(nessun tasto)')
        else:
            raise ValueError(f'voce sconosciuta: {v}')
    return ', '.join(pezzi)


def carica():
    return tomllib.loads(TABELLA.read_text(encoding='utf-8'))['schermata']


def tasti_schermate(dati):
    """Le tabelle per schermata dell'appendice A"""
    dei_campi, predefiniti = comandi_dei_campi(), tasti_predefiniti()
    out = []
    for n, s in enumerate(carica(), start=2):   # A.1 e' la tabella dei comandi (manuale.py, tasti)
        out.append(f"### A.{n} {s['titolo']}\n")
        if s.get('nota'):
            out.append(s['nota'].strip() + '\n')
        gruppo = None
        for r in s['tasto']:
            if r.get('gruppo', '') != gruppo:
                if gruppo is not None:
                    out.append('')
                gruppo = r.get('gruppo', '')
                if gruppo:
                    out.append(f'**{gruppo}**\n')
                out += ['| Tasti | Che cosa fa |', '|---|---|']
            out.append(f"| {scrivi_tasti(r.get('tasti', r['uso']), dei_campi, predefiniti)} | {r['cosa'].strip()} |")
        out.append('')
    return '\n'.join(out)


def controlla_tasti():
    """I problemi fra il codice delle schermate e la tabella: lista vuota se tutto torna"""
    campi, comandi = campi_comandi(), set(tasti_predefiniti())
    problemi = []
    for s in carica():
        try:
            nel_codice = uso_nel_codice(s, campi, comandi)
        except (OSError, ValueError) as e:   # una funzione rinominata o spostata: va aggiornato `funzioni`
            problemi.append(f"tasti, {s['titolo']}: {e}")
            continue
        scritto = {u for r in s['tasto'] for u in r['uso']}
        for u in sorted(nel_codice - scritto):
            problemi.append(f"tasti, {s['titolo']}: {u} è nel codice ma non nel manuale")
        for u in sorted(scritto - nel_codice):
            problemi.append(f"tasti, {s['titolo']}: {u} è nel manuale ma non più nel codice")
    if not USCITA_SCHERMATA.exists() or USCITA_SCHERMATA.read_text(encoding='utf-8') != schermata_comandi():
        problemi.append('tasti: data/motore/comandi_schermata.toml non è aggiornato (manuale_tasti.py --schermata)')
    return problemi


# --- la schermata dei comandi in partita (F11) -------------------------------------------------------------------------

USCITA_SCHERMATA = RADICE / 'data' / 'motore' / 'comandi_schermata.toml'
# Le schede, nell'ordine dei tasti: l'id, l'etichetta, la stazione e i gruppi di «In mare» che ci vanno (per inizio del titolo)
SCHEDE = (
    ('F2', 'F2 RADIO', 2, ('Radio',)),
    ('F3', 'F3 STATO', 3, ('Stato',)),
    ('F4', 'F4 MANOVRA', 4, ('Manovra',)),
    ('F5', 'F5 PERISCOPIO', 5, ('Periscopio',)),
    ('F6', 'F6 CARTA', 6, ('Navigazione', 'Carta')),
    ('F7', 'F7 ARMI', 7, ('Armi',)),
    ('F8', 'F8 SONAR', 8, ('Sonar',)),
    ('OVUNQUE', 'OVUNQUE', 0, ('Da ogni stazione', 'Scelta del bersaglio', 'Alfa')),
)


def voci_schermata(voci, dei_campi):
    """Le voci dei tasti per il gioco, che li scrive dalla sua mappa (i tasti rimappati): comando:X, pulsanti:A-B, fisso:testo"""
    out = []
    for v in voci:
        tipo, nome = v.split(':', 1)
        if tipo == 'lettera':
            out.append(f'fisso:{nome}')
        elif tipo == 'fisso':
            out.append('fisso:' + nome.replace('`', ''))
        elif tipo == 'campo' and nome in CAMPI_FISSI:
            out.append('fisso:' + CAMPI_FISSI[nome].replace('`', ''))
        elif tipo == 'campo':
            out += [f'comando:{c}' for c in dei_campi.get(nome, [])]
        else:
            out.append(v)
    return out


def parti_cosa(cosa):
    """La riga divisa per modo di gioco: [(solo, testo)]. «classico: A; Enhanced: B» fa due righe; i rimandi al manuale si tolgono"""
    testo = re.sub(r' *\((?:cap\.|§)[^)]*\)', '', ' '.join(cosa.split())).strip()
    out = []
    for pezzo in re.split(r'; (?=(?:classico|Enhanced)\b)', testo):
        m = re.match(r'(classico|Enhanced)(?:, (688|Alfa))?: (.*)', pezzo)
        altra = re.match(r'(classico|Enhanced), ([^:]+: .*)', pezzo)   # «Enhanced, dalle altre stazioni: ...»
        if m:
            solo = m.group(2).lower() if m.group(2) else m.group(1).lower()
            out.append((solo, m.group(3)))
        elif altra:
            out.append((altra.group(1).lower(), altra.group(2)))
        else:
            out.append(('', pezzo))
    return out


def stringa_toml(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'


def schermata_comandi():
    """Il testo di data/motore/comandi_schermata.toml"""
    dei_campi = comandi_dei_campi()
    dati = tomllib.loads(TABELLA.read_text(encoding='utf-8'))
    in_mare = next(s for s in dati['schermata'] if s['titolo'] == 'In mare')
    righe = ['# Generato da tools/pipeline/manuale_tasti.py da tasti_schermate.toml: non modificare a mano.', '']
    for id_, etichetta, stazione, gruppi in SCHEDE:
        righe += ['[[scheda]]', f'id = "{id_}"', f'etichetta = "{etichetta}"', f'stazione = {stazione}', '']
        for c in dati.get('consiglio', []):
            if c['scheda'] == id_:
                righe += ['[[scheda.consiglio]]', f'solo = "{c.get("solo", "")}"', f'testo = {stringa_toml(" ".join(c["testo"].split()))}', '']
        visti = []
        for r in in_mare['tasto']:
            g = r.get('gruppo', '')
            if g.startswith(gruppi) and g not in visti:
                visti.append(g)
        for g in visti:
            titolo = re.sub(r' *\((?:Enhanced)[^)]*\)', '', g)
            solo_g = 'alfa' if g.startswith('Alfa') else 'enhanced' if 'Enhanced' in g else ''
            righe += ['[[scheda.gruppo]]', f'titolo = {stringa_toml(titolo)}', f'solo = "{solo_g}"', '']
            for r in in_mare['tasto']:
                if r.get('gruppo', '') != g:
                    continue
                voci = voci_schermata(r.get('tasti', r['uso']), dei_campi)
                for solo, testo in parti_cosa(r['cosa']):
                    righe += ['[[scheda.gruppo.riga]]', 'tasti = [' + ', '.join(stringa_toml(v) for v in voci) + ']',
                              f'solo = "{solo}"', f'cosa = {stringa_toml(testo)}', '']
    return '\n'.join(righe)


TABELLE_TASTI = {'tasti_schermate': tasti_schermate}

if __name__ == '__main__':
    if '--schermata' in sys.argv:
        USCITA_SCHERMATA.write_text(schermata_comandi(), encoding='utf-8')
    elif '--elenco' in sys.argv:
        campi, comandi = campi_comandi(), set(tasti_predefiniti())
        for s in carica():
            print(s['titolo'], ' '.join(sorted(uso_nel_codice(s, campi, comandi))))
    else:
        print('\n'.join(controlla_tasti()) or 'tasti: tutto torna')
