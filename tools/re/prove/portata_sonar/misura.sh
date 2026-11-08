#!/bin/bash
# le corse della portata: sonda_portata corsa, poche alla volta e con nice. Non tocca data/. Uscita in esiti/corse/
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; U=$R/tools/re/prove/portata_sonar/esiti/corse
mkdir -p "$U"; export TMPDIR=$B/tmp
OTT="15 10 1989 9"
una() {   # data(giu|ott) modo km seme secondi
    local f=$U/$1_$2_k$3_s$4.tsv
    [ -s "$f" ] && grep -q "^# riassunto" "$f" && return
    if [ "$1" = ott ]; then PORTATA_DATA="$OTT" nice -n 15 "$B/sonda_portata" corsa "$3" 90 "$2" "$4" "$5" > "$f" 2>"$f.err"
    else nice -n 15 "$B/sonda_portata" corsa "$3" 90 "$2" "$4" "$5" > "$f" 2>"$f.err"; fi
}
export -f una; export U B OTT
{
    for data in ott giu; do for modo in ia g; do for km in 40 60 80 95 100 110 130; do for seme in 1 2; do echo "$data $modo $km $seme 3000"; done; done; done; done
    for seme in 1 2; do echo "ott ia_quota 100 $seme 3000"; echo "giu ia_quota 95 $seme 3000"; done
    for km in 40 95; do for seme in 1 2; do echo "ott ia $km $seme 7200"; done; done
} | sed 's/^ott ia \(40\|95\) \([12]\) 7200$/ott ia \1 \2 7200/' | awk '!(($1=="ott") && ($2=="ia") && ($3==40||$3==95) && ($5==3000))' | xargs -P 5 -L 1 bash -c 'una "$0" "$1" "$2" "$3" "$4"'
echo fine
