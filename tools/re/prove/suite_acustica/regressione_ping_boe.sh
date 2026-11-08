#!/bin/bash
# regressione intera dopo i diff del libretto e dei ping delle boe (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_ping_boe.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
rm -rf $L/prove_pb $L/tmp_pb; mkdir -p $L/prove_pb $L/tmp_pb $L/rilievo $L/casi_pb
cp build_acustica/prove $L/prove_pb/prove
(cd $L/prove_pb && TMPDIR=$L/tmp_pb ./prove > $L/suite_pb.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_pb.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_pb.log
CARTELLA=$L/casi_pb ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_pb.txt 2> $L/casi_pb.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_pb.txt > $L/casi_pb.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_pb.diff; }
wait
tail -3 $L/suite_pb.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_pb.log | sort -u
