#!/bin/bash
# Le scene delle figure del sonar col binario di adesso, togliendo una cosa alla volta (07/11/2026, solo lettura: nessuna
# figura e nessun sorgente toccati). melchior688_m e' build_trailer1 linkato con matrice/acustica_m.o: V_SENZA_SPETTRO toglie
# i -17 dB dello spettro, V_G100 riporta la griglia a 100 km; d_prima sono i dati prima delle firme civili (S3 e B).
R=/home/randolph/Claude/melchior-688
E=$R/engine
M=$E/build_portata/tmp/matrice
D=$R/tools/re/prove/portata_sonar/punto1/esiti/cascata
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$E" || exit 1
foto() {   # nome missione secondi traffico [dati] [env...]
    local n=$1 mi=$2 s=$3 tr=$4 da=${5:-$R/data/motore}; shift 5
    env "$@" nice -n 19 "$M/melchior688_m" --missione "$mi" --secondi "$s" --sonar moderno --regola simulazione --traffico "$tr" --dati "$da" --stazione 8 --istantanea "$D/$n.bmp" 2>&1 | grep -E "impronta" | sed "s|$D/||"
}
P=$M/d_prima/motore
{
    foto ctl_s2b_no SCENE2B 299 no "" A=1 &
    foto no17_s2b_no SCENE2B 299 no "$P" V_SENZA_SPETTRO=1 &
    foto no17_s2b_si SCENE2B 299 si "$P" V_SENZA_SPETTRO=1 &
    foto no17_s1a_no SCENE1A 200 no "$P" V_SENZA_SPETTRO=1 &
    wait
    foto no17_s1a_si SCENE1A 200 si "$P" V_SENZA_SPETTRO=1 &
    foto no17_s2a_no SCENE2A 250 no "$P" V_SENZA_SPETTRO=1 &
    foto no17_s2a_si SCENE2A 250 si "$P" V_SENZA_SPETTRO=1 &
    foto g100_s2b_no SCENE2B 299 no "" V_G100=1 &
    wait
} > "$D/varianti.txt" 2>&1
python3 - "$D" <<'E'
import glob, sys
from PIL import Image
for f in glob.glob(sys.argv[1] + "/*.bmp"):
    Image.open(f).save(f[:-4] + ".png")
E
echo fine
