"""Modello per il motore (modello.mdl) e compartimentazione (scafo.toml) della classe AGI Okean, senza toccare i moduli
condivisi del kit.

esporta_modello.py e compartimenti.py leggono i dati di classe da due tabelle condivise (opera_viva.OPERA_VIVA e
compartimenti.CLASSI) che non hanno ancora la voce 'agi'. Questo script la aggiunge in memoria, solo per questa
esecuzione, poi lancia i due strumenti come farebbero da riga di comando. Quando le due voci saranno copiate nelle
tabelle condivise (vedi extracted/navi/agi/RAPPORTO.md) basteranno i comandi normali:
    tools/venv/bin/python tools/navi/esporta_modello.py . tools/navi/classi/agi.yaml --verifica
    tools/venv/bin/python tools/navi/compartimenti.py . tools/navi/classi/agi.yaml

Uso (dalla radice del progetto): tools/venv/bin/python -I tools/navi/classi/agi_esporta.py [--solo-modello | --solo-scafo]
"""
import os, runpy, sys
RADICE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
NAVI = os.path.join(RADICE, 'tools', 'navi')
sys.path.insert(0, NAVI)
os.chdir(RADICE)
import opera_viva, compartimenti, costruisci, yaml

CLASSE = 'tools/navi/classi/agi.yaml'

# Opera viva: pescaggio 3,7 m a pieno carico, 760 t; un'elica a 4 pale (diesel singolo da 540 CV), un timone.
# Sezioni 'militare' (tonde, specchio immerso): il peschereccio laterale ha la poppa tonda a incrociatore.
OPERA_VIVA_AGI = dict(T=3.7, D=760, tipo='militare', eliche=(1, 2.0, 4), timoni=1)

# Compartimentazione (stima di gioco, affidabilita' bassa): sala macchine sotto la tuga di coperta (a poppavia del
# centro, come nei pescherecci laterali), ex stive del pesce a proravia trasformate in locali per gli apparati,
# nessun deposito di munizioni importante (solo gli Strela-2M portatili). Un peschereccio convertito: scafo
# civile, poche squadre, controllo dei danni modesto.
CLASSI_AGI = dict(tipo='militare', dislocamento_t=760, GM=0.7, compartimenti=6, squadre=1, qualita=0.5,
                  resistenza=0.6, macchine=[(29, 40)], depositi=[(25, 27, 'missili')])


def esporta_modello():
    opera_viva.OPERA_VIVA['agi'] = OPERA_VIVA_AGI
    sys.argv = ['esporta_modello.py', '.', CLASSE, '--verifica']
    runpy.run_path(os.path.join(NAVI, 'esporta_modello.py'), run_name='__main__')


def scafo_toml():
    """Come il ramo per le navi di superficie di compartimenti.py (__main__), per la sola classe agi."""
    opera_viva.OPERA_VIVA['agi'] = OPERA_VIVA_AGI
    compartimenti.CLASSI['agi'] = CLASSI_AGI
    cl = yaml.safe_load(open(CLASSE)); d = CLASSI_AGI
    mesh = costruisci.modello_intero(cl, os.path.dirname(os.path.abspath(CLASSE)))
    xs, dx, sez = compartimenti.sezioni(mesh)
    idro = compartimenti.idrostatica(xs, dx, sez)
    B = compartimenti.larghezza_galleggiamento(sez)
    celle, fuoco = compartimenti.compartimentazione(cl, d, xs, dx, B)
    compartimenti.scrivi(f"{cl['uscita']}/scafo.toml", cl, d, xs, dx, sez, celle, fuoco, idro, B)
    V, xb, kb, bm, aw = idro
    z_chiglia = min(z0 for _, z0, hb in sez if hb)
    print(f"{'classe':12s} {'disl. geom.':>11s} {'pubblicato':>10s} {'rapporto':>8s} {'KB':>5s} {'BM':>6s} {'KM':>6s} {'celle':>5s}")
    print(f"{'agi':12s} {V * 1.025:11.0f} {d['dislocamento_t']:10d} {V * 1.025 / d['dislocamento_t']:8.2f} "
          f"{kb - z_chiglia:5.2f} {bm:6.2f} {kb - z_chiglia + bm:6.2f} {len(celle):5d}   larghezza al galleggiamento {B:.2f} m")


if __name__ == '__main__':
    if '--solo-scafo' not in sys.argv:
        argv = list(sys.argv); esporta_modello(); sys.argv = argv
    if '--solo-modello' not in sys.argv:
        scafo_toml()
