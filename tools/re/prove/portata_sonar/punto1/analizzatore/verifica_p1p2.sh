#!/bin/bash
# Dopo P1 e P2: casi col riferimento, rilievo, suite intera, e le letture del kit prima e dopo (impronte e immagini). Binari di build_portata.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; O=$R/tools/re/prove/portata_sonar/punto1/analizzatore/esiti/verifica; K=$R/tools/re/prove/suite_acustica/sonda_figure/kit_vol1_c3; S=$R/tools/re/prove/portata_sonar/punto1/analizzatore/esiti/schermo
mkdir -p "$O/casi" "$O/rilievo" "$O/suite/tmp" "$S/finale"; export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu; cd "$E" || exit 1
cp "$B/melchior688" "$C/m688_p1p2"
( CARTELLA=$O/casi bash "$R/tools/regressione/casi.sh" "$C/m688_p1p2" > "$O/casi.txt" 2>&1; diff "$R/tools/regressione/riferimento.txt" "$O/casi.txt" > "$O/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$O/casi.diff") righe)" > "$O/casi.esito"; rm -rf "$O/casi" ) &
( CARTELLA=$O/rilievo bash "$R/tools/regressione/rilievo_classico.sh" "$B/rilievo_missioni" > "$O/rilievo.txt" 2>&1 ) &
( cd "$O/suite" && TMPDIR=$O/suite/tmp "$C/prove_ana" > "$O/suite/registro.txt" 2>&1; echo "suite: rc $?" > "$O/suite.esito"; tail -3 "$O/suite/registro.txt" >> "$O/suite.esito" ) &
foto() { # nome missione secondi tasti [extra]
    n=$1; mi=$2; se=$3; ta=$4; shift 4
    for pair in "prima $C/melchior688_m3" "dopo $C/m688_p1p2"; do set -- $pair "$@"; v=$1; b=$2; shift 2
        nice -n 10 "$b" "$@" --missione "$mi" --secondi "$se" --sonar moderno --regola simulazione --stazione 8 --tasti-a-tempo "$S/$ta" --istantanea "$S/finale/${v}_$n.bmp" 2>&1 | grep -o "impronta [0-9a-f]*, contatti [0-9a-f]*" | sed "s/^/$v $n /" >> "$O/impronte.txt"
    done
}
: > "$O/impronte.txt"
foto alfa SCENE2A 250 s2a2.tasti
foto tre_merc SCENE2A 250 s2a1.tasti
foto spruance SCENE1A 402 s1a.tasti
foto perry KIT_DEMON 250 kit.tasti --cartella-missioni "$K/missioni" --ordini "$K/bersaglio.ord"
wait
cat "$O/casi.esito" "$O/rilievo.txt" "$O/suite.esito" | cut -c1-200; cat "$O/impronte.txt"
