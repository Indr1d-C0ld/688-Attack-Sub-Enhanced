#!/bin/bash
# Verifica completa della principale da zero: build pulita di engine/build-principale, poi i casi (diff col
# riferimento), il rilievo del classico, la suite e le partite sandbox. Prima va chiesta alle altre sessioni una
# finestra senza modifiche agli header (una build mista da` binari sbagliati). Scritto il 29/10/2026 (principale).
set -u
R=$(cd "$(dirname "$0")/../.." && pwd)
B=$R/engine/build-principale
P=$R/tools/re/prove/verifica
mkdir -p "$P/regressione" "$B/tmp"
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
cmake --build "$B" --clean-first -j6 --target melchior688 prove rilievo_missioni > "$P/build.log" 2>&1 || { echo "build fallita"; tail -30 "$P/build.log"; exit 1; }
cp "$B/melchior688" "$P/m688.verifica"
echo "build finita alle $(date +%T): la finestra senza header si puo' chiudere"
cd "$R/engine"
CARTELLA=$P/regressione bash "$R/tools/regressione/casi.sh" "$P/m688.verifica" > "$P/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$P/casi.txt" > "$P/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$P/casi.diff") righe)"
bash "$R/tools/regressione/rilievo_classico.sh" "$B/rilievo_missioni" 2>&1 | tail -3
# la suite senza limite di tempo e col registro intero: con la macchina carica suite.sh muore a un'ora (04/11/2026)
mkdir -p "$P/suite/tmp"; cp "$B/prove" "$P/suite/prove_688"  # nome proprio: un `pkill -x prove` di un altro progetto ha gia' fermato una suite (05/11/2026)
(cd "$P/suite" && TMPDIR=$P/suite/tmp ./prove_688 > "$P/suite/registro.txt" 2>&1; echo "suite: rc $?"; tail -3 registro.txt)
bash "$R/tools/regressione/sandbox_scene.sh" "$P/m688.verifica" 3600 > "$P/scene.txt" 2>&1
echo "scene: $(grep -c '^== ' "$P/scene.txt") partite, $(grep '^== ' "$P/scene.txt" | grep -vc 'uscita 0,') con uscita diversa da 0"
