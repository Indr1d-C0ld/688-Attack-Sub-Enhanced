#!/bin/bash
# col binario corretto: M4 A e C, S5 di controllo, che cosa ferma il Victor, le nove missioni vere (via della principale, 05/11)
R=/home/randolph/Claude/melchior-688; F=$R/engine/build_radio/fisso2; S=$R/engine/build_radio/sonda_scena/melchior688_sonda; U=$R/engine/build_radio/misura
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
E=$R/data/motore/esperimenti; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH
md5sum "$S" "$F/melchior688" > "$U/md5.txt"
{
for s in 1 2 3 5; do echo "m4a_$s --missione prove/PROVA_M4_QUADRO --seme $s --secondi 7200"; done
for s in 2 6 7 10; do echo "m4c_$s --esperimento $E/quadro_m4_krivak.m688v --seme $s --secondi 7200"; done
for s in 1 3 7; do echo "s5_$s --codice $S5 --seme $s --secondi 7200"; done
for m in $(cd "$R/data/missioni/libreria" && ls LIB_I02_*.lua LIB_I05_*.lua LIB_I06_*.lua LIB_I07_*.lua LIB_S03_*.lua LIB_S07_*.lua LIB_S08_*.lua LIB_S11_*.lua LIB_S15_*.lua); do
    n=${m%.lua}; c=$(echo "$n" | cut -d_ -f2); d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$m" | head -1 | cut -d' ' -f3)
    for s in 1 2; do echo "${c}_$s --missione libreria/$n --seme $s --secondi $d"; done
done
} > "$U/lavori.txt"
gioca() { n=$1; shift; t0=$(date +%s); nice -n 15 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "$n rc $? $(( $(date +%s) - t0 )) s" >> "$U/fatti.txt"; rm -f "$U/$n.bmp"; }
export -f gioca; export S U
xargs -P 7 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori.txt"
# il controllo: S5 semi 1 e 2 sul binario pulito, contro dopo/s5
nice -n 15 $F/serie_sandbox --semi 2 --da 1 --lavori 2 --motore $F/melchior688 --uscita "$U/ctl_s5" --codice $S5 > "$U/ctl_s5.log" 2>&1
date > "$U/fine.txt"
