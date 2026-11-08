#!/bin/bash
# Le partite sandbox di controllo dopo ogni fase (docs/19_sandbox.md §8): ogni scena di prova S1-S7
# (data/missioni/prove) su tre semi, senza finestra, coi messaggi riassunti per codice (senza i ping) e l'impronta
# finale. Uso: sandbox_scene.sh [binario di melchior688] [secondi, predefiniti 3600]. I file di lavoro vanno in
# $CARTELLA (predefinita: tools/re/prove/sandbox, nel progetto). Scritto il 27/10/2026 (principale).
R=/home/randolph/Claude/melchior-688
B=${1:-$R/engine/build-principale/melchior688}
S=${2:-3600}
D=${CARTELLA:-$R/tools/re/prove/sandbox}
mkdir -p "$D" || exit 1
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
# una copia: la build comune puo' ricompilare mentre gira
C="$D/melchior688.$$"
cp "$B" "$C" || exit 1
cd "$R/engine" || exit 1
for f in "$R"/data/missioni/prove/PROVA_*.lua; do
    m=prove/$(basename "$f" .lua)
    for seme in 1 2 3; do
        "$C" --sandbox --missione "$m" --seme $seme --secondi "$S" --istantanea "$D/s.bmp" > "$D/log.txt" 2>&1
        rc=$?
        impronta=$(grep -o 'impronta [0-9a-f]*' "$D/log.txt" | tail -1)
        echo "== $m seme $seme: uscita $rc, $impronta"
        grep '^  t=' "$D/log.txt" | grep -v 'Sonar attivo\|pingando' | awk '{$1=$2=$3=""; print}' | sort | uniq -c | sort -rn | head -8
    done
done
rm -f "$C" "$D/s.bmp" "$D/log.txt"
