#!/bin/bash
# Quello che manca alla corsa di base: S11 semi 3-8 (per il seme in cui il Los Angeles passa ad A2, §9.8), poi M3 e S22 a 12 semi sul binario pulito (§9.4). A nice 19, 6 alla volta.
R=/home/randolph/Claude/melchior-688; Z=$R/engine/build_radio/fisso3; S=$Z/melchior688_sonda; U=$R/engine/build_radio/base2; C=$R/engine/build_radio/fisso/sonda_codice
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
M3=8CGVJJ-KR0000-04K000-0000HZ-G001WN
f=$(cd "$R/data/missioni/libreria" && ls LIB_S11_*.lua); n=${f%.lua}; d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$f" | head -1 | cut -d' ' -f3)
: > "$U/lavori_extra.txt"
for s in 3 4 5 6 7 8; do echo "S11_$s --missione libreria/$n --seme $s --secondi $d" >> "$U/lavori_extra.txt"; done
gioca() { n=$1; shift; t0=$(date +%s); nice -n 19 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "$n rc $? $(( $(date +%s) - t0 )) s" >> "$U/fatti_extra.txt"; rm -f "$U/$n.bmp"; }
export -f gioca; export S U
xargs -P 6 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori_extra.txt"
date > "$U/fine_s11.txt"
nice -n 19 "$Z/serie_sandbox" --semi 12 --da 1 --lavori 6 --motore "$Z/melchior688" --uscita "$U/m3" --codice $M3 > "$U/m3.log" 2>&1
date > "$U/fine_m3.txt"
f=$(cd "$R/data/missioni/libreria" && ls LIB_S22_*.lua); n=${f%.lua}; d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$f" | head -1 | cut -d' ' -f3)
"$Z/serie_sandbox" --semi 1 --secondi 60 --motore "$Z/melchior688" --uscita "$U/cod_S22" -- --missione libreria/$n > /dev/null 2>&1
base=$(grep -m1 '^codice' "$U/cod_S22/serie.toml" | cut -d'"' -f2); cod=$("$C" "$base" $((d / 60)))
echo "S22 durata $d base $base codice $cod" >> "$U/codici.txt"
nice -n 19 "$Z/serie_sandbox" --semi 12 --da 1 --lavori 6 --motore "$Z/melchior688" --uscita "$U/s22" --codice "$cod" > "$U/s22.log" 2>&1
date > "$U/fine_extra.txt"
