#!/bin/bash
# coda.sh LISTA PARALLELE CARTELLA [file.lua]: una corsa per riga «seme modo»; salta quelle che hanno gia' il file; ogni corsa nel suo sottoprocesso
cd /home/randolph/Claude/melchior-688/engine/build_s22
mkdir -p "$3"
while read -r s m; do
  [ -e "$3/${m}_$s.txt" ] && continue
  while [ "$(jobs -r | wc -l)" -ge "$2" ]; do wait -n; done
  ./sonda_esca "$s" "$m" $4 > "$3/${m}_$s.txt" 2>&1 &
done < "$1"
wait
echo fine
