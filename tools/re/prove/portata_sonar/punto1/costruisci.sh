#!/bin/bash
# Build di build_portata e copia congelata dei binari. Uso: costruisci.sh CARTELLA [pulita]
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; D=$B/$1
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
cd "$B" || exit 1
[ "$2" = pulita ] && nice -n 10 ninja -j6 -t clean > /dev/null
nice -n 10 ninja -j6 melchior688 prove rilievo_missioni serie_sandbox scene_interazioni 2>&1 | tail -4 || exit 1
bash "$R/tools/re/prove/portata_sonar/punto1/compila.sh" || exit 1
mkdir -p "$D"
for f in melchior688 serie_sandbox rilievo_missioni scene_interazioni sonda_portata sonda_tracce; do cp -p "$B/$f" "$D/$f"; done
md5sum "$D"/* > "$D/md5.txt"
echo fine "$(date +%T)"
