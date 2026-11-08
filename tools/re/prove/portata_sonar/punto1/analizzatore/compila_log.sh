#!/bin/bash
# Il gioco vero che a ogni analizza() scrive su stderr le righe viste e le ipotesi, per provare la sonda contro la partita.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$C" <<'P' || exit 1
import sys
C = sys.argv[1]
t = open(C + "/sonar_enhanced_v.cpp").read()
a = "    if (sovrapposta_)\n        for (const auto& x : confronti_)\n            if (x.id == sovrapposta_->id"
b = """    if (std::getenv("ANALIZZA_LOG")) {
        const auto* gg = m.giocatore();
        std::fprintf(stderr, "ANALIZZA t %.0f fascio %.1f pos %.0f %.0f rotta %.0f righe", m.tempo(), fascio_, gg->pos.x, gg->pos.y, gg->rotta);
        for (const auto& q : righe) std::fprintf(stderr, " %.1f:%.0f", q.hz, q.snr_db);
        std::fprintf(stderr, " | pale %d |", pale.value_or(0));
        for (std::size_t q = 0; q < confronti_.size() && q < 3; ++q) std::fprintf(stderr, " %s %.3f c%zu", confronti_[q].id.c_str(), confronti_[q].confidenza, confronti_[q].concordi.size());
        std::fprintf(stderr, " | firma %s %.3f passo %s\\n", ipotesi_.empty() ? "-" : ipotesi_.front().classe.c_str(), ipotesi_.empty() ? 0.0 : ipotesi_.front().confidenza, nome_passo(passo_da_ipotesi(ipotesi_)));
        for (const auto& q : m.scena_acustica(gg->id, sensore_).sorgenti) if (q.firma && q.firma->nazionalita != "civile" && q.id != gg->id) std::fprintf(stderr, "  NEL_FASCIO %d %s %.1f km ril %.1f v %.1f\\n", q.id, q.firma->classe.c_str(), distanza(gg->pos, q.pos) / 1000.0, rilevamento(gg->pos, q.pos), q.velocita_kn);
    }
""" + a
assert t.count(a) == 1; t = t.replace(a, b)
t = t.replace("#include <cstdlib>\n", "#include <cstdlib>\n#include <cstdio>\n", 1)
open(C + "/sonar_enhanced_log.cpp", "w").write(t)
P
cd "$B" || exit 1
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_log#g' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-200
L=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
L=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_log analizzatore/sonar_enhanced_log.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$L")
nice -n 10 bash -c "$L" || exit 1
ls -la "$C/melchior688_log"
