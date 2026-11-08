#!/bin/bash
# compila_prova.sh SORGENTE USCITA: la sola prova della libreria, contro le librerie copiate da build-principale
cd /home/randolph/Claude/melchior-688/engine
F='-DM688_DATI="/home/randolph/Claude/melchior-688/engine/../data/motore" -DM688_MISSIONI="/home/randolph/Claude/melchior-688/engine/../data/missioni" -Iapp -Ipiattaforma/include -Igrafica/include -Inucleo/include -Imissioni/include -Itest -isystem terzi/doctest -isystem terzi/tomlplusplus -O2 -DNDEBUG -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion'
c++ $F -c "$1" -o build_chiusura/$2.o || exit 1
[ -f build_chiusura/main.o ] || c++ $F -c test/main.cpp -o build_chiusura/main.o || exit 1
c++ -O2 -o build_chiusura/$2 build_chiusura/main.o build_chiusura/$2.o build_chiusura/lib/libgrafica.a build_chiusura/lib/libmissioni.a build_chiusura/lib/libnucleo.a build_chiusura/lib/libymfm.a build_chiusura/lib/liblua54.a -lm
