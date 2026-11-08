"""I fari dei teatri da Wikidata (CC0) -> extracted/fari_wikidata/<teatro>.csv (download autorizzato dall'utente il
31/10/2026; scelta della principale dopo il portale NGA fermo).

Una interrogazione SPARQL per teatro, sul riquadro di tools/pipeline/teatri.yaml: gli elementi che sono fari
(Q39715, anche per sottoclasse) con le coordinate dentro il riquadro, e quando ci sono la data di prima accensione
(P571 fondazione, P729 entrata in servizio), la dismissione (P576, P730), la caratteristica della luce (P1030),
l'altezza focale (P2923), la portata (P2929), l'altezza della torre (P2048) e i numeri NGA (P3563) e ARLHS (P2980).
I file scaricati sono dati: si leggono soltanto.

Uso: fari_wikidata.py <radice> [teatro ...]
"""
import csv, io, os, sys, time, urllib.parse, urllib.request

import yaml

ENDPOINT = 'https://query.wikidata.org/sparql'
UA = {'User-Agent': 'melchior-688-kit/0.1 (fari dei teatri, dati CC0)', 'Accept': 'text/csv'}

QUERY = """
SELECT ?faro ?nome ?coord ?inizio ?servizio ?fine ?ritiro ?caratteristica ?focale ?portata ?altezza ?nga ?arlhs WHERE {{
  ?faro wdt:P31/wdt:P279* wd:Q39715 .
  SERVICE wikibase:box {{
    ?faro wdt:P625 ?coord .
    bd:serviceParam wikibase:cornerSouthWest "Point({lon0} {lat0})"^^geo:wktLiteral .
    bd:serviceParam wikibase:cornerNorthEast "Point({lon1} {lat1})"^^geo:wktLiteral .
  }}
  OPTIONAL {{ ?faro wdt:P571 ?inizio . }}
  OPTIONAL {{ ?faro wdt:P729 ?servizio . }}
  OPTIONAL {{ ?faro wdt:P576 ?fine . }}
  OPTIONAL {{ ?faro wdt:P730 ?ritiro . }}
  OPTIONAL {{ ?faro wdt:P1030 ?caratteristica . }}
  OPTIONAL {{ ?faro wdt:P2923 ?focale . }}
  OPTIONAL {{ ?faro wdt:P2929 ?portata . }}
  OPTIONAL {{ ?faro wdt:P2048 ?altezza . }}
  OPTIONAL {{ ?faro wdt:P3563 ?nga . }}
  OPTIONAL {{ ?faro wdt:P2980 ?arlhs . }}
  SERVICE wikibase:label {{ bd:serviceParam wikibase:language "en,it,fo,no,ru,es,fr,de,sv,da,pl,ja" . ?faro rdfs:label ?nome . }}
}}
"""


def interroga(lat0, lat1, lon0, lon1):
    q = QUERY.format(lat0=lat0, lat1=lat1, lon0=lon0, lon1=lon1)
    req = urllib.request.Request(ENDPOINT + '?' + urllib.parse.urlencode({'query': q}), headers=UA)
    for k in range(4):
        try:
            return urllib.request.urlopen(req, timeout=90).read().decode('utf-8')
        except Exception as e:   # il servizio limita le richieste: si riprova con calma
            if k == 3:
                raise
            print('riprovo:', e)
            time.sleep(65 * (k + 1))   # il servizio puo' limitare a una richiesta al minuto


def main():
    radice = sys.argv[1] if len(sys.argv) > 1 else '.'
    teatri = {t['id']: t for t in yaml.safe_load(open(os.path.join(radice, 'tools', 'pipeline', 'teatri.yaml')))['teatri']}
    scelti = sys.argv[2:] or [t for t in teatri if 'bbox' in teatri[t]]
    uscita = os.path.join(radice, 'extracted', 'fari_wikidata')
    os.makedirs(uscita, exist_ok=True)
    for t in scelti:
        if os.path.exists(os.path.join(uscita, f'{t}.csv')):   # gia' scaricato
            continue
        lat0, lat1, lon0, lon1 = teatri[t]['bbox']
        testo = interroga(lat0, lat1, lon0, lon1)
        righe = list(csv.DictReader(io.StringIO(testo)))
        with open(os.path.join(uscita, f'{t}.csv'), 'w', encoding='utf-8') as f:
            f.write(testo)
        print(f'{t}: {len(righe)} righe, {len(testo)} byte', flush=True)
        time.sleep(65)
    with open(os.path.join(uscita, 'CREDITI.md'), 'w', encoding='utf-8') as f:
        f.write('# I fari dei teatri\n\nDa Wikidata (https://www.wikidata.org), dati sotto CC0 1.0, interrogati il '
                f'{time.strftime("%d/%m/%Y")} con tools/pipeline/fari_wikidata.py (download autorizzato dall\'utente '
                'il 31/10/2026). Una riga per valore: un faro con piu\' valori di una proprieta\' compare piu\' volte.\n')


if __name__ == '__main__':
    main()
