#!/bin/bash
# Rilievo del classico: le 110 partite delle missioni originali col giocatore passivo, confrontate col riferimento
# (engine/strumenti/riferimento/rilievo_missioni.tsv). Deve dare «0 partite diverse su 110». Uso: rilievo_classico.sh
# [binario di rilievo_missioni]. I file di lavoro vanno in $CARTELLA (predefinita: tools/re/prove/rilievo, nel
# progetto, perche' /tmp si svuota ai riavvii). Scritto il 26/10/2026 (sessione dell'acustica).
R=/home/randolph/Claude/melchior-688
B=${1:-$R/engine/build/rilievo_missioni}
D=${CARTELLA:-$R/tools/re/prove/rilievo}
mkdir -p "$D/tmp" || exit 1
export TMPDIR=$D/tmp
# una copia per esecuzione: la build comune puo' ricompilare mentre gira, e due esecuzioni insieme non si pestano
C="$D/rilievo.$$"
cp "$B" "$C" || exit 1
"$C" > "$D/classico.$$.tsv" 2> "$D/classico.$$.err" || { echo "rilievo fallito: $D/classico.$$.err"; rm -f "$C"; exit 1; }
rm -f "$C" "$D/classico.$$.err"
python3 -I "$R/engine/strumenti/confronta_rilievo.py" "$R/engine/strumenti/riferimento/rilievo_missioni.tsv" "$D/classico.$$.tsv" | tail -2
rm -f "$D/classico.$$.tsv"
