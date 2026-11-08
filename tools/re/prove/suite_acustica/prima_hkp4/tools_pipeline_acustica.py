"""Firme di rumore per classe e sensori dell'Enhanced -> data/motore/acustica.toml (fase 2, acustica).
Li legge nucleo/acustica.hpp (carica_acustica); il significato dei campi e' in FirmaRumore e Sensore.

Firme: livello a banda larga 5-200 Hz (dB re 1 uPa @1 m) per velocita', dalla tabella del dossier 01 §3.2
(Miasnikov 1998, Tyler 1992, graduatorie USN; confidenza bassa-media, centri degli intervalli). Il dossier 02 §2.6
usa la banda 100 Hz-10 kHz con numeri piu' alti: si segue l'01, come chiesto dalla sessione principale, perche'
ha i punti per velocita'. Navi: 01 §3.2 (Ross, Urick), con il Prairie-Masker (-10 dB) sulle navi USA che lo hanno.
Forza di bersaglio: 02 §1.4, -10 dB sui battelli con le piastrelle anecoiche (Victor III, Alfa, Akula).
Sensori: 02 §1.2, §1.5, §2.5 (rumore proprio e baffles), tutti STIMA DI GIOCO salvo le formule.
Tonali (LOFAR): il livello della riga piu' forte (02 §2.6) alla quinta armonica della rete di bordo, 300 Hz per le
unita' occidentali (60 Hz) e 250 Hz per le sovietiche (50 Hz) (02 §2.2; la frequenza e' una scelta di gioco).
Uso: acustica.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

# classe: (punti (kn, dB), v_cavitazione_kn in superficie, tonale_db, ts prua, ts traverso, snorkel_db, fonte).
# L'insorgenza in superficie e' la tabella della dinamica (DinamicaSottomarino, C6a; dossier 02 §2.3): la legge da qui
SOTTOMARINI = {
    'los_angeles': ([(4.5, 112.5), (8, 116), (15, 128), (32, 143)], 5.0, 110, 12, 25, 0, '01 §3.2: 688 Flight I, 60 Hz'),
    'sturgeon':    ([(4.5, 117.5), (8, 122), (15, 135), (26, 145)], 4.5, 114, 12, 25, 0, '01 §3.2'),
    'trafalgar':   ([(4.5, 110.5), (8, 114), (15, 126), (32, 140)], 5.5, 107, 12, 25, 0, '01 §3.2: pump-jet'),
    'victor3':     ([(4.5, 122.5), (8, 127), (15, 140), (30, 150)], 4.5, 118, 2, 15, 0, '01 §3.2; anecoico (02 §1.4)'),
    'alfa':        ([(4.5, 127.5), (8, 135), (15, 145), (40, 160)], 4.0, 125, 2, 15, 0, '01 §3.2: pompe Pb-Bi; anecoico'),
    'akula':       ([(4.5, 112.5), (8, 117), (15, 130), (33, 145)], 5.5, 108, 2, 15, 0, '01 §3.2 (Miasnikov 110); anecoico'),
    'oscar':       ([(4.5, 120.5), (8, 125), (15, 138), (32, 150)], 4.0, 112, 15, 30, 0, '01 §3.2: scafo grande, 2 assi'),
    'charlie2':    ([(4.5, 132.5), (8, 138), (15, 148), (24, 155)], 3.5, 135, 12, 25, 0, '01 §3.2: 2a generazione, 50 Hz'),
    'typhoon':     ([(6, 125), (8, 125.5), (15, 140), (25, 148)], 4.0, 114, 15, 30, 0, '01 §3.2 (Miasnikov 125)'),
    'foxtrot':     ([(4.5, 122.5), (8, 128), (15, 140)], 3.0, 110, 12, 25, 22.5, '01 §3.2: a batteria; snorkel +20-25'),
    'kilo':        ([(4.5, 114.5), (8, 120), (15, 132), (20, 138)], 3.5, 100, 12, 25, 22.5, '01 §3.2 (Miasnikov); snorkel +20-25'),
    # primo lotto della libreria allargata (docs/ricerca/31, 29/10/2026): STIMA dalle fonti aperte e da Miasnikov
    'victor1':     ([(4.5, 135), (8, 141), (15, 151), (32, 160)], 3.0, 138, 10, 22, 0,
                    '31 §1: 1a generazione, «il piu\' rumoroso» (Conway), sopra il Charlie II; senza rafting'),
    'delta3':      ([(4.5, 127.5), (8, 132), (15, 144), (24, 152)], 3.5, 120, 15, 30, 0,
                    '31 §2: Miasnikov 125-130 dB a 4 kn; 2 assi, eliche a 5 pale'),
    'sierra':      ([(4.5, 117.5), (8, 122), (15, 134), (35, 148)], 5.0, 112, 2, 15, 0,
                    '31 §3: fra Delta III e Akula; rafting e anecoico; «come i primi Los Angeles»'),
    # secondo lotto (docs/ricerca/32): l'Ohio «a 20 nodi silenzioso come un Lafayette a 6»; il 206 piccolo e amagnetico
    'ohio':        ([(4.5, 105), (8, 108), (15, 117), (20, 123), (25, 130)], 6.0, 100, 15, 32, 0,
                    '32 §1: il piu\' silenzioso degli USA, circolazione naturale del S8G, 7 pale'),
    'type206':     ([(4.5, 110), (8, 116), (15, 128), (17, 131)], 3.5, 98, 8, 18, 22.5,
                    '32 §5: SSK di 48 m, amagnetico; snorkel +20-25 come Kilo e Foxtrot'),
    # il Näcken (A14) svedese (docs/ricerca/35): 49,5 m, 980 t, 20 nodi in immersione, 300 m; un diesel MTU e il motore
    # elettrico su un asse; la Stirling AIP (1988-89) sul solo Näcken: il motore non la modella (resta il diesel a snorkel)
    'nacken':      ([(4.5, 108), (8, 114), (15, 125), (20, 132)], 3.5, 96, 9, 20, 22.5,
                    '35: SSK svedese come il 206 ma piu\' grande (STIMA); snorkel +22,5 come gli altri diesel'),
    'sottomarino_generico': ([(4.5, 120), (8, 125), (15, 138), (30, 150)], 4.5, 112, 12, 25, 0, 'classe senza firma: SSN medio'),
}
# classe: (punti, tonale_db, ts prua, ts traverso, fonte)
NAVI = {
    'perry':      ([(5, 135), (15, 150), (29, 165)], 130, 20, 32, '01 §3.2 fregate 150-170, Prairie-Masker -10'),
    'kidd':       ([(5, 138), (15, 152), (33, 168)], 132, 20, 34, '01 §3.2, turbine a gas, Prairie-Masker'),
    'spruance':   ([(5, 138), (15, 152), (32.5, 168)], 132, 20, 34, '01 §3.2, turbine a gas, Prairie-Masker'),
    'krivak2':    ([(5, 145), (15, 160), (32, 172)], 140, 20, 32, '01 §3.2 fregate senza Masker'),
    'kara':       ([(5, 145), (15, 160), (34, 173)], 140, 22, 35, '01 §3.2'),
    'slava':      ([(5, 145), (15, 160), (32, 172)], 140, 22, 35, '01 §3.2'),
    'koni':       ([(5, 145), (15, 160), (27, 170)], 140, 18, 30, '01 §3.2'),
    'nanuchka':   ([(5, 145), (15, 160), (33, 172)], 140, 15, 28, '01 §3.2 (piccola)'),
    # docs/ricerca/35: la Oslo a vapore su un asse come la Knox, piu' piccola; la Stockholm CODAG su tre assi, piccola; la
    # Kitty Hawk a vapore con otto caldaie su quattro assi, piu' rumorosa della Nimitz nucleare
    'oslo':       ([(5, 140), (15, 155), (25, 165)], 134, 18, 30, '35: fregata norvegese (Dealey modificata), 1.760 t, '
                                                                 'turbina a vapore su 1 asse, 20.000 shp, 25 nodi'),
    'stockholm':  ([(5, 135), (15, 150), (32, 163)], 130, 10, 20, '35: corvetta svedese, 372 t, CODAG su 3 assi (2 MTU '
                                                                 '16V396 e una Allison 570), 32 nodi'),
    'kitty_hawk': ([(5, 158), (15, 173), (32, 183)], 153, 25, 40, '35: portaerei a vapore, 8 caldaie e 4 assi, 280.000 shp, '
                                                                 '32 nodi; sopra la Nimitz (caldaie, ventilatori, pompe)'),
    'hauk':       ([(5, 130), (20, 150), (34, 163)], 125, 8, 16, 'STIMA: motocannoniera norvegese Hauk, 36,5 m e 148 t, due '
                                                                'diesel veloci su 2 assi, 34 nodi (kit, extracted/navi/hauk)'),
    'kirov':      ([(5, 150), (15, 165), (32, 177)], 145, 25, 40, '01 §3.2 portaerei/BB 170-180 (incrociatore nucleare)'),
    'iowa':       ([(5, 155), (15, 170), (33, 180)], 150, 25, 40, '01 §3.2 portaerei/BB 170-180'),
    'nimitz':     ([(5, 155), (15, 170), (30, 180)], 150, 25, 40, '01 §3.2 portaerei/BB 170-180'),
    'sacramento': ([(5, 150), (15, 168), (26, 177)], 145, 25, 38, '01 §3.2'),
    'mercantile': ([(5, 160), (12, 175), (18, 183)], 155, 25, 38, '01 §3.2 mercantili 175-185'),
    'cargo':      ([(5, 162), (12, 177), (16.4, 183)], 157, 25, 40, '01 §3.2 mercantili 175-185'),
    'ugra':       ([(5, 148), (12, 160), (17, 168)], 143, 24, 38, 'STIMA: base galleggiante Progetto 1886, 141 m e 9.650 t, due '
                                                                  'diesel su 2 assi, 17 nodi (kit, extracted/navi/ugra); fra le '
                                                                  'combattenti diesel e i mercantili'),
    'petroliera': ([(5, 165), (12, 180), (15, 186)], 160, 25, 40, '01 §3.2 petroliere 180-190'),
    'ticonderoga': ([(5, 138), (15, 152), (32, 168)], 132, 22, 36, '32 §2: scafo e macchine della Spruance, Prairie-Masker'),
    'knox':        ([(5, 142), (15, 157), (27, 168)], 136, 20, 33, '32 §3: vapore su un asse, Prairie-Masker non confermato'),
    'type22':      ([(5, 140), (15, 155), (30, 169)], 135, 20, 33, '32 §4: COGOG, Agouti; diesel dei generatori senza raft'),
    'type22b2':    ([(5, 140), (15, 155), (30, 169)], 135, 20, 34, '32 §4: il Batch 2, scafo allungato di 15 m per la 2031Z; '
                                                                    'le stesse macchine e i diesel senza raft (STIMA)'),
    'udaloy':     ([(5, 145), (15, 160), (30, 172)], 140, 22, 36, '31 §4: COGAG su 2 assi, nessuna cortina d\'aria nota (come Kara)'),
    'grisha3':    ([(5, 143), (15, 158), (32, 171)], 138, 15, 27, '31 §5: CODAG su 3 assi, piccola (come Koni)'),
    'nave_generica': ([(5, 145), (15, 160), (30, 172)], 140, 20, 32, 'classe senza firma: fregata senza Masker'),
    # AGI classe Okean (ex peschereccio, 50,8 m, 760 t): un diesel R8DV-148 da 540 CV, un'elica a passo fisso. Ferma in
    # deriva col generatore acceso: la tabella parte da 1 nodo (il livello sotto e' quello del generatore). STIMA da
    # 01 §3.2 (pescherecci 150-165) e dalla potenza, un quinto di quella del peschereccio d'altura
    'agi':        ([(1, 135), (5, 150), (9, 158), (11.5, 163)], 132, 15, 28,
                   'STIMA: AGI Okean, diesel da 540 CV; in deriva il solo generatore (kit, extracted/navi/agi)'),
    # peschereccio d'altura a rampa poppiera (1987, 57 m, ~2000 t): diesel da ~4000 CV a giri costanti su un'elica a
    # passo variabile. Al lavoro (3-5 nodi) il motore tira la rete quasi a piena potenza e l'elica a passo alto cavita:
    # forte quasi come in trasferimento; fra 5 e 8 nodi, a passo ridotto, piu' quieto. STIMA (01 §3.2; 09 §5.2)
    'peschereccio': ([(1, 148), (2.5, 166), (5, 167), (7, 161), (10, 167), (14, 174)], 152, 18, 32,
                     'STIMA: peschereccio a rampa poppiera, diesel da 4000 CV, passo variabile; al lavoro a 2-5,5 kn '
                     '(kit, extracted/navi/peschereccio)'),
}
# falsi bersagli (02 §1.4, §4.5): balene a bassa frequenza, eco debole
ALTRI = {
    'biologico':  ([(1, 140), (10, 140)], 150, 0, 10, '02 §4.5: balenottere 180-190 dB a 10-40 Hz (impulsi); STIMA in banda'),
    'adc_mk2':    ([(0.1, 150), (10, 150)], 140, 10, 10, '05 §5.1: esca ADC Mk 2, 150 dB, eco puntuale (C6b)'),
    # siluri in corsa: 150-160 dB a 45 kn (piano del runtime §6, STIMA); TS 02 §1.4 (-25 prua, -20 traverso)
    'mk48':       ([(28, 140), (40, 150), (55, 160)], 140, -25, -20, 'STIMA: siluro pesante, 150-160 dB a 45 kn'),
    'mk48_adcap': ([(28, 138), (40, 148), (60, 158)], 138, -25, -20, 'STIMA: ADCAP piu\' silenzioso'),
    'tigerfish':  ([(24, 138), (35, 148)], 138, -25, -20, 'STIMA'),
    'spearfish':  ([(30, 140), (60, 160)], 140, -25, -20, 'STIMA'),
    'uset80':     ([(35, 145), (45, 155)], 145, -25, -20, 'STIMA; anche il type53 delle missioni'),
    'test71m':    ([(24, 140), (40, 152)], 140, -25, -20, 'STIMA'),
    'type53_65k': ([(44, 158), (46, 158)], 150, -25, -20, 'STIMA: segui-scia a vapore-gas, rumoroso'),
    'mk46':       ([(30, 140), (45, 152)], 140, -28, -23, 'STIMA: leggero'),
    'at1':        ([(26, 145), (28, 145)], 140, -28, -23, 'STIMA'),
    'siluro_generico': ([(28, 140), (45, 155)], 140, -25, -20, 'arma senza firma'),
}

# Righe dei macchinari ed eliche per l'analizzatore di firma (S3; dossier 02 §2.1-2.2 e §2.7, dossier 01 §3.3).
# STIMA DI GIOCO: le firme vere sono segrete. Le regole seguono le fonti (rete a 60 Hz USA e UK, 50 Hz URSS, 400 Hz
# dell'Alfa; pompe del primario lente e veloci, turbogeneratori e riduttori; diesel dei battelli a snorkel e dei
# mercantili; pump-jet senza riga di pala; impulsi a 20 Hz delle balenottere), le frequenze sono diverse per classe.
# riga: (hz, dB rispetto alla tonale, tipo, v_min_kn, v_max_kn); elica: (pale, assi, giri al minuto per nodo).
R = lambda hz, db, tipo, vmin=0.0, vmax=1e9: (hz, db, tipo, vmin, vmax)

# Blocco 2 del sonar (docs/22, deciso dall'utente il 28/10/2026): le righe deboli di turbina e riduttore legate all'asse
# dei sottomarini nucleari, come multipli della frequenza d'asse (RigaFirma::multiplo_asse): il rotore della turbina
# (circa 50-100 Hz a piena potenza), l'ingranamento del 2° e del 1° stadio (circa 250 Hz e 1,2 kHz a 32 kn: dossier
# 28 §4.2). Salgono coi giri sul LOFAR proprio e altrui; la classificazione resta sulle righe fisse. Tutto STIMA,
# diverso per classe: classe -> [(multiplo, dB sulla tonale, tipo)]
ASSE = {
    'los_angeles': [(23, -18, 'turbina'), (78, -15, 'riduttore'), (375, -20, 'riduttore')],
    'sturgeon':    [(21, -17, 'turbina'), (72, -14, 'riduttore'), (330, -20, 'riduttore')],
    'trafalgar':   [(25, -19, 'turbina'), (81, -16, 'riduttore'), (400, -21, 'riduttore')],
    'victor3':     [(19, -15, 'turbina'), (66, -13, 'riduttore'), (290, -19, 'riduttore')],
    'akula':       [(22, -17, 'turbina'), (74, -15, 'riduttore'), (350, -20, 'riduttore')],
    'alfa':        [(27, -14, 'turbina'), (88, -12, 'riduttore'), (420, -18, 'riduttore')],
    'oscar':       [(20, -16, 'turbina'), (70, -14, 'riduttore'), (310, -19, 'riduttore')],
    'charlie2':    [(18, -14, 'turbina'), (64, -12, 'riduttore'), (280, -18, 'riduttore')],
    'typhoon':     [(17, -17, 'turbina'), (60, -15, 'riduttore'), (270, -20, 'riduttore')],
    'victor1':     [(18, -13, 'turbina'), (62, -11, 'riduttore'), (270, -17, 'riduttore')],   # 31 §1: senza rafting, forti
    'delta3':      [(17, -15, 'turbina'), (60, -13, 'riduttore'), (265, -19, 'riduttore')],
    'sierra':      [(21, -17, 'turbina'), (72, -15, 'riduttore'), (340, -20, 'riduttore')],   # 31 §3: rafting
    'ohio':        [(24, -20, 'turbina'), (80, -17, 'riduttore'), (380, -22, 'riduttore')],   # 32 §1: il piu' quieto
}
USA = [R(60, -6, 'rete'), R(120, -9, 'rete'), R(180, -12, 'rete'), R(300, 0, 'rete')]
URSS = [R(50, -6, 'rete'), R(100, -9, 'rete'), R(150, -12, 'rete'), R(250, 0, 'rete')]
ANALISI = {
    'los_angeles': (USA + [R(42.3, -3, 'pompa', 0, 15), R(84.6, -2, 'pompa', 15), R(97.5, -4, 'turbina'), R(168, -8, 'riduttore')],
                    (7, 1, 6.0), 'usa', 'ssn'),
    'sturgeon':    (USA + [R(38.1, -3, 'pompa', 0, 12), R(76.2, -2, 'pompa', 12), R(112.0, -4, 'turbina'), R(150, -7, 'riduttore')],
                    (5, 1, 7.0), 'usa', 'ssn'),
    'trafalgar':   (USA + [R(47.0, -3, 'pompa', 0, 14), R(94.0, -2, 'pompa', 14), R(104.0, -4, 'turbina'), R(140, -8, 'riduttore')],
                    (0, 1, 6.5), 'uk', 'ssn'),   # pump-jet: niente riga di pala netta (01 §3.2)
    'victor3':     (URSS + [R(36.0, -2, 'pompa'), R(88.0, -4, 'turbina'), R(132, -6, 'riduttore'), R(185, -9, 'ausiliario')],
                    (4, 1, 8.0), 'urss', 'ssn'),   # eliche in tandem a 4 pale
    'akula':       (URSS + [R(44.0, -3, 'pompa', 0, 12), R(88.5, -2, 'pompa', 12), R(93.0, -4, 'turbina'), R(155, -8, 'riduttore')],
                    (7, 1, 6.0), 'urss', 'ssn'),
    'alfa':        (URSS + [R(400, -3, 'ausiliario'), R(29.0, 0, 'pompa'), R(120.0, -4, 'turbina'), R(210, -7, 'riduttore')],
                    (7, 1, 4.5), 'urss', 'ssn'),   # rete a 400 Hz, pompe del metallo liquido
    'oscar':       (URSS + [R(33.0, -3, 'pompa', 0, 14), R(66.0, -2, 'pompa', 14), R(79.0, -4, 'turbina'), R(142, -8, 'riduttore')],
                    (7, 2, 5.0), 'urss', 'ssgn'),
    'charlie2':    (URSS + [R(31.0, -1, 'pompa'), R(72.0, -3, 'turbina'), R(118, -6, 'riduttore'), R(196, -9, 'ausiliario')],
                    (5, 1, 8.0), 'urss', 'ssgn'),
    'typhoon':     (URSS + [R(27.0, -3, 'pompa', 0, 12), R(54.0, -2, 'pompa', 12), R(69.0, -4, 'turbina'), R(126, -7, 'riduttore')],
                    (7, 2, 4.0), 'urss', 'ssbn'),
    # primo lotto della libreria allargata (docs/ricerca/31): righe STIMA sulle regole di sopra, frequenze proprie
    'victor1':     (URSS + [R(34.0, 0, 'pompa'), R(68.0, -2, 'pompa'), R(83.0, -3, 'turbina'), R(124, -5, 'riduttore'),
                            R(176, -8, 'ausiliario')],
                    (5, 1, 8.5), 'urss', 'ssn'),   # elica a 5 pale (31 §1, media-bassa) e due ausiliarie
    'delta3':      (URSS + [R(29.0, -2, 'pompa', 0, 12), R(58.0, -1, 'pompa', 12), R(74.0, -3, 'turbina'), R(137, -6, 'riduttore'),
                            R(205, -9, 'ausiliario')],
                    (5, 2, 6.5), 'urss', 'ssbn'),   # 2 assi, eliche a 5 pale (31 §2)
    'sierra':      (URSS + [R(41.0, -3, 'pompa', 0, 12), R(82.0, -2, 'pompa', 12), R(97.0, -4, 'turbina'), R(161, -8, 'riduttore')],
                    (7, 1, 6.0), 'urss', 'ssn'),   # pale non trovate: 7 come l'Akula (STIMA, 31 §3)
    # secondo lotto (docs/ricerca/32): l'Ohio senza le pompe a bassa velocita' (circolazione naturale del S8G); il 206 coi
    # diesel solo a snorkel e la rete a 60 Hz (STIMA: non trovata; a 50 Hz si leggerebbe come sovietico)
    'ohio':        (USA + [R(31.0, -6, 'pompa', 10), R(62.0, -5, 'pompa', 18), R(88.0, -6, 'turbina'), R(132, -10, 'riduttore')],
                    (7, 1, 4.5), 'usa', 'ssbn'),
    'type206':     (USA + [R(70.0, -4, 'ausiliario'), R(16.5, 0, 'diesel'), R(33.0, -3, 'diesel'), R(49.5, -6, 'diesel')],
                    (5, 1, 12.0), 'germania', 'ssk'),   # 5 pale sul 206, 7 sul 206A (tre convertiti a fine 1989)
    'nacken':      (USA + [R(65.0, -4, 'ausiliario'), R(25.0, 0, 'diesel'), R(12.5, -3, 'diesel'), R(75.0, -6, 'diesel')],
                    (5, 1, 11.0), 'svezia', 'ssk'),   # il MTU 16V652 a 1.500 giri, i diesel solo a snorkel (STIMA)
    'kilo':        (URSS + [R(75.0, -4, 'ausiliario'), R(12.5, 0, 'diesel'), R(25.0, -3, 'diesel'), R(37.5, -6, 'diesel')],
                    (6, 1, 12.0), 'urss', 'ssk'),   # i diesel solo a snorkel
    'foxtrot':     (URSS + [R(68.0, -4, 'ausiliario'), R(11.0, 0, 'diesel'), R(22.0, -3, 'diesel'), R(33.0, -6, 'diesel')],
                    (6, 3, 14.0), 'urss', 'ssk'),
    'perry':       (USA + [R(115.0, -2, 'turbina'), R(230, -6, 'riduttore')], (5, 1, 6.0), 'usa', 'fregata'),
    'kidd':        (USA + [R(118.0, -2, 'turbina'), R(236, -6, 'riduttore')], (5, 2, 5.5), 'usa', 'cacciatorpediniere'),
    'spruance':    (USA + [R(118.0, -2, 'turbina'), R(177, -6, 'riduttore')], (5, 2, 5.5), 'usa', 'cacciatorpediniere'),
    'iowa':        (USA + [R(85.0, -2, 'turbina'), R(160, -6, 'riduttore')], (4, 4, 6.0), 'usa', 'corazzata'),
    'nimitz':      (USA + [R(82.0, -2, 'turbina'), R(41.0, -4, 'pompa'), R(164, -6, 'riduttore')], (5, 4, 6.0), 'usa', 'portaerei'),
    'sacramento':  (USA + [R(78.0, -2, 'turbina'), R(156, -6, 'riduttore')], (5, 2, 6.0), 'usa', 'rifornitrice'),
    'krivak2':     (URSS + [R(105.0, -2, 'turbina'), R(210, -6, 'riduttore')], (5, 2, 6.0), 'urss', 'fregata'),
    'kara':        (URSS + [R(110.0, -2, 'turbina'), R(165, -6, 'riduttore')], (5, 2, 5.5), 'urss', 'incrociatore'),
    'slava':       (URSS + [R(98.0, -2, 'turbina'), R(196, -6, 'riduttore')], (5, 2, 5.5), 'urss', 'incrociatore'),
    'koni':        (URSS + [R(14.0, -2, 'diesel'), R(28.0, -5, 'diesel'), R(125.0, -4, 'turbina')], (4, 3, 7.0), 'urss', 'fregata'),
    'nanuchka':    (URSS + [R(17.0, -2, 'diesel'), R(34.0, -5, 'diesel')], (4, 3, 9.0), 'urss', 'corvetta'),
    # la Hauk: i diesel veloci (STIMA: 1.800 giri, 30 Hz, e l'accensione a 150), eliche piccole e veloci; la rete a 60 Hz come
    # le altre NATO (a 50 l'analizzatore la leggerebbe sovietica: la stessa scelta della Type 206, dossier 32 §5)
    'hauk':        (USA + [R(30.0, -2, 'diesel'), R(150.0, -4, 'diesel')], (3, 2, 30.0), 'norvegia', 'pattugliatore'),
    # docs/ricerca/35 (STIMA): la Oslo come la Knox (turbina, riduttore, pompe d'alimento); la Stockholm coi MTU a 1.900 giri
    # e la turbina; la Kitty Hawk come la Nimitz piu' le caldaie; il Näcken come il 206. Le nordiche con la rete a 60 Hz
    # come le altre NATO (a 50 l'analizzatore le leggerebbe sovietiche, la scelta della Type 206)
    'oslo':        (USA + [R(98.0, -2, 'turbina'), R(196, -6, 'riduttore'), R(29.0, -4, 'pompa')], (5, 1, 7.0), 'norvegia', 'fregata'),
    'stockholm':   (USA + [R(31.7, -2, 'diesel'), R(63.3, -5, 'diesel'), R(140.0, -4, 'turbina')], (4, 3, 12.0), 'svezia',
                    'corvetta'),
    'kitty_hawk':  (USA + [R(80.0, -2, 'turbina'), R(160, -6, 'riduttore'), R(32.0, -3, 'pompa'), R(48.0, -5, 'ausiliario')],
                    (5, 4, 6.0), 'usa', 'portaerei'),
    'ticonderoga': (USA + [R(121.0, -2, 'turbina'), R(181.5, -6, 'riduttore')], (5, 2, 5.5), 'usa', 'incrociatore'),   # pale STIMA
    'knox':        (USA + [R(93.0, -2, 'turbina'), R(186, -6, 'riduttore'), R(28.0, -4, 'pompa')], (5, 1, 6.5), 'usa', 'fregata'),
    'type22':      (USA + [R(108.0, -2, 'turbina'), R(22.5, -1, 'diesel'), R(45.0, -4, 'diesel')], (5, 2, 6.0), 'uk',
                    'fregata'),   # i generatori diesel senza raft: righe forti (32 §4); lontane dai 50 Hz sovietici
    'type22b2':    (USA + [R(108.0, -2, 'turbina'), R(22.5, -1, 'diesel'), R(45.0, -4, 'diesel')], (5, 2, 6.0), 'uk',
                    'fregata'),   # come il Batch 1: la testimonianza dei diesel senza raft e' proprio del Batch 2 (32 §4)
    'udaloy':      (URSS + [R(102.0, -2, 'turbina'), R(204, -6, 'riduttore')], (5, 2, 5.5), 'urss', 'cacciatorpediniere'),   # 31 §4
    'grisha3':     (URSS + [R(21.0, -2, 'diesel'), R(42.0, -5, 'diesel'), R(130.0, -4, 'turbina')], (4, 3, 7.5), 'urss',
                    'corvetta'),   # 31 §5: diesel laterali, turbina centrale (frequenze proprie, lontane da AGI, Koni, Nanuchka)
    'kirov':       (URSS + [R(92.0, -2, 'turbina'), R(46.0, -4, 'pompa'), R(138, -6, 'riduttore')], (5, 2, 5.0), 'urss', 'incrociatore'),
    'mercantile':  (URSS + [R(8.3, 0, 'diesel'), R(16.7, -2, 'diesel'), R(25.0, -4, 'diesel')], (4, 1, 6.0), 'civile', 'mercantile'),
    'cargo':       (USA + [R(9.5, 0, 'diesel'), R(19.0, -2, 'diesel'), R(28.5, -4, 'diesel')], (6, 1, 5.5), 'civile', 'mercantile'),
    'ugra':        (URSS + [R(12.5, -1, 'diesel'), R(25.0, -3, 'diesel'), R(37.5, -5, 'diesel'), R(60.0, -6, 'ausiliario')],
                    (4, 2, 8.0), 'urss', 'rifornitrice'),   # STIMA: la base galleggiante, con le officine e i generatori
    'petroliera':  (USA + [R(7.2, 0, 'diesel'), R(14.4, -2, 'diesel'), R(21.6, -4, 'diesel')], (5, 1, 6.0), 'civile', 'petroliera'),
    'biologico':   ([R(20.0, 0, 'biologico'), R(40.0, -8, 'biologico')], (0, 0, 0.0), 'biologico', 'biologico'),
    # AGI: rete a 50 Hz e il generatore (6 cilindri a 500 giri: 8,3 e 25 Hz) sempre; il diesel principale (8 cilindri a
    # 4 tempi, ~230 giri a 9 nodi in presa diretta: 3,8 Hz di rotazione, 15,3 di scoppio) solo in moto; elica a 4 pale,
    # ~26 giri al minuto per nodo. STIMA: la riga del motore in presa diretta e' quella della crociera
    'agi':         (URSS + [R(25.0, -2, 'diesel'), R(8.3, -6, 'diesel'), R(15.3, 0, 'diesel', 1), R(30.6, -3, 'diesel', 1),
                            R(3.8, -8, 'diesel', 1)], (4, 1, 26.0), 'urss', 'agi'),
    # peschereccio: rete a 50 Hz, il diesel a giri costanti (12 cilindri a V a 750 giri: 12,5 Hz di rotazione, 75 Hz di
    # scoppio, l'ordine 1,5 a 112,5 Hz) e l'elica a passo variabile a giri costanti (~160 giri, 4 pale: la riga di pala
    # resta a 10,7 Hz a ogni velocita'; pale = 0, le righe sono qui). Al lavoro (2-5,5 kn): i cavi della rete che
    # vibrano (scie di vortici, ~14 Hz e armoniche per 2 m/s su cavi da 28 mm, St 0,2) e la pompa idraulica dei
    # verricelli (9 pistoni a 1500 giri, 225 Hz). STIMA
    'peschereccio': (URSS + [R(75.0, 0, 'diesel'), R(112.5, -5, 'diesel'), R(37.5, -6, 'diesel'), R(12.5, -5, 'diesel'),
                             R(10.7, -2, 'elica'), R(21.3, -6, 'elica'), R(32.0, -9, 'elica'),
                             R(14.0, -6, 'ausiliario', 2, 5.5), R(28.0, -10, 'ausiliario', 2, 5.5),
                             R(42.0, -14, 'ausiliario', 2, 5.5), R(225.0, -8, 'ausiliario', 2, 5.5)],
                     (0, 1, 0.0), 'civile', 'peschereccio'),
    # siluri (R7, note 18_note/transitori_armi_r7.md §2.2 e §6.3; STIMA da fisica elementare: giri n = k_n V[kn] al
    # secondo, righe di pala e del motore alle velocita' di corsa, livelli rispetto alla tonale della firma). Niente
    # riga di pala dalla formula (pale = 0): le righe sono qui, una serie per velocita'
    'mk48':        ([R(660, 2, 'elica', 0, 47), R(1320, -4, 'elica', 0, 47), R(132, -4, 'riduttore', 0, 47),
                     R(1188, -4, 'riduttore', 0, 47), R(264, -4, 'turbina', 0, 47),
                     R(907.5, 12, 'elica', 47), R(1815, 6, 'elica', 47), R(181.5, 6, 'riduttore', 47),
                     R(1633.5, 6, 'riduttore', 47), R(363, 6, 'turbina', 47)], (0, 1, 0.0), 'arma', 'siluro'),
    'mk48_adcap':  ([R(660, -1, 'elica', 0, 47), R(1320, -7, 'elica', 0, 47), R(264, -10, 'turbina', 0, 47),
                     R(907.5, 9, 'elica', 47), R(1815, 3, 'elica', 47), R(363, 0, 'turbina', 47)], (0, 1, 0.0), 'arma', 'siluro'),
    'mk46':        ([R(157, -10, 'elica', 0, 36), R(235, -10, 'elica', 0, 36), R(392, -16, 'elica', 0, 36),
                     R(252, 2, 'elica', 36), R(378, 2, 'elica', 36), R(630, -4, 'elica', 36)], (0, 1, 0.0), 'arma', 'siluro'),
    'type53_65k':  ([R(180, -2, 'elica'), R(360, -6, 'elica'), R(5000, 0, 'turbina')], (0, 1, 0.0), 'arma', 'siluro'),
    'uset80':      ([R(154, -12, 'elica', 0, 40), R(308, -16, 'elica', 0, 40), R(385, -14, 'turbina', 0, 40),
                     R(198, -2, 'elica', 40), R(396, -6, 'elica', 40), R(495, -4, 'turbina', 40)], (0, 1, 0.0), 'arma', 'siluro'),
    'test71m':     ([R(105.6, -12, 'elica', 0, 32), R(211, -16, 'elica', 0, 32), R(264, -14, 'turbina', 0, 32),
                     R(176, 0, 'elica', 32), R(352, -4, 'elica', 32), R(440, -2, 'turbina', 32)], (0, 1, 0.0), 'arma', 'siluro'),
    'at1':         ([R(140, -10, 'elica'), R(280, -14, 'elica'), R(350, -12, 'turbina')], (0, 1, 0.0), 'arma', 'siluro'),
}
SIGLE_ARMI = ['adc_mk2', 'mk48', 'mk48_adcap', 'tigerfish', 'spearfish', 'uset80', 'test71m', 'type53_65k', 'mk46', 'at1',
              'siluro_generico']

SOVIETICHE = {'victor3', 'alfa', 'akula', 'oscar', 'charlie2', 'typhoon', 'foxtrot', 'kilo', 'krivak2', 'kara', 'slava',
               'koni', 'nanuchka', 'kirov', 'mercantile', 'petroliera', 'agi', 'peschereccio',
               'victor1', 'delta3', 'sierra', 'udaloy', 'grisha3', 'ugra'}   # rete a 50 Hz

# id: campi del sensore (02 §1.2, §1.5, §2.5; STIMA)
SENSORI = {
    'bqq5_sfera': dict(modo='passivo', antenna='sfera', dimensione_m=4.6, bande_hz=[150, 300, 1000],
                       integrazione_s=10, indice_rilevazione=14, nl_macchina_db=45, nl_flusso_db=50, v0_flusso_kn=8,
                       velocita_max_kn=25, baffles_gradi=35, baffles_db=15, lofar_bin_hz=0.1, lofar_integrazione_s=60,
                       isolamento_proprio_db=30, nota='688: sfera BQS-13 del BQQ-5 in passivo a bassa frequenza (FAS) con il LOFAR; sorda oltre 20-25 kn'),
    'bqq5_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.6, bande_hz=[3500], indice_rilevazione=14,
                        nl_macchina_db=45, nl_flusso_db=50, v0_flusso_kn=8, velocita_max_kn=25, baffles_gradi=35,
                        sl_attivo_db=225, impulso_s=0.5, nota='688: BQQ-5 attivo, ~3,5 kHz, SL 220-230'),
    'tb16': dict(modo='passivo', antenna='lineare', dimensione_m=73, bande_hz=[50, 150, 300, 1000], integrazione_s=30,
                 indice_rilevazione=14, nl_macchina_db=35, nl_flusso_db=45, v0_flusso_kn=12, velocita_max_kn=17,
                 baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=120, endfire_gradi=25, conico=True,
                 lofar_bin_hz=0.05, lofar_integrazione_s=120, cavo_diametro_m=0.0094,
                 nota='schiera trainata TB-16, apertura 73 m; inutilizzabile oltre 15-18 kn; cieca in virata oltre '
                      '0,3 gradi/s, poi si raddrizza in 2 min (docs/07 §3.4); endfire verso il battello cieco'),
    'skat': dict(modo='passivo', antenna='lineare', dimensione_m=80, bande_hz=[50, 150, 300], integrazione_s=10,
                 indice_rilevazione=100, nl_macchina_db=50, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=14,
                 baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=150, endfire_gradi=25, conico=True,
                 lofar_bin_hz=0.1, lofar_integrazione_s=120, cavo_diametro_m=0.012,
                 nota='schiera trainata sovietica (Skat-KS «Barrakuda» del Victor III, «Pelamida» dell\'Akula): '
                      'dossier 04 §3 e §6.2, tutto STIMA; 200 m (80 di apertura efficace), 5-400 Hz, sorda verso 12-14 kn; '
                      'il rumore proprio e l\'elaborazione peggiori delle schiere USA (STIMA tarata perche\' il vantaggio di scoperta '
                      'fra le classi del dossier 24 §5.1 resti entro 4 dB)'),
    'wlr9': dict(modo='passivo', antenna='scafo', dimensione_m=0.3, bande_hz=[3500, 10000, 25000], integrazione_s=1,
                 indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=8, velocita_max_kn=30,
                 baffles_gradi=0, di_fisso_db=0,
                 nota='WLR-9: intercettazione degli impulsi attivi (contro-scoperta, 02 §5.3); idrofoni a larga apertura, '
                      'nessun guadagno d\'antenna (06 §2: STIMA)'),
    'intercettazione_sov': dict(modo='passivo', antenna='scafo', dimensione_m=0.3, bande_hz=[3500, 10000, 25000],
                                integrazione_s=1, indice_rilevazione=14, nl_macchina_db=55, nl_flusso_db=55,
                                v0_flusso_kn=8, velocita_max_kn=30, baffles_gradi=0, di_fisso_db=0,
                                nota='intercettazione sovietica (stesso modello del WLR-9, Enisej sull\'Alfa): STIMA'),
    'nave_sovietica': dict(modo='passivo', antenna='sfera', dimensione_m=4.0, bande_hz=[1000, 3500], integrazione_s=10,
                           indice_rilevazione=14, nl_macchina_db=64, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=30,
                           baffles_gradi=30, baffles_db=15, lofar_bin_hz=1.0, lofar_integrazione_s=30,
                           nota='navi sovietiche: sonar di prua in passivo, STIMA; LOFAR grossolano (1 Hz, 30 s: STIMA, '
                                'come i sonobe di prima generazione) per le righe forti delle navi veloci'),
    'sovietico_3gen': dict(modo='passivo', antenna='sfera', dimensione_m=4.0, bande_hz=[150, 300, 1000],
                           integrazione_s=10, indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=8,
                           velocita_max_kn=25, baffles_gradi=40, baffles_db=15, lofar_bin_hz=0.2, lofar_integrazione_s=60,
                           isolamento_proprio_db=28, nota='MGK-400/503 (Victor III, Akula, Oscar, Typhoon): STIMA; LOFAR piu\' grossolano'),
    'sovietico_2gen': dict(modo='passivo', antenna='sfera', dimensione_m=3.0, bande_hz=[300, 1000],
                           integrazione_s=10, indice_rilevazione=14, nl_macchina_db=56, nl_flusso_db=55, v0_flusso_kn=7,
                           velocita_max_kn=22, baffles_gradi=45, baffles_db=15,
                           lofar_bin_hz=1.0, lofar_integrazione_s=30, isolamento_proprio_db=24,
                           nota='MG-200/Kerch (Charlie, Foxtrot, Alfa): STIMA; LOFAR grossolano (1 Hz, 30 s: STIMA, come i '
                                'sonobe di prima generazione): le righe forti delle navi veloci passano, quelle dei sottomarini '
                                'quieti no'),
    'sovietico_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.0, bande_hz=[3500], indice_rilevazione=14,
                             nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=8, velocita_max_kn=25, baffles_gradi=40,
                             sl_attivo_db=222, impulso_s=0.5, nota='attivo dei sottomarini sovietici: STIMA'),
    'sqs53': dict(modo='passivo', antenna='sfera', dimensione_m=5.0, bande_hz=[1000, 3500], integrazione_s=10,
                  indice_rilevazione=14, nl_macchina_db=60, nl_flusso_db=56, v0_flusso_kn=10, velocita_max_kn=30,
                  baffles_gradi=30, baffles_db=15, lofar_bin_hz=1.0, lofar_integrazione_s=30,
                  nota='nave: sonar di prua in passivo (SQS-53/26); LOFAR grossolano (1 Hz, 30 s: STIMA, come i sonobe di '
                       'prima generazione)'),
    'sqs53_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=5.0, bande_hz=[3500], indice_rilevazione=14,
                         nl_macchina_db=60, nl_flusso_db=56, v0_flusso_kn=10, velocita_max_kn=30, baffles_gradi=30,
                         sl_attivo_db=235, impulso_s=1.0, nota='SQS-53 attivo, ~3 kHz, SL 235 nominale (02 §1.5)'),
    'sqs56_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=2.5, bande_hz=[10000], indice_rilevazione=14,
                         nl_macchina_db=60, nl_flusso_db=56, v0_flusso_kn=10, velocita_max_kn=30, baffles_gradi=30,
                         sl_attivo_db=222, impulso_s=0.5, nota='FFG-7: SQS-56, 6,8-8,2 kHz (banda di 10 kHz), SL 220-225'),
    'titan2_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.0, bande_hz=[3500], indice_rilevazione=14,
                          nl_macchina_db=62, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=30, baffles_gradi=30,
                          sl_attivo_db=228, impulso_s=0.5, nota='navi sovietiche: MG-332 Titan-2 e simili, 3-8 kHz'),
    # MGK-355 «Polinom» dell'Udaloy (docs/ricerca/31 §4): la cupola «Horse Jaw» di prua lunga circa 30 m, a bassa frequenza;
    # il «Horse Tail» a profondita' variabile e' polinom_vds (docs/ricerca/33), quindi la cupola ha il rumore proprio delle
    # navi sovietiche. Frequenze e livelli STIMA (non trovati)
    'polinom': dict(modo='passivo', antenna='sfera', dimensione_m=7.0, bande_hz=[300, 1000, 3500], integrazione_s=10,
                    indice_rilevazione=14, nl_macchina_db=64, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=30,
                    baffles_gradi=30, baffles_db=15, lofar_bin_hz=0.5, lofar_integrazione_s=60,
                    nota='Udaloy: MGK-355 Polinom in passivo, cupola di prua «Horse Jaw» a bassa frequenza (STIMA)'),
    'polinom_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=7.0, bande_hz=[3500], indice_rilevazione=14,
                           nl_macchina_db=64, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=30, baffles_gradi=30,
                           sl_attivo_db=232, impulso_s=1.0,
                           nota='Udaloy: MGK-355 Polinom attivo a bassa frequenza navale (2-4 kHz come l\'SQS-53), SL 232, '
                                'cupola grande (STIMA; «90 km» nel capitolato)'),
    # Sonar a profondita' variabile delle navi (docs/ricerca/33): il pesce al cavo, alla quota di nucleo/schiera
    # ([schiere.<passivo>]); lontano dalle macchine: niente via strutturale (isolamento 45 dB), macchinario 10 dB sotto
    # il sonar di prua, il flusso del pesce, sordo oltre la velocita' di traino. "vds" nell'id: il motore lo riconosce
    # cosi'. Tutto STIMA salvo le quote massime e le frequenze dell'SQS-35
    'sqs35_vds': dict(modo='passivo', antenna='sfera', dimensione_m=4.8, bande_hz=[3500, 10000], integrazione_s=10,
                      indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=10, velocita_max_kn=20,
                      baffles_gradi=0, isolamento_proprio_db=45,
                      nota='Knox: AN/SQS-35 IVDS in ascolto («listen»), pesce di 4,8 m fino a 180 m (33 §1)'),
    'sqs35_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.8, bande_hz=[10000], indice_rilevazione=14,
                             nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=10, velocita_max_kn=20, baffles_gradi=0,
                             sl_attivo_db=220, impulso_s=0.5,
                             nota='Knox: AN/SQS-35 attivo a 13 kHz (banda di 10 kHz), SL 220 (STIMA)'),
    'sqr18a': dict(modo='passivo', antenna='lineare', dimensione_m=200, bande_hz=[50, 150, 300, 1000], integrazione_s=30,
                   indice_rilevazione=14, nl_macchina_db=42, nl_flusso_db=50, v0_flusso_kn=12, velocita_max_kn=20,
                   baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=150, endfire_gradi=25, conico=True,
                   lofar_bin_hz=0.1, lofar_integrazione_s=120, cavo_diametro_m=0.02,
                   nota='Knox: AN/SQR-18A TACTAS trainata dal pesce dell\'SQS-35 (33 §1); misure STIMA'),
    'sqr19': dict(modo='passivo', antenna='lineare', dimensione_m=242, bande_hz=[50, 150, 300, 1000], integrazione_s=30,
                  indice_rilevazione=14, nl_macchina_db=40, nl_flusso_db=48, v0_flusso_kn=12, velocita_max_kn=25,
                  baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=150, endfire_gradi=25, conico=True,
                  lofar_bin_hz=0.05, lofar_integrazione_s=120, cavo_diametro_m=0.0254,
                  nota='Spruance, Perry, Ticonderoga: AN/SQR-19 TACTAS dell\'SQQ-89, 1.700-1.800 m di cavo, fino a '
                       '335-360 m (33 §1); rumore e LOFAR come le schiere USA, un po\' sopra la TB-16 (STIMA)'),
    'type2031z': dict(modo='passivo', antenna='lineare', dimensione_m=500, bande_hz=[50, 150, 300], integrazione_s=30,
                      indice_rilevazione=14, nl_macchina_db=46, nl_flusso_db=50, v0_flusso_kn=12, velocita_max_kn=20,
                      baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=180, endfire_gradi=25, conico=True,
                      lofar_bin_hz=0.05, lofar_integrazione_s=120, cavo_diametro_m=0.03,
                      nota='Type 22 Batch 2: Type 2031Z passiva VLF, circa 500 m (04 §4); il rumore proprio sopra la SQR-19 '
                           'per i diesel senza raft (32 §4); tutto il resto STIMA'),
    # le nordiche (docs/ricerca/35): la Oslo con l'SQS-36 di scafo (la famiglia dell'SQS-35, 13 kHz) e il Terne III; la
    # Stockholm col Simrad SA950 di scafo e il VDS Thomson-CSF TSM 2643 «Salmon». Frequenze e livelli STIMA
    'sqs36': dict(modo='passivo', antenna='sfera', dimensione_m=3.0, bande_hz=[3500, 10000], integrazione_s=10,
                  indice_rilevazione=14, nl_macchina_db=60, nl_flusso_db=56, v0_flusso_kn=10, velocita_max_kn=25,
                  baffles_gradi=30, baffles_db=15, nota='Oslo: AN/SQS-36 di scafo in ascolto (35); STIMA'),
    'sqs36_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=3.0, bande_hz=[10000], indice_rilevazione=14,
                         nl_macchina_db=60, nl_flusso_db=56, v0_flusso_kn=10, velocita_max_kn=25, baffles_gradi=30,
                         sl_attivo_db=218, impulso_s=0.5, nota='Oslo: SQS-36 attivo a 13 kHz (banda di 10 kHz), col Terne III (35)'),
    'sa950': dict(modo='passivo', antenna='sfera', dimensione_m=1.5, bande_hz=[10000, 25000], integrazione_s=10,
                  indice_rilevazione=14, nl_macchina_db=60, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=25,
                  baffles_gradi=30, baffles_db=15, nota='Stockholm: Simrad SA950 di scafo, alta frequenza (35); STIMA'),
    'sa950_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=1.5, bande_hz=[25000], indice_rilevazione=14,
                         nl_macchina_db=60, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=25, baffles_gradi=30,
                         sl_attivo_db=212, impulso_s=0.2, nota='Stockholm: SA950 attivo (35); STIMA'),
    'salmon_vds': dict(modo='passivo', antenna='sfera', dimensione_m=1.5, bande_hz=[3500, 10000], integrazione_s=10,
                       indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=18,
                       baffles_gradi=0, isolamento_proprio_db=45,
                       nota='Stockholm: TSM 2643 «Salmon», VDS leggero (35); STIMA'),
    'salmon_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=1.5, bande_hz=[10000], indice_rilevazione=14,
                              nl_macchina_db=50, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=18, baffles_gradi=0,
                              sl_attivo_db=215, impulso_s=0.3, nota='Stockholm: Salmon attivo (35); STIMA'),
    'polinom_vds': dict(modo='passivo', antenna='sfera', dimensione_m=4.0, bande_hz=[1000, 3500], integrazione_s=10,
                        indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=20,
                        baffles_gradi=0, isolamento_proprio_db=45, lofar_bin_hz=0.5, lofar_integrazione_s=60,
                        nota='Udaloy: «Horse Tail», il VDS del Polinom (33 §1), fino a 200 m (STIMA)'),
    'polinom_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.0, bande_hz=[3500], indice_rilevazione=14,
                               nl_macchina_db=54, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=20, baffles_gradi=0,
                               sl_attivo_db=226, impulso_s=1.0, nota='Udaloy: «Horse Tail» attivo (STIMA)'),
    'vega_vds': dict(modo='passivo', antenna='sfera', dimensione_m=2.5, bande_hz=[3500, 10000], integrazione_s=10,
                     indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18,
                     baffles_gradi=0, isolamento_proprio_db=45,
                     nota='Kara, Krivak II: MG-325 «Vega» («Mare Tail»), a poppa, fino a 100 m (33 §1)'),
    'vega_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=2.5, bande_hz=[3500], indice_rilevazione=14,
                            nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18, baffles_gradi=0,
                            sl_attivo_db=222, impulso_s=0.5, nota='MG-325 «Vega» attivo, media frequenza (STIMA)'),
    'bronza_vds': dict(modo='passivo', antenna='sfera', dimensione_m=3.0, bande_hz=[1000, 3500], integrazione_s=10,
                       indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18,
                       baffles_gradi=0, isolamento_proprio_db=45,
                       nota='Krivak III: MG-345 «Bronza» rimorchiata, come VDS (STIMA dichiarata, 33 §1: le fonti '
                            'discordano); nessuna classe del gioco per ora'),
    'shelon_vds': dict(modo='passivo', antenna='sfera', dimensione_m=1.5, bande_hz=[3500, 10000], integrazione_s=10,
                       indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=55, v0_flusso_kn=3, velocita_max_kn=3,
                       baffles_gradi=0, isolamento_proprio_db=45,
                       nota='Grisha III: «Shelon» («Elk Tail»), calabile a nave ferma (31 §5), fino a 150 m (STIMA)'),
    'shelon_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=1.5, bande_hz=[10000], indice_rilevazione=14,
                              nl_macchina_db=50, nl_flusso_db=55, v0_flusso_kn=3, velocita_max_kn=3, baffles_gradi=0,
                              sl_attivo_db=215, impulso_s=0.1, nota='«Shelon» attivo (STIMA)'),
}

# Boe e sonar a immersione dell'ASW aereo (docs/13_asw_aereo.md, A2; dossier 17 §4.1-4.2, §6.4-6.5): ricevitori della
# stessa equazione del sonar (nucleo/acustica_asw.hpp). Nessun macchinario a bordo: il rumore proprio e' quello della
# deriva. Le americane si ascoltano sull'aereo con l'analisi LOFAR (bin e tempi STIMA); le sovietiche a orecchio
# (integrazione breve, niente LOFAR). di_fisso_db: guadagno dell'antenna direzionale (DIFAR cardioide, VLAD verticale),
# STIMA salvo nota; sigma_rilevamento_deg: errore misurato in mare. I sonar a immersione hanno 12 dB e la DICASS 10
# (STIMA): con questi l'asimmetria del §8.3 (il ping si sente 7-12 volte piu' lontano della scoperta dell'eco) esce
# dall'equazione (test/prove_acustica_asw.cpp).
# mare_proprio_db = 0: il rumore del mare proprio (rumore_mare_proprio_db) non vale per boe e dip, che il mare tocca gia'
# con mare_max delle boe e col dip vietato col mare forte (24 §7.4 lo da' ai soli sonar di scafo e schiere)
_BOA = dict(antenna='sfera', dimensione_m=0.124, integrazione_s=10, indice_rilevazione=14, nl_macchina_db=20,
            nl_flusso_db=40, v0_flusso_kn=8, velocita_max_kn=30, baffles_gradi=0, baffles_db=0, mare_proprio_db=0)
_DIP = dict(antenna='sfera', dimensione_m=0.5, di_fisso_db=12, integrazione_s=10, indice_rilevazione=14, nl_macchina_db=30,
            nl_flusso_db=40, v0_flusso_kn=8, velocita_max_kn=30, baffles_gradi=0, baffles_db=0, mare_proprio_db=0)
BOE = {
    'ssq41b': dict(_BOA, modo='passivo', bande_hz=[50, 150, 300, 1000, 3500], gamma_hz=[10, 10000], di_fisso_db=0, omni=True,
                   lofar_bin_hz=1.0, lofar_integrazione_s=60, nota='LOFAR omni, 10 Hz-10 kHz: solo «contatto nel cerchio» (17 §4.1)'),
    'ssq53b': dict(_BOA, modo='passivo', bande_hz=[50, 150, 300, 1000], gamma_hz=[10, 2400], di_fisso_db=3, sigma_rilevamento_deg=7,
                   lofar_bin_hz=1.0, lofar_integrazione_s=60,
                   nota='DIFAR 10 Hz-2,4 kHz; sigma del rilevamento 5-9 gradi in mare (17 §4.1 [AAD-DIFAR])'),
    'ssq77a': dict(_BOA, modo='passivo', bande_hz=[50, 150, 300, 1000], gamma_hz=[10, 2400], di_fisso_db=15, sigma_rilevamento_deg=7,
                   lofar_bin_hz=1.0, lofar_integrazione_s=60,
                   nota='VLAD: DIFAR a schiera verticale, +10-15 dB contro il traffico orizzontale (17 §4.1 [HOLLER])'),
    'ssq57a': dict(_BOA, modo='passivo', bande_hz=[50, 150, 300, 1000], di_fisso_db=0, omni=True,
                   nota='passiva tarata: misura del rumore ambiente (17 §4.1)'),
    'ssq47b': dict(_BOA, modo='attivo', bande_hz=[16000], di_fisso_db=3, omni=True, sl_attivo_db=195, impulso_s=0.1,
                   sigma_distanza_m=50, nota='attiva, solo distanza: CW ogni 10 s a 13-19 kHz (17 §4.1); SL e durata STIMA'),
    'ssq62b': dict(_BOA, modo='attivo', bande_hz=[8500], di_fisso_db=10, sigma_rilevamento_deg=5, sl_attivo_db=200,
                   impulso_s=0.25, risoluzione_s=0.01, sigma_distanza_m=30,
                   nota='DICASS 6,5-9,5 kHz a comando: distanza, rilevamento, Doppler (17 §4.1, §8.3: 200 dB); in FM la '
                        'compressione da\' una risoluzione di ~10 ms (banda 100 Hz, STIMA) per la riverberazione'),
    'rgb_nm': dict(_BOA, modo='passivo', bande_hz=[8500], di_fisso_db=0, omni=True, integrazione_s=1,
                   nota='RGB-NM e NM-1: 7-10 kHz a orecchio, con la soglia (17 §4.2 [ART-Be12] [CIA72])'),
    'rgb1': dict(_BOA, modo='passivo', bande_hz=[300, 1000, 3500, 10000], di_fisso_db=0, omni=True, integrazione_s=2,
                 nota='RGB-1: schiera di 5 idrofoni, omni, con la soglia (17 §4.2); bande STIMA'),
    'rgb2': dict(_BOA, modo='passivo', bande_hz=[300, 1000, 3500], di_fisso_db=8, sigma_rilevamento_deg=6,
                 integrazione_s=2, nota='RGB-2: antenna di 13 elementi che ruota a 8 giri/min, errore 5-7 gradi (17 §4.2)'),
    'rgb75': dict(_BOA, modo='passivo', bande_hz=[20, 50], gamma_hz=[2, 60], di_fisso_db=0, omni=True, integrazione_s=60,
                  lofar_bin_hz=0.05, lofar_integrazione_s=600,
                  nota='RGB-75: infrasonora 2-60 Hz, continua fino a 5 h (17 §4.2 [ART-Tu142]): righe di pala in un LOFAR '
                       'stretto (bin 0,05 Hz in 10 min, STIMA del trattamento del Korshun)'),
    'rgb15': dict(_BOA, modo='passivo', bande_hz=[50, 150, 300, 1000, 3500], gamma_hz=[2, 5000], di_fisso_db=0, omni=True, integrazione_s=2,
                  nota='RGB-15: 2-5000 Hz, anche ricevitore dell\'eco esplosiva (17 §4.2)'),
    'rgb25': dict(_BOA, modo='passivo', bande_hz=[150, 300, 1000, 3500], di_fisso_db=6, sigma_rilevamento_deg=6,
                  integrazione_s=2, nota='RGB-25: direzionale, idrofono fino a 150 m (17 §4.2); bande e guadagno STIMA'),
    'rgb55a': dict(_BOA, modo='attivo', bande_hz=[8500], di_fisso_db=0, omni=True, sl_attivo_db=200, impulso_s=0.1,
                   sigma_distanza_m=90, nota='RGB-55A: attiva omni comandata, errore di distanza 75-100 m (17 §4.2); '
                                         'frequenza e SL STIMA'),
    # sonar a immersione: l'attivo e il passivo (stessa antenna; il passivo e' debole)
    'aqs13b': dict(_DIP, modo='attivo', bande_hz=[10000], sl_attivo_db=211, impulso_s=0.035, sigma_distanza_m=25,
                   nota='AQS-13B (SH-3H): ~10 kHz, >= 211 dB, 3,5 o 35 ms (17 §6.4, §8.3 [MILAQS13B])'),
    'aqs13b_passivo': dict(_DIP, modo='passivo', bande_hz=[10000], nota='AQS-13B in passivo: banda stretta a 10 kHz, '
                                                                         'solo contro la cavitazione vicina (17 §6.4)'),
    'aqs13f': dict(_DIP, modo='attivo', bande_hz=[10000], sl_attivo_db=215, impulso_s=0.035, sigma_distanza_m=25,
                   nota='AQS-13F (SH-60F): 215 dB come nella tabella dell\'asimmetria (17 §8.3)'),
    'aqs13f_passivo': dict(_DIP, modo='passivo', bande_hz=[10000], nota='AQS-13F in passivo (17 §6.4)'),
    'vgs3': dict(_DIP, modo='attivo', bande_hz=[8500], sl_attivo_db=205, impulso_s=0.1, sigma_distanza_m=50,
                 nota='VGS-3 (Ka-27PL): l\'attivo solo con un contatto (17 §6.5); frequenza e SL STIMA'),
    'vgs3_passivo': dict(_DIP, modo='passivo', bande_hz=[1000, 3500], dimensione_m=0.6,
                         nota='VGS-3 in passivo: un giro <= 30 s per 3-10 min (17 §6.5); bande STIMA'),
    'vgs2': dict(_DIP, modo='attivo', bande_hz=[8000], sl_attivo_db=200, impulso_s=0.1, sigma_distanza_m=60,
                 nota='VGS-2 (Ka-25): STIMA'),
    'vgs2_passivo': dict(_DIP, modo='passivo', bande_hz=[1000, 3500], nota='VGS-2 in passivo: STIMA'),
}
SENSORI.update(BOE)


# classe -> sensori di bordo (le navi senza sonar e i mercantili non ne hanno)
OCCIDENTALI = ['bqq5_sfera', 'tb16', 'bqq5_attivo', 'wlr9']
SOV_3 = ['sovietico_3gen', 'sovietico_attivo', 'intercettazione_sov']
SOV_2 = ['sovietico_2gen', 'sovietico_attivo', 'intercettazione_sov']
SUITE = {
    'los_angeles': OCCIDENTALI, 'sturgeon': OCCIDENTALI, 'trafalgar': OCCIDENTALI,
    # la schiera sovietica (blocco 2 del sonar, per la simmetria di D): Victor III, Akula; Oscar e Typhoon col punto
    # d'uscita non documentato (dossier 04 §3); il Kilo no
    'victor3': SOV_3 + ['skat'], 'akula': SOV_3 + ['skat'], 'oscar': SOV_3 + ['skat'], 'typhoon': SOV_3 + ['skat'],
    'kilo': SOV_3,
    'alfa': SOV_2, 'charlie2': SOV_2, 'foxtrot': SOV_2,
    # primo lotto (docs/ricerca/31): il Rubin del Victor I e' di 2a generazione; Rubikon e Skat di 3a, senza la schiera (del
    # Sierra I non confermata)
    'victor1': SOV_2, 'delta3': SOV_3, 'sierra': SOV_3,
    # secondo lotto (docs/ricerca/32): l'Ohio come il 688 (BQQ-6, TB-16); il DBQS-21 del 206 senza un sensore suo, per ora i
    # numeri del passivo di 2a generazione (STIMA); le navi col sonar di prua e l'attivo (SQS-53, SQS-26, Type 2016: STIMA)
    'ohio': OCCIDENTALI, 'type206': SOV_2,
    # i sensori fuori scafo delle navi (docs/ricerca/33): la SQR-19 sulle navi dell'SQQ-89, il VDS e la SQR-18A sulla Knox,
    # i VDS sovietici (Horse Tail, Vega, Shelon); a tutta la classe (STIMA: le date per unita' non si sono trovate)
    'ticonderoga': ['sqs53', 'sqs53_attivo', 'sqr19'], 'type22': ['sqs53', 'sqs53_attivo'],
    'type22b2': ['sqs53', 'sqs53_attivo', 'type2031z'],   # la 2031Z solo sul Batch 2 (04 §4, 33 §1)
    'knox': ['sqs53', 'sqs53_attivo', 'sqs35_vds', 'sqs35_vds_attivo', 'sqr18a'],
    'udaloy': ['polinom', 'polinom_attivo', 'polinom_vds', 'polinom_vds_attivo'],
    'grisha3': ['nave_sovietica', 'titan2_attivo', 'shelon_vds', 'shelon_vds_attivo'],   # Platina del Grisha: come il Titan-2
    'perry': ['sqs53', 'sqs56_attivo', 'sqr19'], 'kidd': ['sqs53', 'sqs53_attivo'], 'spruance': ['sqs53', 'sqs53_attivo', 'sqr19'],
    'krivak2': ['nave_sovietica', 'titan2_attivo', 'vega_vds', 'vega_vds_attivo'],
    'kara': ['nave_sovietica', 'titan2_attivo', 'vega_vds', 'vega_vds_attivo'],
    'slava': ['nave_sovietica', 'titan2_attivo'], 'kirov': ['nave_sovietica', 'titan2_attivo'],
    'koni': ['nave_sovietica', 'titan2_attivo'], 'nanuchka': ['nave_sovietica'],
    'ugra': ['nave_sovietica'],
    'oslo': ['sqs36', 'sqs36_attivo'], 'stockholm': ['sa950', 'sa950_attivo', 'salmon_vds', 'salmon_vds_attivo'],
    'nacken': SOV_2,   # il CSU-83 (DBQS-21 e FAS-3) come il 206: i numeri del passivo di 2a generazione (STIMA)   # la base galleggiante: nessun sonar di rilievo noto (STIMA)
}


# Schiere trainate (blocco 1 del sonar, D): dossier 04 §1.5 (catenaria di Pode, cavo della TB-16 da 9,4 mm e 2 N/m in
# acqua) e §6.2 (parametri per sistema; (S) = STIMA DI GIOCO). La chiave e' il sensore quando c'e' (tb16), se no il
# sistema. nucleo/schiera.hpp
SCHIERE = {
    'tb16': dict(schiera_m=73, cavo_max_m=790, cavo_diametro_m=0.0094, peso_cavo_n_m=2.0, cn=1.2, v_flessione_kn=1.2,
                 filata_m_s=0.9, recupero_m_s=0.75, v_manovra_kn=12, v_max_kn=20, quota_max_m=700,
                 fonte='04 §1.5 [FAS-TB16] cavo e peso; §6.2 velocita\' e quota (S)'),
    'tb23': dict(schiera_m=290, cavo_max_m=2300, cavo_diametro_m=0.0094, peso_cavo_n_m=1.5, cn=1.2, v_flessione_kn=0.6,
                 filata_m_s=1.0, recupero_m_s=0.85, v_manovra_kn=10, v_max_kn=16, quota_max_m=700,
                 fonte='04 §6.2 (S): thin line, v0 circa 1 kn'),
    'skat': dict(schiera_m=200, cavo_max_m=800, cavo_diametro_m=0.012, peso_cavo_n_m=3.0, cn=1.2, v_flessione_kn=1.2,
                 filata_m_s=0.8, recupero_m_s=0.7, v_manovra_kn=10, v_max_kn=18, quota_max_m=600,
                 fonte='04 §3 e §6.2 (S): Skat-KS, Skat-3'),
    'sqr19': dict(schiera_m=242, cavo_max_m=1700, cavo_diametro_m=0.0254, peso_cavo_n_m=20.0, cn=1.2, v_flessione_kn=1.2,
                  filata_m_s=1.2, recupero_m_s=1.0, v_manovra_kn=15, v_max_kn=25, quota_max_m=360,
                  fonte='04 §1.5 e §4 (S): cavo da 1 pollice, 380 m di discesa a 10 kn'),
    'type2031z': dict(schiera_m=500, cavo_max_m=1500, cavo_diametro_m=0.03, peso_cavo_n_m=25.0, cn=1.2, v_flessione_kn=1.2,
                      filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=300,
                      fonte='04 §4 (circa 500 m, VLF); cavo, velocita\' e quota (S)'),
    'sqr18a': dict(schiera_m=200, cavo_max_m=1500, cavo_diametro_m=0.02, peso_cavo_n_m=10.0, cn=1.2, v_flessione_kn=1.2,
                   filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=360,
                   fonte='33 §1 e §2 (S): trainata dal pesce dell\'SQS-35, misure non trovate'),
    # i VDS delle navi (33 §2): il pesce e' una schiera corta al cavo; il peso del pesce e del depressore e' spalmato sul
    # cavo (peso equivalente, STIMA), cosi' la legge di Pode da' la quota che sale con la velocita'. Le quote massime
    # dalle fonti (SQS-35 180 m, Vega 100 m), il resto STIMA
    'sqs35_vds': dict(schiera_m=4.8, cavo_max_m=250, cavo_diametro_m=0.04, peso_cavo_n_m=280.0, cn=1.0, v_flessione_kn=0.1,
                      filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=180,
                      fonte='33 §1 (Knox-class: fino a 600 ft, 180 m); cavo, peso e velocita\' (S)'),
    'polinom_vds': dict(schiera_m=5.0, cavo_max_m=300, cavo_diametro_m=0.04, peso_cavo_n_m=250.0, cn=1.0, v_flessione_kn=0.1,
                        filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=200,
                        fonte='33 §1 e §2 (S): «Horse Tail», nessun numero trovato'),
    'salmon_vds': dict(schiera_m=2.0, cavo_max_m=150, cavo_diametro_m=0.03, peso_cavo_n_m=150.0, cn=1.0, v_flessione_kn=0.1,
                       filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=10, v_max_kn=18, quota_max_m=120,
                       fonte='35 (S): il VDS leggero della Stockholm, nessun dato trovato'),
    'vega_vds': dict(schiera_m=3.0, cavo_max_m=150, cavo_diametro_m=0.04, peso_cavo_n_m=150.0, cn=1.0, v_flessione_kn=0.1,
                     filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=18, quota_max_m=100,
                     fonte='33 §1 (Kara-class: fino a 100 m); cavo, peso e velocita\' (S)'),
    'bronza_vds': dict(schiera_m=4.0, cavo_max_m=200, cavo_diametro_m=0.04, peso_cavo_n_m=200.0, cn=1.0, v_flessione_kn=0.1,
                       filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=18, quota_max_m=150,
                       fonte='33 §1 (S): come VDS per decisione della principale, le fonti discordano'),
    'shelon_vds': dict(schiera_m=1.5, cavo_max_m=200, cavo_diametro_m=0.03, peso_cavo_n_m=400.0, cn=1.0, v_flessione_kn=0.1,
                       filata_m_s=1.5, recupero_m_s=1.2, v_manovra_kn=0.5, v_max_kn=3, quota_max_m=150,
                       fonte='31 §5 e 33 §1 (S): calabile a nave ferma'),
}


# Intercettazione acustica (blocco 2 del sonar, E; dossier 28 §1-3). Bande [f_lo, f_hi, errore del rilevamento in
# gradi]; tutto STIMA salvo dove il dossier cita una fonte: del WLR-9 non c'e' nessun numero aperto, i sovietici da
# Kazakov 2019 («Skat» misura anche la deviazione FM, «Rubin» solo frequenza e rilevamento, coi falsi allarmi)
INTERCETTATORI = {
    'wlr9': dict(bande=[[1000, 6000, 12], [6000, 15000, 6], [15000, 45000, 6], [45000, 100000, 8]], soglia_db=10,
                 risoluzione_hz=100, misura_durata=True, misura_fm=False, falsi_allarmi_h=0.1,
                 classi=['los_angeles', 'sturgeon', 'trafalgar', 'ohio'], fonte='28 §1.3 (STIMA); Military Periscope'),
    'skat_ogs': dict(bande=[[1500, 12000, 8], [12000, 60000, 6]], soglia_db=10, risoluzione_hz=100, misura_durata=True,
                 misura_fm=True, falsi_allarmi_h=0.5, classi=['victor3', 'akula', 'oscar', 'typhoon', 'sierra'],
                 fonte='28 §2 (Kazakov 2019: Skat e Skat-3, falsi allarmi dal proprio battello)'),
    'rubikon': dict(bande=[[1500, 12000, 12], [12000, 40000, 10]], soglia_db=12, risoluzione_hz=200, misura_durata=True,
                    misura_fm=False, falsi_allarmi_h=1.0, classi=['kilo', 'delta3'], fonte='28 §2 (MGK-400 «Rubikon»: STIMA)'),
    'rubin': dict(bande=[[2000, 10000, 15], [10000, 40000, 15]], soglia_db=12, risoluzione_hz=300, misura_durata=False,
                  misura_fm=False, falsi_allarmi_h=2.0, classi=['alfa', 'charlie2', 'foxtrot', 'victor1'],
                  fonte='28 §2 (Kazakov 2019: «Rubin», 2 bande, molti falsi allarmi); l\'Alfa come il «Rubin» (STIMA)'),
}
# Le emissioni attive (dossier 28 §3.1): la chiave e' il sensore attivo che emette, piu' il BQS-15; le teste dei siluri
# vengono dai [transitori.ping_testa_*]. pri_s: la cadenza tipica
EMISSIONI = {
    'sqs53_attivo': dict(tipo='scafo', f_hz=3300, durata_s=1.5, fm=True, pri_s=25),       # 2,6-3,3 kHz, FM 1-2 s, 20-30 s
    'sqs56_attivo': dict(tipo='scafo', f_hz=7500, durata_s=0.1, fm=False, pri_s=20),      # variante estera, STIMA
    'titan2_attivo': dict(tipo='scafo', f_hz=5000, durata_s=0.5, fm=False, pri_s=15),     # STIMA 3-8 kHz
    'sqs36_attivo': dict(tipo='scafo', f_hz=13000, durata_s=0.5, fm=False, pri_s=15),     # la famiglia dell'SQS-35 (35)
    'sa950_attivo': dict(tipo='scafo', f_hz=24000, durata_s=0.2, fm=False, pri_s=5),      # STIMA
    'salmon_vds_attivo': dict(tipo='scafo', f_hz=12000, durata_s=0.3, fm=False, pri_s=10),  # STIMA
    'sqs35_vds_attivo': dict(tipo='scafo', f_hz=13000, durata_s=0.5, fm=False, pri_s=15),  # 13 kHz (33 §1)
    'polinom_vds_attivo': dict(tipo='scafo', f_hz=3000, durata_s=1.0, fm=False, pri_s=25), # STIMA
    'vega_vds_attivo': dict(tipo='scafo', f_hz=7000, durata_s=0.5, fm=False, pri_s=15),    # STIMA, media frequenza
    'shelon_vds_attivo': dict(tipo='scafo', f_hz=10000, durata_s=0.1, fm=False, pri_s=10), # STIMA, come i calabili
    'bqq5_attivo': dict(tipo='scafo', f_hz=3500, durata_s=0.5, fm=False, pri_s=60),
    'sovietico_attivo': dict(tipo='scafo', f_hz=4500, durata_s=0.3, fm=False, pri_s=60),  # STIMA
    'bqs15': dict(tipo='ricerca_hf', f_hz=25000, durata_s=0.01, fm=False, pri_s=2),        # «> 10 kHz»
    'ssq47b': dict(tipo='boa', f_hz=16000, durata_s=0.1, fm=False, pri_s=10),             # 13-19 kHz, CW ogni 10 s
    'ssq62b': dict(tipo='boa', f_hz=8000, durata_s=0.1, fm=True, pri_s=10),               # DICASS 6,5-9,5 kHz
    'rgb55a': dict(tipo='boa', f_hz=10000, durata_s=0.1, fm=False, pri_s=10),             # STIMA
    'aqs13b': dict(tipo='immersione', f_hz=10000, durata_s=0.035, fm=False, pri_s=8),     # 9,2-10,8 kHz, 3,5 o 35 ms
    'aqs13f': dict(tipo='immersione', f_hz=10000, durata_s=0.035, fm=False, pri_s=8),
    'vgs3': dict(tipo='immersione', f_hz=8000, durata_s=0.05, fm=False, pri_s=8),         # STIMA
    'vgs2': dict(tipo='immersione', f_hz=9000, durata_s=0.05, fm=False, pri_s=8),         # STIMA
    'fathometer': dict(tipo='scafo', f_hz=12000, durata_s=0.01, fm=False, pri_s=60),      # ecoscandaglio, 06 §1.2 (STIMA)
    # l'MG-70 / MT-70 dei Victor III e degli Akula, l'MG-24 dell'Alfa: il sonar di prua per mine, ghiaccio e ostacoli
    # (docs/08 §3: nessun dato pubblicato; frequenza e cadenza come il BQS-15, STIMA)
    'mg70': dict(tipo='ricerca_hf', f_hz=20000, durata_s=0.01, fm=False, pri_s=5),
    # il BQS-8 degli Sturgeon (docs/ricerca/34): sonar da ghiaccio ad alta frequenza, nessun dato pubblicato (STIMA)
    'bqs8': dict(tipo='ricerca_hf', f_hz=40000, durata_s=0.005, fm=False, pri_s=5),
}

# L'ecoscandaglio dei sottomarini (quadro del fondale, docs/24 §2.1; dossier 06 §1.2 e §7.1): un solo modello per
# tutti, per analogia con l'AN/UQN-4 di superficie (12 kHz); i valori del fathometer dei 688 e dei sovietici non sono
# pubblici: STIMA DI GIOCO
ECOSCANDAGLI = {
    'fathometer': dict(cono_deg=30, portata_piena_m=6000, portata_bassa_m=2000, sl_piena_db=210, sl_bassa_db=190,
                       f_hz=12000, errore_scala=0.01, rumore_rel=0.002, falso_fondo=0.02, dsl_min_m=300, dsl_max_m=500,
                       scia_kn=20, falso_scia=0.10,
                       fonte='06 §1.2: AN/UQN-4A per analogia (12 kHz), cono di 30 gradi e Bowditch §2605-2608; '
                             'livelli, portate e falsi fondi STIMA', confidenza='bassa'),
}


# Il BQS-15 sul fondo e sulle pareti (quadro del fondale, docs/24 §2.2; dossier 06 §1.1 e §7.1): portate dal calcolo del
# dossier (f = 25 kHz, SL 205 dB, NL mare 3, DI 20, DT 10: pareti ~3,2 km), il fondo radente e il degrado con la velocita'
# STIMA. I sovietici hanno MG-70 / MT-70 (Victor, Akula) e MG-24 (Alfa) senza dati pubblicati: le portate del BQS-15
# ridotte (docs/08 §3, STIMA)
BQS15_FONDO = {
    'bqs15': dict(settore_az_deg=60, elevazione_min_deg=-30, elevazione_max_deg=45, passo_az_deg=7.5,
                  portata_parete_m=3200, portata_fondo_m=1500, pendenza_parete=0.3, velocita_media_kn=15,
                  fattore_media=0.7, velocita_alta_kn=20, fattore_alta=0.4, errore_angolo_deg=1, errore_distanza_m=5,
                  passo_m=25, margine_ostacolo_m=30, fattore_portata=1.0, mine=True, emissione='bqs15', classi=['los_angeles'],
                  fonte='06 §1.1: calcolo di portata del compilatore (pareti 3,2 km); fondo radente e degrado STIMA',
                  confidenza='bassa'),
    'mg70': dict(settore_az_deg=60, elevazione_min_deg=-30, elevazione_max_deg=45, passo_az_deg=7.5,
                 portata_parete_m=3200, portata_fondo_m=1500, pendenza_parete=0.3, velocita_media_kn=15,
                 fattore_media=0.7, velocita_alta_kn=20, fattore_alta=0.4, errore_angolo_deg=1.5, errore_distanza_m=8,
                 passo_m=25, margine_ostacolo_m=30, fattore_portata=0.7, mine=True, emissione='mg70',
                 classi=['victor3', 'akula', 'victor1', 'sierra'],
                 fonte='docs/08 §3: MG-70 / MT-70, nessun dato pubblicato; le portate del BQS-15 ridotte (STIMA)',
                 confidenza='bassa'),
    'mg24': dict(settore_az_deg=60, elevazione_min_deg=-30, elevazione_max_deg=45, passo_az_deg=7.5,
                 portata_parete_m=3200, portata_fondo_m=1500, pendenza_parete=0.3, velocita_media_kn=15,
                 fattore_media=0.7, velocita_alta_kn=20, fattore_alta=0.4, errore_angolo_deg=1.5, errore_distanza_m=8,
                 passo_m=25, margine_ostacolo_m=30, fattore_portata=0.6, mine=True, emissione='mg70', classi=['alfa'],
                 fonte='docs/08 §3: MG-24 «Luč» dell\'Alfa, sonar per mine; portate ridotte (STIMA)', confidenza='bassa'),
    # il BQS-8 degli Sturgeon (docs/ricerca/34): il sonar da ghiaccio che guarda in alto e avanti, per le chiglie; non vede
    # le mine (nessuna fonte aperta lo dice: le vedevano il BQS-14 della sola Lipscomb, il BQS-15 dei 688). Niente fondo
    # (dallo zenit all'orizzonte); portata meta' del BQS-15, settore piu' stretto: STIMA
    'bqs8': dict(settore_az_deg=45, elevazione_min_deg=0, elevazione_max_deg=60, passo_az_deg=7.5,
                 portata_parete_m=1600, portata_fondo_m=750, pendenza_parete=0.3, velocita_media_kn=15,
                 fattore_media=0.7, velocita_alta_kn=20, fattore_alta=0.4, errore_angolo_deg=1.5, errore_distanza_m=8,
                 passo_m=25, margine_ostacolo_m=30, fattore_portata=0.5, mine=False, emissione='bqs8', classi=['sturgeon'],
                 fonte='34: AN/BQS-8 degli Sturgeon (Navypedia); portate, settore e frequenza STIMA', confidenza='bassa'),
}

def valore(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if v is None: return '""'
    if isinstance(v, str): return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list): return '[' + ', '.join(valore(x) for x in v) + ']'
    if isinstance(v, tuple): return '[' + ', '.join(valore(x) for x in v) + ']'
    return repr(float(v)) if isinstance(v, (int, float)) else str(v)


def valore_esatto(v):   # come valore, ma gli interi restano interi (mesi, armoniche, gruppi)
    if isinstance(v, bool) or not isinstance(v, (int, list, tuple)): return valore(v)
    if isinstance(v, (list, tuple)): return '[' + ', '.join(valore_esatto(x) for x in v) + ']'
    return str(v)


def tabella(d):   # tabella in linea TOML
    return '{ ' + ', '.join(f'{k} = {valore_esatto(v)}' for k, v in d.items()) + ' }'


def esporta_acustica(radice):
    righe = ['# Firme di rumore e sensori dell\'Enhanced (generato da tools/pipeline/acustica.py: non modificare a mano).', '']
    hz = lambda c: 250.0 if c in SOVIETICHE else 300.0

    def analisi(c):
        if c in ANALISI:
            righe, (pale, assi, giri), naz, tipo = ANALISI[c]
            # le righe legate all'asse: hz alla velocita' di riferimento di 10 kn (per la libreria), poi il multiplo
            asse = [f'[{round(m * giri * 10.0 / 60.0, 2)}, {d}, "{t}", 0.0, 1000000000.0, {float(m)}]' for m, d, t in ASSE.get(c, [])]
            return [f'righe = [' + ', '.join([f'[{h}, {d}, "{t}", {a}, {b}]' for h, d, t, a, b in righe] + asse) + ']',
                    f'pale = {pale}', f'assi = {assi}', f'giri_per_nodo = {valore(giri)}',
                    f'nazionalita = "{naz}"', f'tipo_unita = "{tipo}"']
        if c in SIGLE_ARMI:
            return [f'nazionalita = "arma"', f'tipo_unita = "{"esca" if c == "adc_mk2" else "siluro"}"']
        return ['nazionalita = "ignota"', 'tipo_unita = "generica"']
    for c, (punti, vcav, ton, tsp, tst, snork, fonte) in SOTTOMARINI.items():
        righe += [f'[firme.{c}]', f'punti = {valore(punti)}', f'v_cavitazione_kn = {valore(vcav)}', f'tonale_db = {valore(ton)}',
                  f'tonale_hz = {valore(hz(c))}', *analisi(c),
                  f'ts_prua_db = {valore(tsp)}', f'ts_traverso_db = {valore(tst)}', f'snorkel_db = {valore(snork)}',
                  f'fonte = {valore(fonte)}', '']
    # l'innesco della cavitazione delle navi, solo per il DEMON (il livello di sorgente delle navi la contiene gia'):
    # STIMA di gioco, 11 nodi le navi americane (eliche moderne e Prairie-Masker), 8 le sovietiche, 6 i mercantili
    def vcav_nave(c):
        naz = ANALISI[c][2] if c in ANALISI else ''
        return {'usa': 11.0, 'urss': 8.0, 'civile': 6.0}.get(naz, 8.0) if c in NAVI else 0.0
    for c, (punti, ton, tsp, tst, fonte) in {**NAVI, **ALTRI}.items():
        righe += [f'[firme.{c}]', f'punti = {valore(punti)}', 'nave = true', f'v_cavitazione_kn = {valore(vcav_nave(c))}',
                  f'tonale_db = {valore(ton)}', f'tonale_hz = {valore(hz(c))}', *analisi(c), f'ts_prua_db = {valore(tsp)}',
                  f'ts_traverso_db = {valore(tst)}',
                  f'fonte = {valore(fonte)}', '']
    for s, campi in SENSORI.items():
        righe += [f'[sensori.{s}]', *(f'{k} = {valore(v)}' for k, v in campi.items()), '']
    righe += ['[suite]', *(f'{c} = {valore(ids)}' for c, ids in SUITE.items()), '']
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from sorgenti_libreria import S as SORGENTI   # paesaggio sonoro (S5c)
    for c, campi in SORGENTI.items():
        righe.append(f'[sorgenti.{c}]')
        for k, v in campi.items():
            if k == 'componenti':
                righe.append('componenti = [' + ', '.join(tabella(x) for x in v) + ']')
            elif k == 'ritmo':
                righe.append(f'ritmo = {tabella(v)}')
            else:
                righe.append(f'{k} = {valore_esatto(v)}')
        righe.append('')
    from sorgenti_asw import T as TRANSITORI   # ASW aereo (A2)
    for c, campi in TRANSITORI.items():
        righe.append(f'[transitori.{c}]')
        for k, v in campi.items():
            if k == 'componenti':
                righe.append('componenti = [' + ', '.join(tabella(x) for x in v) + ']')
            elif k == 'ritmo':
                righe.append(f'ritmo = {tabella(v)}')
            else:
                righe.append(f'{k} = {valore_esatto(v)}')
        righe.append('')
    from civili import righe_civili   # tipi di navi civili del traffico (blocco 1 del sonar)
    righe += righe_civili()
    for sezione, tabella_dati in (('intercettatori', INTERCETTATORI), ('emissioni', EMISSIONI),   # blocco 2, E
                                  ('ecoscandagli', ECOSCANDAGLI), ('bqs15_fondo', BQS15_FONDO)):   # docs/24
        for k, campi in tabella_dati.items():
            righe += [f'[{sezione}.{k}]', *(f'{c} = {valore(v)}' for c, v in campi.items()), '']
    for k, campi in SCHIERE.items():   # la geometria delle schiere trainate (blocco 1 del sonar, D)
        righe += [f'[schiere.{k}]', *(f'{c} = {valore(v)}' for c, v in campi.items()), '']
    open(os.path.join(radice, 'data', 'motore', 'acustica.toml'), 'w').write('\n'.join(righe))
    assert all(i in SENSORI for ids in SUITE.values() for i in ids)
    for c, (righe, *_) in ANALISI.items():   # la riga piu' forte e' la tonale della firma (LOFAR di valuta)
        assert any(abs(h - hz(c)) < 1e-9 and d == 0 for h, d, *_ in righe) or c in SIGLE_ARMI or c in ('biologico', 'kilo', 'foxtrot', 'alfa', 'mercantile', 'cargo', 'petroliera'), c
    return len(SOTTOMARINI) + len(NAVI) + len(ALTRI), len(SENSORI)


if __name__ == '__main__':
    print('firme, sensori:', esporta_acustica(sys.argv[1] if len(sys.argv) > 1 else '.'))
