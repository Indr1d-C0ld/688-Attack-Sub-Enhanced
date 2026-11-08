#!/bin/bash
# oggi.sh <prima|dopo> <melchior688> <sonda_identita>: la prova d'identita' sui sorgenti comuni di oggi (engine/build_s22), senza copie nuove: le stesse partite di tracce.sh e casi.sh
O=/home/randolph/Claude/melchior-688/engine/build_danni/oggi; A=$1; M=$2; S=$3; U=$O/$A; rm -rf "$U"; mkdir -p "$U/casi" "$U/tracce"
export TMPDIR=/home/randolph/Claude/melchior-688/engine/build_danni/tmp LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd /home/randolph/Claude/melchior-688/engine || exit 1
nice -n 19 "$S" "$U/identita.txt" &
( for x in "SCENE1A 1" "SCENE4A 1" "libreria/LIB_I02_PESCATORE_DI_TERIBERKA 1" "libreria/LIB_I02_PESCATORE_DI_TERIBERKA 2" "libreria/LIB_I06_PONTE_DI_EKATERINA 1"; do
  set -- $x; n=$(basename $1)_$2
  nice -n 19 "$M" --missione $1 --seme $2 --regola simulazione --secondi 600 --stazione 4 --urto-giocatore 200,20,90 --traffico no --salva-traccia "$U/tracce/$n.m688tr" --istantanea "$U/tracce/$n.bmp" 2>&1 | grep -E "impronta|ESITO" | sed "s|$U/tracce/||" > "$U/tracce/$n.txt"
done ) &
( CARTELLA=$U/casi nice -n 19 /home/randolph/Claude/melchior-688/tools/regressione/casi.sh "$M" > "$U/casi.txt" 2>&1; rm -f $U/casi/*.bmp ) &
wait
md5sum "$U/identita.txt" | cut -c1-32; wc -c < "$U/identita.txt"; ls "$U/tracce" | wc -l; cat "$U/tracce/"*.txt | cut -c1-100 | head -6; echo fine $A
