#!/bin/bash
# Verifica del runtime del 04/11/2026: rilievo del classico, casi contro il riferimento, prove mirate; sui binari copiati.
R=/home/randolph/Claude/melchior-688
V=$R/tools/re/prove/runtime_verifica_0411
B=$R/engine/build_runtime
cp "$B/melchior688" "$V/melchior688"; cp "$B/prove" "$V/prove"; cp "$B/rilievo_missioni" "$V/rilievo_missioni"
cd "$R/engine"
( CARTELLA=$V/rilievo bash "$R/tools/regressione/rilievo_classico.sh" "$V/rilievo_missioni" > "$V/rilievo.txt" 2>&1 ) &
( CARTELLA=$V/casi bash "$R/tools/regressione/casi.sh" "$V/melchior688" > "$V/casi.txt" 2>&1; diff "$R/tools/regressione/riferimento.txt" "$V/casi.txt" > "$V/casi.diff"; echo "casi: diff rc $?" > "$V/casi.esito" ) &
mkdir -p "$V/mirate"; cd "$V/mirate"
: > mirate.log
for t in 'interventi*' 'missili nella missione*' 'comandante IA*' 'sottomarini*' 'modifica del profilo*' 'copertura nella missione*' '*pannello*' 'difesa*' 'asw missione*' 'libretto nella missione*' '*spazi*' 'evasione dell*'; do
  echo "== $t" >> mirate.log
  TMPDIR=$PWD timeout 1500 "$V/prove" -tc="$t" 2>&1 | grep -E 'ERROR|values|test cases' | cut -c1-200 >> mirate.log
done
wait
echo finita
cat "$V/rilievo.txt" | tail -3; cat "$V/casi.esito"; wc -l "$V/casi.diff"; grep -E '^==|test cases' "$V/mirate/mirate.log" | cut -c1-100
