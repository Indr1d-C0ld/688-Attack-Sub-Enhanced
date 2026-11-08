"""Il libretto di riconoscimento al periscopio (docs/25_libretto_riconoscimento.md) -> data/motore/libretto.toml.
Lo legge grafica/libretto.hpp (libretto::Voce) per la pagina accanto alla vista; le sagome si fanno dai modelli 3D
(engine/strumenti/libretto_tavole stampa l'altezza del modello accanto a quella del libretto, per il controllo).

I dati sono **quelli noti nel 1989**, non la verita' del gioco:
- `albero_m`: la testa d'albero sul galleggiamento come la sapeva l'intelligence, `albero_sigma_m` la sua incertezza a
  1 sigma (STIMA: 1 m per le navi della NATO, 2 m per quelle sovietiche, 1,5 per i sottomarini sovietici, 0,5 per
  quelli della NATO; 1-3 m per le civili, di piu' per i tipi generici). Oggi e' l'altezza del modello del kit
  arrotondata al metro: il telemetro usa questo numero, e una classe sbagliata da' una distanza sbagliata.
  Le civili generiche (portarinfuse, traghetto...) hanno la sagoma e l'albero del modello di base.
- `riferimento_nome`, `riferimento_m`, `riferimento_sigma_m`: per le navi con l'albero oltre il quadrante del Type 2 e
  del Type 18 (39,62 m), un secondo punto noto entro il quadrante (il tetto della plancia, un fumaiolo), misurato sulle
  parti del modello del kit, con la stessa incertezza dell'albero. Il telemetro lo usa quando l'albero non ci sta;
- `segni`: i segni distintivi, quelli che si cercano all'oculare. Al piu' ~95 caratteri, tre righe della pagina;
- `armi` e `sensori`: quelli che si vedono da fuori (al piu' ~64 caratteri in tutto, due righe);
- `radar`: i nomi NATO brevi, per l'ESM (al piu' ~90 caratteri); `emettitori_id` viene da emettitori.toml [suite];
- `vede`: le etichette del filtro «cosa vedo» (VEDO); i sottomarini hanno «vela» e dove sta, com'e', i timoni.
La pagina e' di 210x340: libretto_tavole avverte se il testo di una classe non ci sta.
Lunghezza dalle classi, rimando al museo da enciclopedia.toml. `fonte`, `conf`; i campi in `stime` sono STIMA.
Uso: libretto.py <radice del progetto>. Dopo classi, emettitori ed enciclopedia."""
import os
import sys
import tomllib

SEZIONI = {
    ('patto', False): 'NAVI DA GUERRA - URSS',
    ('nato', False): 'NAVI DA GUERRA - NATO',
    ('patto', True): 'SOTTOMARINI - URSS',
    ('nato', True): 'SOTTOMARINI - NATO',
    ('civili', False): 'NAVI CIVILI',
    ('patto', 'aereo'): 'AEREI ED ELICOTTERI - URSS',
    ('nato', 'aereo'): 'AEREI ED ELICOTTERI - NATO',
}


def sezione(v):
    return SEZIONI[(v['parte'], 'aereo') if v['tipo'] in ('aereo', 'elicottero') else (v['parte'], v['sottomarino'])]
SIGMA = {('nato', False): 1.0, ('patto', False): 2.0, ('nato', True): 0.5, ('patto', True): 1.5}

FONTE_GUERRA = 'dossier 01 (piattaforme e armi) e le voci del museo; i radar dalle tavole del kit, da verificare'
FONTE_CIVILI = 'dossier 26 (navi civili e traffico) e classi.toml'


def v(nome, sottotitolo, parte, tipo, dislocamento, albero, segni, armi, sensori, radar, sottomarino=False, sigma=None,
      fonte=None, conf='media', riferimento=None):
    s = SIGMA.get((parte, sottomarino), 2.0) if sigma is None else sigma
    rif_nome, rif_m = riferimento if riferimento else ('', 0.0)
    return {'riferimento_nome': rif_nome, 'riferimento_m': float(rif_m), 'riferimento_sigma_m': float(s) if riferimento else 0.0,
            'nome': nome, 'sottotitolo': sottotitolo, 'parte': parte, 'tipo': tipo, 'sottomarino': sottomarino,
            'dislocamento_t': dislocamento, 'albero_m': float(albero), 'albero_sigma_m': float(s), 'segni': segni,
            'armi': armi, 'sensori': sensori, 'radar': radar,
            'fonte': fonte or (FONTE_CIVILI if parte == 'civili' else FONTE_GUERRA), 'conf': conf,
            'stime': ['albero_m', 'albero_sigma_m'] + ([] if parte == 'civili' else ['radar'])}


# l'ordine del libretto: navi sovietiche, della NATO, sottomarini sovietici e della NATO, civili
VOCI = {
    # --- navi da guerra sovietiche ---
    'kirov': v('KIROV', 'PR. 1144, INCROCIATORE NUCLEARE', 'patto', 'incrociatore', 24300, 51,
               "IL PIU' GRANDE; CASTELLO LUNGO COI PORTELLI DEI VLS; NESSUN FUMAIOLO VERO; HANGAR SOTTO IL PONTE DI POPPA",
               ['20 SS-N-19', '12 SA-N-6', '2 SA-N-4', '2 X 100 MM', '3 KA-27'], [], ['TOP PAIR', 'TOP DOME', 'POP GROUP', 'KITE SCREECH'],
               riferimento=('TETTO DELLA PLANCIA', 25)),
    'slava': v('SLAVA', 'PR. 1164, INCROCIATORE MISSILI', 'patto', 'incrociatore', 11500, 40,
               "OTTO TUBI BINATI SS-N-12 INCLINATI AI LATI DEL CASSERO; FUMAIOLI GEMELLI; TORRE BINATA DA 130 A PRUA",
               ['16 SS-N-12', '8 SA-N-6', '2 X 130 MM', '6 AK-630', 'KA-25'], [], ['TOP PAIR', 'TOP DOME', 'FRONT DOOR', 'KITE SCREECH'],
               riferimento=('FUMAIOLI', 19)),
    'kara': v('KARA', 'PR. 1134B, INCROCIATORE ANTISOM', 'patto', 'incrociatore', 9700, 35,
              "FUMAIOLO QUADRATO ENORME A CENTRO NAVE; SA-N-3 BINATI A PRUA E A POPPA; SS-N-14 AI LATI DEL PONTE",
              ['2 SA-N-3', '8 SS-N-14', '2 SA-N-4', '4 X 76 MM', 'KA-25'], ['VDS'], ['TOP SAIL', 'HEAD NET-C', 'HEAD LIGHT X 2', 'POP GROUP']),
    'krivak2': v('KRIVAK II', 'PROGETTO 1135M, FREGATA ANTISOM', 'patto', 'fregata', 3700, 29,
                 "SS-N-14 QUADRUPLO A SCATOLA A PRUA; UN FUMAIOLO LARGO; DUE TORRI SINGOLE A POPPA; VDS SULLO SPECCHIO",
                 ['SS-N-14', '2 SA-N-4', '2 X 100 MM', '2 RBU-6000', '8 TUBI'], ['VDS'], ['HEAD NET-C', 'DON KAY', 'EYE BOWL', 'POP GROUP', 'KITE SCREECH']),
    'koni': v('KONI', 'PR. 1159, FREGATA', 'patto', 'fregata', 1900, 24,
              "PICCOLA, UN FUMAIOLO; TORRI BINATE DA 76 A PRUA E A POPPA; SA-N-4 A SCOMPARSA A POPPA",
              ['4 X 76 MM', '4 X 30 MM', 'SA-N-4', '2 RBU-6000', 'MINE'], [], ['STRUT CURVE', 'DON-2', 'POP GROUP', 'HAWK SCREECH']),
    # il primo lotto delle firme nuove (docs/ricerca/31, 29/10/2026); l'albero dal modello riscalato del kit
    'udaloy': v('UDALOY', 'PR. 1155, CACCIATORPEDINIERE ASW', 'patto', 'cacciatorpediniere', 7620, 35,
                "DUE FUMAIOLI APPAIATI; DUE HANGAR AFFIANCATI A POPPA; SS-N-14 A QUATTRO AI LATI DELLA PLANCIA",
                ['8 SS-N-14', '8 SA-N-9', '2 X 100 MM', '4 AK-630', '2 KA-27'], [], ['STRUT PAIR', 'EYE BOWL', 'KITE SCREECH']),
    'grisha3': v('GRISHA III', 'PR. 1124M, CORVETTA ASW', 'patto', 'corvetta', 1120, 20,
                 "PICCOLA E BASSA; RBU-6000 A PRUA; SA-N-4 A SCOMPARSA SUL CASTELLO; BINATO DA 57 A POPPA",
                 ['SA-N-4', '2 X 57 MM', 'AK-630', '2 RBU-6000', '4 TUBI 533'], [], ['STRUT CURVE', 'POP GROUP', 'BASS TILT']),
    'nanuchka': v('NANUCHKA', 'PR. 1234, CORVETTA MISSILI', 'patto', 'corvetta', 670, 20,
                  "SOVRASTRUTTURA ALTA E LARGA CON LA CUPOLA BAND STAND; DUE LANCIATORI TRINATI AI LATI DEL PONTE",
                  ['6 SS-N-9', 'SA-N-4', '2 X 57 MM'], [], ['PEEL PAIR', 'BAND STAND', 'POP GROUP', 'MUFF COB']),
    'agi': v('AGI, NAVE SPIA', "PESCHERECCIO DELL'INTELLIGENCE", 'patto', 'nave spia', 700, 19,
             "SCAFO DA PESCHERECCIO, MA ANTENNE E RADOMI SULLA PLANCIA; BANDIERA DA GUERRA; SEGUE I GRUPPI NAVALI",
             ['NESSUNA (A VOLTE SA-7)'], ['ANTENNE ESM E COMINT'], ['DON-2']),
    'ugra': v('UGRA', 'PROGETTO 1886, BASE GALLEGGIANTE', 'patto', 'nave appoggio', 9650, 25,
              "SCAFO LUNGO DA MERCANTILE; GRANDE GRU A CENTRO NAVE; FUMAIOLO TONDO; DUE BINATI DA 57 A PRUA E DUE A POPPA",
              ['8 X 57 MM (4 BINATI)'], [], ['STRUT CURVE', 'DON-2', 'MUFF COB'],
              fonte='extracted/navi/ugra/RAPPORTO.md'),
    # --- navi da guerra della NATO ---
    'hauk': v('HAUK', 'MOTOCANNONIERA NORVEGESE', 'nato', 'motocannoniera', 148, 12,
              "PICCOLA E BASSA; CANNONE DA 40 A PRUA; I SEI PENGUIN A POPPA; PLANCIA A CENTRO NAVE",
              ['6 PENGUIN', '2 TUBI 533 MM', '40 MM', '20 MM'], [], ['NAVIGAZIONE', 'DIREZIONE DEL TIRO'],
              fonte='extracted/navi/hauk/RAPPORTO.md'),
    'oslo': v('OSLO', 'F300, FREGATA NORVEGESE', 'nato', 'fregata', 1745, 24.5,
              "CASTELLO ALTO E SLANCIATO; BINATI DA 76 A PRUA E A POPPA; TERNE DAVANTI ALLA PLANCIA; TRALICCIO SCURO "
              "E FUMAIOLO INCLINATO A CENTRO NAVE",
              ['4 X 76 MM (2 BINATI)', 'TERNE III', 'SEA SPARROW', '6 PENGUIN', '6 TUBI 324 MM'], ['SQS-36', 'TERNE III'],
              ['DRBV 22', 'DECCA', 'MK 91'], fonte='extracted/navi/oslo/RAPPORTO.md'),
    # la Svezia neutrale sta con la NATO nel libretto: le tavole sono della marina occidentale
    'stockholm': v('STOCKHOLM', 'K11, CORVETTA SVEDESE', 'nato', 'corvetta', 372, 23,
                   "PRUA ALTA; TORRE DEL 57 A PRUA; PLANCIA E TORRE DELL'ALBERO A CENTRO NAVE; TUGA POPPIERA COL RADOME",
                   ['57 MM', '40 MM', '8 RBS-15', '4 TUBI 400 MM', 'LANCIARAZZI ELMA'], ['SIMRAD', 'VDS'],
                   ['SEA GIRAFFE', '9LV200'], fonte='extracted/navi/stockholm/RAPPORTO.md'),
    'kitty_hawk': v('KITTY HAWK', 'CV-63, PORTAEREI', 'nato', 'portaerei', 81800, 61.6,
                    "COME LA NIMITZ MA L'ISOLA PIU' A PRUA COL FUMAIOLO; TRALICCIO DEL RADAR A POPPAVIA DELL'ISOLA",
                    ['3 SEA SPARROW', '3 PHALANX', '85 AEREI'], [], ['SPS-43A', 'SPS-48', 'SPS-10'],
                    riferimento=("TETTO DELL'ISOLA", 38), fonte='extracted/navi/kitty_hawk/RAPPORTO.md'),
    'nimitz': v('NIMITZ', 'CVN-68, PORTAEREI NUCLEARE', 'nato', 'portaerei', 91000, 63,
                "PONTE DI VOLO ANGOLATO; ISOLA PICCOLA A DRITTA, A CENTRO NAVE; QUATTRO ELEVATORI SUI FIANCHI",
                ['SEA SPARROW', '3-4 PHALANX', '80-90 AEREI'], [], ['SPS-48', 'SPS-49', 'SPS-67'],
                riferimento=("TETTO DELL'ISOLA", 37)),
    'iowa': v('IOWA', 'BB-61, CORAZZATA', 'nato', 'corazzata', 57000, 55,
              "TRE TORRI TRINATE DA 406, DUE A PRUA; DUE FUMAIOLI; SOVRASTRUTTURA A TORRE; SCAFO LUNGHISSIMO",
              ['9 X 406 MM', '12 X 127 MM', '32 TOMAHAWK', '16 HARPOON', '4 PHALANX'], [], ['SPS-49', 'SPS-67'],
              riferimento=('PRIMO FUMAIOLO', 31)),
    'sacramento': v('SACRAMENTO', 'AOE-1, RIFORNITORE VELOCE', 'nato', 'rifornitore', 53600, 70,
                    "PORTALI DI RIFORNIMENTO SUL PONTE; PLANCIA A PRUA; FUMAIOLO E HANGAR A POPPA, PONTE DI VOLO",
                    ['SEA SPARROW', '2 PHALANX', '2 ELICOTTERI'], [], ['SPS-40', 'SPS-10'],
                    riferimento=('TETTO DELLA PLANCIA', 30)),
    'kidd': v('KIDD', 'DDG-993, CACCIATORPEDINIERE', 'nato', 'cacciatorpediniere', 9500, 41,
              "DUE FUMAIOLI SQUADRATI SFALSATI; LANCIATORI BINATI MK 26 A PRUA E A POPPA; DUE TORRI DA 127",
              ['2 MK 26 (SM-2)', '2 X 127 MM', 'HARPOON', '2 PHALANX'], ['SQS-53'], ['SPS-48', 'SPS-55', 'SPG-51', 'SPQ-9'],
              riferimento=('FUMAIOLO PRODIERO', 20)),
    'spruance': v('SPRUANCE', 'DD-963, CACCIATORPEDINIERE', 'nato', 'cacciatorpediniere', 8040, 43,
                  "DUE FUMAIOLI SQUADRATI SFALSATI; SCAFO ALTO E LISCIO; ASROC A SCATOLA A PRUA; HANGAR DOPPIO",
                  ['2 X 127 MM', 'ASROC', 'SEA SPARROW', 'HARPOON', '2 PHALANX'], ['SQS-53'], ['SPS-40', 'SPS-55', 'SPQ-9'],
                  riferimento=('FUMAIOLO PRODIERO', 21)),
    'perry': v('OLIVER HAZARD PERRY', 'FFG-7, FREGATA', 'nato', 'fregata', 3600, 36,
               "MK 13 A BRACCIO SINGOLO A PRUA; FUMAIOLO SQUADRATO DIETRO L'ALBERO; HANGAR DOPPIO E PONTE DI VOLO",
               ['MK 13 (SM-1, HARPOON)', '76 MM', 'PHALANX', '2 SH-60B'], ['SQS-56'], ['SPS-49', 'SPS-55', 'STIR (MK 92)']),
    # il secondo lotto delle firme nuove (docs/ricerca/32, 29/10/2026); l'albero dal modello riscalato del kit
    'ticonderoga': v('TICONDEROGA', 'CG-47, INCROCIATORE AEGIS', 'nato', 'incrociatore', 9800, 43,
                     "LO SCAFO DELLA SPRUANCE; I PANNELLI OTTAGONALI DELL'SPY-1 SULLE SOVRASTRUTTURE; DUE ALBERI",
                     ['2 X 127 MM', 'MK 26 O VLS', 'HARPOON', '2 PHALANX'], ['SQS-53'], ['SPY-1A', 'SPS-49', 'SPQ-9'],
                     riferimento=('FUMAIOLO PRODIERO', 21)),
    'knox': v('KNOX', 'FF-1052, FREGATA ANTISOM', 'nato', 'fregata', 4065, 39,
              "IL MACK CILINDRICO A CENTRO NAVE; ASROC A PRUA DELLA PLANCIA; UN SOLO PEZZO DA 127 A PRUA",
              ['127 MM', 'ASROC', 'HARPOON', 'PHALANX', 'SH-2'], ['SQS-26'], ['SPS-40', 'SPS-10', 'SPG-53']),
    'type22': v('TYPE 22 BROADSWORD', 'BATCH 1, FREGATA RN', 'nato', 'fregata', 4400, 33,
                "NIENTE CANNONE; DUE SEA WOLF A SESTUPLO A PRUA E A POPPA; EXOCET A PRUA; FUMAIOLO LARGO",
                ['4 EXOCET', '2 SEA WOLF', '2 LYNX'], ['TYPE 2016'], ['TYPE 967/968', 'TYPE 910']),
    'type22b2': v('TYPE 22 BOXER', 'BATCH 2, FREGATA RN', 'nato', 'fregata', 4800, 32,
                  "COME LA BATCH 1 MA PIU' LUNGA, CON LA PRUA SLANCIATA; NIENTE CANNONE; DUE SEA WOLF; EXOCET A PRUA",
                  ['4 EXOCET', '2 SEA WOLF', '2 LYNX'], ['TYPE 2016', 'TYPE 2031Z'], ['TYPE 967/968', 'TYPE 910']),
    # --- sottomarini sovietici: la vela e gli alberi ---
    'typhoon': v('TYPHOON', 'PROGETTO 941, SSBN', 'patto', 'sottomarino', 23200, 21,
                 "ENORME E LARGO; VELA ARRETRATA COI POZZI DEI MISSILI DAVANTI; SCAFO PIATTO SOPRA IL MARE",
                 ['20 SS-N-20', '6 TUBI'], [], ['SNOOP PAIR'], sottomarino=True),
    'oscar': v('OSCAR II', 'PROGETTO 949A, SSGN', 'patto', 'sottomarino', 13900, 13,
               "SCAFO LARGHISSIMO; VELA LUNGA E BASSA; I PORTELLI DEI 24 SS-N-19 LUNGO I FIANCHI",
               ['24 SS-N-19', 'TUBI 533 E 650'], [], ['SNOOP PAIR'], sottomarino=True),
    'akula': v('AKULA I', 'PROGETTO 971, SSN', 'patto', 'sottomarino', 8140, 13,
               "VELA LUNGA, BASSA, RACCORDATA ALLO SCAFO; IL BULBO DELLA SCHIERA IN CIMA AL TIMONE VERTICALE",
               ['TUBI 533 E 650', 'SS-N-15/16'], [], ['SNOOP PAIR'], sottomarino=True),
    'victor3': v('VICTOR III', 'PROGETTO 671RTM, SSN', 'patto', 'sottomarino', 4850, 15,
                 "VELA BASSA E ARROTONDATA; IL BULBO DELLA SCHIERA SUL TIMONE VERTICALE; SCAFO A GOCCIA",
                 ['TUBI 533 E 650', 'SS-N-15/16'], [], ['SNOOP TRAY'], sottomarino=True),
    'victor1': v('VICTOR I', 'PROGETTO 671, SSN', 'patto', 'sottomarino', 4700, 15,
                 "COME IL VICTOR III MA PIU' CORTO E SENZA IL BULBO SUL TIMONE; VELA BASSA E ARROTONDATA",
                 ['6 TUBI DA 533', 'SS-N-15'], [], ['SNOOP TRAY'], sottomarino=True),
    'delta3': v('DELTA III', 'PROGETTO 667BDR, SSBN', 'patto', 'sottomarino', 13700, 12,
                "LA GOBBA DEI SILOS DEI MISSILI DIETRO LA VELA; TIMONI SULLA VELA; MOLTO LUNGO",
                ['16 SS-N-18', '4 TUBI DA 533'], [], ['SNOOP TRAY'], sottomarino=True),
    'sierra': v('SIERRA I', 'PROGETTO 945, SSN IN TITANIO', 'patto', 'sottomarino', 8300, 12,
                "VELA LUNGA, BASSA E SQUADRATA; SCAFO LARGO A GOCCIA; IL BULBO DELLA SCHIERA SUL TIMONE",
                ['TUBI 533 E 650', 'SS-N-15/16'], [], ['SNOOP PAIR'], sottomarino=True),
    'alfa': v('ALFA', 'PROGETTO 705, SSN', 'patto', 'sottomarino', 2300, 14,
              "PICCOLO; VELA BASSA E AFFUSOLATA CHE SI RACCORDA ALLO SCAFO, COME UNA LIMOUSINE",
              ['6 TUBI DA 533', 'SS-N-15'], [], ['SNOOP TRAY'], sottomarino=True),
    'charlie2': v('CHARLIE II', 'PROGETTO 670M, SSGN', 'patto', 'sottomarino', 4300, 12,
                  "PRUA TOZZA E ALTA COI PORTELLI DEGLI SS-N-9; VELA ARROTONDATA A CENTRO",
                  ['8 SS-N-9', '6 TUBI'], [], ['SNOOP TRAY'], sottomarino=True),
    'kilo': v('KILO', 'PROGETTO 877, SSK DIESEL', 'patto', 'sottomarino', 2325, 9,
              "CORTO E LARGO; VELA CON LA SOMMITA' PIATTA; SCAFO A GOCCIA", ['6 TUBI DA 533', 'MINE'], [], ['SNOOP TRAY'],
              sottomarino=True),
    'foxtrot': v('FOXTROT', 'PROGETTO 641, SSK DIESEL', 'patto', 'sottomarino', 1950, 16,
                 "VELA ALTA CON GLI ALBERI; SCAFO LUNGO E STRETTO, PONTE PIATTO; PRUA DA SOTTOMARINO CLASSICO",
                 ['10 TUBI DA 533', 'MINE'], [], ['SNOOP TRAY'], sottomarino=True),
    # --- sottomarini della NATO ---
    'los_angeles': v('LOS ANGELES', 'SSN-688, SSN', 'nato', 'sottomarino', 6000, 14,
                     "VELA BASSA E LUNGA A PRUA COI TIMONI ORIZZONTALI SULLA VELA; SCAFO CILINDRICO",
                     ['4 TUBI DA 533', 'TOMAHAWK', 'HARPOON'], ['BQQ-5', 'TB-16'], ['BPS-15'], sottomarino=True),
    'ohio': v('OHIO', 'SSBN-726, SSBN', 'nato', 'sottomarino', 18750, 16,
              "MOLTO LUNGO; IL PONTE PIATTO DEI 24 SILOS DIETRO LA VELA; VELA A PRUA COI TIMONI ORIZZONTALI",
              ['24 TRIDENT', '4 TUBI DA 533'], ['BQQ-6'], ['BPS-15'], sottomarino=True),
    'type206': v('TYPE 206', 'U 13-30, SSK TEDESCO', 'nato', 'sottomarino', 500, 8,
                 "PICCOLISSIMO; LA PRUA TONDA E PIATTA; VELA LUNGA A DUE PIANI SU UNO SCAFO BASSO; DEL BALTICO",
                 ['8 TUBI DA 533', 'MINE'], [], ['CALYPSO'], sottomarino=True),
    'nacken': v('NACKEN', 'A14, SSK SVEDESE', 'nato', 'sottomarino', 1085, 8.6,
                "CORTO E LARGO; PRUA TONDA; VELA ALTA E DIRITTA A META' NAVE COI TIMONI; CASSERO LUNGO E PIATTO; DEL "
                "BALTICO", ['6 TUBI DA 533', '2 TUBI DA 400', 'MINE'], [], ['TERMA'], sottomarino=True,
                fonte='extracted/navi/nacken/RAPPORTO.md'),
    'sturgeon': v('STURGEON', 'SSN-637, SSN', 'nato', 'sottomarino', 4250, 16,
                  "VELA ALTA COI TIMONI ORIZZONTALI ALTI SULLA VELA; SCAFO CORTO", ['4 TUBI DA 533', 'HARPOON'],
                  ['BQQ-5'], ['BPS-15'], sottomarino=True),
    'trafalgar': v('TRAFALGAR', 'SSN BRITANNICO', 'nato', 'sottomarino', 4700, 7,
                   "VELA PICCOLA; TIMONI DI PRUA SULLO SCAFO; NIENTE ELICA A VISTA (IDROGETTO)",
                   ['5 TUBI DA 533', 'HARPOON'], ['TYPE 2020'], ['TYPE 1006'], sottomarino=True),
    # --- aerei ed elicotteri (31/10/2026): li si vede al periscopio, di fianco o dal basso; niente albero ne' telemetro,
    # l'«albero» e' l'altezza dell'aereo (STIMA); i radar da emettitori.toml ---
    'p3c': v('P-3C ORION', 'PATTUGLIATORE ASW, US NAVY', 'nato', 'aereo', 0, 10,
             "QUATTRO TURBOELICHE SULL'ALA BASSA; LA CODA DEL MAD DIETRO LA DERIVA; RADOME SOTTO IL MUSO",
             ['MK 46', 'BOE SONORE', 'HARPOON'], ['MAD', 'BOE'], ['APS-115'], sigma=1.0, conf='alta'),
    's3a': v('S-3A VIKING', 'ANTISOM IMBARCATO, US NAVY', 'nato', 'aereo', 0, 7,
             "DUE REATTORI SOTTO L'ALA ALTA; COMPATTO; LA CODA DEL MAD ESCE DA POPPA",
             ['MK 46', 'BOE SONORE'], ['MAD', 'BOE'], ['APS-116'], sigma=1.0),
    's3b': v('S-3B VIKING', 'ANTISOM IMBARCATO, US NAVY', 'nato', 'aereo', 0, 7,
             "COME L'S-3A; IL RADAR NUOVO SI SENTE ALL'ESM",
             ['MK 46', 'BOE SONORE', 'HARPOON'], ['MAD', 'BOE'], ['APS-137'], sigma=1.0),
    'sh60b': v('SH-60B SEAHAWK', 'LAMPS III, US NAVY', 'nato', 'elicottero', 0, 5,
               "ROTORE A QUATTRO PALE; LA CODA LUNGA; RADOME SOTTO LA CABINA; LANCIABOE A SINISTRA",
               ['MK 46'], ['MAD', 'BOE'], ['APS-124'], sigma=0.5),
    'sh60f': v('SH-60F OCEAN HAWK', 'ANTISOM, US NAVY', 'nato', 'elicottero', 0, 5,
               "COME L'SH-60B, SENZA RADOME; IL SONAR A IMMERSIONE SOTTO LA CABINA",
               ['MK 46'], ['SONAR AQS-13F'], [], sigma=0.5),
    'sh3h': v('SH-3H SEA KING', 'ANTISOM, US NAVY', 'nato', 'elicottero', 0, 5,
              "GRANDE; SCAFO ANFIBIO CON LE GONDOLE DEL CARRELLO; ROTORE A CINQUE PALE",
              ['MK 46'], ['SONAR AQS-13'], [], sigma=0.5),
    'sh2f': v('SH-2F SEASPRITE', 'LAMPS I, US NAVY', 'nato', 'elicottero', 0, 5,
              "PICCOLO; ROTORE A QUATTRO PALE; IL MUSO CORTO COL RADAR SOTTO",
              ['MK 46'], ['MAD', 'BOE'], [], sigma=0.5),
    'lynx': v('LYNX HAS.3', 'ANTISOM, ROYAL NAVY', 'nato', 'elicottero', 0, 3,
              "PICCOLO E BASSO; MUSO TONDO DEL SEASPRAY; TRAVE DI CODA SOTTILE; CARRELLO FISSO A TRICICLO",
              ['STINGRAY', 'MK 46', 'SEA SKUA'], ['MAD'], ['SEASPRAY'], sigma=0.5),
    'hkp4': v('HKP 4', 'VERTOL 107, MARINA SVEDESE', 'nato', 'elicottero', 0, 5,
              "DUE ROTORI IN TANDEM; FUSOLIERA A SCAFO; GONDOLE A POPPA",
              ['SIL. 42', 'BOMBE'], ['SONAR'], [], sigma=0.5),
    'il38': v('IL-38 MAY', 'PATTUGLIATORE ASW, AV-MF', 'patto', 'aereo', 0, 10,
              "QUATTRO TURBOELICHE SULL'ALA BASSA, PIU' AVANTI CHE SUL P-3; CODA DEL MAD; FUSOLIERA LUNGA",
              ['AT-1', 'BOE', 'BOMBE'], ['MAD', 'BOE'], ['WET EYE'], sigma=1.0),
    'tu142': v('TU-142M BEAR-F', 'PATTUGLIATORE ASW, AV-MF', 'patto', 'aereo', 0, 12,
               "ENORME; ALI A FRECCIA; QUATTRO TURBOELICHE CON LE ELICHE CONTROROTANTI; CODA DEL MAD",
               ['AT-1', 'BOE', 'BOMBE'], ['MAD', 'BOE'], ['KORSHUN'], sigma=1.0),
    'be12': v('BE-12 MAIL', 'IDROVOLANTE ASW, AV-MF', 'patto', 'aereo', 0, 7,
              "ANFIBIO; ALA A GABBIANO CON DUE TURBOELICHE IN ALTO; DERIVE DOPPIE; CODA DEL MAD",
              ['AT-1', 'BOE', 'BOMBE'], ['MAD', 'BOE'], ['INITSIATIVA-2B'], sigma=1.0),
    'ka25': v('KA-25PL HORMONE-A', 'ANTISOM, AV-MF', 'patto', 'elicottero', 0, 5,
              "DUE ROTORI COASSIALI; TRE DERIVE; RADOME SOTTO IL MUSO; CORTO E ALTO",
              ['AT-1', 'BOMBE'], ['SONAR', 'BOE'], ['INITSIATIVA-2K'], sigma=0.5),
    'ka27': v('KA-27PL HELIX-A', 'ANTISOM, AV-MF', 'patto', 'elicottero', 0, 6,
              "DUE ROTORI COASSIALI; DUE DERIVE; PIU' GRANDE DEL KA-25; RADOME SOTTO IL MUSO",
              ['AT-1', 'BOMBE'], ['SONAR', 'BOE'], ['INITSIATIVA-2KM'], sigma=0.5),
    # --- navi civili ---
    'mercantile': v('LENINSKIY KOMSOMOL', 'TIPO CARGO SECCO, 16.000 TPL', 'civili', 'mercantile', 0, 39,
                    "SOVRASTRUTTURA E FUMAIOLO A CENTRO NAVE; ALBERI E BIGHI A PRUA E A POPPA; SEI BOCCAPORTE",
                    [], [], ['NAV. CIVILE (X E S)'], sigma=1.0),
    'cargo': v('PORTACONTAINER', 'TIPO ESTELLE MAERSK, 1980', 'civili', 'mercantile', 0, 46,
               "SOVRASTRUTTURA E FUMAIOLO A POPPA; FILE DI CONTAINER IN COPERTA; PRUA ALTA; NIENTE GRU",
               [], [], ['NAV. CIVILE (X E S)'], sigma=1.0, riferimento=('FUMAIOLO', 38)),
    'petroliera': v('PETROLIERA', 'TIPO SEA ISLE CITY, 1981', 'civili', 'petroliera', 0, 38,
                    "SOVRASTRUTTURA E FUMAIOLO A POPPA; PONTE LUNGO E PIATTO CON LE TUBOLATURE; MANICHETTE A CENTRO",
                    [], [], ['NAV. CIVILE (X E S)'], sigma=1.0),
    'portarinfuse': v('PORTARINFUSE', 'NAVE DA CARICO ALLA RINFUSA', 'civili', 'mercantile', 0, 39,
                      "SOVRASTRUTTURA A POPPA (QUI COL PROFILO DEL CARGO); GRANDI BOCCAPORTE; GRU FRA I BOCCAPORTE",
                      [], [], ['NAV. CIVILE (X E S)'], sigma=3.0),
    'petroliera_costiera': v('PETROLIERA COSTIERA', 'E CHIMICHIERA', 'civili', 'petroliera', 0, 31,
                             "COME UNA PETROLIERA, PIU' PICCOLA; SOVRASTRUTTURA A POPPA; TUBOLATURE IN COPERTA",
                             [], [], ['NAV. CIVILE (X E S)'], sigma=3.0),
    'traghetto': v('TRAGHETTO', 'RO-RO E PASSEGGERI', 'civili', 'traghetto', 0, 39,
                   "SOVRASTRUTTURA ALTA E LUNGA, MOLTI PONTI; PORTELLONE A POPPA (QUI COL PROFILO DEL CARGO)",
                   [], [], ['NAV. CIVILE (X E S)'], sigma=3.0),
    'rompighiaccio': v('ROMPIGHIACCIO', 'DIESEL-ELETTRICO', 'civili', 'rompighiaccio', 0, 39,
                       "PRUA ROMPIGHIACCIO; SCAFO LARGO E ROSSO; PONTE DI VOLO A POPPA (QUI COL PROFILO DEL CARGO)",
                       [], [], ['NAV. CIVILE (X E S)'], sigma=3.0),
    'peschereccio': v('PESCHERECCIO A STRASCICO', "D'ALTURA", 'civili', 'peschereccio', 0, 22,
                      "SOVRASTRUTTURA A PRUA; RAMPA E PORTALE A POPPA; ALBERI DI CARICO", [], [], ['NAV. CIVILE (X)'],
                      sigma=2.0),
    'bmrt': v('BMRT', 'GRANDE PESCHERECCIO SOVIETICO', 'civili', 'peschereccio', 0, 29,
              "DUE FUMAIOLI AFFIANCATI; PORTALI A PRUA E A POPPA; SPESSO VICINO ALLE ESERCITAZIONI",
              [], [], ['DON-2'], sigma=3.0),
    'nave_ricerca': v('NAVE OCEANOGRAFICA', 'DA RICERCA', 'civili', 'ricerca', 0, 38,
                      "SOVRASTRUTTURA LUNGA A PRUA; RADOME SUL FUMAIOLO; PORTALE AD A A POPPA",
                      [], [], ['NAV. CIVILE (X)'], sigma=3.0),
    'rimorchiatore': v("RIMORCHIATORE D'ALTURA", '', 'civili', 'rimorchiatore', 0, 22,
                       "PLANCIA ALTA A PRUA; PONTE BASSO E LIBERO A POPPA COL GANCIO (QUI COL PROFILO DEL PESCHERECCIO)",
                       [], [], ['NAV. CIVILE (X)'], sigma=3.0),
    'peschereccio_costiero': v('PESCHERECCIO COSTIERO', 'PALANGARO, RETI DA POSTA', 'civili', 'peschereccio', 0, 11,
                               "PICCOLO; TIMONERIA A PRUA; ALBERO CORTO; GRU A MEZZANAVE, PORTALE A POPPA",
                               [], [], ['NAV. CIVILE (X)'], sigma=3.0),
    'cianciolo': v('CIANCIOLO', 'PESCHERECCIO A CIRCUIZIONE', 'civili', 'peschereccio', 0, 18,
                   "SCAFO NERO, SOVRASTRUTTURA BIANCA; TORRE DI VEDETTA; BOMA LUNGO; RAMPA A POPPA",
                   [], [], ['NAV. CIVILE (X)'], sigma=3.0),
}


# «cosa vedo» (docs/25 §3.1): le etichette del filtro, dal vocabolario VEDO. Restringono le tavole a quelle che hanno
# tutto cio' che si e' scelto
VEDO = ('ponte_volo', 'hangar', 'lanciatori', 'torri', 'un_fumaiolo', 'piu_fumaioli', 'nessun_fumaiolo', 'radomi',
        'gru_bighi', 'container', 'plancia_prua', 'plancia_centro', 'plancia_poppa', 'portali', 'vela',
        # i sottomarini: dove sta la vela sullo scafo, se e' alta o bassa, se si raccorda allo scafo, i timoni
        # orizzontali sulla vela, il bulbo della schiera rimorchiata in cima al timone verticale. Dai segni della tavola
        # e dalla posizione della vela sui modelli del kit
        'vela_prua', 'vela_centro', 'vela_arretrata', 'vela_alta', 'vela_bassa', 'vela_raccordata', 'timoni_vela',
        'bulbo_timone',
        # la forma della vela vista di fianco, misurata sui modelli del kit (31/10/2026): corta o lunga (lunghezza alla
        # base sull'altezza, sotto 2,2 o da 2,3: Trafalgar 1,7, Foxtrot 1,9, Kilo 2,5, Charlie II 4,2), squadrata o
        # spiovente (quanto arretra il bordo d'uscita per metro di quota, fino a 0,3 o da 1,2: Sierra 0,24, Victor III
        # 1,32). Ogni sottomarino ne ha una per coppia, e nessun sottomarino ha le stesse etichette di un altro
        'vela_corta', 'vela_lunga', 'vela_squadrata', 'vela_spiovente',
        # gli aerei e gli elicotteri (31/10/2026): i motori, le ali, lo scafo, la coda del MAD, i rotori
        'turboeliche', 'reattori', 'quattro_motori', 'due_motori', 'ali_a_freccia', 'anfibio', 'coda_mad',
        'rotore_singolo', 'rotori_coassiali', 'rotori_tandem')
VEDE = {
    'p3c': ['turboeliche', 'quattro_motori', 'coda_mad'], 'il38': ['turboeliche', 'quattro_motori', 'coda_mad'],
    'tu142': ['turboeliche', 'quattro_motori', 'ali_a_freccia', 'coda_mad'],
    'be12': ['turboeliche', 'due_motori', 'anfibio', 'coda_mad'],
    's3a': ['reattori', 'due_motori', 'coda_mad'], 's3b': ['reattori', 'due_motori', 'coda_mad'],
    'sh60b': ['rotore_singolo'], 'sh60f': ['rotore_singolo'], 'sh3h': ['rotore_singolo', 'anfibio'],
    'sh2f': ['rotore_singolo'], 'lynx': ['rotore_singolo'], 'hkp4': ['rotori_tandem', 'anfibio'],
    'ka25': ['rotori_coassiali'], 'ka27': ['rotori_coassiali'],
    'kirov': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'nessun_fumaiolo', 'radomi', 'plancia_centro'],
    'slava': ['lanciatori', 'torri', 'piu_fumaioli', 'radomi', 'plancia_centro'],
    'kara': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'un_fumaiolo', 'radomi', 'plancia_centro'],
    'krivak2': ['lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'koni': ['lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'nanuchka': ['lanciatori', 'torri', 'nessun_fumaiolo', 'radomi', 'plancia_centro'],
    'agi': ['radomi', 'un_fumaiolo', 'plancia_prua'],
    'ugra': ['gru_bighi', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'hauk': ['lanciatori', 'torri', 'nessun_fumaiolo', 'plancia_centro'],
    'oslo': ['lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'stockholm': ['lanciatori', 'torri', 'nessun_fumaiolo', 'radomi', 'plancia_centro'],
    'nimitz': ['ponte_volo', 'nessun_fumaiolo', 'radomi'],
    'kitty_hawk': ['ponte_volo', 'un_fumaiolo', 'radomi'],
    'iowa': ['torri', 'lanciatori', 'piu_fumaioli', 'plancia_centro'],
    'sacramento': ['ponte_volo', 'hangar', 'portali', 'un_fumaiolo', 'plancia_prua'],
    'kidd': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'piu_fumaioli', 'plancia_centro'],
    'spruance': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'piu_fumaioli', 'plancia_centro'],
    'perry': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'ticonderoga': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'piu_fumaioli', 'radomi', 'plancia_centro'],
    'knox': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'type22': ['ponte_volo', 'hangar', 'lanciatori', 'un_fumaiolo', 'plancia_centro'],
    'type22b2': ['ponte_volo', 'hangar', 'lanciatori', 'un_fumaiolo', 'plancia_centro'],
    'mercantile': ['gru_bighi', 'un_fumaiolo', 'plancia_centro'],
    'cargo': ['container', 'un_fumaiolo', 'plancia_poppa'],
    'petroliera': ['un_fumaiolo', 'plancia_poppa'],
    'portarinfuse': ['gru_bighi', 'un_fumaiolo', 'plancia_centro'],
    'petroliera_costiera': ['un_fumaiolo', 'plancia_poppa'],
    'traghetto': ['un_fumaiolo', 'plancia_centro'],
    'rompighiaccio': ['un_fumaiolo', 'plancia_centro'],
    'peschereccio': ['portali', 'un_fumaiolo', 'plancia_prua'],
    'bmrt': ['gru_bighi', 'portali', 'piu_fumaioli', 'plancia_centro'],
    'nave_ricerca': ['portali', 'un_fumaiolo', 'radomi', 'plancia_prua'],
    'rimorchiatore': ['un_fumaiolo', 'plancia_prua'],
    'peschereccio_costiero': ['gru_bighi', 'portali', 'nessun_fumaiolo', 'plancia_prua'],
    'cianciolo': ['gru_bighi', 'portali', 'nessun_fumaiolo', 'plancia_prua'],
    'typhoon': ['vela', 'vela_arretrata', 'vela_lunga', 'vela_squadrata'],
    'oscar': ['vela', 'vela_centro', 'vela_bassa', 'vela_lunga', 'vela_squadrata'],
    'akula': ['vela', 'vela_centro', 'vela_bassa', 'vela_raccordata', 'bulbo_timone', 'vela_lunga', 'vela_spiovente'],
    'victor3': ['vela', 'vela_centro', 'vela_bassa', 'bulbo_timone', 'vela_lunga', 'vela_spiovente'],
    'victor1': ['vela', 'vela_centro', 'vela_bassa', 'vela_lunga', 'vela_spiovente'],
    'delta3': ['vela', 'vela_prua', 'vela_alta', 'timoni_vela', 'vela_lunga', 'vela_squadrata'],
    'sierra': ['vela', 'vela_centro', 'vela_bassa', 'bulbo_timone', 'vela_lunga', 'vela_squadrata'],
    'udaloy': ['ponte_volo', 'hangar', 'lanciatori', 'torri', 'piu_fumaioli', 'plancia_centro'],
    'grisha3': ['lanciatori', 'torri', 'un_fumaiolo', 'plancia_centro'],
    'alfa': ['vela', 'vela_centro', 'vela_bassa', 'vela_raccordata', 'vela_lunga', 'vela_spiovente'],
    'charlie2': ['vela', 'vela_centro', 'vela_lunga', 'vela_spiovente'],
    'kilo': ['vela', 'vela_centro', 'vela_lunga', 'vela_squadrata'],
    'foxtrot': ['vela', 'vela_centro', 'vela_alta', 'vela_corta', 'vela_squadrata'],
    'los_angeles': ['vela', 'vela_prua', 'vela_bassa', 'timoni_vela', 'vela_corta', 'vela_squadrata'],
    'ohio': ['vela', 'vela_prua', 'timoni_vela', 'vela_corta', 'vela_squadrata'],
    'type206': ['vela', 'vela_centro', 'vela_alta', 'vela_lunga', 'vela_squadrata'],
    'nacken': ['vela', 'vela_centro', 'vela_alta', 'timoni_vela', 'vela_corta', 'vela_squadrata'],
    'sturgeon': ['vela', 'vela_prua', 'vela_alta', 'timoni_vela', 'vela_corta', 'vela_squadrata'],
    'trafalgar': ['vela', 'vela_centro', 'vela_corta', 'vela_squadrata'],
}


def toml_str(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'


def toml_lista(v):
    return '[' + ', '.join(toml_str(x) for x in v) + ']'


def esporta_libretto(radice):
    """Scrive data/motore/libretto.toml; restituisce il numero delle tavole. Da esporta_motore, dopo l'enciclopedia."""
    motore = os.path.join(radice, 'data', 'motore')
    with open(os.path.join(motore, 'classi.toml'), 'rb') as f:
        classi = tomllib.load(f)
    with open(os.path.join(motore, 'emettitori.toml'), 'rb') as f:
        suite = tomllib.load(f).get('suite', {})
    with open(os.path.join(motore, 'enciclopedia.toml'), 'rb') as f:
        indice = tomllib.load(f).get('indice', {})
    righe = ['# Il libretto di riconoscimento al periscopio: generato da tools/pipeline/libretto.py, non modificare a mano.',
             '# I dati sono quelli noti nel 1989 (docs/25_libretto_riconoscimento.md), non la verita\' del gioco.', '']
    for classe, x in VOCI.items():   # una riga della pagina: 32 caratteri del font piccolo
        if len(x['sottotitolo']) > 32:
            raise SystemExit(f"libretto: il sottotitolo di {classe} e' piu' lungo di 32 caratteri")
    QUADRANTE_MAX_M = 39.62   # il quadrante del Type 2 e del Type 18 (periscopi.toml): 130 piedi
    for classe, x in VOCI.items():   # oltre il quadrante serve il riferimento, e il riferimento ci deve stare
        if x['albero_m'] > QUADRANTE_MAX_M and not x['riferimento_nome']:
            raise SystemExit(f"libretto: {classe} ha l'albero oltre il quadrante e nessun riferimento")
        if x['riferimento_nome'] and not (4.57 <= x['riferimento_m'] <= QUADRANTE_MAX_M):
            raise SystemExit(f"libretto: il riferimento di {classe} e' fuori dal quadrante")
    for classe, x in VEDE.items():
        if classe not in VOCI or any(e not in VEDO for e in x):
            raise SystemExit(f'libretto: etichette «cosa vedo» sbagliate per {classe}')
    for classe, x in VOCI.items():   # ogni sottomarino dice dove sta la vela e com'e', una volta sola per gruppo
        if not x['sottomarino']:
            continue
        for gruppo in (('vela_prua', 'vela_centro', 'vela_arretrata'), ('vela_corta', 'vela_lunga'),
                       ('vela_squadrata', 'vela_spiovente')):
            if sum(e in VEDE.get(classe, []) for e in gruppo) != 1:
                raise SystemExit(f'libretto: {classe} deve avere una e una sola fra {", ".join(gruppo)}')
    visti = {}   # e nessun sottomarino ha le stesse etichette di un altro: il filtro deve poterli separare
    for classe, x in VOCI.items():
        if x['sottomarino']:
            chiave = frozenset(VEDE.get(classe, []))
            if chiave in visti:
                raise SystemExit(f'libretto: {classe} e {visti[chiave]} hanno le stesse etichette «cosa vedo»')
            visti[chiave] = classe
    for ordine, (classe, v) in enumerate(VOCI.items(), 1):
        if classe not in classi:
            raise SystemExit(f'libretto: la classe {classe} non e\' in classi.toml')
        c = classi[classe]
        museo = next((k for k in (f'nave.{classe}', f'sottomarino.{classe}', f'civile.{classe}', f'aereo.{classe}',
                                  f'elicottero.{classe}') if k in indice), '')
        righe += [f'[{classe}]',
                  f'ordine = {ordine}',
                  f'nome = {toml_str(v["nome"])}',
                  f'sottotitolo = {toml_str(v["sottotitolo"])}',
                  f'sezione = {toml_str(sezione(v))}',
                  f'parte = {toml_str(v["parte"])}',
                  f'tipo = {toml_str(v["tipo"])}',
                  f'sottomarino = {"true" if v["sottomarino"] else "false"}',
                  f'dislocamento_t = {float(v["dislocamento_t"])}',
                  f'lunghezza_m = {float(c.get("lunghezza_m", 0.0))}',
                  f'albero_m = {v["albero_m"]}',
                  f'albero_sigma_m = {v["albero_sigma_m"]}',
                  f'riferimento_nome = {toml_str(v["riferimento_nome"])}',
                  f'riferimento_m = {v["riferimento_m"]}',
                  f'riferimento_sigma_m = {v["riferimento_sigma_m"]}',
                  f'segni = {toml_str(v["segni"])}',
                  f'armi = {toml_lista(v["armi"])}',
                  f'sensori = {toml_lista(v["sensori"])}',
                  f'emettitori = {toml_lista(v["radar"])}',
                  f'emettitori_id = {toml_lista(suite.get(classe, []))}',
                  f'vede = {toml_lista(VEDE.get(classe, ["vela"] if v["sottomarino"] else []))}',
                  f'voce_museo = {toml_str(museo)}',
                  f'fonte = {toml_str(v["fonte"])}',
                  f'conf = {toml_str(v["conf"])}',
                  f'stime = {toml_lista(v["stime"])}',
                  '']
    with open(os.path.join(motore, 'libretto.toml'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(righe))
    return len(VOCI)


if __name__ == '__main__':
    print('libretto.toml:', esporta_libretto(sys.argv[1] if len(sys.argv) > 1 else '.'), 'classi')
