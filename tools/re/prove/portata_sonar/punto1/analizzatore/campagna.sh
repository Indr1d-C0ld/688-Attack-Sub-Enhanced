#!/bin/bash
# L'analizzatore su tutte le unita' entro 40 km, in piu' scene e semi, e una classe alla volta. Oggi, senza S3, col LOFAR ai raggi. Nice 15, -P 5.
R=/home/randolph/Claude/melchior-688; E=$R/engine; C=$E/build_portata/analizzatore; O=$R/tools/re/prove/portata_sonar/punto1/analizzatore/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
rm -rf "$O/oggi" "$O/senza_s3" "$O/raggi"; mkdir -p "$O/oggi" "$O/senza_s3" "$O/raggi" "$C/lua"; cd "$E" || exit 1
P=$E/build_portata/tmp/matrice/d_prima/motore
python3 - "$R" > "$O/classi.txt" <<'E2'
import sys, tomllib
d = tomllib.load(open(sys.argv[1] + "/data/motore/acustica.toml", "rb"))
for k, v in d["firme"].items():
    if v.get("righe") and v.get("tipo_unita") not in ("siluro", "esca", "biologico", "generica"): print(k)
E2
lavori() {
    for s in 1 2 3 4 5; do
        echo "g SCENE1A 200,400,800 si $s"
        echo "g SCENE1A 200,400,800 no $s"
        echo "g SCENE2A 250,600 si $s"
        echo "g SCENE2A 250,600 no $s"
        echo "g SCENE2B 299,600 si $s"
        echo "g SCENE2B 299,600 no $s"
        echo "g SCENE4A 900 si $s"
        echo "g SCENE5A 900,3000 si $s"
        echo "g prove/PROVA_M3_ORIZZONTE 2400 si $s"
        echo "g prove/PROVA_S5_CONVOGLIO 3600 si $s"
        echo "g libreria/LIB_S22_ROTTA_LUNGA 3000 si $s"
    done
    for s in 1 2 3; do
        while read -r k; do
            echo "spazzata $k los_angeles $s $C/lua"
            echo "spazzata $k alfa $s $C/lua"
        done < "$O/classi.txt"
    done
}
# le scene dentro il gioco vero (melchior688_mis): la partita resta quella del gioco; la spazzata con la sonda (un bersaglio solo)
gioco() {  # cartella binario nome missione tempi traffico seme [env...]
    local o=$1 b=$2 n=$3 mi=$4 te=$5 tr=$6 se=$7; shift 7
    local fine=${te##*,}; fine=$((fine + 2))
    env "$@" ANALIZZA_SPAZZA="$te" ANALIZZA_USCITA="$O/$o/$n.tsv" ANALIZZA_SCENA="$mi" ANALIZZA_SEME="$se" ANALIZZA_TRAFFICO="$tr" nice -n 15 "$C/$b" --missione "$mi" --secondi "$fine" --seme "$se" --sonar moderno --regola simulazione --traffico "$tr" --stazione 8 --istantanea "$C/foto_$o_$n.bmp" $D_ARG > /dev/null 2> "$O/$o/$n.err"
    rm -f "$C/foto_$o_$n.bmp"
}
una() {
    local n; n=$(echo "$*" | tr ' /,' '___' | sed "s#_home.*##")
    if [ "$1" = g ]; then
        D_ARG="" gioco oggi melchior688_mis "$n" "$2" "$3" "$4" "$5" A=1
        D_ARG="" gioco raggi melchior688_mis "$n" "$2" "$3" "$4" "$5" LOFAR_RAGGI=1
        D_ARG="--dati $P" gioco senza_s3 melchior688_mis_s3 "$n" "$2" "$3" "$4" "$5" V_SENZA_SPETTRO=1
        return
    fi
    nice -n 15 "$C/sonda" "$@" > "$O/oggi/$n.tsv" 2> "$O/oggi/$n.err"
    LOFAR_RAGGI=1 nice -n 15 "$C/sonda_v" "$@" > "$O/raggi/$n.tsv" 2> "$O/raggi/$n.err"
    PORTATA_DATI=$P V_SENZA_SPETTRO=1 nice -n 15 "$C/sonda_senza_s3" "$@" > "$O/senza_s3/$n.tsv" 2> "$O/senza_s3/$n.err"
}
export -f una gioco; export C O P
"$C/sonda" intestazione > "$O/intestazione.tsv"
lavori | xargs -P 5 -L 1 bash -c 'una "$@"' _
find "$O/oggi" "$O/senza_s3" "$O/raggi" -name '*.err' -empty -delete
echo "file: $(ls "$O/oggi" | wc -l) e $(ls "$O/senza_s3" | wc -l); errori: $(ls "$O"/oggi/*.err "$O"/senza_s3/*.err "$O"/raggi/*.err 2>/dev/null | wc -l)"
