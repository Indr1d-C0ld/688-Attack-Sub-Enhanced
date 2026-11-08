#!/bin/bash
# regressione intera dopo il rumore del lancio dell IA (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_lx2f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*,*sente le boe*,*la testa del Mk 48*,*evasion*,*difes*,*ping lontano*,*siluri*,*Sting*,*acustica*,*catalogo*,*suite*,*tma*,*TMA*" > $L/asw_lx.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_lx.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_lx.log)
rm -rf $L/prove_lx $L/tmp_lx; mkdir -p $L/prove_lx $L/tmp_lx $L/rilievo $L/casi_lx
cp build_acustica/prove $L/prove_lx/prove
(cd $L/prove_lx && TMPDIR=$L/tmp_lx ./prove > $L/suite_lx.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_lx.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_lx.log
CARTELLA=$L/casi_lx ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_lx.txt 2> $L/casi_lx.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_lx.txt > $L/casi_lx.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_lx.diff; }
diff $L/casi_s3.txt $L/casi_lx.txt > $L/casi_lx_vs_s3.diff && echo "casi identici a quelli della regressione delle 11:49" || { echo "casi DIVERSI dalla regressione delle 11:49"; cat $L/casi_lx_vs_s3.diff; }
wait
tail -3 $L/suite_lx.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_lx.log | sort -u
