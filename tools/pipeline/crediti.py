"""Crediti e licenze degli asset del progetto: data/motore/crediti.toml (per la schermata dei crediti e il Museo) e
docs/CREDITI.md (leggibile).

Raccoglie:
- foto e disegni del kit (extracted/**/fonti.json e extracted/navi/*/foto/crediti.json). Una foto e' «derivata» quando una classe la usa nelle sue `foto:`
  (tools/navi/classi/*.yaml): allora la sua tessitura finisce negli sprite (data/motore/navi/<classe>.spr) e nel
  modello 3D (data/motore/modelli/<classe>.mdl) di quella classe. Le altre sono solo consultate (misure, verifiche)
  o scartate: si citano, ma non impongono obblighi;
- batimetria e oceanografia dei teatri (data/teatri/*/heightmap.json, ssp_mensile.json) e i file del motore che ne
  derivano (fondali, carte d'epoca, profili del suono);
- gli asset dell'originale del 1989 che l'esportazione copia dai dischi (font, immagini, palette, testi, missioni
  convertite): proprieta' dell'editore, non ridistribuibili;
- le librerie di terze parti del motore e degli strumenti.

Per ogni voce: file derivati, autore, licenza, URL, obblighi (attribuzione, condivisione allo stesso modo), la
`categoria` (nostro: fatto dal progetto; terzi: opera altrui con la sua licenza; originale: del gioco del 1989, proprieta'
dell'editore) e uno `stato`: ok (pubblico dominio, CC0, dati liberi), attribuzione (CC BY), condivisione (CC BY-SA: i file derivati
vanno distribuiti con la stessa licenza), da_verificare (licenza mancante, GFDL o non riconosciuta), proprietario
(non ridistribuibile: va estratto dalla copia dell'utente).

Ogni file di data/motore, data/suoni e data/missioni (cio' che va nel pacchetto) deve stare fra i derivati di almeno
una voce, e ogni derivato deve esistere: altrimenti lo script si ferma con l'elenco (07/11/2026, crediti del pacchetto).

Uso: tools/venv/bin/python -I tools/pipeline/crediti.py [radice]
"""
import glob, json, os, re, sys
import yaml

RADICE = sys.argv[1] if len(sys.argv) > 1 else '.'
MOTORE = f'{RADICE}/data/motore'
CARTELLE_INVERSE = {'oscar2': 'oscar'}   # cartella di extracted/navi -> classe del motore (esporta_motore.CARTELLE)


def classifica(licenza):
    """(tipo, stato, attribuzione obbligatoria, condivisione allo stesso modo) dalla licenza dichiarata."""
    l = (licenza or '').strip().lower()
    if not l or l == '?':
        return 'ignota', 'da_verificare', False, False
    if 'by-sa' in l or 'by sa' in l:
        return licenza.strip(), 'condivisione', True, True
    if 'cc0' in l:
        return 'CC0', 'ok', False, False
    if 'public domain' in l or 'pubblico dominio' in l or 'pd-' in l or 'governo usa' in l or 'government' in l:
        return 'pubblico dominio', 'ok', False, False
    if 'cc by' in l or 'cc-by' in l:
        return licenza.strip(), 'attribuzione', True, False
    if 'gfdl' in l:   # immagini sotto GFDL: il testo intero della licenza va distribuito con l'opera derivata
        return licenza.strip(), 'da_verificare', True, True
    return licenza.strip(), 'da_verificare', True, False


def pulisci(s, n=200):
    s = re.sub(r'<[^>]*>', '', str(s or '')).replace('\n', ' ').strip()
    s = re.sub(r'^(.+?)\1$', r'\1', s)   # «Unknown authorUnknown author»
    return s[:n].rstrip('. ')


def classi_del_kit():
    """file di foto (percorso relativo) -> classi del motore che lo proiettano."""
    uso = {}
    for f in sorted(glob.glob(f'{RADICE}/tools/navi/classi/*.yaml')):
        try:
            cl = yaml.safe_load(open(f))
        except yaml.YAMLError:
            continue
        if not isinstance(cl, dict) or 'uscita' not in cl:
            continue
        cartella = os.path.basename(os.path.normpath(cl['uscita']))
        classe = CARTELLE_INVERSE.get(cartella, cartella)
        for fc in cl.get('foto') or []:
            if isinstance(fc, dict) and fc.get('file'):
                uso.setdefault(os.path.normpath(fc['file']), set()).add(classe)
    return uso


# versioni piene, in miniatura da 1920 px, ritagliate o corrette della stessa foto: il kit proietta l'una, le fonti
# registrano l'altra (krivak2 _full, sacramento _1920 e _ritaglio, spruance _corretta)
SUFFISSI_FOTO = re.compile(r'_(full|1920|ritaglio|corretta)(?=(_[a-z0-9]+)*\.[^.]+$)')


def canonica(percorso, note):
    """Il file delle fonti di una foto del kit: krivak2/foto/prua_dritta_1983_full.jpg -> prua_dritta_1983.jpg."""
    if percorso in note:
        return percorso
    chiave = lambda f: os.path.splitext(SUFFISSI_FOTO.sub('', f))[0]
    trovate = [n for n in note if chiave(n) == chiave(percorso)]
    return trovate[0] if len(trovate) == 1 else None


def derivati_di(classi):
    out = []
    for c in sorted(classi):
        for p in (f'navi/{c}.spr', f'modelli/{c}.mdl'):
            if os.path.exists(f'{MOTORE}/{p}'):
                out.append(f'data/motore/{p}')
    return out


def voci_foto():
    uso = classi_del_kit()
    voci = []
    sorgenti = []
    for fj in sorted(glob.glob(f'{RADICE}/extracted/**/fonti.json', recursive=True)):
        d = json.load(open(fj))
        if isinstance(d, dict):
            d = d.get('foto', [d])
        sorgenti.append((fj, d))
    # le foto dei lotti delle firme nuove (tools/navi/scarica_foto.py): {"File:titolo": {file, autore, licenza, url}}
    for fj in sorted(glob.glob(f'{RADICE}/extracted/navi/*/foto*/crediti.json')):   # anche foto_kit/ (petroliera costiera)
        sorgenti.append((fj, [dict(file=v['file'], titolo_commons=k, pagina=v['url'], licenza=v['licenza'],
                                   autore=v['autore']) for k, v in json.load(open(fj)).items()]))
    note = {os.path.normpath(f"{os.path.relpath(os.path.dirname(fj), RADICE)}/{x.get('file') or x.get('file_usato')}")
            for fj, d in sorgenti for x in d if x.get('file') or x.get('file_usato')}
    uso_canonico, orfane = {}, []
    for f, cl in uso.items():
        c = canonica(f, note)
        if c is None:
            orfane.append(f)
        else:
            uso_canonico.setdefault(c, set()).update(cl)
    if orfane:
        sys.exit('foto proiettate dal kit senza voce nelle fonti: ' + ', '.join(sorted(orfane)))
    for fj, d in sorgenti:
        cartella = os.path.relpath(os.path.dirname(fj), RADICE)
        for x in d:
            nome = x.get('file') or x.get('file_usato')
            percorso = os.path.normpath(f'{cartella}/{nome}') if nome else None
            classi = uso_canonico.get(percorso, set()) if percorso else set()
            if x.get('usata') is False:
                classi = set()
            tipo, stato, attr, sa = classifica(x.get('licenza') or x.get('license'))
            derivati = derivati_di(classi)
            voci.append(dict(
                gruppo='foto e disegni del kit', titolo=pulisci(x.get('titolo_commons') or x.get('fonte') or nome, 160),
                file=percorso or '', url=x.get('pagina') or x.get('url_file') or '', autore=pulisci(x.get('autore') or x.get('credito')),
                licenza=tipo, attribuzione=attr, condivisione=sa, uso='derivata' if derivati else 'consultata',
                derivati=derivati, stato=stato if derivati else 'consultata',
                nota=pulisci(x.get('nota_licenza') or x.get('uso') or '', 240)))
    foto_enciclopedia(voci)
    return voci


def foto_enciclopedia(voci):
    """Le foto digitalizzate «alla 1989» del Museo (data/motore/immagini/enciclopedia/<id>_fotoK.img) vengono dalle foto
    del kit: la K-esima con licenza libera di fonti.json, con le regole di tools/pipeline/enciclopedia.py
    (immagini_e_crediti), che qui si ripetono. Ogni immagine si aggiunge ai derivati della sua foto; autore e licenza
    devono essere quelli che enciclopedia.toml scrive accanto all'immagine."""
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from enciclopedia import LICENZE_LIBERE, cartella_classe
    import tomllib
    per_file = {v['file']: v for v in voci if v['file']}
    enc = tomllib.load(open(f'{MOTORE}/enciclopedia.toml', 'rb'))['voci']
    for img in sorted(glob.glob(f'{MOTORE}/immagini/enciclopedia/*.img')):
        vid, k = os.path.basename(img)[:-4].rsplit('_foto', 1)
        fonti = f'{RADICE}/extracted/navi/{cartella_classe(vid.split(".", 1)[1])}/foto/fonti.json'
        scelte = [x for x in json.load(open(fonti)) if x.get('file')
                  and any(l in (x.get('licenza') or '').lower() for l in LICENZE_LIBERE)
                  and os.path.exists(os.path.join(os.path.dirname(fonti), x['file']))]
        x = scelte[int(k) - 1]
        rel = os.path.relpath(img, RADICE)
        scritta = [i for i in enc[vid]['immagini'] if i.get('file') == rel.removeprefix('data/motore/')]
        if not scritta or (scritta[0].get('autore'), scritta[0].get('licenza')) != (x.get('autore', ''), x.get('licenza', '')):
            sys.exit(f'{rel}: la foto di partenza non e\' quella che dice enciclopedia.toml')
        v = per_file[os.path.normpath(os.path.relpath(os.path.join(os.path.dirname(fonti), x['file']), RADICE))]
        v['derivati'] = sorted(set(v['derivati']) | {rel})
        if v['uso'] == 'consultata':   # la foto non e' nelle tessiture, ma il Museo la mostra digitalizzata
            tipo, stato, _, _ = classifica(x.get('licenza'))
            v.update(uso='derivata', stato=stato)


def voci_teatri():
    voci = {}
    for hj in sorted(glob.glob(f'{RADICE}/data/teatri/*/heightmap.json')):
        t = os.path.basename(os.path.dirname(hj))
        derivati = [p for p in (f'data/motore/teatri/{t}.fdl', f'data/motore/teatri/{t}_carta_nato.fdl',
                                f'data/motore/teatri/{t}_carta_urss.fdl') if os.path.exists(f'{RADICE}/{p}')]
        for f in json.load(open(hj)).get('fonti', []):
            k = f.get('nome')
            v = voci.setdefault(k, dict(gruppo='batimetria e oceanografia', titolo=k, file='', url=f.get('accesso', ''),
                                        autore=f.get('attribuzione', ''), derivati=[], licenza_grezza=f.get('licenza', '')))
            v['derivati'] += derivati
            if 'ETOPO' in k:   # le carte delle basi del Museo (carte_basi.py), una volta sola
                v['derivati'] += [p for p in sorted(f'data/motore/basi/{os.path.basename(x)}' for x in glob.glob(f'{MOTORE}/basi/*.fdl')) if p not in v['derivati']]
        sj = f'{RADICE}/data/teatri/{t}/ssp_mensile.json'
        if os.path.exists(sj):
            fonte = json.load(open(sj)).get('fonte', 'NOAA World Ocean Atlas 2023')
            v = voci.setdefault('woa23', dict(gruppo='batimetria e oceanografia', titolo=fonte, file='',
                                              url='https://www.ncei.noaa.gov/products/world-ocean-atlas',
                                              autore='NOAA NCEI, World Ocean Atlas 2023', derivati=[],
                                              licenza_grezza='dati del governo USA, uso libero'))
            if os.path.exists(f'{MOTORE}/teatri/{t}.suono.toml'):
                v['derivati'].append(f'data/motore/teatri/{t}.suono.toml')
        if os.path.exists(f'{RADICE}/data/teatri/{t}/woa23_8594_stagionale.nc'):   # l'epoca del gioco (fetch_woa_8594.py)
            v8 = voci.setdefault('woa23_8594', dict(
                gruppo='batimetria e oceanografia',
                titolo='NOAA World Ocean Atlas 2023, decennio 1985-1994 (8594): mensile a 1 grado, stagionale a 0.25 gradi',
                file='', url='https://www.ncei.noaa.gov/products/world-ocean-atlas',
                autore='NOAA NCEI, World Ocean Atlas 2023 (Locarnini et al. 2024, vol. 1 Temperature, NOAA Atlas NESDIS 89; '
                       'Reagan et al. 2024, vol. 2 Salinity, NOAA Atlas NESDIS 90)',
                derivati=[], licenza_grezza='dati del governo USA, uso libero'))
            st = f'{MOTORE}/teatri/{t}.suono.toml'   # derivato quando il profilo del motore viene dal decennio (suono.py)
            if os.path.exists(st) and '1985-1994' in ''.join(open(st).readlines()[:3]):
                v8['derivati'].append(f'data/motore/teatri/{t}.suono.toml')
    out = []
    for v in voci.values():
        tipo, stato, attr, sa = classifica(v.pop('licenza_grezza'))
        derivati = sorted(set(v['derivati']))   # senza derivati (scaricata, non ancora usata): consultata
        out.append(dict(v, licenza=tipo, attribuzione=attr, condivisione=sa, uso='derivata' if derivati else 'consultata',
                        stato=stato if derivati else 'consultata', derivati=derivati, nota=''))
    return out


# gli autori del 1989, dai crediti della scatola e del manuale (docs/manuale_enhanced/3_storia/01_gioco_1989.md):
# «Software (c) 1988 John Ratcliff ... (c) 1988 Electronic Arts»
EA = 'Electronic Arts (1988-1989)'
NON_RIDISTRIBUIBILE = ('Copiati o convertiti dai file del gioco originale: non ridistribuibili. Vanno estratti dalla copia '
                       'del giocatore al primo avvio (o forniti a parte da chi ne ha i diritti)')


def voci_originale():
    """Asset copiati dai dischi del gioco del 1989 (tools/pipeline/esporta_motore.py legge extracted/raw/DISK1) e file
    ricavati da quelli. Elenchi esatti, non cartelle intere: in font/, immagini/ e musica/ ci sono anche i file del
    progetto (font delle stazioni, foto del Museo, musica FM), che hanno le loro voci (07/11/2026)."""
    def rel(*modelli):
        return sorted(os.path.relpath(p, RADICE) for m in modelli for p in glob.glob(f'{RADICE}/{m}'))
    voce = dict(gruppo='asset dell\'originale', url='', licenza='proprietaria', attribuzione=True, condivisione=False,
                uso='derivata', stato='proprietario', categoria='originale')
    return [dict(voce, titolo='688 Attack Sub (1989): font, schermate dei titoli, palette e testi di bordo',
                 file='extracted/raw/DISK1', autore=f'software di John W. Ratcliff, grafica di Michael Kosaka e Wilfredo J. '
                 f'Aguilar; {EA}', nota=NON_RIDISTRIBUIBILE,
                 derivati=rel('data/motore/font/originale.fnt', 'data/motore/immagini/MAIN.*', 'data/motore/immagini/TITLE.*',
                              'data/motore/palette/*', 'data/motore/testi/*')),
            dict(voce, titolo='688 Attack Sub (1989): scenari delle missioni (SCENE*.LZA) convertiti in Lua', file='extracted/raw',
                 autore=f'John W. Ratcliff, Paul Grace e Randall Breen (progetto); {EA}', derivati=rel('data/missioni/SCENE*.lua'),
                 nota='Conversione degli scenari originali: stessi vincoli dei file da cui derivano'),
            # la musica dei titoli (tools/pipeline/musica.py) e i campioni del classico (tools/pipeline/suoni_classici.py)
            dict(voce, titolo='688 Attack Sub (1989): musica dei titoli (AdLib, Tandy, Covox)', file='extracted/musica',
                 autore=f'Rob Hubbard (musica); {EA}', derivati=rel('data/motore/musica/titolo_*'),
                 nota=NON_RIDISTRIBUIBILE + '. Rese dal sequencer di CALLS.EXE'),
            dict(voce, titolo='688 Attack Sub (1989): i sei campioni .T del classico', file='extracted/raw/DISK2',
                 autore=f'Rob Hubbard (effetti sonori, crediti della scatola); {EA}', derivati=rel('data/motore/suoni_classici/*'),
                 nota=NON_RIDISTRIBUIBILE + '. Copiati senza modifiche'),
            # scritti dal progetto, ma col contenuto delle missioni originali: seguono le missioni da cui vengono
            dict(voce, titolo='Ricavati dalle missioni del 1989: kit delle rapide, obiettivi delle 22 missioni, piano del '
                 'comandante IA della sandbox', file='tools/rapide/estrai_kit.py, tools/pipeline/sandbox_originali.py',
                 autore=f'melchior-688, dalle missioni di John W. Ratcliff, Paul Grace e Randall Breen; {EA}',
                 derivati=rel('data/motore/missioni_rapide/kit_*.lua', 'data/missioni/obiettivi/*.lua',
                              'data/motore/sandbox_originali.toml'),
                 nota='Il kit e\' estratto dal codice delle missioni convertite; gli obiettivi riassumono briefing e condizioni '
                      'di vittoria degli script; il piano della sandbox cita i briefing. Senza le missioni originali non servono')]


# codice di terzi: quello compilato nel gioco o messo nel pacchetto ha il testo della licenza in `testo_licenza` (file
# del progetto separati da « + »; «lua.h» vuol dire il riquadro della licenza in fondo all'header), che
# tools/pipeline/licenze.py copia in LICENZE/<file_licenza>. Nessun testo si scarica: se manca dal progetto, licenze.py
# si ferma.
LIBRERIE = [
    ('Lua 5.4.7', 'engine/terzi/lua', 'Lua.org, PUC-Rio (R. Ierusalimschy, L. H. de Figueiredo, W. Celes)', 'MIT',
     'https://www.lua.org', 'compilata nel gioco: le missioni', 'engine/terzi/lua/lua.h', 'Lua.txt'),
    ('toml++ 3.4.0', 'engine/terzi/tomlplusplus', 'Mark Gillard', 'MIT', 'https://marzer.github.io/tomlplusplus/',
     'compilata nel gioco: i file di dati', 'engine/terzi/tomlplusplus/LICENSE', 'tomlplusplus.txt'),
    ('ymfm', 'engine/terzi/ymfm, tools/ymfm', 'Aaron Giles', 'BSD 3-Clause', 'https://github.com/aaronsgiles/ymfm',
     'compilata nel gioco: la musica FM dal vivo (nucleo/musica_fm); negli strumenti, l\'analisi della musica originale',
     'engine/terzi/ymfm/LICENSE', 'ymfm.txt'),
    ('SDL3 3.2.10', 'tools/deps/sysroot (pacchetto Debian libsdl3-0)', 'Sam Lantinga e collaboratori', 'zlib',
     'https://www.libsdl.org', 'nel pacchetto (lib/libSDL3.so.0): finestra, ingresso, audio',
     'tools/deps/sysroot/usr/share/doc/libsdl3-0/copyright', 'SDL3.txt'),
    ('sndio 1.10', 'tools/deps/sysroot (pacchetto Debian libsndio7.0)', 'Alexandre Ratchov e collaboratori', 'ISC',
     'https://sndio.org', 'nel pacchetto (lib/libsndio.so.7): dipendenza di SDL3',
     'tools/deps/sysroot/usr/share/doc/libsndio7.0/copyright', 'sndio.txt'),
    ('doctest', 'engine/terzi/doctest', 'Viktor Kirilov', 'MIT', 'https://github.com/doctest/doctest',
     'solo le prove: non entra nel gioco ne\' nel pacchetto', '', ''),
    ('libmt32emu (Munt) 2.8.3', 'tools/mt32', 'Dean Beeler, Jerome Fisher e il gruppo Munt (mt32emu/AUTHORS.txt)', 'LGPL-2.1+',
     'https://github.com/munt/munt', 'solo lo strumento tools/mt32, che rende i brani MT-32: non entra nel gioco ne\' nel '
     'pacchetto', '', '')]

# i caratteri incorporati nei PDF del manuale (tools/pipeline/manuale_pdf.py, rilascio/manuale/), come li dice pdffonts:
# Charis dal kit (tools/pdf/caratteri), gli altri dai pacchetti Debian del sistema, coi testi copiati in
# tools/deps/licenze_caratteri (08/11/2026)
CARATTERI = 'tools/deps/licenze_caratteri'
CARATTERI_PDF = [
    ('Charis 7.000', 'tools/pdf/caratteri/charis-7.000', 'SIL Global (1997-2025), nomi riservati «Charis» e «SIL»',
     'SIL OFL 1.1', 'https://software.sil.org/charis/', 'il testo dei PDF del manuale (Regular, Italic, Bold, BoldItalic)',
     'tools/pdf/caratteri/charis-7.000/OFL.txt', 'Charis.txt'),
    ('Liberation Sans e Serif 2.1.5', 'pacchetto Debian fonts-liberation', 'Red Hat, Inc. (2012), nome riservato '
     '«Liberation»; dati digitali Google (2010)', 'SIL OFL 1.1', 'https://github.com/liberationfonts',
     'incorporato nei PDF del manuale (Serif solo per le lettere greche)', f'{CARATTERI}/liberation_2.1.5_copyright.txt', 'Liberation.txt'),
    ('Liberation Sans Narrow 1.07.6', 'pacchetto Debian fonts-liberation-sans-narrow', 'Red Hat, Inc. (2007-2011)',
     'GPL-2 con l\'eccezione dei caratteri', 'https://github.com/liberationfonts/liberation-sans-narrow',
     'incorporato nei PDF del manuale', f'{CARATTERI}/liberation-sans-narrow_1.07.6_copyright.txt + {CARATTERI}/GPL-2.txt',
     'LiberationSansNarrow.txt'),
    ('JetBrains Mono 2.304', 'pacchetto Debian fonts-jetbrains-mono', 'The JetBrains Mono Project Authors (2020)',
     'SIL OFL 1.1 (progetto); Apache-2.0 (file Debian)', 'https://github.com/JetBrains/JetBrainsMono',
     'incorporato nei PDF del manuale', f'{CARATTERI}/jetbrains-mono_2.304_copyright.txt + {CARATTERI}/Apache-2.0.txt',
     'JetBrainsMono.txt'),
    ('DejaVu Sans e Sans Mono 2.37', 'pacchetti Debian fonts-dejavu-core e fonts-dejavu-mono',
     'Bitstream, Inc. (2003); modifiche DejaVu in pubblico dominio', 'Bitstream Vera',
     'https://dejavu-fonts.github.io', 'incorporato nel PDF del volume 2 (riferimento)',
     f'{CARATTERI}/dejavu_2.37_copyright.txt', 'DejaVu.txt')]


def voci_librerie():
    out = []
    for gruppo, elenco in (('librerie', LIBRERIE), ('caratteri dei PDF', CARATTERI_PDF)):
        for nome, cart, autore, lic, url, uso, testo, nome_file in elenco:
            out.append(dict(gruppo=gruppo, titolo=nome, file=cart, url=url, autore=autore, licenza=lic,
                            attribuzione=True, condivisione=False, uso=uso, derivati=[], stato='attribuzione',
                            categoria='terzi', testo_licenza=testo, file_licenza=nome_file,
                            nota=('incorporato nei PDF del manuale: la licenza va con i PDF' if gruppo != 'librerie' else
                                  'la licenza va inclusa con i binari' if testo else
                                  'le ROM dell\'MT-32 restano dell\'utente, fuori dal progetto' if lic.startswith('LGPL') else
                                  'non distribuita')))
    return out


def voci_mt32():
    # i brani resi dall'utente con le ROM Roland (tools/mt32/rendi_mt32): fuori da data/ nel progetto, il pacchetto privato li porta in data/mt32/
    return [dict(gruppo='suoni e musica', titolo='Musica del remake resa con le ROM Roland MT-32 (i WAV di data/mt32/ nel pacchetto privato)',
                 file='tools/mt32/rendi_mt32', url='', autore='composizioni di melchior-688 (musica/fm, CC0); timbri delle ROM '
                 'di Roland Corporation; resa con libmt32emu (Munt)', licenza='proprietaria (Roland): uso personale',
                 attribuzione=True, condivisione=False, uso='musica delle schermate con l\'uscita MT-32', derivati=[],
                 stato='proprietario', categoria='terzi',
                 nota='I WAV contengono i campioni delle ROM Roland: solo nel pacchetto privato, per uso personale, mai '
                      'ridistribuiti. Le ROM non sono nel progetto ne\' nel pacchetto')]


def voci_meteo():
    # climatologia del meteo dell'Enhanced (docs/15_meteo_e_mare.md, M0): ERA5 1981-1990 scaricato da Open-Meteo
    return [dict(gruppo='meteo e clima', titolo='ERA5, rianalisi oraria 1981-1990 (atmosfera e onde), 33 punti dei teatri',
                 file='tools/meteo/climatologia_era5.json', url='https://cds.climate.copernicus.eu/datasets/reanalysis-era5-single-levels',
                 autore='Copernicus Climate Change Service (C3S), ECMWF; Hersbach et al. 2020 (doi:10.1002/qj.3803)',
                 licenza='CC BY 4.0 (licenza Copernicus)', attribuzione=True, condivisione=False,
                 uso='climatologia per teatro e mese del meteo dinamico', derivati=['data/motore/meteo.toml'],
                 stato='attribuzione', nota='«Contiene informazioni modificate del Copernicus Climate Change Service (1981-1990)»'),
            dict(gruppo='meteo e clima', titolo='Open-Meteo, API storiche (meteo e mare)', file='tools/meteo/scarica_era5.py',
                 url='https://open-meteo.com', autore='Open-Meteo (P. Zippenfenig)', licenza='CC BY 4.0',
                 attribuzione=True, condivisione=False, uso='accesso ai dati ERA5', derivati=['data/motore/meteo.toml'],
                 stato='attribuzione', nota='dati via Open-Meteo.com')]


def voci_fari():
    # i fari dei teatri (manuale C9 §2.15, kit 31/10/2026): da Wikidata, CC0 (tools/pipeline/fari_wikidata.py)
    if not os.path.exists(f'{MOTORE}/fari.toml'):
        return []
    return [dict(gruppo='batimetria e oceanografia', titolo='Wikidata: i fari dei teatri (posizione, caratteristica, '
                 'altezza focale, portata, prima accensione)', file='extracted/fari_wikidata',
                 url='https://www.wikidata.org', autore='i contributori di Wikidata', licenza='CC0', attribuzione=False,
                 condivisione=False, uso='i fari al periscopio, accesi nel 1989', derivati=['data/motore/fari.toml'],
                 stato='ok', nota='')]


def voci_suoni():
    # banco di campioni del trailer (docs/trailer/01/tema.md): le note di copyright vanno riportate in ogni derivato
    autori = ('S. Christian Collins (adattamento MuseScore_General, 2018-19); Frank Wen (FluidR3, 2000-02); '
              'Michael Cowgill (FluidR3Mono, 2014-17); Ethan Winer (Temple Blocks, 2002); '
              'Michael Schorsch (Drumline Cymbals, 2016)')
    return [dict(gruppo='suoni e musica', titolo='Banco di campioni MuseScore General (coro, ottoni, archi, taiko, percussioni)',
                 file='tools/deps/soundfont/MuseScore_General.sf3', url='https://ftp.osuosl.org/pub/musescore/soundfont/MuseScore_General/',
                 autore=autori, licenza='MIT', attribuzione=True, condivisione=False,
                 uso='trailer 01: tema «Tikhaya okhota» reso con FluidSynth', derivati=['docs/trailer/01/tema_trailer.mp3',
                 'docs/trailer/01/tema_lungo.mp3', 'docs/trailer/01/trailer01_it.mp4', 'docs/trailer/01/trailer01_en.mp4'],
                 stato='attribuzione', nota='testo completo della licenza e delle note in tools/deps/soundfont/MuseScore_General_License.md')] + voci_campioni() + voci_effetti()


def voci_effetti():
    # gli effetti di bordo registrati (tools/pipeline/effetti_veri.py, roadmap 38): una voce per file, dal manifesto
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from effetti_veri import MANIFESTO
    out = []
    for e in MANIFESTO:
        tipo, stato, attr, cond = classifica(e['licenza'] if not e['licenza'].startswith('PD') else 'pubblico dominio')
        out.append(dict(gruppo='suoni e musica', titolo=f'Effetto «{e["id"]}» ({e["voce"]})',
                        file=f'tools/campioni/effetti_grezzi/{e["cartella"]}/{e["file"]}', url=e['pagina'], autore=e['autore'],
                        licenza=e['licenza'], attribuzione=attr, condivisione=cond, uso='effetti di bordo e della sala radio',
                        derivati=[f'data/suoni/effetti/{e["id"]}.wav'], stato=stato, nota=e['note']))
    from effetti_sintetici import EFFETTI
    out.append(dict(gruppo='suoni e musica', titolo='Effetti di bordo sintetizzati (periscopio, tubi, zavorra, scafo, allarmi sovietici...)',
                    file='tools/pipeline/effetti_sintetici.py', url='', autore='melchior-688', licenza='del progetto',
                    attribuzione=False, condivisione=False, uso='effetti di bordo senza una registrazione vera',
                    derivati=[f'data/suoni/effetti/{i}.wav' for i, _, _ in EFFETTI], stato='ok', nota='sintesi, nessuna fonte esterna'))
    out.append(dict(gruppo='suoni e musica', titolo='Musiche FM e timbri del remake', file='tools/pipeline/musiche_fm.py',
                    url='', autore='melchior-688', licenza='del progetto', attribuzione=False, condivisione=False,
                    uso='musica delle schermate (nucleo/musica_fm)', derivati=['data/motore/timbri_fm.toml']
                    + sorted(os.path.relpath(p, RADICE) for p in glob.glob(f'{MOTORE}/musica/fm/*.mid')),
                    stato='ok', nota='composte per il progetto e dichiarate CC0 (musiche_fm.py, timbri_fm.py); nessun tema o '
                                     'banco d\'epoca copiato'))
    return out


def voci_campioni():
    # campioni NOAA e NPS della libreria R4 (tools/pipeline/campioni_analisi.py, docs/ricerca/18_note/campioni_r4.md): le
    # voci arrivano gia' complete, con lo stato deciso voce per voce (non da classifica(): la licenza dice «pubblico
    # dominio» anche dove l'autore non e' federale o non e' dichiarato)
    f = f'{RADICE}/tools/campioni/crediti.json'
    voci = json.load(open(f)) if os.path.exists(f) else []
    for v in voci:   # decisione della principale del 07/11/2026: va bene per il pacchetto privato, non oltre
        if 'presunto' in v['licenza']:
            v['nota'] = v['nota'] + '. Da sostituire o verificare prima di un\'uscita pubblica: la pagina NOAA non dice nulla sui diritti'
    return voci


def voci_font():
    # font bitmap delle stazioni della Fase 3 (dossier 19 §9.4), disegnati da zero per il remake
    fnt = sorted(glob.glob(f'{MOTORE}/font/*.fnt'))
    derivati = [os.path.relpath(f, RADICE) for f in fnt if not f.endswith('originale.fnt')]
    return [dict(gruppo='font del progetto', titolo='Fosforo 688, Incisione 688, Stampino URSS e Cifre (font bitmap delle stazioni)',
                 file='tools/pipeline/font_stazioni.py', url='', autore='melchior-688 (disegnati da zero)',
                 licenza='del progetto', attribuzione=False, condivisione=False, uso='stazioni della Fase 3 (dossier 19)',
                 derivati=derivati, stato='ok', nota='nessuna fonte esterna: tracciato 5x7 disegnato a mano, alla Hershey')]


def voci_toponimi():
    # i nomi delle carte dei teatri (tools/pipeline/toponimi.py): Natural Earth, piu' le aggiunte a mano di toponimi_aggiunte.yaml
    derivati = sorted(os.path.relpath(p, RADICE) for p in glob.glob(f'{MOTORE}/teatri/*.toponimi.toml'))
    return [dict(gruppo='batimetria e oceanografia', titolo='Natural Earth 1:10m: aree marine, regioni fisiche e localita\'',
                 file='tools/toponimi/grezzi/naturalearth', url='https://www.naturalearthdata.com',
                 autore='Natural Earth (naturalearthdata.com)', licenza='pubblico dominio', attribuzione=False,
                 condivisione=False, uso='i nomi e i punti delle carte dei teatri', derivati=derivati, stato='ok',
                 categoria='terzi', nota='«Made with Natural Earth»: citazione gradita, non obbligatoria')] if derivati else []


def voci_nostre():
    """Il lavoro del progetto: ogni file di dati che non e' copiato da altri ha qui la sua voce (anche quando e' ricavato
    da dati di terzi, che hanno la loro voce: le due si sommano). I fatti dei dossier (docs/ricerca) vengono da fonti
    pubbliche citate campo per campo nei file stessi: sono dati, non opere, e non impongono licenze."""
    def rel(*modelli, tranne=()):
        return sorted(r for m in modelli for r in (os.path.relpath(p, RADICE) for p in glob.glob(f'{RADICE}/{m}'))
                      if os.path.isfile(f'{RADICE}/{r}') and r not in tranne)
    voce = dict(gruppo='lavoro del progetto', url='', autore='melchior-688', licenza='del progetto', attribuzione=False,
                condivisione=False, stato='ok', categoria='nostro')
    return [dict(voce, titolo='Cataloghi, regole e tabelle del motore (classi, armi, sensori, acustica, meteo, carriera, '
                 'enciclopedia, tasti...)', file='tools/pipeline', uso='i dati del motore',
                 derivati=rel('data/motore/*.toml', tranne=('data/motore/timbri_fm.toml', 'data/motore/sandbox_originali.toml')),
                 nota='scritti dal progetto: i numeri vengono dai dossier di docs/ricerca, con la fonte per campo'),
            dict(voce, titolo='Modelli 3D, sprite e scafi delle unita\' (il kit grafico)', file='tools/navi',
                 uso='le unita\' al periscopio, nel Museo e nella fisica dei danni',
                 derivati=rel('data/motore/navi/*', 'data/motore/modelli/*', 'data/motore/scafi/*'),
                 nota='geometria del progetto; le tessiture fotografiche hanno la voce della loro foto'),
            dict(voce, titolo='Foto del Museo digitalizzate «alla 1989» (320x200, 16 grigi)', file='tools/pipeline/enciclopedia.py',
                 uso='le foto delle voci del Museo', derivati=rel('data/motore/immagini/enciclopedia/*'),
                 nota='elaborazione del progetto: ogni immagine ha anche la voce della sua foto, con la licenza di quella'),
            dict(voce, titolo='Fondali, carte d\'epoca, carte delle basi, profili del suono, toponimi e traffico dei teatri',
                 file='tools/pipeline (build_heightmap, carta_epoca, carte_basi, suono, toponimi, traffico)',
                 uso='i teatri', derivati=rel('data/motore/teatri/*', 'data/motore/basi/*'),
                 nota='elaborazione del progetto dai dati di batimetria e oceanografia, che hanno le loro voci'),
            dict(voce, titolo='Missioni del progetto: libreria, prove della sandbox, esperimenti, video d\'apertura, radio e '
                 'ASW aereo delle missioni originali', file='docs/17_libreria_missioni.md, docs/28, docs/29',
                 uso='missioni e contenuti dell\'Enhanced',
                 derivati=rel('data/missioni/libreria/*', 'data/missioni/prove/*', 'data/missioni/radio/*',
                              'data/missioni/asw/*', 'data/missioni/API.md', 'data/motore/esperimenti/*',
                              'data/motore/apertura/*'),
                 nota='radio/ e asw/ aggiungono alle missioni originali contenuti scritti dal progetto; non contengono testi '
                      'del 1989')]


def categoria(v):
    return v.get('categoria') or ('nostro' if v['licenza'] == 'del progetto' else 'terzi')


def controlla_copertura(voci):
    """Ogni file del pacchetto (data/motore, data/suoni, data/missioni) ha almeno una voce; ogni derivato esiste."""
    coperti = {d for v in voci for d in v['derivati']}
    tutti = {os.path.relpath(os.path.join(c, f), RADICE) for d in ('data/motore', 'data/suoni', 'data/missioni')
             for c, _, fs in os.walk(f'{RADICE}/{d}') for f in fs}
    senza = sorted(tutti - coperti)
    assenti = sorted(d for d in coperti if not os.path.isfile(f'{RADICE}/{d}'))
    if senza or assenti:
        sys.exit('crediti incompleti:\n' + ''.join(f'  senza voce: {f}\n' for f in senza)
                 + ''.join(f'  derivato assente: {f}\n' for f in assenti))
    return len(tutti)


def toml_str(s):
    return '"' + str(s).replace('\\', '\\\\').replace('"', '\\"') + '"'


def scrivi_toml(voci, percorso):
    righe = ['# Crediti e licenze degli asset (generato da tools/pipeline/crediti.py: non modificare a mano).',
             '# stato: ok | attribuzione | condivisione (CC BY-SA: i derivati con la stessa licenza) | consultata |',
             '#        da_verificare | proprietario (non ridistribuibile)',
             '# categoria: nostro (fatto dal progetto) | terzi (opera altrui con la sua licenza) | originale (gioco del 1989,',
             '#        proprieta\' di Electronic Arts). Un file puo\' stare in piu\' voci: i suoi obblighi si sommano.', '']
    for v in voci:
        righe += ['[[asset]]'] + [f'{k} = {toml_str(v[k])}' for k in ('gruppo', 'categoria', 'titolo', 'autore', 'licenza', 'stato',
                                                                        'uso', 'url', 'file', 'nota')]
        for k in ('testo_licenza', 'file_licenza'):
            if v.get(k):
                righe.append(f'{k} = {toml_str(v[k])}')
        righe += [f'attribuzione = {str(v["attribuzione"]).lower()}', f'condivisione = {str(v["condivisione"]).lower()}',
                  'derivati = [' + ', '.join(toml_str(d) for d in v['derivati']) + ']', '']
    open(percorso, 'w').write('\n'.join(righe))


def scrivi_md(voci, percorso):
    da_segnalare = [v for v in voci if v['stato'] in ('da_verificare', 'proprietario', 'condivisione', 'attribuzione')]
    r = ['# Crediti e licenze', '',
         'Generato da `tools/pipeline/crediti.py` (non modificare a mano); la stessa tabella, completa, è in',
         '`data/motore/crediti.toml` per la schermata dei crediti e il Museo.', '',
         'Una foto è **derivata** quando una classe del kit la proietta negli sprite e nel modello 3D; le foto solo',
         '**consultate** (misure, verifiche, scartate) si citano ma non impongono obblighi.', '',
         '## Da segnalare', '']
    for st, titolo in (('proprietario', 'Non ridistribuibili (asset dell\'originale)'),
                       ('da_verificare', 'Licenza incerta o mancante'),
                       ('condivisione', 'CC BY-SA: i file derivati vanno distribuiti con la stessa licenza e attribuiti'),
                       ('attribuzione', 'Attribuzione obbligatoria (CC BY; licenze delle librerie da includere coi binari)')):
        sel = [v for v in da_segnalare if v['stato'] == st]
        if not sel:
            continue
        r += [f'### {titolo}', '']
        for v in sel:
            der = ', '.join(f'`{d}`' for d in v['derivati'][:6]) + (' …' if len(v['derivati']) > 6 else '')
            r.append(f"- **{v['titolo']}** — {v['autore'] or 'autore non indicato'}, {v['licenza']}"
                     + (f" ([pagina]({v['url']}))" if v['url'] and ' ' not in v['url'] else (f" ({v['url']})" if v['url'] else ''))
                     + (f'. Derivati: {der}' if der else '')
                     + (f". {v['nota'][0].upper() + v['nota'][1:].rstrip('.')}" if v['nota'] else '') + '.')
        r.append('')
    for gruppo in ('foto e disegni del kit', 'batimetria e oceanografia', 'meteo e clima', 'librerie', 'caratteri dei PDF',
                   'suoni e musica', 'font del progetto',
                   'lavoro del progetto', "asset dell'originale"):
        sel = [v for v in voci if v['gruppo'] == gruppo and (v['uso'] != 'consultata' or gruppo != 'foto e disegni del kit')]
        if not sel:
            continue
        r += [f'## {gruppo[0].upper() + gruppo[1:]}', '', '| Titolo | Autore | Licenza | Stato | Derivati |', '|---|---|---|---|---|']
        for v in sel:
            t = f"[{v['titolo']}]({v['url']})" if v['url'] and ' ' not in v['url'] else v['titolo']
            der = ', '.join(os.path.basename(d) for d in v['derivati'][:4]) + (' …' if len(v['derivati']) > 4 else '')
            r.append(f"| {t.replace('|', '/')} | {(v['autore'] or '').replace('|', '/')} | {v['licenza']} | {v['stato']} | {der} |")
        r.append('')
    cons = [v for v in voci if v['uso'] == 'consultata' and v['gruppo'] == 'foto e disegni del kit']
    r += ['## Fonti consultate (non proiettate)', '', f'{len(cons)} foto e disegni usati solo per misure e verifiche; '
          'l\'elenco completo è in `data/motore/crediti.toml` (`uso = "consultata"`).', '']
    open(percorso, 'w').write('\n'.join(r))


def esporta_crediti(radice=RADICE):
    voci = (voci_foto() + voci_teatri() + voci_toponimi() + voci_fari() + voci_meteo() + voci_librerie() + voci_suoni()
            + voci_mt32() + voci_font() + voci_nostre() + voci_originale())
    for v in voci:
        v['categoria'] = categoria(v)
    controlla_copertura(voci)
    scrivi_toml(voci, f'{radice}/data/motore/crediti.toml')
    scrivi_md(voci, f'{radice}/docs/CREDITI.md')
    from collections import Counter
    return dict(Counter(v['stato'] for v in voci))


if __name__ == '__main__':
    print('crediti:', esporta_crediti())
