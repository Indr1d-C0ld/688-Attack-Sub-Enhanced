"""Tabelle generate del manuale dell'Enhanced (docs/manuale_enhanced, README «Regole comuni»).

Le tabelle che vengono da data/motore non si scrivono a mano: nei capitoli c'è un segnaposto

    <!-- generato: NOME -->
    <!-- fine: NOME -->

e questo script mette fra i due segni la tabella NOME, rifatta dai dati. Quello che c'era fra i segni si sostituisce;
il resto del file non si tocca. Rifarlo con gli stessi dati dà gli stessi file.

Uso: manuale.py [--controlla] [--foto] [cartella del manuale] [cartella dei dati]
  --controlla  non scrive: esce con 1 se qualche tabella non è aggiornata, o se un tasto del codice delle schermate
               manca nell'appendice A (manuale_tasti.py), per le prove
  --foto       prepara prima le foto ridotte dei crediti dell'enciclopedia in figure/foto

Le immagini in gioco delle schede del volume 3 (figure/unita/<voce>_<k>.png) vengono da
engine/build/anteprima_enciclopedia <dati> --manuale <cartella>, ritagliate a 380x240.
"""
import pathlib
import re
import sys
import tomllib

RADICE = pathlib.Path(__file__).resolve().parents[2]
# i moduli delle aree (manuale_<area>.py) stanno accanto: anche con python -I (la regressione), che non mette la
# cartella dello script nel percorso
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
SEGNO = re.compile(r'(<!-- generato: ([a-z0-9_]+) -->\n)(.*?)(<!-- fine: \2 -->)', re.S)


def tasti(dati):
    """I tasti predefiniti, coi commenti del file come spiegazione, nell'ordine del file"""
    righe = ['| Comando | Tasti | Che cosa fa |', '|---|---|---|']
    testo = (dati / 'tasti_predefiniti.toml').read_text(encoding='utf-8')
    valori = tomllib.loads(testo)['tasti']
    for linea in testo.splitlines():
        m = re.match(r'^([a-z0-9_]+) = .*?#\s*(.*)$', linea)
        if not m or m[1] not in valori:
            continue
        v = valori[m[1]]
        v = [v] if isinstance(v, str) else v
        tasti_ = ', '.join(f'`{t}`' for t in v if t) or '(nessuno)'
        righe.append(f'| `{m[1]}` | {tasti_} | {accenti(m[2].strip())} |')
    return '\n'.join(righe) + '\n'


def numero(v, cifre=0):
    """Numero all'italiana: virgola decimale"""
    return f'{v:.{cifre}f}'.replace('.', ',')


def scafi(dati):
    """I file degli scafi (scafi/<classe>.toml) con il nome della classe da classi.toml, in ordine di nome"""
    classi = tomllib.loads((dati / 'classi.toml').read_text(encoding='utf-8'))
    out = []
    for f in sorted((dati / 'scafi').glob('*.toml')):
        s = tomllib.loads(f.read_text(encoding='utf-8'))
        out.append((f.stem, classi.get(f.stem, {}).get('nome', f.stem), s))
    return out


def scafi_sottomarini(dati):
    """Gli scafi dei sottomarini: dislocamenti, quote, riserva di spinta, intercapedine, squadre"""
    righe = ['| Classe | Scafo | Dislocamento in emersione / immersione (t) | Collaudo / collasso previsto (m) | '
             'Riserva di spinta | Intercapedine (m) | Squadre | Compartimenti |',
             '|---|---|---|---|---|---|---|---|']
    for _, nome, s in scafi(dati):
        sm = s.get('sottomarino')
        if not sm:
            continue
        comp = sum(1 for c in s.get('cella', []) if c['uso'] != 'zavorra')
        righe.append(f"| {nome} | {sm['scafo']} | {numero(sm['dislocamento_emersione_t'])} / "
                     f"{numero(sm['dislocamento_immersione_t'])} | {numero(sm['quota_collaudo_m'])} / "
                     f"{numero(sm['quota_collasso_m'])} | {numero(sm['riserva_spinta'], 2)} | "
                     f"{numero(sm['intercapedine_m'], 2)} | {s['nave']['squadre']} | {comp} |")
    return '\n'.join(righe) + '\n'


def compartimenti_battelli(dati):
    """I compartimenti dello scafo resistente del 688 e dell'Alfa, da prua a poppa; le casse zavorra contate a parte"""
    righe = ['| Battello | Compartimento | Uso | Da x a x (m) | Permeabilità |', '|---|---|---|---|---|']
    for stem, nome, s in scafi(dati):
        if stem not in ('los_angeles', 'alfa'):
            continue
        celle = s.get('cella', [])
        for c in sorted((c for c in celle if c['uso'] != 'zavorra'), key=lambda c: -c['x1']):
            uso = c['uso'] + (f" ({c['deposito']})" if c.get('deposito') else '')
            righe.append(f"| {nome} | {c['nome']} | {uso} | {numero(c['x1'], 1)} … {numero(c['x0'], 1)} | "
                         f"{numero(c['permeabilita'], 2)} |")
        casse = sum(1 for c in celle if c['uso'] == 'zavorra')
        righe.append(f'| {nome} | (casse zavorra) | zavorra | {casse} celle, alle estremità o fra i due scafi | 0,98 |')
    return '\n'.join(righe) + '\n'


def scafi_navi(dati):
    """Gli scafi delle navi di superficie: dimensioni, stabilità, controllo danni, trave, celle stagne"""
    righe = ['| Classe | Tipo | Dislocamento (t) | Lunghezza × larghezza (m) | GM (m) | Squadre | Qualità del CD | '
             'Resistenza della trave | Celle stagne |',
             '|---|---|---|---|---|---|---|---|---|']
    for _, nome, s in scafi(dati):
        n = s['nave']
        if 'sottomarino' in s:
            continue
        celle = s.get('cella', [])
        righe.append(f"| {nome} | {n['tipo']} | {numero(n['dislocamento_pubblicato_t'])} | "
                     f"{numero(n['lunghezza_m'], 1)} × {numero(n['larghezza_m'], 1)} | {numero(n['GM_m'], 2)} | "
                     f"{n['squadre']} | {numero(n['qualita_controllo_danni'], 2)} | {numero(n['resistenza_trave'], 2)} | "
                     f"{len(celle)} |")
    return '\n'.join(righe) + '\n'


VARIANTI = {'los_angeles_flight2': 'Los Angeles (Flight II)', 'los_angeles_688i': 'Los Angeles (688I)'}


def sistemi_navigazione(dati):
    """I sistemi di navigazione per classe (navigazione.toml): deriva dell'inerziale, fonte e confidenza"""
    nav = tomllib.loads((dati / 'navigazione.toml').read_text(encoding='utf-8'))
    classi = tomllib.loads((dati / 'classi.toml').read_text(encoding='utf-8'))
    righe = ['| Classe | Sistema | Deriva (nm al giorno, 1σ) | Fonte | Confidenza |', '|---|---|---|---|---|']
    for classe, chiave in nav['classi'].items():
        s = nav['sistemi'][chiave]
        nome = classi.get(classe, {}).get('nome', VARIANTI.get(classe, classe))
        deriva = numero(s['deriva_nm_giorno'], 2) if s['deriva_nm_giorno'] > 0 else 'la corrente non nota'
        righe.append(f"| {nome} | {s['nome']} | {deriva} | {s['fonte']} | {s['confidenza']} |")
    return '\n'.join(righe) + '\n'


def punti_nave(dati):
    """I punti nave (navigazione.toml): precisione, condizioni, marina, confidenza"""
    nav = tomllib.loads((dati / 'navigazione.toml').read_text(encoding='utf-8'))
    righe = ['| Punto nave | Precisione (nm, 1σ) | Durata (s) | Condizioni | Marina, dal | Confidenza |',
             '|---|---|---|---|---|---|']
    for p in nav['punti'].values():
        cond = []
        if p['quota_max_m'] < 1000:
            cond.append(f"entro {numero(p['quota_max_m'])} m")
        if p['velocita_max_kn'] < 1000:
            cond.append(f"sotto {numero(p['velocita_max_kn'])} kn")
        cond += [t for k, t in (('albero', 'albero fuori'), ('filo', 'filo galleggiante'), ('emissione', 'emette'),
                                ('cielo', 'cielo sereno'), ('notte', 'notte o crepuscolo'), ('carta', 'carta')) if p[k]]
        if p['costa_km'] > 0:
            cond.append(f"entro {numero(p['costa_km'])} km dalla costa")
        if p['teatri']:
            cond.append('teatri: ' + ', '.join(TEATRI.get(x, x) for x in p['teatri']))
        if p['opzione']:
            cond.append(f"opzione «{p['opzione']}»")
        prec = numero(p['sigma_min'], 2) + ('' if p['sigma_max'] == p['sigma_min'] else f"-{numero(p['sigma_max'], 2)}")
        if p['per_kn'] > 0:
            prec += f" + {numero(p['per_kn'], 2)} per nodo d'incertezza"
        if p['frazione_isobate'] > 0:
            prec = f"{numero(p['frazione_isobate'], 2)} della spaziatura delle isobate"
        righe.append(f"| {p['nome']} | {prec} | {numero(p['durata_s'])} | {'; '.join(cond) or '-'} | "
                     f"{p['marina']}, {numero(p['anno_dal']) if p['anno_dal'] > 0 else 'sempre'} | {p['confidenza']} |")
    return '\n'.join(righe) + '\n'


def teatri_navigazione(dati):
    """Per teatro la latitudine di riferimento e la corrente non nota che la stima accumula (navigazione.toml)"""
    nav = tomllib.loads((dati / 'navigazione.toml').read_text(encoding='utf-8'))
    righe = ['| Teatro | Latitudine | Corrente non nota (kn) |', '|---|---|---|']
    for teatro, v in nav['teatri'].items():
        righe.append(f"| {TEATRI.get(teatro, teatro)} | {numero(v['latitudine'], 1)}° N | {numero(v['corrente_kn'], 1)} |")
    return '\n'.join(righe) + '\n'


# --- schede delle unita' del volume 3, dall'enciclopedia (data/motore/enciclopedia.toml) ---------------------------

MANUALE = RADICE / 'docs' / 'manuale_enhanced'   # main() la sostituisce con la cartella data
NAZIONI = {'usa': 'Stati Uniti', 'urss': 'Unione Sovietica', 'uk': 'Regno Unito', 'nato': 'NATO', 'jp': 'Giappone',
           'entrambe': 'entrambe le parti', 'neutrale': 'neutrale'}
LARGHEZZA_FOTO = 560


def _enciclopedia(dati):
    return tomllib.loads((dati / 'enciclopedia.toml').read_text(encoding='utf-8'))


def _immagine_utile(percorso):
    """Un'immagine in gioco serve se non e' quasi tutta di un colore (una carta senza la costa, per esempio: 99,2% di un
    colore). Il 98%: il profilo sottile del 688 in immersione sta al 97,4%, e prima restava fuori"""
    from PIL import Image
    im = Image.open(percorso)
    colori = im.getcolors(1 << 16) or []
    return bool(colori) and max(n for n, _ in colori) < 0.98 * im.width * im.height


def prepara_foto(dati, manuale):
    """Le foto dei crediti ridotte per il manuale (figure/foto/<voce>_<n>.jpg, larghe al piu' LARGHEZZA_FOTO)"""
    from PIL import Image
    cartella = manuale / 'figure' / 'foto'
    cartella.mkdir(parents=True, exist_ok=True)
    for chiave, v in _enciclopedia(dati)['voci'].items():
        for n, c in enumerate(v.get('crediti', [])):
            sorgente = RADICE / c.get('file', '')
            if not c.get('file') or not sorgente.exists():
                continue
            im = Image.open(sorgente)
            im = im.convert('L' if im.mode in ('L', 'LA', '1') else 'RGB')
            if im.width > LARGHEZZA_FOTO:
                im = im.resize((LARGHEZZA_FOTO, round(im.height * LARGHEZZA_FOTO / im.width)), Image.LANCZOS)
            im.save(cartella / f'{chiave}_{n}.jpg', quality=82, optimize=True)


# I termini del glossario della traduzione (docs/traduzione/glossario.md) nei testi presi dai dati e dai dossier
GLOSSARIO = [
    (r"\b([Uu]n|[Ll]|[Dd]ell|[Nn]ell|[Ss]ull|[Aa]ll|[Dd]all|[Cc]oll)a schiera (?:rimorchiat|trainat)a\b",
     lambda m: m[1] + "'antenna rimorchiata"),
    (r'\bschier([ae]) (?:rimorchiat|trainat)([ae])\b', lambda m: 'antenn' + m[1] + ' rimorchiat' + m[2]),
    (r'\bsonobo([ae])\b', lambda m: 'boa sonora' if m[1] == 'a' else 'boe sonore'),
    (r'\bidroplani\b', 'timoni orizzontali'),
    (r'\bFaroe\b', 'Fær Øer'),
    (r'\bOchotsk\b', 'Okhotsk'),   # come nella traduzione e nel glossario
]


def glossario(t):
    """Il testo coi termini del glossario e gli accenti veri; i link [..](..) e il codice `..` restano come sono"""
    pezzi = re.split(r'(\[[^\]]*\]\([^)]*\)|`[^`]*`)', t)
    out = []
    for p in pezzi:
        if p.startswith('[') or p.startswith('`'):
            out.append(p)
            continue
        for modello, nuovo in GLOSSARIO:
            p = re.sub(modello, nuovo, p)
        out.append(accenti(p))
    return ''.join(out)


def _riga_scheda(r):
    gioco = r.get('gioco_testo_it', '')
    valore = r.get('testo_lat') or r['testo_it']   # il cirillico traslitterato, come nel Museo
    return (f"| {r['nome_it']} | {valore} | {gioco.replace('nel gioco: ', '') if gioco else '-'} | "
            f"{r.get('conf', '')} | {r.get('fonte', '')} |")


def unita(categorie):
    """Le schede delle voci delle categorie date, in ordine di nome"""
    def genera(dati):
        enc = _enciclopedia(dati)
        indice, voci = enc['indice'], enc['voci']
        scelte = sorted(((k, v) for k, v in voci.items() if v['categoria'] in categorie), key=lambda kv: kv[1]['nome'])
        out = []
        for chiave, v in scelte:
            out.append(f"### {v.get('nome_lat') or v['nome']}\n")
            out.append(f"*{v['gruppo']} · {NAZIONI.get(v['nazione'], v['nazione'])}*\n")
            figure = [f for k in (0, 1) if (f := MANUALE / 'figure' / 'unita' / f'{chiave}_{k}.png').exists() and _immagine_utile(f)]
            for f in figure:
                out.append(f"![{glossario(v.get('nome_lat') or v['nome'])}](../figure/unita/{f.name})\n")
            if v.get('testo_it'):
                out.append(v['testo_it'] + '\n')
            if v.get('uso_it'):
                out.append(f"**In partita.** {v['uso_it']}\n")
            if v.get('scheda'):
                out.append('| Dato | Valore | Nel gioco | Confidenza | Fonte |\n|---|---|---|---|---|')
                out.append('\n'.join(_riga_scheda(r) for r in v['scheda']) + '\n')
            rel = {}
            for r in v.get('riferimenti', []):
                altra = voci.get(r['voce'], {})
                nome = altra.get('nome_lat') or altra.get('nome') or indice.get(r['voce'], {}).get('nome', r['voce'])
                rel.setdefault(r['relazione'], []).append(nome)
            for relazione, nomi in rel.items():
                out.append(f"**{relazione[0].upper() + relazione[1:]}:** {', '.join(nomi)}.\n")
            if v.get('armi_tipiche_altre'):
                out.append(f"**Altre armi tipiche:** {'; '.join(v['armi_tipiche_altre'])}.\n")
            if v.get('fonti'):
                out.append(f"**Fonti:** {'; '.join(v['fonti'])}.\n")
            foto = []
            for n, c in enumerate(v.get('crediti', [])):
                f = MANUALE / 'figure' / 'foto' / f'{chiave}_{n}.jpg'
                autore = c.get('autore', '').strip()
                credito = f"[{c['titolo'].removeprefix('File:')}]({c['url']})" + (f", {autore}" if autore else '') + f", {c['licenza']}"
                if f.exists():
                    foto.append(f"![{c['titolo'].removeprefix('File:')}](../figure/foto/{f.name})\n\n*Foto: {credito}.*\n")
                else:
                    foto.append(f"*Foto: {credito}.*\n")
            out.extend(foto)
        return glossario('\n'.join(out))
    return genera


# --- episodi della Guerra fredda per il volume 3, dal dossier 23 -------------------------------------------------------

LIBRERIA_EPISODI = {'E30': ('LIB-S01', 'Vestfjord'), 'E03': ('LIB-S02', 'Nave mineraria'), 'E02': ('LIB-S03', 'Quarantena'),
                    'E08': ('LIB-S04', "Campane d'edera; LIB-S05, Il guardiano del cavo"), 'E26': ('LIB-S06', 'Petroliere'),
                    'E09': ('LIB-S07', 'Tappo'), 'E01': ('LIB-S08', 'Valona'), 'E21': ('LIB-S09', 'Aport'),
                    'E19': ('LIB-S10', 'Il cavo della fregata'), 'E14': ('LIB-S11', 'Tenete il confine'),
                    'E10': ('LIB-S12', 'Il gruppo cacciatore')}
TEATRI = {'faroe': 'Fær Øer e varco GIUK', 'barents': 'Mare di Barents', 'gibilterra': 'Stretto di Gibilterra',
          'otranto': "Canale d'Otranto", 'kunashir': 'Curili e Mare di Okhotsk', 'celtica': 'Mar Celtico',
          'baltico': 'Mar Baltico'}
VOCI_EPISODIO = {'Dove': 'Dove', 'Unità': 'Unità', 'Fatti': 'Che cosa accadde', 'Noto / conteso': 'Che cosa è certo',
                 'Esito': 'Esito', 'Perché giocarlo': 'Perché conta'}


def episodi(dati):
    """Le schede degli episodi (docs/ricerca/23, §2) per il lettore: senza la giocabilità e i rimandi alle classi del
    gioco, col rimando alla missione della libreria che ne nasce"""
    testo = (RADICE / 'docs' / 'ricerca' / '23_episodi_guerra_fredda.md').read_text(encoding='utf-8')
    catalogo = testo.split('## 2. Catalogo degli episodi', 1)[1].split('\n## 3.', 1)[0]
    out = []
    for blocco in re.split(r'\n(?=### E\d\d )', catalogo)[1:]:
        righe = blocco.strip().splitlines()
        titolo = righe[0].removeprefix('### ')
        codice = titolo[:3]
        out.append(f'### {titolo}\n')
        voce, testo_voce = None, []

        def chiudi():
            if voce in VOCI_EPISODIO:
                t = '\n'.join(testo_voce).strip()
                if voce == 'Dove':   # il teatro del gioco e le note sul riquadro, dopo la freccia
                    t = re.sub(r'\s*→.*', '', t, flags=re.S).rstrip(' ,;')
                    t = re.sub(r'\*\*`([a-z]+)`\*\*', lambda m: TEATRI.get(m[1], m[1]), t)   # solo i teatri del gioco
                    t = t if t.endswith('.') else t + '.'
                elif voce == 'Unità':   # le classi del gioco, dopo la freccia, fino alle fonti
                    t = re.sub(r'\s*→.*?(?=\s*`\[|$)', '', t, flags=re.S)
                t = re.sub(r'\s*\[SOST[^\]]*\]', '', t)
                t = t[:1].upper() + t[1:]
                sep = '\n\n' if re.match(r'\d+\.', t) else ' '   # un elenco numerato va a capo
                out.append(f'**{VOCI_EPISODIO[voce]}.**{sep}{t}\n')
        for r in righe[1:]:
            m = re.match(r'- \*\*([^*]+?):\*\*\s*(.*)', r)
            if m:
                chiudi()
                voce, testo_voce = m[1], [m[2]]
            elif voce:
                testo_voce.append(r[2:] if r.startswith('  ') else r)
        chiudi()
        if codice in LIBRERIA_EPISODI:
            cod, nome = LIBRERIA_EPISODI[codice]
            out.append(f'**Nella libreria:** {cod}, {nome} (volume 1, capitolo 11).\n')
    return glossario('\n'.join(out))


# --- gradi, decorazioni e comandanti storici per il volume 3 (carriera.toml, albo_onore_storico.toml) ------------------

MARINE = {'usn': 'US Navy', 'vmf': 'Marina sovietica (VMF)'}
TIPI_DECORAZIONE = {'personale': 'personale', 'reparto': 'di reparto', 'servizio': 'di servizio', 'distintivo': 'distintivo'}


def gradi(dati):
    """I gradi delle due marine, dal piu' basso (carriera.toml [gradi])"""
    c = tomllib.loads((dati / 'carriera.toml').read_text(encoding='utf-8'))
    righe = ['| Marina | Sigla | Grado | In italiano | Codice NATO |', '|---|---|---|---|---|']
    for marina, gg in c['gradi'].items():
        for g in sorted(gg.values(), key=lambda g: g['ordine']):
            righe.append(f"| {MARINE.get(marina, marina)} | {g['sigla']} | {g['nome']} | {g['nome_it']} | {g['nato']} |")
    return '\n'.join(righe) + '\n'


def decorazioni(dati):
    """Le decorazioni delle due marine in ordine di precedenza, con la medaglia disegnata dal gioco"""
    c = tomllib.loads((dati / 'carriera.toml').read_text(encoding='utf-8'))
    out = []
    for marina in ('usn', 'vmf'):
        out.append(f'### {MARINE[marina]}\n')
        voci = sorted(((k, v) for k, v in c['decorazioni'].items() if v['marina'] == marina), key=lambda kv: kv[1]['precedenza'])
        for chiave, v in voci:
            nome = v['nome'] if v['nome'] == v['nome_it'] else f"{v['nome_it']} ({v['nome']})"
            out.append(f"#### {nome}\n")
            f = MANUALE / 'figure' / 'decorazioni' / f'{chiave}.png'
            if f.exists():
                out.append(f'![{v["nome_it"]}](../figure/decorazioni/{f.name})\n')
            out.append(f"Decorazione {TIPI_DECORAZIONE.get(v['tipo'], v['tipo'])}, precedenza {v['precedenza']}. "
                       f"Fonte: dossier 13 {v['fonte']} (confidenza {v['confidenza']}).\n")
    return '\n'.join(out)


def accenti(t):
    """Gli accenti scritti con l'apostrofo nei file di dati (piu', perche', e') tornano accenti veri; «un po'» resta"""
    t = re.sub(r"\b(perch|poich|ben|affinch|giacch)e'", r'\1é', t)

    def accento(m):
        parola = m[1]
        if parola.lower() == 'po':
            return m[0]
        return parola[:-1] + 'àèìòù'['aeiou'.index(parola[-1])]
    return re.sub(r"\b(\w*[aeiou])'(?=[\s.,;:!?)»]|$)", accento, t)


def comandanti_storici(dati):
    """I comandanti e i battelli storici dell'albo d'onore (albo_onore_storico.toml)"""
    st = tomllib.loads((dati / 'albo_onore_storico.toml').read_text(encoding='utf-8'))['storici']
    out = []
    for v in sorted(st.values(), key=lambda v: (v['marina'] != 'usn', v['anno'])):
        chi = f", comandante {v['comandante']}" if v.get('comandante') else ''
        out.append(f"### {v['anno']}, {v['battello']}\n")
        out.append(f"*{MARINE.get(v['marina'], v['marina'])}{chi}*\n")
        out.append(f"**L'impresa.** {accenti(v['impresa_it'])}.\n")
        out.append(f"**La decorazione.** {accenti(v['decorazione_it'])}.\n")
        out.append(f"*Fonte: {v['fonte']} (confidenza {v['confidenza']}).*\n")
    return '\n'.join(out)


# --- il progetto in numeri (volume 3, capitolo 4): cambia a ogni lavoro, quindi --controlla non lo guarda -------------

CARTELLE_CPP = ('nucleo', 'missioni', 'grafica', 'app', 'piattaforma', 'test', 'strumenti')
CARTELLE_PY = ('pipeline', 'navi', 're', 'rapide', 'taratura', 'regressione', 'museo', 'meteo', 'radio', 'campioni',
               'libreria', 'trailer', 'ghidra_scripts')


def _righe(file):
    return sum(1 for _ in open(file, encoding='utf-8', errors='replace'))


def progetto_in_numeri(dati):
    import datetime
    motore = RADICE / 'engine'
    cpp = [f for c in CARTELLE_CPP for f in (motore / c).rglob('*') if f.suffix in ('.cpp', '.hpp')]
    py = [f for c in CARTELLE_PY if (RADICE / 'tools' / c).exists() for f in (RADICE / 'tools' / c).rglob('*.py')]
    lua = list((RADICE / 'data' / 'missioni').rglob('*.lua'))
    prove = sum(len(re.findall(r'^TEST_CASE\(', f.read_text(encoding='utf-8', errors='replace'), re.M))
                for f in (motore / 'test').glob('*.cpp'))
    dossier = [f for f in (RADICE / 'docs' / 'ricerca').glob('[0-9][0-9]_*.md')]
    enc = _enciclopedia(dati)
    classi = tomllib.loads((dati / 'classi.toml').read_text(encoding='utf-8'))
    ach = tomllib.loads((dati / 'achievement.toml').read_text(encoding='utf-8'))
    n_ach = sum(len(v) for v in ach.values() if isinstance(v, (dict, list)))
    car = tomllib.loads((dati / 'carriera.toml').read_text(encoding='utf-8'))
    teatri = [f for f in (dati / 'teatri').glob('*.fdl') if '_carta_' not in f.name]
    carte = [f for f in (dati / 'teatri').glob('*_carta_*.fdl')]
    originali = list((RADICE / 'data' / 'missioni').glob('SCENE*.lua'))
    libreria = list((RADICE / 'data' / 'missioni' / 'libreria').glob('*.lua'))
    font = list((dati / 'font').glob('*.fnt'))
    manuale = [f for f in MANUALE.rglob('*.md')]
    parole = sum(len(f.read_text(encoding='utf-8').split()) for f in manuale)
    righe = [
        ('Righe di C++ del motore e delle prove', f"{sum(_righe(f) for f in cpp):,}".replace(',', '.'), f'{len(cpp)} file'),
        ('Righe di Python degli strumenti', f"{sum(_righe(f) for f in py):,}".replace(',', '.'), f'{len(py)} file'),
        ('Righe di Lua delle missioni', f"{sum(_righe(f) for f in lua):,}".replace(',', '.'), f'{len(lua)} file'),
        ('Casi di prova automatici', str(prove), 'doctest'),
        ('Dossier di ricerca', str(len(dossier)), '`docs/ricerca`'),
        ('Voci dell\'enciclopedia', str(len(enc['voci'])), 'il Museo'),
        ('Classi di unità', str(len(classi)), '`classi.toml`'),
        ('Modelli 3D', str(len(list((dati / 'modelli').glob('*.mdl')))), 'dal kit, con le foto'),
        ('Scafi per la fisica dei danni', str(len(list((dati / 'scafi').glob('*.toml')))), ''),
        ('Teatri', str(len(teatri)), f'{len(carte)} carte d\'epoca'),
        ('Missioni originali convertite', str(len(originali)), 'scene, 10 missioni'),
        ('Missioni della libreria', str(len(libreria)), ''),
        ('Riconoscimenti (achievement)', str(n_ach), ''),
        ('Decorazioni', str(len(car['decorazioni'])), 'due marine'),
        ('Font', str(len(font)), 'l\'originale e quelli delle stazioni'),
        ('Parole di questo manuale', f"{parole:,}".replace(',', '.'), f'{len(manuale)} file'),
    ]
    out = [f"*Contati il {datetime.date.today().strftime('%d/%m/%Y')} da `tools/pipeline/manuale.py`.*\n",
           '| | Quanti | Note |', '|---|---|---|']
    out += [f'| {a} | {b} | {c} |' for a, b, c in righe]
    return '\n'.join(out) + '\n'


VOLATILI = {'progetto_in_numeri'}


TABELLE = {'tasti': tasti, 'scafi_sottomarini': scafi_sottomarini, 'compartimenti_battelli': compartimenti_battelli,
           'scafi_navi': scafi_navi, 'sistemi_navigazione': sistemi_navigazione, 'punti_nave': punti_nave,
           'teatri_navigazione': teatri_navigazione,
           'unita_sottomarini': unita({'sottomarino'}), 'unita_navi': unita({'nave'}),
           'unita_aerei': unita({'aereo', 'elicottero'}),
           'unita_armi': unita({'siluro', 'missile', 'mina', 'esca', 'bersaglio'}),
           'unita_sensori': unita({'sonar', 'radar', 'esm', 'periscopio', 'boa'}),
           'unita_sistemi': unita({'sistema'}), 'unita_basi': unita({'base', 'marcatore'}),
           'unita_teatri': unita({'teatro'}), 'episodi': episodi, 'gradi': gradi,
           'decorazioni': decorazioni, 'comandanti_storici': comandanti_storici,
           'progetto_in_numeri': progetto_in_numeri}

# Le tabelle dei capitoli delle altre sessioni stanno nei loro moduli, con la stessa forma (nome -> funzione(dati)):
# tools/pipeline/manuale_<area>.py con un dizionario TABELLE_<AREA>. Un modulo che non c'e' ancora non ferma niente.
for _modulo, _dizionario in (('manuale_acustica', 'TABELLE_ACUSTICA'), ('manuale_missioni', 'TABELLE_MISSIONI'),
                             ('manuale_tasti', 'TABELLE_TASTI'), ('manuale_sandbox', 'TABELLE_SANDBOX')):
    try:
        TABELLE.update(getattr(__import__(_modulo), _dizionario))
    except ImportError:
        pass


def rifai(testo, dati, controlla=False):
    def sostituisci(m):
        nome = m[2]
        if nome not in TABELLE:   # il modulo di un'altra area non ancora pronto: il segnaposto resta com'e'
            print(f'avviso: tabella non ancora disponibile: {nome}', file=sys.stderr)
            return m[0]
        if controlla and nome in VOLATILI:   # i numeri del progetto cambiano a ogni lavoro: non sono «da rifare»
            return m[0]
        return m[1] + TABELLE[nome](dati) + m[4]
    # i segni dentro i blocchi di codice (gli esempi dello schema per gli autori) restano come sono
    pezzi = re.split(r'(^```.*?^```)', testo, flags=re.S | re.M)
    return ''.join(p if p.startswith('```') else SEGNO.sub(sostituisci, p) for p in pezzi)


def main():
    # un'opzione sconosciuta (anche --help) mostra l'uso e non tocca il manuale: prima lo rigenerava
    if any(a.startswith('-') and a not in ('--controlla', '--foto') for a in sys.argv[1:]):
        print(__doc__.split('Uso:', 1)[1].split('\n\n')[0].join(['Uso:', '']) if 'Uso:' in __doc__ else __doc__)
        sys.exit(2)
    argomenti = [a for a in sys.argv[1:] if not a.startswith('--')]
    controlla = '--controlla' in sys.argv
    manuale = pathlib.Path(argomenti[0]) if argomenti else RADICE / 'docs' / 'manuale_enhanced'
    dati = pathlib.Path(argomenti[1]) if len(argomenti) > 1 else RADICE / 'data' / 'motore'
    global MANUALE
    MANUALE = manuale
    if '--foto' in sys.argv:   # le foto ridotte dai crediti dell'enciclopedia (una volta; le schede le usano se ci sono)
        prepara_foto(dati, manuale)
    vecchi = 0
    for f in sorted(manuale.rglob('*.md')):
        testo = f.read_text(encoding='utf-8')
        nuovo = rifai(testo, dati, controlla)
        if nuovo != testo:
            vecchi += 1
            if controlla:
                print(f'da rifare: {f.relative_to(manuale)}')
            else:
                f.write_text(nuovo, encoding='utf-8')
                print(f'aggiornato: {f.relative_to(manuale)}')
    if controlla:   # i tasti delle schermate: ogni tasto del codice nell'appendice A (manuale_tasti.py)
        from manuale_tasti import controlla_tasti
        problemi = controlla_tasti()
        for p in problemi:
            print(p)
        if vecchi or problemi:
            sys.exit(1)


if __name__ == '__main__':
    main()
