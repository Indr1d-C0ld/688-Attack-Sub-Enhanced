#!/bin/bash
# Passaggio sul binario finale (07/11/2026, principale): build incrementale di build-principale dopo il reticolo,
# casi, rilievo, suite. Il binario per l'utente si copia a parte dopo casi e rilievo.
set -u
R=/home/randolph/Claude/melchior-688
B=$R/engine/build-principale
P=$R/tools/re/prove/verifica_finale
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
cmake --build "$B" -j6 --target melchior688 prove rilievo_missioni > "$P/build.log" 2>&1 || { echo "build fallita"; tail -30 "$P/build.log"; exit 1; }
cp "$B/melchior688" "$P/m688.finale"; md5sum "$P/m688.finale"
echo "build finita alle $(date +%T)"
cd "$R/engine"
CARTELLA=$P/regressione bash "$R/tools/regressione/casi.sh" "$P/m688.finale" > "$P/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$P/casi.txt" > "$P/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$P/casi.diff") righe) alle $(date +%T)"
bash "$R/tools/regressione/rilievo_classico.sh" "$B/rilievo_missioni" 2>&1 | tail -3
echo "rilievo finito alle $(date +%T)"
cp "$B/prove" "$P/suite/prove_688"
(cd "$P/suite" && TMPDIR=$P/suite/tmp ./prove_688 > "$P/suite/registro.txt" 2>&1; echo "suite: rc $? alle $(date +%T)"; tail -3 registro.txt)
