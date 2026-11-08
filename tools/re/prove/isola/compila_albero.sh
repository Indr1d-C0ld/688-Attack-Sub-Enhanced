#!/bin/bash
# compila_albero.sh <prima|lavoro> [bersagli...]: compila una delle copie private dei sorgenti di engine/build_danni (punto 4, prova d'identita'):
# stessa base, stessi dati copiati con i suoni; i sorgenti condivisi non le toccano
B=/home/randolph/Claude/melchior-688/engine/build_danni
export PATH=/home/randolph/Claude/melchior-688/tools/venv/lib/python3.13/site-packages/cmake/data/bin:$PATH
A=$1; shift
export TMPDIR=$B/tmp
S=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu/cmake/SDL3
[ -f "$B/b_$A/build.ninja" ] || cmake -S "$B/albero_$A/engine" -B "$B/b_$A" -G Ninja -DCMAKE_MAKE_PROGRAM=/home/randolph/Claude/melchior-688/tools/venv/bin/ninja -DSDL3_DIR="$S" >/dev/null || exit 1
exec nice -n 19 cmake --build "$B/b_$A" -j6 ${*:+--target $*}
