"""Enciclopedia dell'Enhanced (docs/00_ROADMAP.md, requisito 14: il Museo esteso a tutto cio' che e' nella simulazione)
-> data/motore/enciclopedia.toml e le foto digitalizzate in data/motore/immagini/enciclopedia/ (.img).

I dati tecnici si generano dai cataloghi che gia' esistono (classi, carico, siluri, acustica, emettitori, asw_aereo,
periscopi, meteo, crediti, teatri.yaml): ogni dato ha una sola fonte e resta coerente con la simulazione. I testi
(storia e impiego in italiano e inglese, l'uso in gioco per armi e sensori) stanno in
tools/pipeline/enciclopedia_testi/*.yaml (un file per gruppo di voci), scritti a mano dai dossier di docs/ricerca con le loro fonti.

Struttura del file:
- [indice.<id>]: tutte le voci possibili (categoria, nazione, nome, catalogo d'origine, se la voce e' scritta): i
  riferimenti incrociati puntano qui, anche a voci non ancora scritte;
- [voci.<id>]: le voci scritte, con
  - nome, nome_en, categoria, gruppo (la sezione del Museo), nazione;
  - [[voci.<id>.scheda]]: le righe della scheda tecnica (campo, nome it/en, valore, unita', testo it/en gia'
    formattato, fonte, conf, dato = il campo del catalogo da cui viene);
  - testo_it, testo_en, uso_it, uso_en, fonti;
  - [[voci.<id>.riferimenti]]: relazione e voce (portata da, porta, sensori, armi, contro, mezzi, ...);
  - [[voci.<id>.immagini]]: le immagini in gioco (sprite per angolo ed elevazione, modello 3D in posa, sagoma
    dell'arma, vista dall'oculare, effetti, cascata, carta) e le foto digitalizzate «alla 1989» (.img 320x200 in
    16 grigi della palette del gioco, come gli sprite fotografici dell'originale);
  - [[voci.<id>.crediti]]: chi ha fatto le foto da cui vengono sprite, modelli e foto digitalizzate.
Gli id sono «categoria.chiave» (sottomarino.los_angeles, siluro.mk48, boa.ssq53b, sonar.bqq5, teatro.faroe...).
Uso: enciclopedia.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import json, os, sys, tomllib

# i siluri di siluri.toml che non sono nel carico ma hanno una forma nelle sagome (grafica/sagome_armi, sagoma_da_id)
SAGOME_SENZA_CARICO = ('tigerfish', 'spearfish', 'mk46', 'at1', 'at2', 'umgt1', 'apr2')

import numpy as np
import yaml
from PIL import Image, ImageFilter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import valore   # noqa: E402

# --- categorie e gruppi del Museo ---
GRUPPI = {
    'sottomarino': 'Sottomarini', 'nave': 'Navi di superficie', 'aereo': 'Aerei', 'elicottero': 'Elicotteri',
    'missile': 'Missili', 'siluro': 'Siluri', 'mina': 'Mine', 'esca': 'Esche e contromisure', 'boa': 'Boe sonore',
    'sonar': 'Sonar', 'radar': 'Radar', 'esm': 'ESM', 'periscopio': 'Periscopi', 'marcatore': 'Marcatori',
    'bersaglio': 'Bersagli', 'teatro': 'Teatri', 'base': 'Basi', 'sistema': 'Sistemi di bordo',
}
GRUPPI_EN = {
    'sottomarino': 'Submarines', 'nave': 'Surface ships', 'aereo': 'Aircraft', 'elicottero': 'Helicopters',
    'missile': 'Missiles', 'siluro': 'Torpedoes', 'mina': 'Mines', 'esca': 'Decoys and countermeasures',
    'boa': 'Sonobuoys', 'sonar': 'Sonars', 'radar': 'Radars', 'esm': 'ESM', 'periscopio': 'Periscopes',
    'marcatore': 'Markers', 'bersaglio': 'Targets', 'teatro': 'Theatres', 'base': 'Bases', 'sistema': 'Onboard systems',
}
ELICOTTERI = {'sh60b', 'sh60f', 'sh3h', 'ka25', 'ka27', 'sh2f', 'hkp4', 'lynx'}
AEREI = {'p3c', 'il38', 'tu142', 's3a', 's3b', 'be12'}
MISSILI_CLASSI = {'tomahawk': 'tlam_c', 'harpoon': 'harpoon', 'p700': 'p700'}   # classe in volo -> arma (o se stessa)
BERSAGLI = {'bqm74'}
NAZIONE_CLASSE = {   # parte delle classi (classi.toml non la porta)
    'usa': {'perry', 'kidd', 'spruance', 'iowa', 'nimitz', 'sacramento', 'tomahawk', 'harpoon', 'bqm74', 'p3c',
            'los_angeles', 'sturgeon', 'sh60b', 's3a', 's3b', 'sh3h', 'sh60f'},
    'uk': {'trafalgar', 'type22', 'type22b2', 'lynx'},
    'neutrale': {'petroliera', 'mercantile', 'cargo', 'peschereccio', 'portarinfuse', 'petroliera_costiera', 'traghetto',
                 'rimorchiatore', 'peschereccio_costiero', 'cianciolo'},
    # le classi nuove dei lotti e delle missioni S16-S18 (30/10/2026), come le nazioni dell'acustica (disassemblatore)
    'germania': {'type206'},
    'norvegia': {'oslo', 'hauk'},
    'svezia': {'stockholm', 'nacken', 'hkp4'},
}
NAZIONE_CLASSE['usa'] |= {'ticonderoga', 'knox', 'ohio', 'sh2f', 'kitty_hawk'}


def categoria_arma(v):
    """Categoria dell'enciclopedia per un'arma di carico.toml: le esche semoventi da tubo (famiglia esca_mobile) stanno
    fra le esche anche se occupano il posto di un siluro."""
    return 'esca' if v.get('famiglia') == 'esca_mobile' else v['tipo']
# sistemi sonar: piu' sensori del modello acustico (modi) in una voce; i sensori generici del modello non sono voci
SISTEMI_SONAR = {
    'bqq5': ('AN/BQQ-5', 'usa', ['bqq5_sfera', 'bqq5_attivo']), 'tb16': ('AN/BQR-15 con la schiera TB-16', 'usa', ['tb16']),
    'wlr9': ('AN/WLR-9 (intercettatore acustico)', 'usa', ['wlr9']), 'sqs53': ('AN/SQS-53', 'usa', ['sqs53', 'sqs53_attivo']),
    'sqs56': ('AN/SQS-56', 'usa', ['sqs56_attivo']), 'titan2': ('MG-332 «Titan-2»', 'urss', ['titan2_attivo']),
    'aqs13b': ('AN/AQS-13B', 'usa', ['aqs13b', 'aqs13b_passivo']), 'aqs13f': ('AN/AQS-13F', 'usa', ['aqs13f', 'aqs13f_passivo']),
    'vgs3': ("VGS-3 «Ros'-V»", 'urss', ['vgs3', 'vgs3_passivo']), 'vgs2': ('VGS-2 «Oka»', 'urss', ['vgs2', 'vgs2_passivo']),
}
SENSORI_GENERICI = {'intercettazione_sov', 'nave_sovietica', 'sovietico_3gen', 'sovietico_2gen', 'sovietico_attivo'}
# la guida delle armi di carico.toml in inglese (le schede sono bilingui)
GUIDA_EN = {
    'ancorata: libera un siluro Mk 46 al passaggio di un sottomarino': 'moored: releases a Mk 46 torpedo when a submarine passes',
    'esca acustica a quota fissa, elica intubata': 'acoustic decoy at fixed depth, ducted propeller',
    'esca che disturba o imita la firma del battello': 'decoy that jams or mimics the boat\'s signature',
    'filoguidato, poi testa attiva o passiva': 'wire-guided, then active or passive homing',
    'filoguidato, testa attiva o passiva': 'wire-guided, active or passive homing',
    'filoguidato, testa attiva o passiva a scansione elettronica': 'wire-guided, electronically scanned active or passive homing',
    'inerziale, TERCOM e DSMAC': 'inertial, TERCOM and DSMAC',
    'inerziale, radar attivo in banda J; capsula dal tubo': 'inertial, J-band active radar; capsule from the tube',
    'inerziale, radar attivo, volo radente': 'inertial, active radar, sea-skimming',
    'inerziale; capsula, poi siluro leggero Mk 50 a paracadute': 'inertial; capsule, then a Mk 50 light torpedo by parachute',
    'mina da fondo acustico-induttiva, fino a 50 m di fondale': 'acoustic-magnetic bottom mine, in up to 50 m of water',
    'mina mobile: corre fino a 13 km, poi si posa sul fondo': 'mobile mine: runs up to 13 km, then settles on the bottom',
    "mina-razzo antisom ormeggiata a 200-400 m: razzo a 80-100 m/s, attacco in 7 s al piu'":
        'moored anti-submarine rocket mine at 200-400 m: rocket at 80-100 m/s, attack within 7 s',
    'mina-razzo antisom ormeggiata: lancia un razzo subacqueo contro un sottomarino sopra di lei':
        'moored anti-submarine rocket mine: fires an underwater rocket at a submarine above it',
    "razzo antisom dal tubo, carica di profondita'": 'anti-submarine rocket from the tube, depth charge',
    'segui-scia, immune alle esche': 'wake-homing, immune to decoys',
    'supercavitante a razzo, non guidato': 'supercavitating rocket, unguided',
    'testa attiva o passiva': 'active or passive homing',
    'testa passiva': 'passive homing',
}
LICENZE_LIBERE = ('public domain', 'pd', 'cc0', 'cc by', 'cc-by', 'ogl', 'open government')


# --- traslitterazione (il font del gioco non ha il cirillico: il Museo mostra nome_lat) ---
# sistema scientifico semplificato, come nei dossier (Lebed', Signal, PZNG)
CIRILLICO = {'а': 'a', 'б': 'b', 'в': 'v', 'г': 'g', 'д': 'd', 'е': 'e', 'ё': 'ë', 'ж': 'zh', 'з': 'z', 'и': 'i', 'й': 'j',
             'к': 'k', 'л': 'l', 'м': 'm', 'н': 'n', 'о': 'o', 'п': 'p', 'р': 'r', 'с': 's', 'т': 't', 'у': 'u', 'ф': 'f',
             'х': 'kh', 'ц': 'ts', 'ч': 'ch', 'ш': 'sh', 'щ': 'shch', 'ъ': '', 'ы': 'y', 'ь': "'", 'э': 'e', 'ю': 'yu', 'я': 'ya'}


def ha_cirillico(t):
    return any('\u0400' <= c <= '\u04ff' for c in str(t))


def latino(t):
    out = []
    for c in str(t):
        l = CIRILLICO.get(c.lower())
        if l is None: out.append(c)
        else: out.append(l.capitalize() if c.isupper() and l else l)
    s = ''.join(out)
    # sigle tutte maiuscole (ПЗНГ -> PZNG, МТ -> MT)
    import re
    return re.sub(r"\b[A-Z][a-z]?(?:[A-Z][a-z]?)+\b", lambda m: m.group(0).upper() if len(m.group(0)) <= 8 and not any(
        ch.islower() for ch in m.group(0)[1:] if ch not in 'hsc') else m.group(0), s)


# --- formattazione ---
def numero(v, dec, lingua):
    s = f'{v:,.{dec}f}'
    if lingua == 'it': s = s.replace(',', '#').replace('.', ',').replace('#', '.')
    return s


def riga(campo, it, en, v, unita, fonte, conf, dato, dec=None, testo_it=None, testo_en=None):
    """Una riga della scheda; v numerico (formattato con `dec` decimali) o testo"""
    if isinstance(v, (int, float)) and not isinstance(v, bool):
        d = dec if dec is not None else (0 if float(v).is_integer() else 1)
        ti = testo_it or (numero(v, d, 'it') + (f' {unita}' if unita else ''))
        te = testo_en or (numero(v, d, 'en') + (f' {unita}' if unita else ''))
    else:
        ti, te = testo_it or str(v), testo_en or str(v)
    return dict(campo=campo, nome_it=it, nome_en=en, valore=v, unita=unita, testo_it=ti, testo_en=te,
                fonte=fonte, conf=confidenza(conf), dato=dato)


def confidenza(c):
    """Le confidenze composte dei cataloghi («media-alta», «alta / media») si riducono alla piu' bassa"""
    c = str(c).strip()
    if c.upper() == 'STIMA': return 'STIMA'
    parti = [x.strip().lower() for x in c.replace('/', '-').split('-') if x.strip()]
    for livello in ('bassa', 'media', 'alta'):
        if livello in parti: return livello
    return c


def sino(b):
    return ('sì', 'yes') if b else ('no', 'no')


class Cataloghi:
    def __init__(self, radice):
        self.radice = radice
        L = lambda f: tomllib.load(open(os.path.join(radice, 'data', 'motore', f + '.toml'), 'rb'))
        self.classi, self.carico, self.siluri, self.acustica = L('classi'), L('carico'), L('siluri'), L('acustica')
        self.emettitori, self.asw, self.periscopi, self.meteo, self.crediti = (L('emettitori'), L('asw_aereo'),
                                                                                L('periscopi'), L('meteo'), L('crediti'))
        self.teatri = {t['id']: t for t in yaml.safe_load(open(os.path.join(radice, 'tools', 'pipeline', 'teatri.yaml')))['teatri']}
        # cataloghi dell'enciclopedia: basi e sistemi di bordo (tools/pipeline/basi.yaml, sistemi_bordo.yaml)
        Y = lambda f: (yaml.safe_load(open(os.path.join(radice, 'tools', 'pipeline', f))) or {}) if os.path.exists(
            os.path.join(radice, 'tools', 'pipeline', f)) else {}
        self.basi, self.sistemi = Y('basi.yaml'), Y('sistemi_bordo.yaml')
        # valori storici dei dossier dove il gioco (classi.toml dell'originale, o il carico) usa altro
        self.reali = Y('enciclopedia_reali.yaml')

    def nazione_classe(self, c):
        for naz, ins in NAZIONE_CLASSE.items():
            if c in ins: return naz
        return 'urss'


# --- indice: tutte le voci possibili ---
def indice(K):
    ind = {}
    def metti(cat, chiave, nome, naz, origine):
        ind[f'{cat}.{chiave}'] = dict(categoria=cat, nazione=naz, nome=nome, origine=origine)
        if ha_cirillico(nome): ind[f'{cat}.{chiave}']['nome_lat'] = latino(nome)
    for c, v in K.classi.items():
        tu = K.acustica['firme'].get(c, {}).get('tipo_unita', '')
        if c in BERSAGLI: cat = 'bersaglio'
        elif c in MISSILI_CLASSI: continue   # il missile in volo e' la stessa voce dell'arma (sotto)
        elif c in ELICOTTERI: cat = 'elicottero'
        elif c in AEREI: cat = 'aereo'
        elif tu in ('ssn', 'ssk', 'ssbn', 'ssgn'): cat = 'sottomarino'
        else: cat = 'nave'
        metti(cat, c, v['nome'], K.nazione_classe(c), f'classi.toml [{c}]')
    for a, v in K.carico['armi'].items():
        cat = {'siluro': 'siluro', 'missile': 'missile', 'mina': 'mina', 'esca': 'esca'}[categoria_arma(v)]
        metti(cat, a, v['nome'], v['nazione'], f'carico.toml [armi.{a}]')
    metti('missile', 'p700', K.classi['p700']['nome'], 'urss', 'classi.toml [p700]')
    for s, v in K.siluri['siluri'].items():
        if f'siluro.{s}' not in ind: metti('siluro', s, v['nome'], 'urss' if s in ('at1', 'at2', 'umgt1', 'apr2') else
                                           ('uk' if s in ('tigerfish', 'spearfish') else 'svezia' if s == 'tp42' else 'usa'),
                                           f'siluri.toml [siluri.{s}]')
    for m in K.carico['mine']:
        if f'mina.{m}' not in ind: metti('mina', m, m.upper(), 'usa', f'carico.toml [mine.{m}]')
    for b, v in K.asw['boe'].items():
        metti('boa', b, v['nome'], v['parte'], f'asw_aereo.toml [boe.{b}]')
    for mk, v in K.asw['marcatori'].items():
        metti('marcatore', mk, v['nome'], v['parte'], f'asw_aereo.toml [marcatori.{mk}]')
    for s, (nome, naz, _) in SISTEMI_SONAR.items():
        metti('sonar', s, nome, naz, 'acustica.toml [sensori]')
    # radar ed ESM: la nazione delle classi che li portano, se e' una sola (Sea Giraffe svedese, Type 967 britannico,
    # DR-2000 tedesco); se no la regola di prima (i radar sovietici per ruolo, gli altri «usa», gli ESM WLR americani)
    def nazione_portatori(chiave, mappa):
        naz = {K.nazione_classe(c) for c, l in mappa.items() if chiave in (l if isinstance(l, list) else [l])}
        naz.discard('neutrale')
        return naz.pop() if len(naz) == 1 else None
    for r, v in K.emettitori['radar'].items():
        naz = 'neutrale' if r == 'nav_civ' else nazione_portatori(r, K.emettitori['suite'])   # il radar dei mercantili
        metti('radar', r, v['nome'], naz or ('urss' if v.get('ruolo', '').endswith('sov') else 'usa'), f'emettitori.toml [radar.{r}]')
    for e, v in K.emettitori['esm'].items():
        naz = nazione_portatori(e, K.emettitori.get('esm_di', {}))
        metti('esm', e, v['nome'], naz or ('usa' if e.startswith('wlr') else 'urss'), f'emettitori.toml [esm.{e}]')
    for p, v in K.periscopi['periscopi'].items():
        metti('periscopio', p, v['nome'], v['nazione'], f'periscopi.toml [periscopi.{p}]')
    for t, v in K.teatri.items():
        metti('teatro', t, v['nome'], 'neutrale', 'tools/pipeline/teatri.yaml')
    for b, v in K.basi.items():
        metti('base', b, v['nome'], v['nazione'], f'tools/pipeline/basi.yaml [{b}]')
    for si, v in K.sistemi.items():
        metti('sistema', si, v['nome'], v['nazione'], f'tools/pipeline/sistemi_bordo.yaml [{si}]')
    return ind


# --- schede per categoria ---
def scheda_classe(K, c):
    v = K.classi[c]
    f, conf = v['fonte'], v['affidabilita']
    r = [riga('lunghezza', 'Lunghezza', 'Length', v['lunghezza_m'], 'm', f, conf, f'classi.toml [{c}] lunghezza_m'),
         riga('larghezza', 'Larghezza' if c not in AEREI | ELICOTTERI else 'Apertura', 'Beam' if c not in AEREI | ELICOTTERI else 'Span',
              v['larghezza_m'], 'm', f, conf, f'classi.toml [{c}] larghezza_m'),
         riga('velocita', 'Velocità massima', 'Maximum speed', v['velocita_max_nodi'], 'kn', f, conf, f'classi.toml [{c}] velocita_max_nodi')]
    if 'pescaggio_m' in v:
        r.append(riga('pescaggio', 'Pescaggio', 'Draught', v['pescaggio_m'], 'm', f, conf, f'classi.toml [{c}] pescaggio_m'))
    fi = K.acustica['firme'].get(c)
    if fi:
        r.append(riga('pale', 'Eliche: pale', 'Propeller blades', fi['pale'], '', f'acustica.toml: {fi["fonte"]}', 'media',
                      f'acustica.toml [firme.{c}] pale'))
        if fi['v_cavitazione_kn'] > 0:   # 0 nel modello: navi che cavitano sempre (niente soglia da mostrare)
            r.append(riga('cavitazione', 'Inizio della cavitazione', 'Cavitation onset', fi['v_cavitazione_kn'], 'kn',
                          f'acustica.toml: {fi["fonte"]}', 'media', f'acustica.toml [firme.{c}] v_cavitazione_kn'))
    pc = K.periscopi['classi'].get(c)
    if pc:
        nomi = ', '.join(K.periscopi['periscopi'][p]['nome'] for p in pc['periscopi'])
        r.append(riga('periscopi', 'Periscopi', 'Periscopes', nomi, '', f'periscopi.toml: {pc["fonte"]}', pc['conf'],
                      f'periscopi.toml [classi.{c}] periscopi'))
        r.append(riga('quota_periscopica', 'Quota periscopica (chiglia)', 'Periscope depth (keel)', pc['quota_m'], 'm',
                      f'periscopi.toml: {pc["fonte"]}', 'STIMA' if 'quota_m' in pc['stime'] else pc['conf'],
                      f'periscopi.toml [classi.{c}] quota_m'))
    lo = K.carico['locali'].get(c)
    if lo:
        r.append(riga('tubi', 'Tubi lanciasiluri', 'Torpedo tubes', lo['tubi'], f'× {lo["calibro_mm"]} mm', lo['fonte'], 'alta',
                      f'carico.toml [locali.{c}] tubi'))
        r.append(riga('armi', 'Armi a bordo', 'Weapons carried', lo['tubi'] + lo['rastrelliera'], '', lo['fonte'], 'media',
                      f'carico.toml [locali.{c}] tubi + rastrelliera',
                      testo_it=f'{lo["tubi"] + lo["rastrelliera"]} ({lo["tubi"]} nei tubi e {lo["rastrelliera"]} in rastrelliera)',
                      testo_en=f'{lo["tubi"] + lo["rastrelliera"]} ({lo["tubi"]} in the tubes, {lo["rastrelliera"]} in the racks)'))
        r.append(riga('esche', 'Esche', 'Decoys', lo['esche'], '', lo['fonte'], 'media', f'carico.toml [locali.{c}] esche'))
    mz = K.asw['mezzi'].get(c)
    if mz:
        r.append(riga('radar', 'Radar', 'Radar', K.emettitori['radar'].get(mz['radar'], {}).get('nome', mz['radar'] or '—'), '',
                      f'asw_aereo.toml: {mz["fonte"]}', mz['conf'], f'asw_aereo.toml [mezzi.{c}] radar'))
        r.append(riga('quota_ricerca', 'Quota di ricerca', 'Search altitude', mz['quota_ricerca_m'], 'm',
                      f'asw_aereo.toml: {mz["fonte"]}', mz['conf'], f'asw_aereo.toml [mezzi.{c}] quota_ricerca_m'))
        r.append(riga('tempo_in_zona', 'Tempo in zona (base)', 'Time on station (base)', mz['tos_base_h'], 'h',
                      f'asw_aereo.toml: {mz["fonti"].get("tos_base_h", mz["fonte"])}', mz['conf'], f'asw_aereo.toml [mezzi.{c}] tos_base_h'))
        if mz.get('sonar'):
            r.append(riga('sonar', 'Sonar a immersione', 'Dipping sonar', K.asw['sonar_immersione'][mz['sonar']]['nome'], '',
                          f'asw_aereo.toml: {mz["fonte"]}', mz['conf'], f'asw_aereo.toml [mezzi.{c}] sonar'))
    return r


def scheda_siluro(K, s):
    r = []
    a = K.carico['armi'].get(s)
    t = K.siluri['siluri'].get(s)
    if a:
        f, conf = a['fonte'], a['confidenza']
        r += [riga('guida', 'Guida', 'Guidance', a['guida'], '', f, conf, f'carico.toml [armi.{s}] guida', testo_en=GUIDA_EN[a['guida']]),
              riga('lunghezza', 'Lunghezza', 'Length', a['lunghezza_m'], 'm', f, conf, f'carico.toml [armi.{s}] lunghezza_m'),
              riga('diametro', 'Diametro', 'Diameter', a['diametro_mm'], 'mm', f, conf, f'carico.toml [armi.{s}] diametro_mm'),
              riga('peso', 'Peso', 'Weight', a['peso_kg'], 'kg', f, conf, f'carico.toml [armi.{s}] peso_kg'),
              riga('testata', 'Testata', 'Warhead', a['testata_kg'], 'kg', f, conf, f'carico.toml [armi.{s}] testata_kg'),
              riga('servizio', 'In servizio dal', 'In service from', a['in_servizio_dal'], '', f, conf,
                   f'carico.toml [armi.{s}] in_servizio_dal', dec=0, testo_it=str(a['in_servizio_dal']), testo_en=str(a['in_servizio_dal']))]
    if a and not t:   # siluri senza scheda di corsa (Shkval): velocita' e portata dal carico
        r += [riga('velocita', 'Velocità', 'Speed', a['velocita_kn'], 'kn', a['fonte'], a['confidenza'], f'carico.toml [armi.{s}] velocita_kn'),
              riga('portata', 'Portata', 'Range', a['portata_km'], 'km', a['fonte'], a['confidenza'], f'carico.toml [armi.{s}] portata_km')]
    if t:
        f = t['fonte']
        r += [riga('velocita_alta', 'Velocità alta', 'High speed', t['velocita_alta_kn'], 'kn', f, 'media', f'siluri.toml [siluri.{s}] velocita_alta_kn'),
              riga('portata_alta', 'Portata a velocità alta', 'Range at high speed', t['portata_alta_m'] / 1000.0, 'km', f, 'media',
                   f'siluri.toml [siluri.{s}] portata_alta_m'),
              riga('velocita_bassa', 'Velocità bassa', 'Low speed', t['velocita_bassa_kn'], 'kn', f, 'media', f'siluri.toml [siluri.{s}] velocita_bassa_kn'),
              riga('portata_bassa', 'Portata a velocità bassa', 'Range at low speed', t['portata_bassa_m'] / 1000.0, 'km', f, 'media',
                   f'siluri.toml [siluri.{s}] portata_bassa_m'),
              riga('quota_max', 'Quota massima', 'Maximum depth', t['quota_max_m'], 'm', f, 'media', f'siluri.toml [siluri.{s}] quota_max_m')]
        if t['filoguidato']:
            r.append(riga('filo', 'Filo (dal siluro / dal battello)', 'Wire (torpedo / boat)', t['filo_siluro_m'] / 1000.0, 'km', f, 'STIMA',
                          f'siluri.toml [siluri.{s}] filo_siluro_m, filo_battello_m',
                          testo_it=f'{numero(t["filo_siluro_m"] / 1000, 0, "it")} km / {numero(t["filo_battello_m"] / 1000, 0, "it")} km',
                          testo_en=f'{numero(t["filo_siluro_m"] / 1000, 0, "en")} km / {numero(t["filo_battello_m"] / 1000, 0, "en")} km'))
        teste = [x for x, b in (('attiva', t['attivo']), ('passiva', t['passivo']), ('segui-scia', t['segui_scia'])) if b]
        teste_en = [x for x, b in (('active', t['attivo']), ('passive', t['passivo']), ('wake-homing', t['segui_scia'])) if b]
        r.append(riga('testa', 'Testa di ricerca', 'Homing head', ', '.join(teste), '', f, 'media', f'siluri.toml [siluri.{s}] attivo, passivo, segui_scia',
                      testo_en=', '.join(teste_en)))
    return r


def scheda_missile(K, m):
    a = K.carico['armi'].get(m)
    if not a:   # P-700: solo la classe in volo
        return scheda_classe(K, m)
    f, conf = a['fonte'], a['confidenza']
    r = [riga('guida', 'Guida', 'Guidance', a['guida'], '', f, conf, f'carico.toml [armi.{m}] guida', testo_en=GUIDA_EN[a['guida']]),
         riga('velocita', 'Velocità', 'Speed', a['velocita_kn'], 'kn', f, conf, f'carico.toml [armi.{m}] velocita_kn'),
         riga('portata', 'Portata', 'Range', a['portata_km'], 'km', f, conf, f'carico.toml [armi.{m}] portata_km'),
         riga('testata', 'Testata', 'Warhead', a['testata_kg'], 'kg', f, conf, f'carico.toml [armi.{m}] testata_kg'),
         riga('lunghezza', 'Lunghezza', 'Length', a['lunghezza_m'], 'm', f, conf, f'carico.toml [armi.{m}] lunghezza_m'),
         riga('peso', 'Peso', 'Weight', a['peso_kg'], 'kg', f, conf, f'carico.toml [armi.{m}] peso_kg'),
         riga('servizio', 'In servizio dal', 'In service from', a['in_servizio_dal'], '', f, conf, f'carico.toml [armi.{m}] in_servizio_dal',
              dec=0, testo_it=str(a['in_servizio_dal']), testo_en=str(a['in_servizio_dal']))]
    if a.get('what_if'):
        r.append(riga('what_if', 'Nel 1989', 'In 1989', 'non ancora in servizio', '', f, conf, f'carico.toml [armi.{m}] what_if',
                      testo_en='not yet in service'))
    return r


def scheda_esm(K, e):
    v = K.emettitori['esm'][e]
    f = 'emettitori.toml (dossier 11 §2)'
    return [riga('banda', 'Banda coperta', 'Band covered', v['ghz_min'], 'GHz', f, 'media', f'emettitori.toml [esm.{e}] ghz_min',
                 testo_it=f'{numero(v["ghz_min"], 2, "it")}-{numero(v["ghz_max"], 0, "it")} GHz',
                 testo_en=f'{numero(v["ghz_min"], 2, "en")}-{numero(v["ghz_max"], 0, "en")} GHz'),
            riga('intercetto', 'Probabilità d\'intercetto', 'Intercept probability', 100 * v['p_intercetto'], '%', f, 'STIMA',
                 f'emettitori.toml [esm.{e}] p_intercetto', dec=0),
            riga('errore', 'Errore di rilevamento (banda I/J)', 'Bearing error (I/J band)', v['errore_ij_gradi'], '°', f, 'STIMA',
                 f'emettitori.toml [esm.{e}] errore_ij_gradi')]


def scheda_catalogo(dati):
    """Basi e sistemi di bordo: le righe vengono gia' dal catalogo (tools/pipeline/basi.yaml, sistemi_bordo.yaml)"""
    return [riga(d['campo'], d['nome_it'], d['nome_en'], d.get('valore', d['testo_it']), d.get('unita', ''), d['fonte'], d['conf'],
                 d.get('dato', 'catalogo dell\'enciclopedia'), testo_it=d['testo_it'], testo_en=d['testo_en']) for d in dati]


def scheda_mina(K, m):
    v = K.carico['mine'][m]
    f = v['fonte']
    a = K.carico['armi'].get(m, {})
    r = [riga('tipo', 'Tipo', 'Type', v['tipo'], '', f, 'media', f'carico.toml [mine.{m}] tipo'),
         riga('carica', 'Carica', 'Charge', v['carica_kg'], 'kg', f, 'alta', f'carico.toml [mine.{m}] carica_kg'),
         riga('raggio', 'Raggio d\'azione', 'Lethal radius', v['raggio_m'], 'm', f, 'STIMA', f'carico.toml [mine.{m}] raggio_m'),
         riga('profondita', 'Profondità di posa', 'Laying depth', v['profondita_min_m'], 'm', f, 'STIMA',
              f'carico.toml [mine.{m}] profondita_min_m, profondita_max_m',
              testo_it=f'{numero(v["profondita_min_m"], 0, "it")}-{numero(v["profondita_max_m"], 0, "it")} m',
              testo_en=f'{numero(v["profondita_min_m"], 0, "en")}-{numero(v["profondita_max_m"], 0, "en")} m'),
         riga('armamento', 'Tempo d\'armamento', 'Arming delay', v['armamento_s'] / 60.0, 'min', f, 'STIMA', f'carico.toml [mine.{m}] armamento_s')]
    if v['corsa_max_m'] > 0:
        r.append(riga('corsa', 'Corsa fino alla posa', 'Run to laying point', v['corsa_max_m'] / 1000.0, 'km', f, 'alta',
                      f'carico.toml [mine.{m}] corsa_max_m'))
    if a:
        r += [riga('lunghezza', 'Lunghezza', 'Length', a['lunghezza_m'], 'm', a['fonte'], a['confidenza'], f'carico.toml [armi.{m}] lunghezza_m'),
              riga('peso', 'Peso', 'Weight', a['peso_kg'], 'kg', a['fonte'], a['confidenza'], f'carico.toml [armi.{m}] peso_kg')]
    bersagli = [x for x, b in (('navi', v['navi']), ('sottomarini', v['sottomarini'])) if b]
    r.append(riga('bersagli', 'Contro', 'Against', ' e '.join(bersagli), '', f, 'media', f'carico.toml [mine.{m}] navi, sottomarini',
                  testo_en=' and '.join({'navi': 'ships', 'sottomarini': 'submarines'}[x] for x in bersagli)))
    return r


def scheda_esca(K, e):
    a = K.carico['armi'].get(e, {})
    if e not in K.siluri['esche']:   # esche senza modello acustico (MG-84): solo i dati del carico
        f, conf = a['fonte'], a['confidenza']
        return [riga('guida', 'Funzionamento', 'Operation', a['guida'], '', f, conf, f'carico.toml [armi.{e}] guida', testo_en=GUIDA_EN[a['guida']]),
                riga('diametro', 'Diametro', 'Diameter', a['diametro_mm'], 'mm', f, conf, f'carico.toml [armi.{e}] diametro_mm'),
                riga('lunghezza', 'Lunghezza', 'Length', a['lunghezza_m'], 'm', f, conf, f'carico.toml [armi.{e}] lunghezza_m'),
                riga('peso', 'Peso', 'Weight', a['peso_kg'], 'kg', f, conf, f'carico.toml [armi.{e}] peso_kg')]
    v = K.siluri['esche'][e]
    r = [riga('livello', 'Livello di rumore', 'Noise level', v['livello_db'], 'dB', v['nota'], 'STIMA', f'siluri.toml [esche.{e}] livello_db'),
         riga('durata', 'Durata', 'Endurance', v['durata_s'] / 60.0, 'min', v['nota'], 'STIMA', f'siluri.toml [esche.{e}] durata_s'),
         riga('quota', 'Quota di stazionamento', 'Hovering depth', v['quota_m'], 'm', v['nota'], 'STIMA', f'siluri.toml [esche.{e}] quota_m')]
    if a:
        r.append(riga('diametro', 'Diametro', 'Diameter', a['diametro_mm'], 'mm', a['fonte'], a['confidenza'], f'carico.toml [armi.{e}] diametro_mm'))
    return r


def scheda_boa(K, b):
    v = K.asw['boe'][b]
    f = v['fonte']
    c = lambda k: 'STIMA' if k in v['stime'] else v['conf']
    funz = {'passiva_omni': ('passiva omnidirezionale', 'passive omnidirectional'), 'passiva_dir': ('passiva direzionale', 'passive directional'),
            'attiva_omni': ('attiva omnidirezionale', 'active omnidirectional'), 'attiva_dir': ('attiva direzionale', 'active directional'),
            'attiva_distanza': ('attiva, solo distanza', 'active, range only'), 'tarata': ('tarata (per i suoni noti)', 'calibrated'),
            'batitermografo': ('batitermografo', 'bathythermograph'), 'esplosiva': ('sorgente esplosiva', 'explosive source'),
            'radar_faro': ('radar-faro', 'radar beacon')}[v['funzione']]
    r = [riga('funzione', 'Funzione', 'Function', funz[0], '', f, v['conf'], f'asw_aereo.toml [boe.{b}] funzione', testo_en=funz[1]),
         riga('quote', 'Quote dell\'idrofono', 'Hydrophone depths', v['quote_m'][0], 'm', f, c('quote_m'), f'asw_aereo.toml [boe.{b}] quote_m',
              testo_it=' / '.join(numero(q, 0, 'it') for q in v['quote_m']) + ' m', testo_en=' / '.join(numero(q, 0, 'en') for q in v['quote_m']) + ' m'),
         riga('vita', 'Durata', 'Life', v['vite_s'][0] / 3600.0, 'h', f, c('vite_s'), f'asw_aereo.toml [boe.{b}] vite_s',
              testo_it=' / '.join(numero(q / 3600, 0, 'it') for q in v['vite_s']) + ' h', testo_en=' / '.join(numero(q / 3600, 0, 'en') for q in v['vite_s']) + ' h')]
    if v['sigma_rilevamento_deg'] > 0:
        r.append(riga('errore', 'Errore di rilevamento (σ)', 'Bearing error (σ)', v['sigma_rilevamento_deg'], '°', f,
                      c('sigma_rilevamento_deg'), f'asw_aereo.toml [boe.{b}] sigma_rilevamento_deg'))
    if v['portata_m'] > 0:
        r.append(riga('portata', 'Portata indicativa (R50)', 'Indicative range (R50)', v['portata_m'] / 1000.0, 'km', f, c('portata_m'),
                      f'asw_aereo.toml [boe.{b}] portata_m'))
    r.append(riga('mare', 'Stato del mare massimo', 'Maximum sea state', v['mare_max'], '', f, c('mare_max'), f'asw_aereo.toml [boe.{b}] mare_max',
                  dec=0, testo_it='nessun limite' if v['mare_max'] >= 9 else f'{v["mare_max"]:.0f}',
                  testo_en='no limit' if v['mare_max'] >= 9 else f'{v["mare_max"]:.0f}'))
    return r


def scheda_sonar(K, s):
    nome, naz, sensori = SISTEMI_SONAR[s]
    r = []
    for se in sensori:
        v = K.acustica['sensori'][se]
        f = f'acustica.toml: {v.get("nota", "")}'
        modo = {'passivo': ('passivo', 'passive'), 'attivo': ('attivo', 'active')}.get(v['modo'], (v['modo'], v['modo']))
        r.append(riga(f'modo_{se}', f'Modo «{se}»', f'Mode «{se}»', modo[0], '', f, 'media', f'acustica.toml [sensori.{se}] modo',
                      testo_en=modo[1]))
        if 'dimensione_m' in v:
            r.append(riga(f'antenna_{se}', 'Antenna', 'Array', v['dimensione_m'], 'm', f, 'media', f'acustica.toml [sensori.{se}] antenna, dimensione_m',
                          testo_it=f'{v.get("antenna", "")} da {numero(v["dimensione_m"], 1, "it")} m',
                          testo_en=f'{v.get("antenna", "")}, {numero(v["dimensione_m"], 1, "en")} m'))
        if 'velocita_max_kn' in v:
            r.append(riga(f'velocita_{se}', 'Velocità massima d\'ascolto', 'Maximum listening speed', v['velocita_max_kn'], 'kn', f, 'media',
                          f'acustica.toml [sensori.{se}] velocita_max_kn'))
        if v.get('baffles_gradi'):
            r.append(riga(f'baffles_{se}', 'Settore cieco di poppa', 'Stern blind sector', 2 * v['baffles_gradi'], '°', f, 'media',
                          f'acustica.toml [sensori.{se}] baffles_gradi (semi-ampiezza)'))
    return r


def scheda_radar(K, r_):
    v = K.emettitori['radar'][r_]
    f = 'emettitori.toml (dossier 11 §1 e §6.4)'
    r = [riga('banda', 'Banda', 'Band', v['banda'], '', f, 'media', f'emettitori.toml [radar.{r_}] banda'),
         riga('frequenza', 'Frequenza', 'Frequency', v['ghz'], 'GHz', f, 'media', f'emettitori.toml [radar.{r_}] ghz'),
         riga('periodo', 'Periodo di rotazione', 'Rotation period', v['periodo_s'], 's', f, 'bassa', f'emettitori.toml [radar.{r_}] periodo_s')]
    if v.get('r50_periscopio_mare2_m'):
        r.append(riga('periscopio_m2', 'Scoperta di un periscopio (R50, mare 2)', 'Periscope detection (R50, sea state 2)',
                      v['r50_periscopio_mare2_m'] / 1852.0, 'nm', f, 'STIMA', f'emettitori.toml [radar.{r_}] r50_periscopio_mare2_m'))
        r.append(riga('periscopio_m4', 'Scoperta di un periscopio (R50, mare 4)', 'Periscope detection (R50, sea state 4)',
                      v['r50_periscopio_mare4_m'] / 1852.0, 'nm', f, 'STIMA', f'emettitori.toml [radar.{r_}] r50_periscopio_mare4_m'))
    if v.get('contro_nave_m'):
        r.append(riga('nave', 'Portata contro una nave grande', 'Range against a large ship', v['contro_nave_m'] / 1852.0, 'nm', f, 'bassa',
                      f'emettitori.toml [radar.{r_}] contro_nave_m'))
    return r


def scheda_periscopio(K, p):
    v = K.periscopi['periscopi'][p]
    f = v['fonte']
    c = lambda k: 'STIMA' if k in v['stime'] else v['conf']
    it_modi = {'giorno': 'giorno', 'bassa_luminosita': 'bassa luminosità', 'intensificatore': 'intensificatore', 'tv': 'TV', 'termico': 'termico'}
    en_modi = {'giorno': 'day', 'bassa_luminosita': 'low light', 'intensificatore': 'image intensifier', 'tv': 'TV', 'termico': 'thermal'}
    ingr = lambda lingua: ', '.join(f'{numero(i, 1, lingua)}× ({numero(cg, 0, lingua)}°)' for i, cg in zip(v['ingrandimenti'], v['campi_gradi']))
    return [
        riga('ingrandimenti', 'Ingrandimenti (campo)', 'Magnifications (field)', v['ingrandimenti'][-1], '×', f, c('ingrandimenti'),
             f'periscopi.toml [periscopi.{p}] ingrandimenti, campi_gradi', testo_it=ingr('it'), testo_en=ingr('en')),
        riga('elevazione', 'Elevazione', 'Elevation', v['elevazione_max'], '°', f, c('elevazione_max'),
             f'periscopi.toml [periscopi.{p}] elevazione_min, elevazione_max',
             testo_it=f'da {numero(v["elevazione_min"], 0, "it")}° a +{numero(v["elevazione_max"], 1, "it")}°',
             testo_en=f'{numero(v["elevazione_min"], 0, "en")}° to +{numero(v["elevazione_max"], 1, "en")}°'),
        riga('telemetro', 'Telemetro', 'Rangefinder', v['telemetro'], '', f, c('telemetro'), f'periscopi.toml [periscopi.{p}] telemetro',
             testo_it='stadimetro a immagine sdoppiata' if v['telemetro'] == 'sdoppiato' else 'reticolo graduato',
             testo_en='split-image stadimeter' if v['telemetro'] == 'sdoppiato' else 'graduated reticle'),
        riga('modi', 'Modi di visione', 'Viewing modes', ', '.join(it_modi[m] for m in v['modi']), '', f, c('modi'),
             f'periscopi.toml [periscopi.{p}] modi', testo_en=', '.join(en_modi[m] for m in v['modi'])),
        riga('esm', 'ESM nella testa', 'ESM in the head', sino(v['esm'])[0], '', f, c('esm'), f'periscopi.toml [periscopi.{p}] esm',
             testo_en=sino(v['esm'])[1]),
        riga('sezione', 'Sezione radar (1 m esposto)', 'Radar cross-section (1 m exposed)', v['sezione_m2'], 'm²', f, c('sezione_m2'),
             f'periscopi.toml [periscopi.{p}] sezione_m2'),
        riga('alzata', 'Tempo d\'alzata', 'Raise time', v['alzata_s'], 's', f, c('alzata_s'), f'periscopi.toml [periscopi.{p}] alzata_s'),
        riga('v_danno', 'Velocità di danno', 'Damage speed', v['v_danno_kn'], 'kn', f, c('v_danno_kn'), f'periscopi.toml [periscopi.{p}] v_danno_kn'),
    ]


def scheda_marcatore(K, m):
    v = K.asw['marcatori'][m]
    c = lambda k: 'STIMA' if k in v['stime'] else v['conf']
    r = [riga('durata', 'Durata', 'Duration', v['durata_s'] / 60.0, 'min', v['fonte'], c('durata_s'), f'asw_aereo.toml [marcatori.{m}] durata_s')]
    if 'visibile_m' in v:
        r.append(riga('visibile', 'Visibile fino a', 'Visible up to', v['visibile_m'] / 1000.0, 'km', v['fonte'], c('visibile_m'),
                      f'asw_aereo.toml [marcatori.{m}] visibile_m'))
    if v.get('acustico'):   # segnalatore acustico (SUS): si sente, non si vede
        r.append(riga('frequenza', 'Segnale acustico', 'Acoustic signal', v['frequenza_hz'] / 1000.0, 'kHz', v['fonte'], c('frequenza_hz'),
                      f'asw_aereo.toml [marcatori.{m}] frequenza_hz'))
    return r


def scheda_teatro(K, t):
    v = K.teatri[t]
    b = v['bbox']
    me = K.meteo['teatri'][t]
    gen, lug = me['mesi']['1'], me['mesi']['7']
    f, fm = 'tools/pipeline/teatri.yaml (dossier 03)', 'meteo.toml (ERA5 1981-1990; dossier 21 §3)'
    return [riga('area', 'Riquadro', 'Box', b[0], '', f, 'alta', 'teatri.yaml bbox',
                 testo_it=f'{numero(b[0], 2, "it")}-{numero(b[1], 2, "it")}° N, {numero(abs(b[2]), 1, "it")}° {"O" if b[2] < 0 else "E"} - '
                          f'{numero(abs(b[3]), 1, "it")}° {"O" if b[3] < 0 else "E"}',
                 testo_en=f'{numero(b[0], 2, "en")}-{numero(b[1], 2, "en")}° N, {numero(abs(b[2]), 1, "en")}° {"W" if b[2] < 0 else "E"} - '
                          f'{numero(abs(b[3]), 1, "en")}° {"W" if b[3] < 0 else "E"}'),
            riga('missioni', 'Missioni del 1989', '1989 missions', len(v['missioni']), '', f, 'alta', 'teatri.yaml missioni',
                 testo_it=', '.join(str(m) for m in v['missioni']), testo_en=', '.join(str(m) for m in v['missioni'])),
            riga('vento_gen', 'Vento medio (gennaio / luglio)', 'Mean wind (January / July)', gen['vento_media_m_s'], 'm/s', fm, 'media',
                 f'meteo.toml [teatri.{t}.mesi.1] vento_media_m_s',
                 testo_it=f'{numero(gen["vento_media_m_s"], 1, "it")} / {numero(lug["vento_media_m_s"], 1, "it")} m/s',
                 testo_en=f'{numero(gen["vento_media_m_s"], 1, "en")} / {numero(lug["vento_media_m_s"], 1, "en")} m/s'),
            riga('onde', 'Onda significativa media (gennaio / luglio)', 'Mean significant wave (January / July)', gen['hs_media_m'], 'm', fm, 'media',
                 f'meteo.toml [teatri.{t}.mesi.1] hs_media_m',
                 testo_it=f'{numero(gen["hs_media_m"], 1, "it")} / {numero(lug["hs_media_m"], 1, "it")} m',
                 testo_en=f'{numero(gen["hs_media_m"], 1, "en")} / {numero(lug["hs_media_m"], 1, "en")} m'),
            riga('burrasche', 'Ore di burrasca ≥ Bft 8 a gennaio', 'Gale hours ≥ Bft 8 in January', gen['bft8_pct'], '%', fm, 'media',
                 f'meteo.toml [teatri.{t}.mesi.1] bft8_pct'),
            riga('ghiaccio', 'Ghiaccio', 'Ice', me['ghiaccio'], '', 'meteo.toml (dossier 21 §3.1)', 'media', f'meteo.toml [teatri.{t}] ghiaccio')]


# --- riferimenti incrociati ---
def riferimenti(K, vid, ind, testi=None):
    cat, ch = vid.split('.', 1)
    out = []
    def r(rel, altro):
        if altro in ind and (rel, altro) not in [(x['relazione'], x['voce']) for x in out]: out.append(dict(relazione=rel, voce=altro))
    armi_di = lambda c: sorted({a for car in K.carico['carichi'].get(c, {}).values() if isinstance(car, dict)
                                for a in list(car.get('tubi', [])) + list(car.get('rastrelliera', {}))})
    if cat in ('sottomarino', 'nave'):
        for a in armi_di(ch):
            r('armi', f'{categoria_arma(K.carico["armi"][a])}.{a}')
        for p in K.periscopi['classi'].get(ch, {}).get('periscopi', []): r('periscopi', f'periscopio.{p}')
        for se in K.acustica['suite'].get(ch, []):
            for s, (_, _, sens) in SISTEMI_SONAR.items():
                if se in sens: r('sensori', f'sonar.{s}')
        for e in [K.emettitori['esm_di'].get(ch)] + list(K.emettitori['suite'].get(ch, [])):
            if e: r('sensori', f'esm.{e}' if f'esm.{e}' in ind else f'radar.{e}')
        if ch in K.carico['locali'] and K.carico['locali'][ch]['esche']:
            r('armi', 'esca.adc_mk2' if K.carico['locali'][ch]['nazione'] == 'usa' else 'esca.mg84')
    if cat in ('aereo', 'elicottero'):
        mz = K.asw['mezzi'].get(ch, {})
        if mz.get('radar'): r('sensori', f'radar.{mz["radar"]}')
        if mz.get('sonar'): r('sensori', f'sonar.{mz["sonar"]}')
        for car in mz.get('carichi', {}).values():
            for b in car.get('boe', {}): r('boe', f'boa.{b}')
            for a in car.get('armi', {}): r('armi', f'siluro.{a}')
            for m in car.get('marcatori', {}): r('marcatori', f'marcatore.{m}')
    if cat in ('siluro', 'missile', 'mina', 'esca'):
        for c, cars in K.carico['carichi'].items():
            if ch in armi_di(c) and f'sottomarino.{c}' in ind: r('portata da', f'sottomarino.{c}')
        for m, mz in K.asw['mezzi'].items():
            if any(ch in car.get('armi', {}) for car in mz.get('carichi', {}).values()):
                r('portata da', f'{"elicottero" if mz["elicottero"] else "aereo"}.{m}')
    if cat == 'esca':
        for c, lo in K.carico['locali'].items():
            if lo['esche'] and K.carico['armi'].get(ch, {}).get('nazione') == lo['nazione'] and f'sottomarino.{c}' in ind:
                r('portata da', f'sottomarino.{c}')
    if cat in ('boa', 'marcatore'):
        chiave = 'boe' if cat == 'boa' else 'marcatori'
        for m, mz in K.asw['mezzi'].items():
            if any(ch in car.get(chiave, {}) for car in mz.get('carichi', {}).values()):
                r('portata da', f'{"elicottero" if mz["elicottero"] else "aereo"}.{m}')
    if cat == 'sonar':
        sens = SISTEMI_SONAR[ch][2]
        for c, su in K.acustica['suite'].items():
            if any(s in su for s in sens):
                r('montato su', f'sottomarino.{c}' if f'sottomarino.{c}' in ind else f'nave.{c}')
        for m, mz in K.asw['mezzi'].items():
            if mz.get('sonar') == ch: r('montato su', f'{"elicottero" if mz["elicottero"] else "aereo"}.{m}')
    if cat == 'radar':
        for c, su in K.emettitori['suite'].items():
            if ch in su: r('montato su', f'nave.{c}' if f'nave.{c}' in ind else f'sottomarino.{c}')
        for m, mz in K.asw['mezzi'].items():
            if mz.get('radar') == ch: r('montato su', f'{"elicottero" if mz["elicottero"] else "aereo"}.{m}')
    if cat == 'periscopio':
        for c, pc in K.periscopi['classi'].items():
            if ch in pc['periscopi']: r('montato su', f'sottomarino.{c}')
    # armi tipiche delle classi dell'IA (dai testi, dai dossier 01 e 14: non dal carico) e il loro rovescio
    testi = testi or {}
    for a in testi.get(vid, {}).get('armi_tipiche', {}).get('voci', []): r('armi tipiche', a)
    if cat in ('siluro', 'missile', 'mina', 'esca'):
        for altro, t in sorted(testi.items()):
            if vid in t.get('armi_tipiche', {}).get('voci', []): r('portata da (tipica)', altro)
    if cat == 'base':
        r('teatro', f'teatro.{K.basi[ch]["teatro"]}')
    if cat == 'teatro':
        for b, v in K.basi.items():
            if v['teatro'] == ch: r('basi', f'base.{b}')
    if cat == 'sistema':
        for c in K.sistemi[ch].get('classi', []): r('montato su', f'sottomarino.{c}')
    return [x for x in out if x['voce'] in ind]


# --- immagini e crediti ---
# cascate del Museo (engine/grafica/cascata_firma): chi passa davanti ai sonar di una parte, e chi ascolta una classe
BERSAGLIO_CASCATA = {'usa': 'victor3', 'uk': 'victor3', 'urss': 'los_angeles'}
ASCOLTATORE_CASCATA = {True: 'sovietico_3gen', False: 'bqq5_sfera'}   # la classe e' della NATO?


def sensore_cascata(K, ch):
    """Il passivo del sistema sonar per la cascata; i soli attivi ascoltano col passivo della loro piattaforma."""
    for s in SISTEMI_SONAR[ch][2]:
        if K.acustica['sensori'].get(s, {}).get('modo') == 'passivo': return s
        if K.acustica['sensori'].get(s + '_passivo', {}).get('modo') == 'passivo': return s + '_passivo'
    return {'usa': 'sqs53', 'urss': 'nave_sovietica'}[SISTEMI_SONAR[ch][1]]


def cartella_classe(c):
    return {'oscar': 'oscar2'}.get(c, c)


def immagini_e_crediti(K, vid, uscita_foto):
    cat, ch = vid.split('.', 1)
    imm, cred = [], []
    radice = K.radice
    if cat in ('sottomarino', 'nave', 'aereo', 'elicottero', 'bersaglio') and ch in K.classi:
        v = K.classi[ch]
        volante = cat in ('aereo', 'elicottero')
        viste = [(90, True, 0), (45, True, 0), (135, False, 0), (0, True, 0)] if not volante else \
                [(90, True, 0), (45, True, 10), (135, False, 25)]
        for aob, dritta, el in viste:
            imm.append(dict(tipo='sprite', file=v['sprite'], aob=aob, dritta=dritta, elevazione=el,
                            didascalia_it=f'in gioco, AOB {aob}° {"dritta" if dritta else "sinistra"}' + (f', da {el}° sotto' if el else ''),
                            didascalia_en=f'in game, AOB {aob}° {"starboard" if dritta else "port"}' + (f', from {el}° below' if el else '')))
        if 'modello' in v:
            imm.append(dict(tipo='modello', file=v['modello'], aob=60, dritta=True, elevazione=0,
                            didascalia_it='modello 3D in posa da museo', didascalia_en='3D model in museum pose'))
        derivati = {'data/motore/' + v['sprite'], 'data/motore/' + v.get('modello', '-')}
        for a in K.crediti['asset']:   # solo le opere di terzi: le voci del progetto e dell'originale non sono foto
            if derivati & set(a['derivati']) and a.get('categoria', 'terzi') == 'terzi':
                cred.append(dict(titolo=a['titolo'], autore=a['autore'], licenza=a['licenza'], url=a['url'], stato=a['stato'], file=a['file']))
        # foto digitalizzate «alla 1989» dalle foto del kit con licenza libera
        fonti = os.path.join(radice, 'extracted', 'navi', cartella_classe(ch), 'foto', 'fonti.json')
        if os.path.exists(fonti):
            k = 0
            for fo in json.load(open(fonti)):
                lic = (fo.get('licenza') or '').lower()
                if not fo.get('file'): continue   # voce senza file (foto solo consultata o scartata)
                f = os.path.join(os.path.dirname(fonti), fo['file'])
                if not any(l in lic for l in LICENZE_LIBERE) or not os.path.exists(f) or k >= 2: continue
                k += 1
                nome = f'{vid}_foto{k}'
                digitalizza(f, os.path.join(uscita_foto, nome), radice)
                imm.append(dict(tipo='foto', file=f'immagini/enciclopedia/{nome}.img', palette='palette/giorno.pal',
                                didascalia_it=fo.get('nave', ''), didascalia_en=fo.get('nave', ''), licenza=fo.get('licenza', ''),
                                autore=fo.get('autore', '')))
                gia = os.path.relpath(f, radice)
                if not any(c.get('file') == gia for c in cred):   # la stessa foto degli sprite: un credito solo
                    cred.append(dict(titolo=fo.get('titolo_commons', fo['file']).replace('File:', ''), autore=fo.get('autore', ''),
                                     licenza=fo.get('licenza', ''), url=fo.get('pagina', ''), file=gia,
                                     stato='condivisione' if 'sa' in lic.replace('-', ' ').split() or 'by-sa' in lic else 'attribuzione'))
    if cat in ('siluro', 'mina', 'esca', 'missile') and ch in K.carico['armi']:
        imm.append(dict(tipo='sagoma', arma=ch, didascalia_it='sagoma in scala (sala siluri)', didascalia_en='scale silhouette (torpedo room)'))
    elif cat == 'siluro' and ch in SAGOME_SENZA_CARICO:   # i siluri senza voce nel carico: sagoma_da_id della grafica
        imm.append(dict(tipo='sagoma', arma=ch, didascalia_it='sagoma in scala', didascalia_en='scale silhouette'))
    if cat == 'missile' and ch in MISSILI_CLASSI.values():
        for c, a in MISSILI_CLASSI.items():
            if a == ch: imm.append(dict(tipo='sprite', file=K.classi[c]['sprite'], aob=90, dritta=True, elevazione=0,
                                        didascalia_it='in volo', didascalia_en='in flight'))
    if cat == 'periscopio':
        v = K.periscopi['periscopi'][ch]
        imm.append(dict(tipo='oculare', periscopio=ch, ingrandimento=len(v['ingrandimenti']) - 1, modo='giorno',
                        didascalia_it=f'nell\'oculare a {numero(v["ingrandimenti"][-1], 0, "it")}×',
                        didascalia_en=f'through the eyepiece at {numero(v["ingrandimenti"][-1], 0, "en")}×'))
        if len(v['modi']) > 1:
            imm.append(dict(tipo='oculare', periscopio=ch, ingrandimento=1, modo=v['modi'][1],
                            didascalia_it=f'modo {v["modi"][1].replace("_", " ")}', didascalia_en=f'{v["modi"][1].replace("_", " ")} mode'))
    if cat == 'boa':
        imm.append(dict(tipo='effetto', effetto='boa', oggetto=ch, didascalia_it='in mare, vista dal periscopio',
                        didascalia_en='at sea, seen through the periscope'))
    if cat == 'marcatore':
        imm.append(dict(tipo='effetto', effetto='marcatore', oggetto=ch, didascalia_it='in mare, vista dal periscopio',
                        didascalia_en='at sea, seen through the periscope'))
    if cat == 'sonar':
        imm.append(dict(tipo='cascata', sensore=sensore_cascata(K, ch), classe=BERSAGLIO_CASCATA[SISTEMI_SONAR[ch][1]],
                        didascalia_it='cascata dimostrativa', didascalia_en='demonstration waterfall'))
    if cat in ('sottomarino', 'nave') and ch in K.acustica['firme']:
        imm.append(dict(tipo='cascata', classe=ch, sensore=ASCOLTATORE_CASCATA[K.nazione_classe(ch) != 'urss'],
                        didascalia_it='la sua firma: cascata dimostrativa', didascalia_en='its signature: demonstration waterfall'))
    if cat == 'sistema':
        for c in K.sistemi[ch].get('classi', []):
            imm.append(dict(tipo='spaccato', classe=c, sistema=ch, didascalia_it=f'nello spaccato ({c})', didascalia_en=f'in the cutaway ({c})'))
    if cat == 'base':
        v = K.basi[ch]
        # la carta della zona (tools/pipeline/carte_basi.py); se non c'e' ancora, quella d'epoca del teatro, che puo' non coprirla
        if os.path.exists(f'{K.radice}/data/motore/basi/{ch}.fdl'):
            imm.append(dict(tipo='carta', file=f'basi/{ch}.fdl', lat=v['lat'], lon=v['lon'],
                            didascalia_it='la carta della zona', didascalia_en='chart of the area'))
        else:
            imm.append(dict(tipo='carta', file=f'teatri/{v["teatro"]}_carta_{"urss" if v["nazione"] == "urss" else "nato"}.fdl',
                            lat=v['lat'], lon=v['lon'], didascalia_it='sulla carta d\'epoca', didascalia_en='on the period chart'))
    if cat == 'teatro':
        for p in ('nato', 'urss'):
            imm.append(dict(tipo='carta', file=f'teatri/{ch}_carta_{p}.fdl', didascalia_it=f'carta d\'epoca ({p.upper()})',
                            didascalia_en=f'period chart ({p.upper()})'))
    return imm, cred


def digitalizza(sorgente, uscita, radice):
    """Foto «alla 1989» con le regole degli sprite fotografici dell'originale (tools/museo/digitalizza.py, variante
    epoca16): scala di grigi, livelli ai percentili 1-99, nitidezza leggera, ritaglio al centro a 4:3 e 320x200 (i pixel
    del 320x200 non erano quadrati), 16 grigi della palette del gioco (indici 144-158) con la diffusione d'errore di
    Floyd-Steinberg. Indici della palette del giorno: il Museo la mostra senza cambiare palette. File .img (IMG1)."""
    if os.path.exists(uscita + '.img') and os.path.getmtime(uscita + '.img') > os.path.getmtime(sorgente): return
    sys.path.insert(0, os.path.join(radice, 'tools', 'museo'))
    from digitalizza import floyd, livelli   # noqa: E402
    pal = open(os.path.join(radice, 'data', 'motore', 'palette', 'giorno.pal'), 'rb').read()
    rampa = np.array([pal[3 * i] for i in range(144, 159)], float)
    im = Image.open(sorgente).convert('L')
    w, h = im.size
    if w / h > 4 / 3: nw, nh = int(h * 4 / 3), h
    else: nw, nh = w, int(w * 3 / 4)
    im = im.crop(((w - nw) // 2, (h - nh) // 2, (w - nw) // 2 + nw, (h - nh) // 2 + nh))
    im = im.filter(ImageFilter.UnsharpMask(radius=1.2, percent=60, threshold=2)).resize((320, 200), Image.LANCZOS)
    k = floyd(livelli(np.asarray(im, float)), rampa).astype(np.uint8) + 144
    os.makedirs(os.path.dirname(uscita), exist_ok=True)
    open(uscita + '.img', 'wb').write(b'IMG1' + np.array([320, 200], '<u2').tobytes() + k.tobytes())


# --- valori reali accanto a quelli del gioco ---
CAMPI_CLASSE = {'lunghezza': 'lunghezza_m', 'larghezza': 'larghezza_m', 'velocita': 'velocita_max_nodi', 'pescaggio': 'pescaggio_m'}


def applica_reali(K, vid, scheda):
    """Dove il dossier dice altro dal gioco, la riga mostra il valore reale con la sua fonte e porta il valore del gioco
    (gioco_valore, gioco_testo_it/en, gioco_fonte, dato_gioco). Chiavi di enciclopedia_reali.yaml: l'id della classe
    (campi di classi.toml) o l'id della voce (campi della scheda)."""
    cat, ch = vid.split('.', 1)
    originale = ch in K.classi and 'tipo_originale' in K.classi[ch]
    for r in scheda:
        reale, chiave = None, None
        if ch in K.classi and r['campo'] in CAMPI_CLASSE:
            chiave = CAMPI_CLASSE[r['campo']]
            reale = (K.reali.get(ch) or {}).get(chiave)
        elif vid in K.reali:
            chiave = r['campo']
            reale = K.reali[vid].get(chiave)
        if not reale: continue
        if 'valore' in reale:
            v, g = float(reale['valore']), r['valore']
            if isinstance(g, (int, float)) and abs(v - g) <= 0.01 * max(abs(v), 1e-9): continue   # coincide: resta il gioco
            d = 0 if v.is_integer() else (2 if abs(v) < 10 else 1)
            ti = numero(v, d, 'it') + (f' {r["unita"]}' if r['unita'] else '')
            te = numero(v, d, 'en') + (f' {r["unita"]}' if r['unita'] else '')
        else:
            v, ti, te = reale['testo_it'], reale['testo_it'], reale['testo_en']
        qual = 'nel gioco del 1989' if originale and r['campo'] == 'velocita' else 'nel gioco'
        qual_en = 'in the 1989 game' if originale and r['campo'] == 'velocita' else 'in the game'
        r.update(gioco_valore=r['valore'], gioco_testo_it=f'{qual}: {r["testo_it"]}', gioco_testo_en=f'{qual_en}: {r["testo_en"]}',
                 gioco_fonte=r['fonte'], dato_gioco=r['dato'], valore=v, testo_it=ti, testo_en=te, fonte=reale['fonte'],
                 conf=confidenza(reale['conf']), dato=f'enciclopedia_reali.yaml [{ch if ch in K.classi else vid}] {chiave}')
        if reale.get('nota'): r['nota'] = reale['nota']
        if reale.get('gioco_it'): r.update(gioco_testo_it=reale['gioco_it'], gioco_testo_en=reale['gioco_en'])


# --- voci ---
SCHEDE = {'sottomarino': scheda_classe, 'nave': scheda_classe, 'aereo': scheda_classe, 'elicottero': scheda_classe,
          'siluro': scheda_siluro, 'mina': scheda_mina, 'esca': scheda_esca, 'boa': scheda_boa, 'sonar': scheda_sonar,
          'radar': scheda_radar, 'periscopio': scheda_periscopio, 'marcatore': scheda_marcatore, 'teatro': scheda_teatro,
          'missile': scheda_missile, 'esm': scheda_esm, 'bersaglio': scheda_classe,
          'base': lambda K, b: scheda_catalogo(K.basi[b]['dati']), 'sistema': lambda K, si: scheda_catalogo(K.sistemi[si]['dati'])}


def voce(K, vid, ind, testi, uscita_foto):
    cat, ch = vid.split('.', 1)
    t = testi[vid]
    for k in ('it', 'en', 'fonti'):
        assert t.get(k), (vid, k)
    if cat in ('siluro', 'mina', 'esca', 'missile'):
        assert t.get('uso_it') and t.get('uso_en'), (vid, 'uso')
    scheda = SCHEDE[cat](K, ch)
    applica_reali(K, vid, scheda)
    for r in scheda:
        assert r['fonte'] and r['conf'] in ('alta', 'media', 'bassa', 'STIMA'), (vid, r['campo'])
        if ha_cirillico(r['testo_it']): r['testo_lat'] = latino(r['testo_it'])
    imm, cred = immagini_e_crediti(K, vid, uscita_foto)
    nome_en = ind[vid]['nome']
    return dict(nome=ind[vid]['nome'], nome_en=nome_en, nome_lat=ind[vid].get('nome_lat', ''), categoria=cat, gruppo=GRUPPI[cat], gruppo_en=GRUPPI_EN[cat],
                nazione=ind[vid]['nazione'], testo_it=' '.join(t['it'].split()), testo_en=' '.join(t['en'].split()),
                uso_it=' '.join(t.get('uso_it', '').split()), uso_en=' '.join(t.get('uso_en', '').split()), fonti=t['fonti'],
                scheda=scheda, riferimenti=riferimenti(K, vid, ind, testi), immagini=imm, crediti=cred,
                armi_tipiche_altre=t.get('armi_tipiche', {}).get('altre', []),
                armi_tipiche_fonte=t.get('armi_tipiche', {}).get('fonte', ''))


def _campo(k, v):
    if isinstance(v, list): return f'{k} = [' + ', '.join(valore(x) for x in v) + ']'
    return f'{k} = {valore(v)}'


def carica_testi(radice):
    """Tutti i file .yaml di tools/pipeline/enciclopedia_testi/ (uno per gruppo di voci): nessun id due volte"""
    cartella = os.path.join(radice, 'tools', 'pipeline', 'enciclopedia_testi')
    testi = {}
    for f in sorted(os.listdir(cartella)):
        if not f.endswith('.yaml'): continue
        for k, v in (yaml.safe_load(open(os.path.join(cartella, f))) or {}).items():
            assert k not in testi, f'voce {k} scritta due volte ({f})'
            testi[k] = v
    return testi


def esporta_enciclopedia(radice):
    K = Cataloghi(radice)
    ind = indice(K)
    testi = carica_testi(radice)
    ignoti = set(testi) - set(ind)
    assert not ignoti, f'testi senza voce nell\'indice: {ignoti}'
    uscita_foto = os.path.join(radice, 'data', 'motore', 'immagini', 'enciclopedia')
    righe = ['# Enciclopedia dell\'Enhanced (generato da tools/pipeline/enciclopedia.py: non modificare a mano).',
             '# Dati dai cataloghi del motore, testi da tools/pipeline/enciclopedia_testi.yaml (dossier di docs/ricerca).',
             '# conf: alta, media, bassa, STIMA. Gli id sono «categoria.chiave».', '']
    for vid, i in sorted(ind.items()):
        righe += [f'[indice."{vid}"]', *(_campo(k, v) for k, v in i.items()), f'scritta = {valore(vid in testi)}', '']
    n = 0
    for vid in sorted(testi):
        v = voce(K, vid, ind, testi, uscita_foto)
        righe += [f'[voci."{vid}"]', *(_campo(k, v[k]) for k in ('nome', 'nome_en', 'nome_lat', 'categoria', 'gruppo', 'gruppo_en', 'nazione',
                                                                  'testo_it', 'testo_en', 'uso_it', 'uso_en', 'fonti',
                                                                  'armi_tipiche_altre', 'armi_tipiche_fonte')), '']
        for sez in ('scheda', 'riferimenti', 'immagini', 'crediti'):
            for x in v[sez]:
                righe += [f'[[voci."{vid}".{sez}]]', *(_campo(k, w) for k, w in x.items()), '']
        n += 1
    open(os.path.join(radice, 'data', 'motore', 'enciclopedia.toml'), 'w').write('\n'.join(righe))
    return len(ind), n


if __name__ == '__main__':
    print('voci nell\'indice, voci scritte:', esporta_enciclopedia(sys.argv[1] if len(sys.argv) > 1 else '.'))
