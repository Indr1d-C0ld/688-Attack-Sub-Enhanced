#!/bin/bash
# Le scene col binario m2: oggi (5 semi), senza frange (3), TB-16 forzata senza cavo filato (3), LOFAR ai raggi (3). Nice 15, -P 5.
R=/home/randolph/Claude/melchior-688; E=$R/engine; C=$E/build_portata/analizzatore; O=$R/tools/re/prove/portata_sonar/punto1/analizzatore/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
mkdir -p "$O/m2" "$O/m2_fr" "$O/tb16" "$O/raggi2"; cd "$E" || exit 1
una() {  # cartella missione tempi traffico seme [env]
    local o=$1 mi=$2 te=$3 tr=$4 se=$5; shift 5
    local n; n=$(echo "${mi}_${te}_${tr}_${se}" | tr '/,' '__')
    local fine=${te##*,}; fine=$((fine + 2))
    env "$@" ANALIZZA_SPAZZA="$te" ANALIZZA_USCITA="$O/$o/$n.tsv" ANALIZZA_SCENA="$mi" ANALIZZA_SEME="$se" ANALIZZA_TRAFFICO="$tr" nice -n 15 "$C/melchior688_m2" --missione "$mi" --secondi "$fine" --seme "$se" --sonar moderno --regola simulazione --traffico "$tr" --stazione 8 --istantanea "$C/f_${o}_$n.bmp" > /dev/null 2> "$O/$o/$n.err"
    rm -f "$C/f_${o}_$n.bmp"
}
export -f una; export C O
scene() { # seme
    for x in "SCENE1A 200,400,800 si" "SCENE1A 200,400,800 no" "SCENE2A 250,600 si" "SCENE2A 250,600 no" "SCENE2B 299,600 si" "SCENE2B 299,600 no" "SCENE4A 900 si" "SCENE5A 900,3000 si" "prove/PROVA_M3_ORIZZONTE 2400 si" "prove/PROVA_S5_CONVOGLIO 3600 si" "libreria/LIB_S22_ROTTA_LUNGA 3000 si"; do echo "$x $1"; done
}
{
    for s in 1 2 3 4 5; do scene $s | sed 's/^/m2 /; s/$/ A=1/'; done
    for s in 1 2 3; do scene $s | sed 's/^/m2_fr /; s/$/ LOFAR_SENZA_FRANGE=1/'; done
    for s in 1 2 3; do scene $s | sed 's/^/tb16 /; s/$/ ANALIZZA_SENSORE=tb16/'; done
    for s in 1 2 3; do scene $s | sed 's/^/raggi2 /; s/$/ LOFAR_RAGGI=1/'; done
} | xargs -P 5 -L 1 bash -c 'una "$@"' _
find "$O/m2" "$O/m2_fr" "$O/tb16" "$O/raggi2" -name '*.err' -empty -delete
echo "m2 $(ls "$O"/m2/*.tsv | wc -l), fr $(ls "$O"/m2_fr/*.tsv | wc -l), tb16 $(ls "$O"/tb16/*.tsv | wc -l), raggi $(ls "$O"/raggi2/*.tsv | wc -l); errori $(ls "$O"/m2*/*.err "$O"/tb16/*.err "$O"/raggi2/*.err 2>/dev/null | wc -l)"
