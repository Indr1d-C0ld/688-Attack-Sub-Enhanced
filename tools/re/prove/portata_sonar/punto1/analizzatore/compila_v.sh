#!/bin/bash
# Variante privata, sorgenti del gioco non toccati: il LOFAR della stazione con la perdita del sensore (LOFAR_RAGGI=1).
# Senza la variabile d'ambiente fa come il gioco. righe_ricevute, del sonarista, resta al livello A in ogni caso.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$E" "$C" <<'P' || exit 1
import sys
E, C = sys.argv[1:]
t = open(E + "/nucleo/src/analisi_acustica.cpp").read()
a = "    PerditaInterpolata(double f_min, double f_max, double r, double zs, double zr, const Ambiente& amb) {"
b = "    PerditaInterpolata(double f_min, double f_max, double r, double zs, double zr, const Ambiente& amb, const CampoPerdita* campo = nullptr) {"
assert t.count(a) == 1; t = t.replace(a, b)
a = "            tl.push_back(p.tl_db);\n"
b = "            { const auto q = campo ? campo->tl(std::exp(x), r, zr) : std::nullopt; tl.push_back(q ? *q : p.tl_db); }\n"
assert t.count(a) == 1; t = t.replace(a, b)
# solo lo spettro intero (la seconda): non righe_ricevute
a = "        const PerditaInterpolata tl(f_min, f_max, d, s.quota_m, r.quota_m, amb);\n        const double doppler = 1.0 + avvicinamento(s, r) / C_RIFERIMENTO;\n        // i pezzi"
b = "        const CampoPerdita* campo_v = nullptr;\n        if (g_campi_lofar) if (const auto it = g_campi_lofar->find(s.id); it != g_campi_lofar->end()) campo_v = it->second;\n        const PerditaInterpolata tl(f_min, f_max, d, s.quota_m, r.quota_m, amb, campo_v);\n        const double doppler = 1.0 + avvicinamento(s, r) / C_RIFERIMENTO;\n        // i pezzi"
assert t.count(a) == 1; t = t.replace(a, b)
a = "namespace m688 {\n"
b = "#include <map>\nnamespace m688 {\nconst std::map<int, const CampoPerdita*>* g_campi_lofar = nullptr;\n"
assert t.count(a) >= 1; t = t.replace(a, b, 1)
open(C + "/analisi_v.cpp", "w").write(t)
t = open(E + "/app/sonar_enhanced.cpp").read()
a = "    std::vector<Spettro> spettri = spettri_dei_fasci(scena.sorgenti, scena.ricevitore, scena.ambiente, fasci, f_min(), F_MAX, BIN_HZ, fili_per_gli_spettri());\n"
b = "    static const bool lofar_raggi = std::getenv(\"LOFAR_RAGGI\") != nullptr;\n    std::map<int, const CampoPerdita*> campi_v;\n    if (lofar_raggi) { for (std::size_t q = 0; q < scena.sorgenti.size() && q < scena.campi.size(); ++q) if (scena.campi[q]) campi_v[scena.sorgenti[q].id] = scena.campi[q]; g_campi_lofar = &campi_v; }\n" + a + "    g_campi_lofar = nullptr;\n"
assert t.count(a) == 1; t = t.replace(a, b)
a = "namespace m688 {\n"
b = "#include <cstdlib>\n#include <map>\nnamespace m688 { extern const std::map<int, const CampoPerdita*>* g_campi_lofar; }\nnamespace m688 {\n"
t = t.replace(a, b, 1)
open(C + "/sonar_enhanced_v.cpp", "w").write(t)
P
cd "$B" || exit 1
python3 - > "$C/comandi_v.sh" <<'P'
import json, re
C = "analizzatore"
for fine, nome in (("nucleo/src/analisi_acustica.cpp", "analisi_v"), ("app/sonar_enhanced.cpp", "sonar_enhanced_v")):
    for c in json.load(open("compile_commands.json")):
        if c["file"].endswith(fine) and "prove.dir" not in c["command"]:
            k = c["command"]
            k = re.sub(r" -o \S+", f" -o {C}/{nome}.o", k)
            k = re.sub(r" -c \S+", f" -c {C}/{nome}.cpp", k)
            k = k.replace("-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion", "-w").replace('-DTOML_ASSERT="(void)sizeof"', "'-DTOML_ASSERT=(void)sizeof'")
            print(k + " -I../app -I../nucleo/src &")
            break
print("wait")
P
nice -n 10 bash "$C/comandi_v.sh" 2>&1 | grep -E "error|Error" | cut -c1-200
ls -la "$C/analisi_v.o" "$C/sonar_enhanced_v.o" || exit 1
L="$B/libpiattaforma.a $B/libmissioni.a $B/libgrafica.a $R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu/libSDL3.so.0.2.10 $B/libnucleo.a $B/libymfm.a $B/liblua54.a -lm -lpthread"
RP=-Wl,-rpath,$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
FLAG="-DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -DM688_DATI_PREDEFINITI=\"$R/data/motore\" -DM688_MISSIONI_PREDEFINITE=\"$R/data/missioni\" -DTOML_ASSERT=(void)sizeof -I$E/app -I$E/piattaforma/include -I$E/grafica/include -I$E/nucleo/include -I$R/tools/deps/sysroot/usr/include -I$E/missioni/include -isystem $E/terzi/tomlplusplus -isystem $E/terzi/ymfm -O2 -g -DNDEBUG -std=c++20"
nice -n 10 g++ $FLAG -c "$R/tools/re/prove/portata_sonar/punto1/analizzatore/sonda_analizzatore.cpp" -o "$C/sonda2.o" || exit 1
nice -n 10 g++ -O2 $RP "$C/sonda2.o" "$C/sonar_enhanced_v.o" "$C/analisi_v.o" "$C/libapp.a" -o "$C/sonda_v" $L || exit 1
K=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
K=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_v analizzatore/sonar_enhanced_v.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$K")
nice -n 10 bash -c "$K" || exit 1
ls -la "$C/sonda_v" "$C/melchior688_v"
