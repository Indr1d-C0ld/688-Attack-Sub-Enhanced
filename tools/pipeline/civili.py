"""Tipi di navi civili per il traffico dell'Enhanced (blocco 1 del sonar, docs/20_sonar_blocco1.md §2).

Scrive:
- la sezione [civili.*] di data/motore/acustica.toml (chiamata da acustica.py): gli intervalli da cui il motore genera
  ogni nave (engine/nucleo/navi_civili.hpp);
- le classi nuove in data/motore/classi.toml (chiamata da esporta_motore.py dopo esporta_classi, o da sola), col
  modello, lo sprite e lo scafo della classe piu' vicina finche' il kit non ne fa di propri, e `civile = true` anche
  sulle classi civili di sempre.

Fonti: docs/ricerca/26_navi_civili_traffico.md (appunti in 26_note/rumore_navi.md):
- livelli: JOMOPANS-ECHO (MacGillivray e de Jong 2021, CC BY), classi e V_C della Tabella 1; McKenna 2012, Veirs 2016
  per i controlli;
- propulsione per tipo negli anni '80: ITTC 2014, Arveson e Vendittis 2000 e le navi d'esempio del §3 degli appunti.
I numeri senza una fonte puntuale (cilindri, giri dei generatori, rete, innesco della cavitazione) sono STIMA, e cosi'
`epoca_db`: la correzione che porta la media dei livelli di servizio sulle misure (prove `navi civili:`). Il
rimorchiatore ha +13 dB perche' ECHO lo misura a 3,7 kn (lavoro in porto) e qui naviga a 11-15 kn; Veirs 2016 da'
170 +- 5 dB.
"""
import os
import re

# Colonne (intervalli [min, max]): lunghezza, pescaggio, velocita' di servizio, innesco della cavitazione (kn);
# assi, pale, giri dell'asse al servizio; motori, cilindri, giri del motore (diesel col riduttore, diesel-generatori
# del diesel-elettrico, rotore della turbina); generatori, loro cilindri e giri.
TIPI = {
    'mercantile': dict(
        nome='Mercantile da carico generale', categoria='mercantile', classe='mercantile',
        lunghezza_m=[90, 180], pescaggio_m=[6, 9], velocita_servizio_kn=[13, 17], cavitazione_kn=[8, 11],
        larghezza_rel=0.14, echo_vc_kn=13.9, echo_d_lf=0.8, epoca_db=-1.0,
        propulsione='diesel_lento', passo_variabile=False, assi=[1, 1], pale=[4, 4], giri_asse=[110, 160],
        motori=[1, 1], cilindri=[5, 9], giri_motore=[110, 160],
        generatori=[2, 3], cilindri_generatori=[6, 8], giri_generatori=[720, 1200], rete_60=0.5,
        fonte='26 §3 cargo generale; ECHO Bulker (V_C 13,9 kn)', confidenza='media'),
    'cargo': dict(
        nome='Portacontainer', categoria='mercantile', classe='cargo',
        lunghezza_m=[150, 290], pescaggio_m=[9, 12], velocita_servizio_kn=[18, 23], cavitazione_kn=[9, 12],
        larghezza_rel=0.13, echo_vc_kn=18.0, echo_d_lf=0.8, epoca_db=1.0,
        propulsione='diesel_lento', passo_variabile=False, assi=[1, 1], pale=[5, 6], giri_asse=[80, 110],
        motori=[1, 1], cilindri=[6, 12], giri_motore=[80, 110],
        generatori=[3, 4], cilindri_generatori=[6, 9], giri_generatori=[720, 1200], rete_60=0.6,
        fonte='26 §3 portacontainer (Econship 7RLB90 a 98 giri); ECHO Containership', confidenza='media'),
    'portarinfuse': dict(
        nome='Portarinfuse', categoria='mercantile', classe='portarinfuse',
        lunghezza_m=[150, 300], pescaggio_m=[9, 15], velocita_servizio_kn=[13, 15], cavitazione_kn=[7, 10],
        larghezza_rel=0.15, echo_vc_kn=13.9, echo_d_lf=0.8, epoca_db=2.0,
        propulsione='diesel_lento', passo_variabile=False, assi=[1, 1], pale=[4, 5], giri_asse=[80, 140],
        motori=[1, 1], cilindri=[4, 8], giri_motore=[80, 140],
        generatori=[2, 3], cilindri_generatori=[6, 8], giri_generatori=[720, 1200], rete_60=0.6,
        fonte='26 §2.1 Overseas Harriette, §3 portarinfuse; ECHO Bulker', confidenza='media'),
    'petroliera': dict(
        nome='Petroliera grande (VLCC a turbina)', categoria='petroliera', classe='petroliera',
        lunghezza_m=[230, 370], pescaggio_m=[14, 22], velocita_servizio_kn=[14, 16], cavitazione_kn=[7, 10],
        larghezza_rel=0.18, echo_vc_kn=12.4, echo_d_lf=1.0, epoca_db=-3.0,
        propulsione='turbina', passo_variabile=False, assi=[1, 1], pale=[5, 6], giri_asse=[75, 90],
        motori=[1, 1], cilindri=[0, 0], giri_motore=[4000, 6000],
        generatori=[1, 2], cilindri_generatori=[6, 8], giri_generatori=[720, 1200], rete_60=0.6,
        fonte='26 §3 VLCC (Mobil Hawk, turbina GE, elica a 90 giri); ECHO Tanker', confidenza='media'),
    'petroliera_costiera': dict(
        nome='Petroliera costiera e chimichiera', categoria='petroliera', classe='petroliera_costiera',
        lunghezza_m=[60, 180], pescaggio_m=[5, 10], velocita_servizio_kn=[12, 15], cavitazione_kn=[8, 11],
        larghezza_rel=0.16, echo_vc_kn=12.4, echo_d_lf=1.0, epoca_db=1.0,
        propulsione='diesel_medio', passo_variabile=False, assi=[1, 1], pale=[4, 4], giri_asse=[150, 250],
        motori=[1, 1], cilindri=[6, 12], giri_motore=[500, 750],
        generatori=[2, 3], cilindri_generatori=[6, 6], giri_generatori=[1200, 1800], rete_60=0.4,
        fonte='26 §3 petroliera prodotti e chimica; McKenna 2012 chimichiere; ECHO Tanker', confidenza='bassa'),
    'traghetto': dict(
        nome='Traghetto ro-ro e passeggeri', categoria='traghetto', classe='traghetto',
        lunghezza_m=[100, 200], pescaggio_m=[5, 7], velocita_servizio_kn=[17, 23], cavitazione_kn=[8, 12],
        larghezza_rel=0.16, echo_vc_kn=9.7, echo_d_lf=0.0, epoca_db=0.0,
        propulsione='diesel_medio', passo_variabile=True, assi=[2, 2], pale=[4, 4], giri_asse=[130, 250],
        motori=[2, 4], cilindri=[6, 16], giri_motore=[400, 750],
        generatori=[3, 4], cilindri_generatori=[6, 8], giri_generatori=[720, 1200], rete_60=0.3,
        fonte='26 §3 traghetti (Herald of Free Enterprise, Silja Serenade); ECHO Passenger', confidenza='bassa'),
    'rimorchiatore': dict(
        nome="Rimorchiatore d'altura", categoria='rimorchiatore', classe='rimorchiatore',
        lunghezza_m=[30, 70], pescaggio_m=[4, 6], velocita_servizio_kn=[11, 15], cavitazione_kn=[4, 7],
        larghezza_rel=0.27, echo_vc_kn=3.7, echo_d_lf=0.0, epoca_db=13.0,
        propulsione='diesel_medio', passo_variabile=False, assi=[2, 2], pale=[4, 4], giri_asse=[150, 300],
        motori=[2, 2], cilindri=[8, 16], giri_motore=[750, 900],
        generatori=[1, 2], cilindri_generatori=[6, 6], giri_generatori=[1200, 1800], rete_60=0.5,
        fonte='26 §3 rimorchiatori (EMD 645, riduttore Falk); ECHO Tug; Veirs 2016 170 ± 5 dB', confidenza='bassa'),
    'rompighiaccio': dict(
        nome='Rompighiaccio diesel-elettrico', categoria='rompighiaccio', classe='rompighiaccio',
        lunghezza_m=[120, 140], pescaggio_m=[8, 11], velocita_servizio_kn=[14, 19], cavitazione_kn=[8, 11],
        larghezza_rel=0.20, echo_vc_kn=8.0, echo_d_lf=0.0, epoca_db=3.0,
        propulsione='diesel_elettrico', passo_variabile=False, assi=[3, 3], pale=[4, 4], giri_asse=[120, 180],
        motori=[6, 9], cilindri=[9, 12], giri_motore=[500, 750],
        generatori=[1, 2], cilindri_generatori=[6, 6], giri_generatori=[1000, 1500], rete_60=0.0,
        fonte='26 §3 rompighiaccio (Krasin, Kapitan Khlebnikov); ECHO Government; DOSITS +10 dB nel ghiaccio', confidenza='bassa'),
    'nave_ricerca': dict(
        nome='Nave oceanografica', categoria='ricerca', classe='nave_ricerca',
        lunghezza_m=[50, 100], pescaggio_m=[4, 6], velocita_servizio_kn=[10, 14], cavitazione_kn=[8, 11],
        larghezza_rel=0.19, echo_vc_kn=8.0, echo_d_lf=0.0, epoca_db=0.0,
        propulsione='diesel_medio', passo_variabile=True, assi=[1, 2], pale=[4, 5], giri_asse=[150, 250],
        motori=[1, 2], cilindri=[6, 12], giri_motore=[600, 1000],
        generatori=[2, 4], cilindri_generatori=[6, 8], giri_generatori=[1200, 1800], rete_60=0.5,
        fonte="26 §3 navi oceanografiche (Johan Hjort, Revelle); ECHO Government/Research", confidenza='bassa'),
    'peschereccio': dict(
        nome='Peschereccio a rampa', categoria='peschereccio', classe='peschereccio',
        lunghezza_m=[40, 70], pescaggio_m=[4, 6], velocita_servizio_kn=[11, 14], cavitazione_kn=[3, 5],
        larghezza_rel=0.23, echo_vc_kn=6.4, echo_d_lf=0.0, epoca_db=0.0,
        propulsione='diesel_medio', passo_variabile=True, assi=[1, 1], pale=[4, 4], giri_asse=[150, 250],
        motori=[1, 1], cilindri=[6, 12], giri_motore=[600, 1000],
        generatori=[1, 2], cilindri_generatori=[6, 6], giri_generatori=[1200, 1800], rete_60=0.4, strascico=True,
        fonte='26 §3 peschereccio a rampa occidentale; ECHO Fishing; Veirs 2016 164 ± 9 dB', confidenza='bassa'),
    'bmrt': dict(
        nome='Grande peschereccio sovietico (BMRT)', categoria='peschereccio', classe='bmrt',
        lunghezza_m=[84, 105], pescaggio_m=[5, 6], velocita_servizio_kn=[12, 16], cavitazione_kn=[3, 5],
        larghezza_rel=0.16, echo_vc_kn=6.4, echo_d_lf=0.0, epoca_db=2.0,
        propulsione='diesel_medio', passo_variabile=True, assi=[1, 1], pale=[4, 4], giri_asse=[140, 200],
        motori=[2, 2], cilindri=[6, 8], giri_motore=[500, 750],
        generatori=[2, 3], cilindri_generatori=[6, 6], giri_generatori=[750, 1000], rete_60=0.0, strascico=True,
        fonte='26 §3 BMRT Pulkovskiy Meridian (2 x 520 giri, CPP a 146 giri); ECHO Fishing', confidenza='media'),
    'peschereccio_costiero': dict(
        nome='Peschereccio costiero (palangaro, reti da posta)', categoria='peschereccio', classe='peschereccio_costiero',
        lunghezza_m=[15, 45], pescaggio_m=[2, 4], velocita_servizio_kn=[9, 12], cavitazione_kn=[4, 7],
        larghezza_rel=0.25, echo_vc_kn=6.4, echo_d_lf=0.0, epoca_db=0.0,
        propulsione='diesel_veloce', passo_variabile=False, assi=[1, 1], pale=[3, 4], giri_asse=[300, 600],
        motori=[1, 1], cilindri=[6, 12], giri_motore=[1200, 1800],
        generatori=[1, 1], cilindri_generatori=[4, 6], giri_generatori=[1500, 1800], rete_60=0.5,
        fonte='26 §3 palangari e reti da posta; ECHO Fishing', confidenza='bassa'),
    'cianciolo': dict(
        nome='Cianciolo', categoria='peschereccio', classe='cianciolo',
        lunghezza_m=[30, 70], pescaggio_m=[4, 7], velocita_servizio_kn=[12, 15], cavitazione_kn=[4, 6],
        larghezza_rel=0.25, echo_vc_kn=6.4, echo_d_lf=0.0, epoca_db=0.0,
        propulsione='diesel_medio', passo_variabile=True, assi=[1, 1], pale=[4, 4], giri_asse=[200, 350],
        motori=[1, 1], cilindri=[6, 12], giri_motore=[750, 1000],
        generatori=[1, 2], cilindri_generatori=[6, 6], giri_generatori=[1200, 1800], rete_60=0.5,
        fonte='26 §3 ciancioli; ECHO Fishing', confidenza='bassa'),
}

# le classi nuove: la classe di classi.toml da cui prendono sprite, modello e scafo (finche' il kit non ne fa di propri)
VICINA = {
    'portarinfuse': 'mercantile', 'petroliera_costiera': 'petroliera', 'traghetto': 'mercantile',
    'rimorchiatore': 'peschereccio', 'rompighiaccio': 'mercantile', 'nave_ricerca': 'peschereccio',
    'bmrt': 'peschereccio', 'peschereccio_costiero': 'peschereccio', 'cianciolo': 'peschereccio',
}
CIVILI_DI_SEMPRE = ('mercantile', 'cargo', 'petroliera', 'peschereccio')


def proprie(radice):
    """Le classi nuove con la cartella propria del kit (extracted/navi/<classe>: la vicina riscalata alle misure vere,
    tools/navi/riscala_classe.py, o un modello proprio): sprite, modello e scafo loro (esporta_motore.py li esporta)."""
    return [c for c in VICINA if os.path.exists(os.path.join(radice, 'extracted', 'navi', c, 'sprite', 'ancore.json'))]


def _valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, str):
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, (list, tuple)):
        return '[' + ', '.join(_valore(x) for x in v) + ']'
    return repr(float(v))


def righe_civili():
    """Le righe TOML di [civili.*] per acustica.toml."""
    righe = []
    for t, campi in TIPI.items():
        assert campi['classe'] == t, t   # il tipo e' anche la classe: la classificazione la confronta con la libreria
        righe.append(f'[civili.{t}]')
        righe += [f'{k} = {_valore(v)}' for k, v in campi.items()]
        righe.append('')
    return righe


def aggiungi_classi_civili(radice):
    """Mette in classi.toml le classi civili nuove e `civile = true` su tutte le civili. Ripetibile: toglie prima il
    blocco scritto l'altra volta."""
    percorso = os.path.join(radice, 'data', 'motore', 'classi.toml')
    testo = open(percorso).read()
    inizio = '# --- classi civili del traffico (tools/pipeline/civili.py) ---'
    testo = testo.split('\n' + inizio)[0].rstrip('\n') + '\n'
    testo = re.sub(r'\ncivile = true\n', '\n', testo)
    for c in CIVILI_DI_SEMPRE:   # dopo il nome della classe
        testo = re.sub(rf'(\n\[{c}\]\nnome = [^\n]*\n)', r'\1civile = true\n', testo)
    vecchie = {}
    for blocco in re.split(r'\n(?=\[)', testo):
        m = re.match(r'\[(\w+)\]', blocco)
        if m:
            vecchie[m.group(1)] = dict(re.findall(r'^(\w+) = (.*)$', blocco, re.M))
    righe = ['', inizio, "# Modello, sprite e scafo propri (la classe piu' vicina riscalata alle misure vere, o il modello del",
             "# kit), oppure quelli della classe piu' vicina tali e quali.", '']
    loro = set(proprie(radice))
    for c, vicina in VICINA.items():
        t, v = TIPI[c], vecchie[vicina]
        if c in loro:
            v = dict(sprite=f'"navi/{c}.spr"', modello=f'"modelli/{c}.mdl"', scafo=f'"scafi/{c}.toml"')
        L = 0.5 * (t['lunghezza_m'][0] + t['lunghezza_m'][1])
        righe += [f'[{c}]', f'nome = "{t["nome"]}"', 'civile = true', f'sprite = {v["sprite"]}',
                  '# tipo_originale: assente nel gioco del 1989',
                  f'lunghezza_m = {round(L, 1)}', f'larghezza_m = {round(L * t["larghezza_rel"], 1)}',
                  f'velocita_max_nodi = {round(t["velocita_servizio_kn"][1] * 1.1, 1)}',
                  f'pescaggio_m = {round(0.5 * (t["pescaggio_m"][0] + t["pescaggio_m"][1]), 1)}',
                  *([f'modello = {v["modello"]}'] if 'modello' in v else []),
                  *([f'scafo = {v["scafo"]}'] if 'scafo' in v else []),
                  (f'fonte = "tools/pipeline/civili.py (docs/ricerca/26_navi_civili_traffico.md); extracted/navi/{c}/RAPPORTO.md"'
                   if c in loro else
                   f'fonte = "tools/pipeline/civili.py (docs/ricerca/26_navi_civili_traffico.md); modello di {vicina}"'),
                  'affidabilita = "bassa"', '']
    open(percorso, 'w').write(testo + '\n'.join(righe))
    return list(VICINA)


if __name__ == '__main__':
    radice = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    print('classi civili:', ' '.join(aggiungi_classi_civili(radice)))
