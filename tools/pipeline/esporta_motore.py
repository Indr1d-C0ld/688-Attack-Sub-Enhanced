"""Esporta i dati per il motore C++ in data/motore/ (formati binari semplici, documentati in engine/README.md).

  palette/giorno.pal, notte.pal   768 byte RGB a 8 bit (DAY.PAL e NIGHT.PAL dell'originale, con i colori degli effetti nei posti liberi)
  font/originale.fnt              font proporzionale di GAME.EXE (DS:9D5C) con le lettere accentate italiane
  navi/<classe>.spr               sprite a 16 grigi di una classe con le ancore (da extracted/navi/<classe>)
  classi.toml                     database delle classi navali (dimensioni, velocita', fonte, affidabilita')
  immagini/<NOME>.img, .pal       schermate originali 320x200 indicizzate con la loro palette (TITLE, MAIN)
  testi/messaggi_bordo.tsv        messaggi di bordo tradotti (codice decimale, TAB, testo italiano) dalla traduzione
  modelli/<classe>.mdl            modello 3D con l'opera viva e tessitura cotta (tools/navi/esporta_modello.py), per
                                  gli sprite generati al volo delle navi danneggiate
  scafi/<classe>.toml             sezioni e compartimentazione per la fisica dei danni (tools/navi/compartimenti.py)
  teatri/<teatro>.fdl             fondale del teatro (heightmap di data/teatri/<teatro>, stessa griglia e codifica)
  teatri/<teatro>_carta_<parte>.fdl  carta d'epoca della parte (nato, urss): tools/pipeline/carta_epoca.py
  basi/<base>.fdl                 la carta della zona di una base, per il Museo: tools/pipeline/carte_basi.py
  siluri.toml                     siluri ed esche dell'Enhanced (tools/pipeline/siluri.py)
  missili.toml                    classi di missile della stazione di tiro (tools/pipeline/missili.py)
  artiglieria.toml                artiglieria navale contro il sottomarino (tools/pipeline/artiglieria.py)
  acustica.toml                   firme di rumore per classe e sensori (tools/pipeline/acustica.py)
  carico.toml                     armi, locali siluri e carichi della Sala siluri (tools/pipeline/carico.py)
  carriera.toml                   gradi, decorazioni, soglie e motivazioni della carriera (tools/pipeline/carriera.py)
  achievement.toml                achievement; albo_onore_storico.toml i comandanti storici (tools/pipeline/achievement.py)
  libretto.toml                   tavole del libretto di riconoscimento al periscopio (tools/pipeline/libretto.py)
  luci.toml                       luci di navigazione e di fonda per classe, COLREG 1972 (tools/pipeline/luci.py)
  fari.toml                       i fari dei teatri nel 1989, da Wikidata (tools/pipeline/fari.py)
  navigazione.toml                sistemi di navigazione, punti nave, teatri (tools/pipeline/navigazione.py)
  interazioni.toml                interazioni fra IA: manovre di TMA, carico e filo dell'IA (tools/pipeline/interazioni.py)
  teatri/<teatro>.suono.toml      profili mensili della velocita' del suono (tools/pipeline/suono.py)
  suoni_classici.toml, suoni_classici/<nome>.t  campioni .T del classico, copiati dal disco 2 (tools/pipeline/suoni_classici.py)

Formato .fnt: "FNT1", u16 numero di glifi, u8 righe per glifo (15); per glifo: u32 codepoint, u8 larghezza,
righe byte (bit 7 = colonna 0). Le righe 0-1 sono riservate agli accenti: il glifo originale parte dalla riga 2.
Codici 0x20-0x7F come ASCII (lo slot delle minuscole contiene maiuscole grandi 7x7, quello delle maiuscole
maiuscole piccole 5x5); 0x80-0xFF dell'originale (icone, cifre grandi) in U+E080-U+E0FF.

Formato .spr: "SPR2", u16 numero di sprite, f32 scala in pixel per metro; per sprite: u16 aob (gradi),
u8 lato (0 dritta, 1 sinistra), i8 elevazione (gradi: 0 per le navi; per gli oggetti in volo l'angolo sotto cui
l'osservatore li vede sopra l'orizzonte), u16 larghezza, u16 altezza, f32 ancora x, f32 ancora y (centro nave al
galleggiamento, o centro del modello per gli oggetti in volo, in pixel dello sprite), u32 byte compressi, poi
larghezza*altezza indici di palette (159 = trasparente) compressi con DEFLATE grezzo (RFC 1951, senza
intestazione zlib; un blocco per sprite, 10 volte piu' piccoli degli indici in chiaro di SPR1).

Formato .img: "IMG1", u16 larghezza, u16 altezza, larghezza*altezza indici di palette.

Formato .fdl: "FDL2", u16 larghezza, u16 altezza, f32 lato della cella in metri, f32 scala, f32 offset, u32 byte
compressi, poi larghezza*altezza valori u16 (riga 0 = nord, colonna 0 = ovest; quota z in metri = valore * scala +
offset, positiva verso l'alto) compressi senza perdite: scarto (mod 65536) dalla predizione a piano
sinistra + sopra - diagonale (0 fuori dalla griglia), byte bassi di tutti i valori e poi byte alti, DEFLATE grezzo
(RFC 1951, senza intestazione zlib). 32 -> 12 MB per i 7 teatri; FDL1 (valori in chiaro) non si legge piu'.
Il centro della griglia e' il centro della proiezione azimutale del teatro, l'origine delle posizioni delle
missioni (docs/09_schema_missioni.md §3); dopo i dati, il blocco GEO1 con latitudine e longitudine del centro (f64,
WGS84). Le carte d'epoca <teatro>_carta_<nato|urss>.fdl (tools/pipeline/carta_epoca.py) hanno lo stesso formato,
piu' i blocchi RIL1 e ANN1 (formato in carta_epoca.scrivi_fdl). Accanto ci sono i profili del suono <teatro>.suono.toml (tools/pipeline/
suono.py), che esporta_teatri non tocca.

Uso: esporta_motore.py <radice del progetto>
"""
import json, os, struct, sys, zlib
import numpy as np
from PIL import Image

RADICE = sys.argv[1] if len(sys.argv) > 1 else '.'
sys.path.insert(0, os.path.join(RADICE, 'tools', 're'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))   # moduli fratelli (carta_epoca, siluri, ...) anche con python -I
import cls_layout   # noqa: E402  font originale da GAME.EXE

USCITA = os.path.join(RADICE, 'data', 'motore')
RIGHE = 15          # 2 righe per gli accenti + 13 del glifo originale

# classe -> (nome mostrato, tipo nell'originale, L, B, velocita' max in nodi, affidabilita'[, fonte]);
# fonte predefinita: dossier 01
CLASSI = {
    'perry':      ('Oliver Hazard Perry (FFG-7)', 0x0d, 135.6, 13.7, 29, 'alta'),
    'kidd':       ('Kidd (DDG-993)', 0x0c, 171.6, 16.8, 33, 'alta'),
    # sostituisce il Forrest Sherman (tipo 0x00), radiato nel 1982-83 (docs/00_ROADMAP.md, requisito 28)
    'spruance':   ('Spruance (DD-963)', 0x00, 171.7, 16.8, 32.5, 'media'),   # valori da verificare: non nel dossier 01
    'iowa':       ('Iowa (BB-61)', 0x0b, 270.4, 33.0, 33, 'alta'),
    'nimitz':     ('Nimitz (CVN-68)', 0x0a, 332.8, 40.8, 30, 'media'),
    'sacramento': ('Sacramento (AOE-1)', 0x0e, 241.0, 33.0, 26, 'media'),
    'krivak2':    ('Krivak II (Progetto 1135M)', 0x07, 123.5, 14.1, 32, 'media'),
    'kara':       ('Kara (Progetto 1134B)', 0x05, 173.2, 18.6, 34, 'media'),
    'kirov':      ('Kirov (Progetto 1144)', 0x06, 252.0, 28.5, 32, 'bassa'),
    'slava':      ('Slava (Progetto 1164)', 0x09, 186.4, 20.8, 32, 'media'),
    'koni':       ('Koni (Progetto 1159)', 0x08, 95.0, 12.8, 27, 'media'),
    'udaloy':     ('Udaloy (Progetto 1155)', None, 163.5, 19.0, 30, 'media', 'docs/ricerca/31_firme_lotto1.md'),
    'grisha3':    ('Grisha III (Progetto 1124M)', None, 71.1, 10.15, 32, 'media', 'docs/ricerca/31_firme_lotto1.md'),
    # il secondo lotto (docs/ricerca/32_firme_lotto2.md, 29/10/2026); per ora la parente riscalata come il primo
    'ticonderoga': ('Ticonderoga (CG-47)', None, 172.5, 16.8, 32.5, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    'knox':       ('Knox (FF-1052)', None, 134.0, 14.25, 27, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    'type22':     ('Type 22 Broadsword (Batch 1)', None, 131.2, 14.8, 30, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    # la Batch 2 a parte (30/10/2026): lo scafo allungato per la schiera Type 2031Z, che la Batch 1 non ha
    'type22b2':   ('Type 22 Boxer (Batch 2)', None, 146.5, 14.8, 30, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    'nanuchka':   ('Nanuchka I (Progetto 1234)', None, 59.3, 12.6, 33, 'media'),
    # la nave appoggio delle eskadre (LIB-S15; modello proprio del kit, 30/10/2026)
    'ugra':       ('Ugra (Progetto 1886)', None, 141.4, 17.7, 17, 'media', 'extracted/navi/ugra/RAPPORTO.md'),
    # la motocannoniera norvegese della LIB-S16 (modello proprio del kit dal 30/10/2026, blocco D; prima la nanuchka
    # riscalata)
    'hauk':       ('Hauk (motocannoniera norvegese)', None, 36.5, 6.3, 34, 'media', 'extracted/navi/hauk/RAPPORTO.md'),
    # la fregata norvegese della LIB-S16 (modello proprio del kit, blocco C, 30/10/2026)
    'oslo':       ('Oslo (F300)', None, 96.6, 11.2, 25, 'media', 'extracted/navi/oslo/RAPPORTO.md'),
    # la portaerei convenzionale della LIB-S17 (variante della nimitz del kit, blocco H, 30/10/2026)
    'kitty_hawk': ('Kitty Hawk (CV-63)', None, 323.6, 39.6, 33, 'media', 'extracted/navi/kitty_hawk/RAPPORTO.md'),
    # la corvetta svedese della LIB-S18 (modello proprio del kit, blocco E, 30/10/2026)
    'stockholm':  ('Stockholm (K11)', None, 50.0, 7.5, 32, 'media', 'extracted/navi/stockholm/RAPPORTO.md'),
    # navi civili: una nave reale rappresentativa per tipo (dimensioni nel RAPPORTO.md della classe)
    'petroliera': ('Petroliera (Sea Isle City, 1981)', 0x10, 231.8, 44.0, 15, 'media',
                   'extracted/navi/petroliera/RAPPORTO.md'),
    'mercantile': ('Mercantile (Ravenstvo, tipo Leninskiy Komsomol)', 0x01, 169.9, 21.8, 18, 'bassa',
                   'extracted/navi/mercantile/RAPPORTO.md'),   # velocita' di servizio da verificare
    'cargo':      ('Portacontainer (Cpl. Louis J. Hauge Jr., ex Estelle Maersk)', 0x02, 230.25, 27.43, 16.4, 'media',
                   'extracted/navi/cargo/RAPPORTO.md'),
    # pescherecci: la nave spia sovietica (AGI) e il peschereccio d'altura civile (libreria missioni, docs/17 LIB-I02)
    'agi':        ('AGI classe Okean (LINZA)', None, 50.8, 8.8, 11.5, 'bassa', 'extracted/navi/agi/RAPPORTO.md'),
    'peschereccio': ("Peschereccio d'altura (Atlantic Peace, 1987)", None, 57.0, 13.0, 14, 'bassa',
                     'extracted/navi/peschereccio/RAPPORTO.md'),
    # missili e bersagli (oggetti in volo: sprite per elevazione); larghezza = apertura alare; velocita' di crociera
    'tomahawk':   ('BGM/UGM-109 Tomahawk', 0x23, 5.56, 2.67, 475, 'media', 'extracted/navi/tomahawk/RAPPORTO.md'),
    'harpoon':    ('RGM/UGM-84 Harpoon', None, 3.84, 0.91, 465, 'media', 'extracted/navi/harpoon/RAPPORTO.md'),
    'p700':       ('P-700 Granit (SS-N-19)', 0x28, 10.0, 2.6, 860, 'bassa', 'extracted/navi/p700/RAPPORTO.md'),
    'bqm74':      ('BQM-74C Chukar (bersaglio)', 0x0f, 3.87, 1.76, 500, 'media', 'extracted/navi/bqm74/RAPPORTO.md'),
    # aerei antisommergibile (oggetti in volo); larghezza = apertura alare; velocita' di crociera (stima)
    'p3c':        ('P-3C Orion', 0x18, 35.61, 30.38, 330, 'media', 'extracted/navi/p3c/RAPPORTO.md'),
    'il38':       ('Il-38 May', None, 39.6, 37.4, 330, 'media', 'extracted/navi/il38/RAPPORTO.md'),
    'tu142':      ('Tu-142MK Bear-F', None, 53.07, 50.0, 400, 'media', 'extracted/navi/tu142/RAPPORTO.md'),
    # sottomarini in superficie (sprite della parte emersa); velocita' massima in immersione
    'los_angeles': ('Los Angeles (SSN-688, Flight I)', 0x14, 110.3, 10.06, 32, 'media', 'extracted/navi/los_angeles/RAPPORTO.md'),
    'victor3':    ('Victor III (Progetto 671RTM)', 0x17, 106.0, 10.6, 30, 'media', 'extracted/navi/victor3/RAPPORTO.md'),
    'kilo':       ('Kilo (Progetto 877)', 0x20, 72.6, 9.9, 17, 'media', 'extracted/navi/kilo/RAPPORTO.md'),
    'alfa':       ('Alfa (Progetto 705)', 0x15, 81.4, 9.5, 41, 'media', 'extracted/navi/alfa/RAPPORTO.md'),
    'akula':      ('Akula I (Progetto 971)', 0x1d, 110.3, 13.6, 33, 'media', 'extracted/navi/akula/RAPPORTO.md'),
    'oscar':      ('Oscar II (Progetto 949A)', 0x1c, 154.0, 18.2, 32, 'media', 'extracted/navi/oscar2/RAPPORTO.md'),
    'charlie2':   ('Charlie II (Progetto 670M)', 0x1e, 104.5, 9.9, 24, 'bassa', 'extracted/navi/charlie2/RAPPORTO.md'),
    'typhoon':    ('Typhoon (Progetto 941)', 0x1f, 175.0, 23.0, 27, 'media', 'extracted/navi/typhoon/RAPPORTO.md'),
    'foxtrot':    ('Foxtrot (Progetto 641)', 0x21, 91.3, 7.5, 16, 'media', 'extracted/navi/foxtrot/RAPPORTO.md'),
    'sturgeon':   ('Sturgeon (SSN-637)', 0x1b, 89.1, 9.65, 26, 'media', 'extracted/navi/sturgeon/RAPPORTO.md'),
    'trafalgar':  ('Trafalgar (Royal Navy)', 0x22, 85.4, 9.8, 32, 'media', 'extracted/navi/trafalgar/RAPPORTO.md'),
    # il primo lotto delle firme nuove (docs/ricerca/31_firme_lotto1.md, del disassemblatore, 29/10/2026): assenti nel
    # gioco del 1989; per ora la parente riscalata (vedi SEGNAPOSTO)
    'victor1':    ('Victor I (Progetto 671)', None, 93.0, 10.6, 32, 'media', 'docs/ricerca/31_firme_lotto1.md'),
    'delta3':     ('Delta III (Progetto 667BDR)', None, 156.0, 11.7, 24, 'media', 'docs/ricerca/31_firme_lotto1.md'),
    'sierra':     ('Sierra I (Progetto 945)', None, 107.2, 12.3, 35, 'bassa', 'docs/ricerca/31_firme_lotto1.md'),
    'ohio':       ('Ohio (SSBN-726)', None, 170.7, 12.8, 25, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    'type206':    ('Type 206 (U 13-30)', None, 48.5, 4.6, 17, 'media', 'docs/ricerca/32_firme_lotto2.md'),
    # il sottomarino svedese della LIB-S18 (modello proprio del kit, blocco G, 30/10/2026); senza dieresi: i font del
    # gioco hanno solo le accentate italiane
    'nacken':     ('Nacken (A14)', None, 49.5, 5.7, 20, 'media', 'extracted/navi/nacken/RAPPORTO.md'),
    # elicotteri (oggetti in volo); larghezza = diametro del rotore; velocita' di crociera (stima)
    'sh60b':      ('SH-60B Seahawk (LAMPS III)', 0x12, 19.76, 16.36, 125, 'media', 'extracted/navi/sh60b/RAPPORTO.md'),
    'ka25':       ('Ka-25PL Hormone-A', 0x11, 9.75, 15.74, 105, 'media', 'extracted/navi/ka25/RAPPORTO.md'),
    # ASW aereo dell'Enhanced (docs/13_asw_aereo.md, A0; dossier 17 §2-3): assenti nel gioco del 1989. Larghezza =
    # apertura alare o diametro del rotore; velocita' di crociera (stima). Sprite ricostruiti dal kit (pezzo A4).
    's3a':        ('S-3A Viking', None, 16.26, 20.93, 350, 'media', 'extracted/navi/s3a/RAPPORTO.md'),
    's3b':        ('S-3B Viking', None, 16.26, 20.93, 350, 'media', 'extracted/navi/s3b/RAPPORTO.md'),
    'sh3h':       ('SH-3H Sea King', None, 16.69, 18.9, 114, 'media', 'extracted/navi/sh3h/RAPPORTO.md'),
    'sh60f':      ('SH-60F Ocean Hawk', None, 19.76, 16.36, 125, 'media', 'extracted/navi/sh60f/RAPPORTO.md'),   # dal 1990
    'ka27':       ('Ka-27PL Helix-A', None, 11.3, 15.9, 120, 'media', 'extracted/navi/ka27/RAPPORTO.md'),
    'be12':       ('Be-12 Mail', None, 30.17, 29.84, 245, 'media', 'extracted/navi/be12/RAPPORTO.md'),
    # l'elicottero imbarcato delle Knox (runtime, 30/10/2026; dossier 17 §2.1b); sprite propri del kit dal 30/10/2026
    'sh2f':       ('SH-2F Seasprite (LAMPS I)', None, 16.0, 13.41, 120, 'media', 'extracted/navi/sh2f/RAPPORTO.md'),
    # l'elicottero antisom svedese della LIB-S18 (kit, blocco F, 30/10/2026): fusoliera 13,66 m, rotori in tandem di 15,24
    'hkp4':       ('Hkp 4 (Vertol 107)', None, 13.66, 15.24, 137, 'media', 'extracted/navi/hkp4/RAPPORTO.md'),
    # l'elicottero imbarcato delle Type 22 (kit, 31/10/2026; dossier 38 §2): fusoliera 13,2 m (15,24 coi rotori), rotore di 12,8 m a 4 pale;
    # crociera 125 nodi STIMA (i dati della HAS.3 non si trovano)
    'lynx':       ('Lynx HAS.3', None, 13.2, 12.8, 125, 'media', 'extracted/navi/lynx/RAPPORTO.md'),
}

# classi senza sprite propri -> classe di cui usano lo sprite (segnaposto; il kit li sostituisce nel pezzo A4 di
# docs/13_asw_aereo.md, poi la voce si toglie da qui)
SEGNAPOSTO = {}   # nessuna, dal 30/10/2026 (l'sh2f ha i suoi sprite)
# il primo lotto delle firme nuove (29/10/2026): modello, sprite e scafo della parente riscalati alle misure vere in
# extracted/navi/<classe> (tools/navi/riscala_classe.py, passo 2; i modelli propri sono il passo 3): victor1 dal
# victor3, delta3 dall'oscar, sierra dall'akula, udaloy dal kara, grisha3 dal koni; il secondo lotto (dossier 32): ohio
# dal los_angeles, ticonderoga dalla spruance, knox, type22 e type22b2 dalla perry, type206 dal kilo. Hanno ora il
# modello proprio (passo 3): i sette dei lotti 1 e 2 e, dal 30/10/2026, type22, type22b2, knox e type206

# Colori degli effetti nei posti liberi della palette (segnaposto magenta 255,0,202 nell'originale), docs/10 §5.7:
# brace e cuore bianco-giallo della rampa del fuoco (con 175-178), fumo illuminato dal fuoco, fango e fuliggine.
COLORI_EFFETTI = {
    190: (120, 24, 0),     # brace
    192: (255, 250, 190),  # cuore della fiamma
    193: (78, 40, 22),     # fumo illuminato dal basso, scuro
    194: (150, 72, 30),    # fumo illuminato dal basso, chiaro
    196: (96, 80, 54),     # fango
    197: (146, 124, 84),   # fango chiaro
    198: (62, 52, 44),     # fuliggine
}

def esporta_palette():
    os.makedirs(f'{USCITA}/palette', exist_ok=True)
    for src, dst in (('DAY.PAL', 'giorno.pal'), ('NIGHT.PAL', 'notte.pal')):
        d = bytearray(open(f'{RADICE}/extracted/raw/DISK1/{src}', 'rb').read())
        assert len(d) == 768
        for i, rgb in COLORI_EFFETTI.items():
            assert tuple(d[3 * i:3 * i + 3]) == (255, 0, 202), f'{src}: il posto {i} non e\' libero'
            d[3 * i:3 * i + 3] = bytes(rgb)
        open(f'{USCITA}/palette/{dst}', 'wb').write(bytes(d))

def _accentato(glifo, accento):
    """Copia del glifo (larghezza, righe da 15) con un accento grave o acuto di due pixel nelle righe 0-1."""
    w, righe = glifo
    r = list(righe); c = (w - 1) // 2
    if accento == 'grave': r[0] |= 0x80 >> max(c - 1, 0); r[1] |= 0x80 >> c
    else:                  r[0] |= 0x80 >> min(c + 1, 7); r[1] |= 0x80 >> c
    return w, r

def esporta_font():
    img = cls_layout.carica_exe(RADICE)
    f = cls_layout.font(img)
    glifi = {}
    for c in range(32, 256):
        w, righe = f[chr(c)]
        cp = c if c < 128 else 0xE000 + c
        glifi[cp] = (w, [0, 0] + list(righe))
    # lettere accentate: minuscole -> maiuscole grandi con accento, maiuscole -> maiuscole piccole con accento
    for base, gr, ac in (('a', 'à', 'á'), ('e', 'è', 'é'), ('i', 'ì', 'í'), ('o', 'ò', 'ó'), ('u', 'ù', 'ú'),
                         ('A', 'À', 'Á'), ('E', 'È', 'É'), ('I', 'Ì', 'Í'), ('O', 'Ò', 'Ó'), ('U', 'Ù', 'Ú')):
        glifi[ord(gr)] = _accentato(glifi[ord(base)], 'grave')
        if ac: glifi[ord(ac)] = _accentato(glifi[ord(base)], 'acuto')
    glifi[ord('’')] = glifi[ord("'")]
    # punto e virgola: nell'originale la virgola sta 4-6 righe sotto il punto e, col set piccolo che il motore abbassa di 2,
    # scende sulla riga sotto. Rifatto alto come i due punti: il punto in alto, la virgola sulla base (come ',')
    glifi[ord(';')] = (2, [0, 0, 0b11000000, 0b11000000, 0, 0b01000000, 0b10000000] + [0] * (RIGHE - 7))
    # simboli che l'originale non ha: grado (cerchietto 3x3 in alto) e segno di moltiplicazione
    glifi[ord('°')] = (3, [0, 0, 0b01000000, 0b10100000, 0b01000000] + [0] * (RIGHE - 5))
    glifi[ord('×')] = (5, [0, 0, 0, 0b10001000, 0b01010000, 0b00100000, 0b01010000, 0b10001000] + [0] * (RIGHE - 8))
    # paragrafo (le fonti dei dossier nei testi del Museo), alto come le maiuscole grandi
    glifi[ord('§')] = (4, [0, 0, 0b01100000, 0b10000000, 0b01100000, 0b10010000, 0b01100000, 0b00010000, 0b01100000]
                       + [0] * (RIGHE - 9))
    os.makedirs(f'{USCITA}/font', exist_ok=True)
    with open(f'{USCITA}/font/originale.fnt', 'wb') as o:
        o.write(b'FNT1' + struct.pack('<HB', len(glifi), RIGHE))
        for cp in sorted(glifi):
            w, righe = glifi[cp]
            o.write(struct.pack('<IB', cp, w) + bytes(righe))
    return len(glifi)

# chiave della classe (come nelle missioni) -> cartella in extracted/navi, quando sono diverse
CARTELLE = {'oscar': 'oscar2'}

def esporta_sprite(classe):
    base = f'{RADICE}/extracted/navi/{CARTELLE.get(classe, classe)}/sprite'
    anc = json.load(open(f'{base}/ancore.json'))
    voci = []
    for nome, (ax, ay) in sorted(anc['ancore'].items()):
        parti = nome.split('_')            # aob_XXX_lato[_eNN]
        aob, lato = parti[1], parti[2]
        el = int(parti[3][1:]) if len(parti) > 3 else 0
        k = np.asarray(Image.open(f'{base}/{nome}.png'))
        assert k.dtype == np.uint8 and k.ndim == 2, nome
        voci.append((int(aob), 0 if lato == 'dritta' else 1, el, k, ax, ay))
    os.makedirs(f'{USCITA}/navi', exist_ok=True)
    with open(f'{USCITA}/navi/{classe}.spr', 'wb') as o:
        o.write(b'SPR2' + struct.pack('<Hf', len(voci), anc['scala_px_m']))
        for aob, lato, el, k, ax, ay in voci:
            h, w = k.shape
            z = zlib.compressobj(9, zlib.DEFLATED, -15, 9)
            b = z.compress(k.tobytes()) + z.flush()
            o.write(struct.pack('<HBbHHffI', aob, lato, el, w, h, ax, ay, len(b)) + b)
    return len(voci)

SCHERMATE = ('TITLE', 'MAIN')

def esporta_schermate():
    os.makedirs(f'{USCITA}/immagini', exist_ok=True)
    for nome in SCHERMATE:
        im = Image.open(f'{RADICE}/extracted/png/{nome}.png')
        assert im.mode == 'P' and im.size == (320, 200), (nome, im.mode, im.size)
        pal = (im.getpalette() + [0] * 768)[:768]
        k = np.asarray(im, np.uint8)
        open(f'{USCITA}/immagini/{nome}.img', 'wb').write(b'IMG1' + struct.pack('<HH', 320, 200) + k.tobytes())
        open(f'{USCITA}/immagini/{nome}.pal', 'wb').write(bytes(pal))

# Messaggi del motore che non sono nella tabella di bordo: il 67 dell'originale (fuoco amico, testo nell'EXE) e quelli
# della versione Enhanced (codici 200+, docs/10_danni_affondamento_esplosioni.md §7: cio' che il sonar e la radio
# riferiscono di una nave colpita)
MESSAGGI_EXE = (0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43)   # danni ai sistemi (§9.4) e fuoco amico
MESSAGGI_MOTORE = {
    200: 'Rumori di rottura dal bersaglio, {COMANDANTE}.',
    201: 'Esplosione secondaria a bordo del bersaglio!',
    202: 'Il bersaglio si è fermato: eliche ferme, {COMANDANTE}.',
    203: 'Il bersaglio sbanda e imbarca acqua.',
    204: 'Il bersaglio è affondato, {COMANDANTE}.',
    205: 'Intercettata una richiesta di soccorso: abbandonano la nave.',
    206: 'Unità nemiche dirigono sul punto dell\'attacco.',
    207: 'Intercettato dalla radio del bersaglio: incendio nel deposito.',
    208: 'Intercettato dalla radio del bersaglio: allagano il deposito.',
    # rifiuti dell'equipaggio (docs/11_comandi_e_stazioni.md §3: limiti del manuale del 1989)
    210: 'Troppo vicini alla superficie per aumentare, {COMANDANTE}: sopra i 100 piedi al massimo 2/3.',
    211: 'Nessun punto di rotta impostato per il pilota automatico, {COMANDANTE}.',
    212: 'Periscopio e antenna si alzano solo a quota periscopica, {COMANDANTE}.',
    # domanda del telegrafo con l'antenna rimorchiata fuori (sensori_e_navigazione.md §10.4)
    213: 'Con l\'antenna rimorchiata fuori va tagliata: Invio per tagliarla, Esc per annullare.',
    # rifiuti per i sistemi danneggiati nell'Enhanced (BattelloDanni, docs/10 §10)
    214: 'Il timone non risponde, {COMANDANTE}.',
    215: 'I timoni orizzontali non rispondono, {COMANDANTE}.',
    # posarsi sul fondo (Enhanced, docs/11 §6)
    216: 'Ci posiamo sul fondo, {COMANDANTE}: macchine ferme, discesa lenta.',
    217: 'Non possiamo posarci con l\'antenna rimorchiata filata, {COMANDANTE}.',
    218: 'Il fondale qui è troppo ripido per posarci, {COMANDANTE}.',
    219: 'Il fondale qui è oltre la quota di collaudo, {COMANDANTE}.',
    # sottomarini colpiti (docs/10 §10, fase D6c: missioni/danni_sottomarini.hpp): rapporti dell'equipaggio del
    # giocatore (270-279) e cio' che il sonar sente di un sottomarino nemico (280-281)
    270: 'Allagamento a bordo, {COMANDANTE}!',
    271: 'Avarie ai sistemi dopo l\'urto: rapporto alla stazione Stato.',
    272: 'SCRAM del reattore: siamo sul motore d\'emergenza, {COMANDANTE}.',
    273: 'Reattore di nuovo critico: propulsione ripristinata.',
    274: 'Riparazione completata, {COMANDANTE}.',
    275: 'Falla tamponata.',
    276: 'Ha ceduto una paratia: l\'acqua passa nel compartimento vicino!',
    277: 'Esaurimento d\'emergenza delle casse!',
    278: 'Aria delle bombole esaurita, {COMANDANTE}.',
    279: 'Rottura dello scafo resistente!',
    280: 'Rumori di rottura da un sottomarino, {COMANDANTE}.',
    281: 'Implosioni in profondità: il sottomarino è distrutto.',
    282: 'Siamo posati sul fondo e pesanti, {COMANDANTE}: senza esaurire le casse non risaliamo.',
    283: 'Un sottomarino nemico esaurisce le casse: sta emergendo in emergenza!',
    284: 'Incendio a bordo, {COMANDANTE}! Squadre antincendio con le maschere.',
    285: 'Esplosione a bordo!',
    286: 'Incendio spento, {COMANDANTE}.',
    287: 'Cloro dalle batterie allagate: maschere!',
    288: 'Una tubolatura dell\'aria ad alta pressione alimenta l\'incendio!',
    289: 'Esplosione a bordo di un sottomarino nemico, {COMANDANTE}.',
}
# Frasi della stazione Armi dell'originale (testi dell'EXE, docs/originale/sensori_e_navigazione.md §10.1-§10.3) con
# il codice che usa il motore (nucleo/armi_giocatore.hpp)
MESSAGGI_EXE_DS = {
    220: 'DS:081A', 221: 'DS:0835', 222: 'DS:0867', 223: 'DS:087D', 224: 'DS:0893',   # stato e selezione dei tubi
    225: 'DS:1BA4', 226: 'DS:1CA0', 227: 'DS:1BC3', 228: 'DS:1BE3', 229: 'DS:1C2E',   # rifiuti del lancio
    230: 'DS:1C08', 231: 'DS:1C4F', 232: 'DS:1C73',
    233: 'DS:1CD2', 234: 'DS:1CB9', 235: 'DS:1CEA', 236: 'DS:1D1C',                   # guida a filo
    237: 'DS:1DB6', 238: 'DS:1DC6', 239: 'DS:1DD9',                                    # TARGET e SELEZ.
    # stazione Sonar (§10.4-§10.5): ping, schiera, filtro, ANALYZE
    240: 'DS:140C', 241: 'DS:1437', 242: 'DS:1453', 243: 'DS:1470', 244: 'DS:148F', 245: 'DS:14B1',
    246: 'DS:14D0', 247: 'DS:1544', 248: 'DS:1525', 249: 'DS:1B06', 250: 'DS:14EF', 251: 'DS:150F',
    # carta e scelta dei segni: zoom, punti di rotta, TARGET
    252: 'DS:126E', 253: 'DS:128E', 254: 'DS:15DB', 255: 'DS:163C', 256: 'DS:1655', 257: 'DS:1D04',
    258: 'DS:1D59', 259: 'DS:1D34', 260: 'DS:1DA8', 261: 'DS:1D6E', 262: 'DS:1627',
}
# Rapporti sui siluri dell'Enhanced (C6b-C6c), dagli eventi del modello: testa attivata, aggancio, contatto perso,
# spegnimento (il filo tranciato, la fine corsa e il filo quasi finito riusano 23, 24 e 39 dell'originale)
MESSAGGI_MOTORE.update({
    263: 'Testa del siluro attiva, {COMANDANTE}.',
    264: 'Il siluro ha agganciato un bersaglio, {COMANDANTE}.',
    265: 'Il siluro ha perso il contatto, {COMANDANTE}.',
    266: 'Siluro spento via filo: affonda senza esplodere.',
    267: 'Stadimetro: distanza misurata, {COMANDANTE}.',
    268: 'Stadimetro: serve il bersaglio di superficie nel periscopio alzato, {COMANDANTE}.',
    269: 'Radio: per trasmettere serve l\'antenna alzata a quota periscopica, {COMANDANTE}.',
    # fascia 370-399 della sessione principale (rapporti dei tracker del sonar, S5a)
    370: 'Sonar: nuovo tracker assegnato, {COMANDANTE}.',
    371: 'Sonar: tracker perso.',
    372: 'Sonar: possibile sottomarino, {COMANDANTE}.',
    373: 'Sonar: probabile sottomarino, {COMANDANTE}!',
    374: 'Sonar: sottomarino certo, {COMANDANTE}!',
    375: 'Sonar: il contatto non è un sottomarino.',
    376: 'Sonar: tutti i tracker sono occupati.',
    377: 'Radio: nessuna boa SLOT da lanciare con questo ritardo, {COMANDANTE}.',
    378: 'Navigazione: il fondo sale sulla rotta, oltre il margine di sicurezza, {COMANDANTE}!',
    379: 'Navigazione: la rotta entra in una zona da evitare, {COMANDANTE}.',
})
# Sonar dell'Enhanced (acustica nella missione, Missione::messaggi_sonar): avvisi al giocatore (sessione sulla cadenza)
MESSAGGI_MOTORE.update({
    310: 'Sonar: nuovo contatto, {COMANDANTE}.',
    311: 'Sonar: siluro in acqua!',
    312: 'Ci stanno pingando, {COMANDANTE}.',
    313: 'Stiamo cavitando, {COMANDANTE}.',
    314: 'Schiera cieca in virata.',
})
# Sala radio dell'Enhanced (Missione::messaggi_radio, pezzo S4): avvisi al giocatore (sessione sul runtime)
MESSAGGI_MOTORE.update({
    315: 'Radio: messaggio in arrivo.',
    316: 'Radio: ricevuto un messaggio FLASH, {COMANDANTE}!',
    317: 'Radio: ricevuto un messaggio IMMEDIATE, {COMANDANTE}.',
    318: 'Radio: messaggio ricevuto.',
    319: 'Radio: manca un messaggio nella serie del broadcast.',
    320: 'Chiamata ELF: il comando chiede di salire a quota di comunicazione.',
    321: 'Il filo galleggiante si è spezzato, {COMANDANTE}.',
    322: 'Filo galleggiante senza segnale.',
    323: 'Satellite non disponibile.',
    324: 'Trasmissione completata.',
    325: 'Traffico del satellite ricevuto.',
})
# Radio R1 ed ESM ripensato dell'Enhanced (Missione::messaggi_radio e messaggi_esm; sessione sul runtime)
MESSAGGI_MOTORE.update({
    326: 'Radio: gli ordini in vigore sono cambiati, {COMANDANTE}.',
    327: 'Radio: contatto d\'intelligence riportato sulla carta.',
    331: 'Radio: supporto in arrivo.',
    332: 'La boa SLOT ha trasmesso.',
    333: 'Comandante, quel lancio viola le regole d\'ingaggio.',
    334: 'ESM: nuovo emettitore radar.',
    335: 'ESM: radar di un aereo antisommergibile!',
    336: 'ESM: rischio di scoperta alto, {COMANDANTE}.',
    337: 'ESM: siamo illuminati da un radar di tiro!',
    338: 'ESM: emettitore perso.',
    # radio R2: zone, scadenze, sessioni, autenticazione
    328: 'Siamo entrati in una zona protetta, {COMANDANTE}.',
    329: 'Radio: il compito è scaduto.',
    330: 'Radio: sessione mancata, il comando chiede notizie.',
    339: 'Radio: il gruppo d\'autenticazione non torna. Messaggio falso.',
})

# stazione sonar completa (S3, fascia 340-369 del disassemblatore). 342 e 343 li emette la stazione dopo ANALIZZA
# (classe e confidenza nel pannello del Sonar); 340, 341 e 344-347 sono eventi della simulazione (sessione sul runtime)
MESSAGGI_MOTORE.update({
    340: 'Sonar: contatto classificato, sottomarino.',
    341: 'Sonar: contatto classificato, nave di superficie.',
    342: 'Analizzatore: classificazione pronta, {COMANDANTE}.',
    343: 'Analizzatore: classe non identificata.',
    344: 'Sonar: il contatto sta cavitando.',
    345: 'Sonar: transitorio dal contatto.',
    346: 'Sonar: contatto biologico.',
    347: 'Sonar: nuova riga al LOFAR.',
    # ricarica realistica dell'Enhanced (dossier 14 §5.4; nucleo/armi_giocatore.hpp)
    348: 'Siluristi: ricarica iniziata.',
    349: 'Siluristi: due ricariche in corso, il tubo è in coda.',
    350: 'Siluristi: ricarica annullata, arma in rastrelliera.',
    351: 'Silenzio a bordo: ricariche rallentate, {COMANDANTE}.',
    352: 'Fine del silenzio a bordo.',
    **{353 + i: f'Siluristi: tubo {i + 1} ricaricato.' for i in range(7)},
    360: 'Siluristi: tubo disarmato, l\'arma resta nel tubo.',
    # carico della Sala siluri (M2): cambio d'arma nel tubo, mine
    361: 'Siluristi: cambio d\'arma, ricarica iniziata.',
    362: 'Siluristi: arma scelta per il tubo.',
    363: 'Siluristi: tubo armato, prima va disarmato.',
    364: 'Mina: posa non possibile, {COMANDANTE}.',
    365: 'Siluristi: arma non in rastrelliera.',
    366: 'Mina armata.',
    # posa delle mine (M2b): li emette la missione
    367: 'Mina posata.',
    368: 'Mina posata su un fondale inadatto: resta inerte.',
    369: 'Esplosione di una mina.',
    # ASW aereo (docs/13, A5; codici fissati dal runtime in missioni/asw_missione.hpp); {RIL} = rilevamento
    380: 'Comandante, aereo antisommergibile in zona.',
    381: 'Sonar: tonfo in superficie, rilevamento {RIL}.',
    382: 'Sonar: aereo in sorvolo, {COMANDANTE}.',
    383: 'Sonar: sonar a immersione, rilevamento {RIL}, {COMANDANTE}!',
    384: 'Periscopio: fumogeno in superficie, {COMANDANTE}.',
    385: 'Sonar: segnalatore SUS, attacco simulato.',
    386: 'Sonar: ping di boa attiva, rilevamento {RIL}.',
    387: 'Sonar: siluro aviolanciato in acqua, rilevamento {RIL}!',
    388: 'ESM: radar d\'aereo in modo periscopio, {COMANDANTE}!',
    389: 'Radio: l\'aereo amico ci passa un contatto.',
    # misure del sonarista e ascolto (S5d, principale: fascia 390-399)
    390: 'Sonar: riga agganciata al verniero.',
    391: 'Sonar: verniero sganciato.',
    392: 'Sonar: prima il cursore sulla riga (9), {COMANDANTE}.',
    393: 'Sonar: ascolto registrato.',
    394: 'Sonar: ascolto non registrato.',
    395: 'Stadimetro: allineare la testa d\'albero fantasma al galleggiamento, poi H.',
    # la schiera trainata dei sottomarini (docs/20 §7, D; runtime, missione_schiera.cpp)
    396: 'Schiera a contatto col fondo, {COMANDANTE}.',
    397: 'La schiera è danneggiata: ascolta male.',
    398: 'Abbiamo perso la schiera trainata.',
    399: 'Una rete da pesca ha agganciato la schiera!',
    # periscopi dell'Enhanced (docs/14, P2; codici del runtime in missioni/periscopi_missione.hpp)
    400: 'Periscopio scelto.',
    401: 'Periscopio su.',
    402: 'Periscopio: troppa velocità, l\'immagine trema, {COMANDANTE}.',
    403: 'Periscopio danneggiato dalla velocità!',
    404: 'Periscopio in avaria: non sale, {COMANDANTE}.',
    405: 'Comandante, siamo esposti da trenta secondi.',
    406: 'Periscopio nemico in vista!',
    407: 'Foto scattata.',
    408: 'Comandante, la vela affiora!',
    409: 'Sonar: transitorio impulsivo, nessun contatto.',
    # il sonarista dell'equipaggio (S5d nella missione; il testo vero lo compone nucleo/sonarista: questi sono i modelli)
    410: 'Sonar: nuovo contatto, Sierra {SIERRA}, rilevamento {RIL}.',
    411: 'Sonar: Sierra {SIERRA} classificato {CLASSE}.',
    412: 'Sonar: perso il contatto con Sierra {SIERRA}.',
    413: 'Sonar: riacquisito Sierra {SIERRA}, rilevamento {RIL}.',
    414: 'Sonar: il ping precedente non e\' ancora tornato, {COMANDANTE}.',   # Enhanced: 10 s fra due ping (simmetria, 31/10/2026)
    # artiglieria navale (docs/16, G6, principale: fascia 420-429); li compone l'app dagli impatti e dagli eventi
    420: 'Sonar: esplosioni in superficie a cadenza regolare, tiro d\'artiglieria, rilevamento {RIL}!',
    421: 'Vedetta: vampe di cannone, rilevamento {RIL}!',
    422: 'Colonne d\'acqua vicine, {COMANDANTE}!',
    423: 'Colpi d\'avvertimento sulla nostra prua, {COMANDANTE}.',
    424: 'Il periscopio d\'attacco non risponde!',
    425: 'Il periscopio di ricerca è colpito!',
    426: 'Schegge sulla testa del periscopio: vetro incrinato, entra acqua!',
    427: 'Colpito un albero, {COMANDANTE}!',
    428: 'Colpo in pieno sulla vela, {COMANDANTE}!',
    429: 'Illuminanti in cielo, rilevamento {RIL}: ci cercano.',
    # collisioni (dossier 16, P4, principale: fascia 430-439)
    430: 'Immersione d\'emergenza! Avanti tutta, alla quota di sicurezza.',
    431: 'Sonar: contatto a rilevamento costante, si avvicina, rilevamento {RIL}!',
    432: 'Allarme collisione! Contatto vicinissimo, rilevamento {RIL}!',
    433: 'Quasi collisione con un\'unità che non avevamo in contatto, {COMANDANTE}!',
    434: 'Quasi collisione, {COMANDANTE}: siamo passati a meno di duecento metri.',
    435: 'Sonar: la nave a rilevamento {RIL} fila una schiera trainata, il cavo è vicino!',
    436: 'Siamo impigliati in un cavo! L\'elica non gira, {COMANDANTE}.',
    437: 'Abbiamo toccato il cavo di una schiera trainata, {COMANDANTE}.',
    # DIRECT dalla carta (C4): il punto per il siluro filoguidato
    438: 'Scegliere il punto per il siluro, {COMANDANTE}.',
    439: 'Siluro diretto sul punto.',
    # l'Alfa: Akkord, Boksit, Tan, casse (dossier 19; sessione dell'acustica: fascia 440-449)
    440: 'Akkord guasto: nessuna arma utilizzabile, {COMANDANTE}.',
    441: 'Akkord di nuovo in servizio: le armi sono disponibili.',
    442: 'Akkord: bersaglio designato.',
    443: 'Tan inserito: assetto automatico.',
    444: 'Tan disinserito.',
    445: 'Tan fuori servizio: pompe in avaria.',
    446: 'Avaria alle casse!',
    447: 'Boksit: tappa del programma raggiunta.',
    448: 'Boksit: programma concluso.',
    449: 'Sfoghi aperti.',
    # BQS-15, il sonar delle mine (S06, runtime: fascia 450-459; missioni/src/missione_bqs15.cpp)
    450: 'BQS-15 in trasmissione: chi ascolta ci può sentire, {COMANDANTE}.',
    451: 'BQS-15: mina a prua, {COMANDANTE}!',
    452: 'BQS-15 spento.',
    # acque territoriali dichiarate dalla missione (dossier 22 §2.4; missioni/src/missione_artiglieria.cpp)
    453: 'Siamo entrati nelle acque territoriali di un altro paese, {COMANDANTE}.',
    454: 'Siamo usciti dalle acque territoriali.',
    455: 'Esplosioni in acqua: bombe d\'avvertimento, {COMANDANTE}!',
    # armi da esercitazione (schede I03 e S01; missioni/src/missione.cpp, Missione::colpisci)
    456: 'Colpo a segno del siluro d\'esercitazione.',
    457: 'Colpiti da un siluro d\'esercitazione, {COMANDANTE}!',
    # il radar del giocatore (docs/18 fase 2; Missione::radar_giocatore_acceso)
    458: 'Radar acceso: ci possono intercettare, {COMANDANTE}.',
    459: 'Radar spento: l\'albero e\' sott\'acqua.',
    # lo spettatore: i fatti fra unita' dell'IA che l'equipaggio puo' sapere (docs/18 fase 5, principale: fascia 460-469;
    # app/partita.cpp, Partita::interazioni_ia)
    # il controllo danni dell'Enhanced (docs/10 §10.19; DanniSottomarini)
    415: 'Sgombero del compartimento: si chiudono le porte fra un minuto, {COMANDANTE}.',
    416: 'Compartimento isolato: porte chiuse.',
    417: 'Compartimento riaperto, le squadre possono rientrare.',
    418: 'Il posto di comando non si isola, {COMANDANTE}.',
    419: 'Gli uomini chiusi nel compartimento sono perduti, {COMANDANTE}.',
    468: 'Quel compartimento è perduto: non c\'è niente da isolare, {COMANDANTE}.',
    469: 'Timoniere: sotto i 3 nodi i piani non portano su, serve velocità, {COMANDANTE}.',
    500: 'Periscopio e antenna si alzano solo a quota periscopica: Ctrl+P e almeno 3 nodi, {COMANDANTE}.',
    460: 'Sonar: un\'arma in acqua a rilevamento {RIL}, non è diretta a noi.',
    461: 'Sonar: colpi d\'artiglieria, rilevamento {RIL}.',
    462: 'Vedetta: vampe di cannone, rilevamento {RIL}.',
    463: 'Sonar: rumore d\'urto fra due scafi, rilevamento {RIL}.',
    464: 'Sonar: esche in acqua, rilevamento {RIL}: un sottomarino si difende.',
    465: 'Sonar: il contatto a rilevamento {RIL} accelera e si allontana.',
    466: 'Sonar: il contatto a rilevamento {RIL} ci segue, {COMANDANTE}.',
    467: 'Una nave ci passa a meno di mille iarde, rilevamento {RIL}.',
    # la navigazione incerta del giocatore (runtime: fascia 470-479; missioni/src/missione_navigazione.cpp)
    470: 'Navigatore: punto nave fatto, la stima è corretta, {COMANDANTE}.',
    471: 'Navigatore: in attesa del passaggio del satellite.',
    472: 'Navigatore: punto nave impossibile adesso.',
    473: 'Navigatore: punto nave interrotto.',
    474: 'Navigatore: inerziale fuori servizio, si naviga a stima.',
    # l'intercettazione (blocco 2 del sonar, E; runtime, missione_intercettazione.cpp)
    475: 'Sonar: SILURO IN ACQUISIZIONE!',
    # i rischi del fondo (runtime, missione_fondo.cpp)
    476: 'Il fango ci trattiene sul fondo, {COMANDANTE}.',
    477: 'Lo scafo striscia sulle rocce: timone e cupola a rischio.',
    478: 'Prese a mare intasate dal fango: avaria al raffreddamento.',
    # i missili (runtime, missione_missili.cpp)
    479: 'Il missile ha agganciato il bersaglio, {COMANDANTE}.',
    # il quadro del fondale (runtime, missione_fondale.cpp; fascia 480-489)
    480: 'Ecoscandaglio: meno di cento metri sotto la chiglia.',
    481: 'Ecoscandaglio: meno di trenta metri sotto la chiglia, {COMANDANTE}!',
    482: 'Ecoscandaglio: il fondo sale.',
    483: 'BQS-15: ostacolo a prua!',
    484: 'BQS-15: troppo veloci per quello che vediamo davanti, {COMANDANTE}.',
    485: 'Ecoscandaglio: nessun eco dal fondo.',
    # sotto il ghiaccio (disassemblatore, missione_ghiaccio.cpp; nella fascia del fondale, ok della principale 29/10)
    486: 'Sondatore: ghiaccio basso sopra di noi, {COMANDANTE}.',
    487: 'Sonar del ghiaccio: chiglia a prua!',
    488: 'Sondatore: acqua libera sopra di noi.',
    489: 'Ghiaccio sopra: alberi e periscopio non possono salire.',
    # il BQS-8 degli Sturgeon (disassemblatore, docs/ricerca/34)
    490: 'BQS-8 acceso.',
    491: 'BQS-8 spento.',
    # i sonar sovietici delle mine e del ghiaccio, per un sottomarino sovietico del giocatore (disassemblatore,
    # 31/10/2026): l'MG-70 di Victor, Akula e Sierra, l'MG-24 «Luč» dell'Alfa; come il BQS-15 (450-452)
    493: 'MG-70 in trasmissione: chi ascolta ci può sentire, {COMANDANTE}.',
    494: 'MG-70: mina a prua, {COMANDANTE}!',
    495: 'MG-70 spento.',
    496: 'MG-24 in trasmissione: chi ascolta ci può sentire, {COMANDANTE}.',
    497: 'MG-24: mina a prua, {COMANDANTE}!',
    498: 'MG-24 spento.',
    # il telemetro a reticolo dei periscopi sovietici (kit, 31/10/2026)
    492: 'Telemetro: galleggiamento sulla linea di mezzo, cursore sulla testa d\'albero ([ ]), poi H.',
})

def esporta_testi():
    d = json.load(open(f'{RADICE}/docs/traduzione/messaggi_bordo.json'))
    exe = json.load(open(f'{RADICE}/docs/traduzione/messaggi_exe.json'))
    os.makedirs(f'{USCITA}/testi', exist_ok=True)
    righe = []
    for k, v in d.items():
        if not k.startswith('0x') or not isinstance(v, dict) or not v.get('it'): continue
        righe.append(f"{int(k, 16)}\t{v['it'].replace(chr(9), ' ').replace(chr(10), ' ')}")
    for k in MESSAGGI_EXE:
        righe.append(f"{k}\t{exe[f'0x{k:02x}']['it']}")
    righe += [f'{k}\t{t}' for k, t in MESSAGGI_MOTORE.items()]
    righe += [f"{k}\t{exe[ds]['it']}" for k, ds in MESSAGGI_EXE_DS.items()]
    open(f'{USCITA}/testi/messaggi_bordo.tsv', 'w').write('\n'.join(righe) + '\n')
    return len(righe)

# pescaggio a pieno carico in metri (cupola del sonar compresa; per i sottomarini in superficie), valori
# indicativi dei dossier e delle schede pubbliche: servono al motore per gli incagli (engine/README.md)
PESCAGGIO = {
    'perry': 6.7, 'kidd': 9.6, 'spruance': 8.8, 'iowa': 11.6, 'nimitz': 11.3, 'sacramento': 11.6,
    'krivak2': 7.3, 'kara': 6.7, 'kirov': 9.1, 'slava': 8.4, 'koni': 4.2, 'nanuchka': 3.1,
    'petroliera': 14.0, 'mercantile': 9.0, 'cargo': 10.5, 'agi': 3.7, 'peschereccio': 5.0,
    'los_angeles': 9.4, 'victor3': 7.0, 'kilo': 6.2, 'alfa': 7.6, 'akula': 9.7, 'oscar': 9.2,
    'charlie2': 8.0, 'typhoon': 11.0, 'foxtrot': 6.0, 'sturgeon': 8.9, 'trafalgar': 9.5,
    # il primo lotto (dossier 31): l'Udaloy e il Grisha alla cupola del sonar
    'victor1': 7.2, 'delta3': 8.7, 'sierra': 8.8, 'udaloy': 7.8, 'grisha3': 6.1,
    # il secondo lotto (dossier 32): le navi alla cupola del sonar
    'ohio': 10.8, 'ticonderoga': 10.2, 'knox': 7.6, 'type22': 6.0, 'type206': 4.3,
    'type22b2': 6.4,   # STIMA: la Batch 2 un poco piu' pescante della Batch 1
    'ugra': 6.4, 'hauk': 1.6,
    'oslo': 5.5,   # alla cupola del sonar (4,4 lo scafo)
    'nacken': 5.5,
    'stockholm': 3.3, 'kitty_hawk': 11.4,
}

# forme per le collisioni (docs/ricerca/16_collisioni.md §4.1), misurate sui modelli 3D del kit (data/motore/modelli,
# parti «vela», «scafo», «carena», «cupola_sonar»; x riportate alla lunghezza della classe, prua positiva, 0 a mezza nave):
# sottomarini (altezza della vela sopra il dorso, x di poppa e di prua della vela); navi (sporgenza del duomo sonar
# sotto la chiglia, x di poppa e di prua del duomo). Il pescaggio_m delle navi comprende gia' il duomo.
VELA = {
    'los_angeles': (5.7, 19.0, 27.2), 'victor3': (5.2, 0.5, 19.0), 'kilo': (5.7, 0.7, 16.8), 'alfa': (3.0, -0.8, 22.1),
    'akula': (3.9, -6.9, 23.2), 'oscar': (4.3, 17.4, 47.0), 'charlie2': (2.9, -5.2, 13.9), 'typhoon': (9.2, -23.4, 4.9),
    'foxtrot': (6.0, -3.8, 9.9), 'sturgeon': (7.0, 15.1, 26.0), 'trafalgar': (4.6, 6.1, 15.4),
    # il primo lotto, STIMA finche' non ci sono i modelli: il Victor I come il Victor III in proporzione; la vela del
    # Delta III davanti al ponte dei missili; quella della Sierra bassa e lunga, appena a prua del mezzo
    'victor1': (4.8, 4.0, 22.7), 'delta3': (7.9, 29.6, 50.5), 'sierra': (4.0, -1.8, 20.0),
    # il secondo lotto: la vela dell'Ohio dal modello (da 29,2 a 42 m dalla prua, cima a 10 m, coperta a 3,5); quella
    # del 206 dal modello (la vela lunga a due piani, cielo a 6,1 m sopra il cassero a 1,5)
    'ohio': (6.5, 43.4, 56.2), 'type206': (4.6, -4.8, 11.5),
    # la Näcken dal modello (30/10/2026): la vela da 16,5 a 24,7 m dalla prua, cielo a 7,0 m sopra il cassero a 1,85
    'nacken': (5.2, 0.0, 8.3),
}
DUOMO = {
    'perry': (2.2, 15.8, 24.6), 'kidd': (3.4, 62.1, 80.1), 'spruance': (2.8, 60.0, 78.0), 'krivak2': (2.7, 39.4, 49.4),
    'kara': (1.2, 62.8, 76.8), 'kirov': (1.1, 80.9, 104.9), 'slava': (1.4, 63.4, 79.4), 'koni': (0.6, 19.3, 24.3),
    # il primo lotto, STIMA: la cupola dell'Udaloy sotto la prua (dossier 31: 7,8 m alla cupola), quella del Grisha a
    # mezza prua (6,1 m alla cupola, 3,7 lo scafo)
    'udaloy': (1.6, 60.0, 74.0), 'grisha3': (2.4, 14.0, 22.0),
    # il secondo lotto: la SQS-53 della Ticonderoga come quella della Kidd (modello proprio, stesso scafo; 10,2 m alla
    # cupola, 6,5 lo scafo); la SQS-26 della Knox sotto la prua e il Type 2016 delle Type 22 dai modelli
    'ticonderoga': (3.7, 62.4, 80.3), 'knox': (3.0, 50.5, 65.5), 'type22': (1.0, 49.0, 59.0),
    'type22b2': (1.0, 47.3, 57.3),   # dai modelli propri (30/10/2026); le cupole STIMA (tools/navi/opera_viva.py)
    'oslo': (1.1, 26.6, 34.6),   # l'SQS-36 sotto la prua, dal modello proprio (30/10/2026); la cupola STIMA
}

def esporta_teatri():
    from carta_epoca import blocco_geo, scrivi_fdl   # tools/pipeline/carta_epoca.py: stesso scrittore delle carte d'epoca
    os.makedirs(f'{USCITA}/teatri', exist_ok=True)
    fatti = []
    for t in sorted(os.listdir(f'{RADICE}/data/teatri')):
        d = f'{RADICE}/data/teatri/{t}'
        if not os.path.exists(f'{d}/heightmap.json'): continue
        h = json.load(open(f'{d}/heightmap.json'))
        g, c = h['griglia'], h['codifica']
        z = np.asarray(Image.open(f'{d}/heightmap_u16.png'))
        assert z.shape == (g['altezza'], g['larghezza']) and z.dtype.kind in 'iu', (t, z.shape, z.dtype)
        scrivi_fdl(f'{USCITA}/teatri/{t}.fdl', z, g['cella_m'], c['scala'], c['offset'], [blocco_geo(h)])
        fatti.append(t)
    return fatti

def esporta_modelli(esportate):
    """Copia i modelli 3D (extracted/navi/<classe>/modello.mdl) delle classi esportate; restituisce quelle con modello."""
    import shutil
    os.makedirs(f'{USCITA}/modelli', exist_ok=True)
    con = []
    for c in esportate:
        f = f'{RADICE}/extracted/navi/{CARTELLE.get(c, c)}/modello.mdl'
        if os.path.exists(f):
            shutil.copyfile(f, f'{USCITA}/modelli/{c}.mdl'); con.append(c)
    return con

def esporta_scafi(esportate):
    """Copia le sezioni e la compartimentazione (extracted/navi/<classe>/scafo.toml) delle navi di superficie."""
    import shutil
    os.makedirs(f'{USCITA}/scafi', exist_ok=True)
    con = []
    for c in esportate:
        f = f'{RADICE}/extracted/navi/{CARTELLE.get(c, c)}/scafo.toml'
        if os.path.exists(f):
            shutil.copyfile(f, f'{USCITA}/scafi/{c}.toml'); con.append(c)
    return con

def esporta_classi(esportate, con_modello=(), con_scafo=(), segnaposto=()):
    righe = ['# Classi navali per il motore (generato da tools/pipeline/esporta_motore.py: non modificare a mano).',
             '# Fonte e affidabilita\' indicate per ogni classe.', '']
    for c in esportate:
        nome, tipo, L, B, v, aff, *f = CLASSI[c]
        fonte = f[0] if f else 'docs/ricerca/01_piattaforme_e_armi.md'
        sprite = SEGNAPOSTO[c] if c in segnaposto else c
        righe += [f'[{c}]', f'nome = "{nome}"', f'sprite = "navi/{sprite}.spr"',
                  *([f'# sprite segnaposto: quello di {sprite} (manca la ricostruzione della classe)'] if c in segnaposto else []),
                  f'tipo_originale = {tipo}' if tipo is not None else '# tipo_originale: assente nel gioco del 1989',
                  f'lunghezza_m = {L}', f'larghezza_m = {B}', f'velocita_max_nodi = {v}',
                  *([f'pescaggio_m = {PESCAGGIO[c]}'] if c in PESCAGGIO else []),
                  *([f'altezza_vela_m = {VELA[c][0]}', f'vela_x_m = [{VELA[c][1]}, {VELA[c][2]}]'] if c in VELA else []),
                  *([f'duomo_m = {DUOMO[c][0]}', f'duomo_x_m = [{DUOMO[c][1]}, {DUOMO[c][2]}]'] if c in DUOMO else []),
                  *([f'modello = "modelli/{c}.mdl"'] if c in con_modello else
                    [f'modello = "modelli/{sprite}.mdl"  # della classe parente'] if c in segnaposto and sprite in con_modello else []),
                  *([f'scafo = "scafi/{c}.toml"'] if c in con_scafo else
                    [f'scafo = "scafi/{sprite}.toml"  # della classe parente'] if c in segnaposto and sprite in con_scafo else []),
                  f'fonte = "{fonte}"', f'affidabilita = "{aff}"', '']
    open(f'{USCITA}/classi.toml', 'w').write('\n'.join(righe))

if __name__ == '__main__':
    esporta_palette()
    esporta_schermate()
    print('messaggi di bordo:', esporta_testi())
    print('font: glifi', esporta_font())
    from font_stazioni import esporta_font_stazioni   # tools/pipeline/font_stazioni.py (Fase 3, dossier 19 §9.4)
    print('font delle stazioni:', esporta_font_stazioni(RADICE))
    print('teatri:', ' '.join(esporta_teatri()))
    from carta_epoca import esporta_carte   # tools/pipeline/carta_epoca.py (carte d'epoca, docs/08 §5)
    print('carte d\'epoca:', ' '.join(esporta_carte(RADICE)))
    from siluri import esporta_siluri   # tools/pipeline/siluri.py (fase C6b)
    print('siluri, esche:', esporta_siluri(RADICE))
    from missili import esporta_missili   # tools/pipeline/missili.py (docs/21_stazione_missili.md §11)
    print('missili, armi:', esporta_missili(RADICE))
    from artiglieria import esporta_artiglieria   # tools/pipeline/artiglieria.py (docs/16_artiglieria.md, G0)
    print('pezzi, classi, sagome dell\'artiglieria:', esporta_artiglieria(RADICE))
    from carico import esporta_carico   # tools/pipeline/carico.py (Fase 4a, M2)
    print('armi, locali, carichi della sala siluri:', esporta_carico(RADICE))
    from carriera import esporta_carriera   # tools/pipeline/carriera.py (Fase 4a, M3)
    print('gradi, decorazioni, soglie della carriera:', esporta_carriera(RADICE))
    from achievement import esporta_achievement   # tools/pipeline/achievement.py (roadmap 15)
    print('achievement, comandanti storici:', esporta_achievement(RADICE))
    from musica import esporta_musica   # tools/pipeline/musica.py: la musica dei titoli dell'originale
    print('musica dei titoli:', ' '.join(esporta_musica(RADICE)))
    from timbri_fm import esporta_timbri_fm   # tools/pipeline/timbri_fm.py: i timbri della musica FM dal vivo (roadmap 38)
    print('timbri FM (programmi, ritmo):', esporta_timbri_fm(RADICE))
    from musiche_fm import esporta_musiche_fm   # tools/pipeline/musiche_fm.py: le musiche delle schermate
    print('musiche FM:', esporta_musiche_fm(RADICE))
    from effetti_veri import esporta_effetti_veri   # tools/pipeline/effetti_veri.py: allarmi e sala radio registrati
    print('effetti veri:', esporta_effetti_veri(RADICE))
    sys.path.insert(0, os.path.join(RADICE, 'tools', 'mt32'))
    from controlla_rom import controlla as controlla_rom   # tools/mt32: nessun derivato delle ROM MT-32 dell'utente
    errori_rom, _, _ = controlla_rom(RADICE)
    if errori_rom:
        raise SystemExit('derivati delle ROM MT-32 nel progetto:\n' + '\n'.join(errori_rom))
    from suoni_classici import esporta_suoni_classici   # tools/pipeline/suoni_classici.py: i .T del classico
    print('campioni del classico:', ' '.join(esporta_suoni_classici(RADICE)))
    from navigazione import esporta_navigazione   # tools/pipeline/navigazione.py (roadmap 26)
    print('sistemi, punti nave, teatri della navigazione:', esporta_navigazione(RADICE))
    from interazioni import esporta_interazioni   # tools/pipeline/interazioni.py (roadmap 31, fase 4; dopo carico)
    print('dottrine di TMA, carichi e filo dell\'IA:', esporta_interazioni(RADICE))
    from acustica import esporta_acustica   # tools/pipeline/acustica.py (fase 2)
    print('firme acustiche, sensori:', esporta_acustica(RADICE))
    from asw_aereo import esporta_asw_aereo   # tools/pipeline/asw_aereo.py (docs/13_asw_aereo.md, A0)
    print('boe, mezzi e sonar a immersione dell\'ASW aereo:', esporta_asw_aereo(RADICE))
    from suono import esporta_suono   # tools/pipeline/suono.py (fase 2)
    print('profili del suono:', ' '.join(esporta_suono(RADICE)))
    fatte, segnaposto = [], []
    for c in CLASSI:
        if os.path.exists(f'{RADICE}/extracted/navi/{CARTELLE.get(c, c)}/sprite/ancore.json'):
            try:
                print(f'{c}: sprite', esporta_sprite(c)); fatte.append(c)
            except (OSError, AssertionError) as e:   # classe in ricostruzione: sprite incompleti
                print(f'{c}: sprite incompleti, classe saltata ({e})')
        elif c in SEGNAPOSTO:
            print(f'{c}: sprite segnaposto di {SEGNAPOSTO[c]}'); fatte.append(c); segnaposto.append(c)
        else:
            print(f'{c}: manca ancore.json (ricostruire la classe con costruisci.py)')
    con_modello = esporta_modelli(fatte)
    print('modelli 3D:', len(con_modello))
    con_scafo = esporta_scafi(fatte)
    print('scafi per i danni:', len(con_scafo))
    esporta_classi(fatte, con_modello, con_scafo, segnaposto)
    from civili import aggiungi_classi_civili, proprie   # tools/pipeline/civili.py: le classi del traffico (blocco 1 del sonar)
    civili_proprie = proprie(RADICE)   # quelle con la cartella propria del kit: sprite, modello e scafo loro
    for c in civili_proprie:
        print(f'{c}: sprite', esporta_sprite(c))
    esporta_modelli(civili_proprie)
    esporta_scafi(civili_proprie)
    print('classi civili del traffico:', ' '.join(aggiungi_classi_civili(RADICE)))
    from traffico import esporta_traffico   # tools/pipeline/traffico.py: le rotte del traffico per teatro
    print('rotte del traffico, problemi:', len(esporta_traffico(RADICE)))
    from periscopi import esporta_periscopi   # tools/pipeline/periscopi.py (docs/14_periscopi.md, P0)
    print('periscopi, classi coi periscopi:', esporta_periscopi(RADICE))
    from meteo import esporta_meteo   # tools/pipeline/meteo.py (docs/15_meteo_e_mare.md, M0)
    print('meteo: teatri, punti ERA5:', esporta_meteo(RADICE))
    from crediti import esporta_crediti   # tools/pipeline/crediti.py: crediti e licenze degli asset
    print('crediti:', esporta_crediti(RADICE))
    from carte_basi import esporta_carte_basi   # tools/pipeline/carte_basi.py: le carte del Museo, prima dell'enciclopedia che le cita
    print('carte delle basi:', len(esporta_carte_basi(RADICE)[0]))
    from enciclopedia import esporta_enciclopedia   # tools/pipeline/enciclopedia.py (requisito 14): dopo i cataloghi e i crediti
    print("enciclopedia: voci nell'indice, voci scritte:", esporta_enciclopedia(RADICE))
    from libretto import esporta_libretto   # tools/pipeline/libretto.py (docs/25): dopo le classi, gli emettitori e l'enciclopedia
    print('tavole del libretto di riconoscimento:', esporta_libretto(RADICE))
    from luci import esporta_luci   # tools/pipeline/luci.py (docs/27): dopo le classi e i modelli
    print('luci di navigazione, classi:', esporta_luci(RADICE))
    from fari import esporta_fari   # tools/pipeline/fari.py: dai CSV di extracted/fari_wikidata
    print('fari:', esporta_fari(RADICE))
