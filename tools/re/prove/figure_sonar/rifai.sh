#!/bin/bash
# Le sei figure del sonar del manuale (1_uso/03_sonar.md), rifatte col binario della cascata coi raggi (kit, 07/11/2026).
# Scrive solo in nuove/: le figure si copiano a mano, dopo averle guardate. Col traffico acceso, seme 1 (predefinito).
# I tasti: 9 fa avanzare il cursore (5 volte righe, 6 volte storia), 4 apre la pagina dell'analizzatore.
# Uso: rifai.sh <melchior688>
B=${1:?binario}
R=/home/randolph/Claude/melchior-688
D=$R/tools/re/prove/figure_sonar/nuove
mkdir -p "$D"
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
cd "$R/engine" || exit 1
tasti() { f=$D/$1.tasti; shift; : > "$f"; for r in "$@"; do echo "$r" >> "$f"; done; echo "$f"; }
foto() { n=$1; shift; nice -n 19 "$B" "$@" --regola simulazione --stazione 8 --istantanea "$D/$n.bmp" > "$D/$n.log" 2>&1; grep -E "impronta|ESITO" "$D/$n.log" | sed "s|$D/||"; }
foto sonar_688 --missione SCENE1A --secondi 200 --sonar moderno
foto sonar_688_1989 --missione SCENE1A --secondi 200 --sonar 1989
# l'analizzatore e il DEMON: la Perry della scena di prova, col bersaglio designato e il fascio sul tracker (6). Nella SCENE1A al
# fascio 000 non c'e' nessuno: «nessuna ipotesi», e il «FOXTROT» della figura del 29/10 era un caso del traffico (vedi rapporto).
K=$R/tools/re/prove/suite_acustica/sonda_figure/kit_vol1_c3
foto sonar_analizzatore --cartella-missioni "$K/missioni" --missione KIT_DEMON --secondi 250 --sonar moderno --ordini "$K/bersaglio.ord" --tasti-a-tempo "$(tasti analizzatore '100 pulsante_6' '248 pulsante_4')"
# la scritta con un nome: il fascio sul tracker dei tre mercantili della SCENE2A (la seconda schermata di sonar_analizzatore, portata 07/11)
foto sonar_analizzatore_nome --missione SCENE2A --secondi 250 --sonar moderno --tasti-a-tempo "$(tasti analizzatore_nome '100 pulsante_6' '248 pulsante_4')"
foto sonar_demon --cartella-missioni "$K/missioni" --missione KIT_DEMON --secondi 250 --sonar moderno --ordini "$K/bersaglio.ord" --tasti-a-tempo "$(tasti demon '100 pulsante_6' '232 pulsante_9' '234 pulsante_9' '236 pulsante_9' '238 pulsante_9')"
foto sonar_righe --missione SCENE2A --secondi 250 --sonar moderno --tasti-a-tempo "$(tasti righe '232 pulsante_9' '234 pulsante_9' '236 pulsante_9' '238 pulsante_9' '240 pulsante_9')"
foto sonar_storia --missione SCENE2A --secondi 250 --sonar moderno --tasti-a-tempo "$(tasti storia '230 pulsante_9' '232 pulsante_9' '234 pulsante_9' '236 pulsante_9' '238 pulsante_9' '240 pulsante_9')"
foto sonar_alfa --missione SCENE2B --secondi 299 --sonar moderno
"$R/tools/venv/bin/python3" -I - "$D" <<'P'
import glob, os, sys
from PIL import Image
for f in glob.glob(sys.argv[1] + "/*.bmp"):
    Image.open(f).save(f[:-4] + ".png")
    os.remove(f)
P
echo fine
