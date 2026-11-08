// Rilievo delle scene originali con il giocatore passivo: ogni scena con N semi per D secondi, sul fondale reale.
// Per partita: esito e quando; armi andate a segno (vittime per classe), attacchi riusciti su tentati (cariche e
// missili, ATTACK e ATTACK_RND), unita' distrutte; prima scoperta del giocatore da parte di un'unita' ostile
// dell'IA (e quante lo scoprono), prima scoperta di un'unita' ostile da parte del giocatore.
//
//   rilievo_missioni [--ogni-coppia] [--enhanced [--senza-asw] [--meteo] [--variazioni | --senza-variazioni] [--navigazione] [--incagli]] [SECONDI=3600] [SEMI=5] [SCENE...]
// --ogni-coppia: impatti su ogni coppia a ogni passo, come la regola di simulazione (predefiniti: dell'originale)
// --enhanced: la regola di simulazione intera, come la partita (DanniMissione e DanniSottomarini: acustica, radio,
// elettronica, siluri, dinamica, ASW aereo, meteo); --senza-asw la stessa senza l'ASW aereo (docs/13_asw_aereo.md);
// --variazioni con le variazioni acustiche per unita' (R6, ora di serie), --senza-variazioni senza; --meteo col meteo realistico del teatro (docs/15_meteo_e_mare.md; senza: semplificato, mare 3 e sereno)
// --navigazione con la navigazione incerta (roadmap 26; l'IA governa sulla stima, A2): su stderr, per partita, gli
// incagli dell'IA (sottomarini e navi fermati da un fondo piu' alto della chiglia) e le chiglie dei sottomarini sul fondo;
// --incagli gli stessi conti senza la navigazione incerta, per il confronto
//
// Righe separate da tabulazioni, per confrontare due versioni del runtime (engine/README.md, "Danni nelle missioni").

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;

int main(int argc, char** argv) {
    bool impatti_originale = true, enhanced = false, asw = true, meteo_realistico = false, variazioni = false, senza_variazioni = false,
         navigazione = false, incagli = false;
    std::vector<std::string> arg;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--ogni-coppia") impatti_originale = false;
        else if (std::string(argv[i]) == "--enhanced") enhanced = true;
        else if (std::string(argv[i]) == "--senza-asw") asw = false;
        else if (std::string(argv[i]) == "--meteo") meteo_realistico = true;
        else if (std::string(argv[i]) == "--variazioni") variazioni = true;
        else if (std::string(argv[i]) == "--senza-variazioni") senza_variazioni = true;
        else if (std::string(argv[i]) == "--navigazione") navigazione = incagli = true;
        else if (std::string(argv[i]) == "--incagli") incagli = true;
        else arg.push_back(argv[i]);
    }
    const double durata = arg.size() > 0 ? std::stod(arg[0]) : 3600.0;
    const int semi = arg.size() > 1 ? std::stoi(arg[1]) : 5;
    const std::string dati = M688_DATI, dir = M688_MISSIONI;
    std::map<std::string, double> pescaggi;
    const auto classi = carica_classi(dati + "/classi.toml");
    for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
    std::map<std::string, std::shared_ptr<const Fondale>> fondali;
    std::vector<std::string> file;
    for (const auto& e : std::filesystem::directory_iterator(dir))
        if (e.path().extension() == ".lua" &&
            (arg.size() <= 2 || std::find(arg.begin() + 2, arg.end(), e.path().stem().string()) != arg.end()))
            file.push_back(e.path().string());
    std::sort(file.begin(), file.end());
    std::printf("scena\tseme\tvariante\tesito\tt_esito\tarmi\tattacchi\tdistrutte\tt_primo_colpo\tt_scoperto\tscopritori\tt_scopre\tvittime\tdistrutte_classi\n");
    for (const auto& f : file)
        for (int seme = 1; seme <= semi; ++seme) {
            Missione m(f, static_cast<std::uint64_t>(seme));
            m.impatti_dell_originale(impatti_originale);
            auto& fd = fondali[m.teatro()];
            if (!fd) fd = std::make_shared<const Fondale>(Fondale::carica(dati + "/teatri/" + m.teatro() + ".fdl"));
            m.imposta_fondale(fd, pescaggi);
            std::unique_ptr<DanniMissione> dm;
            std::unique_ptr<DanniSottomarini> ds;
            if (enhanced) {
                m.usa_asw(asw);
                if (meteo_realistico) m.usa_meteo(Missione::ModoMeteo::realistico);
                if (variazioni) m.usa_variazioni_acustiche(true);
                if (senza_variazioni) m.usa_variazioni_acustiche(false);
                if (navigazione) m.usa_navigazione(true);
                dm = std::make_unique<DanniMissione>(classi, dati, static_cast<std::uint64_t>(seme), RegolaDanni::simulazione);
                ds = std::make_unique<DanniSottomarini>(classi, dati, static_cast<std::uint64_t>(seme));
                if (const auto* g = m.giocatore()) ds->prepara(m, g->id);
            }
            double t_esito = -1, t_scoperto = -1, t_scopre = -1;
            std::map<int, double> scopritori;   // unita' ostile dell'IA -> prima volta che ha il giocatore fra i contatti
            std::map<int, std::pair<bool, bool>> stato_fondo;   // --navigazione: incagliata e sul fondo al passo prima
            int incagli_sub = 0, incagli_navi = 0, sul_fondo = 0;
            for (long i = 0; i < static_cast<long>(durata * 9); ++i) {
                m.avanza(1.0 / 9.0);
                if (dm) {
                    dm->avanza(m, 1.0 / 9.0);
                    ds->avanza(m, 1.0 / 9.0);
                    m.raccogli_danni(ds.get(), dm.get());
                }
                if (m.esito() && t_esito < 0) t_esito = m.tempo();
                if (incagli)
                    for (const auto& [id, u] : m.unita()) {
                        if (u.giocatore || u.distrutta || u.persa || (u.categoria != "sottomarino" && u.categoria != "nave")) continue;
                        auto& [incagliata, fondo] = stato_fondo[id];
                        if (u.incagliata && !incagliata) ++(u.categoria == "nave" ? incagli_navi : incagli_sub);
                        if (u.sul_fondo && !fondo) ++sul_fondo;
                        incagliata = u.incagliata;
                        fondo = u.sul_fondo;
                    }
                const auto* g = m.giocatore();
                if (!g) continue;
                for (const auto& [id, u] : m.unita()) {
                    if (u.giocatore || u.distrutta || u.persa || ((u.flag ^ g->flag) & 0x18) != 0x18) continue;
                    if (u.contatti.count(g->id) && !scopritori.count(id)) scopritori[id] = m.tempo();
                    if (t_scopre < 0 && g->contatti.count(id)) t_scopre = m.tempo();
                }
                if (t_scoperto < 0 && !scopritori.empty()) t_scoperto = m.tempo();
            }
            int armi = 0, attacchi = 0, riusciti = 0, distrutte = 0;
            double t_primo = -1;
            std::map<std::string, int> vittime, perdite;
            for (const auto& c : m.colpi()) {
                (c.tipo == Colpo::Tipo::arma ? armi : attacchi) += 1;
                riusciti += c.tipo != Colpo::Tipo::arma && c.riuscito;
                distrutte += c.distrutta;
                if (c.distrutta) ++perdite[c.classe_vittima];
                if (c.tipo == Colpo::Tipo::arma) ++vittime[c.classe_vittima];
                if (t_primo < 0 && c.riuscito) t_primo = c.tempo;
            }
            const auto elenco = [](const std::map<std::string, int>& conti) {
                std::string v;
                for (const auto& [classe, n] : conti) v += (v.empty() ? "" : ",") + classe + "x" + std::to_string(n);
                return v.empty() ? std::string("-") : v;
            };
            std::printf("%s\t%d\t%d\t%s\t%.0f\t%d\t%d/%d\t%d\t%.0f\t%.0f\t%zu\t%.0f\t%s\t%s\n", std::filesystem::path(f).stem().c_str(),
                        seme, m.variante(), m.esito() ? std::to_string(*m.esito()).c_str() : "-", t_esito, armi, riusciti, attacchi,
                        distrutte, t_primo, t_scoperto, scopritori.size(), t_scopre, elenco(vittime).c_str(),
                        elenco(perdite).c_str());
            std::fflush(stdout);
            if (incagli)
                std::fprintf(stderr, "#incagli\t%s\t%d\tincagli_sottomarini=%d\tincagli_navi=%d\tsul_fondo=%d\n",
                             std::filesystem::path(f).stem().c_str(), seme, incagli_sub, incagli_navi, sul_fondo);
        }
    return 0;
}
