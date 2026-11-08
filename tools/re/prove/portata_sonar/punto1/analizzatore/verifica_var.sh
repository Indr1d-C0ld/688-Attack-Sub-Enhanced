#!/bin/bash
# Variante del POSSUB: sonda m4, guasti, casi col riferimento, impronte e immagini prima/dopo con tasto 4. Niente suite ne' rilievo (le fa la principale, da build pulita).
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; A=$R/tools/re/prove/portata_sonar/punto1/analizzatore; O=$A/esiti/variante; K=$R/tools/re/prove/suite_acustica/sonda_figure/kit_vol1_c3; S=$A/esiti/schermo
mkdir -p "$O/casi" "$S/variante"; export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
bash "$A/compila_m4.sh" > "$O/m4.txt" 2>&1
bash "$A/guasti_variante.sh" > "$O/guasti.txt" 2>&1
cd "$E" || exit 1
cp "$B/melchior688" "$C/m688_var"
( CARTELLA=$O/casi bash "$R/tools/regressione/casi.sh" "$C/m688_var" > "$O/casi.txt" 2>&1; diff "$R/tools/regressione/riferimento.txt" "$O/casi.txt" > "$O/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$O/casi.diff") righe)" > "$O/casi.esito"; rm -rf "$O/casi" ) &
foto() { # nome missione secondi tasti [extra]
    n=$1; mi=$2; se=$3; ta=$4; shift 4
    for pair in "prima $C/m688_prima" "dopo $C/m688_var"; do set -- $pair "$@"; v=$1; b=$2; shift 2
        nice -n 10 "$b" "$@" --missione "$mi" --secondi "$se" --sonar moderno --regola simulazione --stazione 8 --tasti-a-tempo "$S/$ta" --istantanea "$S/variante/${v}_$n.bmp" 2>&1 | grep -o "impronta [0-9a-f]*, contatti [0-9a-f]*" | sed "s/^/$v $n /" >> "$O/impronte.txt"
    done
}
: > "$O/impronte.txt"
foto alfa SCENE2A 250 s2a2.tasti
foto tre_merc SCENE2A 250 s2a1.tasti
foto spruance SCENE1A 402 s1a.tasti
foto perry KIT_DEMON 250 kit.tasti --cartella-missioni "$K/missioni" --ordini "$K/bersaglio.ord"
wait
tail -1 "$O/m4.txt" | cut -c1-200; grep -v "^TEST CASE" "$O/guasti.txt" | cut -c1-120; cat "$O/casi.esito" "$O/impronte.txt"
