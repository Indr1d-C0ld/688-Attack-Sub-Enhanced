#!/bin/bash
# regressione intera dopo le tarature del sonarista per D3 (disassemblatore, 03/11/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_d3t.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_d3t.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*sonarista*,*percezione*,*interventi*,*libretto*,*classific*" > $L/asw_d3t.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_d3t.log)
rm -rf $L/prove_d3t $L/tmp_d3t; mkdir -p $L/prove_d3t $L/tmp_d3t $L/rilievo $L/casi_d3t
cp build_acustica/prove $L/prove_d3t/prove
(cd $L/prove_d3t && TMPDIR=$L/tmp_d3t ./prove > $L/suite_d3t.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_d3t.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_d3t.log
CARTELLA=$L/casi_d3t ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_d3t.txt 2> $L/casi_d3t.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_d3t.txt > $L/casi_d3t.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_d3t.diff; }
diff $L/casi_lx.txt $L/casi_d3t.txt > $L/casi_d3t_vs_lx.diff && echo "casi identici a quelli di regressione_lx" || { echo "casi DIVERSI dalla regressione delle 11:49"; cat $L/casi_d3t_vs_lx.diff; }
wait
tail -3 $L/suite_d3t.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_d3t.log | sort -u
