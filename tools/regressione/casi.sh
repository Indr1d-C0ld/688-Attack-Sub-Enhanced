#!/bin/bash
# Casi di regressione della principale: impronta della partita e md5 dell'istantanea (classico c1-c2, Enhanced e1-e4,
# danni p1). Uso: casi.sh [binario] > uscita.txt, poi diff col riferimento (riferimento.txt). Le istantanee e i file di
# lavoro vanno in $CARTELLA (predefinita: una cartella temporanea). Ricostruito il 26/10/2026 dopo il riavvio che ha
# svuotato /tmp: lo script ora sta nel progetto.
B=${1:-/home/randolph/Claude/melchior-688/engine/build/melchior688}
D=${CARTELLA:-$(mktemp -d)}
export LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
cd /home/randolph/Claude/melchior-688/engine || exit 1
O=$D/ord_l.txt
printf '1 carica_tubo 0 0 1\n40 lancia 0 0 1\n' > $O
caso() { n=$1; shift; "$B" "$@" --istantanea $D/$n.bmp 2>&1 | grep -E "impronta|ESITO" | sed "s|$D/||"; md5sum $D/$n.bmp | cut -c1-12; }
caso c1 --missione SCENE1A --secondi 600 --stazione 4
caso c2 --missione SCENE2A --secondi 900 --stazione 7 --ordini $O
caso e1 --missione SCENE1A --regola simulazione --secondi 600 --stazione 8 --traffico no
caso e2 --missione SCENE4A --regola simulazione --secondi 900 --stazione 7 --ordini $O --traffico no
caso e3 --missione SCENE1A --regola simulazione --secondi 30 --stazione 3 --urto-giocatore 200,20,90 --traffico no
caso e4 --missione SCENE1B --regola simulazione --secondi 500 --stazione 4 --periscopio-su 2 --traffico no
# il traffico civile (docs/20 §3): Gibilterra, il sonar
caso e5 --missione SCENE5A --regola simulazione --secondi 900 --stazione 8 --traffico si
caso p1 --colpo siluro_chiglia --secondi 20
# il manuale: le tabelle generate dai dati (tasti, classi, siluri...) devono essere aggiornate (tools/pipeline/manuale.py)
/home/randolph/Claude/melchior-688/tools/venv/bin/python -I /home/randolph/Claude/melchior-688/tools/pipeline/manuale.py --controlla >/dev/null 2>&1
echo "manuale: tabelle aggiornate se 0 -> $?"
