#!/bin/bash
# regressione intera dopo il rumore del lancio dell IA (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -C build_acustica -t clean > $L/clean_s3.log 2>&1; echo "pulita: $(tail -1 $L/clean_s3.log)"
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_s32f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*,*sente le boe*,*la testa del Mk 48*,*evasion*,*difes*,*ping lontano*" > $L/asw_s3.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_s3.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_s3.log)
rm -rf $L/prove_s3 $L/tmp_s3; mkdir -p $L/prove_s3 $L/tmp_s3 $L/rilievo $L/casi_s3
cp build_acustica/prove $L/prove_s3/prove
(cd $L/prove_s3 && TMPDIR=$L/tmp_s3 ./prove > $L/suite_s3.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_s3.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_s3.log
CARTELLA=$L/casi_s3 ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_s3.txt 2> $L/casi_s3.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_s3.txt > $L/casi_s3.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_s3.diff; }
diff $L/casi_s2.txt $L/casi_s3.txt > $L/casi_s3_vs_s2.diff && echo "casi identici a quelli della regressione delle 09:45" || { echo "casi DIVERSI dalla regressione delle 09:45"; cat $L/casi_s3_vs_s2.diff; }
wait
tail -3 $L/suite_s3.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_s3.log | sort -u
