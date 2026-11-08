#!/bin/bash
# Punto 1: congela il binario di oggi, fa statico e anteprime (griglia e sorgenti senza toccare i sorgenti), poi le basi. Nice 19, al massimo 6 processi.
set -u
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; P=$R/tools/re/prove/portata_sonar/punto1; Z=$B/p1_base; U=$P/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
mkdir -p "$Z" "$U/statico" "$U/corse" "$U/scene" "$U/serie_base"
for f in melchior688 serie_sandbox rilievo_missioni scene_interazioni sonda_portata sonda_tracce; do cp -p "$B/$f" "$Z/$f"; done
md5sum "$Z"/* > "$U/md5_base.txt"
md5sum "$R"/data/motore/acustica.toml "$R"/data/missioni/prove/PROVA_{M1_ANTINAVE,M3_ORIZZONTE,M4_QUADRO,S5_CONVOGLIO,S1_KPUG,S6_USCITA}.lua >> "$U/md5_base.txt"
AP=$P/anteprima/motore
cd "$R/engine" || exit 1
# lo statico: oggi (colonne B e Bx) e con le sorgenti nuove
nice -n 19 "$Z/sonda_portata" statico 90 > "$U/statico/oggi.tsv" 2> "$U/statico/oggi.err" &
PORTATA_DATI=$AP nice -n 19 "$Z/sonda_portata" statico 90 > "$U/statico/s3.tsv" 2> "$U/statico/s3.err" &
PORTATA_DATA="15 10 1989 9" PORTATA_DATI=$AP nice -n 19 "$Z/sonda_portata" statico 90 > "$U/statico/s3_ottobre.tsv" 2> "$U/statico/s3_ottobre.err" &
wait
echo statico fatto "$(date +%T)"
una() {   # conf modo km seme secondi
    local f=$U/corse/$1_$2_k$3_s$4.tsv extra=()
    case $1 in g) extra=(PORTATA_GRIGLIA=300);; gs) extra=(PORTATA_GRIGLIA=300 PORTATA_DATI=$AP);; esac
    env "${extra[@]}" nice -n 19 "$Z/sonda_portata" corsa "$3" 90 "$2" "$4" "$5" > "$f" 2> "$f.err"
}
scena() {  # conf scena seme secondi
    local f=$U/scene/$1_$2_s$3.txt extra=()
    case $1 in gs) extra=(PORTATA_GRIGLIA=300 PORTATA_DATI=$AP);; esac
    env "${extra[@]}" nice -n 19 "$Z/sonda_portata" scena "$2" "$3" "$4" > "$f" 2> "$f.err"
}
export -f una scena; export U Z AP
{
  for s in 1 2 3; do
    for k in 40 99 130; do echo "una oggi ia $k $s 3000"; echo "una g ia $k $s 3000"; done
    for k in 40 60 99 130 190; do echo "una gs ia $k $s 3000"; done
    for k in 40 99 130 190; do echo "una gs g $k $s 3000"; done
    for c in oggi gs; do
      echo "scena $c PROVA_M3_ORIZZONTE $s 3600"; echo "scena $c PROVA_S1_KPUG $s 3600"
      echo "scena $c PROVA_S6_USCITA $s 7200"; echo "scena $c PROVA_S5_CONVOGLIO $s 7200"
    done
  done
} | xargs -P 6 -L 1 bash -c '"$0" "$@"'
echo anteprime fatte "$(date +%T)"
# le basi vere, 10 semi
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH; S=(nice -n 19 "$Z/serie_sandbox" --semi 10 --da 1 --lavori 3 --motore "$Z/melchior688")
"${S[@]}" --uscita "$U/serie_base/m4" --codice $M4 > "$U/serie_base/m4.log" 2>&1 &
"${S[@]}" --uscita "$U/serie_base/s5" --codice $S5 > "$U/serie_base/s5.log" 2>&1 &
wait
"${S[@]}" --uscita "$U/serie_base/m3" -- --missione prove/PROVA_M3_ORIZZONTE > "$U/serie_base/m3.log" 2>&1 &
"${S[@]}" --uscita "$U/serie_base/m1" -- --missione prove/PROVA_M1_ANTINAVE > "$U/serie_base/m1.log" 2>&1 &
wait
echo basi fatte "$(date +%T)"
