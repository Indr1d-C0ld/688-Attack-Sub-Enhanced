#!/bin/bash
# melchior688_m2: il gioco di misura con le frequenze a due decimali, le prime sei voci, le righe tolte dal colore per rilevamento
# (lati) e il sensore forzabile (ANALIZZA_SENSORE). Le letture restano a missione ferma; lo stato della stazione si rimette a posto.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$C" <<'P'
import sys
C=sys.argv[1]; t=open(C+"/sonar_enhanced_fr.cpp").read()
def sost(a,b,n=1):
    global t
    assert t.count(a)>=1, a
    t=t.replace(a,b) if n==0 else t.replace(a,b,n)
sost('"%s%.1f:%.0f:%s:%s", elenco.empty()','"%s%.2f:%.0f:%s:%s", elenco.empty()')
sost('if (elenco.size() < 500) elenco += b;','elenco += b;')
sost('    const bool colore = s.colore_ril_;\n','    const bool colore = s.colore_ril_;\n    const std::string sensore_prima = s.sensore_;\n    if (const char* e = std::getenv("ANALIZZA_SENSORE")) s.sensore_ = e;\n')
sost('s.fascio_fisso_ = fisso; s.segue_tracker_ = segue;','s.sensore_ = sensore_prima; s.fascio_fisso_ = fisso; s.segue_tracker_ = segue;')
sost('    s.colore_ril_ = false;\n    s.lofar_misure_.clear();\n    s.lettura_.reset();','    s.colore_ril_ = true;\n    s.lofar_misure_.clear();\n    s.lettura_.reset();')
sost('elenco.empty() ? "-" : elenco.c_str());\n}','''elenco.empty() ? "-" : elenco.c_str());
    // le prime sei voci; e le stesse senza le righe che il colore per rilevamento da' a un fascio vicino
    const auto voci = [&](const std::vector<ConfrontoRapporti>& v) {
        std::string out;
        for (std::size_t i = 0; i < v.size() && i < 6; ++i) {
            std::vector<double> l;
            for (const auto& [o, x] : v[i].concordi) if (std::find(l.begin(), l.end(), x.hz) == l.end()) l.push_back(x.hz);
            char b[120];
            std::snprintf(b, sizeof b, "%s%s:%.4f:%d:%zu:%zu:%zu", out.empty() ? "" : ";", v[i].id.c_str(), v[i].confidenza, osservate(v[i]), l.size() + v[i].mancanti.size(), v[i].in_piu.size(), l.size());
            out += b;
        }
        return out.empty() ? std::string("-") : out;
    };
    std::vector<RigaOsservata> senza_lati;
    int tolte = 0;
    const Spettro& ult = s.lofar_[0].front();
    for (const auto& o : righe) {
        const auto i = static_cast<std::size_t>(std::floor((o.hz - ult.f0_hz) / ult.df_hz + 1e-6));
        if (!s.lati_.empty() && i < s.lati_.front().size() && s.lati_.front()[i] != 0) ++tolte;
        else senza_lati.push_back(o);
    }
    std::fprintf(f, "%s\\t%d\\t%s\\n", voci(confronti).c_str(), tolte, voci(confronta_rapporti(senza_lati, cat, pale_l)).c_str());
}''')
# la riga finisce con le colonne nuove: il primo fprintf non va a capo
sost('%s\\t%s\\t%.1f\\t%d\\t%d\\t%s\\t%s\\n", primo(senza_pale)','%s\\t%s\\t%.1f\\t%d\\t%d\\t%s\\t%s\\t", primo(senza_pale)')
open(C+"/sonar_enhanced_m2.cpp","w").write(t)
P
cd "$B" || exit 1
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_m2#g; s/ -g / /' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-240
[ -f "$C/sonar_enhanced_m2.o" ] || exit 1
Q=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
Q1=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_m2 analizzatore/sonar_enhanced_m2.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$Q")
nice -n 10 bash -c "$Q1" || exit 1
strip --strip-debug "$C/melchior688_m2"; ls -la "$C/melchior688_m2"
