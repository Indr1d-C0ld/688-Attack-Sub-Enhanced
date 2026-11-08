#!/bin/bash
# La verifica di B e di C su binari fissi: casi identici, rilievo del classico, prove vicine, S07 seme 1 col quadro spento, a nice 19.
# uso: verifica_colpi.sh ETICHETTA (i binari vanno in build_difese/foto/ETICHETTA, il registro in tmp/verifica_ETICHETTA)
R=/home/randolph/Claude/melchior-688; E=$1
B=$R/engine/build_difese; F=$B/foto/$E; P=$B/tmp/verifica_$E
mkdir -p "$F" "$P/casi" "$P/rilievo" "$P/tmp" || exit 1
for x in melchior688 prove rilievo_missioni serie_sandbox; do cp "$B/$x" "$F/$x" || exit 1; done
mv "$F/prove" "$F/prove_688"   # un nome proprio: un `pkill -x prove` di un altro progetto ha gia' ucciso una corsa
bash "$R/tools/re/prove/difese_navi/spento.sh" "$F/melchior688_spento" > "$P/spento.txt" 2>&1 || { tail -3 "$P/spento.txt"; exit 1; }
md5sum "$F"/* | cut -c1-12,33- > "$P/md5.txt"
export TMPDIR=$P/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
CARTELLA=$P/casi nice -n 19 bash "$R/tools/regressione/casi.sh" "$F/melchior688" > "$P/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$P/casi.txt" > "$P/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$P/casi.diff") righe)"
CARTELLA=$P/rilievo nice -n 19 bash "$R/tools/regressione/rilievo_classico.sh" "$F/rilievo_missioni" 2>&1 | tail -3
(cd "$P" && nice -n 19 "$F/prove_688" -tc='artiglieria*,statistiche*,achievement*,riconoscimenti*,carriera*,sandbox*' > vicine.txt 2>&1; echo "prove vicine: rc $?"; tail -3 vicine.txt)
"$R/tools/venv/bin/python" -I "$R/tools/re/prove/difese_navi/artiglieria_s07.py" "s07_$E" "$F/melchior688_spento" 1
A=$B/tmp/artiglieria
tail -1 "$A/fatti.txt"; tail -1 "$A/s07_$E.txt" | cut -c70-200
"$B/prima/conta_artiglieria" "$A/s07_$E.m688tr"
date
