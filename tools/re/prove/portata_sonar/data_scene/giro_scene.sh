#!/bin/bash
# Le 23 scene di prova col motore dato: GIRO = uscita, BIN = motore, EXTRA=1 aggiunge --data/--ora del file di ogni scena.
# Una riga per scena: nome, data usata, impronta e impronta dei contatti dell'istantanea. Non tocca data/.
# Uso: BIN=... GIRO=... [EXTRA=1] [SECONDI=900] [SEME=1] giro_scene.sh
R=/home/randolph/Claude/melchior-688
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
export TMPDIR=$R/engine/build_portata/tmp
SECONDI=${SECONDI:-900}; SEME=${SEME:-1}
mkdir -p "$GIRO" || exit 1
una() {
    local f=$1 m; m=$(basename "$f" .lua)
    local riga d o extra=""
    riga=$(grep -m1 -E '^\s*data = "' "$f")
    d=$(sed -E 's/.*data = "([0-9-]+)".*/\1/' <<< "$riga"); o=$(sed -E 's/.*ora_utc = ([0-9.]+).*/\1/' <<< "$riga")
    [ -n "$EXTRA" ] && extra="--data $d --ora $o"
    cd "$R/engine" || exit 1
    # shellcheck disable=SC2086
    nice -n 12 "$BIN" --sandbox --missione "prove/$m" --seme "$SEME" --secondi "$SECONDI" $extra --istantanea "$GIRO/$m.bmp" --salva-traccia "$GIRO/$m.m688tr" > "$GIRO/$m.txt" 2>&1
    echo "$m $d $o rc=$? $(grep -o 'impronta [0-9a-f]*, contatti [0-9a-f]*' "$GIRO/$m.txt" | tail -1)" >> "$GIRO/righe.txt"
    rm -f "$GIRO/$m.bmp"
}
export -f una; export R GIRO BIN EXTRA SECONDI SEME
rm -f "$GIRO/righe.txt"
ls "$R"/data/missioni/prove/PROVA_*.lua | xargs -P "${LAVORI:-6}" -I{} bash -c 'una {}'
sort -o "$GIRO/righe.txt" "$GIRO/righe.txt"
wc -l "$GIRO/righe.txt"
