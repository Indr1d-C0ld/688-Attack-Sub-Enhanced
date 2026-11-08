#!/bin/bash
# i conti dei tracker e dei contatti, vecchia cascata e nuova sul binario v; immagini anche per i momenti di traffico pieno
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata/cascata; O=$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$R/engine/build_portata/tmp
cd "$R/engine" || exit 1
foto() { local n=$1 mi=$2 s=$3 tr=$4; shift 4; rm -f "$O/$n.log"; env "$@" CASCATA_LOG="$O/$n.log" nice -n 19 "$B/melchior688_v" --missione "$mi" --secondi "$s" --sonar moderno --regola simulazione --traffico "$tr" --stazione 8 --istantanea "$O/$n.bmp" 2>&1 | grep impronta | sed "s|$O/||" >> "$O/impronte_conti.txt"; }
giro() { foto "$1_vecchia" "$2" "$3" "$4" CASCATA_A=1 & foto "$1_dopo" "$2" "$3" "$4" A=1 & }
rm -f "$O/impronte_conti.txt"
giro s2b_si SCENE2B 299 si; giro s2b_no SCENE2B 299 no; giro s2a_si SCENE2A 250 si; wait
giro s2a_no SCENE2A 250 no; giro s1a_si SCENE1A 200 si; giro s1a_no SCENE1A 200 no; wait
giro s5a_si SCENE5A 3000 si; giro m3_si prove/PROVA_M3_ORIZZONTE 2400 si; giro s5_si prove/PROVA_S5_CONVOGLIO 3600 si; wait
giro m4_si prove/PROVA_M4_QUADRO 6000 si; giro s7_si prove/PROVA_S7_PORTAEREI 3000 si; giro s3_si prove/PROVA_S3_BASTIONE 3000 si; wait
python3 - "$O" <<'P'
import glob, sys
from PIL import Image
for f in glob.glob(sys.argv[1] + "/*.bmp"):
    Image.open(f).save(f[:-4] + ".png")
P
rm -f "$O"/*.bmp
echo fine
