#!/bin/bash
# La build per il limite delle fotografie, a nice 19: melchior688 e prove in build_difese. uso: costruisci_foto.sh
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_difese
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
mkdir -p "$TMPDIR"
cd "$R/engine" || exit 1
SYS=$R/tools/deps/sysroot/usr; LIB=$SYS/lib/x86_64-linux-gnu
nice -n 19 cmake -S . -B build_difese -G Ninja -DCMAKE_PREFIX_PATH=$SYS -DCMAKE_BUILD_RPATH=$LIB -DCMAKE_EXE_LINKER_FLAGS="-Wl,--disable-new-dtags" > "$B/cmake.log" 2>&1 || { tail -5 "$B/cmake.log"; exit 1; }
cd "$B" || exit 1
nice -n 19 ninja -j 6 "$@" 2>&1 | tail -3
date
