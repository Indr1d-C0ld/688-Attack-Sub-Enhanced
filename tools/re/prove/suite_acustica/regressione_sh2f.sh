#!/bin/bash
# regressione intera dopo il controllore dell SH-2F (LAMPS I) (disassemblatore, 30/10/2026)
cd /home/randolph/Claude/melchior-688/engine
L=/home/randolph/Claude/melchior-688/tools/re/prove/suite_acustica
export TMPDIR=$PWD/build_acustica/tmp PATH=$PWD/../tools/venv/bin:$PATH LD_LIBRARY_PATH=$PWD/../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
ninja -C build_acustica -t clean > $L/clean_sh2f.log 2>&1; echo "pulita: $(tail -1 $L/clean_sh2f.log)"
ninja -k 0 -C build_acustica melchior688 prove rilievo_missioni > $L/build_sh2f.log 2>&1 || { echo "BUILD FALLITA"; tail -20 $L/build_ping_boe.log; exit 1; }
echo "build ok"
(cd /home/randolph/Claude/melchior-688/engine && TMPDIR=$L/tmp ./build_acustica/prove -tc="*asw*,*DICASS*,*SSQ*,*boa*,*boe*,*SH-2F*" > $L/asw_sh.log 2>&1; echo "prove asw rc=$?"; tail -3 $L/asw_sh.log; grep -E "MESSAGE: .*(ping a comando|SSQ-47B in acqua|SH-2F su)" $L/asw_sh.log)
rm -rf $L/prove_sh $L/tmp_sh; mkdir -p $L/prove_sh $L/tmp_sh $L/rilievo $L/casi_sh
cp build_acustica/prove $L/prove_sh/prove
(cd $L/prove_sh && TMPDIR=$L/tmp_sh ./prove > $L/suite_sh.log 2>&1; echo "suite rc=$?") &
CARTELLA=$L/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $L/rilievo_sh.log 2>&1; echo "rilievo rc=$?"; tail -3 $L/rilievo_sh.log
CARTELLA=$L/casi_sh ../tools/regressione/casi.sh build_acustica/melchior688 > $L/casi_sh.txt 2> $L/casi_sh.log; echo "casi rc=$?"; diff ../tools/regressione/riferimento.txt $L/casi_sh.txt > $L/casi_sh.diff && echo "casi identici" || { echo "casi DIVERSI"; head -20 $L/casi_sh.diff; }
diff $L/casi_bb.txt $L/casi_sh.txt > $L/casi_sh_vs_bb.diff && echo "casi identici a quelli di stanotte" || { echo "casi DIVERSI da stanotte"; cat $L/casi_sh_vs_bb.diff; }
wait
tail -3 $L/suite_sh.log
awk '/TEST CASE:/{tc=$0} /ERROR:|FATAL/{print tc}' $L/suite_sh.log | sort -u
