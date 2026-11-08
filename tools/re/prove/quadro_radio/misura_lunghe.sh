#!/bin/bash
# le cinque missioni piu' lunghe di 7200 s, fino alla durata scritta: la durata passa dal codice sandbox. A nice 19, 4 alla volta: la macchina e' della S22
R=/home/randolph/Claude/melchior-688; F=$R/engine/build_radio/fisso2; S=$R/engine/build_radio/sonda_scena/melchior688_sonda; C=$R/engine/build_radio/fisso/sonda_codice; U=$R/engine/build_radio/misura_lunghe
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
md5sum "$S" "$F/melchior688" > "$U/md5.txt"
: > "$U/lavori.txt"
for m in I06 I07 S03 S07 S15; do
    f=$(cd "$R/data/missioni/libreria" && ls LIB_${m}_*.lua); n=${f%.lua}
    d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$f" | head -1 | cut -d' ' -f3)
    $F/serie_sandbox --semi 1 --secondi 60 --motore $F/melchior688 --uscita "$U/cod_$m" -- --missione libreria/$n > /dev/null 2>&1
    base=$(grep -m1 '^codice' "$U/cod_$m/serie.toml" | cut -d'"' -f2)
    cod=$($C "$base" $((d / 60)))
    echo "$m durata $d base $base codice $cod" >> "$U/codici.txt"
    for s in 1 2; do echo "${m}_$s --codice $cod --seme $s --secondi $d" >> "$U/lavori.txt"; done
done
gioca() { n=$1; shift; t0=$(date +%s); nice -n 19 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "$n rc $? $(( $(date +%s) - t0 )) s" >> "$U/fatti.txt"; rm -f "$U/$n.bmp"; }
export -f gioca; export S U
xargs -P 4 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori.txt"
# il controllo sul binario pulito: stessa partita del binario di misura
l=$(grep '^S15_1 ' "$U/lavori.txt" | cut -d' ' -f2-)
nice -n 19 $F/melchior688 --sandbox $l --istantanea "$U/ctl.bmp" > "$U/ctl_S15_1.txt" 2>&1; rm -f "$U/ctl.bmp"
date > "$U/fine.txt"
