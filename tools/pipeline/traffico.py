"""Rotte del traffico civile per teatro (blocco 1 del sonar, docs/20_sonar_blocco1.md §3) ->
data/motore/teatri/<teatro>.traffico.toml, letto da engine/nucleo/traffico.hpp (carica_traffico).

Le corsie e le zone sono in lat/lon. Qui si proiettano sul piano del teatro, con la stessa proiezione del fondale
(carta_epoca.Teatro), e si controllano sul fondale:
- una corsia sta in acqua di almeno il pescaggio piu' fondo dei suoi tipi piu' un metro, dentro la griglia del teatro
  (fuori non si controlla): i punti a terra o sulle secche (i porti) si agganciano all'acqua utile piu' vicina, e i
  tratti che toccano terra si instradano col percorso d'acqua piu' breve (A* sulla griglia);
- le zone di pesca diventano punti d'acqua buona, fra le profondita' della zona, ogni 3 km.

Fonti: docs/ricerca/26_navi_civili_traffico.md e gli appunti 26_note/rotte_traffico.md (sezioni citate per teatro).
Quasi tutto e' STIMA: le polilinee ricostruite dalla geografia, le portate degli anni '80 riportate dai conteggi
moderni (§0 degli appunti). Le portate sono navi al giorno per senso.

Uso: traffico.py <radice del progetto> [teatro ...]
"""
import math
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from carta_epoca import Teatro   # noqa: E402  (stessa griglia e proiezione del fondale)
from civili import TIPI   # noqa: E402  (il pescaggio dei tipi civili)

NM = 1852.0
PASSO_PESCA_M = 3000.0
PUNTI_PESCA_MAX = 160

# miscele dei mercantili in transito (appunti §0 e per teatro): tipo civile -> peso
TRANSITO = {'mercantile': 40, 'portarinfuse': 15, 'petroliera': 12, 'petroliera_costiera': 8, 'cargo': 9,
            'rimorchiatore': 1, 'nave_ricerca': 1}
TRANSITO_PETROLIERE = {**TRANSITO, 'petroliera': 15, 'petroliera_costiera': 9}   # Gibilterra, Ouessant
COSTIERO = {'mercantile': 60, 'petroliera_costiera': 25, 'rimorchiatore': 5, 'peschereccio_costiero': 10}
TRAGHETTI = {'traghetto': 1}
STRASCICO = {'peschereccio': 70, 'peschereccio_costiero': 30}
COSTA = {'peschereccio_costiero': 60, 'cianciolo': 40}
SOVIETICI = {'bmrt': 70, 'peschereccio': 30}
INVERNO = [11, 12, 1, 2, 3, 4]
ESTATE = [6, 7, 8, 9]
FUORI_ESTATE = [1, 2, 3, 4, 5, 10, 11, 12]


def corsia(nome, punti, navi, miscela, larghezza_nm=4.0, doppio=True, mesi=None, partenze=None, fondo=None):
    """fondo: l'acqua minima della corsia in metri, se e' meno del pescaggio dei tipi piu' un metro (porti e secche
    del Baltico, dove le navi di quei tipi sono comunque le piu' piccole)"""
    return dict(nome=nome, punti=punti, navi_giorno=navi, miscela=miscela, larghezza_m=larghezza_nm * NM,
                doppio_senso=doppio, mesi=mesi or [], partenze_h=partenze or [], fondo=fondo)


def zona(nome, navi, miscela, poligono=None, fascia=None, profondita=(10, 1000), velocita=3.5, mesi=None):
    """poligono: [(lat, lon), ...]; oppure fascia: (spezzata [(lat, lon)], distanza minima e massima in nm dalla
    spezzata, entrambi i lati se la minima e' negativa)"""
    return dict(nome=nome, navi=navi, miscela=miscela, poligono=poligono, fascia=fascia, profondita=profondita,
                velocita_kn=velocita, mesi=mesi or [])


# --- i teatri (lat, lon) ---
TEATRI = {
    # appunti §1: TSS dello stretto (verso ovest a nord, verso est a sud), ventagli fuori dallo schema; 130-150 navi
    # al giorno in transito, divise fra i ventagli 50/40/10; traghetti 1.4; pesca 1.3
    'gibilterra': dict(
        corsie={
            'ovest_nord': corsia("Verso l'Atlantico, Capo San Vincenzo e Nord Europa",
                                 [(36.40, -3.50), (36.10, -4.50), (36.00, -5.25), (36.005, -5.42), (35.977, -5.60),
                                  (35.960, -5.75), (35.95, -5.95), (36.10, -6.40), (36.45, -7.50), (37.0, -9.1)],
                                 35, TRANSITO_PETROLIERE, 2.0, False),
            'ovest_sud': corsia("Verso l'Atlantico, Canarie e Africa occidentale",
                                [(35.95, -3.50), (35.95, -4.50), (35.99, -5.25), (36.005, -5.42), (35.977, -5.60),
                                 (35.960, -5.75), (35.94, -5.95), (35.80, -6.40), (35.45, -7.50)],
                                28, TRANSITO_PETROLIERE, 2.0, False),
            'ovest_cadice': corsia("Verso il Golfo di Cadice (Cadice, Huelva)",
                                   [(35.30, -3.50), (35.60, -4.20), (35.98, -5.25), (36.005, -5.42), (35.977, -5.60),
                                    (35.960, -5.75), (35.96, -5.95), (36.25, -6.40), (36.45, -6.45)],
                                   7, TRANSITO, 2.0, False),
            'est_nord': corsia("Verso il Mediterraneo nord",
                               [(37.0, -9.1), (36.40, -7.50), (36.05, -6.40), (35.91, -5.95), (35.92, -5.75),
                                (35.915, -5.60), (35.95, -5.45), (35.975, -5.25), (36.08, -4.50), (36.38, -3.50)],
                               35, TRANSITO_PETROLIERE, 2.0, False),
            'est_centro': corsia("Verso il Mediterraneo centrale e Suez (asse dell'Alboran)",
                                 [(35.42, -7.50), (35.77, -6.40), (35.90, -5.95), (35.92, -5.75), (35.915, -5.60),
                                  (35.95, -5.45), (35.965, -5.25), (35.93, -4.50), (35.92, -3.50)],
                                 28, TRANSITO_PETROLIERE, 2.0, False),
            'est_africa': corsia("Verso l'Algeria e il Marocco mediterraneo",
                                 [(36.43, -6.47), (36.22, -6.40), (35.91, -5.95), (35.92, -5.75), (35.915, -5.60),
                                  (35.95, -5.45), (35.955, -5.25), (35.58, -4.20), (35.28, -3.50)],
                                 7, TRANSITO, 2.0, False),
            'algeciras_ceuta': corsia('Traghetti Algeciras-Ceuta', [(36.12, -5.42), (36.03, -5.38), (35.90, -5.31)],
                                      8, TRAGHETTI, 0.5),
            'algeciras_tangeri': corsia('Traghetti Algeciras-Tangeri',
                                        [(36.12, -5.42), (36.02, -5.45), (35.88, -5.62), (35.80, -5.79)], 4, TRAGHETTI, 0.5),
            'tarifa_tangeri': corsia('Traghetti Tarifa-Tangeri', [(36.00, -5.60), (35.90, -5.68), (35.80, -5.79)],
                                     1.5, TRAGHETTI, 0.5),
            'malaga_melilla': corsia('Traghetti Malaga-Melilla', [(36.70, -4.41), (36.30, -3.90), (35.30, -2.95)],
                                     1, TRAGHETTI, 1.0),
        },
        zone={
            'cadice': zona('Golfo di Cadice, strascico e cianciolo', 25, {**STRASCICO, 'cianciolo': 20},
                           poligono=[(36.20, -6.40), (37.05, -7.35), (36.70, -7.50), (36.00, -6.65)], profondita=(30, 400)),
            'alboran_nord': zona("Costa dell'Alboran, ciancioli di Malaga ed Estepona", 12, COSTA,
                                 fascia=([(36.42, -5.20), (36.50, -4.90), (36.70, -4.40), (36.72, -4.00)], 2, 10),
                                 profondita=(15, 200), velocita=2.5),
            'marocco': zona('Costa atlantica del Marocco', 15, {'peschereccio_costiero': 60, 'peschereccio': 40},
                            poligono=[(35.80, -6.05), (35.80, -6.60), (35.20, -6.90), (35.20, -6.30)], profondita=(20, 300)),
        }),
    # appunti §2: nessun TSS, fasci; 40-60 al giorno sull'asse, traghetti per stagione, pesca 2.3
    'otranto': dict(
        corsie={
            'adriatico_ionio': corsia("Asse dell'Adriatico, ramo Ionio e Suez",
                                      [(42.50, 17.40), (41.60, 18.10), (40.80, 18.75), (40.20, 19.00), (39.50, 19.40)],
                                      14, TRANSITO, 6.0),
            'adriatico_messina': corsia("Asse dell'Adriatico, ramo Messina e Mediterraneo occidentale",
                                        [(42.50, 17.40), (41.60, 18.10), (40.80, 18.75), (40.20, 19.00), (39.75, 18.55),
                                         (39.50, 18.10)], 11, TRANSITO, 6.0),
            'costiero': corsia('Costiero pugliese', [(41.05, 17.20), (40.75, 17.85), (40.45, 18.25), (40.12, 18.62),
                                                     (39.75, 18.45)], 5, COSTIERO, 2.0),
            'albania': corsia('Durazzo verso il largo', [(41.30, 19.40), (41.20, 18.70), (41.00, 18.40)], 0.3,
                              {'mercantile': 1}, 2.0),
            'brindisi_grecia_inverno': corsia('Traghetti Brindisi-Corfu-Igoumenitsa',
                                              [(40.66, 18.00), (40.35, 18.70), (39.85, 19.55), (39.78, 19.90), (39.62, 19.95)],
                                              2, TRAGHETTI, 2.0, mesi=FUORI_ESTATE),
            'brindisi_grecia_estate': corsia('Traghetti Brindisi-Corfu-Igoumenitsa (estate)',
                                             [(40.66, 18.00), (40.35, 18.70), (39.85, 19.55), (39.78, 19.90), (39.62, 19.95)],
                                             10, TRAGHETTI, 2.0, mesi=ESTATE),
            'bari_grecia': corsia('Traghetti Bari-Grecia', [(41.18, 16.95), (40.80, 17.80), (40.25, 18.90), (39.85, 19.55),
                                                            (39.78, 19.90)], 1, TRAGHETTI, 2.0),
            'bari_dubrovnik': corsia('Traghetti Bari-Dubrovnik', [(41.14, 16.90), (41.70, 17.60), (42.60, 18.05)],
                                     0.5, TRAGHETTI, 2.0),
            'bari_bar': corsia('Traghetti Bari-Bar', [(41.14, 16.90), (41.60, 18.00), (42.07, 19.06)], 0.5, TRAGHETTI, 2.0),
        },
        zone={
            'fossa_sud_adriatica': zona('Scarpata della Fossa Sud-Adriatica', 25, STRASCICO,
                                        poligono=[(41.90, 17.30), (41.90, 18.30), (41.00, 18.30), (41.00, 17.30)],
                                        profondita=(150, 900)),
            'piattaforma_pugliese': zona('Piattaforma pugliese', 30, {**STRASCICO, 'cianciolo': 25},
                                         fascia=([(41.13, 16.90), (40.95, 17.30), (40.65, 17.98), (40.15, 18.50)], 2, 20),
                                         profondita=(30, 200)),
            'scarpata_ionica': zona('Scarpata ionica di Leuca e Gallipoli', 10, STRASCICO,
                                    poligono=[(39.95, 18.10), (39.95, 18.45), (39.50, 18.60), (39.50, 18.20)],
                                    profondita=(300, 800)),
            'epiro': zona('Costa di Corfu e dell\'Epiro, lampare', 10, COSTA,
                          fascia=([(39.90, 19.65), (39.70, 19.95), (39.45, 20.15)], 1, 5), profondita=(10, 150),
                          velocita=1.5),
        }),
    # appunti §3: Ouessant a est del riquadro; nel riquadro i flussi atlantici (25-45 al giorno) e la pesca al ciglio
    'celtica': dict(
        corsie={
            'ouessant_nordamerica': corsia('Ouessant-Nord America', [(48.70, -5.50), (49.15, -8.52), (49.52, -11.49),
                                                                    (49.60, -12.50)], 6, TRANSITO_PETROLIERE, 10.0),
            'ouessant_caraibi': corsia('Ouessant-Azzorre, Caraibi e Sud America', [(48.40, -5.80), (47.94, -7.15),
                                                                                  (47.65, -10.68), (47.20, -12.50)],
                                       6, TRANSITO_PETROLIERE, 10.0),
            'irlanda_iberia': corsia('Irlanda-Finisterre, Lisbona e Mediterraneo', [(51.0, -9.6), (49.5, -9.6), (47.0, -9.6),
                                                                                   (46.0, -9.6)], 5, TRANSITO, 10.0),
            'mare_irlanda_finisterre': corsia("Mare d'Irlanda e San Giorgio-Finisterre",
                                              [(50.5, -7.4), (49.5, -7.6), (48.0, -7.9), (47.0, -8.3), (46.0, -8.6)],
                                              7, TRANSITO, 10.0),
        },
        zone={
            'ciglio': zona('Ciglio della piattaforma, spagnoli del Gran Sol e francesi', 40, STRASCICO,
                           fascia=([(49.5, -11.6), (49.0, -11.0), (48.5, -9.8), (48.0, -8.8), (47.5, -7.9), (47.2, -7.5)],
                                   -15, 15), profondita=(100, 700)),
            'germon': zona('Tonno bianco sopra la scarpata', 25, {'peschereccio_costiero': 70, 'peschereccio': 30},
                           poligono=[(47.0, -12.0), (48.5, -12.0), (48.0, -9.5), (47.0, -9.0)], profondita=(300, 5000),
                           velocita=5.0, mesi=[6, 7, 8, 9, 10]),
        }),
    # appunti §4: Route T, Kiel, Travemunde, Sund e Bornholm; traghetti fitti; ghiaccio nel 1985-87
    'baltico': dict(
        corsie={
            'route_t': corsia('Route T, dal Grande Belt a Bornholm',
                              [(55.30, 11.05), (55.00, 10.95), (54.75, 11.05), (54.62, 11.30), (54.47, 11.95),
                               (54.45, 12.18), (54.78, 12.72), (54.85, 13.40), (55.20, 14.30), (55.30, 15.20)],
                              20, COSTIERO | {'mercantile': 40, 'portarinfuse': 5, 'petroliera_costiera': 15, 'cargo': 5}, 2.0),
            'kiel_bornholm': corsia('Kiel, Fehmarn Belt, Kadetrinne e Bornholm',
                                    [(54.45, 10.20), (54.50, 10.50), (54.55, 10.90), (54.60, 11.20), (54.47, 11.95),
                                     (54.45, 12.18), (54.78, 12.72), (54.85, 13.40), (55.20, 14.30), (55.30, 15.20)],
                                    25, COSTIERO | {'mercantile': 45}, 2.0, fondo=8.0),
            'lubecca': corsia('Lubecca e Travemunde verso il Baltico',
                              [(53.97, 10.90), (54.20, 11.10), (54.55, 11.25), (54.47, 11.95), (54.45, 12.18),
                               (54.78, 12.72), (54.85, 13.40)], 10, COSTIERO, 2.0),
            'rostock': corsia('Rostock e Warnemunde', [(54.19, 12.08), (54.35, 12.10), (54.47, 12.20), (54.78, 12.72),
                                                       (54.85, 13.40), (55.20, 14.30)], 8, COSTIERO, 2.0),
            'sund_bornholm': corsia('Sund verso Bornholmsgat', [(55.60, 12.70), (55.30, 12.78), (55.05, 13.30),
                                                                (55.20, 14.30), (55.30, 15.20)],
                                    30, COSTIERO | {'mercantile': 45}, 2.0, fondo=8.0),
            'sund_stettino': corsia('Sund verso Swinoujscie', [(55.60, 12.70), (55.30, 12.78), (54.95, 12.90),
                                                               (54.60, 13.70), (54.10, 14.25), (53.95, 14.28)], 8, COSTIERO, 2.0, fondo=8.0),
            'rompighiaccio': corsia('Rompighiaccio sulle rotte dei porti tedeschi',
                                    [(54.19, 12.08), (54.35, 12.10), (54.47, 12.20), (54.60, 11.20), (54.20, 11.10),
                                     (53.97, 10.90)], 1, {'rompighiaccio': 1}, 1.0, mesi=[1, 2, 3]),
            'puttgarden_rodby': corsia('Traghetti Puttgarden-Rodby', [(54.50, 11.23), (54.58, 11.29), (54.65, 11.35)],
                                       30, TRAGHETTI, 0.5),
            'warnemunde_gedser': corsia('Traghetti Warnemunde-Gedser', [(54.19, 12.08), (54.40, 12.00), (54.56, 11.93)],
                                        4, TRAGHETTI, 0.5, fondo=5.0),
            'sassnitz_trelleborg': corsia('Traghetti Sassnitz-Trelleborg', [(54.51, 13.65), (54.80, 13.45), (55.36, 13.15)],
                                          4, TRAGHETTI, 1.0),
            'mukran_klaipeda': corsia('Traghetti ferroviari Mukran-Klaipeda', [(54.48, 13.63), (54.60, 14.00), (54.95, 15.20)],
                                      1, TRAGHETTI, 1.0),
            'travemunde_trelleborg': corsia('Traghetti Travemunde-Trelleborg', [(53.97, 10.90), (54.30, 11.30), (54.55, 11.80),
                                                                                (54.85, 12.60), (55.36, 13.15)],
                                            2, TRAGHETTI, 1.0, fondo=5.0),
            'travemunde_gedser': corsia('Traghetti Travemunde-Gedser', [(53.97, 10.90), (54.30, 11.35), (54.56, 11.93)],
                                        3, TRAGHETTI, 1.0, fondo=5.0),
            'grande_belt': corsia('Traghetti del Grande Belt', [(55.34, 11.12), (55.32, 11.00), (55.30, 10.88)], 25,
                                  TRAGHETTI, 0.5),
            'spodsbjerg_tars': corsia('Traghetti Spodsbjerg-Tars', [(54.93, 10.83), (54.90, 10.95), (54.88, 11.05)], 15,
                                      TRAGHETTI, 0.3, fondo=5.0),
            'ronne_ystad': corsia('Traghetti Ronne-Ystad', [(55.10, 14.68), (55.30, 14.20), (55.42, 13.82)], 2.5,
                                  TRAGHETTI, 1.0),
            'ronne_copenaghen': corsia('Traghetti Ronne-Copenaghen', [(55.10, 14.68), (55.20, 14.00), (55.30, 13.20),
                                                                      (55.60, 12.75)], 1.5, TRAGHETTI, 1.0),
            'swinoujscie_ystad': corsia('Traghetti Swinoujscie-Ystad', [(53.95, 14.28), (54.30, 14.20), (54.90, 13.95),
                                                                        (55.42, 13.82)], 1, TRAGHETTI, 1.0),
        },
        zone={
            'arkona_bornholm': zona('Bacini di Arkona e Bornholm, merluzzo, aringa e spratto', 25,
                                    {'peschereccio_costiero': 60, 'peschereccio': 40},
                                    poligono=[(55.60, 13.50), (55.60, 15.00), (54.60, 15.00), (54.60, 13.50)],
                                    profondita=(25, 100)),
            'kiel_mecklemburgo': zona('Baia di Kiel, Fehmarn e Mecklemburgo', 12, {'peschereccio_costiero': 1},
                                      poligono=[(54.65, 10.60), (54.65, 11.00), (54.15, 11.80), (54.30, 12.40), (54.15, 12.40),
                                                (54.00, 11.20)], profondita=(10, 40), velocita=2.5),
        }),
    # appunti §5: le corsie dal Golfo di Kola, i banchi del Murman, i rompighiaccio da novembre a giugno
    'barents': dict(
        corsie={
            'kola_mar_bianco': corsia('Kola verso il Mar Bianco',
                                      [(69.32, 33.62), (69.35, 33.80), (69.42, 35.20),
                                       (69.28, 36.50), (68.95, 37.80), (68.55, 39.00)], 5,
                                      {'mercantile': 40, 'portarinfuse': 25, 'petroliera_costiera': 15, 'bmrt': 10,
                                       'rimorchiatore': 5, 'nave_ricerca': 5}, 4.0),
            'kola_mare_del_nord': corsia('Kola verso la Rotta del Mare del Nord',
                                         [(69.32, 33.62), (69.45, 33.75), (69.74, 36.59),
                                          (69.99, 39.00), (70.20, 41.00)], 3,
                                         {'portarinfuse': 45, 'mercantile': 30, 'petroliera_costiera': 20, 'nave_ricerca': 5}, 6.0),
            'kola_ovest': corsia('Kola verso Vardo e l\'Europa',
                                 [(69.32, 33.62), (69.45, 33.75), (69.79, 32.84),
                                  (70.13, 31.90), (70.24, 31.20)], 4,
                                 {'mercantile': 40, 'bmrt': 30, 'portarinfuse': 15, 'petroliera_costiera': 15}, 6.0),
            'kola_svalbard': corsia('Kola verso Svalbard e i banchi del nord',
                                    [(69.32, 33.62), (69.45, 33.75), (70.60, 32.25),
                                     (71.74, 30.57)], 1.5, {'mercantile': 40, 'bmrt': 60}, 6.0),
            'rompighiaccio': corsia('Rompighiaccio verso il Mar Bianco e il Kara',
                                    [(69.32, 33.62), (69.45, 33.75), (69.74, 36.59),
                                     (69.99, 39.00)], 0.5, {'rompighiaccio': 1}, 3.0, mesi=INVERNO + [5, 6]),
        },
        zone={
            'banchi_murman': zona('Banchi della costa del Murman', 25, SOVIETICI,
                                  poligono=[(70.30, 32.00), (70.30, 37.00), (69.45, 37.00), (69.45, 32.00)],
                                  profondita=(80, 300)),
            'zona_grigia': zona('Zona grigia', 8, {'bmrt': 70, 'peschereccio': 30},
                                poligono=[(72.00, 32.10), (72.00, 37.00), (70.60, 37.00), (70.60, 32.10)],
                                profondita=(150, 400)),
        }),
    # appunti §6: le rotte del varco e dei traghetti faroesi; pesca sulla piattaforma, sul Banco, sul ciglio
    'faroe': dict(
        corsie={
            'fair_isle_reykjavik': corsia('Fair Isle-Reykjavik', [(60.40, -4.50), (60.57, -5.04), (60.91, -6.24),
                                                                   (61.15, -7.10), (61.55, -8.71), (61.85, -9.98),
                                                                   (62.10, -11.0)], 2.5, TRANSITO, 10.0),
            'pentland_reykjavik': corsia('Pentland Firth-Reykjavik', [(60.20, -6.40), (60.50, -7.55), (60.66, -8.82),
                                                                      (61.02, -9.94), (61.30, -11.0)], 1.5, TRANSITO, 10.0),
            'bergen_islanda': corsia('Bergen e Norvegia-Islanda', [(62.20, -3.00), (62.40, -4.36), (62.92, -7.70),
                                                                   (63.36, -11.15)], 1, TRANSITO, 10.0),
            'transito_pesca': corsia('Pescherecci sovietici verso l\'Atlantico occidentale',
                                     [(62.50, -3.00), (62.40, -4.36), (62.20, -6.00), (61.85, -9.98), (61.70, -11.0)],
                                     2, SOVIETICI, 15.0),
            'torshavn_scozia': corsia('Torshavn-Scrabster e Aberdeen', [(62.00, -6.74), (61.85, -6.55), (61.54, -6.18),
                                                                       (61.12, -5.78), (60.50, -5.20), (60.20, -4.80)],
                                      0.5, {'mercantile': 60, 'traghetto': 40}, 3.0),
            'torshavn_lerwick': corsia('Torshavn-Lerwick, Bergen e Danimarca', [(62.00, -6.74), (61.95, -6.55),
                                                                               (61.52, -5.17), (61.30, -4.46), (61.20, -3.50)],
                                       0.5, {'mercantile': 50, 'traghetto': 50}, 3.0),
            'torshavn_seydisfjordur': corsia('Torshavn-Seydisfjordur (Norrona)', [(62.00, -6.74), (62.10, -6.60),
                                                                                  (62.38, -7.42), (62.81, -8.27),
                                                                                  (63.24, -9.14), (63.50, -9.70)],
                                             0.15, TRAGHETTI, 2.0, mesi=[5, 6, 7, 8, 9]),
        },
        zone={
            'piattaforma': zona('Piattaforma delle Faroe, palangari e strascico', 15,
                                {'peschereccio_costiero': 50, 'peschereccio': 50},
                                poligono=[(62.70, -7.80), (62.70, -6.00), (61.20, -6.00), (61.20, -7.80)],
                                profondita=(60, 200)),
            'banco': zona('Banco delle Faroe', 4, {'peschereccio_costiero': 50, 'peschereccio': 50},
                          poligono=[(61.10, -9.00), (61.10, -8.00), (60.70, -8.00), (60.70, -9.00)], profondita=(80, 200)),
            'pesce_azzurro': zona('Pesce azzurro sulla scarpata ovest', 12, SOVIETICI,
                                  poligono=[(62.50, -9.00), (62.50, -8.00), (61.40, -8.50), (61.40, -9.60)],
                                  profondita=(300, 1000), velocita=4.0, mesi=[1, 2, 3, 4]),
        }),
    # appunti §7: poco traffico mercantile, la massa e' la pesca per stagione
    'kunashir': dict(
        corsie={
            'korsakov_ekaterina': corsia('Korsakov-Stretto di Ekaterina-Yuzhno-Kurilsk',
                                         [(45.60, 146.00), (44.70, 146.55), (44.43, 146.72), (44.10, 146.30), (44.02, 145.90)],
                                         1.5, {'mercantile': 60, 'petroliera_costiera': 20, 'bmrt': 20}, 4.0),
            'curili_shikotan': corsia('Yuzhno-Kurilsk-Malokurilskoye', [(44.02, 145.90), (43.95, 146.30), (43.87, 146.80)],
                                      1, {'mercantile': 50, 'bmrt': 50}, 2.0),
            'ekaterina_pacifico': corsia('Stretto di Ekaterina verso il Pacifico',
                                         [(44.43, 146.72), (44.00, 147.20), (43.50, 148.00), (43.20, 148.60)],
                                         1, {'mercantile': 60, 'portarinfuse': 20, 'petroliera_costiera': 20}, 6.0),
            'kurilsk': corsia('Ekaterina-Kurilsk', [(44.43, 146.72), (44.60, 146.90), (45.00, 147.40), (45.23, 147.80)],
                              0.5, {'mercantile': 60, 'bmrt': 40}, 2.0),
            'nemuro': corsia('Costiero giapponese di Nemuro', [(44.02, 145.22), (43.66, 145.15), (43.58, 145.32),
                                                               (43.40, 145.50), (43.33, 145.60)],
                             6, {'peschereccio_costiero': 70, 'mercantile': 30}, 1.0),
        },
        zone={
            'saury': zona('Saury (sanma) del lato Pacifico', 60, {'peschereccio_costiero': 80, 'cianciolo': 20},
                          poligono=[(44.30, 146.40), (44.30, 148.50), (43.30, 148.50), (43.30, 146.00)],
                          profondita=(100, 6000), velocita=1.5, mesi=[8, 9, 10, 11]),
            'kombu': zona('Kombu delle Habomai', 40, {'peschereccio_costiero': 1},
                          poligono=[(43.55, 145.75), (43.55, 146.10), (43.35, 146.10), (43.35, 145.75)],
                          profondita=(3, 40), velocita=1.0, mesi=[9]),
            'granchio': zona('Granchio di Shikotan e delle Habomai', 10, {'peschereccio_costiero': 70, 'bmrt': 30},
                             poligono=[(43.95, 146.50), (43.95, 147.10), (43.60, 147.10), (43.60, 146.40)],
                             profondita=(30, 300), mesi=[4, 5, 9, 10]),
            # il grosso e' nell'Okhotsk nord-occidentale, fuori dal riquadro: qui la scarpata di Iturup
            'merluzzo_okhotsk': zona("Merluzzo dell'Alaska sulla scarpata di Iturup", 10, {'bmrt': 1},
                                     poligono=[(45.45, 146.50), (45.45, 148.00), (44.60, 148.00), (44.60, 146.50)],
                                     profondita=(100, 1500), mesi=[1, 2, 3, 4]),
            'salmone': zona('Salmone, reti e palangari', 15, {'peschereccio_costiero': 1},
                            poligono=[(44.30, 145.30), (44.30, 146.20), (43.80, 146.20), (43.80, 145.40)],
                            profondita=(20, 500), velocita=2.0, mesi=[5, 6, 7, 8]),
        }),
}


def proietta(T, punti):
    lat = np.array([p[0] for p in punti]); lon = np.array([p[1] for p in punti])
    x, y = T.proj(lon, lat)
    return np.stack([np.asarray(x, float), np.asarray(y, float)], axis=1)


def profondita(T, x, y):
    """Profondita' in metri (positiva in acqua, <= 0 a terra); None fuori dalla griglia."""
    c = int((x + T.w * T.cella / 2) // T.cella); r = int((T.hh * T.cella / 2 - y) // T.cella)
    if c < 0 or r < 0 or c >= T.w or r >= T.hh:
        return None
    return -float(T.z[r, c])


def libero(T, a, b, minimo):
    """Il tratto da a a b sta in acqua di almeno `minimo` metri (dove esce dalla griglia non si controlla)."""
    n = max(1, int(np.hypot(*(b - a)) // (T.cella / 2)))
    for k in range(n + 1):
        x, y = a + (b - a) * k / n
        z = profondita(T, x, y)
        if z is not None and z < minimo:
            return False
    return True


def cella_di(T, x, y):
    return int((T.hh * T.cella / 2 - y) // T.cella), int((x + T.w * T.cella / 2) // T.cella)


def xy_di(T, r, c):
    return np.array([(c + 0.5) * T.cella - T.w * T.cella / 2, T.hh * T.cella / 2 - (r + 0.5) * T.cella])


_ACQUA = {}


def acqua(T, minimo):
    """La griglia dell'acqua utile e, per ogni cella, la cella utile piu' vicina."""
    if (T.id, minimo) not in _ACQUA:
        from scipy import ndimage
        utile = -T.z >= minimo
        _, (rr, cc) = ndimage.distance_transform_edt(~utile, return_indices=True)
        _ACQUA[(T.id, minimo)] = (utile, rr, cc)
    return _ACQUA[(T.id, minimo)]


def aggancia(T, p, minimo, raggio_m=15000.0):
    """Il punto, o il centro della cella utile piu' vicina (entro il raggio); fuori dalla griglia resta com'e'."""
    r, c = cella_di(T, *p)
    if not (0 <= r < T.hh and 0 <= c < T.w):
        return p
    utile, rr, cc = acqua(T, minimo)
    if utile[r, c]:
        return p
    q = xy_di(T, rr[r, c], cc[r, c])
    return q if np.hypot(*(q - p)) <= raggio_m else None


def instrada(T, a, b, minimo, margine_m=30000.0, passo=2):
    """Il percorso d'acqua piu' breve fra a e b (A* su celle di passo x cella, nel riquadro dei due con un margine),
    ridotto ai vertici che servono; None se non c'e'."""
    import heapq
    utile, _, _ = acqua(T, minimo)
    (r0, c0), (r1, c1) = cella_di(T, *a), cella_di(T, *b)
    m = int(margine_m // T.cella)
    rmin, rmax = max(min(r0, r1) - m, 0), min(max(r0, r1) + m, T.hh - 1)
    cmin, cmax = max(min(c0, c1) - m, 0), min(max(c0, c1) + m, T.w - 1)
    griglia = utile[rmin:rmax + 1:passo, cmin:cmax + 1:passo]
    H, W = griglia.shape
    da = (min((r0 - rmin) // passo, H - 1), min((c0 - cmin) // passo, W - 1))
    a_ = (min((r1 - rmin) // passo, H - 1), min((c1 - cmin) // passo, W - 1))
    griglia[da] = griglia[a_] = True
    h = lambda q: math.hypot(q[0] - a_[0], q[1] - a_[1])
    aperti, padre, costo = [(h(da), da)], {da: None}, {da: 0.0}
    while aperti:
        _, q = heapq.heappop(aperti)
        if q == a_:
            break
        for dr in (-1, 0, 1):
            for dc in (-1, 0, 1):
                n = (q[0] + dr, q[1] + dc)
                if (dr or dc) and 0 <= n[0] < H and 0 <= n[1] < W and griglia[n]:
                    nc = costo[q] + math.hypot(dr, dc)
                    if nc < costo.get(n, 1e18):
                        costo[n], padre[n] = nc, q
                        heapq.heappush(aperti, (nc + h(n), n))
    if a_ not in padre:
        return None
    celle = []
    q = a_
    while q is not None:
        celle.append(q)
        q = padre[q]
    celle.reverse()
    pts = [a] + [xy_di(T, rmin + r * passo, cmin + c * passo) for r, c in celle[1:-1]] + [b]
    # i soli vertici che servono: si spezza il tratto dove non e' libero
    def riduci(i, j):
        if j - i <= 1 or libero(T, pts[i], pts[j], minimo):
            return [i]
        k = (i + j) // 2
        return riduci(i, k) + riduci(k, j)
    return [pts[i] for i in riduci(0, len(pts) - 1)] + [b]


def sistema_corsia(T, nome, xy, minimo):
    """La corsia in acqua utile: i punti agganciati all'acqua, i tratti che toccano terra o secche instradati."""
    problemi, punti = [], []
    for p in xy:
        q = aggancia(T, p, minimo)
        if q is None:
            lon, lat = T.proj(*p, inverse=True)
            problemi.append(f'{nome}: nessuna acqua di {minimo:.0f} m entro 15 km da {lat:.3f}, {lon:.3f}')
            q = p
        punti.append(q)
    out = [punti[0]]
    for a, b in zip(punti[:-1], punti[1:]):
        if libero(T, a, b, minimo):
            out.append(b)
            continue
        via = instrada(T, a, b, minimo)
        if via is None:
            lon, lat = T.proj(*a, inverse=True)
            problemi.append(f'{nome}: nessun passaggio di {minimo:.0f} m da {lat:.3f}, {lon:.3f}')
            out.append(b)
        else:
            out += via[1:]
    return np.array(out), problemi


def dentro(poligono, x, y):
    c = False
    for (x1, y1), (x2, y2) in zip(poligono, np.roll(poligono, -1, axis=0)):
        if (y1 > y) != (y2 > y) and x < (x2 - x1) * (y - y1) / (y2 - y1) + x1:
            c = not c
    return c


def punti_di_pesca(T, z):
    lo, hi = z['profondita']
    candidati = []
    if z['poligono']:
        pol = proietta(T, z['poligono'])
        x0, y0 = pol.min(axis=0); x1, y1 = pol.max(axis=0)
        prova = lambda x, y: dentro(pol, x, y)
    else:
        spezzata, d_min, d_max = z['fascia']
        sp = proietta(T, spezzata)
        x0, y0 = sp.min(axis=0) - d_max * NM; x1, y1 = sp.max(axis=0) + d_max * NM

        def prova(x, y):   # una minima negativa: da 0 (i due lati della spezzata); il lato giusto lo fa il fondale
            d = min(distanza_segmento(x, y, a, b) for a, b in zip(sp[:-1], sp[1:]))
            return max(d_min, 0) * NM <= d <= d_max * NM
    for x in np.arange(x0, x1, PASSO_PESCA_M):
        for y in np.arange(y0, y1, PASSO_PESCA_M):
            if not prova(x, y):
                continue
            p = profondita(T, x, y)
            if p is not None and lo <= p <= hi:
                candidati.append((round(float(x)), round(float(y))))
    if len(candidati) > PUNTI_PESCA_MAX:   # a passo regolare, sempre gli stessi
        passo = len(candidati) / PUNTI_PESCA_MAX
        candidati = [candidati[int(i * passo)] for i in range(PUNTI_PESCA_MAX)]
    return candidati


def distanza_segmento(x, y, a, b):
    ab = b - a; t = np.clip(((x - a[0]) * ab[0] + (y - a[1]) * ab[1]) / max(ab @ ab, 1e-9), 0.0, 1.0)
    return float(np.hypot(x - a[0] - t * ab[0], y - a[1] - t * ab[1]))


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, str):
        return '"' + v.replace('"', '\\"') + '"'
    if isinstance(v, (list, tuple)):
        return '[' + ', '.join(valore(x) for x in v) + ']'
    if isinstance(v, int):
        return str(v)
    return repr(round(float(v), 3))


def esporta_teatro(radice, teatro):
    T = Teatro(radice, teatro)
    dati = TEATRI[teatro]
    righe = [f'# Traffico civile del teatro {teatro} (generato da tools/pipeline/traffico.py: non modificare a mano).',
             '# x, y in metri sul piano del teatro (come il fondale); navi_giorno per senso. Fonti e confidenza:',
             '# docs/ricerca/26_navi_civili_traffico.md (quasi tutto STIMA).', '', f'teatro = "{teatro}"', '']
    problemi = []
    for k, c in dati['corsie'].items():
        # l'acqua che serve: il pescaggio piu' fondo dei tipi della corsia, piu' un metro
        minimo = c['fondo'] or max(TIPI[t]['pescaggio_m'][1] for t in c['miscela']) + 1.0
        xy, p = sistema_corsia(T, k, proietta(T, c['punti']), minimo)
        problemi += p
        righe += [f'[corsie.{k}]', f'nome = {valore(c["nome"])}',
                  'punti = [' + ', '.join(f'[{round(float(x))}, {round(float(y))}]' for x, y in xy) + ']',
                  f'larghezza_m = {round(c["larghezza_m"])}', f'doppio_senso = {valore(c["doppio_senso"])}',
                  f'navi_giorno = {valore(float(c["navi_giorno"]))}',
                  'miscela = { ' + ', '.join(f'{t} = {valore(float(w))}' for t, w in c['miscela'].items()) + ' }']
        if c['partenze_h']:
            righe.append(f'partenze_h = {valore([float(h) for h in c["partenze_h"]])}')
        if c['mesi']:
            righe.append(f'mesi = {valore(c["mesi"])}')
        righe.append('')
    for k, z in dati['zone'].items():
        punti = punti_di_pesca(T, z)
        if len(punti) < 5:
            problemi.append(f'zona {k}: solo {len(punti)} punti fra {z["profondita"][0]} e {z["profondita"][1]} m')
        righe += [f'[zone.{k}]', f'nome = {valore(z["nome"])}', f'navi = {valore(float(z["navi"]))}',
                  f'velocita_kn = {valore(float(z["velocita_kn"]))}',
                  'miscela = { ' + ', '.join(f'{t} = {valore(float(w))}' for t, w in z['miscela'].items()) + ' }',
                  'punti = [' + ', '.join(f'[{x}, {y}]' for x, y in punti) + ']']
        if z['mesi']:
            righe.append(f'mesi = {valore(z["mesi"])}')
        righe.append('')
    open(os.path.join(radice, 'data', 'motore', 'teatri', f'{teatro}.traffico.toml'), 'w').write('\n'.join(righe))
    return problemi


def esporta_traffico(radice, teatri=None):
    problemi = []
    for t in teatri or TEATRI:
        problemi += [f'{t}: {p}' for p in esporta_teatro(radice, t)]
    return problemi


if __name__ == '__main__':
    argomenti = sys.argv[1:]
    radice = os.path.abspath(argomenti[0] if argomenti else os.path.join(os.path.dirname(__file__), '..', '..'))
    problemi = esporta_traffico(radice, argomenti[1:] or None)
    for p in problemi:
        print('ATTENZIONE', p)
    print('teatri col traffico:', ' '.join(argomenti[1:] or TEATRI), '-', len(problemi), 'problemi')
