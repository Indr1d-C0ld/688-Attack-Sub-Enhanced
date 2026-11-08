"""Unisce la traduzione italiana dei dialoghi di missione.

Entrate: docs/traduzione/sorgenti/SCENE*.json (da lza_testi.py), docs/traduzione/lotti/lotto_*.json
(prosa tradotta a mano, per id di paragrafo). Le intestazioni si traducono con le regole del glossario
(docs/traduzione/glossario.md).
Uscite: docs/traduzione/memoria_it.json, docs/traduzione/scene/SCENE*.json (pagine con en/it, per il
motore) e docs/traduzione/scene/SCENE*.md (bilingue, per la revisione).
Esce con errore se un paragrafo resta senza traduzione.
"""
import os, re, sys, glob, json

CLASSIFICHE = {'(UNCLASSIFIED)': '(NON CLASSIFICATO)', '(CONFIDENTIAL)': '(RISERVATISSIMO)',
               '(SECRET)': '(SEGRETO)', '(TOP SECRET)': '(SEGRETISSIMO)'}
GRADI = [('VITSE ADMIRAL ', 'VICEAMMIRAGLIO '), ('KONTR ADMIRAL ', 'CONTRAMMIRAGLIO '),
         ('VADM ', 'VICEAMMIRAGLIO '), ('RADM ', 'CONTRAMMIRAGLIO ')]
DESTINATARI = {
    'ALL SUBS ATLANTIC': 'TUTTI I SOTTOMARINI ATLANTICO', 'ALL SUBS PACIFIC': 'TUTTI I SOTTOMARINI PACIFICO',
    'ALL SHIPS ATLANTIC': 'TUTTE LE UNITÀ ATLANTICO', 'ALL SHIPS TORPEX': 'TUTTE LE UNITÀ TORPEX',
    'ALL SHIPS SEVERNY FLOT': 'TUTTE LE UNITÀ SEVERNY FLOT', 'ALL SHIPS SEVERNYI FLOT': 'TUTTE LE UNITÀ SEVERNYI FLOT',
    'ALL SHIPS TIKHIJ OKEANSKIJ FLOT': 'TUTTE LE UNITÀ TIKHIJ OKEANSKIJ FLOT',
}
OGGETTI = {
    'ADRIATIC ESCORT': 'SCORTA IN ADRIATICO', 'AMERICAN CONVOY INTERCEPT': 'INTERCETTAZIONE DEL CONVOGLIO AMERICANO',
    'ATLANTIC CROSSING': 'TRAVERSATA ATLANTICA', 'ATLANTIC TRAGEDY': 'TRAGEDIA IN ATLANTICO',
    'ATTACK GROUP ESCORT': 'SCORTA AL GRUPPO D\'ATTACCO', 'BOOMER BANGER': 'CACCIA AL LANCIAMISSILI',
    'COASTAL PATROL': 'PATTUGLIAMENTO COSTIERO', 'COASTAL PATROL TRAGEDY': 'TRAGEDIA DEL PATTUGLIAMENTO COSTIERO',
    'CONVOY INTERCEPT': 'INTERCETTAZIONE DEL CONVOGLIO', 'ESCAPE': 'FUGA',
    'FAEROE ISLANDS RANGE': 'POLIGONO DELLE FÆR ØER', 'FAEROE ISLANDS RANGE ACCIDENT': 'INCIDENTE AL POLIGONO DELLE FÆR ØER',
    'FAEROE ISLANDS RANGE RESULTS': 'RISULTATI AL POLIGONO DELLE FÆR ØER',
    'FAEROE ISLANDS RANGE TRAGEDY': 'TRAGEDIA AL POLIGONO DELLE FÆR ØER', 'HIT AND RUN': 'COLPISCI E FUGGI',
    'LENINSKII KOMSOMOL SINKING': 'AFFONDAMENTO DEL LENINSKII KOMSOMOL',
    'LENINSKII KOMSOMOL TRAGEDY': 'TRAGEDIA DEL LENINSKII KOMSOMOL', 'MEDITERRANEAN ACCIDENT': 'INCIDENTE NEL MEDITERRANEO',
    'MEDOPS': 'OPERAZIONI NEL MEDITERRANEO', 'NORTH SEA OPERATIONS': 'OPERAZIONI NEL MARE DEL NORD',
    'PLA LENINSKII KOMSOMOL TRAGEDY': 'TRAGEDIA DELLA PLA LENINSKII KOMSOMOL', 'SEVERNY TRAGEDY': 'TRAGEDIA NEL SEVERNY',
    'SOVIET BOOMER INTERCEPT': 'INTERCETTAZIONE DEL LANCIAMISSILI SOVIETICO', 'SOVIET INTERCEPT': 'INTERCETTAZIONE SOVIETICA',
    'SSBN PROTECTION': 'PROTEZIONE DELL\'SSBN', 'SUBOPS NLANT': 'OPERAZIONI SUBACQUEE NORD ATLANTICO',
    'SURVEILLANCE MISSION': 'MISSIONE DI SORVEGLIANZA', 'TASK FORCE ESCORT': 'SCORTA AL GRUPPO NAVALE',
    'TORPEX NLANT': 'TORPEX NORD ATLANTICO', 'U.S. SURFACE FLEET': 'FLOTTA DI SUPERFICIE STATUNITENSE',
    'U.S. SURFACE SHIPS': 'UNITÀ DI SUPERFICIE STATUNITENSI',
}

def intestazione(en):
    if en in CLASSIFICHE: return CLASSIFICHE[en]
    if en == 'MISSION ORDERS': return 'ORDINI DI MISSIONE'
    if re.fullmatch(r'\*+ EMERGENCY \*+', en): return en.replace('EMERGENCY', 'EMERGENZA')
    m = re.fullmatch(r'CODE: (\S+)', en)
    if m: return f'CODICE: {m.group(1)}'
    m = re.fullmatch(r'FM: (.+)', en)
    if m: return f'DA: {m.group(1)}'
    m = re.fullmatch(r'INFO: (.+)', en)
    if m: return f'P.C.: {DESTINATARI.get(m.group(1), m.group(1))}'
    m = re.fullmatch(r'TO: (.+)', en)
    if m: return f'A: {DESTINATARI.get(m.group(1), m.group(1))}'
    m = re.fullmatch(r'SUBJ: (.+)', en)
    if m and m.group(1) in OGGETTI: return f'OGG.: {OGGETTI[m.group(1)]}'
    for a, b in GRADI:
        if en.startswith(a): return b + en[len(a):]
    return None

if __name__ == '__main__':
    root = sys.argv[1]; base = f'{root}/docs/traduzione'
    prosa = {}
    for f in sorted(glob.glob(f'{base}/lotti/lotto_*.json')):
        prosa.update(json.load(open(f)))
    mem = json.load(open(f'{base}/memoria_en.json'))
    it, mancano = {}, []
    for k, v in mem.items():
        t = prosa.get(k) if v['tipo'] == 'testo' else intestazione(v['en'])
        if t is None: mancano.append((k, v['en']))
        else: it[k] = t
    if mancano:
        for k, e in mancano: print('MANCA', k, e)
        sys.exit(f'{len(mancano)} paragrafi senza traduzione')
    json.dump(it, open(f'{base}/memoria_it.json', 'w'), indent=1, ensure_ascii=False)
    os.makedirs(f'{base}/scene', exist_ok=True)
    parole_en = parole_it = 0
    for f in sorted(glob.glob(f'{base}/sorgenti/SCENE*.json')):
        sc = json.load(open(f))
        md = [f"# {sc['scena']}: testi di missione (EN → IT)", '']
        for pg in sc['pagine']:
            for p in pg['paragrafi']:
                p['it'] = it[p['id']]
                parole_en += len(p['en'].split()); parole_it += len(p['it'].split())
            titolo = {'ordini': 'Ordini di missione', 'rapporto': 'Intestazione del rapporto finale'}.get(pg['ruolo'], f"Esito {pg['esito']}")
            md += [f'## {titolo}', '', '| EN | IT |', '|---|---|']
            md += [f"| {p['en']} | {p['it']} |" for p in pg['paragrafi']]
            md.append('')
        json.dump(sc, open(f"{base}/scene/{sc['scena']}.json", 'w'), indent=1, ensure_ascii=False)
        open(f"{base}/scene/{sc['scena']}.md", 'w').write('\n'.join(md))
    print(f'scene: {len(glob.glob(base + "/scene/*.json"))}  paragrafi unici: {len(it)}  '
          f'parole EN {parole_en} -> IT {parole_it} ({parole_it / parole_en:.0%})')
