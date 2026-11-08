"""Tabelle generate dei capitoli del disassemblatore nel manuale dell'Enhanced (volume 2: acustica C1-C5 e C8, carico D5,
carriera F2, riconoscimenti F3).

Stessa forma di tools/pipeline/manuale.py, del curatore: ogni funzione prende la cartella dei dati e restituisce la
tabella in Markdown; manuale.py le prende da TABELLE_ACUSTICA e riempie i segnaposto <!-- generato: NOME -->.
Dati: data/motore/acustica.toml, classi.toml, campioni.toml, carico.toml, carriera.toml, teatri/<teatro>.suono.toml.
"""
import math
import tomllib

# nomi dei sensori come nel gioco (le chiavi di [sensori.*] di acustica.toml)
NOMI_SENSORI = {
    'bqq5_sfera': 'BQQ-5, sfera (passivo)', 'bqq5_attivo': 'BQQ-5, sfera (attivo)', 'tb16': 'TB-16 (schiera)',
    'wlr9': 'WLR-9 (intercettatore)', 'intercettazione_sov': 'intercettatore sovietico (Enisej)',
    'nave_sovietica': 'sonar di prua delle navi sovietiche', 'sovietico_3gen': 'MGK-400/503 (terza generazione)',
    'sovietico_2gen': 'MG-200 Kerch (seconda generazione)', 'sovietico_attivo': 'attivo dei sottomarini sovietici',
    'sqs53': 'SQS-53 (passivo)', 'sqs53_attivo': 'SQS-53 (attivo)', 'sqs56_attivo': 'SQS-56 (attivo)',
    'titan2_attivo': 'MG-332 Titan-2 (attivo)', 'skat': 'schiera sovietica (Skat, Pelamida)',
}
# firme senza classe in classi.toml
NOMI_FIRME = {
    'sottomarino_generico': 'sottomarino generico', 'nave_generica': 'nave generica', 'biologico': 'biologico',
    'adc_mk2': 'esca ADC Mk 2', 'mk48': 'siluro Mk 48', 'mk48_adcap': 'siluro Mk 48 ADCAP', 'tigerfish': 'siluro Tigerfish',
    'spearfish': 'siluro Spearfish', 'uset80': 'siluro USET-80', 'test71m': 'siluro TEST-71M',
    'type53_65k': 'siluro 53-65K', 'mk46': 'siluro Mk 46', 'at1': 'siluro AT-1', 'siluro_generico': 'siluro generico',
}
MESI = {2: 'feb', 5: 'mag', 8: 'ago', 11: 'nov'}
GRADIENTE_PRESSIONE = 0.0163


def _toml(dati, nome):
    return tomllib.loads((dati / nome).read_text(encoding='utf-8'))


def numero(v, cifre=0):
    """Numero all'italiana: virgola decimale"""
    return f'{v:.{cifre}f}'.replace('.', ',')


def _cella(t):
    """Un testo dei dati in una cella: i termini del glossario e gli accenti veri (manuale.glossario del curatore)"""
    from manuale import glossario   # dentro la funzione: manuale.py importa questo modulo
    return glossario(str(t).replace('|', '/').replace('\n', ' '))


def _teatro(t):
    from manuale import TEATRI   # il nome per il lettore, come nelle tabelle del curatore
    return TEATRI.get(t, t)


def _classi(dati):
    return _toml(dati, 'classi.toml')


def _nome_classe(classi, k):
    return classi.get(k, {}).get('nome') or NOMI_FIRME.get(k, k)


def sensori_sonar(dati):
    """I sensori di bordo (quelli delle [suite]): modo, antenna, bande, integrazione, rumore proprio, limiti, LOFAR"""
    a = _toml(dati, 'acustica.toml')
    usati = []
    for lista in a['suite'].values():
        usati += [s for s in lista if s not in usati]
    righe = ['| Sensore | Modo | Antenna (m) | Bande (Hz) | Integrazione (s), d | Rumore proprio: macchina / flusso a V₀ (dB, kn) '
             '| Velocità max (kn) | Settore cieco | LOFAR: bin (Hz), T (s) |', '|---|---|---|---|---|---|---|---|---|']
    for k in usati:
        s = a['sensori'][k]
        attivo = s['modo'] == 'attivo'
        modo = f"attivo, SL {numero(s.get('sl_attivo_db', 0))} dB, τ {numero(s.get('impulso_s', 0), 1)} s" if attivo else 'passivo'
        bande = ', '.join(numero(b) for b in s.get('bande_hz', []))
        integ = f"{numero(s.get('integrazione_s', 0))}, {numero(s.get('indice_rilevazione', 0))}" if not attivo else \
            f"—, {numero(s.get('indice_rilevazione', 0))}"
        rumore = f"{numero(s.get('nl_macchina_db', 0))} / {numero(s.get('nl_flusso_db', 0))} a {numero(s.get('v0_flusso_kn', 0))}"
        cieco = []
        if s.get('baffles_gradi', 0) > 0:
            extra = f", +{numero(s['baffles_db'])} dB" if s.get('baffles_db', 0) > 0 else ''
            cieco.append(f"baffles ±{numero(s['baffles_gradi'])}°{extra}")
        if s.get('endfire_gradi', 0) > 0:
            cieco.append(f"endfire ±{numero(s['endfire_gradi'])}°")
        if s.get('virata_max_deg_s', 1e9) < 100:
            cieco.append(f"virata oltre {numero(s['virata_max_deg_s'], 1)} °/s")
        lofar = f"{numero(s['lofar_bin_hz'], 2)}, {numero(s.get('lofar_integrazione_s', 0))}" if s.get('lofar_bin_hz', 0) > 0 else '—'
        antenna = f"{s.get('antenna', '')} {numero(s.get('dimensione_m', 0), 1)}"
        righe.append(f"| {NOMI_SENSORI.get(k, k)} | {modo} | {antenna} | {bande} | {integ} | {rumore} | "
                     f"{numero(s.get('velocita_max_kn', 0))} | {'; '.join(cieco) or '—'} | {lofar} |")
    return '\n'.join(righe) + '\n'


def suite_sonar(dati):
    """Le classi e i loro sensori ([suite] di acustica.toml)"""
    a, classi = _toml(dati, 'acustica.toml'), _classi(dati)
    righe = ['| Classe | Sensori |', '|---|---|']
    for k, lista in a['suite'].items():
        righe.append(f"| {_nome_classe(classi, k)} | {', '.join(NOMI_SENSORI.get(s, s) for s in lista)} |")
    return '\n'.join(righe) + '\n'


# --- profili: la stessa analisi di ProfiloSuono::analizza (nucleo/src/acustica.cpp) ---

def _c(z_m, c_m_s, z):
    if z <= z_m[0]:
        return c_m_s[0]
    if z >= z_m[-1]:
        return c_m_s[-1] + GRADIENTE_PRESSIONE * (z - z_m[-1])
    for i in range(1, len(z_m)):
        if z < z_m[i]:
            t = (z - z_m[i - 1]) / (z_m[i] - z_m[i - 1])
            return c_m_s[i - 1] + t * (c_m_s[i] - c_m_s[i - 1])
    return c_m_s[-1]


def analizza(z_m, c_m_s):
    """strato (m), asse (m), profondita' critica (m, -1 se mai), prima CZ (m, 0 se nessuna)"""
    imax = 0
    for i, z in enumerate(z_m):
        if z <= 400.0 and c_m_s[i] > c_m_s[imax]:
            imax = i
    sld = z_m[imax]
    imin = min(range(len(z_m)), key=lambda i: (c_m_s[i], i))
    c_vertice, z_vertice = 0.0, 0.0
    for i in range(imin + 1):
        if c_m_s[i] > c_vertice:
            c_vertice, z_vertice = c_m_s[i], z_m[i]
    critica = -1.0
    for i in range(imin + 1, len(z_m)):
        if c_m_s[i] >= c_vertice:
            t = (c_vertice - c_m_s[i - 1]) / (c_m_s[i] - c_m_s[i - 1])
            critica = z_m[i - 1] + t * (z_m[i] - z_m[i - 1])
            break
    cz = 0.0
    if critica - z_vertice >= 800.0 and c_vertice > c_m_s[imin] + 0.5:
        x, z = 0.0, z_vertice
        while z < critica:
            cm = _c(z_m, c_m_s, min(z + 0.5, critica - 1e-3))
            d = c_vertice * c_vertice - cm * cm
            if d > 0.0:
                x += cm / math.sqrt(d) * min(1.0, critica - z)
            z += 1.0
        cz = 2.0 * x
    return sld, z_m[imin], critica, cz


def strato(sld):
    if sld <= 0:
        return 'nessuno'
    return f'≥ {numero(sld)}' if sld >= 400.0 else numero(sld)   # 400 m: il limite della ricerca dello strato


def profili_suono(dati):
    """Per teatro e per quattro mesi: velocita' in superficie, strato, asse, prima zona di convergenza; il traffico"""
    righe = ['| Teatro | Mese | c in superficie (m/s) | Strato (m) | Asse (m) | Prima CZ (km) | Traffico |',
             '|---|---|---|---|---|---|---|']
    for f in sorted((dati / 'teatri').glob('*.suono.toml')):
        t = tomllib.loads(f.read_text(encoding='utf-8'))
        for m, nome in MESI.items():
            p = t['mesi'][str(m)]
            sld, asse, _, cz = analizza(p['z_m'], p['c_m_s'])
            predefinito = ' (predefinito)' if m == t.get('mese_predefinito') else ''
            righe.append(f"| {_teatro(t['teatro'])} | {nome}{predefinito} | {numero(p['c_m_s'][0], 1)} | "
                         f"{strato(sld)} | {numero(asse) if asse > 0 else 'superficie'} | "
                         f"{numero(cz / 1000, 1) if cz > 0 else 'nessuna'} | {numero(t.get('traffico', 0), 1)} |")
    return '\n'.join(righe) + '\n'


def sorgenti_ambiente(dati):
    """La libreria delle sorgenti del paesaggio sonoro ([sorgenti.*])"""
    a = _toml(dati, 'acustica.toml')
    righe = ['| Sorgente | Categoria | Livello (dB, rms) | Teatri | Mesi | Densità (per 1000 nm²) | Fonte | Confidenza |',
             '|---|---|---|---|---|---|---|---|']
    for s in sorted(a['sorgenti'].values(), key=lambda s: (s['categoria'], s['nome'])):
        righe.append(f"| {_cella(s['nome'])} | {s['categoria']}, {s.get('sottocategoria', '')} | {numero(s['livello_db'])} | "
                     f"{', '.join(_teatro(t) for t in s.get('teatri', [])) or 'tutti'} | {', '.join(str(m) for m in s.get('mesi', [])) or 'tutto l’anno'} | "
                     f"{numero(s.get('densita', 0), 2)} | {_cella(s.get('fonte', ''))} | {_cella(s.get('confidenza', ''))} |")
    return '\n'.join(righe) + '\n'


def _livello(punti, v):
    if v <= punti[0][0]:
        return punti[0][1]
    for i in range(1, len(punti)):
        if v <= punti[i][0] or i == len(punti) - 1:
            a, b = math.log10(punti[i - 1][0]), math.log10(punti[i][0])
            return punti[i - 1][1] + (punti[i][1] - punti[i - 1][1]) * (math.log10(v) - a) / (b - a)


def firme_acustiche(dati):
    """Le firme: banda larga 5-200 Hz a quattro velocita' (dalla tabella, senza cavitazione), cavitazione, tonale,
    elica, forza di bersaglio, fonte"""
    a, classi = _toml(dati, 'acustica.toml'), _classi(dati)
    righe = ['| Firma | Banda larga a 5 / 10 / 20 / 30 kn (dB) | Cavitazione in superficie (kn) | Tonale (Hz, dB) | '
             'Elica: pale, assi, giri per nodo | TS prua / traverso (dB) | Fonte |', '|---|---|---|---|---|---|---|']
    for k, f in a['firme'].items():
        punti = f.get('punti', [])
        bl = ' / '.join(numero(_livello(punti, v)) if punti and v <= punti[-1][0] else '—' for v in (5, 10, 20, 30)) if punti else '—'
        cav = numero(f['v_cavitazione_kn'], 1) if f.get('v_cavitazione_kn', 0) > 0 and not f.get('nave') else '—'
        ton = f"{numero(f['tonale_hz'], 1)}, {numero(f['tonale_db'])}" if f.get('tonale_hz', 0) > 0 else '—'
        elica = f"{f['pale']}, {f.get('assi', 1)}, {numero(f.get('giri_per_nodo', 0), 1)}" if f.get('pale', 0) > 0 else '—'
        ts = f"{numero(f.get('ts_prua_db', 0))} / {numero(f.get('ts_traverso_db', 0))}"
        righe.append(f"| {_nome_classe(classi, k)} | {bl} | {cav} | {ton} | {elica} | {ts} | {_cella(f.get('fonte', ''))} |")
    return '\n'.join(righe) + '\n'


def righe_firme(dati):
    """Le righe tonali delle firme: Hz, dB relativi alla tonale, origine, velocita' in cui ci sono"""
    a, classi = _toml(dati, 'acustica.toml'), _classi(dati)
    righe = ['| Firma | Righe: Hz (dB sulla tonale, origine, velocità) |', '|---|---|']
    for k, f in a['firme'].items():
        if not f.get('righe'):
            continue
        pezzi = []
        for hz, db, tipo, da, a_, *asse in f['righe']:
            vel = '' if da <= 0 and a_ >= 1e8 else f', da {numero(da)} kn' if a_ >= 1e8 else f', fino a {numero(a_)} kn' if da <= 0 \
                else f', {numero(da)}-{numero(a_)} kn'
            if asse and asse[0] > 0:   # legata all'asse (blocco 2 del sonar): la frequenza e' quella a 10 kn
                pezzi.append(f'{numero(hz, 1)} a 10 kn ({numero(db)}, {tipo}, {numero(asse[0])} volte l\'asse{vel})')
                continue
            pezzi.append(f'{numero(hz, 1)} ({numero(db)}, {tipo}{vel})')
        righe.append(f"| {_nome_classe(classi, k)} | {'; '.join(pezzi)} |")
    return '\n'.join(righe) + '\n'


PROPULSIONI = {'diesel_lento': 'diesel lento, presa diretta', 'diesel_medio': 'diesel medio, riduttore',
               'diesel_veloce': 'diesel veloce', 'diesel_elettrico': 'diesel-elettrico', 'turbina': 'turbina'}


def tipi_civili(dati):
    """I tipi delle navi civili generate ([civili.*]): gli intervalli da cui il seme fa l'individuo"""
    a = _toml(dati, 'acustica.toml')
    def da_a(v, cifre=0):
        lo, hi = v
        return numero(lo, cifre) if lo == hi else f'{numero(lo, cifre)}-{numero(hi, cifre)}'
    righe = ['| Tipo | Lunghezza (m) | Servizio (kn) | Cavitazione (kn) | Propulsione | Assi, pale | Giri dell\'asse | '
             'Individui con la rete a 60 Hz | Fonte | Confidenza |', '|---|---|---|---|---|---|---|---|---|---|']
    for c in a.get('civili', {}).values():
        prop = PROPULSIONI.get(c['propulsione'], c['propulsione']) + (', passo variabile' if c.get('passo_variabile') else '')
        righe.append(f"| {_cella(c['nome'])} | {da_a(c['lunghezza_m'])} | {da_a(c['velocita_servizio_kn'])} | "
                     f"{da_a(c['cavitazione_kn'])} | {prop} | {da_a(c['assi'])}, {da_a(c['pale'])} | {da_a(c['giri_asse'])} | "
                     f"{numero(100 * c.get('rete_60', 0))}% | {_cella(c.get('fonte', ''))} | {_cella(c.get('confidenza', ''))} |")
    return '\n'.join(righe) + '\n'


def transitori_acustici(dati):
    """I transitori della libreria ([transitori.*])"""
    a = _toml(dati, 'acustica.toml')
    righe = ['| Transitorio | Categoria | Livello (dB) | Durata (s) | Indizi | Fonte | Confidenza |',
             '|---|---|---|---|---|---|---|']
    for t in sorted(a['transitori'].values(), key=lambda t: (t['categoria'], t['nome'])):
        durata = numero(t['vita_s'], 1) if t.get('vita_s', 0) > 0 else '—'
        righe.append(f"| {_cella(t['nome'])} | {t['categoria']}, {t.get('sottocategoria', '')} | {numero(t['livello_db'])} | {durata} | "
                     f"{_cella('; '.join(t.get('indizi', [])))} | {_cella(t.get('fonte', ''))} | {_cella(t.get('confidenza', ''))} |")
    return '\n'.join(righe) + '\n'


NOMI_CAMPIONI = {'vento': 'Vento sul mare', 'pioggia': 'Pioggia sul mare', 'mercantile': 'Mercantile'}


def campioni_audio(dati):
    """I campioni registrati in uso (campioni.toml, stato «ok» o «attribuzione», cioe' CC BY col credito): la voce col
    nome per il lettore, che cosa e' stato registrato (un sostituto, per gli aerei), durata, banda valida, licenza,
    attribuzione"""
    c = _toml(dati, 'campioni.toml')['campioni']
    a = _toml(dati, 'acustica.toml')
    righe = ['| Voce | Registrato al posto della voce | Durata vera (s) | Banda valida (Hz) | Licenza | Attribuzione |',
             '|---|---|---|---|---|---|']
    for k, x in sorted(c.items(), key=lambda kv: (kv[1].get('voce', kv[0]), kv[0])):
        if x.get('stato', 'ok') not in ('ok', 'attribuzione'):
            continue
        v = x.get('voce', k)
        nome = NOMI_CAMPIONI.get(v) or a['sorgenti'].get(v, {}).get('nome') or a['transitori'].get(v, {}).get('nome') or v
        banda = f"{numero(x['f_min_valida_hz'])}-{numero(x['f_max_valida_hz'])}"
        # per aerei ed elicotteri la nota dice quale macchina suona davvero (un sostituto coi motori della voce)
        sostituto = _cella(x.get('note', '')) if v.startswith(('aereo_', 'elicottero_')) and x.get('note') else '—'
        righe.append(f"| {_cella(nome)} | {sostituto} | {numero(x.get('durata_vera_s', 0), 1)} | {banda} | "
                     f"{_cella(x.get('licenza', ''))} | {_cella(x.get('attribuzione', ''))} |")
    return '\n'.join(righe) + '\n'


NAZIONI_CARICO = {'usa': 'USA', 'urss': 'URSS', 'uk': 'Regno Unito'}
NOMI_LOCALI = {'los_angeles': 'Los Angeles (Flight I)', 'los_angeles_flight2': 'Los Angeles (Flight II)',
               'los_angeles_688i': 'Los Angeles (688I)', 'alfa': 'Alfa (Progetto 705)'}


def _stelle(n):
    return '★' * int(n) + '☆' * (3 - int(n))


def armi_carico(dati):
    """Le armi del carico (carico.toml [armi]): marina, famiglia, prestazioni, posti, servizio, efficacia, limiti"""
    a = _toml(dati, 'carico.toml')['armi']
    righe = ['| Arma | Marina | Famiglia | Guida | Velocità (kn), portata (km), testata (kg) | Posti | In servizio | '
             'Contro sottomarini / superficie / terra / come mina | Limiti | Fonte, confidenza |',
             '|---|---|---|---|---|---|---|---|---|---|']
    for k, x in a.items():
        prest = ', '.join(numero(x[c]) if x.get(c, 0) > 0 else '—' for c in ('velocita_kn', 'portata_km', 'testata_kg'))
        servizio = f"dal {x['in_servizio_dal']}" + (f", ritirata nel {x['ritirata_nel']}" if x.get('ritirata_nel', 0) else '')
        posti = numero(x.get('posti', 1), 1 if x.get('posti', 1) % 1 else 0) + (' (pozzo)' if x.get('pozzo_verticale') else '')
        e = x.get('efficacia', {})
        eff = ' / '.join(_stelle(e.get(c, 0)) for c in ('sottomarino', 'superficie', 'terra', 'mine'))
        limiti = [t for c, t in (('solo_ia', 'solo IA'), ('what_if', 'what-if'), ('moderno', 'solo nel modo moderno'),
                                 ('nucleare', 'nucleare')) if x.get(c)]
        if x.get('massimo_primo_anno', 0):
            limiti.append(f"al più {x['massimo_primo_anno']} nei primi due anni")
        righe.append(f"| {_cella(x['nome'])} | {NAZIONI_CARICO.get(x['nazione'], x['nazione'])} | {x['famiglia']} | "
                     f"{_cella(x.get('guida', ''))} | {prest} | {posti} | {servizio} | {eff} | {', '.join(limiti) or '—'} | "
                     f"{_cella(x.get('fonte', ''))}; {_cella(x.get('confidenza', ''))} |")
    return '\n'.join(righe) + '\n'


def locali_siluri(dati):
    """I locali siluri per classe e variante (carico.toml [locali])"""
    a = _toml(dati, 'carico.toml')['locali']
    righe = ['| Battello | Tubi (calibro) | Rastrelliera | Pozzi verticali | Lanciatori d\'esche, esche | Armi in acqua al più | Fonte |',
             '|---|---|---|---|---|---|---|']
    for k, x in a.items():
        righe.append(f"| {_cella(x.get('nome', NOMI_LOCALI.get(k, k)))} | {x['tubi']} ({x['calibro_mm']} mm) | {x['rastrelliera']} | "
                     f"{x.get('pozzi', 0) or '—'} | {x.get('lanciatori_esche', 0)}, {x.get('esche', 0)} | {x.get('limite_in_acqua', 0)} | "
                     f"{_cella(x.get('fonte', ''))} |")
    return '\n'.join(righe) + '\n'


def carichi_standard(dati):
    """I carichi standard per tipo di missione (carico.toml [carichi]), coi nomi delle armi"""
    c = _toml(dati, 'carico.toml')
    nomi = {k: x['nome'] for k, x in c['armi'].items()}
    nomi.update({k: k.upper() for k in c.get('mine', {}) if k not in nomi})
    righe = ['| Battello | Missione | Nei tubi | In rastrelliera | Nei pozzi |', '|---|---|---|---|---|']

    def elenco(d):
        return ', '.join(f"{n} {nomi.get(a, a)}" for a, n in d.items()) or '—'
    for k, x in c['carichi'].items():
        for tipo, car in x.items():
            if tipo == 'predefinito':
                continue
            tubi = {}
            for a in car.get('tubi', []):
                tubi[a] = tubi.get(a, 0) + 1
            nome_tipo = tipo.replace('_', ' ') + (' (predefinito)' if tipo == x.get('predefinito') else '')
            righe.append(f"| {NOMI_LOCALI.get(k, k)} | {nome_tipo} | {elenco(tubi)} | {elenco(car.get('rastrelliera', {}))} | "
                         f"{elenco(car.get('pozzi', {}))} |")
    return '\n'.join(righe) + '\n'


def _carriera(dati):
    return _toml(dati, 'carriera.toml')


def _marina(m):
    from manuale import MARINE   # i nomi del curatore
    return MARINE.get(m, m)


USCITE = {'missioni': 'dopo le missioni', 'qualifica': 'con la qualifica', 'commissione': 'con la commissione',
          'fine': 'fine della carriera'}


def fasi_carriera(dati):
    """Le fasi della carriera per marina (carriera.toml [fasi]): grado, incarico, missioni, come si esce, soglie"""
    c = _carriera(dati)
    righe = ['| Marina | Grado | Incarico | Missioni | Uscita | Media in zona / sotto zona (punti) |', '|---|---|---|---|---|---|']
    for marina, fasi in c['fasi'].items():
        for f in fasi:
            g = c['gradi'][marina].get(f['grado'], {})
            grado = f"{g.get('nome_it', f['grado'])} ({g.get('sigla', f['grado'])})"
            uscita = USCITE.get(f['uscita'], f['uscita'])
            if f.get('qualifica'):
                uscita += f" ({f['qualifica'].replace('_', ' ')})"
            soglie = f"{f['in_zona']} / {f['sotto_zona']}" if 'in_zona' in f else (f"media almeno {f['media_minima']}"
                                                                                  if 'media_minima' in f else '—')
            righe.append(f"| {_marina(marina)} | {_cella(grado)} | {_cella(f['incarico'])} | {f['missioni'] or '—'} | "
                         f"{uscita} | {soglie} |")
    return '\n'.join(righe) + '\n'


def soglie_decorazioni(dati):
    """La decorazione personale di una missione per punteggio, marina e situazione (carriera.toml [soglie])"""
    c = _carriera(dati)
    condizioni = {'pericolo': 'con il pericolo', 'speciale': 'solo nelle operazioni speciali'}
    righe = ['| Marina | Situazione | Punti | Decorazione | Note |', '|---|---|---|---|---|']
    for marina, per in c['soglie'].items():
        for situazione, voci in per.items():
            for v in voci:
                d = c['decorazioni'].get(v['decorazione'], {})
                nome = d.get('nome_it', v['decorazione'])
                note = []
                if v.get('dispositivo') == 'v':
                    note.append('con la «V» del combattimento')
                if v.get('condizione'):
                    note.append(condizioni.get(v['condizione'], v['condizione']))
                righe.append(f"| {_marina(marina)} | {'pace e crisi' if situazione == 'pace' else situazione} | {v['punti']} | "
                             f"{_cella(nome)} | {', '.join(note) or '—'} |")
    return '\n'.join(righe) + '\n'


def _titolo_missione(dati, scena):
    """Il titolo italiano di una missione originale, dal suo file Lua (data/missioni/<SCENA>.lua)"""
    import re
    f = dati.parent / 'missioni' / f'{scena}.lua'
    if f.exists():
        m = re.search(r'titolo\s*=\s*\{\s*it\s*=\s*"([^"]*)"', f.read_text(encoding='utf-8'))
        if m:
            return m[1]
    return scena


def carriera_originali(dati):
    """Le 22 missioni originali per la carriera (carriera.toml [missioni_originali]): situazione, avversario,
    difficolta', operazione speciale, Artico"""
    c = _carriera(dati)
    righe = ['| Missione | Situazione | Avversario | Difficoltà | Operazione speciale | Artico |', '|---|---|---|---|---|---|']
    for k, x in c['missioni_originali'].items():
        righe.append(f"| {_cella(_titolo_missione(dati, k))} ({k}) | {x['situazione']} | {x['avversario']} | {x['difficolta']} | "
                     f"{'sì' if x.get('operazione_speciale') else '—'} | {'sì' if x.get('artico') else '—'} |")
    return '\n'.join(righe) + '\n'


def giudizi_carriera(dati):
    """I giudizi a parole del punteggio (carriera.toml [giudizi.livelli])"""
    c = _carriera(dati)
    righe = ['| Da punti | Giudizio |', '|---|---|']
    for v in sorted(c['giudizi']['livelli'], key=lambda v: -v['punti']):
        righe.append(f"| {v['punti']} | {v['it']} |")
    return '\n'.join(righe) + '\n'


CATEGORIE_ACHIEVEMENT = ['tattico', 'stealth', 'tecnico', 'storico', 'carriera', 'editor', 'nascosto']
AMBITI = {'missione': 'in una missione', 'somma': 'sommato sulle missioni', 'carriera': 'dal fascicolo'}


def _achievement(dati, nascosti):
    a = _toml(dati, 'achievement.toml')['achievement']
    righe = ['| Achievement | Categoria | Che cosa | Livelli | Ambito | Condizioni |', '|---|---|---|---|---|---|']
    voci = [(k, v) for k, v in a.items() if bool(v.get('nascosto')) == nascosti]
    voci.sort(key=lambda kv: CATEGORIE_ACHIEVEMENT.index(kv[1]['categoria']) if kv[1]['categoria'] in CATEGORIE_ACHIEVEMENT else 99)
    for k, v in voci:
        lv = v['livelli']
        livelli = f'oro {lv[0]}' if len(lv) == 1 else ', '.join(f'{n} {x}' for n, x in zip(('bronzo', 'argento', 'oro'), lv))
        ambito = AMBITI.get(v['ambito'], v['ambito'])
        if v.get('distinti'):
            ambito += f" (valori diversi di «{v['distinti']}»)"
        cond = '; '.join(f'`{m} {op} {x}`' for m, op, x in v.get('condizioni', [])) or '—'
        righe.append(f"| {_cella(v['nome_it'])} | {v['categoria']} | {_cella(v['descrizione_it'])} (`{v['misura']}`) | {livelli} | "
                     f"{ambito} | {cond} |")
    return '\n'.join(righe) + '\n'


def achievement_elenco(dati):
    """Gli achievement visibili (achievement.toml): categoria, descrizione con la misura, livelli, ambito, condizioni"""
    return _achievement(dati, False)


def achievement_nascosti(dati):
    """Gli achievement nascosti, che il gioco non mostra finche' non si guadagnano"""
    return _achievement(dati, True)


# --- E5, ASW aereo: i mezzi (data/motore/asw_aereo.toml [mezzi.*]) ---
RADAR_ASW = {'aps124': 'radar APS-124', 'aps115': 'radar APS-115', 'aps116': 'radar APS-116', 'aps137': 'radar APS-137',
             'seaspray': 'radar Sea Spray', 'initsiativa2k': 'radar Initsiativa-2K', 'initsiativa2km': 'radar Initsiativa-2KM',
             'initsiativa2b': 'radar Initsiativa-2B', 'wet_eye': 'radar Wet Eye', 'korshun': 'radar Korshun'}
ARMI_ASW = {'mk46': 'Mk 46', 'stingray': 'Sting Ray', 'tp42': 'Tp 42', 'at1': 'AT-1', 'at2': 'AT-2', 'apr2': 'APR-2',
            'plab250': 'PLAB-250'}
PARTI_ASW = {'usa': 'NATO', 'urss': 'Patto di Varsavia'}
NAZIONI_ASW = {'lynx': 'Regno Unito', 'hkp4': 'Svezia'}   # mezzi della parte NATO che non sono americani
EQUIPAGGI_ASW = {'scarso': 'scarso', 'medio': 'medio', 'esperto': 'esperto'}


def _breve_boa(nome):
    nome = nome.split(' (')[0].split(' «')[0]
    return nome.replace('AN/', '')


def mezzi_asw(dati):
    d = _toml(dati, 'asw_aereo.toml')
    boe, sonar = d.get('boe', {}), d.get('sonar_immersione', {})
    out = ['| Mezzo | Parte | Sensori | Carico predefinito | Armi | TOS a 0 nm (h) | Equipaggio |',
           '|---|---|---|---|---|---|---|']
    for k, m in d['mezzi'].items():
        parte = PARTI_ASW.get(m['parte'], m['parte'])
        if k == 'hkp4':   # la Svezia nel 1989 era neutrale: nel gioco l'elicottero sta solo dal lato NATO
            parte = 'Svezia (neutrale; dal lato NATO nel gioco)'
        elif k in NAZIONI_ASW:
            parte += f' ({NAZIONI_ASW[k]})'
        elif m['parte'] == 'usa':
            parte += ' (USA)'
        if m.get('dal_anno'):
            parte += f', dal {m["dal_anno"]}'
        carico = m['carichi'][m['carico_predefinito']]
        sensori = []
        if m.get('sonar'):
            sensori.append('sonar a immersione ' + _breve_boa(sonar.get(m['sonar'], {}).get('nome', m['sonar'])))
        if carico.get('boe') or m.get('canali_boe'):
            sensori.append(f'boe ({m.get("canali_boe", 0)} canali)' if m.get('canali_boe') else 'boe')
        if 'mad' in m:
            sensori.append('MAD')
        if m.get('radar'):
            sensori.append(RADAR_ASW.get(m['radar'], m['radar']))
        b = ', '.join(f'{n} {_breve_boa(boe.get(t, {}).get("nome", t))}' for t, n in carico.get('boe', {}).items()) or '—'
        armi = ', '.join(f'{n} {ARMI_ASW.get(t, t)}' for t, n in carico.get('armi', {}).items()) or '—'
        out.append(f'| {m["nome"]} | {parte} | {", ".join(sensori) or "—"} | {b} | {armi} | '
                   f'{numero(m["tos_base_h"], 1 if abs(m["tos_base_h"] * 10 - round(m["tos_base_h"] * 10)) < 1e-9 else 2)} | {EQUIPAGGI_ASW.get(m["equipaggio"], m["equipaggio"])} |')
    return '\n'.join(out) + '\n'


# --- T4, tattiche: le pale, la rete e la velocita' di cavitazione (data/motore/acustica.toml [firme.*]) ---
# La formula e' quella del motore, nucleo/src/acustica.cpp velocita_cavitazione_kn: v_cav * sqrt((10,33 + quota) / 10,33)
CLASSI_CAVITAZIONE = ('los_angeles', 'sturgeon', 'ohio', 'victor3', 'alfa', 'akula', 'sierra', 'oscar', 'typhoon', 'delta3',
                      'kilo', 'charlie2')
QUOTE_CAVITAZIONE = (30.0, 150.0, 300.0)


def cavitazione_classi(dati):
    firme = _toml(dati, 'acustica.toml')['firme']
    classi = _classi(dati)
    out = ['| Classe | Pale | Rete (Hz) | Cavita oltre, a 30 m | a 150 m | a 300 m |', '|---|---|---|---|---|---|']
    for k in CLASSI_CAVITAZIONE:
        f = firme.get(k)
        if not f or f.get('nave'):
            continue
        rete = ', '.join(str(int(r[0])) for r in f.get('righe', []) if len(r) > 2 and r[2] == 'rete')
        v0 = f.get('v_cavitazione_kn', 0.0)
        celle = ' | '.join(f'{numero(v0 * math.sqrt((10.33 + q) / 10.33), 1)} kn' for q in QUOTE_CAVITAZIONE)
        out.append(f'| {_nome_classe(classi, k)} | {f.get("pale", "—")} | {rete or "—"} | {celle} |')
    return '\n'.join(out) + '\n'


# --- D1, siluri: la tabella di data/motore/siluri.toml [siluri.*] ---
def siluri_tabella(dati):
    d = _toml(dati, 'siluri.toml')['siluri']
    out = ['| Classe | Alta / bassa (kn) | Autonomia alta / bassa (km) | Quota max (m) | Testa | Acquisizione attiva / passiva (m) '
           '| Filo (km, siluro / battello) | Testata (kg) | Fondale minimo (m) | In servizio |',
           '|---|---|---|---|---|---|---|---|---|---|']
    for k, x in d.items():
        va, vb = x.get('velocita_alta_kn', 0), x.get('velocita_bassa_kn', 0)
        pa, pb = x.get('portata_alta_m', 0) / 1000, x.get('portata_bassa_m', 0) / 1000
        vel = numero(va) if not vb or vb == va else f'{numero(va)} / {numero(vb)}'
        aut = numero(pa) if not pb or pb == pa else f'{numero(pa)} / {numero(pb)}'
        testa = 'segui-scia' if x.get('segui_scia') else ', '.join(t for t, f in (('attiva', x.get('attivo')), ('passiva', x.get('passivo'))) if f) or '—'
        if not x.get('attivo') and x.get('passivo') and not x.get('segui_scia'):
            testa = 'solo passiva'
        aa, ap = x.get('acquisizione_attiva_m', 0), x.get('acquisizione_passiva_m', 0)
        acq = '—' if not aa and not ap else f'{numero(aa) if aa else "—"} / {numero(ap) if ap else "—"}'
        filo = f'{numero(x.get("filo_siluro_m", 0) / 1000)} / {numero(x.get("filo_battello_m", 0) / 1000)}' if x.get('filoguidato') else '—'
        fm = x.get('fondale_minimo_m', 0)
        out.append(f'| `{k}` {x.get("nome", k)} | {vel} | {aut} | {numero(x.get("quota_max_m", 0))} | {testa} | {acq} | {filo} | '
                   f'{numero(x.get("testata_kg", 0))} | {numero(fm) if fm else "—"} | {x.get("in_servizio_dal", "—")} |')
    return '\n'.join(out) + '\n'


# --- F1, registro: i tipi d'evento dall'enum TipoEvento (engine/missioni/include/missioni/registro_missione.hpp) ---
def eventi_registro(dati):
    import re
    hpp = (dati / '..' / '..' / 'engine' / 'missioni' / 'include' / 'missioni' / 'registro_missione.hpp').resolve()
    righe = hpp.read_text(encoding='utf-8').splitlines()
    i = next(k for k, r in enumerate(righe) if 'enum class TipoEvento' in r)
    tipi, gruppo, ultimo = [], '', None
    for r in righe[i + 1:]:
        codice, _, commento = r.partition('//')
        nomi = [n.strip() for n in codice.split(',') if re.fullmatch(r'\s*[a-z_]+\s*', n)]
        if 'numero' in nomi:
            break
        if nomi:
            for n in nomi:
                tipi.append([n, gruppo, ''])
            ultimo = tipi[-1]
            if commento.strip():
                ultimo[2] = commento.strip()
        elif commento.strip():
            if len(r) - len(r.lstrip()) < 20:   # un commento di gruppo, all'inizio della riga
                gruppo, ultimo = commento.strip(), None
            elif ultimo is not None:
                ultimo[2] += ' ' + commento.strip()
    out = [f'*{len(tipi)} tipi, dall\'enum `TipoEvento` di `missioni/registro_missione.hpp`, con il commento del codice.*', '',
           '| Tipo | Che cosa |', '|---|---|']
    for n, g, c in tipi:
        c = re.sub(r"([aeiou])'(?![a-zA-Z])", lambda m: {'a': 'à', 'e': 'è', 'i': 'ì', 'o': 'ò', 'u': 'ù'}[m.group(1)], c).replace('|', '/')
        out.append(f'| `{n}` | {c or "—"} |')
    return '\n'.join(out) + '\n'


TABELLE_ACUSTICA = {'mezzi_asw': mezzi_asw, 'eventi_registro': eventi_registro, 'siluri_tabella': siluri_tabella, 'cavitazione_classi': cavitazione_classi, 'sensori_sonar': sensori_sonar, 'suite_sonar': suite_sonar, 'profili_suono': profili_suono,
                    'sorgenti_ambiente': sorgenti_ambiente, 'firme_acustiche': firme_acustiche, 'righe_firme': righe_firme,
                    'tipi_civili': tipi_civili,
                    'transitori_acustici': transitori_acustici, 'campioni_audio': campioni_audio,
                    'armi_carico': armi_carico, 'locali_siluri': locali_siluri, 'carichi_standard': carichi_standard,
                    'fasi_carriera': fasi_carriera, 'soglie_decorazioni': soglie_decorazioni,
                    'carriera_originali': carriera_originali, 'giudizi_carriera': giudizi_carriera,
                    'achievement_elenco': achievement_elenco, 'achievement_nascosti': achievement_nascosti}
