#!/bin/bash
# La sonda con la copia del tiro, nella cartella di build delle difese. Il libmissioni.a resta com'e': la copia viene prima e vince. uso: costruisci.sh
R=/home/randolph/Claude/melchior-688; Q=$R/tools/re/prove/difese_navi/terza_perry; B=$R/engine/build_difese; D=$B/terza_perry; E=$R/engine
mkdir -p "$D" || exit 1
python3 "$Q/copia_tiro.py" "$E/missioni/src/missione_artiglieria.cpp" "$D/tiro_copia.cpp" "$E/missioni/src/missione_reazioni.cpp" "$D/reazioni_copia.cpp" || exit 1
I="-DTOML_ASSERT=(void)sizeof -DM688_DATI=\"$R/data/motore\" -I$E/missioni/include -I$E/nucleo/include -isystem $E/terzi/tomlplusplus -isystem $E/terzi/ymfm -isystem $E/terzi/lua -O2 -g -DNDEBUG -std=c++20"
nice -n 19 /usr/bin/c++ $I -c "$D/tiro_copia.cpp" -o "$D/tiro_copia.o" & A=$!
nice -n 19 /usr/bin/c++ $I -c "$D/reazioni_copia.cpp" -o "$D/reazioni_copia.o" & R=$!
nice -n 19 /usr/bin/c++ $I -c "$Q/sonda_perche.cpp" -o "$D/sonda_perche.o" & C=$!
nice -n 19 /usr/bin/c++ $I -c "$Q/sonda_ferma.cpp" -o "$D/sonda_ferma.o" & F=$!
wait $A && wait $C && wait $F && wait $R || exit 1
cd "$B" && /usr/bin/c++ "$D/sonda_perche.o" "$D/tiro_copia.o" "$D/reazioni_copia.o" -o "$D/sonda_perche" libmissioni.a libnucleo.a libymfm.a liblua54.a -lm && /usr/bin/c++ "$D/sonda_ferma.o" "$D/tiro_copia.o" "$D/reazioni_copia.o" -o "$D/sonda_ferma" libmissioni.a libnucleo.a libymfm.a liblua54.a -lm && echo fatto
