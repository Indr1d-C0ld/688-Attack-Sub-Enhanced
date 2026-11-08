"""Figure del manuale stampato, ritagliate dalla scansione di Internet Archive.

Fonte: https://archive.org/details/688attacksub_201909 (manuale DOS 1988, 80 pagine). Le pagine si scaricano
con l'archivio "688 Attack Sub_jp2.zip" (una JP2 da 1653x2114 per pagina, circa 200 dpi; il foglio n contiene
la pagina stampata n-1), conservato in extracted/manuale_stampa/scansione_jp2.zip (SHA-256 24c10b16...02b6).
Il PDF in original-game/ e' un'altra scansione, a 150 dpi (817x1042): le coordinate qui sotto non valgono per quello.

Ogni figura ha un riquadro approssimato a mano (coordinate del foglio), che lo script stringe sul contenuto
scuro con un margine. Uscite: extracted/manuale_stampa/figure/*.png (scala di grigi, risoluzione della
scansione) e docs/traduzione/manuale/figure.json, usato da manuale_unisci.py per inserire le immagini:
'ancora' e' l'id del blocco del sorgente; 'dove' = 'didascalia' (l'immagine sostituisce la figura ASCII
che precede la didascalia), 'dopo' (dopo il blocco) o 'prima' (prima del blocco).

Uso:  manuale_figure.py <radice> [archivio zip o cartella delle JP2]
"""
import io, os, sys, json, zipfile
from PIL import Image

# nome, foglio, riquadro (x0, y0, x1, y1), ancora, dove, didascalia EN (stampa), didascalia IT
# zone della pagina da sbiancare prima del ritaglio (segni del testo vicino che entrano nel riquadro)
CANCELLA = {'rilevamento': [(650, 560, 742, 610)]}

FIGURE = [
    ('copertina', 0, (0, 0, 1653, 2114), '0920199af4', 'prima', 'Cover', 'Copertina del manuale'),
    ('fig_1_1_centrale', 7, (410, 555, 1200, 1180), '4cd4ce3a7a', 'didascalia', 'Figure 1.1: 688 CONN', None),
    ('fig_1_2_sala_radio', 9, (410, 505, 1190, 1050), '33067b292a', 'didascalia', 'Figure 1.2: Radio Room', None),
    ('fig_2_1_sala_radio', 11, (410, 1310, 1180, 1835), '22086b46f7', 'didascalia', 'Figure 2.1: Radio Room', None),
    ('fig_2_2_periscopio', 12, (380, 845, 1135, 1450), '3a041dd505', 'didascalia', 'Figure 2.2: 688 Periscope', None),
    ('fig_2_3_navigazione', 13, (410, 1075, 1520, 1605), '8f01abc3a8', 'didascalia', 'Figure 2.3: Navigation Board', None),
    ('fig_2_4_governo', 14, (380, 780, 1400, 1310), '3208c992bd', 'didascalia', "Figure 2.4: Ship's Control Panel", None),
    ('fig_2_5_armi', 15, (410, 210, 1400, 705), 'c12399932d', 'didascalia', 'Figure 2.5: Weapon Control Panel', None),
    ('fig_2_6_sonar', 16, (375, 210, 1185, 705), '179a75316c', 'didascalia', 'Figure 2.6: Sonar Room', None),
    ('fig_2_7_navigazione', 16, (375, 1100, 1185, 1588), 'dd350a2694', 'didascalia', 'Figure 2.7: Navigation Board', None),
    ('fig_2_8_armi', 17, (240, 205, 1425, 675), '68e18cb3cc', 'didascalia', 'Figure 2.8: Weapon Control Panel', None),
    ('fig_2_9_navigazione', 18, (375, 205, 1495, 705), 'c2ddc78f52', 'didascalia', 'Figure 2.9: Navigation Board', None),
    ('fig_3_1_faer_oer', 20, (395, 270, 1170, 760), '038aa50ebb', 'didascalia', 'Figure 3.1: Faeroe Islands - Norwegian Sea', None),
    ('fig_3_2_barents', 22, (400, 270, 1170, 815), '43616d32b4', 'didascalia', 'Figure 3.2: Barents Sea', None),
    ('fig_3_3_gibilterra', 23, (400, 315, 1170, 805), 'e059fc7334', 'didascalia', 'Figure 3.3: Strait of Gibraltar', None),
    ('fig_3_4_adriatico', 24, (395, 1015, 1165, 1500), 'c864bba153', 'didascalia', 'Figure 3.4: Mouth of the Adriatic Sea', None),
    ('fig_3_5_kunashir', 25, (415, 742, 1185, 1228), '93ec4a290d', 'didascalia', 'Figure 3.5: Kunashir Passage', None),
    ('fig_3_6_piattaforma_celtica', 26, (400, 1308, 1170, 1793), 'fc273e68b0', 'didascalia', 'Figure 3.6: Edge of the Celtic Shelf', None),
    ('fig_3_7_baltico', 28, (395, 772, 1165, 1255), '94212d2ba7', 'didascalia', 'Figure 3.7: Baltic Sea', None),
    ('fig_5_1_carta', 38, (175, 250, 1425, 860), '224fb7470b', 'didascalia', 'Figure 5.1: Top-Down Map', None),
    ('fig_5_2_profilo', 39, (400, 1170, 1175, 1810), '4d061a568f', 'didascalia', 'Figure 5.2: Contour Map', None),
    ('comandi_carta', 41, (395, 385, 1475, 635), '68f2d969a4', 'dopo', '688 / Alfa Map Display Control',
     "Comandi della carta del 688 (a sinistra) e dell'Alfa (a destra)"),
    ('barra_bersaglio', 41, (415, 1498, 1525, 1588), '526c1134e7', 'dopo', 'Target information',
     'Il riquadro delle informazioni sul bersaglio'),
    ('rilevamento', 42, (385, 575, 742, 845), 'f8e472767a', 'prima', 'Bearing',
     'Rilevamento: il riferimento (000°) è la prua del sottomarino'),
    ('rotta', 42, (385, 1050, 742, 1315), 'f8e472767a', 'prima', 'Course', 'Rotta: il riferimento (000°) è il nord'),
    ('timone', 47, (415, 905, 835, 1135), '6703272967', 'dopo', 'Rudder: neutral position, hard left, hard right',
     'Il timone: posizione neutra, tutto a sinistra, tutto a destra'),
    ('fig_5_5_analizzatore', 47, (415, 1468, 945, 1700), 'a61138b4b7', 'didascalia', 'Figure 5.5: Sonar Analyzer', None),
    ('fig_5_6_antenna_rimorchiata', 49, (420, 205, 1130, 675), '4ca0c51a9f', 'didascalia',
     'Figure 5.6: Increased Sonar Capabilities With Towed Array', None),
    ('fig_6_1_strati_termici', 56, (405, 1378, 1245, 1910), '6b2a65c538', 'didascalia', 'Figure 6.1: Thermal Layers', None),
    ('fig_6_2_esche', 58, (395, 636, 880, 1250), '4e2da7c1e8', 'dopo', 'Figure 6.2: Using Noisemakers',
     'Figura 6.2: uso delle esche acustiche'),
    ('fig_6_3_evitare_siluri', 60, (405, 476, 930, 1003), 'bb54154131', 'dopo', 'Figure 6.3: Evading Torpedoes',
     'Figura 6.3: evitare i siluri'),
    ('fig_6_4_baffles', 60, (405, 1378, 890, 1885), '7b175a9b6a', 'dopo', 'Figure 6.4: Baffles',
     "Figura 6.4: il cono d'ombra poppiero (baffles)"),
    ('fig_a_null_modem', 71, (430, 680, 1555, 1805), '8526ba70d5', 'didascalia', 'Figure A: Null-Modem Connections', None),
]

def carta_bianca(im, soglia=235):
    # il fondo della carta (grigio chiaro nella scansione) diventa bianco
    return im.point(lambda v: 255 if v >= soglia else v)

def stringi(im, soglia=200, margine=10):
    # riquadro del contenuto scuro (la fascia grigia chiara del margine sinistro resta sopra la soglia)
    bb = im.point(lambda v: 255 if v < soglia else 0).getbbox()
    if not bb: return im
    x0, y0, x1, y1 = bb
    return im.crop((max(0, x0 - margine), max(0, y0 - margine), min(im.width, x1 + margine), min(im.height, y1 + margine)))

if __name__ == '__main__':
    root = sys.argv[1]
    sorgente = sys.argv[2] if len(sys.argv) > 2 else f'{root}/extracted/manuale_stampa/scansione_jp2.zip'
    if sorgente.endswith('.zip'):
        arch = zipfile.ZipFile(sorgente); voci = sorted(n for n in arch.namelist() if n.endswith(".jp2"))
        leggi = lambda i: Image.open(io.BytesIO(arch.read(voci[i])))
    else:
        voci = sorted(f for f in os.listdir(sorgente) if f.endswith('.jp2'))
        leggi = lambda i: Image.open(os.path.join(sorgente, voci[i]))
    dst = f'{root}/extracted/manuale_stampa/figure'; os.makedirs(dst, exist_ok=True)
    man = []
    for nome, foglio, box, ancora, dove, en, it in FIGURE:
        pag = leggi(foglio).convert('L')
        for z in CANCELLA.get(nome, []): pag.paste(255, z)
        im = pag.crop(box) if nome == 'copertina' else carta_bianca(stringi(pag.crop(box)))
        im.save(f'{dst}/{nome}.png', optimize=True)
        man.append(dict(nome=nome, file=f'extracted/manuale_stampa/figure/{nome}.png', foglio=foglio,
                        pagina=foglio - 1 if foglio else None, dimensioni=list(im.size), ancora=ancora, dove=dove,
                        didascalia_en=en, didascalia_it=it))
        print(f'{nome:30s} foglio {foglio:2d}  {im.size[0]}x{im.size[1]}')
    json.dump(dict(fonte='https://archive.org/details/688attacksub_201909', figure=man),
              open(f'{root}/docs/traduzione/manuale/figure.json', 'w'), indent=1, ensure_ascii=False)
