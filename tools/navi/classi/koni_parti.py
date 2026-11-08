"""Parti proprie della classe Koni (Progetto 1159), non presenti nel kit condiviso.

ak230: impianto binato da 30 mm AK-230 (torretta chiusa a cupola, senza serventi): basamento
cilindrico basso, corpo cilindrico, cupola e due canne corte. Sul Koni ce ne sono due, affiancati
all'altezza dell'albero sul ponte 01 (posizione e quote misurate sulla foto di fianco della 356 cubana).
ak726: impianto binato da 76 mm AK-726 (impianto_binato del kit in materiale chiaro).
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import kit


def ak726(**kw):
    """AK-726 binato da 76 mm: l'impianto_binato del kit (casamatta a fronte smussato, due canne) con la
    casamatta in materiale chiaro. Le facce rivolte a poppa non sono viste da nessuna foto e restano
    all'ombreggiatura del modello: con l'albedo chiara si avvicinano al grigio chiaro delle foto."""
    m = kit.impianto_binato(**kw)
    m.mat = ['bianco' if a == 'sovrastruttura' else a for a in m.mat]
    return m


PARTI = dict(ak726=ak726)
