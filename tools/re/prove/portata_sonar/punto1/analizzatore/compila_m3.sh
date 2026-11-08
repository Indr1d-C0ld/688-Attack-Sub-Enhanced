#!/bin/bash
# melchior688_m3: m2 piu' le colonne del giocatore (classe, velocita', cavo) e le tracce di tutti i sonaristi a ogni istante di misura. Niente simboli: disco.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$C" <<'P'
import sys
C=sys.argv[1]; t=open(C+"/sonar_enhanced_m2.cpp").read()
def sost(a,b):
    global t
    assert t.count(a)==1, a
    t=t.replace(a,b)
sost('''    const auto sc = m.scena_acustica(g->id, s.sensore_);
    for (const Sorgente& t : sc.sorgenti) {''','''    const auto sc = m.scena_acustica(g->id, s.sensore_);
    // le tracce di tutti: quello che il sonarista dice al giocatore e quello che crede ogni unita' dell'IA
    if (const char* usc = std::getenv("ANALIZZA_USCITA")) {
        static std::FILE* ft = std::fopen((std::string(usc) + ".tracce").c_str(), "w");
        std::map<int, std::string> vero;
        std::map<int, const Sorgente*> per_id;
        for (const Sorgente& x : sc.sorgenti) { vero[x.id] = x.firma ? x.firma->classe : x.fonte ? x.fonte->id : "?"; per_id[x.id] = &x; }
        vero[g->id] = g->classe;
        const auto scrivi = [&](const Sonarista* so, const char* chi, int oid, const std::string& ocl) {
            if (!so) return;
            for (const auto& x : so->tracce()) {
                const auto it = vero.find(x.sorgente);
                const double km = it != vero.end() && per_id.count(x.sorgente) ? distanza(sc.ricevitore.pos, per_id[x.sorgente]->pos) / 1000.0 : -1.0;
                std::fprintf(ft, "%s\\t%s\\t%s\\t%.0f\\t%s\\t%d\\t%s\\t%d\\t%s\\t%d\\t%s\\t%s\\t%d\\t%.1f\\t%s\\t%.1f\\n", std::getenv("ANALIZZA_SCENA"), std::getenv("ANALIZZA_SEME"), std::getenv("ANALIZZA_TRAFFICO"), m.tempo(), chi, oid, ocl.c_str(),
                             x.sorgente, it != vero.end() ? it->second.c_str() : "?", static_cast<int>(x.stato), x.classe.empty() ? "~" : x.classe.c_str(), nome_confidenza(x.confidenza), x.errata ? 1 : 0, x.se_db, x.tipo.empty() ? "~" : x.tipo.c_str(), km);
            }
        };
        scrivi(m.sonarista(), "gioc", g->id, g->classe);
        for (const Sorgente& x : sc.sorgenti)
            if (x.firma) scrivi(m.sonarista_ia(x.id), "ia", x.id, x.firma->classe);
        std::fflush(ft);
    }
    for (const Sorgente& t : sc.sorgenti) {''')
sost('''std::fprintf(f, "%s\\t%d\\t%s\\n", voci(confronti).c_str(), tolte, voci(confronta_rapporti(senza_lati, cat, pale_l)).c_str());''',
'''std::fprintf(f, "%s\\t%d\\t%s\\t%s\\t%s\\t%.1f\\t%.0f\\n", voci(confronti).c_str(), tolte, voci(confronta_rapporti(senza_lati, cat, pale_l)).c_str(), g->classe.c_str(), s.sensore_.c_str(), sc.ricevitore.velocita_kn, sc.ricevitore.cavo_filato_m);''')
open(C+"/sonar_enhanced_m3.cpp","w").write(t)
P
cd "$B" || exit 1
A=$(sed -n 1p "$C/comandi_v.sh"); A=$(sed -E 's/ -g / /; s/ &$//' <<< "$A")
[ -f "$C/analisi_v.o" ] || nice -n 10 bash -c "$A" 2>&1 | grep -E "error" | cut -c1-240
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_m3#g; s/ -g / /' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-300
[ -f "$C/sonar_enhanced_m3.o" ] || exit 1
Q=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
Q1=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_m3 analizzatore/sonar_enhanced_m3.o analizzatore/analisi_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$Q")
nice -n 10 bash -c "$Q1" || exit 1
strip --strip-debug "$C/melchior688_m3"; ls -la "$C/melchior688_m3"; rm -f "$C/sonar_enhanced_m3.o"
