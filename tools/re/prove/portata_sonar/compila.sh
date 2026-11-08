#!/bin/bash
# le sonde della portata, in engine/build_portata, contro gli archivi di build_radio (solo lettura: nucleo fermo dalle 12:16, missioni dalle 17:04)
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; A=$E/build_radio; T=$R/tools/re/prove/portata_sonar
mkdir -p "$B/tmp"; export TMPDIR=$B/tmp
FLAG="-DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -DM688_PORTATA_TMP=\"$B/tmp\" -I$E/missioni/include -I$E/nucleo/include -isystem $E/terzi/ymfm -O2 -g -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG -c "$T/sonda_tracce.cpp" -o "$B/sonda_tracce.o" &
nice -n 10 g++ $FLAG -c "$T/sonda_portata.cpp" -o "$B/sonda_portata.o" &
wait
g++ -O2 "$B/sonda_tracce.o" -o "$B/sonda_tracce" "$A/libmissioni.a" "$A/libnucleo.a" "$A/libymfm.a" "$A/liblua54.a" -lm -lpthread
g++ -O2 "$B/sonda_portata.o" -o "$B/sonda_portata" "$A/libmissioni.a" "$A/libnucleo.a" "$A/libymfm.a" "$A/liblua54.a" -lm -lpthread
ls -la "$B/sonda_tracce" "$B/sonda_portata"
