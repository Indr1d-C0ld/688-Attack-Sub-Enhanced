#!/bin/bash
# la sonda della M4 sulla radice di prova: $1 rilevamento, $2 durata e secondi, $3 semi; poi le distanze in km. Non tocca data/
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio/fisso; S=$R/engine/build_radio/sonda_scena
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$S
RIL=$1; DUR=$2; SEMI=$3; shift 3
cd "$R/engine" || exit 1
for km in "$@"; do
    C=$S/r${RIL}_k${km}_d${DUR}; mkdir -p "$C/m"
    M4_KM=$km M4_RIL=$RIL M4_DUR=$DUR $S/scene --uscita "$C/m" | grep "M4:"
    nice -n 15 $B/serie_sandbox --semi $SEMI --da 1 --lavori $SEMI --secondi $DUR --motore $B/melchior688 --uscita "$C/u" -- --cartella-missioni "$C/m" --missione PROVA_M4_QUADRO > "$C/log.txt" 2>&1 &
done
wait
for km in "$@"; do tail -1 "$S/r${RIL}_k${km}_d${DUR}/log.txt" | cut -c1-120; done
