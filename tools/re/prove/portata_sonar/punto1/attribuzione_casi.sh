#!/bin/bash
# Chi cambia i casi e2, e3, e5: griglia e chiusura senza S3 (p1_griglia2 coi dati di prima) contro S3 (p1_s3 coi dati di oggi).
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata; U=$R/tools/re/prove/portata_sonar/punto1/esiti/s3/attribuzione
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$B/tmp
V=$B/tmp/dati_prima_s3; rm -rf "$V"; mkdir -p "$V" "$U"
for f in "$R"/data/motore/*; do ln -s "$f" "$V/$(basename "$f")"; done
rm "$V/acustica.toml"; cp "$B/tmp/acustica_prima_s3.toml" "$V/acustica.toml"
cd "$R/engine" || exit 1
O=$U/ord_l.txt; printf '1 carica_tubo 0 0 1\n40 lancia 0 0 1\n' > "$O"
caso() { local et=$1 n=$2 b=$3; shift 3; nice -n 15 "$b" "$@" --istantanea "$U/${et}_$n.bmp" 2>&1 | grep -E 'impronta|ESITO' | sed "s|$U/||"; md5sum "$U/${et}_$n.bmp" | cut -c1-12; }
tutti() { local et=$1 b=$2; shift 2
  caso "$et" e2 "$b" --missione SCENE4A --regola simulazione --secondi 900 --stazione 7 --ordini "$O" --traffico no "$@"
  caso "$et" e3 "$b" --missione SCENE1A --regola simulazione --secondi 30 --stazione 3 --urto-giocatore 200,20,90 --traffico no "$@"
  caso "$et" e5 "$b" --missione SCENE5A --regola simulazione --secondi 900 --stazione 8 --traffico si "$@"
}
tutti griglia2 "$B/p1_griglia2/melchior688" --dati "$V" > "$U/griglia2.txt" 2>&1 &
tutti griglia2n "$B/p1_griglia2/melchior688" > "$U/griglia2_tonali.txt" 2>&1 &
wait
tutti s3 "$B/p1_s3/melchior688" > "$U/s3.txt" 2>&1
echo fine "$(date +%T)"
