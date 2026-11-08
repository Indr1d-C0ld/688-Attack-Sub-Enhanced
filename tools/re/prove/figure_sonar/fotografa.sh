#!/bin/bash
# Le scene delle figure del sonar del manuale (1_uso/03_sonar.md), col gioco dei sorgenti di adesso, per confrontarle con
# le figure e dire quali rifare (kit, 06/11/2026). Scrive solo qui: le figure del manuale non si toccano.
# Ogni scena anche senza traffico: quello che sparisce e' il traffico civile.
# Uso: fotografa.sh <melchior688>
B=${1:?binario}
R=/home/randolph/Claude/melchior-688
D=$R/tools/re/prove/figure_sonar/dopo
mkdir -p "$D"
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
cd "$R/engine" || exit 1
foto() { n=$1; shift; nice -n 19 "$B" "$@" --regola simulazione --stazione 8 --istantanea "$D/$n.bmp" 2>&1 | grep -E "impronta|ESITO" | sed "s|$D/||"; }
# sonar_688, sonar_688_1989: SCENE1A a 200 s (10:03:20Z)
foto s1a_moderno --missione SCENE1A --secondi 200 --sonar moderno
foto s1a_1989 --missione SCENE1A --secondi 200 --sonar 1989
foto s1a_moderno_senza --missione SCENE1A --secondi 200 --sonar moderno --traffico no
# sonar_righe e sonar_storia: SCENE2A a 250 s (06:04:10Z); sonar_alfa: SCENE2B a 299 s, sull'Okean
foto s2a_250 --missione SCENE2A --secondi 250 --sonar moderno
foto s2a_250_senza --missione SCENE2A --secondi 250 --sonar moderno --traffico no
foto s2b_299 --missione SCENE2B --secondi 299 --sonar moderno
foto s2b_299_senza --missione SCENE2B --secondi 299 --sonar moderno --traffico no
# istanti con un contatto in vista, per quando si rifanno (a 200 e 250 s la cascata e' quasi vuota)
foto s1a_600 --missione SCENE1A --secondi 600 --sonar moderno
foto s2a_900 --missione SCENE2A --secondi 900 --sonar moderno
