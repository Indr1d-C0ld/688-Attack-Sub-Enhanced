#!/bin/bash
# Il passo 2: griglia e sorgenti contro gli attesi. Nice 19, al massimo 6 processi. Binari in p1_s3.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; P=$R/tools/re/prove/portata_sonar/punto1; Z=$B/p1_base; G=$B/p1_griglia2; S3=$B/p1_s3; U=$P/esiti/s3; D=$P/../data_scene
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
mkdir -p "$U/statico" "$U/corse" "$U/scene" "$U/serie" "$U/casi" "$U/rilievo"
cd "$R/engine" || exit 1
nice -n 19 "$S3/sonda_portata" statico 90 > "$U/statico/s3.tsv" 2> "$U/statico/s3.err" &
PORTATA_DATA="15 10 1989 9" nice -n 19 "$S3/sonda_portata" statico 90 > "$U/statico/s3_ottobre.tsv" 2> "$U/statico/s3_ottobre.err" &
wait
echo statico fatto "$(date +%T)"
una() { nice -n 19 "$S3/sonda_portata" corsa "$2" 90 "$1" "$3" "$4" > "$U/corse/gs_$1_k$2_s$3.tsv" 2> "$U/corse/gs_$1_k$2_s$3.tsv.err"; }
scena() { nice -n 19 "$S3/sonda_portata" scena "$1" "$2" "$3" > "$U/scene/gs_$1_s$2.txt" 2> "$U/scene/gs_$1_s$2.txt.err"; }
export -f una scena; export U S3
{
  for s in 1 2 3; do
    for k in 40 60 99 130 190; do echo "una ia $k $s 3000"; done
    for k in 40 99 130 190; do echo "una g $k $s 3000"; done
    echo "scena PROVA_M3_ORIZZONTE $s 3600"; echo "scena PROVA_S1_KPUG $s 3600"; echo "scena PROVA_S6_USCITA $s 7200"; echo "scena PROVA_S5_CONVOGLIO $s 7200"
  done
} | xargs -P 6 -L 1 bash -c '"$0" "$@"'
echo corse e scene fatte "$(date +%T)"
BIN=$S3/melchior688 GIRO=$U/giro LAVORI=6 bash "$D/giro_scene.sh"
diff "$P/esiti/griglia/giro_griglia2/righe.txt" "$U/giro/righe.txt" > "$U/giro.diff"
echo 23 scene fatte "$(date +%T)"
cd "$R/engine"
for b in "$Z" "$S3"; do
  n=$(basename "$b")
  nice -n 15 "$b/melchior688" --sandbox --missione SCENE4A --seme 1 --secondi 300 --istantanea "$U/scene4a_$n.bmp" > "$U/scene4a_$n.log" 2>&1
  rm -f "$U/scene4a_$n.bmp"
done
CARTELLA=$U/casi nice -n 15 bash "$R/tools/regressione/casi.sh" "$S3/melchior688" > "$U/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$U/casi.txt" > "$U/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$U/casi.diff") righe)"
CARTELLA=$U/rilievo nice -n 15 bash "$R/tools/regressione/rilievo_classico.sh" "$S3/rilievo_missioni" 2>&1 | tail -3
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH; S=(nice -n 19 "$S3/serie_sandbox" --semi 10 --da 1 --lavori 3 --motore "$S3/melchior688")
"${S[@]}" --uscita "$U/serie/m4" --codice $M4 > "$U/serie/m4.log" 2>&1 &
"${S[@]}" --uscita "$U/serie/s5" --codice $S5 > "$U/serie/s5.log" 2>&1 &
wait
"${S[@]}" --uscita "$U/serie/m3" -- --missione prove/PROVA_M3_ORIZZONTE > "$U/serie/m3.log" 2>&1 &
"${S[@]}" --uscita "$U/serie/m1" -- --missione prove/PROVA_M1_ANTINAVE > "$U/serie/m1.log" 2>&1 &
wait
echo fine "$(date +%T)"
