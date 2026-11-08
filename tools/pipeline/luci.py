"""Le luci di navigazione e di fonda per classe (docs/27_luci_navigazione.md, approvato dall'utente il 29/10/2026; la
forma dei dati concordata con la principale): scrive data/motore/luci.toml.

Per ogni classe di superficie e ogni sottomarino del libretto:
- `tipo` (mercantile, peschereccio, militare, sottomarino) e, per i pescherecci, `pesca` (strascico, altre);
- i fanali, ognuno col ruolo, il colore, la posizione sul modello 3D (x a prua, y a sinistra, z sul galleggiamento,
  metri, le coordinate del modello che il gioco disegna), il settore in gradi dalla prua (positivi a dritta, da
  `settore[0]` a `settore[1]` in senso orario; [-180, 180] tutt'intorno), la portata in miglia, gli stati in cui si
  accende e i lampi.

Le regole sono quelle del COLREG 1972: fanali e settori (regole 21-23, 26, 27, 30 e Annesso I), portate (regola 22).
Le posizioni vengono dalle parti dei modelli (data/motore/modelli/*.mdl, formato MDL2 di
tools/navi/esporta_modello.py): l'albero prodiero e il poppiero, la plancia per i laterali, la poppa dello scafo per
il coronamento, la vela dei sottomarini. Le classi civili generiche hanno il modello di base, come nel gioco; la
lunghezza della classe decide la portata e il secondo fanale d'albero (da 50 m in su).
Gli stati: moto, fonda, pesca, manovra_limitata, non_governa (spente: nessun fanale, non c'e' nel file).
Uso: luci.py <radice del progetto>. Dopo le classi e i modelli (esporta_motore)."""
import os
import struct
import sys
import tomllib
import zlib

import numpy as np

# il tipo di ogni classe; le altre (aerei, missili, elicotteri) non hanno fanali
TIPO = {
    'kirov': 'militare', 'slava': 'militare', 'kara': 'militare', 'krivak2': 'militare', 'koni': 'militare',
    'nanuchka': 'militare', 'agi': 'militare', 'nimitz': 'militare', 'iowa': 'militare', 'sacramento': 'militare',
    'kidd': 'militare', 'spruance': 'militare', 'perry': 'militare',
    'typhoon': 'sottomarino', 'oscar': 'sottomarino', 'akula': 'sottomarino', 'victor3': 'sottomarino',
    'alfa': 'sottomarino', 'charlie2': 'sottomarino', 'kilo': 'sottomarino', 'foxtrot': 'sottomarino',
    'los_angeles': 'sottomarino', 'sturgeon': 'sottomarino', 'trafalgar': 'sottomarino',
    # il primo lotto delle firme nuove (docs/ricerca/31): sul modello della classe parente finche' non hanno il loro
    'victor1': 'sottomarino', 'delta3': 'sottomarino', 'sierra': 'sottomarino', 'udaloy': 'militare', 'grisha3': 'militare',
    'ohio': 'sottomarino', 'type206': 'sottomarino', 'ticonderoga': 'militare', 'knox': 'militare', 'type22': 'militare',
    'type22b2': 'militare', 'ugra': 'militare', 'hauk': 'militare', 'oslo': 'militare',
    'stockholm': 'militare', 'nacken': 'sottomarino', 'kitty_hawk': 'militare',
    'mercantile': 'mercantile', 'cargo': 'mercantile', 'petroliera': 'mercantile', 'portarinfuse': 'mercantile',
    'petroliera_costiera': 'mercantile', 'traghetto': 'mercantile', 'rompighiaccio': 'mercantile',
    'nave_ricerca': 'mercantile', 'rimorchiatore': 'mercantile',
    'peschereccio': 'peschereccio', 'bmrt': 'peschereccio', 'peschereccio_costiero': 'peschereccio',
    'cianciolo': 'peschereccio',
}
# la pesca (regola 26): a strascico verde su bianco, le altre rosso su bianco
PESCA = {'peschereccio': 'strascico', 'bmrt': 'strascico', 'peschereccio_costiero': 'altre', 'cianciolo': 'altre'}   # palangaro e reti da posta: altre
# il faro ambra a lampi dei sottomarini americani in superficie (32 CFR 707.7, dal 1983; docs/27 §6)
FARO_AMBRA = {'los_angeles', 'sturgeon', 'ohio'}

# le portate in miglia (regola 22) per lunghezza
def portate(lunghezza):
    if lunghezza >= 50.0:
        return {'albero': 6.0, 'laterale': 3.0, 'coronamento': 3.0, 'tutt_intorno': 3.0}
    if lunghezza >= 12.0:
        return {'albero': 5.0 if lunghezza >= 20.0 else 3.0, 'laterale': 2.0, 'coronamento': 2.0, 'tutt_intorno': 2.0}
    return {'albero': 2.0, 'laterale': 1.0, 'coronamento': 2.0, 'tutt_intorno': 2.0}

PORTATA_COPERTA_NM = 4.0   # le luci da lavoro della coperta: nessuna regola, STIMA (una flotta da pesca si vede da lontano)
PORTATA_FARO_NM = 3.0      # il faro ambra: la norma non dice la portata, STIMA come un tutt'intorno

# i settori (Annesso I e regola 21)
ALBERO = [-112.5, 112.5]
SINISTRO = [-112.5, 0.0]
DRITTO = [0.0, 112.5]
CORONAMENTO = [112.5, -112.5]   # da 112,5 a dritta, passando per la poppa, a 112,5 a sinistra
TUTTO = [-180.0, 180.0]

IN_MOTO = ['moto', 'manovra_limitata', 'non_governa']   # laterali e coronamento: in manovra limitata e senza governo si
                                                        # tengono se la nave ha abbrivio (regola 27; qui sempre)
ALBERO_IN_MOTO = ['moto', 'manovra_limitata']           # i fanali d'albero: senza governo no, solo i due rossi (27 a)

# le parti della plancia, in ordine di preferenza, per i fanali laterali
PLANCIA = ('timoneria', 'ponte_comando', 'plancia', 'finestre_plancia', 'sovrastruttura_prodiera', 'tuga_prodiera')


def leggi_modello(percorso):
    """Le parti del modello MDL2: per parte i vertici (n x 3) sopra il galleggiamento. Restituisce (lunghezza, quota
    massima, {nome: vertici})."""
    with open(percorso, 'rb') as f:
        dati = f.read()
    assert dati[:4] == b'MDL2', percorso
    n, _w, _h, _texel, _tono, _amb, lunghezza, quota = struct.unpack_from('<IIIfffff', dati, 4)
    p = 36
    (n_parti,) = struct.unpack_from('<H', dati, p)
    p += 2
    nomi = []
    for _ in range(n_parti):
        k = dati[p]
        nomi.append(dati[p + 1:p + 1 + k].decode('utf-8'))
        p += 1 + k
    (lung,) = struct.unpack_from('<I', dati, p)
    p += 4
    piani = zlib.decompress(dati[p:p + lung], -15)
    rec = np.frombuffer(piani, np.uint8).reshape(91, n).T.copy()
    v = rec[:, :36].copy().view('<f4').reshape(n, 3, 3)
    parte = rec[:, 89:91].copy().view('<u2').reshape(n)
    out = {}
    for i, nome in enumerate(nomi):
        vv = v[parte == i].reshape(-1, 3)
        vv = vv[vv[:, 2] >= 0.0]
        if len(vv):
            out[nome] = vv
    return float(lunghezza), float(quota), out


def r1(x):
    return round(float(x), 1)


def fanale(ruolo, colore, pos, settore, portata, stati, lampi=''):
    return {'ruolo': ruolo, 'colore': colore, 'pos_m': [r1(pos[0]), r1(pos[1]), r1(pos[2])], 'settore': settore,
            'portata_nm': portata, 'stati': stati, 'lampi': lampi}


def configura(classe, tipo, lunghezza, parti):
    """I fanali di una classe dalle parti del suo modello."""
    po = portate(lunghezza)
    scafo = parti.get('scafo')
    if scafo is None:   # ripiego: tutto il modello
        scafo = np.concatenate(list(parti.values()))
    x0, x1 = scafo[:, 0].min(), scafo[:, 0].max()
    lm = x1 - x0
    # le quote della coperta a poppa e a prua: il punto piu' alto dello scafo nell'ultimo e nel primo ventesimo
    poppa_z = scafo[scafo[:, 0] < x0 + 0.05 * lm][:, 2].max()
    prua_z = scafo[scafo[:, 0] > x1 - 0.05 * lm][:, 2].max()
    mezza = np.abs(scafo[:, 1]).max()
    out = []
    if tipo == 'sottomarino':
        # sulla vela: il fanale d'albero in cima, i laterali sui fianchi, il coronamento a poppa sulla coperta
        vela = parti.get('vela')
        if vela is None:
            raise SystemExit(f'luci: {classe} senza la parte «vela»')
        vx = (vela[:, 0].min() + vela[:, 0].max()) / 2.0
        vz = vela[:, 2].max()
        vy = np.abs(vela[:, 1]).max()
        out.append(fanale('albero', 'bianco', (vx, 0.0, vz + 0.3), ALBERO, po['albero'], ALBERO_IN_MOTO))
        out.append(fanale('laterale_sinistro', 'rosso', (vx, vy + 0.1, vz - 1.0), SINISTRO, po['laterale'], IN_MOTO))
        out.append(fanale('laterale_dritto', 'verde', (vx, -vy - 0.1, vz - 1.0), DRITTO, po['laterale'], IN_MOTO))
        out.append(fanale('coronamento', 'bianco', (x0 + 0.08 * lm, 0.0, poppa_z + 0.5), CORONAMENTO, po['coronamento'],
                          IN_MOTO))
        out.append(fanale('fonda_prora', 'bianco', (vx, 0.0, vz + 0.3), TUTTO, po['tutt_intorno'], ['fonda']))
        if classe in FARO_AMBRA:   # almeno due piedi sopra il fanale d'albero
            out.append(fanale('faro_ambra', 'ambra', (vx, 0.0, vz + 1.0), TUTTO, PORTATA_FARO_NM, ['moto'], '707.7'))
        out += segnali_albero((vx, vz + 0.3 - 2.0), po)
        return out
    # gli alberi: le parti che cominciano con «albero» (o l'alberetto, il pennone), ordinate da prua a poppa
    alberi = []
    for nome, vv in parti.items():
        if nome.startswith('albero') or nome in ('alberetto', 'pennone'):
            alberi.append((float((vv[:, 0].min() + vv[:, 0].max()) / 2.0), float(vv[:, 2].max()), nome))
    if not alberi:
        raise SystemExit(f'luci: {classe} senza alberi nel modello')
    alberi.sort(reverse=True)   # da prua
    prodiero = alberi[0]
    # l'albero poppiero: il piu' alto a poppavia del prodiero, se e' piu' alto di lui (Annesso I: almeno 4,5 m)
    poppavia = [a for a in alberi if a[0] < prodiero[0] - 0.1 * lm]
    poppiero = max(poppavia, key=lambda a: a[1]) if poppavia else None
    if poppiero and poppiero[1] <= prodiero[1]:
        poppiero = None if prodiero[1] - 4.5 < prua_z + 2.0 else poppiero
    x_alb, z_alb = prodiero[0], prodiero[1]
    if lunghezza >= 50.0 and poppiero:
        z_pop = poppiero[1]
        z_alb = min(z_alb, z_pop - 4.5)   # il prodiero piu' basso di almeno 4,5 m
        out.append(fanale('albero', 'bianco', (x_alb, 0.0, z_alb), ALBERO, po['albero'], ALBERO_IN_MOTO))
        out.append(fanale('albero_poppiero', 'bianco', (poppiero[0], 0.0, z_pop), ALBERO, po['albero'], ALBERO_IN_MOTO))
    else:   # un fanale solo: sotto i 50 m, o una militare con un albero solo (regola 1(c))
        out.append(fanale('albero', 'bianco', (x_alb, 0.0, z_alb), ALBERO, po['albero'], ALBERO_IN_MOTO))
    # i laterali sulla plancia
    plancia = next((parti[n] for n in PLANCIA if n in parti), None)
    if plancia is not None:
        px = (plancia[:, 0].min() + plancia[:, 0].max()) / 2.0
        py = np.abs(plancia[:, 1]).max()
        pz = plancia[:, 2].max() - 1.0
    else:
        px, py, pz = x0 + 0.75 * lm, mezza, prua_z + 2.0
    out.append(fanale('laterale_sinistro', 'rosso', (px, py, pz), SINISTRO, po['laterale'], IN_MOTO + ['pesca']))
    out.append(fanale('laterale_dritto', 'verde', (px, -py, pz), DRITTO, po['laterale'], IN_MOTO + ['pesca']))
    out.append(fanale('coronamento', 'bianco', (x0 + 1.0, 0.0, poppa_z + 1.0), CORONAMENTO, po['coronamento'],
                      IN_MOTO + ['pesca']))
    # alla fonda: uno a prua alto, e da 50 m uno a poppa piu' basso di almeno 4,5 m (regola 30)
    z_poppa_fonda = poppa_z + 2.0
    z_prora_fonda = max(prua_z + 6.0, z_poppa_fonda + 4.5 if lunghezza >= 50.0 else 0.0)
    out.append(fanale('fonda_prora', 'bianco', (x1 - 0.04 * lm, 0.0, z_prora_fonda), TUTTO, po['tutt_intorno'], ['fonda']))
    if lunghezza >= 50.0:
        out.append(fanale('fonda_poppa', 'bianco', (x0 + 0.04 * lm, 0.0, z_poppa_fonda), TUTTO, po['tutt_intorno'], ['fonda']))
    # i segnali sull'albero piu' alto: la pesca, la manovra limitata, la nave che non governa
    piu_alto = max(alberi, key=lambda a: a[1])
    if tipo == 'peschereccio':
        sopra = 'verde' if PESCA.get(classe) == 'strascico' else 'rosso'
        out.append(fanale('pesca_alto', sopra, (piu_alto[0], 0.0, piu_alto[1] - 0.5), TUTTO, po['tutt_intorno'], ['pesca']))
        out.append(fanale('pesca_basso', 'bianco', (piu_alto[0], 0.0, piu_alto[1] - 2.5), TUTTO, po['tutt_intorno'], ['pesca']))
        if lunghezza >= 50.0:   # il fanale d'albero in pesca, a poppavia e sopra il verde (regola 26 b ii)
            out.append(fanale('albero_pesca', 'bianco', (piu_alto[0] - 1.0, 0.0, piu_alto[1] + 1.0), ALBERO, po['albero'],
                              ['pesca']))
    out += segnali_albero((piu_alto[0], piu_alto[1] - 0.5), po)
    # le luci da lavoro della coperta: i pescherecci in pesca, le navi da 100 m alla fonda (regola 30 c)
    if tipo == 'peschereccio' or lunghezza >= 100.0:
        stati = ['pesca'] if tipo == 'peschereccio' else ['fonda']
        z_coperta = max(poppa_z, prua_z) + 3.0
        n = max(3, min(8, int(lm // 20)))
        for k in range(n):
            x = x0 + lm * (k + 0.5) / n
            out.append(fanale('coperta', 'luci_coperta', (x, 0.0, z_coperta), TUTTO, PORTATA_COPERTA_NM, stati))
    return out


def segnali_albero(cima, po):
    """Rosso-bianco-rosso della manovra limitata e i due rossi della nave che non governa, sotto la cima `(x, z)`, a 2 m."""
    x, z = cima
    return [fanale('manovra_rosso_alto', 'rosso', (x, 0.0, z), TUTTO, po['tutt_intorno'], ['manovra_limitata']),
            fanale('manovra_bianco', 'bianco', (x, 0.0, z - 2.0), TUTTO, po['tutt_intorno'], ['manovra_limitata']),
            fanale('manovra_rosso_basso', 'rosso', (x, 0.0, z - 4.0), TUTTO, po['tutt_intorno'], ['manovra_limitata']),
            fanale('governo_rosso_alto', 'rosso', (x, 0.0, z), TUTTO, po['tutt_intorno'], ['non_governa']),
            fanale('governo_rosso_basso', 'rosso', (x, 0.0, z - 2.0), TUTTO, po['tutt_intorno'], ['non_governa'])]


def toml_valore(v):
    if isinstance(v, str):
        return '"' + v + '"'
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, list):
        return '[' + ', '.join(toml_valore(x) for x in v) + ']'
    return repr(float(v)) if isinstance(v, float) else str(v)


def controlla(classe, cfg, quota_max):
    """Le regole che i dati devono rispettare: l'albero, i laterali e il coronamento ci sono; il secondo fanale d'albero
    solo da 50 m, piu' alto e a poppavia; tutto dentro il modello."""
    ruoli = [f['ruolo'] for f in cfg['fanali']]
    for r in ('albero', 'laterale_sinistro', 'laterale_dritto', 'coronamento', 'fonda_prora'):
        if r not in ruoli:
            raise SystemExit(f'luci: {classe} senza il fanale «{r}»')
    if 'albero_poppiero' in ruoli:
        a = next(f for f in cfg['fanali'] if f['ruolo'] == 'albero')
        p = next(f for f in cfg['fanali'] if f['ruolo'] == 'albero_poppiero')
        if cfg['lunghezza_m'] < 50.0 or p['pos_m'][2] < a['pos_m'][2] + 4.4 or p['pos_m'][0] >= a['pos_m'][0]:
            raise SystemExit(f'luci: {classe}, il fanale d\'albero poppiero non e\' piu\' alto e a poppavia')
    if 'albero_pesca' in ruoli:
        a = next(f for f in cfg['fanali'] if f['ruolo'] == 'albero_pesca')
        v = next(f for f in cfg['fanali'] if f['ruolo'] == 'pesca_alto')
        if a['pos_m'][2] <= v['pos_m'][2] or a['pos_m'][0] >= v['pos_m'][0]:
            raise SystemExit(f'luci: {classe}, in pesca il fanale d\'albero non e\' a poppavia e sopra il fanale da pesca')
    for f in cfg['fanali']:
        if not (0.0 <= f['pos_m'][2] <= quota_max + 1.5):
            raise SystemExit(f'luci: {classe}, il fanale «{f["ruolo"]}» fuori dal modello ({f["pos_m"][2]} m)')


def esporta_luci(radice):
    """Scrive data/motore/luci.toml; restituisce il numero delle classi."""
    motore = os.path.join(radice, 'data', 'motore')
    with open(os.path.join(motore, 'classi.toml'), 'rb') as f:
        classi = tomllib.load(f)
    p22 = portate(60.0), portate(30.0), portate(15.0)
    righe = ['# Le luci di navigazione e di fonda per classe (docs/27): generato da tools/pipeline/luci.py, non modificare a mano.',
             '# Posizioni sul modello 3D (x a prua, y a sinistra, z sul galleggiamento, metri); settori in gradi dalla prua,',
             '# positivi a dritta, da settore[0] a settore[1] in senso orario; portate in miglia (COLREG 1972, regola 22).',
             '', '[portate]',
             f'lunga_m = 50.0', f'albero_lunga = {p22[0]["albero"]}', f'altre_lunga = {p22[0]["laterale"]}',
             f'media_m = 12.0', f'albero_media = {p22[1]["albero"]}', f'albero_media_sotto20 = {p22[2]["albero"]}',
             f'altre_media = {p22[1]["laterale"]}', f'coperta = {PORTATA_COPERTA_NM}', f'faro_ambra = {PORTATA_FARO_NM}', '']
    n = 0
    for classe, tipo in TIPO.items():
        if classe not in classi or 'modello' not in classi[classe]:
            raise SystemExit(f'luci: la classe {classe} non e\' in classi.toml o non ha modello')
        lunghezza = float(classi[classe]['lunghezza_m'])
        _lm, quota_max, parti = leggi_modello(os.path.join(motore, classi[classe]['modello']))
        cfg = {'tipo': tipo, 'pesca': PESCA.get(classe, ''), 'lunghezza_m': lunghezza,
               'modello': classi[classe]['modello'], 'fanali': configura(classe, tipo, lunghezza, parti)}
        controlla(classe, cfg, quota_max)
        righe += [f'[classe.{classe}]', f'tipo = "{tipo}"', f'pesca = "{cfg["pesca"]}"', f'lunghezza_m = {lunghezza}',
                  f'modello = "{cfg["modello"]}"',
                  'fonte = "modello 3D (alberi, plancia, poppa, vela); COLREG 1972 regole 21-23, 26, 27, 30, Annesso I' +
                  ('; 32 CFR 707.7 per il faro ambra' if classe in FARO_AMBRA else '') + '"',
                  'conf = "media"', '']
        for f in cfg['fanali']:
            righe.append(f'[[classe.{classe}.fanale]]')
            righe += [f'{k} = {toml_valore(v)}' for k, v in f.items()]
            righe.append('')
        n += 1
    with open(os.path.join(motore, 'luci.toml'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(righe))
    return n


if __name__ == '__main__':
    print('luci.toml:', esporta_luci(sys.argv[1] if len(sys.argv) > 1 else '.'), 'classi')
