"""I danni critici per classe (fase 2, kit 31/10/2026): depositi con la carica netta (NEQ, kg di alto esplosivo) e la
protezione (0-1), il carburante (tonnellate, tipo) e le armi in coperta con la loro carica. Dati raccolti dai dossier
07, 22, 36, 01, 05, 38 e dai file del motore (nota kit-depositi-carburante-dati.md); la frazione d'esplosivo delle
testate, il propellente, le protezioni e tutto il carburante sono STIMA. Posizioni in metri dalla prua, come CLASSI.
Si sovrappone a CLASSI di compartimenti.py: `depositi` e `ponte` sostituiscono quelli di la', `carburante` e' nuovo.
"""

CRITICI = {
    'perry': dict(depositi=[(22, 38, 'missili', 1300, 0.5), (78, 88, 'proietti', 510, 0.3), (104, 112, 'siluri', 792, 0.3)], carburante=[(50, 105, 'gasolio', 600), (108, 118, 'jp5', 60)], ponte=[(95, 100, 'mk32', 264)]),
    'kidd': dict(depositi=[(26, 38, 'proietti', 1820, 0.6), (40, 52, 'missili', 752, 0.6), (144, 156, 'missili', 1252, 0.6), (155, 164, 'proietti', 1820, 0.6), (120, 126, 'siluri', 616, 0.3)], carburante=[(60, 130, 'gasolio', 1500), (117, 132, 'jp5', 60)], ponte=[(79, 86, 'harpoon', 800)]),
    'spruance': dict(depositi=[(26, 38, 'proietti', 1820, 0.5), (42, 52, 'missili', 704, 0.5), (140, 150, 'missili', 240, 0.3), (152, 163, 'proietti', 1820, 0.5), (120, 126, 'siluri', 616, 0.3)], carburante=[(60, 130, 'gasolio', 1400), (117, 132, 'jp5', 60)], ponte=[(78, 85, 'harpoon', 800), (44, 50, 'asroc', 352), (143, 149, 'sea_sparrow', 120)]),
    'ticonderoga': dict(depositi=[(26, 34, 'proietti', 2170, 0.6), (35, 46, 'missili', 1290, 0.6), (143, 154, 'missili', 1290, 0.6), (155, 163, 'proietti', 2170, 0.6), (118, 124, 'siluri', 1056, 0.3)], carburante=[(60, 130, 'gasolio', 1700), (104, 117, 'jp5', 60)], ponte=[(163, 169, 'harpoon', 800)]),
    'knox': dict(depositi=[(18, 30, 'proietti', 2240, 0.5), (32, 42, 'missili', 704, 0.5), (88, 95, 'siluri', 704, 0.3)], carburante=[(50, 90, 'nafta', 750), (95, 103, 'jp5', 20)], ponte=[(36, 42, 'asroc', 464), (123, 130, 'sea_sparrow', 120)]),
    'iowa': dict(depositi=[(58, 80, 'proietti', 17600, 0.9), (80, 100, 'proietti', 17600, 0.9), (100, 112, 'proietti', 8910, 0.85), (176, 182, 'proietti', 8910, 0.85), (182, 204, 'proietti', 17600, 0.9)], carburante=[(55, 205, 'gasolio', 7500)], ponte=[(100, 175, 'tomahawk', 6560), (150, 165, 'harpoon', 1600)]),
    'nimitz': dict(depositi=[(80, 108, 'bombe', 312000, 0.8), (232, 260, 'bombe', 312000, 0.8)], carburante=[(60, 280, 'jp5', 9000), (110, 230, 'gasolio', 300)], ponte=[(54, 78, 'sea_sparrow', 120), (290, 310, 'sea_sparrow', 240)]),
    'kitty_hawk': dict(depositi=[(78, 105, 'bombe', 225000, 0.8), (226, 252, 'bombe', 225000, 0.8)], carburante=[(58, 272, 'nafta', 7800), (58, 272, 'jp5', 5600)], ponte=[(52, 76, 'sea_sparrow', 120), (282, 302, 'sea_sparrow', 240)]),
    'sacramento': dict(depositi=[(70, 110, 'munizioni', 540000, 0.5)], carburante=[(25, 165, 'gasolio_carico', 16500), (25, 165, 'jp5_carico', 7400)], ponte=[(29, 36, 'sea_sparrow', 120)]),
    'type22': dict(depositi=[(28, 38, 'missili', 60, 0.5), (106, 114, 'missili', 60, 0.3), (100, 106, 'siluri', 600, 0.3)], carburante=[(60, 110, 'gasolio', 650), (106, 117, 'avio', 30)], ponte=[(36, 42, 'exocet', 300), (100, 106, 'stws', 264), (30, 34, 'sea_wolf', 30), (108, 114, 'sea_wolf', 30)]),
    'type22b2': dict(depositi=[(38, 48, 'missili', 60, 0.5), (118, 126, 'missili', 60, 0.3), (110, 116, 'siluri', 600, 0.3)], carburante=[(70, 120, 'gasolio', 750), (115, 125, 'avio', 30)], ponte=[(30, 36, 'exocet', 300), (108, 114, 'stws', 264), (40, 44, 'sea_wolf', 30), (115, 125, 'sea_wolf', 30)]),
    'oslo': dict(depositi=[(18, 26, 'proietti', 986, 0.5), (28, 36, 'razzi', 1440, 0.3), (66, 74, 'proietti', 986, 0.5)], carburante=[(40, 66, 'nafta', 250)], ponte=[(73, 78, 'sea_sparrow', 120), (84, 91, 'penguin', 324), (56, 62, 'mk32', 264)]),
    'stockholm': dict(depositi=[(10, 17, 'proietti', 224, 0.3), (40, 44, 'proietti', 108, 0.2)], carburante=[(20, 40, 'gasolio', 40)], ponte=[(30, 46, 'rbs15', 800), (36, 44, 'siluri_400', 180), (16, 20, 'elma', 151)]),
    'hauk': dict(depositi=[(8, 13, 'proietti', 108, 0.2)], carburante=[(18, 30, 'gasolio', 15)], ponte=[(24, 36, 'penguin', 324), (30, 36, 'siluri_533', 480)]),
    'krivak2': dict(depositi=[(26, 44, 'missili+razzi', 3432, 0.5), (82, 90, 'missili', 120, 0.5), (92, 108, 'proietti', 1071, 0.5)], carburante=[(50, 95, 'gasolio', 550)], ponte=[(8, 20, 'rastrub', 400), (62, 70, 'siluri_533', 2000)]),
    'kara': dict(depositi=[(18, 34, 'razzi', 3312, 0.5), (30, 46, 'missili', 3200, 0.5), (76, 86, 'proietti', 800, 0.5), (88, 98, 'missili', 240, 0.5), (130, 150, 'missili+razzi', 10300, 0.5)], carburante=[(85, 130, 'gasolio', 1800)], ponte=[(50, 58, 'rastrub', 800), (110, 120, 'siluri_533', 2620)]),
    'kirov': dict(depositi=[(20, 35, 'missili+razzi', 9776, 0.7), (35, 62, 'missili', 10360, 0.7), (180, 188, 'siluri', 2500, 0.3), (188, 210, 'proietti+razzi+missili', 7791, 0.6), (210, 215, 'siluri+bombe', 1860, 0.3)], carburante=[(110, 200, 'nafta', 2500)], ponte=[]),
    'slava': dict(depositi=[(25, 38, 'proietti', 4200, 0.5), (140, 160, 'missili', 4720, 0.5), (165, 175, 'razzi+siluri', 3932, 0.3)], carburante=[(90, 150, 'gasolio', 2500)], ponte=[(39, 86, 'bazalt', 9600), (150, 158, 'siluri_533', 2500)]),
    'udaloy': dict(depositi=[(14, 26, 'missili', 192, 0.5), (30, 50, 'proietti', 1071, 0.5), (104, 110, 'razzi', 3312, 0.5), (110, 125, 'missili+siluri', 1432, 0.5)], carburante=[(70, 115, 'gasolio', 1500)], ponte=[(50, 60, 'rastrub', 800), (88, 96, 'siluri_533', 2000)]),
    'koni': dict(depositi=[(18, 28, 'proietti+razzi', 3761, 0.5), (66, 80, 'proietti+missili', 569, 0.5)], carburante=[(40, 75, 'gasolio', 230)], ponte=[(85, 95, 'bombe', 1200)]),
    'grisha3': dict(depositi=[(12, 20, 'missili+razzi', 2328, 0.5), (58, 66, 'proietti', 348, 0.3)], carburante=[(35, 60, 'gasolio', 140)], ponte=[(36, 44, 'siluri_533', 1000), (66, 71, 'bombe', 1200)]),
    'nanuchka': dict(depositi=[(10, 17, 'missili', 120, 0.4), (52, 57, 'proietti', 154, 0.3)], carburante=[(30, 52, 'gasolio', 130)], ponte=[(20, 31, 'malakhit', 3000)]),   # 6 P-120 da 500 kg d'esplosivo (la testata ne pesa 840)
    'ugra': dict(depositi=[(18, 28, 'proietti', 308, 0.5), (56, 66, 'missili', 4800, 0.5), (66, 80, 'siluri', 15000, 0.5), (104, 114, 'proietti', 308, 0.5)], carburante=[(75, 110, 'gasolio', 1000), (30, 66, 'gasolio_carico', 1500)], ponte=[]),
    'agi': dict(depositi=[], carburante=[(25, 42, 'gasolio', 200)]),   # gli Strela (circa 6 kg): niente deposito, il fuoco resta
    'petroliera': dict(carburante=[(186, 226, 'nafta', 2800), (14, 186, 'greggio_carico', 90000)]),
    'petroliera_costiera': dict(carburante=[(86, 112, 'gasolio', 350), (19, 88, 'prodotti_carico', 8000)]),
    'mercantile': dict(carburante=[(60, 100, 'nafta', 1800)]),
    'cargo': dict(carburante=[(185, 224, 'nafta', 3500)]),
    'traghetto': dict(carburante=[(95, 125, 'gasolio', 400), (10, 140, 'veicoli', 50)]),
    'portarinfuse': dict(carburante=[(180, 215, 'nafta', 2000)]),
    'rompighiaccio': dict(carburante=[(40, 100, 'gasolio', 3000)]),
    'rimorchiatore': dict(carburante=[(15, 40, 'gasolio', 400)]),
    'nave_ricerca': dict(carburante=[(40, 65, 'gasolio', 400)]),
    'bmrt': dict(carburante=[(55, 82, 'gasolio', 800)]),
    'peschereccio': dict(carburante=[(34, 52, 'gasolio', 300)]),
    'cianciolo': dict(carburante=[(8, 24, 'gasolio', 250)]),
    'peschereccio_costiero': dict(carburante=[(15, 26, 'gasolio', 25)]),
}

# Il propellente dei depositi (decisione della principale, 31/10/2026): nello scoppio il solido vale 0,3 del suo peso in
# TNT e brucia; il liquido sulle navi brucia e basta. Tonnellate per classe dove la raccolta le da' (polvere dei cartocci
# nei depositi dei proietti, solido nei missili e nei razzi), divise fra i depositi di quel tipo in proporzione alla
# carica; per le altre classi la regola STIMA di PROPELLENTE_PER_KG.
PROPELLENTE = {
    'perry': dict(solido=11, polvere=1.5), 'kidd': dict(solido=17, polvere=8.8), 'ticonderoga': dict(solido=22, polvere=10.5),
    'iowa': dict(polvere=395), 'kara': dict(solido=63), 'kirov': dict(solido=75), 'nanuchka': dict(solido=1),
}
# kg di propellente per kg di carica netta (STIMA): la polvere del 127 (8,5 kg per 3,5 d'esplosivo); il solido dei
# missili quanto la loro carica
PROPELLENTE_PER_KG = dict(polvere=2.4, solido=1.0)
TIPO_PROPELLENTE = dict(proietti='polvere', missili='solido', razzi='solido')
# lo scoppio di una cella vale al piu' un locale (decisione del 31/10/2026): la catena si fa col fuoco
CARICA_MAX_SCOPPIO_KG = 30000.0


# I sottomarini (fase 3, kit 31/10/2026): il carico iniziale dei depositi per tipo, prima che la missione lo sostituisca
# col carico vivo (la sala siluri del giocatore, la dotazione dell'IA: BattelloDanni::carico_deposito). Per tipo:
# (kg di esplosivo, kg di propellente, protezione); i silos a propellente liquido (R-29R del Delta III, come gli R-27 del
# K-219) portano invece le tonnellate di propellente, che non scoppia: con l'acqua reagisce (K-219, 1986). Piu' celle
# dello stesso tipo si dividono il carico per lunghezza. `fuori`: i contenitori fra i due scafi (missile di
# data/motore/missili.toml, numero), la carica e' l'esplosivo_kg del missile per il numero.
# Testate (kg di esplosivo, STIMA dove non detto): Mk 48 290, Harpoon 100, Tomahawk 205 (dossier 01 §6.4), Tigerfish 134,
# siluri sovietici da 533 mm 300, da 650 mm 450, DM2 e Tp 613 250. Propellente: l'Otto II del Mk 48 (~200 kg, STIMA),
# i booster dei missili; i siluri elettrici e quelli sovietici a vapore non lo contano. I missili balistici: Trident I
# C4 ~28 t di propellente solido ciascuno, R-39 ~80 t, R-29R ~30 t liquide (STIMA dalla massa al lancio).
CRITICI_SOTTOMARINI = {
    'los_angeles': dict(siluri=(15 * 290 + 4 * 100 + 6 * 205, 15 * 200 + 4 * 70 + 6 * 140, 0.3)),   # Flight I: Mk 48, Harpoon, Tomahawk
    'sturgeon': dict(siluri=(19 * 290 + 4 * 100, 19 * 200 + 4 * 70 + 4 * 1000, 0.3)),            # Mk 48, Harpoon, 4 SUBROC (motore)
    'trafalgar': dict(siluri=(20 * 134 + 5 * 100, 5 * 70, 0.3)),                               # Tigerfish elettrici, Sub-Harpoon
    'ohio': dict(siluri=(12 * 290, 12 * 200, 0.3), missili=(0, 24 * 28000, 0.8)),              # 24 Trident I C4 nei silos
    'delta3': dict(siluri=(16 * 300, 0, 0.2), missili_liquidi=16 * 30),                         # 16 R-29R
    'typhoon': dict(siluri=(22 * 300, 0, 0.2), missili=(0, 20 * 80000, 0.8)),                  # 20 R-39
    'victor1': dict(siluri=(18 * 300, 0, 0.2)),
    'victor3': dict(siluri=(20 * 300 + 4 * 450, 0, 0.2)),
    'sierra': dict(siluri=(36 * 300 + 4 * 450, 0, 0.2)),
    'alfa': dict(siluri=(18 * 300, 0, 0.2)),
    'akula': dict(siluri=(28 * 300 + 12 * 450, 0, 0.2)),
    'oscar2': dict(siluri=(24 * 300 + 4 * 450, 0, 0.2), fuori=('p700', 24)),
    'charlie2': dict(siluri=(12 * 300, 0, 0.2), fuori=('p120', 8)),
    'type206': dict(siluri=(8 * 250, 0, 0.2)),
    'nacken': dict(siluri=(8 * 250 + 4 * 45, 0, 0.2)),
    'kilo': dict(siluri=(18 * 300, 0, 0.2)),
    'foxtrot': dict(siluri=(22 * 300, 0, 0.2)),
}
