# Dal sorgente della variante (LOFAR_RAGGI) il binario di misura: dentro aggiorna() della stazione del gioco vero, agli istanti di
# ANALIZZA_SPAZZA, ferma la missione (non la tocca: Missione e' const) e legge ogni unita' entro 40 km come il tasto 4, col fascio
# sul rilevamento vero. Le partite restano quelle del gioco: posizioni, velocita' e campi veri.
import sys
C = sys.argv[1]
t = open(C + "/sonar_enhanced_v.cpp").read()
a = "void SonarEnhanced::aggiorna(const Missione& m, int bersaglio) {\n    const auto* g = m.giocatore();\n"
b = r'''
namespace {
bool sub_t(const std::string& t) { return t == "ssn" || t == "ssgn" || t == "ssbn" || t == "ssk"; }
std::string gruppo_firma(const CatalogoAcustico& c, const std::string& id) {
    if (c.tipo_civile(id)) return "civile";
    if (const auto* f = c.firma(id)) {
        const std::string& t = f->tipo_unita;
        if (sub_t(t)) return "sub";
        if (t == "siluro" || t == "esca") return "arma";
        if (t == "biologico") return "natura";
        if (t == "mercantile" || t == "petroliera" || t == "peschereccio" || t == "generica") return "civile";
        return "guerra";
    }
    if (const auto* s = c.sorgente(id)) return s->categoria == "civile" || s->categoria == "industriale" ? "umano" : "natura";
    return "?";
}
int osservate(const ConfrontoRapporti& x) {
    std::vector<double> hz;
    for (const auto& [o, l] : x.concordi)
        if (std::find(hz.begin(), hz.end(), o.hz) == hz.end()) hz.push_back(o.hz);
    return static_cast<int>(hz.size());
}
std::string primo(const std::vector<ConfrontoRapporti>& v, std::size_t i = 0) {
    char b[120];
    if (v.size() <= i) return "-\t0\t0";
    std::snprintf(b, sizeof b, "%s\t%.3f\t%d", v[i].id.c_str(), v[i].confidenza, osservate(v[i]));
    return b;
}
std::string posto(const std::vector<ConfrontoRapporti>& v, const std::string& c) {
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i].id == c) return std::to_string(i + 1) + "\t" + std::to_string(osservate(v[i]));
    return "0\t0";
}
}  // namespace

// una lettura: il fascio sul bersaglio, 6 misure a missione ferma (l'integrazione), poi analizza()
void spazza_leggi(SonarEnhanced& s, const Missione& m, const Sorgente& t, const CatalogoAcustico& cat, std::FILE* f);

void spazza_gioco(SonarEnhanced& s, const Missione& m) {
    static std::vector<double> tempi;
    static bool letti = false;
    static std::FILE* f = nullptr;
    if (!letti) {
        letti = true;
        if (const char* e = std::getenv("ANALIZZA_SPAZZA")) {
            std::stringstream ss(e);
            std::string x;
            while (std::getline(ss, x, ',')) tempi.push_back(std::atof(x.c_str()));
            f = std::fopen(std::getenv("ANALIZZA_USCITA"), "w");
        }
    }
    if (tempi.empty() || !f || m.tempo() < tempi.front()) return;
    tempi.erase(tempi.begin());
    const auto* g = m.giocatore();
    const auto* cat = m.catalogo_acustico();
    if (!g || !cat || s.sensore_.empty()) return;
    // lo stato della stazione si rimette a posto: chi gioca dopo non vede la misura
    const auto fisso = s.fascio_fisso_;
    const int segue = s.segue_tracker_;
    const bool colore = s.colore_ril_;
    const double prossimo = s.prossimo_;
    const auto sc = m.scena_acustica(g->id, s.sensore_);
    for (const Sorgente& t : sc.sorgenti) {
        if (!t.firma || t.firma->righe.empty() || t.id == g->id) continue;
        const std::string gr = gruppo_firma(*cat, t.firma->classe);
        if (gr == "arma" || gr == "natura") continue;
        if (distanza(sc.ricevitore.pos, t.pos) > 40000.0) continue;
        spazza_leggi(s, m, t, *cat, f);
    }
    std::fflush(f);
    s.fascio_fisso_ = fisso; s.segue_tracker_ = segue; s.colore_ril_ = colore; s.prossimo_ = prossimo;
    s.lofar_misure_.clear();
}

void spazza_leggi(SonarEnhanced& s, const Missione& m, const Sorgente& t, const CatalogoAcustico& cat, std::FILE* f) {
    const auto* g = m.giocatore();
    const auto sc0 = m.scena_acustica(g->id, s.sensore_);
    const double km = distanza(sc0.ricevitore.pos, t.pos) / 1000.0;
    const double ril = rilevamento(sc0.ricevitore.pos, t.pos);
    s.fascio_fisso_ = ril;
    s.segue_tracker_ = 0;
    s.colore_ril_ = false;
    s.lofar_misure_.clear();
    s.lettura_.reset();
    s.demon_casc_.svuota();
    s.rilevatore_.azzera();
    for (int i = 0; i < 6; ++i) { s.prossimo_ = 0.0; s.aggiorna(m, t.id); }
    if (s.lofar_[0].empty()) return;
    const auto sc = m.scena_acustica(g->id, s.sensore_);
    const auto righe = s.righe_per_analisi();
    const bool pag = s.pagina_analisi_;
    // i confronti e le ipotesi si azzerano: analizza() senza righe li lascerebbe della lettura prima (a parte, nella nota)
    s.confronti_.clear();
    s.analizza(m);
    s.pagina_analisi_ = pag;
    const std::vector<ConfrontoRapporti> confronti = s.confronti_;
    const std::vector<Ipotesi> ipotesi = s.ipotesi_;
    const PassoClassifica passo = passo_da_ipotesi(ipotesi);
    const std::optional<LetturaDemon> ld = s.demon_casc_.lettura() ? s.demon_casc_.lettura() : s.lettura_;
    auto rr = righe_ricevute(sc.sorgenti, sc.ricevitore, sc.ambiente, ril, 10.0, 410.0, 0.1);
    std::map<int, std::string> nome_di;
    for (const Sorgente& x : sc.sorgenti) nome_di[x.id] = x.firma ? x.firma->classe : x.fonte ? x.fonte->id : "?";
    const bool raggi = std::getenv("LOFAR_RAGGI") != nullptr;
    if (raggi)
        for (auto& x : rr)
            if (x.sorgente >= 0)
                for (std::size_t i = 0; i < sc.sorgenti.size() && i < sc.campi.size(); ++i)
                    if (sc.sorgenti[i].id == x.sorgente && sc.campi[i]) {
                        const double d = distanza(sc.ricevitore.pos, sc.sorgenti[i].pos);
                        if (const auto tl = sc.campi[i]->tl(x.hz, d, sc.ricevitore.quota_m)) x.snr_db += perdita_trasmissione(x.hz, d, sc.sorgenti[i].quota_m, sc.ricevitore.quota_m, sc.ambiente).tl_db - *tl;
                    }
    int ntar = 0, nalt = 0, npro = 0, nvuo = 0;
    std::string elenco;
    for (const auto& o : righe) {
        const RigaRicevuta* meglio = nullptr;
        for (const auto& x : rr)
            if (std::fabs(x.hz - o.hz) <= 0.15 && (!meglio || x.snr_db > meglio->snr_db)) meglio = &x;
        // del bersaglio se una sua riga sta li' sopra il rumore, anche se un'altra e' piu' forte
        bool del_bersaglio = false;
        for (const auto& x : rr)
            if (x.sorgente == t.id && std::fabs(x.hz - o.hz) <= 0.15 && x.snr_db > -6.0) del_bersaglio = true;
        const char* chi = "v";
        if (del_bersaglio) { chi = "T"; ++ntar; }
        else if (meglio && meglio->snr_db > -6.0) {
            if (meglio->sorgente < 0) { chi = "P"; ++npro; }
            else { chi = "A"; ++nalt; }
        } else ++nvuo;
        char b[96];
        std::snprintf(b, sizeof b, "%s%.1f:%.0f:%s:%s", elenco.empty() ? "" : ",", o.hz, o.snr_db, chi, meglio && meglio->sorgente >= 0 ? nome_di[meglio->sorgente].c_str() : "");
        if (elenco.size() < 500) elenco += b;
    }
    std::string vicini;
    for (const Sorgente& x : sc.sorgenti) {
        if (x.id == t.id || (!x.firma && !x.fonte)) continue;
        if (std::fabs(normalizza_180(rilevamento(sc.ricevitore.pos, x.pos) - ril)) > 5.0) continue;
        const std::string c = x.firma ? x.firma->classe : x.fonte->id;
        vicini += (vicini.empty() ? "" : ",") + c + ":" + std::to_string(static_cast<int>(distanza(sc.ricevitore.pos, x.pos) / 1000.0));
        if (vicini.size() > 120) break;
    }
    const std::optional<int> pale_l = ld ? std::optional<int>(ld->pale) : std::nullopt;
    const auto senza_pale = confronta_rapporti(righe, cat, std::nullopt);
    s.maschera_ = false;
    const auto r_off = s.righe_per_analisi();
    s.maschera_ = true;
    const auto off = confronta_rapporti(r_off, cat, pale_l);
    const auto solo = righe_ricevute({t}, sc.ricevitore, sc.ambiente, ril, 10.0, 410.0, 0.1);
    std::vector<RigaOsservata> rc, rd;
    const CampoPerdita* campo = nullptr;
    for (std::size_t i = 0; i < sc.sorgenti.size(); ++i)
        if (sc.sorgenti[i].id == t.id && i < sc.campi.size()) campo = sc.campi[i];
    for (const auto& x : solo) {
        if (x.sorgente != t.id) continue;
        if (x.snr_db >= 3.0) rc.push_back({x.hz, x.snr_db});
        double delta = 0.0;
        if (campo) {
            const double d = distanza(sc.ricevitore.pos, t.pos);
            if (const auto tl = campo->tl(x.hz, d, sc.ricevitore.quota_m)) delta = perdita_trasmissione(x.hz, d, t.quota_m, sc.ricevitore.quota_m, sc.ambiente).tl_db - *tl;
        }
        if (x.snr_db + delta >= 3.0) rd.push_back({x.hz, x.snr_db + delta});
    }
    const auto cc = confronta_rapporti(rc, cat, pale_l), cd = confronta_rapporti(rd, cat, pale_l);
    std::map<int, const CampoPerdita*> campi_e;
    if (raggi) { if (campo) campi_e[t.id] = campo; g_campi_lofar = &campi_e; }
    std::vector<RigaOsservata> re = righe_osservate(spettro_fascio({t}, sc.ricevitore, sc.ambiente, ril, 10.0, 410.0, 0.1));
    g_campi_lofar = nullptr;
    {
        std::vector<double> hz;
        for (const auto& q : righe_proprie(sc.ricevitore, sc.ambiente, ril, 0.1)) hz.push_back(q.hz);
        togli_righe(re, hz, 0.2);
    }
    const auto ce = confronta_rapporti(re, cat, pale_l);
    double mg_max = -999.0;
    int mg_n = 0, mg_tot = 0;
    for (const RigaEmessa& e : righe_emesse(t)) {
        if (e.hz < 10.0 || e.hz >= 410.0) continue;
        const double mg = e.db - (livello_sorgente(e.hz, t) + 10.0 * std::log10(0.1));
        mg_max = std::max(mg_max, mg);
        mg_n += mg >= 3.0;
        ++mg_tot;
    }
    std::string son = "-\t-\t-";
    if (const Sonarista* so = m.sonarista())
        for (const auto& x : so->tracce())
            if (x.sorgente == t.id && x.stato == TracciaSonar::Stato::classificata) {
                son = (x.classe.empty() ? std::string("~") : x.classe) + "\t" + std::to_string(static_cast<int>(x.confidenza)) + "\t" + (x.errata ? "1" : "0");
                break;
            }
    double q = -1.0;
    if (const auto c = g->contatti.find(t.id); c != g->contatti.end()) q = c->second.qualita;
    const std::string classe = t.firma ? t.firma->classe : t.fonte->id;
    const char* sc_nome = std::getenv("ANALIZZA_SCENA");
    const char* sc_seme = std::getenv("ANALIZZA_SEME");
    const char* sc_tr = std::getenv("ANALIZZA_TRAFFICO");
    std::fprintf(f, "scena\t%s\t%s\t%.0f\t%s\t%d\t%s\t%s\t%.2f\t%.0f\t%.1f\t%.0f\t%.0f\t%s\t%zu\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%.3f\t%d\t%s\t",
                 sc_nome ? sc_nome : "?", sc_seme ? sc_seme : "?", m.tempo(), sc_tr ? sc_tr : "?", t.id, classe.c_str(), gruppo_firma(cat, classe).c_str(), km,
                 normalizza_180(ril - g->rotta), t.velocita_kn, t.quota_m, q, vicini.empty() ? "-" : vicini.c_str(), righe.size(), ntar, nalt, npro, nvuo,
                 t.firma ? t.firma->pale : 0, pale_l.value_or(0), primo(confronti).c_str(), primo(confronti, 1).c_str(),
                 ipotesi.empty() ? "-" : ipotesi.front().classe.c_str(), ipotesi.empty() ? 0.0 : ipotesi.front().confidenza, ipotesi.empty() ? 0 : ipotesi.front().righe_concordi, nome_passo(passo));
    std::fprintf(f, "%s\t%zu\t%s\t%zu\t%s\t%s\t%zu\t%s\t%s\t%zu\t%s\t%s\t%.1f\t%d\t%d\t%s\t%s\n", primo(senza_pale).c_str(), r_off.size(), primo(off).c_str(), rc.size(), primo(cc).c_str(), posto(cc, classe).c_str(),
                 rd.size(), primo(cd).c_str(), posto(cd, classe).c_str(), re.size(), primo(ce).c_str(), posto(ce, classe).c_str(), mg_max, mg_n, mg_tot, son.c_str(), elenco.empty() ? "-" : elenco.c_str());
}

void SonarEnhanced::aggiorna(const Missione& m, int bersaglio) {
    static bool dentro = false;
    if (!dentro) { dentro = true; spazza_gioco(*this, m); dentro = false; }
    const auto* g = m.giocatore();
'''
assert t.count(a) == 1; t = t.replace(a, b)
t = t.replace('#include <cstdlib>\n#include <map>\n', '#include <cstdlib>\n#include <cstdio>\n#include <map>\n#include <sstream>\n', 1)
# gli amici: le funzioni di misura leggono lo stato privato della stazione
a = '#include "sonar_enhanced.hpp"\n'
if t.count(a) != 1:
    a = '#include "app/sonar_enhanced.hpp"\n'
assert t.count(a) == 1, "include"
t = t.replace(a, '''#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <map>
#include <mutex>
#include <random>
#include <variant>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <future>
#include <thread>
#include <atomic>
#include <chrono>
#include <new>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <system_error>
#include <stdexcept>
#include <condition_variable>
#define private public
''' + a + '#undef private\n')
open(C + "/sonar_enhanced_mis.cpp", "w").write(t)
