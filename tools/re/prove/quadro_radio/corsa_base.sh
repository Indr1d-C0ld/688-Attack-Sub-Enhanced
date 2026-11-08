#!/bin/bash
# La corsa di base di proposta.md §9.4 e §9.8, sul codice di oggi: le 18 partite, poi M4 (3 varianti) e S5 (2) con le righe QUADRO. A nice 19, 6 alla volta.
# M1, M4 e S5 esiti e lanci vengono dalla portata (punto1/esiti/b/serie); M3 e S22 con serie_sandbox, senza righe
R=/home/randolph/Claude/melchior-688; Z=$R/engine/build_radio/fisso3; S=$Z/melchior688_sonda; U=${U:-$R/engine/build_radio/base}; E=$R/data/motore/esperimenti
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
md5sum "$S" "$Z/melchior688" "$Z/serie_sandbox" > "$U/md5.txt"; md5sum "$R/data/motore/acustica.toml" >> "$U/md5.txt"
M4=PROVA_M4_QUADRO; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH
{
for m in I02 I05 S08 S11; do f=$(cd "$R/data/missioni/libreria" && ls LIB_${m}_*.lua); n=${f%.lua}; d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$f" | head -1 | cut -d' ' -f3)
    for s in 1 2; do echo "${m}_$s --missione libreria/$n --seme $s --secondi $d"; done; done
for m in I06 I07 S03 S07 S15; do f=$(cd "$R/data/missioni/libreria" && ls LIB_${m}_*.lua); n=${f%.lua}; d=$(grep -o 'durata_s = [0-9]*' "$R/data/missioni/libreria/$f" | head -1 | cut -d' ' -f3)
    "$Z/serie_sandbox" --semi 1 --secondi 60 --motore "$Z/melchior688" --uscita "$U/cod_$m" -- --missione libreria/$n > /dev/null 2>&1
    base=$(grep -m1 '^codice' "$U/cod_$m/serie.toml" | cut -d'"' -f2); cod=$("$R/engine/build_radio/fisso/sonda_codice" "$base" $((d / 60)))
    echo "$m durata $d base $base codice $cod" >> "$U/codici.txt"
    for s in 1 2; do echo "${m}_$s --codice $cod --seme $s --secondi $d"; done; done
} > "$U/lavori_18.txt"
{
for s in 1 2 3 4 5 6 7 8 9 10; do
    echo "m4a_$s --missione prove/$M4 --seme $s --secondi 7200"
    echo "m4b_$s --esperimento $E/quadro_m4_spia.m688v --seme $s --secondi 7200"
    echo "m4c_$s --esperimento $E/quadro_m4_krivak.m688v --seme $s --secondi 7200"
    echo "s5_$s --codice $S5 --seme $s --secondi 7200"
    echo "s5spia_$s --esperimento $E/nave_spia.m688v --seme $s --secondi 7200"
done
} > "$U/lavori_scene.txt"
gioca() { n=$1; shift; t0=$(date +%s); nice -n 19 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "$n rc $? $(( $(date +%s) - t0 )) s" >> "$U/fatti.txt"; rm -f "$U/$n.bmp"; }
export -f gioca; export S U
xargs -P 6 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori_18.txt"
date > "$U/fine_18.txt"
xargs -P 6 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori_scene.txt"
date > "$U/fine_scene.txt"
# il controllo: stessa partita sul binario pulito
nice -n 19 "$Z/melchior688" --sandbox --codice $S5 --seme 1 --secondi 7200 --istantanea "$U/ctl.bmp" > "$U/ctl_s5_1.txt" 2>&1; rm -f "$U/ctl.bmp"
l=$(grep '^S15_1 ' "$U/lavori_18.txt" | cut -d' ' -f2-); nice -n 19 "$Z/melchior688" --sandbox $l --istantanea "$U/ctl.bmp" > "$U/ctl_S15_1.txt" 2>&1; rm -f "$U/ctl.bmp"
date > "$U/fine.txt"
