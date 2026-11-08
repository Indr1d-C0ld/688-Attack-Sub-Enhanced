#!/bin/bash
# regressione intera dopo il rumore del lancio dell IA (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -C build_acustica -t clean > $L/clean_simm.log 2>&1; echo "pulita: $(tail -1 $L/clean_simm.log)"
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_sm2f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*" > $L/asw_sm.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_sm.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_sm.log)
rm -rf $L/prove_sm $L/tmp_sm; mkdir -p $L/prove_sm $L/tmp_sm $L/rilievo $L/casi_sm
cp build_acustica/prove $L/prove_sm/prove
(cd $L/prove_sm && TMPDIR=$L/tmp_sm ./prove > $L/suite_sm.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_sm.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_sm.log
CARTELLA=$L/casi_sm ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_sm.txt 2> $L/casi_sm.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_sm.txt > $L/casi_sm.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_sm.diff; }
diff $L/casi_cv.txt $L/casi_sm.txt > $L/casi_sm_vs_cv.diff && echo "casi identici a quelli del blocco 1" || { echo "casi DIVERSI dal blocco 1"; cat $L/casi_sm_vs_cv.diff; }
wait
tail -3 $L/suite_sm.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_sm.log | sort -u
