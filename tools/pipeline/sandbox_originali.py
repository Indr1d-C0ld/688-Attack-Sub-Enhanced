"""Il piano del comandante IA per le 22 missioni originali nella sandbox (docs/19_sandbox.md, V3) ->
data/motore/sandbox_originali.toml. Lo legge missioni/sandbox_originali.hpp (piano_originale).

Gli obiettivi delle originali (data/missioni/obiettivi) dicono come si vince, non dove andare: il piano lo dice, dal
briefing e dagli script della missione convertita (data/missioni/SCENE*.lua). Ogni voce porta la `fonte`: la frase del
briefing che giustifica la condotta e, per le coordinate, il gestore dello script o l'unita' amica di cui si prende la
rotta. Il piano usa solo cio' che il comandante sa dagli ordini: il briefing, i limiti dell'area (i rettangoli degli
script), le rotte pianificate delle unita' amiche. Mai le posizioni dei nemici, che fra le varianti cambiano.

Campi (metri di missione, x verso est, y verso nord; velocita' in nodi):
- condotta: transito | caccia | scorta | pedinamento | ricognizione | minamento | evasione | attacco_terra;
- fonte: la citazione del briefing (inglese, come nell'originale) e le fonti delle coordinate;
- punti: la rotta del transito; area: { x, y, r } della caccia, del pedinamento o della ricognizione;
- limite: { x0, y0, x1, y1 } il rettangolo da cui non uscire (fuori, l'originale chiude la missione);
- uscita: { x, y, r } dove andare a missione compiuta;
- classi: i bersagli (caccia, attacco_terra) o la classe da pedinare;
- armi: libere | roe | solo_difesa (libere dopo la prima arma nemica in acqua) | classi (solo le classi del piano,
  le altre come solo_difesa);
- quota_m, velocita_kn: la dottrina della condotta (0: quella del comandante);
- seminare: liberarsi di chi pedina; pim: { punti, velocita_kn } la rotta pianificata del convoglio scortato, dalla
  partenza della prima nave; avanti_m: la stazione davanti al convoglio;
- distanza_m: il pedinamento resta entro questa distanza; ping_m: un impulso del sonar sotto questa distanza;
- attesa_s: prima di questo tempo il comandante resta sul posto;
- tubi: le armi del catalogo del carico (data/motore/carico.toml) nei tubi 1..n alla partenza (nei pozzi sul Flight II).
Numeri STIMA DI GIOCO dove non vengono dal briefing o dagli script (quote, velocita' di dottrina, distanze di stazione).
Uso: sandbox_originali.py <radice del progetto>"""
import os
import re
import sys

NM = 1852.0
NUM = r'(-?[\d.]+)'


class Scena:
    """Le parti di uno script convertito che servono al piano: i rettangoli per gestore, le unita' della prima variante."""

    def __init__(self, percorso):
        t = open(percorso, encoding='utf-8').read()
        self.id = os.path.basename(percorso)[:-4]
        self.rettangoli = {}
        for h, corpo in re.findall(r'script\.(L_\w+) = function\(u\)(.*?)\nend', t, re.S):
            for a in re.findall(r'm688\.in_rettangolo\(u, ' + ', '.join([NUM] * 4) + r'\)', corpo):
                self.rettangoli.setdefault(h, []).append(tuple(float(v) for v in a))
        self.unita = {}
        variante = t.split('{ probabilita = ')[1]
        for u in re.findall(r'\{ id = (\d+), classe = "([^"]+)".*?\n\s*posizione = \{ x = ' + NUM + ', y = ' + NUM +
                            r' \}.*?velocita_kn = ' + NUM + r'(.*?)gestori', variante, re.S):
            rotta = u[5].split('rotta_punti')[1] if 'rotta_punti' in u[5] else ''
            self.unita[int(u[0])] = {'classe': u[1], 'pos': (float(u[2]), float(u[3])), 'velocita_kn': float(u[4]),
                                     'rotta': [(float(a), float(b)) for a, b in re.findall(r'\{ x = ' + NUM + ', y = ' + NUM, rotta)]}

    def rettangolo(self, gestore, k=0):
        x0, y0, x1, y1 = self.rettangoli[gestore][k]
        return {'x0': x0, 'y0': y0, 'x1': x1, 'y1': y1}

    def pim(self, unita):
        """La rotta pianificata di un'unita' amica: la partenza e i suoi punti, alla sua velocita' di partenza."""
        u = self.unita[unita]
        return {'punti': [u['pos']] + u['rotta'], 'velocita_kn': u['velocita_kn']}


def centro(r, raggio):
    return {'x': (r['x0'] + r['x1']) / 2, 'y': (r['y0'] + r['y1']) / 2, 'r': raggio}


def piani(s):
    """Il piano di ogni missione. s: le scene per id."""
    p = {}
    torpex = 'TORPEX: «YOUR MISSION IS TO DESTROY EACH OF THE WARSHIPS... THE WEAPONS RANGE IS CLEAR AND WEAPONS ARE FREE»'

    # 0A: i Tomahawk sul bersaglio dei missili (unita' 1, «CRUISE MISSILE TARGET», a 19 km a sud), poi Copenaghen
    a = s['SCENE0A']
    p['SCENE0A'] = dict(
        condotta='attacco_terra', armi='solo_difesa', classi=['bqm74'], quota_m=60, velocita_kn=10,
        tubi=['tasm'] * 4,   # decisione della principale (28/10): i Tomahawk nei tubi da siluro, la dottrina dei 688 senza VLS
        uscita=centro(a.rettangolo('L_0311', 0), 15000), limite=a.rettangolo('L_0311', 1),
        fonte='briefing: «DELIVER YOUR PAYLOAD OF TOMAHAWK MISSILES... ESCAPE INTO COPENHAGEN WATERS AS QUICKLY AS POSSIBLE... THE NAVY... WILL NOT BE ABLE TO SEND ANY ASSISTANCE IF YOUR SUBMARINE IS DISCOVERED» (armi solo in difesa: la fuga prima di tutto); '
              'tubi: quattro Tomahawk nei tubi da siluro (il manuale: «You have only 5 minutes to fire ALL four missiles»; L_02fb a 300 s); uscita: il rettangolo di Copenaghen di L_0311; limite: il secondo rettangolo di L_0311 (fuori: esito 4)')
    # 1A: tre Forrest Sherman in 15 minuti, armi libere, «stay low and quiet»
    a = s['SCENE1A']
    navi = [a.unita[k]['pos'] for k in (2, 3, 4)]
    p['SCENE1A'] = dict(
        condotta='caccia', armi='libere', classi=['spruance'], quota_m=60, velocita_kn=6,
        area={'x': sum(x for x, _ in navi) / 3, 'y': sum(y for _, y in navi) / 3, 'r': 12000},
        fonte=torpex + ', «STAY LOW AND QUIET»; area: il poligono di tiro, dove partono i bersagli dell\'esercitazione '
              '(le navi 2-4 sono uguali in tutte le varianti)')
    # 1B, 1C: la gara col Los Angeles (o col Dallas) dentro il poligono di L_0531 / L_037b
    for k, g in (('SCENE1B', 'L_0531'), ('SCENE1C', 'L_037b')):
        a = s[k]
        p[k] = dict(
            condotta='caccia', armi='libere', classi=['spruance'], quota_m=60, velocita_kn=8,
            area={'x': 30000, 'y': 40000, 'r': 15000}, limite=a.rettangolo(g),
            fonte='briefing: «THERE ARE 5 FOREST SHERMAN CLASS DESTROYERS OUT THERE, AND WHOEVER SCORES THE MOST KILLS '
                  'WINS»; area: le rotte dei cacciatorpediniere nel poligono; limite: il poligono di ' + g)
    # 2A, 2C: seminare l'Alfa senza sparare
    for k in ('SCENE2A', 'SCENE2C'):
        p[k] = dict(
            condotta='evasione', armi='solo_difesa', seminare=True, quota_m=150, velocita_kn=5,
            fonte='briefing: «EITHER YOU SHAKE THE SOVIET ALFA ON YOUR TAIL... I CAN\'T ALLOW YOU TO LAUNCH WEAPONS UNLESS '
                  'THE RUSSIANS DECIDE THEY WANT TO PLAY HARDBALL»')
    # 2B: l'Alfa pedina il Los Angeles entro 5 miglia
    p['SCENE2B'] = dict(
        condotta='pedinamento', armi='solo_difesa', classi=['los_angeles'], distanza_m=5 * NM, quota_m=100,
        fonte='briefing: «CONTINUE TRACKING THE AMERICANS... STAY WITHIN 5 MILES SO THAT YOU MAY FIRE ON COMMAND... '
              'YOU ARE NOT AUTHORIZED TO LAUNCH TORPEDOES UNLESS FIRED UPON»')
    # 3B: l'Alfa cerca lungo la costa, si avvicina all'intruso e lo illumina col sonar, poi lo segue fuori
    a = s['SCENE3B']
    costa = a.unita[3]['rotta']   # la rotta delle navi sovietiche lungo la costa di Polyarnyy
    p['SCENE3B'] = dict(
        condotta='pedinamento', armi='solo_difesa', classi=['trafalgar'], ping_m=1.8 * NM, distanza_m=5 * NM, quota_m=100,
        area={'x': (costa[0][0] + costa[1][0]) / 2, 'y': (costa[0][1] + costa[1][1]) / 2, 'r': 30000},
        limite=a.rettangolo('L_028b'),
        fonte='briefing: «EXECUTE SEARCH PROCEDURES ALONG THE POLYARNYY COASTLINE. SHOULD YOU DISCOVER INTRUDERS, MOVE '
              'CLOSE TO THE INTRUDER AND ACTIVATE SONAR. FOLLOW THE INTRUDER OUT OF THE AREA»; ping_m: L_0325 '
              '(bersaglio entro 1,8 miglia); area: la rotta costiera delle navi sovietiche; limite: L_028b')
    # 4A: le quattro petroliere all'ingresso dello stretto, non le Koni
    a = s['SCENE4A']
    rotta = a.unita[4]['rotta']
    p['SCENE4A'] = dict(
        condotta='caccia', armi='classi', classi=['petroliera'], quota_m=60, velocita_kn=8,
        area={'x': rotta[0][0], 'y': rotta[0][1], 'r': 15000}, limite=a.rettangolo('L_02f7'),
        fonte='briefing: «INTERCEPT AND SINK FOUR LIBYAN OIL TANKERS... ENTERING THE STRAIT OF GIBRALTAR... DO NOT ATTACK '
              'THE FRIGATES UNLESS IT IS NECESSARY»; area: l\'ingresso dello stretto (il primo punto della rotta delle '
              'petroliere, come l\'intelligence la annuncia); limite: L_02f7')
    # 5A, 5C: fuori dallo stretto verso ovest, senza nessuno in coda e senza sparare
    for k, g in (('SCENE5A', 'L_0260'), ('SCENE5C', 'L_0175')):
        a = s[k]
        r = a.rettangolo(g)
        canale = list(reversed(a.unita[9 if k == 'SCENE5A' else 5]['rotta']))   # la rotta dei mercantili, al contrario
        p[k] = dict(
            condotta='transito', armi='solo_difesa', seminare=True, quota_m=150, velocita_kn=6,
            punti=canale[1:] + [(r['x0'] - 6000, canale[-1][1])], uscita={'x': r['x0'] - 6000, 'y': canale[-1][1], 'r': 3000},
            fonte='briefing: «GET PAST THE SOVIETS AND ESCAPE TO THE WEST WITHOUT SO MUCH AS A WHISPER... I DON\'T SEE ANY '
                  'NEED FOR WEAPON TESTING UNLESS THEY DRAW FIRST»; punti: il canale dei mercantili (unita\' 9 o 5) al '
                  'contrario; uscita: a ovest del rettangolo dello stretto di ' + g)
    # 5B: l'Alfa intercetta il 688 all'uscita e lo tiene entro la portata delle armi
    a = s['SCENE5B']
    stretto = a.rettangolo('L_019c', 0)
    p['SCENE5B'] = dict(
        condotta='pedinamento', armi='solo_difesa', classi=['los_angeles'], distanza_m=5 * NM, quota_m=100,
        area=centro(stretto, 12000), limite=a.rettangolo('L_0306'),
        fonte='briefing: «INTERCEPT THE AMERICANS AS THEY EXIT THE MEDITERRANEAN... MAINTAIN SONAR CONTACT WITH THE 688 AS '
              'LONG AS POSSIBLE... YOU MAY LAUNCH WEAPONS ONLY IN THE EVENT THAT THE IMPERIALISTS... TAKE ACTION FIRST»; '
              'distanza_m: le 5 miglia di L_0171; area: il rettangolo dello stretto di L_019c; limite: L_0306')
    # 6A: la scorta dei mercantili a Tivat; Harpoon sulle navi
    a = s['SCENE6A']
    p['SCENE6A'] = dict(
        condotta='scorta', armi='libere', pim=a.pim(3), avanti_m=6000, quota_m=60,
        limite=a.rettangolo('L_01c6'),
        fonte='briefing: «ESCORT THE CARGO SHIPS NORTH TO TRIVAT AND PROTECT THEM FROM SOVIET AGGRESSION BY WHATEVER MEANS '
              'AVAILABLE... WE RECOMMEND THE USE OF MULTIPLE HARPOON MISSILES»; pim: la rotta del mercantile 3; limite: L_01c6')
    # 7A, 7C: il Typhoon al passo; non impegnarsi con le scorte
    for k, g in (('SCENE7A', None), ('SCENE7C', 'L_01e8')):
        a = s[k]
        rotta = a.unita[2]['rotta']
        mezzo = rotta[len(rotta) // 2]
        piano = dict(
            condotta='caccia', armi='classi', classi=['typhoon'], quota_m=150, velocita_kn=6,
            area={'x': mezzo[0], 'y': mezzo[1], 'r': 8000},
            fonte='briefing: «A SOVIET TYPHOON CLASS SUBMARINE... IS HEADING FOR YOUR PATROL SECTOR... DON\'T TANGLE WITH '
                  'THEM [the attack boats] IF YOU CAN AVOID IT. IF YOU CAN CATCH IVAN AT THE PASS»; area: il passo, a meta\' '
                  'della rotta del Typhoon (la ricognizione dal satellite)')
        if g:
            piano['limite'] = a.rettangolo(g)
            piano['fonte'] += '; limite: ' + g
        p[k] = piano
    # 7B: l'Alfa protegge il Typhoon: pattuglia davanti e distrugge le navi che minacciano
    a = s['SCENE7B']
    pim = a.pim(2)
    pim['velocita_kn'] = 6.0
    p['SCENE7B'] = dict(
        condotta='scorta', armi='libere', pim=pim, avanti_m=8000, quota_m=100,
        fonte='briefing: «YOU MUST PROTECT THE PLARB KRASNOGVARDEETS AT ALL COSTS! PATROL THE AREA AHEAD AND DESTROY ANY '
              'THREATENING WARSHIPS»; pim: la rotta del Typhoon (unita\' 2); velocita\' STIMA DI GIOCO (lo script non la dice)')
    # 8A, 8C: esploratore del gruppo di superficie verso nordest
    for k, g in (('SCENE8A', 'L_01f9'), ('SCENE8C', 'L_01be')):
        a = s[k]
        p[k] = dict(
            condotta='scorta', armi='roe', pim=a.pim(2), avanti_m=10000, quota_m=100,
            limite=a.rettangolo(g),
            fonte='briefing: «ACT AS SCOUT FOR THE SURFACE GROUP HEADING NORTHEAST... IT IS YOUR RESPONSIBILITY TO PROTECT '
                  'THE SKIMMERS»; pim: la rotta della Nimitz (unita\' 2); limite: ' + g)
    # 8B: l'Alfa pattuglia le rotte e distrugge le navi NATO
    a = s['SCENE8B']
    gruppo = a.unita[2]
    p['SCENE8B'] = dict(
        condotta='caccia', armi='libere', classi=['nimitz', 'iowa', 'sacramento', 'perry', 'kidd'], quota_m=100, velocita_kn=6,
        area={'x': (gruppo['pos'][0] + gruppo['rotta'][-1][0]) / 2, 'y': gruppo['pos'][1], 'r': 25000},
        limite=a.rettangolo('L_01b8'),
        fonte='briefing: «PREVENT AMERICAN SUPPORT FROM REACHING NATO FORCES IN THE NORWEGIAN SEA. PATROL THE SEA LANES AND '
              'DESTROY ALL NATO VESSELS»; area: la rotta del gruppo NATO verso la Norvegia; limite: L_01b8')
    # 9A, 9C: la scorta del convoglio verso il golfo di Biscaglia, armi libere con l'identificazione
    for k, g in (('SCENE9A', 'L_054e'), ('SCENE9C', 'L_04d2')):
        a = s[k]
        p[k] = dict(
            condotta='scorta', armi='libere', pim=a.pim(2), avanti_m=5000, quota_m=100,
            limite=a.rettangolo(g),
            fonte='briefing: «ESCORT A CONVOY OF MILITARY SUPPLIES EAST TO THE BAY OF BISCAY... WEAPONS ARE FREE BUT ONLY '
                  'ONCE YOU HAVE POSITIVE IDENTIFICATION»; pim: la rotta del mercantile 2; limite: ' + g)
    # 9B: l'Alfa aspetta i missili dell'Oscar, poi finisce il convoglio
    a = s['SCENE9B']
    oscar = a.unita[10]['rotta'][-1]
    p['SCENE9B'] = dict(
        condotta='caccia', armi='libere', classi=['cargo', 'perry'], quota_m=100, velocita_kn=8, attesa_s=900,
        area={'x': oscar[0], 'y': oscar[1], 'r': 15000}, limite=a.rettangolo('L_061d'),
        fonte='briefing: «THE LENINSKII KOMSOMOL WILL WAIT AS THE PSKOVSKII KOMSOMOLETS CRUSHES THE CONVOY WITH A BARRAGE OF '
              'SS-NX-21 MISSILES. ONCE THE MISSILES HAVE IMPACTED, THE LENINSKII KOMSOMOL WILL MOVE IN»; area: dove va '
              'l\'Oscar (unita\' 10); attesa_s STIMA DI GIOCO; limite: L_061d')
    return p


def toml_valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, (int, float)):
        return repr(round(float(v), 1))
    if isinstance(v, str):
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, (list, tuple)):
        return '[' + ', '.join(toml_valore(x) for x in v) + ']'
    if isinstance(v, dict):
        return '{ ' + ', '.join(f'{k} = {toml_valore(x)}' for k, x in v.items()) + ' }'
    raise TypeError(v)


ORDINE = ['condotta', 'fonte', 'armi', 'classi', 'quota_m', 'velocita_kn', 'seminare', 'punti', 'area', 'uscita',
          'limite', 'pim', 'avanti_m', 'distanza_m', 'ping_m', 'attesa_s', 'tubi']


def main(radice):
    cartella = os.path.join(radice, 'data', 'missioni')
    scene = {f[:-4]: Scena(os.path.join(cartella, f)) for f in sorted(os.listdir(cartella)) if re.match(r'SCENE\w+\.lua$', f)}
    p = piani(scene)
    mancanti = sorted(set(scene) - set(p))
    if mancanti:
        sys.exit('senza piano: ' + ', '.join(mancanti))
    righe = ['# Il piano del comandante IA per le missioni originali nella sandbox (generato da',
             '# tools/pipeline/sandbox_originali.py: non modificare a mano). Campi e fonti: vedi lo script.', '']
    for k in sorted(p):
        righe.append(f'[{k}]')
        for campo in ORDINE:
            if campo in p[k]:
                righe.append(f'{campo} = {toml_valore(p[k][campo])}')
        righe.append('')
    uscita = os.path.join(radice, 'data', 'motore', 'sandbox_originali.toml')
    open(uscita, 'w', encoding='utf-8').write('\n'.join(righe))
    print(f'{uscita}: {len(p)} piani')


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
