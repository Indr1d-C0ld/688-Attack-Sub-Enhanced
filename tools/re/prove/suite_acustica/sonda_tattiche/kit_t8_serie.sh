#!/bin/bash
# La serie di T8 in partita vera (kit, 04/11/2026), da lanciare dalla radice quando la macchina e' libera, dopo aver ricompilato
# kit_t8_partita con le librerie aggiornate (comando in testa al .cpp). La distanza di lancio non si sceglie: la decide la nave col
# suo contatto, quindi si confronta per distanza misurata. Uso: kit_t8_serie.sh [semi=3] [seme0=1]
D=tools/re/prove/suite_acustica/sonda_tattiche
SEMI=${1:-3}; S0=${2:-1}
for nodi in 5 20; do
  for reazione in subito allarme; do
    nice -n 19 $D/kit_t8_partita "$SEMI" --seme0 "$S0" --distanze 4,8 --rilevamenti 0,90,180 --manovre niente,A,B,C,C+esca,A+soffitto \
      --nodi "$nodi" --reazione "$reazione" --secondi 2000 > "$D/kit_t8_partita_n${nodi}_${reazione}_s${S0}.tsv" &
  done
done
wait
