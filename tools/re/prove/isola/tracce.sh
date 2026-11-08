#!/bin/bash
# tracce.sh <prima|lavoro> <uscita>: le stesse partite senza i comandi nuovi, 688 e Alfa, con colpi di prova sul giocatore e sui sottomarini: istantanea e traccia del replay.
# Poi cmp fra le due cartelle: devono essere identiche (l'istantanea di F3 no: ha la riga dei pulsanti, per questo qui si fotografa la stazione 4)
B=/home/randolph/Claude/melchior-688/engine/build_danni
A=$1; U=$2; M=$B/b_$A/melchior688
mkdir -p "$U"; export TMPDIR=$B/tmp LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd $B/corse || exit 1
for x in "SCENE1A 1" "SCENE1A 2" "SCENE4A 1" "SCENE4A 2" "LIB_I02_PESCATORE_DI_TERIBERKA 1" "LIB_I02_PESCATORE_DI_TERIBERKA 2" "LIB_I06_PONTE_DI_EKATERINA 1"; do
  set -- $x; n=$1_$2
  nice -n 19 "$M" --missione $1 --seme $2 --regola simulazione --secondi 600 --stazione 4 --urto-giocatore 200,20,90 --traffico no --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" 2>&1 | grep -E "impronta|ESITO" | sed "s|$U/||" > "$U/$n.txt"
done
echo fatto $A
