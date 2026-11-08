#!/bin/bash
# Le schede delle 31 basi del Museo, dal gioco: fotografa.sh BINARIO USCITA
B=$1; U=$2; mkdir -p "$U"
export LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
/home/randolph/Claude/melchior-688/tools/venv/bin/python3 -I -c "
import tomllib
for k,x in tomllib.load(open('/home/randolph/Claude/melchior-688/data/motore/enciclopedia.toml','rb'))['voci'].items():
    if x.get('categoria')=='base': print(k)" | xargs -P 4 -I{} sh -c "nice -n 19 '$B' --schermata museo --voce {} --istantanea '$U/{}.bmp' > /dev/null 2>&1"
