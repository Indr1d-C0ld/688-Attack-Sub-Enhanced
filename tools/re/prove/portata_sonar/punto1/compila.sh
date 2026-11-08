#!/bin/bash
# la sonda della portata contro gli archivi di build_portata (usare solo a build ferma)
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; T=$R/tools/re/prove/portata_sonar
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
FLAG="-DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -DM688_PORTATA_TMP=\"$B/tmp\" -I$E/missioni/include -I$E/nucleo/include -isystem $E/terzi/ymfm -isystem $E/terzi/tomlplusplus -O2 -g -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG -c "$T/sonda_portata.cpp" -o "$B/sonda_portata.o" || exit 1
g++ -O2 "$B/sonda_portata.o" -o "$B/sonda_portata" "$B/libmissioni.a" "$B/libnucleo.a" "$B/libymfm.a" "$B/liblua54.a" -lm -lpthread || exit 1
ls -la "$B/sonda_portata"
FLAG2="-I$E/nucleo/include -isystem $E/terzi/ymfm -isystem $E/terzi/tomlplusplus -O2 -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG2 "$T/sonda_tracce.cpp" -o "$B/sonda_tracce" "$B/libnucleo.a" "$B/libymfm.a" -lm -lpthread || exit 1
ls -la "$B/sonda_tracce"
