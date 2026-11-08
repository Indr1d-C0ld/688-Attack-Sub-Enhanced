#!/bin/bash
# La sonda dell'analizzatore contro gli archivi di build_portata (sorgenti del gioco non toccati) e una seconda, senza S3:
# acustica_m.o (V_SENZA_SPETTRO: niente -17 dB) e una copia di navi_civili con la tonale del traffico di prima.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; T=$R/tools/re/prove/portata_sonar/punto1/analizzatore
M=$B/tmp/matrice
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
mkdir -p "$C"; cd "$B" || exit 1
FLAG="-DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -DM688_DATI_PREDEFINITI=\"$R/data/motore\" -DM688_MISSIONI_PREDEFINITE=\"$R/data/missioni\" -DTOML_ASSERT=(void)sizeof -I$E/app -I$E/piattaforma/include -I$E/grafica/include -I$E/nucleo/include -I$R/tools/deps/sysroot/usr/include -I$E/missioni/include -isystem $E/terzi/tomlplusplus -isystem $E/terzi/ymfm -O2 -g -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG -c "$T/sonda_analizzatore.cpp" -o "$C/sonda.o" || exit 1
L="$B/libpiattaforma.a $B/libmissioni.a $B/libgrafica.a $R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu/libSDL3.so.0.2.10 $B/libnucleo.a $B/libymfm.a $B/liblua54.a -lm -lpthread"
A="$B/CMakeFiles/melchior688.dir/app"
# tutta l'app salvo main, in un archivio: il linker prende quello che sonar_enhanced chiede
rm -f "$C/libapp.a"; nice -n 10 ar crs "$C/libapp.a" $(ls "$A"/*.cpp.o | grep -v "/main.cpp.o")
OB="$C/libapp.a"
RP=-Wl,-rpath,$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
nice -n 10 g++ -O2 $RP "$C/sonda.o" $OB -o "$C/sonda" $L || exit 1
sed -E 's/^(constexpr double TONALE_SOTTO_LARGA_DB = )35\.0;/const double TONALE_SOTTO_LARGA_DB = std::getenv("V_SENZA_SPETTRO") ? 18.0 : 35.0;/; 1i #include <cstdlib>' "$E/nucleo/src/navi_civili.cpp" > "$C/navi_civili_m.cpp"
grep -c "V_SENZA_SPETTRO" "$C/navi_civili_m.cpp"
nice -n 10 g++ $FLAG -c "$C/navi_civili_m.cpp" -o "$C/navi_civili_m.o" || exit 1
nice -n 10 g++ -O2 $RP "$C/sonda.o" $OB "$M/acustica_m.o" "$C/navi_civili_m.o" -o "$C/sonda_senza_s3" $L || exit 1
ls -la "$C/sonda" "$C/sonda_senza_s3"
