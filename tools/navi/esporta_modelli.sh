#!/bin/bash
# Esporta i modelli 3D con l'opera viva (tools/navi/esporta_modello.py) di tutte le classi di navi e sottomarini
# (quelle di tools/navi/opera_viva.py), con il confronto con gli sprite precalcolati.
#   tools/navi/esporta_modelli.sh [classe.yaml ...]
# Registro in extracted/navi/_log/modelli.log
cd "$(dirname "$0")/../.."
mkdir -p extracted/navi/_log
LOG=extracted/navi/_log/modelli.log
if [ $# -eq 0 ]; then
  set -- $(tools/venv/bin/python -I -c "
import sys, yaml, glob, os
sys.path.insert(0, 'tools/navi')
from opera_viva import OPERA_VIVA
for f in sorted(glob.glob('tools/navi/classi/*.yaml')):
    try: cl = yaml.safe_load(open(f))
    except yaml.YAMLError: continue      # maschere e triangolazioni (piu' documenti)
    if isinstance(cl, dict) and 'uscita' in cl and 'scafo' in cl and os.path.basename(os.path.normpath(cl['uscita'])) in OPERA_VIVA:
        print(f)")
fi
for f in "$@"; do
  echo "== $f" | tee -a "$LOG"
  tools/venv/bin/python tools/navi/esporta_modello.py . "$f" --verifica 2>&1 | tee -a "$LOG"
done
