#!/bin/bash
# Ricostruisce in parallelo le classi indicate e i loro fogli di confronto (log in extracted/navi/_log/).
#   tools/navi/ricostruisci_classi.sh [-p PASSO] classe1 classe2 ...     (PASSO predefinito 5)
set -u
cd "$(dirname "$0")/../.."
PASSO=5
if [[ "${1:-}" == "-p" ]]; then PASSO=$2; shift 2; fi
LOG=extracted/navi/_log; mkdir -p "$LOG"
# prefisso degli sprite originali per il foglio di confronto (riferimento di stile se la classe non li ha)
declare -A PREF=([perry]=OHP [krivak2]=KRIVAK [kidd]=KIDD [spruance]=KIDD [iowa]=IOWA [nimitz]=NIMITZ [sacramento]=SAC
  [kara]=KARA [kirov]=KIROV [slava]=SLAVA [koni]=KRIVAK [nanuchka]=NUCHKA [agi]=KRIVAK [petroliera]=TANKER [mercantile]=MERCH [peschereccio]=MERCH
  [cargo]=CARGO [p3c]=SEAHAWK [il38]=SEAHAWK [tu142]=SEAHAWK [sh60b]=SEAHAWK [ka25]=SEAHAWK [tomahawk]=MISSILE
  [harpoon]=MISSILE [p700]=MISSILE [bqm74]=MISSILE)
for c in "$@"; do
  (
    tools/venv/bin/python -I tools/navi/costruisci.py "$PWD" "tools/navi/classi/$c.yaml" "$PASSO" > "$LOG/$c.log" 2>&1 \
    && tools/venv/bin/python -I tools/navi/foglio_confronto.py "$PWD" "extracted/navi/$c" "extracted/png/${PREF[$c]:-KRIVAK}" >> "$LOG/$c.log" 2>&1 \
    && echo "$c OK" || echo "$c ERRORE (vedi $LOG/$c.log)"
  ) &
done
wait
