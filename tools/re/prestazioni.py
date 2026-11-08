"""Prestazioni delle unita' e delle armi di 688 Attack Sub, ricavate dagli script delle scene (.TOK).

Nell'originale non c'e' una tabella delle classi in GAME.EXE: ogni scena imposta il "modello corrente"
con gli opcode della VM (P_VMAX, P_TURN, P_ACCEL, P_DIVE, DEPTH_MAX, P_3A..P_3D, HEALTH, F54_*) e lo copia
nell'unita' con NEW_UNIT. Il tipo (UNIT_TYPE, +4F) e' solo il nome: la simulazione non lo legge (unici
usi in 1000:99BC, per riconoscere i siluri). I campi non impostati restano quelli del modello precedente:
per questo, per esempio, Typhoon, Charlie II, Oscar e Victor III usano lo stesso modello dell'Alfa
(subroutine comune nelle scene).
Le armi del giocatore sono definite nei gestori di lancio HANDLERS_C di ogni scena, uno per tipo d'arma
(0 Mk 48, 1 Type 53, 2 Sea Lance, 3 Tomahawk, 4 Harpoon, 5 esca acustica), eseguiti al momento del lancio.
Una scena che non usa un tipo lascia spesso nel suo gestore un siluro qualsiasi: conta la dotazione.

Conversioni, dal ciclo di moto 2011:083E (eseguito a ogni passo di simulazione: 18 passi per tick,
1 tick = 2 s di gioco, quindi 9 passi al secondo):
- velocita': 1/350 di nodo; P_VMAX = telegrafo 256 ("avanti tutta"), sforzo massimo = x 280/256;
- virata (+3E, rateo standard): unita' di rotta (9000 = 360 gradi) per passo -> x 0,36 gradi/s;
- accelerazione (+40): 1/350 di nodo per passo -> x 9/350 nodi/s;
- quota (+38): 1/8 di piede per passo, a rateo pieno -> x 9/8 piedi/s;
- quota massima (+5A): 1/8 di piede;
- +3A rumorosita' irradiata (moltiplica il termine di rumore di chi e' ascoltato), +3B sensibilita' del
  sonar (moltiplica il termine 40 - velocita' x 32 / massima di chi ascolta); se uno dei due e' 0 non c'e'
  scoperta (2011:0DC8, 0EEB-0F90);
- +3D ha due usi: per un'arma e' il danno all'impatto, tolto all'integrita' del bersaglio (dimezzato in 8
  passi su 18, 2011:0B45); per un'unita' e' la difesa contro gli attacchi a esito casuale (ATTACK_RND dei
  missili, 2011:1C96): il colpo riesce se il numero casuale (0-255) e' >= +3D, quindi P = (256 - 3D) / 256;
- durata della corsa delle armi: tick dell'AT che porta allo script d'arrivo (fine corsa), x 2 s;
- distanza: nm = nodi x tick x 18 x 350 / 65536 / 50 (circa 0,0019): il moto e' accelerato di circa 3,5 volte
  rispetto alla velocita' mostrata, scelta dichiarata dal progettista.

Uso:  prestazioni.py <radice>   ->  docs/originale/prestazioni.json e docs/originale/prestazioni.md
"""
import os, sys, glob, json, copy, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tok_schede as T
from tok_disasm import decode

# distanza percorsa: 18 passi per tick, incremento = velocita' / 65536 unita' per passo, 50 unita' per nm
NM_PER_KN_TICK = 18 * 350 / 65536 / 50

CAMPI = ('vmax', 'p_virata', 'p_accel', 'p_quota', 'quota_max', 'integrita', 'p3a', 'p3b', 'p3c', 'p3d', 'p54', 'flag')

def leggibile(m):
    v = lambda k: m.get(k)
    return dict(
        avanti_tutta_kn=round(v('vmax') / 350, 1) if v('vmax') else None,
        massima_kn=round(v('vmax') * 280 / 256 / 350, 1) if v('vmax') else None,
        virata_gradi_s=round(v('p_virata') * 0.36, 2) if v('p_virata') is not None else None,
        accel_kn_s=round(v('p_accel') * 9 / 350, 3) if v('p_accel') is not None else None,
        quota_ft_s=round(v('p_quota') * 9 / 8, 1) if v('p_quota') is not None else None,
        quota_max_ft=round(v('quota_max') / 8) if v('quota_max') else None,
        integrita=v('integrita'), rumore_3a=v('p3a'), sensibilita_3b=v('p3b'), p3c=v('p3c'), danno_3d=v('p3d'),
        schiera_trainata=bool((v('p54') or 0) & 1))

def gestore(d, pc0):
    """Esegue in modo lineare un gestore di lancio: modello impostato, NEW_UNIT, timer, messaggi."""
    m = {}; pila = []; pc = pc0; timer = []; msg = []; tipo = None; lancio = None; passi = 0; extra = []
    while passi < 300:
        passi += 1
        r = decode(d, pc)
        if r is None: break
        op, name, args, nxt = r; a = [v for c, v in args]; pc = nxt
        if op == 0x00:
            if pila: pc = pila.pop(); continue
            break
        if op == 0x01: pila.append(nxt); pc = a[0]; continue
        if op == 0x43: pc = a[0]; continue
        if op in T.SEMPLICI: m[T.SEMPLICI[op]] = a[0]
        elif op == 0x2f: m['p54'] = (m.get('p54', 0) & ~2) | a[0]
        elif op == 0x30: m['p54'] = (m.get('p54', 0) & ~1) | a[0]
        elif op == 0x2d: lancio = dict(arrivo=a[0], reazione=a[1])
        elif op == 0x14: timer.append((a[0], a[1]))
        elif op in (0x23, 0x22, 0x5d): msg.append(a[0])
        elif op == 0x6a: extra.append(f'ATTACK_RND {a[0]}')
        elif op == 0x10: extra.append('NEW_UNIT')
        if op == 0x28: tipo = a[0]
        if op in T.TERMINA: break
    fine = [t for t, l in timer if lancio and l == lancio['arrivo']]
    eventi = []
    for t, l in timer:
        if lancio and l == lancio['arrivo']: continue
        ii = []; q = l
        for _ in range(8):
            r = decode(d, q)
            if r is None or r[0] == 0x00: break
            ii.append(f"{r[1]} {' '.join(str(v) for c, v in r[2] if c != 'L')}".strip()); q = r[3]
        eventi.append(dict(tick=t, istruzioni=ii))
    return dict(tipo=tipo, modello=m, timer=timer, fine_corsa_tick=max(fine) if fine else None, messaggi=msg,
                altro=extra, eventi=eventi)

# valori del capitolo 7 del manuale (velocita' massima in nodi, quota massima in piedi, rumore)
MANUALE = {0x1d: (45, 1800, 'superiore alla media'), 0x15: (42, 1800, 'superiore alla media'), 0x1e: (24, 1800, 'nella media'),
           0x21: (16, 800, 'basso'), 0x20: (16, 1200, 'molto basso'), 0x14: (37, 1500, 'basso'), 0x1c: (35, 1800, 'nella media'),
           0x22: (32, 1500, 'basso'), 0x1f: (24, 1800, 'superiore alla media'), 0x17: (32, 1800, 'inferiore alla media'),
           0x02: (20, None, 'molto alto'), 0x00: (20, None, 'alto'), 0x0b: (35, None, 'alto'), 0x05: (34, None, 'superiore alla media'),
           0x0c: (33, None, 'superiore alla media'), 0x06: (33, None, 'alto'), 0x08: (29, None, 'nella media'), 0x01: (20, None, 'alto'),
           0x0a: (33, None, 'alto'), 0x0d: (29, None, 'superiore alla media'), 0x0e: (26, None, 'alto'), 0x09: (12, None, 'superiore alla media'),
           0x10: (16, None, 'molto alto'), 0x12: (126, None, None), 0x18: (415, None, None)}
ARMI_MANUALE = {0x16: (52.5, 10), 0x25: (47.5, 10), 0x26: (None, 10)}
# voce = tipo d'arma del tubo (DS:02D8 + tubo, dai byte 7-13 della configurazione della scena in SCENE*.LZA);
# il LANCIO (1000:7A9F) manda il comando 24h + tipo al dispatcher 2011:1F3C. Dotazioni nei byte 1-6.
NOMI_ARMA = {0: 'Mk 48 ADCAP', 1: 'Type 53', 2: 'Sea Lance', 3: 'Tomahawk', 4: 'Harpoon', 5: 'esca acustica'}

def tabella_md(unita, armi):
    md = ['# Prestazioni delle unità e delle armi nell\'originale', '',
          'Generato da `tools/re/prestazioni.py` dagli script delle 22 scene (`.TOK`). Per ogni classe il modello usato nel',
          'maggior numero di scene; *modelli* = quanti modelli diversi compaiono. Velocità alla massima forza (telegrafo 280/256), virata standard,',
          'accelerazione, rateo di quota a pieno comando. Conversioni nell\'intestazione dello strumento.', '',
          '## Unità', '',
          'La quota massima delle navi di superficie è un residuo del modello precedente e non conta. *P colpita* è la',
          'probabilità di subire un attacco a esito casuale (missili): (256 − difesa 3D) / 256.', '',
          '| Tipo | Classe | max kn | manuale kn | quota max ft | manuale ft | virata °/s | acc. kn/s | quota ft/s | integrità | rumore 3A | sonar 3B | difesa 3D | P colpita | rumore nel manuale | modelli |',
          '|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|']
    for u in unita:
        if u['tipo'] is None: continue
        m = u['modelli'][0]; man = MANUALE.get(u['tipo'], (None, None, None))
        pc = f"{(256 - m['danno_3d']) / 256:.0%}" if m['danno_3d'] is not None else '—'
        md.append(f"| {u['tipo']:#04x} | {u['nome'].title()} | {m['massima_kn']} | {man[0] or '—'} | {m['quota_max_ft'] or '—'} | {man[1] or '—'} | "
                  f"{m['virata_gradi_s']} | {m['accel_kn_s']} | {m['quota_ft_s']} | {m['integrita']} | {m['rumore_3a']} | {m['sensibilita_3b']} | "
                  f"{m['danno_3d'] if m['danno_3d'] is not None else '—'} | {pc} | {man[2] or '—'} | {len(u['modelli'])} |")
    md += ['', '## Armi del giocatore (gestori di lancio HANDLERS_C)', '',
           '| Voce | Arma | Scene | corsa kn | durata s | portata nm | danno 3D | rumore 3A | testa 3B | virata °/s | quota ft/s | eventi durante la corsa |',
           '|---|---|---|---|---|---|---|---|---|---|---|---|']
    for a in armi:
        ev = '; '.join(f"+{e['tick'] * 2} s: {', '.join(e['istruzioni'][:3])}" for e in a['eventi'])
        md.append(f"| {a['indice']} ({NOMI_ARMA[a['indice']]}) | {a['nome'].title()} | {len(a['scene'])} | {a['velocita_corsa_kn'] or '—'} | "
                  f"{a['fine_corsa_s'] or '—'} | {a['portata_nm'] or '—'} | {a['danno_3d'] if a['danno_3d'] is not None else '—'} | "
                  f"{a['rumore_3a'] if a['rumore_3a'] is not None else '—'} | {a['sensibilita_3b'] or '—'} | {a['virata_gradi_s'] or '—'} | "
                  f"{a['quota_ft_s'] or '—'} | {ev or '—'} |")
    return md

if __name__ == '__main__':
    root = sys.argv[1]
    agg = collections.defaultdict(lambda: {'modello': {}})
    scene_di = collections.defaultdict(set)
    armi = {}
    for f in sorted(glob.glob(f'{root}/extracted/raw/DISK1/SCENE*.TOK')):
        sc = os.path.basename(f)[:7]; d = open(f, 'rb').read()
        for p in T.esegui_init(d):
            for u in p['unita']:
                t = u.get('tipo'); scene_di[t].add(sc)
                agg[t]['modello'].setdefault(json.dumps({k: u.get(k) for k in CAMPI}, sort_keys=True), set()).add(sc)
        r = decode(d, 0x30)
        if r and r[1] == 'HANDLERS_C':
            for i, (c, lab) in enumerate(r[2]):
                g = gestore(d, lab)
                k = (i, json.dumps(g['modello'], sort_keys=True), g['tipo'], g['fine_corsa_tick'], json.dumps(g['eventi']))
                armi.setdefault(k, []).append(sc)
    unita = []
    for t in sorted(agg, key=lambda x: (x is None, x)):
        modelli = sorted(agg[t]['modello'].items(), key=lambda kv: (-len(kv[1]), kv[0]))
        unita.append(dict(tipo=t, nome=T.TIPI.get(t, '?'), scene=sorted(scene_di[t]),
                          modelli=[dict(scene=sorted(sc), grezzo=json.loads(k), **leggibile(json.loads(k))) for k, sc in modelli]))
    elenco_armi = []
    for (i, mj, tipo, fine, ev), scene in sorted(armi.items(), key=lambda x: (x[0][0], -len(x[1]))):
        m = json.loads(mj)
        corsa = round(m['vmax'] * m.get('telegrafo', 256) / 256 / 350, 1) if m.get('vmax') else None
        elenco_armi.append(dict(indice=i, tipo=tipo, nome=T.TIPI.get(tipo, '—') if tipo is not None else '—',
                                scene=scene, fine_corsa_tick=fine, fine_corsa_s=fine * 2 if fine else None,
                                velocita_corsa_kn=corsa,
                                portata_nm=round(corsa * fine * NM_PER_KN_TICK, 1) if corsa and fine else None,
                                eventi=json.loads(ev), grezzo=m, **leggibile(m)))
    # le armi uguali in piu' scene diventano una riga sola
    righe = {}
    for a in elenco_armi:
        k = (a['indice'], a['nome'], a['velocita_corsa_kn'], a['fine_corsa_s'], a['danno_3d'], a['sensibilita_3b'], json.dumps(a['eventi']))
        if k in righe: righe[k]['scene'] = sorted(set(righe[k]['scene']) | set(a['scene']))
        else: righe[k] = a
    elenco_armi = sorted(righe.values(), key=lambda a: (a['indice'], -len(a['scene'])))
    json.dump(dict(unita=unita, armi=elenco_armi), open(f'{root}/docs/originale/prestazioni.json', 'w'), indent=1, ensure_ascii=False)
    open(f'{root}/docs/originale/prestazioni.md', 'w').write('\n'.join(tabella_md(unita, elenco_armi)) + '\n')
    for u in unita:
        m = u['modelli'][0]
        print(f"{u['tipo']!s:>4} {u['nome'][:34]:34s} {m['massima_kn']!s:>5} kn  vir {m['virata_gradi_s']!s:>5}  acc {m['accel_kn_s']!s:>5}  "
              f"q {m['quota_ft_s']!s:>5}  qmax {m['quota_max_ft']!s:>5}  int {m['integrita']!s:>4}  3a {m['rumore_3a']!s:>3}  3b {m['sensibilita_3b']!s:>3}  "
              f"3d {m['danno_3d']!s:>4}  modelli {len(u['modelli'])}")
    for a in elenco_armi:
        print(f"arma {a['indice']} {a['nome'][:22]:22s} {a['velocita_corsa_kn']!s:>5} kn {a['portata_nm']!s:>5} nm vir {a['virata_gradi_s']!s:>5} q {a['quota_ft_s']!s:>5} "
              f"3a {a['rumore_3a']!s:>3} 3b {a['sensibilita_3b']!s:>3} 3d {a['danno_3d']!s:>4} fine {a['fine_corsa_s']} s  scene {len(a['scene'])}  {[(e['tick'], e['istruzioni'][:3]) for e in a['eventi']]}")
