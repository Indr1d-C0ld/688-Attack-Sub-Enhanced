#!/bin/bash
# La sonda col modo rigioca (due cataloghi), e melchior688_fr: il gioco di misura con LOFAR_SENZA_FRANGE. Senza simboli di debug: disco.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; T=$R/tools/re/prove/portata_sonar/punto1/analizzatore; M=$B/tmp/matrice
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
cd "$B" || exit 1
FLAG="-DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -DM688_DATI_PREDEFINITI=\"$R/data/motore\" -DM688_MISSIONI_PREDEFINITE=\"$R/data/missioni\" -DTOML_ASSERT=(void)sizeof -I$E/app -I$E/piattaforma/include -I$E/grafica/include -I$E/nucleo/include -I$R/tools/deps/sysroot/usr/include -I$E/missioni/include -isystem $E/terzi/tomlplusplus -isystem $E/terzi/ymfm -O2 -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG -c "$T/sonda_analizzatore.cpp" -o "$C/sonda.o" 2>&1 | grep -E "error" | cut -c1-240
L="$B/libpiattaforma.a $B/libmissioni.a $B/libgrafica.a $R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu/libSDL3.so.0.2.10 $B/libnucleo.a $B/libymfm.a $B/liblua54.a -lm -lpthread"
RP=-Wl,-rpath,$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
nice -n 10 g++ -O2 $RP "$C/sonda.o" "$C/libapp.a" -o "$C/sonda" $L || exit 1
nice -n 10 g++ -O2 $RP "$C/sonda.o" "$C/libapp.a" "$M/acustica_m.o" "$C/navi_civili_m.o" -o "$C/sonda_senza_s3" $L || exit 1
strip --strip-debug "$C/sonda" "$C/sonda_senza_s3"
sed 's/std::vector<FascioChiesto> fasci{{fascio_, true}};/std::vector<FascioChiesto> fasci{{fascio_, !std::getenv("LOFAR_SENZA_FRANGE")}};/' "$C/sonar_enhanced_mis.cpp" > "$C/sonar_enhanced_fr.cpp"
grep -c LOFAR_SENZA_FRANGE "$C/sonar_enhanced_fr.cpp"
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_fr#g; s/ -g / /' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-240
[ -f "$C/sonar_enhanced_fr.o" ] || exit 1
Q=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
Q1=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_fr analizzatore/sonar_enhanced_fr.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$Q")
nice -n 10 bash -c "$Q1" || exit 1
strip --strip-debug "$C/melchior688_fr"
ls -la "$C/sonda" "$C/sonda_senza_s3" "$C/melchior688_fr"
