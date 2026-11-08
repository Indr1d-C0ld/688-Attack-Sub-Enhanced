"""I fari dei teatri (C9 §2.15, kit 31/10/2026) -> data/motore/fari.toml, dai CSV di Wikidata (CC0) scaricati da
tools/pipeline/fari_wikidata.py in extracted/fari_wikidata/<teatro>.csv.

Per ogni faro:
- `teatro`, `nome`, `lat`, `lon` (gradi WGS84; il motore li porta nel piano della missione col fondale);
- `quota_m`: l'altezza focale sul mare (P2923); se manca, la torre piu' 10 m o 20 m (STIMA);
- `portata_nm`: la portata nominale (P2929; la maggiore se ce ne sono piu', una per colore); se manca 15 miglia
  (STIMA);
- `colore`: bianco, rosso, verde o giallo, il primo della caratteristica (i settori colorati non si fanno: STIMA);
- `caratteristica` come la scrive l'elenco dei fari («LFl(2) WRG 20s»), `periodo_s` e `lampi`: la sequenza di accesi e
  spenti in secondi dentro il periodo, a cominciare da acceso (vedi `sequenza`; durate STIMA dove l'elenco non le
  dice). Senza caratteristica «Fl W 10s» (STIMA);
- `anno`: la prima accensione se Wikidata la dice (P571, P729), 0 se non la sa;
- `stime`: i campi stimati.
Si tengono i fari accesi nel 1989: accesi entro il 1989 o con l'anno ignoto (STIMA), e non spenti prima del 1990.

Uso: fari.py <radice>   (lo chiama anche esporta_motore.py)
"""
import csv, os, re, sys

COLORI = {'W': 'bianco', 'R': 'rosso', 'G': 'verde', 'Y': 'giallo'}
CARATTERISTICA_STIMA = 'Fl W 10s'
LAMPO_S, LAMPO_LUNGO_S, PAUSA_S = 0.5, 2.0, 1.5   # durate tipiche (STIMA: l'elenco da' solo il periodo)


def sequenza(car):
    """(periodo, [acceso, spento, acceso, ...]) dalla caratteristica; None se non si capisce"""
    s = car.replace('.', ' ').replace(',', ' ')
    m = re.match(r'\s*(Al\s*)?(LFl|Fl|Oc|Iso|VQ|Q|Mo|F)\s*(\(([0-9+]+|[A-Z])\))?', s)
    if not m:
        return None
    tipo, gruppo = m.group(2), m.group(4)
    p = re.search(r'(\d+(?:\s\d+)?)\s*s\b', s[m.end():].replace(' s', 's'))
    periodo = float(p.group(1).replace(' ', '.')) if p else None
    n = sum(int(x) for x in gruppo.split('+')) if gruppo and gruppo[0].isdigit() else 1
    if tipo == 'F':
        return 0.0, []                                     # fisso: sempre acceso
    if tipo in ('Q', 'VQ'):
        d = 0.5 if tipo == 'Q' else 0.25                   # 60 e 120 lampi al minuto
        if gruppo and gruppo[0].isdigit():                 # Q(3) 10s: tre lampi rapidi e buio
            seq = [d, d] * n
            per = periodo or 10.0
            seq[-1] = max(per - sum(seq[:-1]), d)
            return per, seq
        return 2 * d, [d, d]
    if tipo == 'Iso':
        per = periodo or 4.0
        return per, [per / 2, per / 2]
    if tipo == 'Oc':                                       # occultazioni: spento 1 s n volte, il resto acceso
        per = periodo or 10.0
        seq = []
        for _ in range(n):
            seq += [1.0, 1.0]
        seq[0] = max(per - (2 * n - 1) * 1.0, 1.0)         # la luce lunga, poi le n eclissi brevi
        return per, seq
    d = LAMPO_LUNGO_S if tipo == 'LFl' else LAMPO_S        # Fl, LFl, Mo (come un lampo: STIMA)
    per = periodo or max(10.0, n * (d + PAUSA_S) + 3.0)
    seq = []
    for _ in range(n):
        seq += [d, PAUSA_S]
    seq[-1] = max(per - sum(seq[:-1]), PAUSA_S)
    return per, seq


def colore(car):
    m = re.search(r'\b([WRGY]+)\b', car.split('(')[-1] if '(' in car else car)
    lettere = m.group(1) if m else 'W'
    return COLORI['W'] if 'W' in lettere else COLORI[lettere[0]]


def anno(v):
    m = re.match(r'(-?\d{4})', v or '')
    return int(m.group(1)) if m else 0


def fari(radice):
    cartella = os.path.join(radice, 'extracted', 'fari_wikidata')
    out = []
    if not os.path.isdir(cartella):
        return out
    for f in sorted(os.listdir(cartella)):
        if not f.endswith('.csv'):
            continue
        teatro = f[:-4]
        per_faro = {}
        for r in csv.DictReader(open(os.path.join(cartella, f), encoding='utf-8')):
            per_faro.setdefault(r['faro'], []).append(r)
        for qid, righe in per_faro.items():
            def tutti(k):
                return [r[k] for r in righe if r.get(k)]
            m = re.match(r'Point\(([-0-9.]+) ([-0-9.]+)\)', righe[0]['coord'])
            if not m:
                continue
            lon, lat = float(m.group(1)), float(m.group(2))
            inizio = min([anno(x) for x in tutti('inizio') + tutti('servizio') if anno(x)] or [0])
            fine = min([anno(x) for x in tutti('fine') + tutti('ritiro') if anno(x)] or [0])
            if inizio > 1989 or (fine and fine < 1990):
                continue
            stime = []
            car = (tutti('caratteristica') or [''])[0].strip()
            seq = sequenza(car) if car else None
            if not seq:
                car, seq = CARATTERISTICA_STIMA, sequenza(CARATTERISTICA_STIMA)
                stime.append('caratteristica')
            focali = [float(x) for x in tutti('focale')]
            altezze = [float(x) for x in tutti('altezza')]
            if focali:
                quota = max(focali)
            else:
                quota = max(altezze) + 10.0 if altezze else 20.0
                stime.append('quota_m')
            portate = [float(x) for x in tutti('portata')]
            portata = max(portate) if portate else 15.0
            if not portate:
                stime.append('portata_nm')
            if not inizio:
                stime.append('anno')
            out.append(dict(teatro=teatro, id=qid.rsplit('/', 1)[-1], nome=righe[0]['nome'], lat=lat, lon=lon,
                            quota_m=round(quota, 1), portata_nm=round(portata, 1), colore=colore(car),
                            caratteristica=car, periodo_s=round(seq[0], 2), lampi=[round(x, 2) for x in seq[1]],
                            anno=inizio, nga=(tutti('nga') or [''])[0], stime=stime))
    return out


def toml_str(s):
    return '"' + str(s).replace('\\', '\\\\').replace('"', '\\"') + '"'


def esporta_fari(radice):
    elenco = fari(radice)
    righe = ['# I fari dei teatri nel 1989: generato da tools/pipeline/fari.py da Wikidata (CC0), non modificare a mano.',
             '# Le durate dei lampi sono STIMA dove la caratteristica da\' solo il periodo; i settori colorati non ci sono.', '']
    for x in elenco:
        righe += ['[[faro]]', *(f'{k} = {toml_str(v) if isinstance(v, str) else ("[" + ", ".join(toml_str(e) if isinstance(e, str) else repr(e) for e in v) + "]" if isinstance(v, list) else repr(v))}'
                                 for k, v in x.items()), '']
    with open(os.path.join(radice, 'data', 'motore', 'fari.toml'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(righe))
    return len(elenco)


if __name__ == '__main__':
    print('fari:', esporta_fari(sys.argv[1] if len(sys.argv) > 1 else '.'))
