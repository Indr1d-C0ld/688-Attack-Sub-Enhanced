#!/bin/bash
# Il rigioco col binario finale (m4): la TB-16 col cavo filato davvero (1/3 a 1 s, filata a 30 s) e la sfera negli stessi istanti della stessa partita; poi le scene senza ordini,
# per le tracce di sonarista e IA. Gli istanti dopo un danno alla schiera (397, 398) si scartano nei conti. Nice 15, -P 6. Solo tsv: poco disco.
R=/home/randolph/Claude/melchior-688; E=$R/engine; C=$E/build_portata/analizzatore; O=$R/tools/re/prove/portata_sonar/punto1/analizzatore/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
rm -rf "$O/fin_tb16" "$O/fin_sfera" "$O/fin_scene"; mkdir -p "$O/fin_tb16" "$O/fin_sfera" "$O/fin_scene"; cd "$E" || exit 1
una() {  # cartella missione tempi traffico seme sensore|- ordini|-
    local d=$1 mi=$2 te=$3 tr=$4 se=$5 sn=$6 or=$7
    local n; n=$(echo "${mi}_${tr}_${se}" | tr '/,' '__')
    local fine=${te##*,}; fine=$((fine + 2))
    local extra=(); [ "$or" != "-" ] && extra=(--ordini "$or")
    local env_s=(A=1); [ "$sn" != "-" ] && env_s=(ANALIZZA_SENSORE="$sn")
    env "${env_s[@]}" ANALIZZA_SPAZZA="$te" ANALIZZA_USCITA="$O/$d/$n.tsv" ANALIZZA_SCENA="$mi" ANALIZZA_SEME="$se" ANALIZZA_TRAFFICO="$tr" nice -n 15 "$C/melchior688_m4" --missione "$mi" --secondi "$fine" --seme "$se" --sonar moderno --regola simulazione --traffico "$tr" --stazione 8 "${extra[@]}" --istantanea "$C/f_${d}_$n.bmp" 2> "$O/$d/$n.err" | grep -E "messaggio +(17|39[678]) |impronta" | sed 's/.*\(impronta\)/\1/' > "$O/$d/$n.msg"
    rm -f "$C/f_${d}_$n.bmp"
}
export -f una; export C O LD_LIBRARY_PATH
cavo() { for x in "SCENE1A 150,250,350 si" "SCENE1A 150,250,350 no" "SCENE2A 200,300,400 si" "SCENE2A 200,300,400 no" "SCENE4A 200,400,600,900 si" "SCENE5A 300,600,900,1500,3000 si" "prove/PROVA_M3_ORIZZONTE 600,1200,2400 si" "prove/PROVA_S5_CONVOGLIO 600,1200,2400,3600 si" "libreria/LIB_S22_ROTTA_LUNGA 600,1500,3000 si"; do echo "$x $1"; done; }
base() { for x in "SCENE1A 200,400,800 si" "SCENE1A 200,400,800 no" "SCENE2A 250,600 si" "SCENE2A 250,600 no" "SCENE2B 299,600 si" "SCENE2B 299,600 no" "SCENE4A 900 si" "SCENE5A 900,3000 si" "prove/PROVA_M3_ORIZZONTE 2400 si" "prove/PROVA_S5_CONVOGLIO 3600 si" "libreria/LIB_S22_ROTTA_LUNGA 3000 si"; do echo "$x $1"; done; }
{
    for s in 1 2 3 4 5; do
        cavo $s | awk -v f="$O/fila.ord" '{print "fin_tb16",$1,$2,$3,$4,"tb16",f}'
        cavo $s | awk -v f="$O/fila.ord" '{print "fin_sfera",$1,$2,$3,$4,"bqq5_sfera",f}'
        base $s | awk '{print "fin_scene",$1,$2,$3,$4,"-","-"}'
    done
} | xargs -P 6 -L 1 bash -c 'una "$@"' _
find "$O/fin_tb16" "$O/fin_sfera" "$O/fin_scene" -name '*.err' -empty -delete
echo "tb16 $(ls "$O"/fin_tb16/*.tsv | wc -l), sfera $(ls "$O"/fin_sfera/*.tsv | wc -l), tracce $(ls "$O"/fin_scene/*.tsv | wc -l), errori $(ls "$O"/fin_tb16/*.err "$O"/fin_sfera/*.err "$O"/fin_scene/*.err 2>/dev/null | wc -l)"
