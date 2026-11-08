#!/bin/bash
# melchior688_mis (oggi e, con LOFAR_RAGGI, il LOFAR ai raggi) e melchior688_mis_s3 (acustica_m.o, navi_civili_m.o: senza S3)
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; M=$B/tmp/matrice
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 "$R/tools/re/prove/portata_sonar/punto1/analizzatore/spazza_gioco.py" "$C" || exit 1
cd "$B" || exit 1
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_mis#g' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-240
[ -f "$C/sonar_enhanced_mis.o" ] || exit 1
L=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
L1=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_mis analizzatore/sonar_enhanced_mis.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$L")
nice -n 10 bash -c "$L1" || exit 1
L2=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_mis_s3 analizzatore/sonar_enhanced_mis.o analizzatore/analisi_v.o tmp/matrice/acustica_m.o analizzatore/navi_civili_m.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$L")
nice -n 10 bash -c "$L2" || exit 1
ls -la "$C/melchior688_mis" "$C/melchior688_mis_s3"
