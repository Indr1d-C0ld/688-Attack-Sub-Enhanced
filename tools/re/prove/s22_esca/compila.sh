#!/bin/bash
# compila.sh USCITA [altri .cpp o .o ...]: la sonda contro le librerie copiate da build-principale. Gli .o dati prima vincono sui membri degli archivi
cd /home/randolph/Claude/melchior-688/engine
E=$PWD; D=../tools/re/prove/s22_esca; L=build_chiusura/lib; O=$1; shift
c++ -DM688_DATI=\"$E/../data/motore\" -DM688_MISSIONI=\"$E/../data/missioni\" -Iapp -Ipiattaforma/include -Igrafica/include -Inucleo/include -Imissioni/include -isystem terzi/tomlplusplus -O2 -std=c++20 -c $D/sonda_s22.cpp -o build_chiusura/sonda_s22.o && \
c++ -O2 -o build_chiusura/$O build_chiusura/sonda_s22.o "$@" $L/libmissioni.a $L/libnucleo.a $L/libgrafica.a $L/libymfm.a $L/liblua54.a -lm
