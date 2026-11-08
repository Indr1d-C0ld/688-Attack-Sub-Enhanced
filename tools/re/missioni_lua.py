"""Converte le 22 scene originali nel formato missione Lua del motore (docs/09_schema_missioni.md).

Entrate: extracted/raw/DISK1/SCENE*.TOK (bytecode), l'esecutore simbolico di tok_schede.py (varianti
iniziali con probabilita' esatte), i gestori di lancio (prestazioni.py), le traduzioni delle scene
(docs/traduzione/scene/), la georeferenziazione (data/teatri/associazione_surf.json, heightmap.json; per SURFB e SURFG
la ricollocazione re/surf/ricollocazione.json di ricolloca_surf.py) e le
chiavi delle classi (data/motore/classi.toml).
Uscite: data/missioni/<SCENA>.lua e data/missioni/API.md (funzioni m688 usate, con l'opcode d'origine).

Uso:  missioni_lua.py <radice> [SCENA ...] [--verifica] [--uscita CARTELLA]   (senza scene: tutte; --uscita scrive altrove)
"""
import os, sys, glob, json, re, collections, subprocess, tomllib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tok_schede as T
import prestazioni as P
from tok_disasm import decode
from tok_ops import OPS, TERMINA
from pyproj import Transformer

FT = 0.3048
# tipo d'unita' dell'originale -> chiave di classe (data/motore/classi.toml e classi annunciate)
CLASSI = {0x00: 'spruance', 0x01: 'mercantile', 0x02: 'cargo', 0x05: 'kara', 0x06: 'kirov', 0x07: 'krivak2',
          0x08: 'koni', 0x09: 'slava', 0x0a: 'nimitz', 0x0b: 'iowa', 0x0c: 'kidd', 0x0d: 'perry', 0x0e: 'sacramento',
          0x0f: 'bqm74', 0x10: 'petroliera', 0x11: 'ka25', 0x12: 'sh60b', 0x13: 'mss', 0x14: 'los_angeles',
          0x15: 'alfa', 0x16: 'mk48', 0x17: 'victor3', 0x18: 'p3c', 0x19: 'biologico', 0x1a: 'los_angeles',
          0x1b: 'sturgeon', 0x1c: 'oscar', 0x1d: 'akula', 0x1e: 'charlie2', 0x1f: 'typhoon', 0x20: 'kilo',
          0x21: 'foxtrot', 0x22: 'trafalgar', 0x23: 'tomahawk', 0x24: 'sea_lance', 0x25: 'type53', 0x26: 'type40',
          0x27: 'spearfish', 0x28: 'p700', 0x2b: 'boa_marcatrice'}
TITOLI_IT = {0: 'Colpisci e fuggi', 10: 'Colpisci e fuggi', 1: "TORPEX '89", 2: 'Scrollatevelo di dosso', 3: 'Operazioni subacquee', 4: 'Mumar Cadaver',
             5: 'Fuga', 6: 'Gulasch', 7: 'Passerella', 8: 'Festa a sorpresa', 9: 'Ritorno a casa'}   # glossario §8
ARMI = {0: 'mk48', 1: 'type53', 2: 'sea_lance', 3: 'tomahawk', 4: 'harpoon', 5: 'esca'}
LATO = {'A': 'usa', 'B': 'urss', 'C': 'due_giocatori'}

# opcode -> nome dell'API m688; 'c' = condizione (l'ultima etichetta e' il salto)
API = {
 'POS': 'posizione', 'DEPTH_NOW': 'quota_ora', 'TYPE': 'modello_tipo', 'FLAGS': 'modello_flag', 'HEAD_NOW': 'rotta_ora',
 'SPEED_NOW': 'velocita_ora', 'P_DIVE': 'modello_rateo_quota', 'P_TURN': 'modello_virata', 'P_ACCEL': 'modello_accelerazione',
 'P_VMAX': 'modello_avanti_tutta', 'P_3A': 'modello_rumore', 'P_3B': 'modello_sonar', 'P_3C': 'modello_3c', 'P_SKILL': 'modello_difesa',
 'NEW_UNIT': 'nuova_unita', 'CLEAR': 'modello_azzera', 'GOTO_XY': 'vai_a', 'P_4C': 'modello_4c', 'AT': 'dopo', 'STORE': 'salva_modello',
 'SELECT_CUR': 'seleziona_corrente', 'SET_24': 'imposta_24', 'SET_2A': 'imposta_2a', 'TURN_TO': 'accosta_a', 'SPEED_STEP': 'velocita_a_passi',
 'LOAD': 'carica_modello', 'PURSUE': 'insegui', 'PURSUE2': 'insegui2', 'BEHAVIOR': 'comportamento', 'TURN_RATE': 'rateo_virata',
 'TELEGRAPH': 'telegrafo', 'DEPTH_TO': 'quota', 'MSG_IF_PLAYER': 'messaggio_al_giocatore', 'HEAD_TO': 'rotta', 'STOP_NAV': 'ferma_navigazione',
 'UNIT_TYPE': 'modello_classe', 'P_3C2': 'modello_3c', 'PING': 'impulso_sonar', 'CANCEL': 'annulla_timer', 'CANCEL_ALL': 'annulla_tutti_i_timer',
 'LAUNCH': 'lancia', 'BREAK': 'interrompi', 'F54_2': 'periscopio', 'F54_1': 'schiera_trainata', 'NO_BEHAVIOR': 'nessun_comportamento',
 'LINK': 'collega', 'SAVE_TARGET': 'memorizza_bersaglio', 'DEPTH_MAX': 'modello_quota_max', 'TARGET_ID': 'bersaglio',
 'SELECT_LAST': 'togli_unita', 'HEALTH': 'modello_integrita', 'NAVFLAG': 'navigazione', 'LAYERS': 'termoclini',
 'ENGAGE': 'ingaggia', 'IF_TRACK_LONG': 'traccia_lunga', 'IF_TRACK_GOOD': 'traccia_solida', 'IF_TGT_F20': 'bersaglio_arma',
 'IF_TGT_ENEMY': 'bersaglio_nemico', 'IF_TGT_F02': 'bersaglio_nave', 'IF_TGT_NEUTRAL': 'bersaglio_neutrale',
 'IF_TRACK_MED': 'traccia_media', 'IF_RND': 'caso', 'IF_ID': 'unita_id', 'UFLAGS': 'flag_scena', 'IF_UFLAGS': 'flag_scena_uguale',
 'IF_EXISTS': 'unita_esiste', 'UFLAGS_INC': 'flag_scena_piu', 'IF_ID2': 'unita_id', 'IF_TGT_ID': 'bersaglio_id',
 'VAR_SET': 'variabile', 'VAR_INC': 'variabile_piu', 'IF_VAR_GT': 'variabile_maggiore', 'IF_VAR_LT': 'variabile_minore',
 'IF_VAR_EQ': 'variabile_uguale', 'VAR_DEC': 'variabile_meno', 'IF_HIT': 'unita_colpita', 'IF_TGT_NEAR': 'bersaglio_vicino',
 'ATTACK': 'attacca', 'MSG': 'messaggio', 'IF_TGT_LOCKED': 'bersaglio_mi_punta', 'CONTACTS_MARK': 'marca_contatti',
 'SELECT_TGT': 'seleziona_bersaglio', 'MISSILE': 'lancio_missile', 'RETARGET': 'devia_riferimenti', 'F54_4': 'antenna',
 'IF_IN_RECT': 'in_rettangolo', 'IF_TGT_F03': 'bersaglio_aereo', 'UFLAGS_DEC': 'flag_scena_meno', 'RANDOM': 'nuovo_caso',
 'TURN_TO_TGT': 'accosta_verso_bersaglio', 'NAV_ON': 'riprendi_navigazione', 'TURN_REL': 'virata_relativa',
 'GOTO_TGT': 'vai_al_bersaglio', 'IF_VARCMP': 'confronta_variabili', 'MATCH_DEPTH': 'uguaglia_quota', 'MATCH_SPEED': 'uguaglia_velocita',
 'IF_PLAYER1': 'giocatore_secondo', 'IF_STANDARD': 'livello_standard', 'ATTACK_RND': 'attacco_casuale',
 'SELECT_LINKED': 'seleziona_collegata', 'IF_X_GT': 'x_maggiore', 'IF_UFLAG_BIT': 'flag_scena_bit', 'UFLAG_SET': 'flag_scena_accendi',
 'UFLAG_CLR': 'flag_scena_spegni', 'CLR_2C': 'azzera_rateo_quota', 'TURN_AWAY': 'accosta_opposto', 'JUMP_TO_TGT': 'teletrasporta_al_bersaglio',
 'TRANSIENT': 'rumore_transitorio', 'SELECT_ID': 'seleziona', 'IF_TAMPERED': 'manomesso',
}
CONTROLLO = {'END', 'CALL', 'GOTO', 'HANDLERS_A', 'HANDLERS_B', 'HANDLERS_C', 'HANDLERS_D', 'HANDLERS_E', 'HANDLERS_F'}
# ritardo del timer con cui un IF che salta su se stesso si riprova: meno di un passo della VM (1/9 s), piu' di zero
# (un timer a zero scadrebbe nello stesso giro dei timer)
ATTESA_S = 0.05
# Gestori del giocatore che ricevono parametri (docs/originale/sensori_e_navigazione.md §7): il dispatcher li scrive
# negli argomenti di un'istruzione del gestore. (indice dell'istruzione nel flusso dall'etichetta, numero di parametri)
PARAMETRI_GIOCATORE = {'punto_di_rotta': (0, 2), 'telegrafo': (0, 1), 'quota': (0, 2), 'timone': (0, 1),
                       'periscopio': (0, 1), 'antenna': (0, 1), 'designa_bersaglio': (0, 1), 'posizione': (0, 2),
                       'guida_e2': (1, 2)}

PESCAGGI = {}   # classe -> pescaggio (m), da data/motore/classi.toml

def bussola(r):
    """Angolo dell'originale (matematico, 9000esimi) -> gradi bussola 0..360."""
    return (90 - r * 360 / 9000) % 360

def numero(v):
    v = round(v, 4)
    return str(int(v)) if v == int(v) else repr(v)

class Posa:
    """Unita' della carta originale -> metri sul teatro (docs/09_schema_missioni.md §3)."""
    def __init__(self, root, teatro, mappa):
        a = json.load(open(f'{root}/data/teatri/associazione_surf.json'))['mappe'][mappa]
        h = json.load(open(f'{root}/data/teatri/{teatro}/heightmap.json'))
        p = f'{root}/re/surf/ricollocazione.json'
        r = json.load(open(p)).get(mappa) if os.path.exists(p) else None
        self.tr = Transformer.from_crs('EPSG:4326', h['proiezione']['proj4'], always_xy=True)
        self.root, self.teatro, self.griglia = root, teatro, h['griglia']; self._z = None; self._zmin = None
        self.spostate = []                      # (cosa, metri) delle correzioni nell'acqua piu' vicina
        la0, la1, lo0, lo1 = a['bbox_lat_lon']; self.bbox = a['bbox_lat_lon']
        self.aff = a['affidabilita_posizione']
        self.metodo = 'georef' if self.aff in ('alta', 'media') else 'scala_di_gioco'
        self.clat, self.clon = (la0 + la1) / 2, (lo0 + lo1) / 2
        self.cx, self.cy = self.tr.transform(self.clon, self.clat)
        if self.metodo == 'georef':
            self.mx = a['larghezza_nm'] * 1852 / 10240; self.my = a['altezza_nm'] * 1852 / 6400
        elif r:                                 # ricolloca_surf.py: terre emerse della carta sulle coste reali
            self.metodo, self.aff, self.jaccard = 'ricollocata', 'media', r['jaccard']
            self.mx = self.my = r['metri_per_unita']; self.cx, self.cy = r['centro_m']; self.clat, self.clon = r['centro_lat_lon']
        else:                                   # centro del teatro (il riquadro trovato non e' affidabile)
            self.mx = self.my = 1852 / 50
            self.clat, self.clon = h['proiezione']['centro_lat'], h['proiezione']['centro_lon']; self.cx = self.cy = 0.0
    def m(self, x, y):
        if x is None or y is None: return None
        if self.metodo == 'georef':
            la0, la1, lo0, lo1 = self.bbox
            return self.tr.transform(lo0 + (lo1 - lo0) * x / 10240, la0 + (la1 - la0) * y / 6400)
        return self.cx + (x - 5120) * self.mx, self.cy + (y - 3200) * self.my
    def fondale(self, x, y):
        """Profondita' (m, positiva) della heightmap nel punto; None fuori dalla carta."""
        import numpy as np
        from PIL import Image
        if self._z is None:
            h = json.load(open(f'{self.root}/data/teatri/{self.teatro}/heightmap.json'))
            self._z = -(np.array(Image.open(f'{self.root}/data/teatri/{self.teatro}/heightmap_u16.png')).astype(float)
                        * h['codifica']['scala'] + h['codifica']['offset'])
        g = self.griglia; c = g['cella_m']
        col = int((x + g['larghezza'] * c / 2) // c); row = int((g['altezza'] * c / 2 - y) // c)
        if not (0 <= row < self._z.shape[0] and 0 <= col < self._z.shape[1]): return None
        return float(self._z[row, col])
    def acqua(self, x, y, minima, cosa, raggio=10000.0):
        """Punto spostato nella cella piu' vicina con fondale >= minima (entro il raggio); invariato se gia' va bene.
        Per le unita' (cosa == 'unita') il fondale conta come il minimo entro +-400 m, cosi' non partono sul ciglio
        di una secca: a 16 kn una nave fa 60 m in 7 s, meno della frenata."""
        import numpy as np
        f = self.fondale(x, y)
        if f is None: return x, y
        g = self.griglia; c = g['cella_m']; R = int(raggio // c)
        col = int((x + g['larghezza'] * c / 2) // c); row = int((g['altezza'] * c / 2 - y) // c)
        z = self._z
        if cosa == 'unita':
            if self._zmin is None:
                from scipy.ndimage import minimum_filter
                self._zmin = minimum_filter(self._z, size=5, mode='nearest')
            z = self._zmin; f = float(z[row, col])
        if f >= minima: return x, y
        r0, c0 = max(0, row - R), max(0, col - R)
        w = z[r0:row + R + 1, c0:col + R + 1]
        rr, cc = np.nonzero(w >= minima)
        if len(rr) == 0 and minima > 15 and f < 15:      # acque basse ovunque (baia di Pomerania): basta 15 m
            rr, cc = np.nonzero(w >= 15)
        if len(rr) == 0: self.spostate.append((cosa, None)); return x, y
        d = (rr + r0 - row) ** 2 + (cc + c0 - col) ** 2; k = int(np.argmin(d))
        nx = (cc[k] + c0 + 0.5) * c - g['larghezza'] * c / 2; ny = g['altezza'] * c / 2 - (rr[k] + r0 + 0.5) * c
        self.spostate.append((cosa, float(np.hypot(nx - x, ny - y)))); self.ultimo = self.spostate[-1][1]; return nx, ny
    def correzioni(self):
        d = [m for _, m in self.spostate if m is not None]
        return (f'correzioni = {{ unita = {sum(1 for k, m in self.spostate if k == "unita" and m is not None)}, '
                f'punti = {sum(1 for k, m in self.spostate if k != "unita" and m is not None)}, '
                f'spostamento_max_m = {numero(max(d)) if d else 0}, non_risolte = {sum(1 for _, m in self.spostate if m is None)} }}')
    def lua(self):
        nota = {'georef': 'carta originale collocata sul riquadro trovato da georef_surf.py',
                'ricollocata': f'terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard {getattr(self, "jaccard", 0):.2f}); traslazione e scala, senza rotazione',
                'scala_di_gioco': 'collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing'}[self.metodo]
        # moto dell'originale: velocita' grezza x 9 passi/s / 65536 = 0,048065 unita'/s per nodo (circa 3,46 volte il vero)
        classico = 9 * 350 / 65536 * (self.mx + self.my) / 2
        return (f'{{ metodo = "{self.metodo}", centro = {{ lat = {self.clat:.4f}, lon = {self.clon:.4f} }}, '
                f'metri_per_unita = {{ x = {self.mx:.3f}, y = {self.my:.3f} }}, moto_classico_m_s_per_nodo = {classico:.4f}, '
                f'affidabilita = "{self.aff}", nota = "{nota}" }}')

def stringa(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'

class Traduttore:
    """Bytecode della VM -> funzioni Lua che chiamano m688."""
    def __init__(self, d, posa):
        self.d, self.posa = d, posa; self.usate = collections.Counter(); self.riferite = set()
        self.parametri = {}   # etichetta -> (pc dell'istruzione, numero di parametri) dei gestori del giocatore
    def arg(self, name, i, c, v, args):
        if c == 'L': self.riferite.add(v); return f'"L_{v:04x}"'
        if c == 'V': return f'{v}'
        if c == 'T':
            return f'{numero((v & 0x7fff) * 2)}' + (', "assoluto"' if v & 0x8000 else '')
        w = v - 65536 if c == 'w' and v > 32767 else v
        if name == 'IF_TGT_NEAR' or (name == 'ATTACK' and i == 0): return numero(v / 50)
        if name in ('TELEGRAPH',): return numero(v / 256) if v != 999 else '"arresto_rapido"'
        if name in ('DEPTH_NOW', 'DEPTH_MAX') or (name == 'DEPTH_TO' and i == 0): return numero(v / 8 * FT)
        if name == 'DEPTH_TO' and i == 1: return numero(v / 256)
        # angoli: l'originale usa angoli matematici (0 = est, antiorario, 9000 = 360 gradi; numeri_eseguibile.md §3.3);
        # nelle missioni rotte in gradi bussola e virate con il positivo a dritta
        if name in ('HEAD_NOW', 'TURN_TO', 'HEAD_TO'):
            return 'nil' if w == -1 else numero(bussola(w))
        if name == 'TURN_REL': return numero(-w * 360 / 9000)
        if name in ('SPEED_NOW', 'P_VMAX') or (name == 'SPEED_STEP' and i == 0): return numero(v / 350)
        if name == 'P_TURN': return numero(w * 0.36)          # modulo del rateo standard
        if name == 'TURN_RATE': return numero(-w * 0.36)      # positivo = dritta
        if name == 'P_ACCEL': return numero(v * 9 / 350)
        if name == 'P_DIVE': return numero(v * 9 / 8 * FT)
        if name == 'IF_RND': return numero((v + 1) / 256)
        if name in ('F54_1', 'F54_2', 'F54_4'): return '1' if v else '0'   # bit di +54: schiera, periscopio, antenna
        if name == 'UNIT_TYPE': return f'"{CLASSI.get(v, f"tipo_{v:02x}")}"'
        return str(w)
    def chiamata(self, name, args, salta=None, parametri=0):
        api = API.get(name, name.lower()); self.usate[(api, name)] += 1
        if name in ('POS', 'GOTO_XY') or name == 'IF_IN_RECT':
            vv = [v for c, v in args]
            if name == 'IF_IN_RECT':
                a = self.posa.m(vv[0], vv[1]); b = self.posa.m(vv[2], vv[3])
                parti = [numero(a[0]), numero(a[1]), numero(b[0]), numero(b[1])]
            else:
                p = self.posa.m(vv[0], vv[1])
                if name == 'GOTO_XY' and (vv[0], vv[1]) != (0, 6400): p = self.posa.acqua(*p, 10.0, 'script')   # solo destinazioni
                parti = [numero(p[0]), numero(p[1])] + [self.arg(name, i, c, v, args) for i, (c, v) in enumerate(args[2:], 2)]
        elif name == 'IF_X_GT':
            parti = [numero(self.posa.m(args[0][1], 3200)[0])]
        else:
            parti = [self.arg(name, i, c, v, args) for i, (c, v) in enumerate(args) if not (salta is not None and i == salta)]
        # gestore del giocatore: i primi argomenti sono i parametri del comando, con il valore originale come predefinito
        parti = [f'par(p, {i + 1}, {x})' if i < parametri else x for i, x in enumerate(parti)]
        return f'm688.{api}(' + ', '.join(['u'] + parti) + ')'
    def funzione(self, lab):
        """Codice raggiungibile dall'etichetta (salti e cadute; le CALL diventano chiamate)."""
        vis = {}; pila = [lab]
        while pila:
            pc = pila.pop()
            while pc not in vis and 0 <= pc < len(self.d):
                r = decode(self.d, pc)
                if r is None: break
                vis[pc] = r; op, name, args, nxt = r
                if name == 'GOTO': pila.append(args[0][1]); break
                if name.startswith('IF_'): pila.append([v for c, v in args if c == 'L'][-1] if name != 'IF_VARCMP' else args[2][1])
                if name == 'IF_VARCMP': pila.append(args[3][1])
                if op in TERMINA: break
                pc = nxt
        destinazioni = {lab}
        for pc, (op, name, args, nxt) in vis.items():
            if name == 'GOTO' or name.startswith('IF_'):
                destinazioni |= {v for c, v in args if c == 'L'}
            if name == 'IF_VARCMP': destinazioni.add(args[2][1] + 2)
        righe = []; prec_fine = False
        for pc in sorted(vis):
            op, name, args, nxt = vis[pc]
            if pc in destinazioni: righe.append(f'  ::L_{pc:04x}::')
            prec_fine = False
            if name == 'END': righe.append('  do return end'); prec_fine = True
            elif name == 'GOTO': righe.append(f'  goto L_{args[0][1]:04x}'); prec_fine = True
            elif name == 'CALL': self.riferite.add(args[0][1]); righe.append(f'  script.L_{args[0][1]:04x}(u)')
            elif name == 'IF_VARCMP':
                a, b, l1, l2 = [v for c, v in args]
                righe.append(f'  do local c = m688.confronta_variabili(u, {a}, {b}) if c > 0 then goto L_{l2:04x} '
                             f'elseif c == 0 then goto L_{l1 + 2:04x} end end  -- difetto originale: uguali -> etichetta + 2')
            elif name.startswith('IF_'):
                k = max(i for i, (c, v) in enumerate(args) if c == 'L'); dest = args[k][1]
                if dest == pc:
                    # salto su se stesso (SCENE9A 04F7, 9B 05F1): la VM (2011:137C) non cede il turno, ma gli script
                    # girano nell'ISR del timer, rientrante (2011:0162: EOI e sti), e gli interrupt annidati fanno
                    # avanzare la simulazione: un'attesa finche' la condizione vale. Qui si riprova al passo dopo
                    self.riferite.add(pc)
                    righe.append(f'  if {self.chiamata(name, args, salta=k)} then m688.dopo(u, {ATTESA_S}, "L_{pc:04x}") do return end end'
                                 '  -- attesa: si riprova al passo successivo')
                else:
                    righe.append(f'  if {self.chiamata(name, args, salta=k)} then goto L_{dest:04x} end')
            elif name.startswith('HANDLERS'): righe.append(f'  -- {name} (gestori del giocatore: vedi gestori_giocatore)')
            else:
                par = self.parametri.get(lab)
                n = par[1] if par and par[0] == pc else 0
                righe.append(f'  {self.chiamata(name, args, parametri=n)}')
                if op in TERMINA: righe.append('  do return end'); prec_fine = True
        if not prec_fine: righe.append('  do return end')
        # etichette di salto che non sono inizio di istruzione nel flusso: segnaposto
        definite = set(re.findall(r'::L_([0-9a-f]{4})::', '\n'.join(righe)))
        for dst in sorted(destinazioni):
            if f'{dst:04x}' not in definite: righe.append(f'  ::L_{dst:04x}:: do return end  -- destinazione fuori dal flusso')
        return righe

def unita_lua(u, posa, giocatore_id, d):
    t = u.get('tipo'); fl = u.get('flag', 0); cat = {0: 'arma' if fl & 0x20 else 'nessuna', 1: 'sottomarino', 2: 'nave', 3: 'aereo'}[fl & 3]
    if fl & 0x20: cat = 'arma'
    lato = {0x08: 'usa', 0x10: 'urss', 0x18: 'urss'}.get(fl & 0x18, 'neutrale')
    m = P.leggibile(u); x, y = posa.m(u.get('x', 0), u.get('y', 0))
    q = u.get('quota', 0) / 8 * FT; q_orig = None; spost = None
    if cat in ('nave', 'sottomarino') and not fl & 0x80 and (u.get('x', 0), u.get('y', 0)) != (0, 6400):   # niente falsi bersagli
        # navi: almeno il pescaggio della classe + 3 m (come l'acqua comoda dell'IA del motore), mai meno di 10 m
        classe = CLASSI.get(t, '')
        minima = max(10.0, PESCAGGI.get(classe, 7.0) + 3.0) if cat == 'nave' else 25.0
        posa.ultimo = None; x, y = posa.acqua(x, y, minima, 'unita'); spost = posa.ultimo
        f = posa.fondale(x, y)
        if cat == 'sottomarino' and f is not None and q > f - 10:
            q_orig, q = q, max(0.0, f - 10)
    minima_rotta = max(10.0, PESCAGGI.get(CLASSI.get(t, ''), 7.0) + 3.0) if cat == 'nave' else 10.0
    punti, ciclica = T.rotta(d, u.get('arrivo'))
    if u.get('destinazione'): punti = [tuple(u['destinazione'])] + punti
    altri = [k for k, bit in (('non_combattente', 0x40), ('falso_bersaglio', 0x80)) if fl & bit]
    g = lambda k: f'"L_{u[k]:04x}"' if u.get(k) is not None else 'nil'
    modello = (f"{{ massima_kn = {m['massima_kn']}, virata_gradi_s = {m['virata_gradi_s']}, accel_kn_s = {m['accel_kn_s']}, "
               f"quota_m_s = {numero(m['quota_ft_s'] * FT) if m['quota_ft_s'] is not None else 'nil'}, "
               f"quota_max_m = {numero(m['quota_max_ft'] * FT) if m['quota_max_ft'] else 'nil'}, integrita = {m['integrita']}, "
               f"rumore = {m['rumore_3a'] if m['rumore_3a'] is not None else 'nil'}, sonar = {m['sensibilita_3b'] if m['sensibilita_3b'] is not None else 'nil'}, "
               f"difesa = {m['danno_3d'] if m['danno_3d'] is not None else 'nil'}, schiera_trainata = {'true' if m['schiera_trainata'] else 'false'}, "
               f"controllo_fondale = {'true' if (u.get('nav') or 0) & 1 else 'false'}, periodo_controlli = {u.get('p4c') or 'nil'} }}")   # +4E bit 0, +4C
    pp = ', '.join(f'{{ x = {numero(a)}, y = {numero(b)} }}' for a, b in
                   (posa.acqua(*posa.m(px, py), minima_rotta, 'rotta') if cat in ('nave', 'sottomarino') and not fl & 0x80 else posa.m(px, py) for px, py in punti))
    return (f"      {{ id = {u['id']}, classe = \"{CLASSI.get(t, f'tipo_{t:02x}' if t is not None else 'sconosciuta')}\", "
            f"tipo_originale = {t if t is not None else 'nil'}, nome_originale = {stringa(T.TIPI.get(t, '?'))}, "
            f"categoria = \"{cat}\", schieramento = \"{lato}\", flag = {fl}," + (f" tipo = {u['p04']}," if u.get('p04') is not None else '') + (f" attributi = {{ {', '.join(chr(34) + a + chr(34) for a in altri)} }}," if altri else '') +
            (' giocatore = true,' if u['id'] == giocatore_id else '') + '\n'
            f"        posizione = {{ x = {numero(x)}, y = {numero(y)} }}, originale = {{ x = {u.get('x', 0)}, y = {u.get('y', 0)} }},"
            + (f" spostamento_m = {numero(spost)}," if spost else '') + "\n"
            f"        quota_m = {numero(q)}," + (f" quota_originale_m = {numero(q_orig)}," if q_orig is not None else '') + f" rotta = {numero(bussola(u.get('rotta', 0)))}, velocita_kn = {numero(u.get('velocita', 0) / 350)},\n"
            f"        modello = {modello},\n"
            + (f"        rotta_punti = {{ {pp} }}, rotta_ciclica = {'true' if ciclica else 'false'},\n" if punti else '') +
            f"        gestori = {{ bersaglio = {g('comportamento')}, reazione = {g('reazione')}, arrivo = {g('arrivo')} }} }},")

def testi(root, sc):
    p = f'{root}/docs/traduzione/scene/{sc}.json'
    if not os.path.exists(p): return [], {}
    s = json.load(open(p)); brief = []; esiti = {}
    for pg in s['pagine']:
        if pg['ruolo'] == 'ordini': brief = pg['paragrafi']
        elif pg['ruolo'] == 'esito': esiti[pg['esito']] = pg['paragrafi']
    return brief, esiti

def converti(root, path, classi_note):
    sc = os.path.basename(path)[:7]; d = open(path, 'rb').read()
    scheda = json.load(open(f'{root}/re/tok/schede/{sc}.json'))
    cfg = {int(k): v for k, v in scheda['configurazione_lza'].items()}
    posa = Posa(root, scheda['teatro'], scheda['mappa']); tr = Traduttore(d, posa)
    percorsi = sorted(T.esegui_init(d), key=lambda p: -p['prob'])
    tot = sum(p['prob'] for p in percorsi); tenuti = percorsi[:50]; cop = sum(p['prob'] for p in tenuti)
    giocatore = 0
    L = [f'-- {sc}: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).',
         '-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).', '',
         'local script = {}', '',
         '-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale',
         'local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end', '',
         'local missione = {', '  formato = 1,', f'  id = "{sc}",', f"  missione = {scheda['missione']},",
         f"  titolo = {{ it = {stringa(TITOLI_IT[scheda['missione']])}, en = {stringa(T.MISSIONI[scheda['missione'] % 10])} }},",
         f"  lato = \"{LATO[sc[6]]}\",", f"  teatro = \"{scheda['teatro']}\",", f'  posa = {posa.lua()},',
         '  data = nil, ora_utc = nil,']
    term = next((p['glob'].get('termoclini_ft') for p in percorsi if p['glob'].get('termoclini_ft')), None)
    L.append(f"  ambiente = {{ termoclini_m = {{ {', '.join(numero(v * FT) for v in term)} }} }}," if term else '  ambiente = {},')
    dot = {ARMI[i]: cfg.get(i + 1, 0) for i in range(6) if cfg.get(i + 1)}
    L.append('  dotazione = { ' + ', '.join(f'{k} = {v}' for k, v in dot.items()) + ' },')
    L.append('  tubi = { ' + ', '.join(f'"{ARMI.get(cfg.get(7 + i), "?")}"' for i in range(7)) + ' },')
    # armi dai gestori di lancio
    armi = []; gest = {}; pc = 0
    while pc < 0x60:
        x = decode(d, pc)
        if x is None: break
        if x[1].startswith('HANDLERS'): gest[x[1]] = [v for c, v in x[2]]
        if x[1] not in ('HANDLERS_A', 'HANDLERS_B', 'HANDLERS_C', 'HANDLERS_D', 'HANDLERS_E', 'HANDLERS_F'): break
        pc = x[3]
    for i, lab in enumerate(gest.get('HANDLERS_C', [])):
        if not cfg.get(i + 1): continue        # tipo non imbarcato: il gestore e' un segnaposto
        g = P.gestore(d, lab); m = g['modello']
        if m.get('vmax') and g['tipo'] is not None:
            corsa = m['vmax'] * m.get('telegrafo', 256) / 256 / 350
            filo = next((e['tick'] * 2 for e in g['eventi'] if any('LINK 0' in s for s in e['istruzioni'])), None)
            armi.append(f"    {ARMI[i]} = {{ classe = \"{CLASSI.get(g['tipo'], '?')}\", velocita_kn = {numero(corsa)}, "
                        f"corsa_s = {g['fine_corsa_s'] if 'fine_corsa_s' in g else (g['fine_corsa_tick'] or 0) * 2}, filo_s = {filo if filo else 'nil'}, "
                        f"danno = {m.get('p3d', 'nil')}, rumore = {m.get('p3a', 'nil')}, testa = {m.get('p3b', 'nil')}, "
                        f"virata_gradi_s = {numero(m['p_virata'] * 0.36) if m.get('p_virata') else 'nil'}, "
                        f"quota_max_m = {numero(m['quota_max'] / 8 * FT) if m.get('quota_max') else 'nil'}, gestore = \"L_{lab:04x}\" }},")
        else:
            armi.append(f"    {ARMI[i]} = {{ gestore = \"L_{lab:04x}\" }},   -- senza modello di corsa (missile o esca: vedi lo script)")
        tr.riferite.add(lab)
    L += ['  armi = {'] + armi + ['  },']
    # ordine degli argomenti degli opcode HANDLERS_* (tabelle del dispatcher 2011:1F3C, docs/09 §6.1)
    nomi_g = {'HANDLERS_A': ['punto_di_rotta', 'telegrafo', 'quota', 'timone', 'pilota_automatico_off', 'periscopio'],
              'HANDLERS_B': [f'carica_tubo_{i + 1}' for i in range(7)],
              'HANDLERS_D': ['schiera_cala', 'schiera_recupera', 'schiera_persa', 'designa_bersaglio', 'posizione', 'antenna'],
              'HANDLERS_E': ['guida_e0', 'guida_e1', 'guida_e2'], 'HANDLERS_F': ['f0', 'f1']}
    L.append('  gestori_giocatore = {')
    for h, nn in nomi_g.items():
        for n, lab in zip(nn, gest.get(h, [])):
            L.append(f'    {n} = "L_{lab:04x}",'); tr.riferite.add(lab)
            if n in PARAMETRI_GIOCATORE:   # l'istruzione che riceve i parametri, contando dall'etichetta
                k, quanti = PARAMETRI_GIOCATORE[n]; pc = lab
                for _ in range(k): pc = decode(d, pc)[3]
                tr.parametri[lab] = (pc, quanti)
    L.append('  },')
    # varianti
    L.append(f'  copertura = {float(cop / tot):.12g},')
    L.append('  varianti = {')
    for p in tenuti:
        L.append(f"    {{ probabilita = {float(p['prob'] / cop):.12g}, probabilita_originale = \"{p['prob']}\", unita = {{")
        for u in p['unita']:
            L.append(unita_lua(u, posa, giocatore, d))
            for k in ('comportamento', 'reazione', 'arrivo'):
                if u.get(k) is not None: tr.riferite.add(u[k])
        L.append('      },')
        tm = [f"{{ dopo_s = {numero((t['tempo'] & 0x7fff) * 2)},{' assoluto = true,' if t['tempo'] & 0x8000 else ''} unita = {t['unita'] if t['unita'] is not None else 'nil'}, gestore = \"L_{t['etichetta']:04x}\" }}"
              for t in p['timer']]
        for t in p['timer']: tr.riferite.add(t['etichetta'])
        L.append('      timer = { ' + ', '.join(tm) + ' },')
        L.append('      variabili = { ' + ', '.join(f'[{k}] = {v}' for k, v in sorted(p['var'].items())) + ' },')
        L.append('    },')
    L.append('  },')
    brief, esiti = testi(root, sc)
    L.append('  briefing = {')
    for lingua in ('it', 'en'):
        L.append(f'    {lingua} = {{ ' + ', '.join(stringa(x[lingua]) for x in brief) + ' },')
    L.append('  },')
    L.append('  esiti = {')
    for n, pp in sorted(esiti.items()):
        L.append(f"    [{n}] = {{ it = {stringa(chr(10).join(x['it'] for x in pp))}, en = {stringa(chr(10).join(x['en'] for x in pp))} }},")
    L.append('  },')
    usate_classi = sorted({c for c in re.findall(r'classe = "([a-z0-9_]+)"', '\n'.join(L))})
    mancanti = [c for c in usate_classi if c not in classi_note]
    L.append('  classi_mancanti = { ' + ', '.join(f'"{c}"' for c in mancanti) + ' },')
    L.append('  @@CORREZIONI@@')   # riempito dopo gli script (anche loro spostano punti nell'acqua)
    L += ['  script = script,', '}', '']
    # funzioni per tutte le etichette riferite (chiusura: le funzioni ne riferiscono altre)
    fatte = set(); corpo = []
    while tr.riferite - fatte:
        lab = min(tr.riferite - fatte); fatte.add(lab)
        intestazione = f'script.L_{lab:04x} = function(u, p)' if lab in tr.parametri else f'script.L_{lab:04x} = function(u)'
        corpo += [intestazione] + tr.funzione(lab) + ['end', '']
    L += corpo + ['return missione', '']
    testo = '\n'.join(L).replace('@@CORREZIONI@@', '-- punti spostati nell\'acqua piu\' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m\n  '
                                   + posa.correzioni() + ',')
    return testo, tr.usate, mancanti

def verifica_fondale(root):
    """Unita' navali (navi e sottomarini): in acqua, fuori dalla heightmap, sottomarini con fondale sufficiente."""
    import numpy as np
    from PIL import Image
    cache = {}; out = []
    for f in sorted(glob.glob(f'{root}/data/missioni/SCENE*.lua')):
        t = open(f).read(); teatro = re.search(r'teatro = "(\w+)"', t).group(1); metodo = re.search(r'metodo = "(\w+)"', t).group(1)
        if teatro not in cache:
            h = json.load(open(f'{root}/data/teatri/{teatro}/heightmap.json'))
            z = np.array(Image.open(f'{root}/data/teatri/{teatro}/heightmap_u16.png')).astype(float) * h['codifica']['scala'] + h['codifica']['offset']
            cache[teatro] = (h['griglia'], z)
        g, z = cache[teatro]; c = g['cella_m']; n = acqua = fuori = sub = sub_ok = 0
        for cat, resto, x, y, ox, oy, q in re.findall(r'categoria = "(\w+)"([^\n]*)\n\s+posizione = \{ x = (-?[\d.]+), y = (-?[\d.]+) \}, originale = \{ x = (-?\d+), y = (-?\d+) \},[^\n]*\n\s+quota_m = (-?[\d.]+)', t):
            fb = 'falso_bersaglio' in resto
            if cat not in ('nave', 'sottomarino') or fb or (int(ox), int(oy)) == (0, 6400): continue   # parcheggio, falsi bersagli
            col = int((float(x) + g['larghezza'] * c / 2) // c); row = int((g['altezza'] * c / 2 - float(y)) // c)
            if not (0 <= row < z.shape[0] and 0 <= col < z.shape[1]): fuori += 1; continue
            n += 1; acqua += z[row, col] < 0
            if cat == 'sottomarino': sub += 1; sub_ok += -z[row, col] >= float(q) + 10
        out.append((os.path.basename(f), teatro, metodo, acqua, n, fuori, sub_ok, sub))
    return out

# funzioni m688 dell'Enhanced, che le originali non usano: in coda ad API.md (docs/09_schema_missioni.md)
ENHANCED_API = ['', '## Funzioni dell\'Enhanced', '',
    'Nel livello classico non fanno niente. Le chiavi della missione sono in `docs/09_schema_missioni.md` §5.', '',
    '| Funzione | Descrizione |', '|---|---|',
    "| `m688.cue_asw(u, aereo, x, y [, raggio_m [, ritardo_s [, rotta, velocita_kn]]])` | ASW aereo (§5.4): un datum per "
    "l'aereo `aereo` (-1: tutti gli aerei controllati), col fix vecchio di `ritardo_s` secondi (predefiniti 1852 m e 0 s) |"]

if __name__ == '__main__':
    root = sys.argv[1]
    # --uscita CARTELLA: scrive altrove (per confrontare con data/missioni senza toccarle)
    uscita = sys.argv[sys.argv.index('--uscita') + 1] if '--uscita' in sys.argv else None
    dst = uscita or f'{root}/data/missioni'; os.makedirs(dst, exist_ok=True)
    solo = [a for a in sys.argv[2:] if not a.startswith('--') and a != uscita]   # scene da rigenerare (vuoto = tutte)
    if '--verifica' in sys.argv:
        for r in verifica_fondale(root): print('%-12s %-10s %-12s unita navali in acqua %d/%d, fuori dalla carta %d; sottomarini con fondale >= quota + 10 m %d/%d' % r)
        sys.exit(subprocess.call([f'{root}/tools/lua/src/lua', f'{root}/tools/re/verifica_missioni.lua'] + sorted(glob.glob(f'{dst}/SCENE*.lua'))))
    _classi = tomllib.load(open(f'{root}/data/motore/classi.toml', 'rb')); classi_note = set(_classi)
    PESCAGGI.update({k: v['pescaggio_m'] for k, v in _classi.items() if isinstance(v, dict) and 'pescaggio_m' in v})
    usate = collections.Counter(); manc = collections.Counter()
    for f in sorted(glob.glob(f'{root}/extracted/raw/DISK1/SCENE*.TOK')):
        if solo and os.path.basename(f)[:7] not in solo: continue
        testo, u, m = converti(root, f, classi_note); usate.update(u); manc.update(m)
        sc = os.path.basename(f)[:7]; open(f'{dst}/{sc}.lua', 'w').write(testo)
        print(sc, len(testo.splitlines()), 'righe; classi mancanti:', ', '.join(m))
    api = ['# API m688 usata dalle missioni convertite', '', 'Generato da `tools/re/missioni_lua.py`. Livello classico: ogni funzione',
           "corrisponde a un'operazione della VM originale (`docs/04_vm_missioni.md` §3). Il primo argomento è sempre `u`, l'unità",
           'che esegue lo script. Le condizioni restituiscono vero o falso.', '', '| Funzione | Opcode | Descrizione originale | Usi |', '|---|---|---|---|']
    nomi = {n: (op, dsc) for op, (n, a, dsc) in OPS.items()}
    for (f, n), k in sorted(usate.items()):
        api.append(f'| `m688.{f}` | `{n}` ({nomi[n][0]:02X}h) | {nomi[n][1]} | {k} |')
    api += ENHANCED_API
    if not solo: open(f'{dst}/API.md', 'w').write('\n'.join(api) + '\n')   # i conteggi valgono solo su tutte le scene
    print('funzioni API', len(usate), ' classi mancanti in totale:', dict(manc))
