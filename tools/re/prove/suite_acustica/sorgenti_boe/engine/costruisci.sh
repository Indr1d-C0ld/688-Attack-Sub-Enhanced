#!/bin/bash
# Configura, compila e prova il motore senza nulla di installato nel sistema (niente sudo):
# CMake e Ninja dal venv del progetto, SDL3 3.2.10 dai pacchetti Debian estratti in tools/deps/sysroot.
#   engine/costruisci.sh            compila e lancia le prove
#   engine/costruisci.sh --pulito   ricomincia da una cartella di build vuota
set -euo pipefail
RADICE="$(cd "$(dirname "$0")/.." && pwd)"
SYSROOT="$RADICE/tools/deps/sysroot/usr"
LIBDIR="$SYSROOT/lib/x86_64-linux-gnu"
BUILD="$RADICE/engine/build"
export PATH="$RADICE/tools/venv/bin:$PATH"

if [[ "${1:-}" == "--pulito" ]]; then rm -rf "$BUILD"; fi
if [[ ! -f "$LIBDIR/libSDL3.so" ]]; then
  echo "SDL3 mancante: vedi engine/README.md (apt-get download + dpkg -x in tools/deps)" >&2; exit 1
fi
cmake -S "$RADICE/engine" -B "$BUILD" -G Ninja \
  -DCMAKE_PREFIX_PATH="$SYSROOT" \
  -DCMAKE_BUILD_RPATH="$LIBDIR" \
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,--disable-new-dtags" >/dev/null   # RPATH (non RUNPATH): vale anche per le dipendenze di libSDL3 (libsndio)
cmake --build "$BUILD"
ctest --test-dir "$BUILD" --output-on-failure
