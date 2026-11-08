#!/bin/bash
# Prima e dopo sulle stesse scene della stessa schermata (stazione 8). Prima: melchior688_prima (il binario di build_trailer1,
# sorgenti d'ora) per le immagini; CASCATA_A sul binario v solo per il conto dei tracker. Nice 19, al massimo 6 processi.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata/cascata; O=$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$R/engine/build_portata/tmp
mkdir -p "$O"; cd "$R/engine" || exit 1
foto() {  # nome missione secondi traffico binario [env...]
    local n=$1 mi=$2 s=$3 tr=$4 bin=$5; shift 5
    rm -f "$O/$n.log"
    env "$@" nice -n 19 "$bin" --missione "$mi" --secondi "$s" --sonar moderno --regola simulazione --traffico "$tr" --stazione 8 --istantanea "$O/$n.bmp" 2>&1 | grep -E "impronta" | sed "s|$O/||" >> "$O/impronte.txt"
}
giro() {  # nome missione secondi traffico
    foto "$1_prima"  "$2" "$3" "$4" "$B/melchior688_prima" A=1 &
    foto "$1_dopo"   "$2" "$3" "$4" "$B/melchior688_v" CASCATA_LOG="$O/$1_dopo.log" &
    foto "$1_vecchia" "$2" "$3" "$4" "$B/melchior688_v" CASCATA_A=1 CASCATA_LOG="$O/$1_vecchia.log" &
    if [ "$4" = si ]; then foto "$1_oltre" "$2" "$3" "$4" "$B/melchior688_v" CASCATA_OLTRE=1 CASCATA_LOG="$O/$1_oltre.log" & fi
    wait
}
rm -f "$O/impronte.txt"
giro s2b_si SCENE2B 299 si; giro s2b_no SCENE2B 299 no
giro s2a_si SCENE2A 250 si; giro s2a_no SCENE2A 250 no
giro s1a_si SCENE1A 200 si; giro s1a_no SCENE1A 200 no
giro s5a_si SCENE5A 3000 si
giro m4_si prove/PROVA_M4_QUADRO 6000 si
python3 - "$O" <<'P'
import glob, sys
from PIL import Image
for f in glob.glob(sys.argv[1] + "/*.bmp"):
    Image.open(f).save(f[:-4] + ".png")
P
rm -f "$O"/*.bmp
echo fine
