#!/bin/bash
# debriefing.png e replay.png (kit, 07/11/2026): il convoglio di S5, 1800 s, seme 1. Scrive solo qui: le figure si copiano dopo averle guardate.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_kit/melchior688; D=$R/tools/re/prove/figure_sonar/dopo_0711
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
M=prove/PROVA_S5_CONVOGLIO
nice -n 19 "$B" --sandbox --missione $M --seme 1 --secondi 1800 --salva-traccia "$D/s5.m688tr" --istantanea "$D/vivo.bmp" 2>&1 | tail -2
for T in 300 900 1500; do
  nice -n 19 "$B" --schermata replay --replay "$D/s5.m688tr" --secondi $T --istantanea "$D/replay_q_$T.bmp" 2>&1 | tail -1
  nice -n 19 "$B" --schermata replay --replay "$D/s5.m688tr" --secondi $T --pagina 2 --istantanea "$D/replay_v_$T.bmp" 2>&1 | tail -1
done
for K in 1 2 5 12 25; do
  nice -n 19 "$B" --missione $M --regola simulazione --secondi 1800 --schermata debriefing --evento $K --istantanea "$D/deb_$K.bmp" 2>&1 | tail -1
done
"$R/tools/venv/bin/python3" -I - "$D" <<'P'
import glob, os, sys
from PIL import Image
for f in glob.glob(sys.argv[1] + "/*.bmp"):
    Image.open(f).save(f[:-4] + ".png"); os.remove(f)
P
echo fine
