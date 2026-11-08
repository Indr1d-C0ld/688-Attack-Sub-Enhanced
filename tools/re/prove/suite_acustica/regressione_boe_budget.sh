#!/bin/bash
# regressione intera dopo la SSQ-47B per boa e il budget della DICASS (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_boe_budget.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*" > $L/asw_bb.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_bb.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua)" $L/asw_bb.log)
rm -rf $L/prove_bb $L/tmp_bb; mkdir -p $L/prove_bb $L/tmp_bb $L/rilievo $L/casi_bb
cp build_acustica/prove $L/prove_bb/prove
(cd $L/prove_bb && TMPDIR=$L/tmp_bb ./prove > $L/suite_bb.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_bb.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_bb.log
CARTELLA=$L/casi_bb ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_bb.txt 2> $L/casi_bb.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_bb.txt > $L/casi_bb.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_bb.diff; }
diff $L/casi_pb.txt $L/casi_bb.txt > $L/casi_bb_vs_pb.diff && echo "casi identici a ieri sera (prima delle boe)" || { echo "casi DIVERSI da ieri sera"; cat $L/casi_bb_vs_pb.diff; }
wait
tail -3 $L/suite_bb.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_bb.log | sort -u
