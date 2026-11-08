"""Navigazione e incertezza della posizione propria (roadmap 26; docs/08 §6; dossier docs/ricerca/06 §2, §6 e §7.3) ->
data/motore/navigazione.toml. Lo legge nucleo/navigazione.hpp (carica_navigazione): i sistemi di navigazione con la
loro deriva, quale sistema ha ogni classe, i punti nave con precisione, condizioni e costo, e per ogni teatro la
latitudine, la corrente non nota (per la stima a inerziale guasto) e la copertura delle radionavigazioni.
Tutto vale per il giocatore e per l'IA allo stesso modo (docs/18_interazioni_ia.md). Quasi tutto e' STIMA DI GIOCO del
dossier 06: le prestazioni vere sono classificate.
Uso: navigazione.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

# id: nome, deriva (nm/giorno: l'errore radiale a 1 sigma dopo un giorno senza punti nave), ampiezza di Schuler (nm),
# correlazione dell'errore di velocita' (ore: oltre questa l'errore smette di crescere in linea retta), fonte, confidenza
# (correlazione: STIMA DI GIOCO per tutti; per la stimata e' quanto resta uguale la corrente)
SISTEMI = {
    'dmins': ('AN/WSN-1 DMINS', 1.5, 0.15, 72.0, '06 §2.1: STIMA DI GIOCO (deriva non pubblicata)', 'bassa'),
    'wsn3': ('AN/WSN-3 ESGN', 0.5, 0.15, 72.0, '06 §2.1: giroscopio elettrostatico dei 688 con Tomahawk, STIMA', 'bassa'),
    'sins_uk': ('SINS Mk 2', 1.0, 0.15, 72.0, '06 §2.1 [RNSINS]: classe 1 nm/giorno', 'media'),
    'medvedica': ('Medvedica', 1.75, 0.15, 72.0, '06 §6: STIMA 1,5-2 nm/giorno', 'bassa'),
    'soz': ('Soz', 3.0, 0.15, 72.0, '06 §6: STIMA 3 nm/giorno (Alfa)', 'bassa'),
    'sigma': ('Sigma', 3.0, 0.15, 72.0, '06 §6: STIMA 3 nm/giorno (Victor I/II, Charlie)', 'bassa'),
    # senza inerziale: solcometro e girobussola, l'errore e' la corrente non nota del teatro (06 §2.1)
    'stimata': ('Solcometro e girobussola', 0.0, 0.0, 12.0, '06 §2.1: la stima accumula la corrente non nota', 'media'),
}
SCHULER_MIN = 84.4        # periodo di Schuler (minuti): fisica, alta confidenza
LATITUDINE_POLARE = 80.0  # oltre: la deriva inerziale raddoppia (06 §2.1, STIMA)
FATTORE_POLARE = 2.0
# classe (o classe_variante della sala siluri) -> sistema
CLASSI = {
    'los_angeles': 'dmins', 'los_angeles_flight2': 'wsn3', 'los_angeles_688i': 'wsn3',
    'sturgeon': 'dmins', 'trafalgar': 'sins_uk',
    'victor3': 'medvedica', 'akula': 'medvedica', 'oscar': 'medvedica', 'typhoon': 'medvedica',
    'alfa': 'soz', 'charlie2': 'sigma',
    'kilo': 'stimata', 'foxtrot': 'stimata',   # i diesel: girobussola e solcometro (STIMA)
}
SISTEMA_PREDEFINITO = 'stimata'   # navi e classi senza dato: si fanno punti nave spesso, costa e radar

# punti nave. sigma_min-sigma_max: precisione a 1 sigma (nm); per_kn: nm in piu' per nodo d'errore di velocita';
# durata_s: l'esposizione; quota_max_m e velocita_max_kn: le condizioni; albero: serve un albero fuori (antenna o
# periscopio); filo: basta l'antenna a filo galleggiante; emissione: emette (radar, sondaggi); cielo, notte, costa_km:
# condizioni; teatri: la copertura (vuoto = ovunque); anno_dal: dal quale e' disponibile; marina: chi lo usa (nato =
# tutte le marine occidentali); opzione: serve l'opzione di partita; attesa_40/60/70: minuti medi fra due passaggi di
# satellite alle latitudini 40, 60 e 70; scartati: la quota dei passaggi inutili (troppo alti o bassi);
# corsia_nm: la larghezza della corsia (Omega: il fix e' ambiguo di una corsia se la stima e' fuori di mezza corsia);
# allungamento: quanto il fix e' meno preciso lungo la direzione debole (l'isobata per il batimetrico);
# frazione_isobate: la precisione e' questa frazione della spaziatura delle isobate; carta: eredita l'errore della carta
_P = dict(sigma_min=0.0, sigma_max=0.0, per_kn=0.0, durata_s=0.0, quota_max_m=1e9, velocita_max_kn=1e9, albero=False,
          filo=False, emissione=False, cielo=False, notte=False, costa_km=0.0, teatri=[], anno_dal=0,
          marina='tutte', opzione='', attesa_40=0.0, attesa_60=0.0, attesa_70=0.0, scartati=0.0, corsia_nm=0.0,
          allungamento=1.0, frazione_isobate=0.0, carta=False, fari_min=0, fonte='', confidenza='')
PUNTI = {
    'transit': dict(_P, nome='Transit (NAVSAT)', sigma_min=0.05, sigma_max=0.1, per_kn=0.2, durata_s=120.0,
                    quota_max_m=18.0, velocita_max_kn=8.0, albero=True, marina='nato', anno_dal=1964,
                    attesa_40=100.0, attesa_60=60.0, attesa_70=40.0, scartati=0.25,
                    fonte='06 §2.2 [APL-Danchik] [WPTransit] [GSTransit]; attese STIMA', confidenza='alta'),
    'gps': dict(_P, nome='GPS', sigma_min=0.01, sigma_max=0.01, durata_s=60.0, quota_max_m=18.0, albero=True,
                marina='nato', anno_dal=1990, opzione='gps', fonte='06 §2.3: assente nel 1989, opzione «1990+»',
                confidenza='alta'),
    'loran_c': dict(_P, nome='Loran-C', sigma_min=0.1, sigma_max=0.25, durata_s=180.0, quota_max_m=18.0, albero=True,
                    marina='nato', anno_dal=1958, teatri=['faroe', 'otranto', 'kunashir', 'barents', 'baltico', 'celtica'],
                    fonte='06 §2.4 [WPLoran]: Norvegia, Mediterraneo, Hokkaido; Barents solo occidentale, Baltico parziale',
                    confidenza='media'),
    'cajka': dict(_P, nome='Cajka', sigma_min=0.1, sigma_max=0.3, durata_s=180.0, quota_max_m=18.0, albero=True,
                  marina='vmf', anno_dal=1970, teatri=['barents', 'baltico', 'kunashir'],
                  fonte='06 §6 [WPLoran]: l\'equivalente sovietico del Loran-C, nelle acque sovietiche (STIMA)',
                  confidenza='bassa'),
    'omega': dict(_P, nome='Omega', sigma_min=2.0, sigma_max=4.0, durata_s=300.0, quota_max_m=60.0, filo=True,
                  marina='nato', anno_dal=1971, corsia_nm=8.0,
                  fonte='06 §2.4, §7.3 [WPOmega]: 2-4 nm, ambiguita\' di corsia se sigma > 4 nm',
                  confidenza='alta'),
    'rsdn20': dict(_P, nome='RSDN-20 «Alfa»', sigma_min=1.35, sigma_max=3.8, durata_s=300.0, quota_max_m=60.0, filo=True,
                   marina='vmf', anno_dal=1970, fonte='06 §6 [WPAlpha]: 2,5-7 km, ricevibile in immersione',
                   confidenza='media'),
    'parus': dict(_P, nome='Parus (Ciklon-B)', sigma_min=0.05, sigma_max=0.15, per_kn=0.2, durata_s=120.0,
                  quota_max_m=18.0, velocita_max_kn=8.0, albero=True, marina='vmf', anno_dal=1974,
                  attesa_40=110.0, attesa_60=70.0, attesa_70=45.0, scartati=0.25,
                  fonte='06 §6 [WPParus]: come il Transit (STIMA)', confidenza='bassa'),
    'astronomico': dict(_P, nome='Astronomico al periscopio', sigma_min=1.0, sigma_max=3.0, durata_s=450.0,
                        quota_max_m=18.0, albero=True, cielo=True, notte=True,
                        fonte='06 §2.5 [USNI21]: STIMA 1-3 nm, 5-10 min, sereno, crepuscolo o notte', confidenza='media'),
    'radar_costa': dict(_P, nome='Radar sulla costa', sigma_min=0.05, sigma_max=0.2, durata_s=60.0, quota_max_m=18.0,
                        albero=True, emissione=True, costa_km=37.0, carta=True,
                        fonte='06 §7.1, §7.3: entro 20 nm dalla costa, emissione radar', confidenza='media'),
    'visivo': dict(_P, nome='Visivo su punti cospicui', sigma_min=0.05, sigma_max=0.1, durata_s=60.0, quota_max_m=18.0,
                   albero=True, costa_km=20.0, carta=True, fonte='06 §7.3: periscopio, costa in vista', confidenza='media'),
    'fari': dict(_P, nome='Rilevamenti di fari', sigma_min=0.1, sigma_max=0.3, durata_s=120.0, quota_max_m=18.0,
                 albero=True, notte=True, fari_min=2, carta=True,
                 fonte='06 §7.3: due rilevamenti di luci di costa al periscopio di notte (fari.toml del kit, Wikidata); '
                 'STIMA 0,1-0,3 nm, l\'errore di un grado a 10 nm', confidenza='bassa'),
    'batimetrico': dict(_P, nome='Batimetrico (contorni)', sigma_min=0.05, sigma_max=0.5, durata_s=900.0,
                        emissione=True, frazione_isobate=0.25, allungamento=3.0, carta=True, fonte='06 §2.6 [BOW26]: max(1/4 della spaziatura delle isobate, 100 yd) '
                        'piu\' l\'errore della carta; serie di sondaggi su rotta costante', confidenza='alta'),
}
# teatro: latitudine del centro (gradi), corrente non nota (kn, per la stima a inerziale guasto), fonte
TEATRI = {
    'faroe': (62.0, 0.4), 'barents': (72.0, 0.4), 'kunashir': (44.5, 0.8), 'celtica': (50.0, 0.4),
    'otranto': (40.5, 0.3), 'baltico': (56.0, 0.2), 'gibilterra': (36.0, 1.0),
}
FONTE_TEATRI = '06 §7.3: corrente non nota 0,2-1 kn, alta negli stretti (Gibilterra, Kunashir); STIMA'


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, (int, float)):
        return repr(float(v))
    if isinstance(v, str):
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list):
        return '[' + ', '.join(valore(x) for x in v) + ']'
    raise TypeError(v)


def esporta_navigazione(radice):
    righe = ['# Navigazione: sistemi, punti nave, teatri (generato da tools/pipeline/navigazione.py: non modificare a mano).',
             f'sistema_predefinito = {valore(SISTEMA_PREDEFINITO)}', '']
    righe[2:2] = [f'schuler_min = {valore(SCHULER_MIN)}', f'latitudine_polare = {valore(LATITUDINE_POLARE)}',
                  f'fattore_polare = {valore(FATTORE_POLARE)}']
    for k, (nome, deriva, schuler, corr, fonte, conf) in SISTEMI.items():
        righe += [f'[sistemi.{k}]', f'nome = {valore(nome)}', f'deriva_nm_giorno = {valore(deriva)}',
                  f'schuler_nm = {valore(schuler)}', f'correlazione_h = {valore(corr)}', f'fonte = {valore(fonte)}',
                  f'confidenza = {valore(conf)}', '']
    righe += ['[classi]', *(f'{c} = {valore(s)}' for c, s in CLASSI.items()), '']
    assert all(s in SISTEMI for s in CLASSI.values()) and SISTEMA_PREDEFINITO in SISTEMI
    for k, p in PUNTI.items():
        assert set(p) <= set(_P) | {'nome'}, k
        assert p['sigma_min'] <= p['sigma_max'] and all(t in TEATRI for t in p['teatri']), k
        righe.append(f'[punti.{k}]')
        righe += [f'{c} = {valore(v)}' for c, v in p.items()]
        righe.append('')
    for t, (lat, corr) in TEATRI.items():
        righe += [f'[teatri.{t}]', f'latitudine = {valore(lat)}', f'corrente_kn = {valore(corr)}', f'fonte = {valore(FONTE_TEATRI)}', '']
    open(os.path.join(radice, 'data', 'motore', 'navigazione.toml'), 'w').write('\n'.join(righe))
    return len(SISTEMI), len(PUNTI), len(TEATRI)


if __name__ == '__main__':
    print('sistemi, punti nave, teatri:', esporta_navigazione(sys.argv[1] if len(sys.argv) > 1 else '.'))
