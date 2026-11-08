#!/bin/bash
# S03 con la riga di stato al secondo, regola di prima (b_) e P3 (p_). Uso: corsa_carica.sh seme... (proposta.md §9.10)
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; U=$B/${USCITA:-carica}; S=$B/${SONDA:-sonda_carica}/melchior688_sonda
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
cod=$(grep '^S03 ' "$B/base2/codici.txt" | sed 's/.*codice //')
for s in "$@"; do echo "b $s"; echo "p $s"; done > "$U/lavori_$$.txt"
gioca() { k=$1; s=$2; [ "$k" = b ] && export M688_SENZA_P3=1; nice -n 19 "$S" --sandbox --codice "$cod" --seme "$s" --secondi 10800 --salva-traccia "$U/${k}_$s.m688tr" --istantanea "$U/${k}_$s.bmp" > "$U/${k}_$s.txt" 2>&1; echo "${k}_$s rc $?" >> "$U/fatti.txt"; rm -f "$U/${k}_$s.bmp"; }
export -f gioca; export S U cod
xargs -P 6 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori_$$.txt"
rm -f "$U/lavori_$$.txt"; date
