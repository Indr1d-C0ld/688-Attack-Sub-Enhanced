#!/bin/bash
# Le partite non devono cambiare: prima (binario di build_trailer1, sorgenti d'ora senza la modifica) contro dopo (build_portata)
# a byte sulle tracce, col comandante IA (--sandbox) e con la stazione sonar del giocatore aperta. Piu' casi e rilievo. Nice 19.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/cascata; O=$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/esiti/verifica
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
mkdir -p "$O/casi" "$O/ril" "$C/tracce"; cd "$E" || exit 1
md5sum "$B/melchior688" "$C/melchior688_prima" "$B/rilievo_missioni" "$C/prove_cascata" > "$O/md5.txt"
(CARTELLA=$O/casi nice -n 19 "$R/tools/regressione/casi.sh" "$B/melchior688" > "$O/casi.txt" 2>&1; diff "$R/tools/regressione/riferimento.txt" "$O/casi.txt" > "$O/casi.diff"; echo "casi: diff rc $?" > "$O/casi.esito") &
(CARTELLA=$O/ril nice -n 19 "$R/tools/regressione/rilievo_classico.sh" "$B/rilievo_missioni" > "$O/rilievo.txt" 2>&1; echo "rilievo rc $?" > "$O/rilievo.esito") &
wait
una() {  # nome binario modo missione seme secondi
    local n=$1 bin=$2 modo=$3 mi=$4 se=$5 so=$6 T=$C/tracce
    if [ "$modo" = ia ]; then nice -n 19 "$bin" --sandbox --missione "$mi" --seme "$se" --secondi "$so" --salva-traccia "$T/$n.m688tr" --istantanea "$T/$n.bmp" > "$T/$n.txt" 2>&1
    else nice -n 19 "$bin" --missione "$mi" --seme "$se" --secondi "$so" --sonar moderno --regola simulazione --traffico si --stazione 8 --salva-traccia "$T/$n.m688tr" --istantanea "$T/$n.bmp" > "$T/$n.txt" 2>&1; fi
    rm -f "$T/$n.bmp"
}
export -f una; export C
{
  for s in 1 2; do
    for m in "m3 prove/PROVA_M3_ORIZZONTE 3600" "s5 prove/PROVA_S5_CONVOGLIO 7200" "s22 libreria/LIB_S22_ROTTA_LUNGA 7200"; do
      set -- $m
      for modo in ia st; do
        echo "$1_${modo}_${s}_prima $C/melchior688_prima $modo $2 $s $3"
        echo "$1_${modo}_${s}_dopo $B/melchior688 $modo $2 $s $3"
      done
    done
  done
} | xargs -P 6 -L 1 bash -c 'una "$@"' _
{
  for s in 1 2; do for m in m3 s5 s22; do for modo in ia st; do
    n=${m}_${modo}_${s}
    if cmp -s "$C/tracce/${n}_prima.m688tr" "$C/tracce/${n}_dopo.m688tr"; then r=UGUALI; else r=DIVERSE; fi
    echo "$n $r $(stat -c %s "$C/tracce/${n}_dopo.m688tr") byte; $(grep -h -oE 'impronta [0-9a-f]+, contatti [0-9a-f]+|ESITO[^,]*' "$C/tracce/${n}_dopo.txt" | tail -2 | tr '\n' ' ')"
  done; done; done
} > "$O/tracce.txt"
cat "$O/casi.esito" "$O/rilievo.esito"; cut -c1-160 "$O/casi.diff"; tail -2 "$O/rilievo.txt" | cut -c1-200; cat "$O/tracce.txt"
