#!/bin/bash
# rifai_lavoro.sh: la copia di lavoro riparte dalla copia intatta, con la pagina del kit e applica.py sopra
B=/home/randolph/Claude/melchior-688/engine/build_danni
S=/home/randolph/Claude/melchior-688/engine
rsync -a --delete $B/albero_prima/engine/ $B/albero_lavoro/engine/ || exit 1
cp $S/grafica/include/grafica/pagina_squadre.hpp $B/albero_lavoro/engine/grafica/include/grafica/
cp $S/grafica/src/pagina_squadre.cpp $B/albero_lavoro/engine/grafica/src/
sed -i 's|^  grafica/src/spaccato.cpp$|&\n  grafica/src/pagina_squadre.cpp|' $B/albero_lavoro/engine/CMakeLists.txt
python3 -I /home/randolph/Claude/melchior-688/tools/re/prove/isola/applica.py $B/albero_lavoro/engine
