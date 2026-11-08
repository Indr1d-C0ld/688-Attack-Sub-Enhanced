#!/bin/bash
# regressione intera dopo il rumore del lancio dell IA (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_t222f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*,*sente le boe*,*la testa del Mk 48*,*evasion*,*difes*,*ping lontano*,*siluri*,*Sting*,*acustica*,*catalogo*,*suite*" > $L/asw_t22.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_t22.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_t22.log)
rm -rf $L/prove_t22 $L/tmp_t22; mkdir -p $L/prove_t22 $L/tmp_t22 $L/rilievo $L/casi_t22
cp build_acustica/prove $L/prove_t22/prove
(cd $L/prove_t22 && TMPDIR=$L/tmp_t22 ./prove > $L/suite_t22.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_t22.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_t22.log
CARTELLA=$L/casi_t22 ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_t22.txt 2> $L/casi_t22.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_t22.txt > $L/casi_t22.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_t22.diff; }
diff $L/casi_s3.txt $L/casi_t22.txt > $L/casi_t22_vs_s3.diff && echo "casi identici a quelli della regressione delle 11:49" || { echo "casi DIVERSI dalla regressione delle 11:49"; cat $L/casi_t22_vs_s3.diff; }
wait
tail -3 $L/suite_t22.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_t22.log | sort -u
