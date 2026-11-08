#!/bin/bash
# Gioco e prove, poi le copie in foto/d e l'analisi delle voci, a nice 19. uso: costruisci.sh
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_difese; Q=$R/tools/re/prove/difese_navi/carichi
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$B" || exit 1
nice -n 19 ninja -j 6 melchior688 prove rilievo_missioni serie_sandbox > carichi_build.log 2>&1 || { grep -n "error" carichi_build.log | cut -c1-240 | head; exit 1; }
mkdir -p foto/d tmp/carichi
cp melchior688 rilievo_missioni serie_sandbox foto/d/ && cp prove foto/d/prove_688 || exit 1
I="-DTOML_ASSERT=(void)sizeof -DM688_DATI=\"$R/data/motore\" -I$E/missioni/include -I$E/nucleo/include -isystem $E/terzi/tomlplusplus -isystem $E/terzi/ymfm -isystem $E/terzi/lua -O1 -std=c++20"
nice -n 19 /usr/bin/c++ $I "$Q/conta_voci.cpp" -o tmp/carichi/conta_voci libmissioni.a libnucleo.a libymfm.a liblua54.a -lm || exit 1
echo fatto; date
