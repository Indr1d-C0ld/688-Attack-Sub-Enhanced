// Letalita' delle armi sui sottomarini (docs/10_danni_affondamento_esplosioni.md §10.17): un colpo con la geometria vera
// su un sottomarino dell'IA in moto a 5 nodi, poi ore di missione con la regola di simulazione (DanniSottomarini).
// Per ogni prova: fattore d'urto, perso o emerso danneggiato, quando e' fuori combattimento (DanniSottomarini::menomato),
// acqua imbarcata; per ogni classe e quota il riepilogo.
//
// Uso: letalita <dati> <arma> <classi,...> <modo> [semi=12] [ore=4] [quote=50,150,250]
//   arma: mk46, at1, mk48, type53... (la testa la sceglie DanniSottomarini::colpisci dal nome)
//   modo: contatto       lungo lo scafo a caso (entro il 45% della lunghezza), di lato, a contatto
//         lato:D         di traverso a D m dallo scafo, lungo lo scafo a caso
//         sotto:D        D m sotto la chiglia, lungo lo scafo a caso (spoletta d'influenza)
//         spoletta       a 12 m dal centro da una direzione a caso, alla stessa quota (la spoletta del modello)

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/scafo.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

constexpr double DT = 1.0 / 9.0;
constexpr double SPOLETTA_M = 12.0;   // siluri.toml: spoletta_m 6-8 piu' i 5 m dei sottomarini (nucleo/siluro.cpp)
const std::string MODELLO = "modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, "
                            "quota_max_m = 450, integrita = ";

std::vector<std::string> separa(const std::string& s, char c) {
    std::vector<std::string> v;
    std::stringstream in(s);
    for (std::string x; std::getline(in, x, c);)
        if (!x.empty()) v.push_back(x);
    return v;
}

std::string scena(const std::string& classe, double quota) {
    const auto f = fs::temp_directory_path() / ("m688_letalita_" + std::to_string(getpid()) + ".lua");
    std::ofstream(f) << "return { id = 'LETALITA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, "
                     << "posizione = { x = -30000, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 0, " << MODELLO << "160 } },\n"
                     << "{ id = 2, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 3000 }, "
                     << "quota_m = " << quota << ", rotta = 90, velocita_kn = 5, " << MODELLO << "100 } },\n"
                     << "}, timer = {} } } }\n";
    return f.string();
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 5) {
        std::fprintf(stderr, "uso: letalita <dati> <arma> <classi,...> <contatto|lato:D|sotto:D|spoletta> [semi] [ore] [quote]\n");
        return 2;
    }
    const std::string dati = argv[1], arma = argv[2], modo = argv[4];
    const auto classi_prova = separa(argv[3], ',');
    const int semi = argc > 5 ? std::atoi(argv[5]) : 12;
    const double ore = argc > 6 ? std::atof(argv[6]) : 4.0;
    std::vector<double> quote{50.0, 150.0, 250.0};
    if (argc > 7) {
        quote.clear();
        for (const auto& q : separa(argv[7], ',')) quote.push_back(std::atof(q.c_str()));
    }
    const std::string tipo = modo.substr(0, modo.find(':'));
    const double d = modo.find(':') != std::string::npos ? std::atof(modo.substr(modo.find(':') + 1).c_str()) : 0.0;
    if (tipo != "contatto" && tipo != "lato" && tipo != "sotto" && tipo != "spoletta") {
        std::fprintf(stderr, "modo sconosciuto: %s\n", modo.c_str());
        return 2;
    }
    const auto classi = carica_classi(dati + "/classi.toml");
    std::printf("arma\tclasse\tquota_m\tmodo\tseme\tlungo_m\tlato_m\tsotto_m\tfattore_urto\tesito\tmenomato_s\tperso_s\tacqua_m3\n");
    for (const auto& classe : classi_prova) {
        const DatiScafo sc = DatiScafo::carica(dati + "/scafi/" + classe + ".toml");
        for (const double quota : quote) {
            int persi = 0, emersi = 0, menomati = 0;
            double t_persi = 0.0;
            for (int s = 1; s <= semi; ++s) {
                const std::string f = scena(classe, quota);
                Missione m(f, static_cast<std::uint64_t>(s), false);
                m.impatti_dell_originale(false);
                DanniSottomarini ds(classi, dati, static_cast<std::uint64_t>(s));
                m.avanza(DT);
                ds.avanza(m, DT);
                const UnitaClassica v = m.unita().at(2);
                // il punto dello scoppio nel riferimento della vittima
                Casuale caso(static_cast<std::uint64_t>(s) * 7919u);
                double lungo = caso.uniforme(-0.45, 0.45) * sc.lunghezza, lato = 0.0, sotto = 0.0;
                const double verso = caso.uniforme() < 0.5 ? -1.0 : 1.0;
                if (tipo == "contatto") lato = verso * sc.larghezza / 4;            // dentro il raggio: a contatto
                else if (tipo == "lato") lato = verso * (sc.larghezza / 2 + d);
                else if (tipo == "sotto") sotto = sc.larghezza / 2 + d;
                else {
                    const double phi = caso.uniforme(0.0, 2 * PI);
                    lungo = SPOLETTA_M * std::cos(phi);
                    lato = SPOLETTA_M * std::sin(phi);
                }
                const double r = radianti(v.rotta);
                Colpo c;
                c.tipo = Colpo::Tipo::arma;
                c.tempo = m.tempo();
                c.vittima = 2;
                c.classe_vittima = classe;
                c.classe_arma = arma;
                c.pos = {v.pos.x + lungo * std::sin(r) + lato * std::cos(r), v.pos.y + lungo * std::cos(r) - lato * std::sin(r)};
                c.geometria_vera = true;
                c.dz_m = sotto;
                c.distanza_m = std::sqrt(lungo * lungo + lato * lato + sotto * sotto);
                m.registra_colpo(c);
                double menomato = -1.0, perso = -1.0;
                for (long i = 0; i < static_cast<long>(ore * 3600.0 / DT); ++i) {
                    m.avanza(DT);
                    ds.avanza(m, DT);
                    const auto* x = ds.sottomarino(2);
                    if (x && menomato < 0.0 && DanniSottomarini::menomato(*x)) menomato = m.tempo();
                    if (m.unita().count(2) == 0 || m.unita().at(2).distrutta || (x && x->distrutto)) {
                        perso = m.tempo();
                        break;
                    }
                }
                const auto* x = ds.sottomarino(2);
                const bool emerso = x && x->emerso_danneggiato && perso < 0.0;
                persi += perso >= 0.0;
                emersi += emerso;
                menomati += menomato >= 0.0 || perso >= 0.0;
                if (perso >= 0.0) t_persi += perso;
                std::printf("%s\t%s\t%.0f\t%s\t%d\t%.1f\t%.1f\t%.1f\t%.2f\t%s\t%.0f\t%.0f\t%.0f\n", arma.c_str(), classe.c_str(), quota,
                            modo.c_str(), s, lungo, lato, sotto, x ? ds.scossa(2).intensita : 0.0,
                            perso >= 0.0 ? "perso" : emerso ? "emerso" : menomato >= 0.0 ? "menomato" : "in_quota", menomato, perso,
                            x ? x->danni->acqua_imbarcata_m3() : 0.0);
                std::fflush(stdout);
                fs::remove(f);
            }
            std::printf("# %s %s %s a %.0f m: persi %d/%d (in media dopo %.0f s), emersi danneggiati %d, fuori combattimento %d\n",
                        arma.c_str(), classe.c_str(), modo.c_str(), quota, persi, semi, persi ? t_persi / persi : -1.0, emersi, menomati);
            std::fflush(stdout);
        }
    }
}
