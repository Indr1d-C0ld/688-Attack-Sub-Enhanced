#!/bin/bash
# riparato.sh <prima|dopo>: sonda_identita, casi e partite senza comandi sul binario di build_s22, copiato in build_s22/riparato (il messaggio «riparato» ripetuto, 06/11/2026)
E=/home/randolph/Claude/melchior-688/engine; A=$1; O=$E/build_s22/riparato; U=$O/$A; rm -rf "$U"; mkdir -p "$U/casi" "$U/tracce" "$E/build_s22/tmp"
export TMPDIR=$E/build_s22/tmp PATH=/home/randolph/Claude/melchior-688/tools/venv/lib/python3.13/site-packages/cmake/data/bin:$PATH LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$E" || exit 1
nice -n 19 cmake --build build_s22 -j6 --target nucleo missioni grafica melchior688 rilievo_missioni prove 2>&1 | grep -E "error|FAILED|ninja: |Linking CXX executable" | cut -c1-200 | tail -5
c++ -std=c++20 -O2 -DM688_DATI=\"/home/randolph/Claude/melchior-688/data/motore\" -Inucleo/include -Imissioni/include -Igrafica/include -Ipiattaforma/include -isystem terzi/tomlplusplus ../tools/re/prove/isola/sonda_identita.cpp -o "$O/sonda_$A" build_s22/libmissioni.a build_s22/libnucleo.a build_s22/libgrafica.a build_s22/libymfm.a build_s22/liblua54.a -lm 2>&1 | cut -c1-200 | tail -3
cp build_s22/melchior688 "$O/melchior_$A"; cp build_s22/rilievo_missioni "$O/rilievo_$A"; cp build_s22/prove "$O/prove_688"
nice -n 19 "$O/sonda_$A" "$U/identita.txt" &
( for x in "SCENE1A 1" "SCENE4A 1" "libreria/LIB_I02_PESCATORE_DI_TERIBERKA 1" "libreria/LIB_I06_PONTE_DI_EKATERINA 1"; do set -- $x; n=$(basename $1)_$2
  nice -n 19 "$O/melchior_$A" --missione $1 --seme $2 --regola simulazione --secondi 600 --stazione 4 --urto-giocatore 200,20,90 --traffico no --salva-traccia "$U/tracce/$n.m688tr" --istantanea "$U/tracce/$n.bmp" 2>&1 | grep -E "impronta|ESITO" | sed "s|$U/tracce/||" > "$U/tracce/$n.txt"; done ) &
( CARTELLA=$U/casi nice -n 19 /home/randolph/Claude/melchior-688/tools/regressione/casi.sh "$O/melchior_$A" > "$U/casi.txt" 2>&1 ) &
wait
md5sum "$U/identita.txt" | cut -c1-16; grep -c "riparato:" "$U/identita.txt"; grep -c "^ m .* 274$" "$U/identita.txt"; cat "$U/tracce/"*.txt | cut -c1-100; find . ../data/motore -path './build*' -prune -o -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.toml' -o -name '*.tsv' \) -print0 | sort -z | xargs -0 md5sum > "$U/sorgenti.md5"
echo fine $A
