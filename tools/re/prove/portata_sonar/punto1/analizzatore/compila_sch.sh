#!/bin/bash
# melchior688_sch: la proposta per lo schermo, in una copia privata (nessun sorgente del gioco toccato). Solo la scritta: confronti, tracker e partita restano.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$E/app/sonar_enhanced.cpp" "$C/sonar_enhanced_sch.cpp" <<'P'
import sys
t=open(sys.argv[1]).read()
def sost(a,b):
    global t
    assert t.count(a)==1, a
    t=t.replace(a,b)
# la lettura che lo schermo puo' sostenere
sost('void SonarEnhanced::muovi_cursore(double d_hz, int d_righe) {','''namespace {
// Le voci che spiegano almeno 3 righe osservate e il 60% di quelle lette; le due che ne spiegano di piu'. Sotto, nessun nome.
std::string lettura_righe(const std::vector<ConfrontoRapporti>& v, std::size_t lungo, const std::function<std::string(const std::string&)>& nome, std::size_t* n_righe = nullptr) {
    struct Voce { const ConfrontoRapporti* c; int oss, prese; };
    std::vector<Voce> ok;
    std::size_t n = 0;
    for (const auto& c : v) {
        std::vector<double> o, l;
        for (const auto& [ro, rl] : c.concordi) {
            if (std::find(o.begin(), o.end(), ro.hz) == o.end()) o.push_back(ro.hz);
            if (std::find(l.begin(), l.end(), rl.hz) == l.end()) l.push_back(rl.hz);
        }
        n = o.size() + c.in_piu.size();
        if (o.size() >= 3 && l.size() >= 2 && 10 * o.size() >= 6 * n) ok.push_back({&c, static_cast<int>(o.size()), static_cast<int>(l.size())});
    }
    if (n_righe) *n_righe = n;
    std::stable_sort(ok.begin(), ok.end(), [](const Voce& a, const Voce& b) { return a.oss > b.oss; });
    if (ok.empty()) return {};
    char b[96];
    std::snprintf(b, sizeof b, "%s %d/%zu RIGHE", nome(ok[0].c->id).substr(0, lungo).c_str(), ok[0].oss, n);
    std::string s = b;
    if (ok.size() > 1) {
        std::snprintf(b, sizeof b, " O %s %d/%zu", nome(ok[1].c->id).substr(0, lungo).c_str(), ok[1].oss, n);
        s += b;
    }
    return s;
}
}  // namespace

void SonarEnhanced::muovi_cursore(double d_hz, int d_righe) {''')
sost('''    ipotesi_.clear();
    const CatalogoAcustico* c = m.catalogo_acustico();''','''    ipotesi_.clear();
    confronti_.clear();
    famiglie_.clear();
    const CatalogoAcustico* c = m.catalogo_acustico();''')
sost('''        const auto& c = confronti_.front();
        std::snprintf(riga, sizeof riga, "%s %.0f%%  K %.4f %+.1f KN", nome_voce(c.id).substr(0, 16).c_str(), c.confidenza * 100.0,
                      c.fattore_doppler, c.velocita_radiale_kn);
        f.scrivi(t, x, y, riga, colori::GIALLO, 1);''','''        std::size_t n = 0;
        const std::string l = lettura_righe(confronti_, 12, [&](const std::string& id) { return nome_voce(id); }, &n);
        if (!l.empty()) f.scrivi(t, x, y, l, colori::GIALLO, 1);
        else {
            std::snprintf(riga, sizeof riga, n == 1 ? "NESSUN NOME: 1 RIGA NON BASTA" : "NESSUN NOME: %zu RIGHE NON BASTANO", n);
            f.scrivi(t, x, y, riga, c_tenue(), 1);
        }''')
sost('''        const auto& c = confronti_.front();
        std::snprintf(riga, sizeof riga, "%s %.0f PER CENTO  K %.4f %+.1f KN  %zu/%zu RIGHE", nome_voce(c.id).substr(0, 18).c_str(),
                      c.confidenza * 100.0, c.fattore_doppler, c.velocita_radiale_kn, c.concordi.size(), c.concordi.size() + c.mancanti.size());
        std::string voce = riga;
        if (confronti_.size() > 1) {   // la seconda voce, se ci sta
            const std::string poi = voce + "  POI " + nome_voce(confronti_[1].id).substr(0, 12);
            if (f.larghezza(poi, 1) <= spazio) voce = poi;
        }
        if (f.larghezza(voce, 1) > spazio) voce.resize(voce.size() - 6);   // e se non ci sta nemmeno cosi', senza «RIGHE»
        f.scrivi(t, LX + 2, y_sotto + passo, voce, colori::GIALLO, 1);''','''        std::size_t n = 0;
        std::string voce = lettura_righe(confronti_, 14, [&](const std::string& id) { return nome_voce(id); }, &n);
        const bool nome = !voce.empty();
        if (!nome) {
            std::snprintf(riga, sizeof riga, n == 1 ? "NESSUN NOME: 1 RIGA NON BASTA" : "NESSUN NOME: %zu RIGHE NON BASTANO", n);
            voce = riga;
        }
        f.scrivi(t, LX + 2, y_sotto + passo, voce, nome ? colori::GIALLO : c_tenue(), 1);''')
open(sys.argv[2],"w").write(t)
P
cd "$B" || exit 1
K=$(sed -n 2p "$C/comandi_v.sh"); K=$(sed -E 's#sonar_enhanced_v#sonar_enhanced_sch#g; s/ -g / /' <<< "$K"); K=${K% &}
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-240
[ -f "$C/sonar_enhanced_sch.o" ] || exit 1
Q=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
Q1=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_sch analizzatore/sonar_enhanced_sch.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$Q")
nice -n 10 bash -c "$Q1" || exit 1
strip --strip-debug "$C/melchior688_sch"; ls -la "$C/melchior688_sch"
