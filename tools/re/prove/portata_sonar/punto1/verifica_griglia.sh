#!/bin/bash
# Il passo 1: la griglia da sola contro gli attesi. Nice 19, al massimo 6 processi. Prima M3, che puo' fermare tutto.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; P=$R/tools/re/prove/portata_sonar/punto1; Z=$B/p1_base; G=$B/p1_griglia; U=$P/esiti/griglia; D=$P/../data_scene
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
mkdir -p "$G" "$U/corse" "$U/serie" "$U/casi" "$U/rilievo"
for f in melchior688 serie_sandbox rilievo_missioni scene_interazioni sonda_portata sonda_tracce; do cp -p "$B/$f" "$G/$f"; done
md5sum "$G"/* > "$U/md5.txt"
cd "$R/engine" || exit 1
nice -n 5 "$G/sonda_portata" costo > "$U/costo.txt" 2>&1
E=$R/data/motore/esperimenti/spia_oltre_orizzonte.m688v
nice -n 19 "$G/serie_sandbox" --semi 10 --da 1 --lavori 4 --motore "$G/melchior688" --uscita "$U/serie/m3" -- --missione prove/PROVA_M3_ORIZZONTE > "$U/serie/m3.log" 2>&1 &
nice -n 19 "$G/serie_sandbox" --semi 5 --da 1 --lavori 1 --motore "$G/melchior688" --uscita "$U/serie/m3_spia" -- --esperimento "$E" > "$U/serie/m3_spia.log" 2>&1 &
nice -n 19 "$Z/serie_sandbox" --semi 5 --da 1 --lavori 1 --motore "$Z/melchior688" --uscita "$U/serie/m3_spia_base" -- --esperimento "$E" > "$U/serie/m3_spia_base.log" 2>&1 &
wait
echo M3 fatta "$(date +%T)"
BIN=$Z/melchior688 GIRO=$U/giro_base LAVORI=3 bash "$D/giro_scene.sh" &
BIN=$G/melchior688 GIRO=$U/giro_griglia LAVORI=3 bash "$D/giro_scene.sh" &
wait
diff "$U/giro_base/righe.txt" "$U/giro_griglia/righe.txt" > "$U/giro.diff"
echo scene fatte "$(date +%T)"
una() { nice -n 19 "$G/sonda_portata" corsa "$2" 90 "$1" "$3" "$4" > "$U/corse/g_$1_k$2_s$3.tsv" 2> "$U/corse/g_$1_k$2_s$3.tsv.err"; }
export -f una; export U G
{ for s in 1 2 3; do for k in 40 99 130; do echo "ia $k $s 3000"; done; done; } | xargs -P 6 -L 1 bash -c 'una "$0" "$1" "$2" "$3"'
echo corse fatte "$(date +%T)"
CARTELLA=$U/casi nice -n 15 bash "$R/tools/regressione/casi.sh" "$G/melchior688" > "$U/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$U/casi.txt" > "$U/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$U/casi.diff") righe)"
CARTELLA=$U/rilievo nice -n 15 bash "$R/tools/regressione/rilievo_classico.sh" "$G/rilievo_missioni" 2>&1 | tail -3
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH; S=(nice -n 19 "$G/serie_sandbox" --semi 10 --da 1 --lavori 3 --motore "$G/melchior688")
"${S[@]}" --uscita "$U/serie/m4" --codice $M4 > "$U/serie/m4.log" 2>&1 &
"${S[@]}" --uscita "$U/serie/s5" --codice $S5 > "$U/serie/s5.log" 2>&1 &
wait
"${S[@]}" --uscita "$U/serie/m1" -- --missione prove/PROVA_M1_ANTINAVE > "$U/serie/m1.log" 2>&1
echo fine "$(date +%T)"
