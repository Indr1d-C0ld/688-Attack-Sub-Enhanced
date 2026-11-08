#!/bin/bash
# Suite completa delle prove del motore (engine/build/prove), su una copia del binario: la build comune puo'
# ricompilare mentre gira. Stampa le prove rosse e il riepilogo. Uso: suite.sh [binario di prove]. I file di lavoro
# vanno in $CARTELLA (predefinita: tools/re/prove/suite, nel progetto, perche' /tmp si svuota ai riavvii). Insieme a
# rilievo_classico.sh e' la regressione del runtime. Scritto il 26/10/2026 (sessione del runtime); il limite di tempo
# portato a un'ora il 30/10 (oltre mille prove, macchina condivisa).
R=/home/randolph/Claude/melchior-688
B=${1:-$R/engine/build/prove}
D=${CARTELLA:-$R/tools/re/prove/suite}
mkdir -p "$D/tmp" || exit 1
export TMPDIR=$D/tmp
cp "$B" "$D/prove" || exit 1
cd "$D" || exit 1
timeout 3600 ./prove 2>&1 | grep -E "^TEST CASE|ERROR|values|test cases|Status" | tail -20
