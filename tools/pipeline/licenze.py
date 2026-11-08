"""La cartella LICENZE/ del pacchetto di rilascio, da data/motore/crediti.toml (tools/pipeline/crediti.py).

Scrive in USCITA:
- CREDITI.txt: i crediti leggibili, nelle tre categorie di crediti.toml (originale, nostro, terzi). Per ogni opera di
  terzi che entra nel pacchetto: titolo, autore, licenza con l'indirizzo del testo, fonte, cosa e' stato cambiato e i
  file che ne derivano; in fondo l'elenco dei file sotto CC BY-SA, che restano sotto quella licenza;
- PROPRIETARI.txt: i file del gioco del 1989 (Electronic Arts) e, con --mt32, i brani resi con le ROM Roland MT-32
  (data/mt32/ nel pacchetto: il gioco li cerca accanto ai dati, prima della cache dell'utente):
  uso personale, non ridistribuibili. Con --base dice che quei file non ci sono;
- un file per ogni licenza del codice di terzi compilato nel gioco o messo in lib/ (Lua.txt, tomlplusplus.txt,
  ymfm.txt, SDL3.txt, sndio.txt) e dei caratteri incorporati nei PDF del manuale (Charis.txt, Liberation.txt,
  LiberationSansNarrow.txt, JetBrainsMono.txt, DejaVu.txt), copiato dai testi che stanno nel progetto
  (`testo_licenza` delle voci, nome in `file_licenza`). Un testo che manca ferma lo script: non si scarica;
- LEGGIMI.txt: l'indice della cartella.

Lo chiama tools/rilascio/pacchetto_linux.sh. Legge solo il progetto e scrive solo in USCITA.
Uso: tools/venv/bin/python -I tools/pipeline/licenze.py USCITA [--mt32 CARTELLA_DEI_WAV] [--base] [--radice R]
"""
import argparse, os, re, sys, textwrap, tomllib

LARGHEZZA = 100
TESTI_CC = {'by': 'https://creativecommons.org/licenses/by/{v}/', 'by-sa': 'https://creativecommons.org/licenses/by-sa/{v}/'}
# il pacchetto porta data/motore, data/suoni e data/missioni (tools/rilascio/pacchetto_linux.sh)
NEL_PACCHETTO = ('data/motore/', 'data/suoni/', 'data/missioni/')


def indirizzo_licenza(licenza):
    l = licenza.lower().replace('-by', ' by').replace('cc-', 'cc ')
    if 'cc0' in l:
        return 'https://creativecommons.org/publicdomain/zero/1.0/'
    m = re.search(r'cc by(-sa)? (\d\.\d)', l.replace('by sa', 'by-sa'))
    return TESTI_CC['by-sa' if m.group(1) else 'by'].format(v=m.group(2)) if m else ''


def a_capo(testo, rientro='  '):
    return textwrap.fill(testo, LARGHEZZA, initial_indent=rientro, subsequent_indent=rientro + '  ',
                         break_long_words=False, break_on_hyphens=False)


def elenco_file(derivati, rientro='    ', massimo=None):
    r = [f'{rientro}{d}' for d in derivati[:massimo]]
    if massimo and len(derivati) > massimo:
        r.append(f'{rientro}... e altri {len(derivati) - massimo}')
    return r


def nel_pacchetto(v, base):
    if base and v['categoria'] == 'originale':
        return []
    return [d for d in v['derivati'] if d.startswith(NEL_PACCHETTO)]


def modifiche_foto(v):
    m = []
    if any(d.endswith(('.spr', '.mdl')) for d in v['derivati']):
        m.append('ritagliata e proiettata come tessitura sul modello 3D e negli sprite della classe')
    if any(d.endswith('.img') for d in v['derivati']):
        m.append('ridotta a 320x200 in 16 grigi per il Museo')
    return '; '.join(m)


def testo_licenza(radice, voce):
    return '\n\n'.join(un_testo(radice, voce, t.strip()) for t in voce['testo_licenza'].split(' + '))


def un_testo(radice, voce, percorso):
    f = os.path.join(radice, percorso)
    if not os.path.isfile(f):
        sys.exit(f"{voce['titolo']}: manca il testo della licenza {percorso} (non si scarica: va messo nel progetto)")
    t = open(f, encoding='utf-8').read()
    if f.endswith('lua.h'):   # il riquadro della licenza in fondo all'header
        m = re.search(r'/\*{10,}\n(\* Copyright.*?)\*{10,}/', t, re.S)
        if not m:
            sys.exit('licenza di Lua non trovata in fondo a lua.h')
        t = re.sub(r'^\* ?', '', m.group(1), flags=re.M)
    if 'Permission' not in t and 'License' not in t and 'LICENSE' not in t:
        sys.exit(f"{percorso}: non sembra il testo di una licenza")
    return t.rstrip() + '\n'


def crediti(voci, base):
    r = ['688 ATTACK SUB - REMAKE (MELCHIOR): CREDITI', '=' * 42, '',
         a_capo('Generato da tools/pipeline/licenze.py da data/motore/crediti.toml, che resta nel pacchetto con tutte le '
                'voci (anche le fonti solo consultate). Tre categorie: il gioco originale del 1989, proprieta\' di '
                'Electronic Arts; il lavoro di questo progetto; le opere di terzi, ognuna con la sua licenza. Un file '
                'puo\' derivare da piu\' voci: i suoi obblighi si sommano.', ''), '']

    r += ['1. IL GIOCO ORIGINALE', '-' * 21, '',
          a_capo('688 Attack Sub, Electronic Arts, 1989 (copyright 1988). Software (c) 1988 John W. Ratcliff; progetto '
                 'di John W. Ratcliff, Paul Grace e Randall Breen; produttore Paul Grace; grafica di Michael Kosaka e '
                 'Wilfredo J. Aguilar; effetti sonori e musica dei titoli di Rob Hubbard; manuale di David Luoto. '
                 'Il remake non e\' affiliato a Electronic Arts.', ''), '']
    orig = [v for v in voci if v['categoria'] == 'originale']
    if base:
        r += [a_capo('I file ricavati dal gioco originale NON sono in questo pacchetto: vanno presi dalla copia del '
                     'giocatore. Quali sono: data/motore/crediti.toml, voci con categoria = "originale".', ''), '']
    else:
        r += [a_capo('Questo pacchetto contiene file ricavati dal gioco originale, per uso personale: l\'elenco e\' in '
                     'PROPRIETARI.txt.', ''), '']
        for v in orig:
            r.append(a_capo(f"- {v['titolo']}: {len(v['derivati'])} file", ''))
        r.append('')

    r += ['2. IL LAVORO DEL PROGETTO', '-' * 25, '',
          a_capo('Motore (bin/melchior688), strumenti della sandbox (bin/serie_sandbox, bin/serie_ab), manuale '
                 '(manuale/), dati, missioni, modelli 3D, sprite, font, musica FM ed effetti sintetizzati sono '
                 'fatti da melchior-688. Le musiche FM e i loro timbri sono dichiarati CC0. I numeri dei dati vengono '
                 'dai dossier di ricerca del progetto, con la fonte campo per campo.', ''), '']
    for v in voci:
        if v['categoria'] == 'nostro' and nel_pacchetto(v, base):
            r += [a_capo(f"- {v['titolo']} ({len(nel_pacchetto(v, base))} file). {v['nota']}", '')]
    r.append('')

    r += ['3. OPERE DI TERZI', '-' * 17, '']
    for titolo, gruppo in (('3.1 Codice', 'librerie'), ('3.1b Caratteri incorporati nei PDF del manuale', 'caratteri dei PDF')):
        r += [titolo, '']
        for v in voci:
            if v['gruppo'] == gruppo:
                file = f" Testo della licenza: {v['file_licenza']}." if v.get('file_licenza') else ''
                r += [a_capo(f"- {v['titolo']}, {v['autore']}. Licenza {v['licenza']}. {v['url']}. Uso: {v['uso']}.{file}",
                             ''), '']

    terzi = [v for v in voci if v['categoria'] == 'terzi' and v['gruppo'] not in ('librerie', 'caratteri dei PDF')
             and nel_pacchetto(v, base)]
    sezioni = [('3.2 Foto e disegni', 'foto e disegni del kit', modifiche_foto),
               ('3.3 Suoni', 'suoni e musica', None),
               ('3.4 Batimetria, oceanografia, geografia e meteo', ('batimetria e oceanografia', 'meteo e clima'),
                'elaborati nei fondali, nelle carte, nei profili del suono e nel meteo del gioco')]
    for titolo, gruppi, cambiato in sezioni:
        gruppi = (gruppi,) if isinstance(gruppi, str) else gruppi
        sel = [v for v in terzi if v['gruppo'] in gruppi]
        obbligo = [v for v in sel if v['attribuzione'] or v['condivisione']]
        liberi = [v for v in sel if not (v['attribuzione'] or v['condivisione'])]
        r += [titolo, '']
        for v in obbligo:
            ind = indirizzo_licenza(v['licenza'])
            r += [a_capo(f"- {v['titolo']}", ''), a_capo(f"Autore: {v['autore']}"),
                  a_capo(f"Licenza: {v['licenza']}" + (f' ({ind})' if ind else '')),
                  *([a_capo(f"Fonte: {v['url']}")] if v['url'] else []),
                  a_capo(f"Modificato: {cambiato(v) if callable(cambiato) else cambiato or v['uso']}"),
                  *([a_capo(v['nota'])] if v['nota'] else []), '  File:', *elenco_file(nel_pacchetto(v, base)), '']
        if liberi:
            r += [a_capo(f'Pubblico dominio e CC0 ({len(liberi)} opere): nessun obbligo, si citano per correttezza.', ''), '']
            for v in liberi:
                fonte = f" <{v['url']}>" if v['url'] and ' ' not in v['url'] else ''
                r.append(a_capo(f"- {v['titolo']}. {v['autore'] or 'autore non indicato'}. {v['licenza']}.{fonte} "
                                f"File: {', '.join(nel_pacchetto(v, base))}", ''))
            r.append('')
    mt32 = [] if base else [v for v in voci if v['gruppo'] == 'suoni e musica' and v['stato'] == 'proprietario']
    for v in mt32:
        r += [a_capo(f"- {v['titolo']}: {v['autore']}. {v['nota']}.", ''), '']

    sa = {}
    for v in terzi:
        if v['condivisione']:
            for d in nel_pacchetto(v, base):
                sa.setdefault(d, []).append(f"{v['licenza']}: {v['titolo']}")
    r += ['3.5 File sotto CC BY-SA', '',
          a_capo('Questi file derivano da opere CC BY-SA: restano sotto la stessa licenza (la versione piu\' alta fra le '
                 'loro fonti, o una successiva), con l\'attribuzione qui sopra. Il resto del pacchetto non ne e\' toccato.',
                 ''), '']
    for d in sorted(sa):
        r += [f'  {d}', *[a_capo(s, '      ') for s in sa[d]]]
    r.append('')
    cons = sum(1 for v in voci if v['uso'] == 'consultata')
    r += ['4. FONTI CONSULTATE', '-' * 19, '',
          a_capo(f'{cons} foto e disegni sono serviti solo per misure e verifiche: nessun file del pacchetto ne deriva. '
                 'Sono in data/motore/crediti.toml con uso = "consultata".', ''), '']
    return '\n'.join(r)


def proprietari(voci, base, mt32):
    r = ['FILE PROPRIETARI: USO PERSONALE, NON DISTRIBUIRE', '=' * 48, '']
    orig = [v for v in voci if v['categoria'] == 'originale']
    if base:
        r += [a_capo('Questo pacchetto NON contiene file del gioco originale ne\' brani resi con le ROM MT-32.', ''), '']
        return '\n'.join(r)
    n = sum(len(v['derivati']) for v in orig)
    r += [a_capo(f'{n} file ricavati dal gioco originale (688 Attack Sub, Electronic Arts, 1989; software (c) 1988 John '
                 'W. Ratcliff). Sono qui solo per uso personale di chi possiede il gioco: non si ridistribuiscono.', ''), '']
    for v in orig:
        r += [v['titolo'], a_capo(f"autori: {v['autore']}"), a_capo(f"nota: {v['nota']}"), '  file:',
              *elenco_file(v['derivati']), '']
    if mt32:
        wav = sorted(f for f in os.listdir(mt32) if f.endswith('.wav'))
        v = next(v for v in voci if v['gruppo'] == 'suoni e musica' and v['stato'] == 'proprietario')
        r += [v['titolo'], a_capo(f"autori: {v['autore']}"), a_capo(f"nota: {v['nota']}"),
              a_capo(f'{len(wav)} brani, nella cartella data/mt32/ del pacchetto:', '  ')] + elenco_file([f'data/mt32/{w}' for w in wav]) + ['']
    return '\n'.join(r)


def leggimi(scritti, base):
    r = ['LICENZE E CREDITI', '=' * 17, '',
         '  CREDITI.txt               crediti di tutto il pacchetto: gioco originale, progetto, opere di terzi',
         '  PROPRIETARI.txt           ' + ('nessun file proprietario in questo pacchetto' if base else
                                  'i file del gioco originale e i brani MT-32: uso personale')]
    for nome, (voce, _) in scritti.items():
        r.append(f"  {nome:<25} {voce['titolo']} ({voce['licenza']}): {voce['uso']}")
    return '\n'.join(r + ['']) + '\n'


def main():
    a = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    a.add_argument('uscita')
    a.add_argument('--mt32', help='la cartella dei WAV MT-32 messi nel pacchetto (solo nel privato)')
    a.add_argument('--base', action='store_true', help='pacchetto senza i file dell\'originale e senza MT-32')
    a.add_argument('--radice', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
    o = a.parse_args()
    if o.base and o.mt32:
        sys.exit('--base e --mt32 insieme: i brani MT-32 vanno solo nel pacchetto privato')
    radice = os.path.normpath(o.radice)
    voci = tomllib.load(open(os.path.join(radice, 'data/motore/crediti.toml'), 'rb'))['asset']
    scritti = {}
    for v in voci:
        if v.get('testo_licenza'):
            scritti[v['file_licenza']] = (v, testo_licenza(radice, v))
    os.makedirs(o.uscita, exist_ok=True)
    for nome, (_, t) in scritti.items():
        open(os.path.join(o.uscita, nome), 'w', encoding='utf-8').write(t)
    open(os.path.join(o.uscita, 'CREDITI.txt'), 'w', encoding='utf-8').write(crediti(voci, o.base))
    open(os.path.join(o.uscita, 'PROPRIETARI.txt'), 'w', encoding='utf-8').write(proprietari(voci, o.base, o.mt32))
    open(os.path.join(o.uscita, 'LEGGIMI.txt'), 'w', encoding='utf-8').write(leggimi(scritti, o.base))
    print(f"licenze: {len(scritti) + 3} file in {o.uscita}" + (' (base)' if o.base else ''))


if __name__ == '__main__':
    main()
