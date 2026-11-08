"""Le tavole «prima e dopo» delle bozze della rifinitura estetica (roadmap 33): l'istantanea di oggi
(docs/ricerca/schermate_prima) a sinistra, la bozza a destra, alla risoluzione del gioco (640x480 ciascuna), con le
due etichette sopra. Le coppie sono per gruppo; le tavole vanno in docs/ricerca/25_bozze/<gruppo>/prima_dopo_*.png.
Uso: prima_dopo.py [radice del progetto]"""
import pathlib
import sys

from PIL import Image, ImageDraw

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
PRIMA = RADICE / 'docs' / 'ricerca' / 'schermate_prima'
BOZZE = RADICE / 'docs' / 'ricerca' / '25_bozze'
TAVOLE = RADICE / 'docs' / 'ricerca' / '25_tavole'

# gruppo -> (nome della tavola, istantanea di oggi, bozza, didascalia)
GRUPPI = {
    'sonar': [
        ('moderno', PRIMA / '688_sonar.png', BOZZE / 'sonar' / 'sonar_moderno.png',
         'LA STAZIONE MODERNA: 32 LIVELLI E LA DISPOSIZIONE DI OGGI / 128 LIVELLI COL BAYER, BTR, A-SCAN, LOFAR, DEMON'),
        ('1989', PRIMA / '688_sonar_1989.png', BOZZE / 'sonar' / 'sonar_1989.png',
         'IL MODO 1989: UNA PALETTE VERDE / IL FOSFORO CON PERSISTENZA, BAGLIORE, GRANA E RIGHE'),
    ],
    'lamiera': [
        ('scelta', PRIMA / 'menu_originali.png', TAVOLE / 'scelta_A.png',
         'LA SCELTA DELLA MISSIONE: OGGI / CON I PEZZI DI A E IL PIEDE COMUNE'),
        ('regolazioni', PRIMA / 'regolazioni.png', TAVOLE / 'regolazioni_A.png',
         'LE REGOLAZIONI DELLA SANDBOX: OGGI / CON I PEZZI DI A E IL PIEDE COMUNE'),
    ],
    'strumenti': [
        ('statistiche_riepilogo', PRIMA / 'sandbox_statistiche_1.png', BOZZE / 'strumenti' / 'statistiche_riepilogo_C.png',
         'LE STATISTICHE, PAGINA 1: TESTO NUDO SU NERO / DUE SCHERMI NELLA CONSOLLE, I NUMERI ANCHE IN FIGURA'),
        ('statistiche_scoperte', PRIMA / 'sandbox_statistiche_2.png', BOZZE / 'strumenti' / 'statistiche_scoperte_C.png',
         'LE STATISTICHE, PAGINA 2: LA TABELLA DELLE SCOPERTE IN UNO SCHERMO, LA RIGA SCELTA ACCESA'),
        ('replay', PRIMA / 'replay_quadro.png', BOZZE / 'strumenti' / 'replay_C.png',
         'IL REPLAY: LA CARTA GRANDE, LEGENDA E LETTURA IN DUE RIQUADRI, IL TEMPO IN UNA FASCIA SOTTILE, IL PIEDE COMUNE'),
        ('serie', PRIMA / 'serie_2.png', BOZZE / 'strumenti' / 'serie_C.png',
         'LA SERIE, PAGINA 2: UNO SCHERMO, RIGHE ALTERNE, BARRE NATO E PATTO SULLA STESSA SCALA, LE MEDIE BIANCHE'),
        ('statistiche_cumulate', PRIMA / 'statistiche_cumulate.png', BOZZE / 'strumenti' / 'statistiche_cumulate_C.png',
         'LE STATISTICHE GLOBALI SENZA MISSIONI: TRATTINI AL POSTO DEGLI ZERI, LA CARTA CHE DICE COME SI RIEMPIE'),
        ('editor', PRIMA / 'editor_1.png', BOZZE / 'strumenti' / 'editor_C.png',
         'L\'EDITOR, SCHEDA 1: LA CARTA NELLO SCHERMO, LE SCHEDE A LINGUETTE, I CAMPI PER GRUPPI, LA VERIFICA IN VISTA'),
        ('editor_unita', PRIMA / 'editor_2.png', BOZZE / 'strumenti' / 'editor_unita_C.png',
         'L\'EDITOR, SCHEDA 2: LA PROSSIMA UNITA\', L\'ELENCO SULLA CARTA CON LA SCELTA, I CAMPI DELL\'UNITA\' SCELTA'),
        ('editor_briefing', PRIMA / 'editor_7.png', BOZZE / 'strumenti' / 'editor_briefing_C.png',
         'L\'EDITOR, SCHEDA 7: LE RIGHE NUMERATE, LA SCELTA COL CURSORE, I COMANDI COME TASTI'),
    ],
    'stazioni': [
        ('radio', PRIMA / '688_radio.png', BOZZE / 'stazioni' / 'radio_A.png',
         'LA SALA RADIO: STATO IN SPIE E VETRI, IL MESSAGGIO SULLA TELESCRIVENTE, I RICEVUTI IN ELENCO; IL PANNELLO RESTA'),
        ('manovra', PRIMA / '688_manovra.png', BOZZE / 'stazioni' / 'manovra_C.png',
         'LA MANOVRA COL PERISCOPIO GIU\': AL POSTO DELLA VISTA SOTT\'ACQUA, IL PROFILO DI QUOTA E ORDINATO CONTRO ATTUALE'),
        ('armi', PRIMA / '688_armi.png', BOZZE / 'stazioni' / 'armi_C.png',
         'LE ARMI COL PERISCOPIO GIU\': AL POSTO DELLA VISTA SOTT\'ACQUA, LE ARMI IN ACQUA, I TUBI E IL CARICO'),
        ('carta', PRIMA / '688_carta.png', BOZZE / 'stazioni' / 'carta_A.png',
         'LA CARTA: IL PANNELLO DI SOTTO CON LO STATO IN SPIE E VETRI E I COMANDI COME TASTI DELLA TASTIERA'),
        ('radio_alfa', PRIMA / 'alfa_radio.png', BOZZE / 'stazioni' / 'radio_alfa_A.png',
         'ALFA, LA SALA RADIO: LA STESSA DISPOSIZIONE COL TEMA BEIGE E LE SCRITTE A STAMPINO'),
        ('manovra_alfa', PRIMA / 'alfa_manovra.png', BOZZE / 'stazioni' / 'manovra_alfa_C.png',
         'ALFA, LA MANOVRA COL PERISCOPIO GIU\': IL RIPETITORE COL FOSFORO DELL\'ALFA'),
        ('armi_alfa', PRIMA / 'alfa_armi.png', BOZZE / 'stazioni' / 'armi_alfa_C.png',
         'ALFA, LE ARMI COL PERISCOPIO GIU\': SEI TUBI, LE ARMI SOVIETICHE IN ACQUA E IL CARICO'),
        ('carta_alfa', PRIMA / 'alfa_carta.png', BOZZE / 'stazioni' / 'carta_alfa_A.png',
         'ALFA, IL SOZH: IL PANNELLO DI SOTTO IN SPIE, VETRI E TASTI SUL TEMA BEIGE'),
    ],
    'cerimonia': [
        ('fascicolo_piede', BOZZE / 'cerimonia' / 'fascicolo_anteprima.png', BOZZE / 'cerimonia' / 'fascicolo_piede_B.png',
         'IL FASCICOLO: IL PIEDE DI LEGNO E OTTONE DELL\'ALBO AL POSTO DELLA RIGA D\'AIUTO; LA CARTA FINISCE A 424'),
        ('bacheca_piede', PRIMA / 'bacheca.png', BOZZE / 'cerimonia' / 'bacheca_piede_B.png',
         'LA BACHECA: IL PIEDE DELL\'ALBO; LA SCHEDA DEL MEDAGLIONE SALE DI 14 PIXEL'),
        ('esito_piede', PRIMA / 'esito_rapporto.png', BOZZE / 'cerimonia' / 'esito_piede_B.png',
         'L\'ESITO: I DUE PULSANTI GRIGI DIVENTANO I TASTI D\'OTTONE DEL PIEDE; IL RAPPORTO FINISCE A 424'),
    ],
    'documenti': [
        ('briefing', PRIMA / 'briefing.png', BOZZE / 'documenti' / 'briefing_A.png',
         'IL BRIEFING: GLI ORDINI SULLA TELESCRIVENTE, I LIVELLI COME SPIE, LA SCHEDA NEI VETRI, IL PIEDE COMUNE'),
        ('debriefing', PRIMA / 'debriefing_esito.png', BOZZE / 'documenti' / 'debriefing_A.png',
         'IL DEBRIEFING: LA CARTA NEL PANNELLO, IL RAPPORTO DATTILOSCRITTO SU CARTA, IL PIEDE COMUNE'),
        ('museo', PRIMA / 'museo.png', BOZZE / 'documenti' / 'museo_A.png',
         'IL MUSEO: LO SCHEDARIO NEL VETRO, L\'IMMAGINE, LA SCHEDA TECNICA E IL TESTO SU CARTA, I RIMANDI NUMERATI'),
        ('albo', PRIMA / 'albo.png', BOZZE / 'documenti' / 'albo_B.png',
         'L\'ALBO VUOTO: IL CARTELLINO DELLA CARRIERA NUOVA, I SETTE POSTI LIBERI, IL PIEDE DI CERIMONIA'),
    ],
}

L, A, TESTA, MARGINE = 640, 480, 22, 8


def tavola(prima, dopo, didascalia):
    out = Image.new('RGB', (2 * L + 3 * MARGINE, A + TESTA + 2 * MARGINE + 14), (24, 24, 28))
    d = ImageDraw.Draw(out)
    for k, (im, etichetta) in enumerate(((prima, 'PRIMA (OGGI)'), (dopo, 'DOPO (BOZZA)'))):
        x = MARGINE + k * (L + MARGINE)
        d.text((x, 6), etichetta, fill=(255, 255, 255) if k else (190, 190, 190))
        out.paste(im.convert('RGB').resize((L, A), Image.NEAREST), (x, TESTA))
    d.text((MARGINE, TESTA + A + MARGINE), didascalia, fill=(200, 200, 160))
    return out


def main():
    for gruppo, coppie in GRUPPI.items():
        for nome, p, b, didascalia in coppie:
            if not p.exists() or not b.exists():
                print(f'manca: {p if not p.exists() else b}')
                continue
            dest = BOZZE / gruppo / f'prima_dopo_{nome}.png'
            dest.parent.mkdir(parents=True, exist_ok=True)
            tavola(Image.open(p), Image.open(b), didascalia).save(dest)
            print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
