#!/bin/bash
# regressione intera dopo il rumore del lancio dell IA (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_s22f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*,*sente le boe*,*la testa del Mk 48*,*evasion*,*difes*" > $L/asw_s2.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_s2.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_s2.log)
rm -rf $L/prove_s2 $L/tmp_s2; mkdir -p $L/prove_s2 $L/tmp_s2 $L/rilievo $L/casi_s2
cp build_acustica/prove $L/prove_s2/prove
(cd $L/prove_s2 && TMPDIR=$L/tmp_s2 ./prove > $L/suite_s2.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_s2.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_s2.log
CARTELLA=$L/casi_s2 ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_s2.txt 2> $L/casi_s2.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_s2.txt > $L/casi_s2.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_s2.diff; }
diff $L/casi_sm.txt $L/casi_s2.txt > $L/casi_s2_vs_sm.diff && echo "casi identici a quelli della build pulita delle 08:28" || { echo "casi DIVERSI dalla build delle 08:28"; cat $L/casi_s2_vs_sm.diff; }
wait
tail -3 $L/suite_s2.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_s2.log | sort -u
