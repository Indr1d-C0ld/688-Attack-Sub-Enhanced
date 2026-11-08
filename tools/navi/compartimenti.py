"""Sezioni dello scafo e compartimentazione delle navi di superficie e dei sottomarini, per la fisica dei danni del
motore (docs/10_danni_affondamento_esplosioni.md §2, fasi D3 e D6).

Uso: compartimenti.py <radice progetto> <classe.yaml> [...]
Scrive <uscita della classe>/scafo.toml e stampa un controllo idrostatico (dislocamento della geometria contro
quello pubblicato, KB, BM, KM).

Sezioni: il modello del kit con l'opera viva (costruisci.modello_intero) tagliato in 60 stazioni; per ogni stazione
la semilarghezza dello scafo ogni 0,25 m di quota, dalla chiglia alla coperta (parti 'scafo', 'carena', 'bulbo').

Compartimentazione (tabella CLASSI, stime di gioco da schede e piani pubblici, affidabilita' bassa):
- paratie stagne: paratia di collisione al 5% della lunghezza, poi `compartimenti` locali uguali fino a poppa;
- locali per uso, dalle posizioni del modello (distanze dalla prua in m): `macchine`, `depositi` (munizioni, con
  il tipo), `carico` (stive o cisterne), `ali` (casse laterali: frazione della larghezza);
- zone di fuoco sopra coperta: `hangar` (carburante avio), `ponte` (lanciatori in coperta);
- nave: GM intatta (m), dislocamento pubblicato (t, solo controllo), squadre e qualita' del controllo danni,
  fattore di resistenza della trave rispetto al modulo minimo di regolamento.

Permeabilita' (frazione di un locale che l'acqua puo' occupare, valori SOLAS indicativi): macchine 0,85, locali
d'alloggio e servizio 0,95, depositi munizioni e magazzini 0,60, stive di merci varie 0,60, portacontainer 0,70,
cisterne di greggio cariche 0,15 (il greggio esce e l'acqua entra: aumento di peso netto piccolo).

Sottomarini (tabella SOTTOMARINI, base per la fase D6; affidabilita' bassa per posizioni e diametri):
- sezioni dello scafo esterno senza il cassero (sovrastruttura allagabile, che non da' spinta); in emersione il
  sottomarino si comporta come una nave a sezioni tonde, con la riserva di spinta sopra il galleggiamento;
- compartimenti come celle fra paratie (distanze dalla prua in m), con gli usi `zavorra` (casse principali alle
  estremita'), `deposito` (siluri, missili), `reattore`, `batterie`, `macchine`, `locali`; nei doppi scafi
  l'intercapedine ai fianchi di ogni compartimento e' una cella di zavorra per lato;
- scafo resistente per stazione: raggio, quota e distanza dall'asse di simmetria dell'asse (due scafi affiancati
  nel Typhoon). Negli scafi singoli (688, Sturgeon, Trafalgar) coincide con lo scafo esterno fra le casse di
  zavorra; nei doppi e' lo scafo esterno meno un'intercapedine tarata perche' lo scafo resistente sposti il 90%
  del dislocamento in emersione (il peso del battello e' quasi tutto dentro lo scafo resistente): il minore fra
  quello della geometria e quello pubblicato, dove il modello e' piu' grosso del battello vero;
- quote di collaudo e di collasso (collasso 1,5 volte il collaudo dove manca, la regola di progetto del dossier 02
  §9.3), dislocamenti pubblicati in emersione e in immersione, depositi fuori dallo scafo resistente (contenitori
  dei missili fra i due scafi di Oscar II e Charlie II);
- riserva di spinta tarata su quella pubblicata senza spostare il galleggiamento delle foto (`tara_riserva`):
  scafo leggero allagabile sopra il galleggiamento dove la geometria ne ha troppa, zavorra iniziale nelle casse
  dove ne ha troppo poca.
"""
import copy, os, sys
import numpy as np, yaml
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import costruisci
from danni_critici import CRITICI, CRITICI_SOTTOMARINI, PROPELLENTE, PROPELLENTE_PER_KG, TIPO_PROPELLENTE

STAZIONI = 60
DZ = 0.25
PARTI_SCAFO = ('scafo', 'carena', 'bulbo')

# d = distanza dalla prua (m); macchine e carico: [(d0, d1)]; depositi: [(d0, d1, tipo)]; ali: [(d0, d1, frazione B)]
CLASSI = {
    'perry': dict(tipo='militare', dislocamento_t=4100, GM=1.1, compartimenti=12, squadre=4, qualita=1.0, resistenza=0.8,
                  macchine=[(60, 100)], depositi=[(22, 38, 'missili'), (78, 88, 'proietti')], ali=[(60, 100, 0.12)],
                  hangar=[(112, 126)]),
    'spruance': dict(tipo='militare', dislocamento_t=8040, GM=1.4, compartimenti=14, squadre=5, qualita=1.0, resistenza=0.8,
                     macchine=[(70, 88), (104, 122)], depositi=[(26, 38, 'proietti'), (42, 52, 'missili'),
                     (140, 150, 'missili'), (152, 163, 'proietti')], ali=[(70, 122, 0.12)], hangar=[(117, 132)],
                     ponte=[(78, 85, 'harpoon')]),
    'kidd': dict(tipo='militare', dislocamento_t=9574, GM=1.4, compartimenti=14, squadre=5, qualita=1.0, resistenza=0.8,
                 macchine=[(70, 88), (104, 122)], depositi=[(26, 38, 'proietti'), (40, 52, 'missili'),
                 (144, 156, 'missili'), (155, 164, 'proietti')], ali=[(70, 122, 0.12)], hangar=[(117, 132)],
                 ponte=[(79, 86, 'harpoon')]),
    'ticonderoga': dict(tipo='militare', dislocamento_t=9800, GM=1.3, compartimenti=14, squadre=5, qualita=1.0, resistenza=0.8,
                        macchine=[(70, 88), (104, 122)], depositi=[(26, 34, 'proietti'), (35, 46, 'missili'),
                        (143, 154, 'missili'), (155, 163, 'proietti')], ali=[(70, 122, 0.12)], hangar=[(104, 117)],
                        ponte=[(163, 169, 'harpoon')]),
    # la Type 22: COGOG (Olympus e Tyne) in due locali; i depositi del Sea Wolf sotto i due lanciatori, gli Exocet in coperta
    'type22': dict(tipo='militare', dislocamento_t=4400, GM=1.1, compartimenti=12, squadre=4, qualita=1.0, resistenza=0.8,
                   macchine=[(70, 88), (90, 104)], depositi=[(28, 38, 'missili'), (106, 114, 'missili')],
                   ali=[(70, 104, 0.12)], hangar=[(106, 117)], ponte=[(36, 42, 'exocet')]),
    # la base galleggiante Ugra: i depositi dei siluri e dei missili dei battelli sotto la gru, i proietti da 57 (STIMA)
    'ugra': dict(tipo='militare', dislocamento_t=9650, GM=1.2, compartimenti=12, squadre=4, qualita=0.8, resistenza=0.9,
                 macchine=[(80, 105)], depositi=[(18, 28, 'proietti'), (66, 80, 'siluri'), (104, 114, 'proietti')],
                 ali=[(80, 105, 0.10)]),
    # la Knox: vapore, un locale caldaie e uno macchine; il deposito del 127 a prua, quello dell'ASROC sotto il lanciatore
    'knox': dict(tipo='militare', dislocamento_t=4065, GM=1.0, compartimenti=12, squadre=4, qualita=1.0, resistenza=0.8,
                 macchine=[(58, 72), (72, 86)], depositi=[(18, 30, 'proietti'), (32, 42, 'missili')],
                 ali=[(58, 86, 0.12)], hangar=[(95, 103)], ponte=[(123, 130, 'sea_sparrow')]),
    'type22b2': dict(tipo='militare', dislocamento_t=4800, GM=1.1, compartimenti=13, squadre=4, qualita=1.0, resistenza=0.8,
                     macchine=[(82, 100), (102, 116)], depositi=[(38, 48, 'missili'), (118, 126, 'missili')],
                     ali=[(82, 116, 0.12)], hangar=[(118, 130)], ponte=[(48, 54, 'exocet')]),
    # la Oslo: vapore, un locale caldaie e macchine; i depositi dei due binati da 76 e dei razzi Terne; i Penguin sul cassero
    'oslo': dict(tipo='militare', dislocamento_t=1745, GM=0.9, compartimenti=10, squadre=3, qualita=1.0, resistenza=0.7,
                 macchine=[(46, 62)], depositi=[(18, 26, 'proietti'), (28, 36, 'missili'), (66, 74, 'proietti')],
                 ali=[(46, 62, 0.10)], ponte=[(84, 91, 'penguin')]),
    # la Stockholm: lo scafo d'alluminio, le macchine a centro nave; il deposito del 57 sotto la torre, gli RBS-15 in coperta
    'stockholm': dict(tipo='militare', dislocamento_t=372, GM=0.8, compartimenti=7, squadre=2, qualita=1.0, resistenza=0.5,
                      macchine=[(26, 40)], depositi=[(10, 17, 'proietti')], ponte=[(30, 46, 'rbs15')]),
    # la Hauk: due diesel a poppa del mezzo; il deposito del 40 sotto la plancia, i Penguin e i siluri sul cassero
    'hauk': dict(tipo='militare', dislocamento_t=148, GM=0.7, compartimenti=6, squadre=1, qualita=1.0, resistenza=0.4,
                 macchine=[(20, 30)], depositi=[(8, 13, 'proietti')], ponte=[(24, 36, 'penguin')]),
    'iowa': dict(tipo='militare', dislocamento_t=57500, GM=2.8, compartimenti=22, squadre=8, qualita=1.0, resistenza=1.6,
                 macchine=[(112, 188)], depositi=[(58, 80, 'proietti'), (80, 100, 'proietti'), (182, 204, 'proietti')],
                 ali=[(55, 205, 0.15)], ponte=[(100, 175, 'tomahawk')]),
    'nimitz': dict(tipo='militare', dislocamento_t=100000, GM=3.0, compartimenti=30, squadre=10, qualita=1.0, resistenza=1.3,
                   macchine=[(110, 230)], depositi=[(80, 108, 'bombe'), (232, 260, 'bombe')], ali=[(60, 280, 0.12)],
                   hangar=[(70, 250)]),
    # la Kitty Hawk: caldaie e turbine a vapore convenzionali nel tratto delle macchine della Nimitz (STIMA)
    'kitty_hawk': dict(tipo='militare', dislocamento_t=81800, GM=3.0, compartimenti=30, squadre=10, qualita=1.0,
                       resistenza=1.3, macchine=[(105, 225)], depositi=[(78, 105, 'bombe'), (226, 252, 'bombe')],
                       ali=[(58, 272, 0.12)], hangar=[(68, 243)]),
    'sacramento': dict(tipo='militare', dislocamento_t=53600, GM=2.0, compartimenti=16, squadre=6, qualita=1.0, resistenza=1.0,
                       macchine=[(165, 200)], depositi=[(70, 110, 'munizioni')], carico=[(25, 165, 0.30)],
                       ali=[(25, 165, 0.20)], hangar=[(212, 230)]),
    'krivak2': dict(tipo='militare', dislocamento_t=3900, GM=1.0, compartimenti=11, squadre=3, qualita=0.8, resistenza=0.8,
                    macchine=[(58, 90)], depositi=[(26, 44, 'missili'), (82, 90, 'missili'), (92, 108, 'proietti')],
                    ali=[(58, 90, 0.12)]),
    'kara': dict(tipo='militare', dislocamento_t=9700, GM=1.3, compartimenti=14, squadre=4, qualita=0.8, resistenza=0.8,
                 macchine=[(92, 125)], depositi=[(18, 34, 'proietti'), (36, 60, 'missili'), (88, 98, 'missili'),
                 (130, 150, 'missili')], ali=[(92, 125, 0.12)], hangar=[(153, 166)]),
    # l'Udaloy (dossier 31): le macchine fra i due fumaioli, i Rastrub ai lati della plancia, le torri a prua, i due
    # hangar a poppa (STIMA)
    'udaloy': dict(tipo='militare', dislocamento_t=7620, GM=1.3, compartimenti=14, squadre=4, qualita=0.8, resistenza=0.8,
                   macchine=[(78, 108)], depositi=[(30, 50, 'proietti'), (50, 60, 'missili'), (110, 125, 'missili')],
                   ali=[(78, 108, 0.12)], hangar=[(128, 145)]),
    'kirov': dict(tipo='militare', dislocamento_t=24300, GM=2.0, compartimenti=20, squadre=6, qualita=0.8, resistenza=1.0,
                  macchine=[(120, 190)], depositi=[(20, 35, 'missili'), (35, 62, 'missili'), (188, 210, 'proietti')],
                  ali=[(120, 190, 0.12)], hangar=[(215, 235)]),
    'slava': dict(tipo='militare', dislocamento_t=11500, GM=1.5, compartimenti=15, squadre=4, qualita=0.8, resistenza=0.8,
                  macchine=[(100, 140)], depositi=[(25, 38, 'proietti'), (140, 160, 'missili')], ali=[(100, 140, 0.12)],
                  ponte=[(40, 80, 'bazalt')], hangar=[(160, 172)]),
    'grisha3': dict(tipo='militare', dislocamento_t=1120, GM=0.8, compartimenti=8, squadre=2, qualita=0.8, resistenza=0.8,
                    macchine=[(40, 58)], depositi=[(12, 20, 'missili'), (58, 66, 'proietti')]),
    'koni': dict(tipo='militare', dislocamento_t=1900, GM=0.9, compartimenti=9, squadre=2, qualita=0.8, resistenza=0.8,
                 macchine=[(48, 70)], depositi=[(18, 28, 'proietti'), (66, 80, 'proietti')]),
    'nanuchka': dict(tipo='militare', dislocamento_t=670, GM=0.8, compartimenti=6, squadre=1, qualita=0.8, resistenza=0.6,
                     macchine=[(34, 50)], depositi=[(10, 17, 'missili')], ponte=[(26, 36, 'malakhit')]),
    'agi': dict(tipo='militare', dislocamento_t=760, GM=0.7, compartimenti=6, squadre=1, qualita=0.5, resistenza=0.6,
                macchine=[(29, 40)], depositi=[(25, 27, 'missili')]),
    'petroliera': dict(tipo='petroliera', dislocamento_t=100000, GM=4.0, compartimenti=10, squadre=2, qualita=0.5,
                       resistenza=1.0, macchine=[(188, 226)], carico=[(14, 186, 0.15)], ali=[(14, 186, 0.25)]),
    'mercantile': dict(tipo='mercantile', dislocamento_t=22000, GM=0.8, compartimenti=8, squadre=2, qualita=0.5,
                       resistenza=1.0, macchine=[(68, 92)], carico=[(14, 68, 0.60), (92, 160, 0.60)]),
    'traghetto': dict(tipo='mercantile', dislocamento_t=14000, GM=1.2, compartimenti=10, squadre=2, qualita=0.5,
                      resistenza=0.9, macchine=[(100, 125)], carico=[(10, 140, 0.30)]),   # il ponte garage: STIMA
    # la nave oceanografica: le macchine a poppa del mezzo, sotto il fumaiolo; i laboratori nella sovrastruttura
    'nave_ricerca': dict(tipo='mercantile', dislocamento_t=2450, GM=0.7, compartimenti=7, squadre=1, qualita=0.5,
                         resistenza=0.6, macchine=[(48, 62)], carico=[]),
    'rimorchiatore': dict(tipo='mercantile', dislocamento_t=1800, GM=0.9, compartimenti=6, squadre=1, qualita=0.5,
                          resistenza=1.0, macchine=[(20, 35)], carico=[]),
    'portarinfuse': dict(tipo='mercantile', dislocamento_t=60000, GM=1.0, compartimenti=9, squadre=2, qualita=0.5,
                         resistenza=1.0, macchine=[(190, 215)], carico=[(15, 190, 0.70)]),
    'rompighiaccio': dict(tipo='mercantile', dislocamento_t=15000, GM=1.5, compartimenti=10, squadre=2, qualita=0.6,
                          resistenza=1.3, macchine=[(55, 95)], carico=[(12, 50, 0.30)]),   # scafo da ghiaccio: STIMA
    # le civili col modello proprio dal 31/10/2026: le macchine a poppa; il carico (le stive del pesce, le cisterne)
    'bmrt': dict(tipo='mercantile', dislocamento_t=4240, GM=0.8, compartimenti=8, squadre=1, qualita=0.5, resistenza=1.0,
                 macchine=[(62, 80)], carico=[(14, 62, 0.40)]),
    'cianciolo': dict(tipo='mercantile', dislocamento_t=1860, GM=0.8, compartimenti=6, squadre=1, qualita=0.5,
                      resistenza=1.0, macchine=[(10, 22)], carico=[(22, 40, 0.50)]),
    'petroliera_costiera': dict(tipo='petroliera', dislocamento_t=12100, GM=1.2, compartimenti=8, squadre=1, qualita=0.5,
                                resistenza=1.0, macchine=[(90, 112)], carico=[(19, 88, 0.15)], ali=[(19, 88, 0.25)]),
    'peschereccio_costiero': dict(tipo='mercantile', dislocamento_t=360, GM=0.8, compartimenti=5, squadre=1, qualita=0.5,
                                  resistenza=1.0, macchine=[(17, 25)], carico=[(5, 17, 0.40)]),
    'peschereccio': dict(tipo='mercantile', dislocamento_t=2000, GM=0.8, compartimenti=6, squadre=1, qualita=0.5,
                         resistenza=1.0, macchine=[(38, 50)], carico=[(12, 36, 0.60)]),
    'cargo': dict(tipo='portacontainer', dislocamento_t=46000, GM=1.5, compartimenti=9, squadre=2, qualita=0.5,
                  resistenza=1.0, macchine=[(194, 224)], carico=[(18, 192, 0.70)], ali=[(18, 192, 0.10)]),
}

# Sottomarini: dislocamenti (t) e quote (m) dal dossier docs/ricerca/01_piattaforme_e_armi.md, compartimenti da
# schede pubbliche (688 dal dossier 02 §9.3; sovietici dal numero di compartimenti noto, posizioni stimate con la
# centrale sotto la vela). celle: [(d0, d1, uso[, deposito])]; resistente: [(d0, d1, diametro o None, y dell'asse)].
SOTTOMARINI = {
    'los_angeles': dict(scafo='singolo', emersione_t=6082, immersione_t=6927, collaudo=290, collasso=435, GM=0.4,
                        squadre=3, qualita=1.0, resistenza=1.5, resistente=[(14, 95, None, 0.0)],
                        celle=[(0, 14, 'zavorra'), (14, 58, 'deposito', 'siluri'), (58, 68, 'reattore'),
                               (68, 95, 'macchine'), (95, 110.3, 'zavorra')]),
    'sturgeon': dict(scafo='singolo', emersione_t=3698, immersione_t=4714, collaudo=400, collasso=600, GM=0.4,
                     squadre=3, qualita=1.0, resistenza=1.5, resistente=[(12, 76, None, 0.0)],
                     celle=[(0, 12, 'zavorra'), (12, 44, 'deposito', 'siluri'), (44, 52, 'reattore'),
                            (52, 76, 'macchine'), (76, 89.1, 'zavorra')]),
    'trafalgar': dict(scafo='singolo', emersione_t=4700, immersione_t=5250, collaudo=300, collasso=500, GM=0.4,
                      squadre=3, qualita=1.0, resistenza=1.5, resistente=[(11, 72, None, 0.0)],
                      celle=[(0, 11, 'zavorra'), (11, 41, 'deposito', 'siluri'), (41, 50, 'reattore'),
                             (50, 72, 'macchine'), (72, 85.4, 'zavorra')]),
    # il Victor I (dossier 31): come il Victor III, piu' corto; quote STIMA (400 m di collaudo)
    'victor1': dict(scafo='doppio', emersione_t=3500, immersione_t=4700, collaudo=400, collasso=600, GM=0.4,
                    squadre=3, qualita=0.8, resistenza=1.5, resistente=[(9, 82, None, 0.0)],
                    celle=[(0, 9, 'zavorra'), (9, 20, 'deposito', 'siluri'), (20, 30, 'batterie'), (30, 41, 'locali'),
                           (41, 52, 'reattore'), (52, 64, 'macchine'), (64, 82, 'macchine'), (82, 93, 'zavorra')]),
    'victor3': dict(scafo='doppio', emersione_t=4950, immersione_t=7250, collaudo=600, collasso=900, GM=0.4,
                    squadre=3, qualita=0.8, resistenza=1.5, resistente=[(10, 94, None, 0.0)],
                    celle=[(0, 10, 'zavorra'), (10, 22, 'deposito', 'siluri'), (22, 34, 'batterie'), (34, 46, 'locali'),
                           (46, 58, 'reattore'), (58, 72, 'macchine'), (72, 84, 'macchine'), (84, 94, 'locali'),
                           (94, 106, 'zavorra')]),
    # il primo lotto delle firme nuove (dossier 31, 29/10/2026): i dislocamenti dal dossier; quote e celle STIMA (i silos
    # dietro la vela, il reattore e le macchine a poppa, come sui Delta)
    'delta3': dict(scafo='doppio', emersione_t=10600, immersione_t=13700, collaudo=320, collasso=480, GM=0.4,
                   squadre=4, qualita=0.8, resistenza=1.5, resistente=[(12, 140, None, 0.0)],
                   celle=[(0, 12, 'zavorra'), (12, 26, 'deposito', 'siluri'), (26, 40, 'batterie'), (40, 54, 'locali'),
                          (54, 96, 'deposito', 'missili'), (96, 110, 'reattore'), (110, 124, 'macchine'),
                          (124, 140, 'macchine'), (140, 156, 'zavorra')]),
    # l'Ohio (dossier 32, 30/10/2026): i dislocamenti dal dossier; quote e celle STIMA (24 silos dietro la vela, il
    # reattore e le macchine a poppa)
    'ohio': dict(scafo='singolo', emersione_t=16764, immersione_t=18750, collaudo=250, collasso=375, GM=0.4,
                 squadre=4, qualita=1.0, resistenza=1.5, resistente=[(16, 150, None, 0.0)],
                 celle=[(0, 16, 'zavorra'), (16, 40, 'deposito', 'siluri'), (40, 52, 'locali'),
                        (52, 105, 'deposito', 'missili'), (105, 118, 'reattore'), (118, 150, 'macchine'),
                        (150, 170.7, 'zavorra')]),
    # la Sierra I in titanio: 550 m al massimo (dossier 31); il collaudo STIMA
    'sierra': dict(scafo='doppio', emersione_t=5940, immersione_t=8300, collaudo=450, collasso=550, GM=0.4,
                   squadre=3, qualita=0.8, resistenza=1.6, resistente=[(10, 94, None, 0.0)],
                   celle=[(0, 10, 'zavorra'), (10, 22, 'deposito', 'siluri'), (22, 34, 'batterie'), (34, 48, 'locali'),
                          (48, 62, 'reattore'), (62, 76, 'macchine'), (76, 94, 'macchine'), (94, 107.2, 'zavorra')]),
    'alfa': dict(scafo='doppio', emersione_t=2300, immersione_t=3180, collaudo=400, collasso=600, GM=0.4,
                 squadre=2, qualita=0.8, resistenza=1.5, resistente=[(9, 72, None, 0.0)],
                 celle=[(0, 9, 'zavorra'), (9, 19, 'deposito', 'siluri'), (19, 29, 'batterie'), (29, 40, 'locali'),
                        (40, 50, 'reattore'), (50, 62, 'macchine'), (62, 72, 'macchine'), (72, 81.4, 'zavorra')]),
    'akula': dict(scafo='doppio', emersione_t=8140, immersione_t=12770, collaudo=600, collasso=900, GM=0.4,
                  squadre=3, qualita=0.8, resistenza=1.5, resistente=[(12, 96, None, 0.0)],
                  celle=[(0, 12, 'zavorra'), (12, 24, 'deposito', 'siluri'), (24, 34, 'batterie'), (34, 46, 'locali'),
                         (46, 56, 'locali'), (56, 66, 'reattore'), (66, 76, 'macchine'), (76, 86, 'macchine'),
                         (86, 96, 'locali'), (96, 110.3, 'zavorra')]),
    'oscar2': dict(scafo='doppio', emersione_t=14700, immersione_t=19400, collaudo=600, collasso=900, GM=0.6,
                   squadre=4, qualita=0.8, resistenza=1.5, resistente=[(15, 140, None, 0.0)],
                   celle=[(0, 15, 'zavorra'), (15, 30, 'deposito', 'siluri'), (30, 44, 'locali'), (44, 56, 'locali'),
                          (56, 68, 'locali'), (68, 80, 'macchine'), (80, 92, 'reattore'), (92, 106, 'macchine'),
                          (106, 118, 'macchine'), (118, 128, 'macchine'), (128, 140, 'locali'), (140, 154, 'zavorra')],
                   fuori=[(36, 104, 'missili')]),          # 24 contenitori P-700 fra i due scafi
    'charlie2': dict(scafo='doppio', emersione_t=4300, immersione_t=5300, collaudo=300, collasso=450, GM=0.4,
                     squadre=3, qualita=0.8, resistenza=1.5, resistente=[(12, 95, None, 0.0)],
                     celle=[(0, 12, 'zavorra'), (12, 24, 'deposito', 'siluri'), (24, 36, 'batterie'), (36, 48, 'locali'),
                            (48, 58, 'locali'), (58, 68, 'reattore'), (68, 78, 'macchine'), (78, 88, 'macchine'),
                            (88, 95, 'locali'), (95, 104.5, 'zavorra')],
                     fuori=[(10, 28, 'missili')]),         # 8 contenitori P-120 a prua fra i due scafi
    # Typhoon: due scafi resistenti principali affiancati (7,2 m), modulo dei siluri a prua e modulo di poppa sull'asse;
    # i 20 silos dei missili fra i due scafi a proravia della vela; la centrale sotto la vela (modulo a parte, qui
    # compresa nella cella)
    'typhoon': dict(scafo='doppio', emersione_t=23200, immersione_t=48000, collaudo=400, collasso=600, GM=1.0,
                    squadre=4, qualita=0.8, resistenza=1.5,
                    resistente=[(6, 22, 6.0, 0.0), (22, 158, 7.2, 6.4), (158, 168, 5.0, 0.0)],
                    celle=[(0, 6, 'zavorra'), (6, 22, 'deposito', 'siluri'), (22, 42, 'batterie'),
                           (42, 82, 'deposito', 'missili'), (82, 111, 'locali'), (111, 125, 'reattore'),
                           (125, 140, 'macchine'), (140, 158, 'macchine'), (158, 168, 'locali'), (168, 175, 'zavorra')]),
    # il Type 206A: scafo singolo, collaudo 200 m (STIMA); otto tubi a prua, le batterie sotto i locali
    'type206': dict(scafo='singolo', emersione_t=450, immersione_t=498, collaudo=200, collasso=300, GM=0.3,
                    squadre=1, qualita=1.0, resistenza=1.5, resistente=[(3, 42, None, 0.0)],
                    celle=[(0, 3, 'zavorra'), (3, 12, 'deposito', 'siluri'), (12, 26, 'batterie'), (26, 34, 'locali'),
                           (34, 42, 'macchine'), (42, 48.6, 'zavorra')]),
    # la Näcken: scafo singolo, collaudo 300 m (STIMA); sei tubi a prua, le batterie sotto i locali
    'nacken': dict(scafo='singolo', emersione_t=1015, immersione_t=1085, collaudo=300, collasso=450, GM=0.3,
                   squadre=1, qualita=1.0, resistenza=1.5, resistente=[(3, 44, None, 0.0)],
                   celle=[(0, 3, 'zavorra'), (3, 12, 'deposito', 'siluri'), (12, 24, 'batterie'), (24, 33, 'locali'),
                          (33, 44, 'macchine'), (44, 49.5, 'zavorra')]),
    'kilo': dict(scafo='doppio', emersione_t=2325, immersione_t=3075, collaudo=300, collasso=450, GM=0.4,
                 squadre=2, qualita=0.8, resistenza=1.5, resistente=[(6, 66, None, 0.0)],
                 celle=[(0, 6, 'zavorra'), (6, 16, 'deposito', 'siluri'), (16, 26, 'batterie'), (26, 36, 'locali'),
                        (36, 48, 'macchine'), (48, 58, 'macchine'), (58, 66, 'locali'), (66, 72.6, 'zavorra')]),
    'foxtrot': dict(scafo='doppio', emersione_t=1983, immersione_t=2515, collaudo=280, collasso=420, GM=0.4,
                    squadre=2, qualita=0.8, resistenza=1.5, resistente=[(6, 86, None, 0.0)],
                    celle=[(0, 6, 'zavorra'), (6, 18, 'deposito', 'siluri'), (18, 30, 'batterie'), (30, 44, 'locali'),
                           (44, 54, 'batterie'), (54, 66, 'macchine'), (66, 76, 'macchine'),
                           (76, 86, 'deposito', 'siluri'), (86, 91.3, 'zavorra')]),
}
# permeabilita' dei compartimenti di un sottomarino: locali fitti di apparati, schermature del reattore, batterie
PERM_SOTTOMARINO = dict(zavorra=0.98, locali=0.85, macchine=0.85, reattore=0.50, batterie=0.60, deposito=0.60)
QUOTA_RESISTENTE = 0.90   # spinta dello scafo resistente / dislocamento in emersione (doppi scafi)
SOGLIA_RISERVA = 0.03     # differenza di riserva di spinta sotto cui non si tara

def sezioni(mesh, n=STAZIONI, dz=DZ):
    """Stazioni equidistanti sulla lunghezza dello scafo: x, quota della chiglia, semilarghezze ogni dz."""
    sel = np.array([p in PARTI_SCAFO for p in mesh.parte])
    T = mesh.tri[sel]
    xa, xp = T[..., 0].min(), T[..., 0].max()
    dx = (xp - xa) / n
    xs = xa + dx * (np.arange(n) + 0.5)
    out = []
    for x in xs:
        d = T[..., 0] - x
        s = d > 0
        k = s.any(1) & ~s.all(1)                # triangoli tagliati dal piano x
        if not k.any():
            out.append((x, 0.0, [])); continue
        t, dd, ss = T[k], d[k], s[k]
        pts = []                                # i due lati tagliati di ogni triangolo, nell'ordine 01, 12, 20
        for i in range(3):
            j = (i + 1) % 3
            u = (dd[:, i] / np.where(ss[:, i] != ss[:, j], dd[:, i] - dd[:, j], 1.0))[:, None]
            pts.append((ss[:, i] != ss[:, j], t[:, i] + (t[:, j] - t[:, i]) * u))
        primo = np.where(pts[0][0][:, None], pts[0][1], pts[1][1])
        secondo = np.where(pts[2][0][:, None], pts[2][1], pts[1][1])
        S = np.stack([primo[:, 1:], secondo[:, 1:]], 1)      # (n, 2, [y, z])
        z0, z1 = S[..., 1].min(), S[..., 1].max()
        zs = np.arange(z0, z1 + 1e-6, dz)
        hb = []
        for z in zs:
            ya, za = S[:, 0, 0], S[:, 0, 1]; yb, zb = S[:, 1, 0], S[:, 1, 1]
            m = (np.minimum(za, zb) <= z) & (np.maximum(za, zb) >= z) & (np.abs(zb - za) > 1e-9)
            y = ya[m] + (yb[m] - ya[m]) * (z - za[m]) / (zb[m] - za[m])
            piatti = (np.abs(zb - za) <= 1e-9) & (np.abs(za - z) < dz / 2)    # tratti orizzontali (coperta, fondo)
            yy = np.concatenate([np.abs(y), np.abs(ya[piatti]), np.abs(yb[piatti])])
            hb.append(float(yy.max()) if yy.size else 0.0)
        out.append((x, float(z0), hb))
    return xs, dx, out

def idrostatica(xs, dx, sez):
    """Volume, centro di carena e raggio metacentrico trasversale al galleggiamento intatto (z = 0)."""
    V = Mx = Mz = Iw = Aw = 0.0
    for x, z0, hb in sez:
        if not hb: continue
        zs = z0 + DZ * np.arange(len(hb)); hb = np.asarray(hb)
        m = zs <= 0
        if m.sum() < 2: continue
        z, b = zs[m], hb[m]
        A = np.trapezoid(2 * b, z); V += A * dx; Mx += A * x * dx; Mz += np.trapezoid(2 * b * z, z) * dx
        bw = float(np.interp(0.0, zs, hb)); Iw += 2 / 3 * bw ** 3 * dx; Aw += 2 * bw * dx
    return V, Mx / V, Mz / V, Iw / V, Aw

def larghezza_galleggiamento(sez):
    """Larghezza massima al galleggiamento dalle sezioni (alcuni YAML hanno uno scafo fittizio, es. la Nimitz)."""
    return 2 * max(float(np.interp(0.0, z0 + DZ * np.arange(len(hb)), hb)) for _, z0, hb in sez if hb and z0 < 0)

def propellente_kg(d, iv):
    """Il propellente di un deposito (da, a, tipo, neq, protezione): le tonnellate della classe (PROPELLENTE) divise
    fra i depositi dello stesso genere in proporzione alla carica, o la regola STIMA PROPELLENTE_PER_KG."""
    generi = {TIPO_PROPELLENTE[t] for t in iv[2].split('+') if t in TIPO_PROPELLENTE}
    if not generi:
        return 0.0
    genere = 'solido' if 'solido' in generi else 'polvere'
    totale = PROPELLENTE.get(d.get('nome_critici', ''), {}).get(genere)
    if totale is None:
        return PROPELLENTE_PER_KG[genere] * iv[3]
    stessi = [x for x in d.get('depositi', []) if len(x) > 3 and genere in
              {TIPO_PROPELLENTE[t] for t in x[2].split('+') if t in TIPO_PROPELLENTE}]
    return totale * 1000.0 * iv[3] / max(sum(x[3] for x in stessi), 1e-6)

def compartimentazione(cl, d, xs, dx, B):
    """Celle (locali stagni, divisi in ali e centro dove ci sono casse laterali) e zone di fuoco sopra coperta."""
    XB = cl['scafo']['L'] / 2
    xa, xp = xs[0] - dx / 2, xs[-1] + dx / 2; L = xp - xa
    x_di = lambda dd: XB - dd
    collisione = xp - 0.05 * L
    paratie = [xp, collisione] + list(np.linspace(collisione, xa, d['compartimenti'] + 1)[1:])
    paratie = sorted(set(round(p, 3) for p in paratie), reverse=True)
    celle, fuoco = [], []
    def sovrapposti(intervalli, a, b):
        return [iv for iv in intervalli if min(b, x_di(iv[0])) - max(a, x_di(iv[1])) > 0.3 * (b - a)]
    for k, (x1, x0) in enumerate(zip(paratie[:-1], paratie[1:])):
        uso, perm, dep = 'locali', 0.95, ''
        if sovrapposti(d.get('macchine', []), x0, x1): uso, perm = 'macchine', 0.85
        car = sovrapposti(d.get('carico', []), x0, x1)
        if car: uso, perm = 'carico', car[0][2]
        # i depositi: anche quelli corti (meta' del deposito nella cella, come quello dell'AGI) e piu' d'uno per cella
        # (i tipi uniti, «missili+proietti»: il motore guarda l'uso, il tipo va al registro). Prima vinceva il primo
        # e si perdevano il 127 poppiero della Ticonderoga e il deposito dell'AGI (kit, 31/10/2026)
        dp = [iv for iv in d.get('depositi', [])
              if (lambda o: o > 0.3 * (x1 - x0) or o > 0.5 * (iv[1] - iv[0]))(min(x1, x_di(iv[0])) - max(x0, x_di(iv[1])))]
        if dp: uso, perm, dep = 'deposito', 0.60, '+'.join(dict.fromkeys(iv[2] for iv in dp))
        # i danni critici (danni_critici.py): la carica netta e la protezione del deposito, divise fra le celle in
        # proporzione a quanto ne cade in ciascuna; il carburante allo stesso modo
        def quota(iv):
            return max(0.0, min(x1, x_di(iv[0])) - max(x0, x_di(iv[1]))) / max(iv[1] - iv[0], 1e-6)
        critico = {}
        neq = sum(iv[3] * quota(iv) for iv in dp if len(iv) > 3)
        if neq > 0:
            prop = sum(propellente_kg(d, iv) * quota(iv) for iv in dp if len(iv) > 3)
            critico.update(carica_kg=round(neq, 1), protezione=max(iv[4] for iv in dp if len(iv) > 4),
                           **({'propellente_kg': round(prop, 1)} if prop > 0 else {}))
        cb = [iv for iv in d.get('carburante', []) if quota(iv) > 0]
        if cb:
            critico.update(carburante='+'.join(dict.fromkeys(iv[2] for iv in cb)),
                           carburante_t=round(sum(iv[3] * quota(iv) for iv in cb), 1))
        ali = sovrapposti(d.get('ali', []), x0, x1)
        nome = f'L{k + 1:02d}'
        if ali:
            y_in = B / 2 - ali[0][2] * B
            perm_ali = perm if uso == 'carico' else 0.95 if d['tipo'] == 'militare' else 0.98
            celle.append(dict(nome=nome + 'D', x0=x0, x1=x1, y0=-1e3, y1=-y_in, uso='ala', permeabilita=perm_ali, deposito=''))
            celle.append(dict(nome=nome + 'C', x0=x0, x1=x1, y0=-y_in, y1=y_in, uso=uso, permeabilita=perm, deposito=dep,
                              **critico))
            celle.append(dict(nome=nome + 'S', x0=x0, x1=x1, y0=y_in, y1=1e3, uso='ala', permeabilita=perm_ali, deposito=''))
        else:
            celle.append(dict(nome=nome, x0=x0, x1=x1, y0=-1e3, y1=1e3, uso=uso, permeabilita=perm, deposito=dep, **critico))
    for d0, d1 in d.get('hangar', []):
        fuoco.append(dict(nome='hangar', x0=x_di(d1), x1=x_di(d0), carico='avio'))
    for iv in d.get('ponte', []):   # le armi in coperta: con la carica netta, se la si sa (danni_critici.py)
        fuoco.append(dict(nome=iv[2], x0=x_di(iv[1]), x1=x_di(iv[0]), carico='missili',
                          **({'carica_kg': float(iv[3])} if len(iv) > 3 else {})))
    return celle, fuoco

# ------------------------------------------------------------------ sottomarini
def modello_sottomarino(cl, base_dir):
    """Modello intero senza il cassero: la sovrastruttura allagabile non da' spinta."""
    cl = copy.deepcopy(cl); cl['scafo'].pop('cassero', None)
    return costruisci.modello_intero(cl, base_dir)

def scafo_resistente(cl, d, xs, dx, sez, V0):
    """Raggio, quota e distanza dall'asse di simmetria dello scafo resistente per stazione (0 fuori), intercapedine
    usata e volume. Nei doppi scafi senza diametro pubblicato l'intercapedine si tara sulla spinta: QUOTA_RESISTENTE
    del minore fra il volume immerso della geometria (V0, m^3) e quello del dislocamento pubblicato."""
    XB = cl['scafo']['L'] / 2
    geo = []
    for x, z0, hb in sez:
        seg = [s for s in d['resistente'] if s[0] <= XB - x < s[1]]
        if not hb or not seg: geo.append(None); continue
        zs = z0 + DZ * np.arange(len(hb)); zc = (zs[0] + zs[-1]) / 2
        _, _, D, y = seg[0]
        geo.append((zc, y, D, min((zs[-1] - zs[0]) / 2, float(np.interp(zc, zs, hb)) - y)))
    def raggi(gap):
        return [0.0 if g is None else max(min(g[2] / 2 if g[2] else np.inf, g[3] - gap), 0.0) for g in geo]
    def volume(gap):
        return sum(np.pi * r * r * dx * (2 if g and g[1] > 0 else 1) for r, g in zip(raggi(gap), geo))
    if d['scafo'] == 'singolo': gap = 0.0
    elif all(s[2] for s in d['resistente']): gap = 1.0
    else:
        lo, hi = 0.0, 4.0
        for _ in range(40):
            gap = (lo + hi) / 2
            lo, hi = (gap, hi) if volume(gap) > QUOTA_RESISTENTE * min(V0, d['emersione_t'] / 1.025) else (lo, gap)
    r = raggi(gap)
    z = [0.0 if g is None else g[0] for g in geo]; y = [0.0 if g is None else g[1] for g in geo]
    return r, z, y, gap, volume(gap)

def celle_sottomarino(cl, d, xs, rr, yy):
    """Compartimenti fra paratie; nei doppi scafi l'intercapedine ai fianchi dello scafo resistente (|y| oltre il
    suo bordo, mediana delle stazioni del compartimento) e' una cella di zavorra per lato, come le casse laterali
    delle navi: un colpo sul fianco squarcia prima la cassa, poi lo scafo resistente."""
    XB = cl['scafo']['L'] / 2
    celle, k = [], 0
    for i, c in enumerate(d['celle']):
        d0, d1, uso = c[:3]
        x0, x1 = XB - d1, XB - d0
        dep = c[3] if len(c) > 3 else ''
        if uso == 'zavorra':
            celle.append(dict(nome='Z_PRUA' if i == 0 else 'Z_POPPA', x0=x0, x1=x1, y0=-1e3, y1=1e3, uso=uso,
                              permeabilita=PERM_SOTTOMARINO[uso], deposito=''))
            continue
        k += 1
        bordo = [y + r for x, r, y in zip(xs, rr, yy) if x0 <= x < x1 and r > 0]
        if d['scafo'] == 'singolo' or not bordo:
            celle.append(dict(nome=f'C{k}', x0=x0, x1=x1, y0=-1e3, y1=1e3, uso=uso, permeabilita=PERM_SOTTOMARINO[uso],
                              deposito=dep))
            continue
        z = PERM_SOTTOMARINO['zavorra']
        ys = [y for x, r, y in zip(xs, rr, yy) if x0 <= x < x1 and r > 0]
        if np.median(ys) > 0:
            # due scafi affiancati (Typhoon): intercapedine esterna, scafo di dritta, spazio fra i due (zavorra, o i
            # silos dei missili), scafo di sinistra, intercapedine esterna
            ym = float(np.median(ys)); rm = float(np.median([r for x, r, y in zip(xs, rr, yy) if x0 <= x < x1 and r > 0]))
            fra = ('deposito', PERM_SOTTOMARINO['deposito'], dep) if dep == 'missili' else ('zavorra', z, '')
            nello = ('locali', '') if dep == 'missili' else (uso, dep)
            for nome, y0, y1, u, p, de in ((f'C{k}ZD', -1e3, -(ym + rm), 'zavorra', z, ''),
                                           (f'C{k}D', -(ym + rm), -(ym - rm), nello[0], PERM_SOTTOMARINO[nello[0]], nello[1]),
                                           (f'C{k}M', -(ym - rm), ym - rm, *fra),
                                           (f'C{k}S', ym - rm, ym + rm, nello[0], PERM_SOTTOMARINO[nello[0]], nello[1]),
                                           (f'C{k}ZS', ym + rm, 1e3, 'zavorra', z, '')):
                celle.append(dict(nome=nome, x0=x0, x1=x1, y0=y0, y1=y1, uso=u, permeabilita=p, deposito=de))
            continue
        y_in = float(np.median(bordo))
        celle.append(dict(nome=f'C{k}D', x0=x0, x1=x1, y0=-1e3, y1=-y_in, uso='zavorra', permeabilita=z, deposito=''))
        celle.append(dict(nome=f'C{k}', x0=x0, x1=x1, y0=-y_in, y1=y_in, uso=uso, permeabilita=PERM_SOTTOMARINO[uso],
                          deposito=dep))
        celle.append(dict(nome=f'C{k}S', x0=x0, x1=x1, y0=y_in, y1=1e3, uso='zavorra', permeabilita=z, deposito=''))
    fuori = [dict(nome=cosa, x0=XB - d1, x1=XB - d0, carico=cosa) for d0, d1, cosa in d.get('fuori', [])]
    carica_sottomarino(d.get('nome_critici', ''), celle, fuori)
    return celle, fuori

def carica_sottomarino(nome, celle, fuori):
    """Il carico iniziale dei depositi e dei contenitori fuori dallo scafo resistente (danni critici, fase 3), da
    CRITICI_SOTTOMARINI: le celle dello stesso tipo se lo dividono per lunghezza."""
    k = CRITICI_SOTTOMARINI.get(nome)
    if not k: return
    for tipo in ('siluri', 'missili'):
        cc = [c for c in celle if c['uso'] == 'deposito' and c['deposito'] == tipo]
        L = sum(c['x1'] - c['x0'] for c in cc)
        for c in cc:
            q = (c['x1'] - c['x0']) / L
            if tipo == 'missili' and 'missili_liquidi' in k:
                c.update(carburante='propellente_liquido', carburante_t=k['missili_liquidi'] * q, protezione=0.5)
            elif tipo in k:
                carica, propellente, protezione = k[tipo]
                c.update(carica_kg=carica * q, protezione=protezione, propellente_kg=propellente * q)
    if 'fuori' in k:
        import tomllib
        missile, n = k['fuori']
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'data', 'motore', 'missili.toml'), 'rb') as f:
            esplosivo = tomllib.load(f)['missili'][missile]['esplosivo_kg']
        for z in fuori: z['carica_kg'] = esplosivo * n / len(fuori)

def volume_esterno(sez, dx):
    return sum(np.trapezoid(2 * np.asarray(hb), z0 + DZ * np.arange(len(hb))) * dx for _, z0, hb in sez if len(hb) > 1)

def semilarghezza_resistente(z, r, zc, y):
    """Semilarghezza dello scafo resistente (o dei due affiancati) alla quota z: il bordo esterno."""
    return y + np.sqrt(max(r * r - (z - zc) ** 2, 0.0)) if r > 0 and abs(z - zc) < r else 0.0

def volume_sotto_galleggiamento(sez, dx, xs, c):
    """Volume della cella `c` sotto il galleggiamento intatto (z <= 0)."""
    v = 0.0
    for x, (_, z0, hb) in zip(xs, sez):
        if not (c['x0'] <= x < c['x1']) or len(hb) < 2: continue
        zs = z0 + DZ * np.arange(len(hb)); m = zs <= 0
        if m.sum() < 2: continue
        hb = np.asarray(hb)[m]
        larg = np.clip(np.minimum(hb, c['y1']) - np.maximum(-hb, c['y0']), 0, None)
        v += np.trapezoid(larg, zs[m]) * dx
    return v

def tara_riserva(d, xs, dx, sez, rr, zz, yy, celle):
    """Riserva di spinta della fisica uguale a quella pubblicata (immersione / emersione - 1) senza spostare il
    galleggiamento delle foto, che resta quello degli sprite:
    - riserva della geometria troppo alta: parte del volume sopra il galleggiamento e' scafo leggero allagabile
      (cassero, raccordi, estremita' senza casse). Le semilarghezze sopra z = 0 si restringono di un fattore,
      raccordato sui primi 0,5 m perche' il galleggiamento non cambi, mai dentro lo scafo resistente;
    - riserva troppo bassa: nelle foto il battello e' basso sull'acqua, con le casse zavorra in parte piene.
      L'acqua iniziale va nelle celle di zavorra in proporzione al loro volume sotto il galleggiamento; il motore
      la toglie dal peso del battello (stesso equilibrio) e l'equipaggio la soffia quando il battello e' colpito.
    Restituisce le sezioni (eventualmente ristrette) e la descrizione della taratura; scrive `acqua_iniziale_m3`
    nelle celle."""
    V0 = idrostatica(xs, dx, sez)[0]
    r_pub = d['immersione_t'] / d['emersione_t'] - 1
    r_geo = volume_esterno(sez, dx) / V0 - 1
    if abs(r_geo - r_pub) < SOGLIA_RISERVA: return sez, 'nessuna', 0.0
    if r_geo > r_pub:
        def ristrette(k):
            out = []
            for (x, z0, hb), r, zc, y in zip(sez, rr, zz, yy):
                zs = z0 + DZ * np.arange(len(hb))
                f = 1 + (k - 1) * np.clip(zs / 0.5, 0, 1)
                out.append((x, z0, [float(max(h * fi, semilarghezza_resistente(z, r, zc, y))) if z > 0 else h
                                    for h, fi, z in zip(hb, f, zs)]))
            return out
        lo, hi = 0.0, 1.0
        for _ in range(40):
            k = (lo + hi) / 2
            lo, hi = (k, hi) if volume_esterno(ristrette(k), dx) / V0 - 1 < r_pub else (lo, k)
        return ristrette(k), f'scafo leggero allagabile: semilarghezze sopra il galleggiamento x {k:.2f}', 0.0
    W = V0 - volume_esterno(sez, dx) / (1 + r_pub)
    zav = [c for c in celle if c['uso'] == 'zavorra']
    vs = [volume_sotto_galleggiamento(sez, dx, xs, c) * c['permeabilita'] for c in zav]
    f = min(W / max(sum(vs), 1e-9), 1.0)
    for c, v in zip(zav, vs): c['acqua_iniziale_m3'] = v * f
    return sez, f"casse zavorra piene al {f:.0%} sotto il galleggiamento", W * min(sum(vs) / max(W, 1e-9), 1.0)

def scrivi(file, cl, d, xs, dx, sez, celle, fuoco, idro, B, sub=None):
    """`sub` (sottomarini): dati della sezione [sottomarino], scafo resistente per stazione e depositi fuori dallo
    scafo resistente."""
    V, xb, kb, bm, aw = idro
    r = [f'# Scafo per la fisica dei danni (tools/navi/compartimenti.py): non modificare a mano.', '',
         '[nave]', f'tipo = "{d["tipo"]}"', f'GM_m = {d["GM"]}', f'dislocamento_pubblicato_t = {d["dislocamento_t"]}',
         f'squadre = {d["squadre"]}', f'qualita_controllo_danni = {d["qualita"]}', f'resistenza_trave = {d["resistenza"]}',
         f'lunghezza_m = {cl["scafo"]["L"]}', f'larghezza_m = {B:.2f}', '']
    if sub:
        r += ['[sottomarino]'] + [f'{k} = "{v}"' if isinstance(v, str) else f'{k} = {v}' for k, v in sub['dati'].items()] + ['']
    r += ['[stazioni]', f'passo_m = {dx:.4f}', f'dz_m = {DZ}', 'x = [' + ', '.join(f'{x:.3f}' for x in xs) + ']',
          'z_chiglia = [' + ', '.join(f'{z0:.3f}' for _, z0, _ in sez) + ']']
    if sub:
        for k in ('raggio_resistente', 'z_resistente', 'y_resistente'):
            r.append(f'{k} = [' + ', '.join(f'{v:.3f}' for v in sub[k]) + ']')
    r.append('semilarghezze = [')
    for _, _, hb in sez:
        r.append('  [' + ', '.join(f'{b:.3f}' for b in hb) + '],')
    r += [']', '']
    tabella = lambda nome, v: [f'[[{nome}]]'] + [f'{k} = "{x}"' if isinstance(x, str) else f'{k} = {float(x):.3f}' for k, x in v.items()] + ['']
    for c in celle: r += tabella('cella', c)
    for z in fuoco: r += tabella('sopra_coperta', z)
    for z in (sub or {}).get('fuori', []): r += tabella('fuori_resistente', z)
    open(file, 'w').write('\n'.join(r))

if __name__ == '__main__':
    root = sys.argv[1]
    sottomarini, intestazione = [], False
    for fcl in sys.argv[2:]:
        cl = yaml.safe_load(open(fcl)); nome = os.path.basename(os.path.normpath(cl['uscita']))
        if nome in SOTTOMARINI:
            sottomarini.append((fcl, cl, nome)); continue
        d = CLASSI.get(nome)
        if not d: print(f'{nome}: nessun dato di compartimentazione'); continue
        d = {**d, **CRITICI.get(nome, {}), 'nome_critici': nome}   # i danni critici sopra la compartimentazione (fase 2)
        if not intestazione:
            print(f"{'classe':12s} {'disl. geom.':>11s} {'pubblicato':>10s} {'rapporto':>8s} {'KB':>5s} {'BM':>6s} {'KM':>6s} {'celle':>5s}")
            intestazione = True
        mesh = costruisci.modello_intero(cl, os.path.dirname(os.path.abspath(fcl)))
        xs, dx, sez = sezioni(mesh)
        idro = idrostatica(xs, dx, sez)
        B = larghezza_galleggiamento(sez)
        celle, fuoco = compartimentazione(cl, d, xs, dx, B)
        scrivi(os.path.join(root, cl['uscita'], 'scafo.toml'), cl, d, xs, dx, sez, celle, fuoco, idro, B)
        V, xb, kb, bm, aw = idro
        z_chiglia = min(z0 for _, z0, hb in sez if hb)
        print(f"{nome:12s} {V * 1.025:11.0f} {d['dislocamento_t']:10d} {V * 1.025 / d['dislocamento_t']:8.2f} "
              f"{kb - z_chiglia:5.2f} {bm:6.2f} {kb - z_chiglia + bm:6.2f} {len(celle):5d}")

    if sottomarini:
        print(f"{'sottomarino':12s} {'emers. geom.':>12s} {'pubbl.':>7s} {'immers. geom.':>13s} {'pubbl.':>7s} "
              f"{'riserva':>7s} {'pubbl.':>6s} {'intercap.':>9s} {'resist./emers.':>14s} {'GM':>4s} {'celle':>5s}  taratura")
    for fcl, cl, nome in sottomarini:
        d = dict(SOTTOMARINI[nome], tipo='sottomarino', nome_critici=nome); d['dislocamento_t'] = d['emersione_t']
        mesh = modello_sottomarino(cl, os.path.dirname(os.path.abspath(fcl)))
        xs, dx, sez = sezioni(mesh)
        idro = idrostatica(xs, dx, sez)
        B = larghezza_galleggiamento(sez)
        V0 = idro[0]
        r_geo = volume_esterno(sez, dx) / V0 - 1
        rr, zz, yy, gap, Vr = scafo_resistente(cl, d, xs, dx, sez, V0)
        celle, fuori = celle_sottomarino(cl, d, xs, rr, yy)
        sez, taratura, zav = tara_riserva(d, xs, dx, sez, rr, zz, yy, celle)
        Vt = volume_esterno(sez, dx)
        dati = dict(scafo=d['scafo'], dislocamento_emersione_t=d['emersione_t'], dislocamento_immersione_t=d['immersione_t'],
                    quota_collaudo_m=d['collaudo'], quota_collasso_m=d['collasso'],
                    riserva_spinta=round(d['immersione_t'] / d['emersione_t'] - 1, 3),
                    riserva_geometrica=round(r_geo, 3), taratura=taratura, zavorra_iniziale_m3=round(zav),
                    intercapedine_m=round(gap, 2), volume_resistente_m3=round(Vr), volume_esterno_m3=round(Vt))
        scrivi(os.path.join(root, cl['uscita'], 'scafo.toml'), cl, d, xs, dx, sez, celle, [], idro, B,
               sub=dict(dati=dati, raggio_resistente=rr, z_resistente=zz, y_resistente=yy, fuori=fuori))
        print(f"{nome:12s} {V0 * 1.025:12.0f} {d['emersione_t']:7d} {Vt * 1.025:13.0f} {d['immersione_t']:7d} "
              f"{r_geo:7.2f} {dati['riserva_spinta']:6.2f} {gap:9.2f} {Vr / V0:14.2f} {d['GM']:4.1f} {len(celle):5d}  "
              f"{taratura}{f', {zav * 1.025:.0f} t' if zav else ''}; riserva {Vt / (V0 - zav) - 1:.2f}")
