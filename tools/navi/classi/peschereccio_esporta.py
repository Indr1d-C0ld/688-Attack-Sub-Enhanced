"""Esportazione del peschereccio per il motore senza toccare i moduli comuni del kit.

Le tabelle per classe dell'opera viva (tools/navi/opera_viva.py, OPERA_VIVA) e della compartimentazione
(tools/navi/compartimenti.py, CLASSI) non hanno ancora la voce `peschereccio`: questo script la aggiunge in memoria
e poi esegue i due programmi comuni cosi' come sono. Quando le due voci qui sotto saranno copiate nei moduli comuni
(da chi ne ha la proprieta'), basteranno i comandi normali:
  tools/venv/bin/python -I tools/navi/esporta_modello.py <radice> tools/navi/classi/peschereccio.yaml --verifica
  tools/venv/bin/python -I tools/navi/compartimenti.py <radice> tools/navi/classi/peschereccio.yaml

Uso: peschereccio_esporta.py <radice del progetto> [modello|compartimenti|tutto]   (predefinito: tutto)

Dati (stime da schede pubbliche, affidabilita' bassa; vedi extracted/navi/peschereccio/RAPPORTO.md):
- opera viva: pescaggio 5,0 m (estivo dell'Atlantic Peace, 4,99 m), dislocamento a pieno carico ~2.000 t (stima:
  50 x 13 x 5 m al galleggiamento, coefficiente di blocco ~0,58), sezioni piene da peschereccio ('mercantile'
  con esponente di mezzo 4 invece di 8: fondo stellato, non piatto), un'elica a passo variabile da 3,2 m a 4 pale,
  un timone, bulbo moderato come sul Langevin (1988);
- compartimentazione: paratia di collisione e 6 compartimenti; sala macchine sotto il ponte di lavoro (38-50 m dalla
  prua), stiva del pesce congelato a proravia (12-36 m, permeabilita' delle merci varie), GM ~0,8 m, una squadra
  di controllo danni di un peschereccio (qualita' 0,5), scafo robusto da classe ghiaccio (resistenza 1,0).
"""
import os, runpy, sys

RADICE = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..'))
COSA = sys.argv[2] if len(sys.argv) > 2 else 'tutto'
NAVI = os.path.join(RADICE, 'tools', 'navi')
CLASSE = os.path.join(RADICE, 'tools', 'navi', 'classi', 'peschereccio.yaml')
sys.path.insert(0, NAVI)

OPERA_VIVA = dict(T=5.0, D=2000, tipo='mercantile', n_mezzo=4.0, eliche=(1, 3.2, 4), timoni=1, bulbo=True)
COMPARTIMENTI = dict(tipo='mercantile', dislocamento_t=2000, GM=0.8, compartimenti=6, squadre=1, qualita=0.5,
                     resistenza=1.0, macchine=[(38, 50)], carico=[(12, 36, 0.60)])

import opera_viva  # noqa: E402  (stesso modulo che importano costruisci ed esporta_modello)
opera_viva.OPERA_VIVA.setdefault('peschereccio', OPERA_VIVA)


def esegui_con_voce(file, tabella, voce):
    """Esegue un programma del kit come __main__ dopo aver aggiunto `voce` alla sua tabella `tabella` (il programma
    e' diviso in due: definizioni, poi il blocco principale)."""
    sorgente = open(file, encoding='utf-8').read()
    i = sorgente.index("if __name__ == '__main__':")
    spazio = {'__name__': '__main__', '__file__': file}
    exec(compile(sorgente[:i], file, 'exec'), spazio)
    spazio[tabella].setdefault('peschereccio', voce)
    exec(compile(sorgente[:i].count('\n') * '\n' + sorgente[i:], file, 'exec'), spazio)


if COSA in ('modello', 'tutto'):
    sys.argv = [os.path.join(NAVI, 'esporta_modello.py'), RADICE, CLASSE, '--verifica']
    runpy.run_path(sys.argv[0], run_name='__main__')
if COSA in ('compartimenti', 'tutto'):
    sys.argv = [os.path.join(NAVI, 'compartimenti.py'), RADICE, CLASSE]
    esegui_con_voce(sys.argv[0], 'CLASSI', COMPARTIMENTI)
