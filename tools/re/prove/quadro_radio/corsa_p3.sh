#!/bin/bash
# La serie di P3 contro la base di base2 (proposta.md §9.4): le 18 partite con le righe QUADRO, le scene sui semi in cui
# nella base un battello riferisce piu' un seme di controllo per variante, poi M3 e S22 a 12 semi. A nice 19, 6 alla volta.
R=/home/randolph/Claude/melchior-688; Z=$R/engine/build_radio/fisso_p3; S=$R/engine/build_radio/sonda_p3/melchior688_sonda; U=$R/engine/build_radio/p3; A=$R/engine/build_radio/base2
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cp "$S" "$Z/melchior688_sonda"; S=$Z/melchior688_sonda
cd "$R/engine" || exit 1
md5sum "$S" "$Z/melchior688" "$Z/serie_sandbox" > "$U/md5.txt"; md5sum "$R/data/motore/acustica.toml" >> "$U/md5.txt"
gioca() { n=$1; shift; t0=$(date +%s); nice -n 19 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "$n rc $? $(( $(date +%s) - t0 )) s" >> "$U/fatti.txt"; rm -f "$U/$n.bmp"; }
export -f gioca; export S U
xargs -P ${P18:-6} -L 1 bash -c 'gioca "$@"' _ < "$A/lavori_18.txt"
date > "$U/fine_18.txt"
grep -E '^(m4a_(1|3)|m4b_(1|3)|m4c_(1|8)|s5_(1|4|9)|s5spia_(1|4|9)) ' "$A/lavori_scene.txt" > "$U/lavori_scene.txt"
xargs -P 6 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori_scene.txt"
date > "$U/fine_scene.txt"
nice -n 19 "$Z/serie_sandbox" --semi 12 --da 1 --lavori 6 --motore "$Z/melchior688" --uscita "$U/m3" --codice 8CGVJJ-KR0000-04K000-0000HZ-G001WN > "$U/m3.log" 2>&1
date > "$U/fine_m3.txt"
cod=$(grep '^S22 ' "$A/codici.txt" | sed 's/.*codice //')
nice -n 19 "$Z/serie_sandbox" --semi 12 --da 1 --lavori 6 --motore "$Z/melchior688" --uscita "$U/s22" --codice "$cod" > "$U/s22.log" 2>&1
date > "$U/fine.txt"
