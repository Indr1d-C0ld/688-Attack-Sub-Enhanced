#!/bin/bash
# compila_esca.sh: sonda e riscrittura dell'esca contro la copia fissa di engine/build_s22 (librerie, header e dati copiati dopo la build
# pulita, con data/suoni accanto a data/motore): i sorgenti in movimento non le toccano
cd /home/randolph/Claude/melchior-688/engine
E=$PWD/build_s22; F=$E/fisso; L=$E/lib
export TMPDIR=$E/tmp
OPZ="-DM688_DATI=\"$F/motore\" -DM688_MISSIONI=\"$F/missioni_dati\" -I$F/app -I$F/piattaforma/include -I$F/grafica/include -I$F/nucleo/include -I$F/missioni/include -isystem terzi/tomlplusplus -O2 -std=c++20"
LIBS="$L/libmissioni.a $L/libnucleo.a $L/libgrafica.a $L/libymfm.a $L/liblua54.a -lm"
for x in sonda_esca riscrivi_esca; do
  c++ $OPZ -c ../tools/re/prove/s22_esca/$x.cpp -o $E/$x.o && c++ -O2 -o $E/$x $E/$x.o $LIBS || exit 1
done
