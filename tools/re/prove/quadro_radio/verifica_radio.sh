#!/bin/bash
# La verifica del quadro radio sui binari fissi di build_radio/fisso: casi identici, rilievo del classico, suite col registro intero, a nice 15.
R=/home/randolph/Claude/melchior-688
B=$R/engine/build_radio/${FISSO:-fisso}
P=$R/engine/build_radio/${VERIFICA:-verifica}
mkdir -p "$P/casi" "$P/suite/tmp" "$P/rilievo" || exit 1
export TMPDIR=$P/suite/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
md5sum "$B/melchior688" "$B/prove" "$B/rilievo_missioni" > "$P/md5.txt"
cd "$R/engine" || exit 1
CARTELLA=$P/casi nice -n 15 bash "$R/tools/regressione/casi.sh" "$B/melchior688" > "$P/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$P/casi.txt" > "$P/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$P/casi.diff") righe)"
CARTELLA=$P/rilievo nice -n 15 bash "$R/tools/regressione/rilievo_classico.sh" "$B/rilievo_missioni" 2>&1 | tail -3
cp "$B/prove" "$P/suite/prove_688"
(cd "$P/suite" && nice -n 15 ./prove_688 > registro.txt 2>&1; echo "suite: rc $?"; tail -3 registro.txt)
date
