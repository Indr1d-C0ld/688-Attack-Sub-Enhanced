#!/bin/bash
# Variante B: prove vicine, casi, rilievo, M4, S5, M1 col binario p1_s3 (il codice non cambia: cambiano solo i dati).
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; P=$R/tools/re/prove/portata_sonar/punto1; S3=$B/p1_s3; U=$P/esiti/b; D=$P/../data_scene
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
mkdir -p "$U/serie" "$U/casi" "$U/rilievo"
cd "$R/engine" || exit 1
F='acustica*,analisi*,navi civili*,traffico*,sonar*,sonarista*,*mercantil*,*civil*,*cascata*,*lofar*,*tonal*,*convoglio*,*portata*,bordo*,variazioni*,quadro ai sottomarini*,asw missione*'
nice -n 10 "$B/prove" -tc="$F" > "$U/prove_vicine.txt" 2>&1
tail -3 "$U/prove_vicine.txt"
CARTELLA=$U/casi nice -n 15 bash "$R/tools/regressione/casi.sh" "$S3/melchior688" > "$U/casi.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$U/casi.txt" > "$U/casi.diff"; echo "casi: diff rc $? ($(wc -l < "$U/casi.diff") righe)"
CARTELLA=$U/rilievo nice -n 15 bash "$R/tools/regressione/rilievo_classico.sh" "$S3/rilievo_missioni" 2>&1 | tail -2
BIN=$S3/melchior688 GIRO=$U/giro LAVORI=6 bash "$D/giro_scene.sh"
diff "$P/esiti/s3/giro/righe.txt" "$U/giro/righe.txt" > "$U/giro.diff"
echo 23 scene fatte "$(date +%T)"
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH; S=(nice -n 19 "$S3/serie_sandbox" --semi 10 --da 1 --lavori 3 --motore "$S3/melchior688")
"${S[@]}" --uscita "$U/serie/m4" --codice $M4 > "$U/serie/m4.log" 2>&1 &
"${S[@]}" --uscita "$U/serie/s5" --codice $S5 > "$U/serie/s5.log" 2>&1 &
wait
"${S[@]}" --uscita "$U/serie/m1" -- --missione prove/PROVA_M1_ANTINAVE > "$U/serie/m1.log" 2>&1
echo fine "$(date +%T)"
