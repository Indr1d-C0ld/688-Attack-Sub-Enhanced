#!/bin/bash
# compila_sonde.sh <prima|lavoro>: sonda_identita (e, sul lavoro, sonda_isola) contro le librerie e gli header della copia privata
B=/home/randolph/Claude/melchior-688/engine/build_danni
A=$1; E=$B/albero_$A/engine; L=$B/b_$A
export TMPDIR=$B/tmp
OPZ="-std=c++20 -O2 -DM688_DATI=\"$B/dati/motore\" -I$E/nucleo/include -I$E/missioni/include -I$E/grafica/include -I$E/piattaforma/include -isystem $E/terzi/tomlplusplus"
LIBS="$L/libmissioni.a $L/libnucleo.a $L/libgrafica.a $L/libymfm.a $L/liblua54.a -lm"
S=/home/randolph/Claude/melchior-688/tools/re/prove/isola
c++ $OPZ $S/sonda_identita.cpp -o $L/sonda_identita $LIBS || exit 1
[ "$A" = prima ] && { c++ $OPZ $S/sonda_ia.cpp -o $L/sonda_ia $LIBS || exit 1; }
[ "$A" = lavoro ] && { c++ $OPZ $S/sonda_isola.cpp -o $L/sonda_isola $LIBS || exit 1; }
echo fatto $A
