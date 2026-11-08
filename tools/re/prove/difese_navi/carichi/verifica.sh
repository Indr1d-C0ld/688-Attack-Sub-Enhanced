#!/bin/bash
# La verifica dei carichi: casi, rilievo, manuale, e le impronte con il carico.toml di prima e di adesso sullo stesso binario, a nice 19. uso: verifica.sh [impronte]
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_difese; F=$B/foto/d; P=$B/tmp/carichi/verifica
mkdir -p "$P/casi" "$P/rilievo" "$P/tmp" || exit 1
export TMPDIR=$P/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
if [ "$1" != impronte ]; then
CARTELLA=$P/casi nice -n 19 bash "$R/tools/regressione/casi.sh" "$F/melchior688" > "$P/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$P/casi.txt" > "$P/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$P/casi.diff") righe)"
CARTELLA=$P/rilievo nice -n 19 bash "$R/tools/regressione/rilievo_classico.sh" "$F/rilievo_missioni" 2>&1 | tail -3
fi
# stesso binario, due data: quello vero e una copia di link dell'intera data/ col carico.toml di prima (senza data/suoni accanto
# il modello acustico cambia in silenzio)
V=$P/data_prima; rm -rf "$V"; mkdir -p "$V/motore"
for x in "$R"/data/*; do [ "$(basename "$x")" = motore ] || ln -s "$x" "$V/$(basename "$x")"; done
for x in "$R"/data/motore/*; do ln -s "$x" "$V/motore/$(basename "$x")"; done
rm "$V/motore/carico.toml"; cp "$B/carico_prima.toml" "$V/motore/carico.toml"
for prova in "SCENE1A 7200 1" "SCENE2B 7200 2" "libreria/LIB_S07_TAPPO 5400 1" "libreria/LIB_S12_GRUPPO_CACCIATORE 5400 3"; do
  set -- $prova
  for lato in ora prima; do
    D=$R/data/motore; [ $lato = prima ] && D=$V/motore
    nice -n 19 "$F/melchior688" --dati "$D" --missione "$1" --seme "$3" --regola simulazione --secondi "$2" --salva-traccia "$P/$(basename "$1")_$lato.m688tr" --istantanea "$P/$(basename "$1")_$lato.bmp" > "$P/$(basename "$1")_$lato.txt" 2>&1 &
  done
  wait
  a=$(grep -o "impronta [0-9a-f]*" "$P/$(basename "$1")_ora.txt" | tail -1); b=$(grep -o "impronta [0-9a-f]*" "$P/$(basename "$1")_prima.txt" | tail -1)
  cmp -s "$P/$(basename "$1")_ora.m688tr" "$P/$(basename "$1")_prima.m688tr" && t=traccia_uguale || t=TRACCIA_DIVERSA
  echo "$1 seme $3: $t | $a | $b"
done
date
