// Verifica del fondale sulle scene originali: ogni scena con N semi per D secondi, con e senza fondale.
// Per partita: esiti (senza/con), urti dell'IA con danni, tocchi senza danni, unita' collocate, unita' sulla terra.
//
//   verifica_fondale [SECONDI=3600] [SEMI=5]
//
// Atteso (engine/README.md, "Fondale e incagli"): nessun urto, nessuna unita' sulla terra. Gli esiti possono
// differire: da quando le armi colpiscono (§9.8 del documento sull'originale) le rotte diverse attorno alle secche
// cambiano gli scontri; la differenza si segnala ma non e' un errore.

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;

int main(int argc, char** argv) {
    const double durata = argc > 1 ? std::stod(argv[1]) : 3600.0;
    const int semi = argc > 2 ? std::stoi(argv[2]) : 5;
    const std::string dati = M688_DATI, dir = M688_MISSIONI;
    std::map<std::string, double> pescaggi;
    for (const auto& [id, c] : carica_classi(dati + "/classi.toml")) pescaggi[id] = c.pescaggio_m;
    std::map<std::string, std::shared_ptr<const Fondale>> fondali;
    std::vector<std::string> file;
    for (const auto& e : std::filesystem::directory_iterator(dir))
        if (e.path().extension() == ".lua") file.push_back(e.path().string());
    std::sort(file.begin(), file.end());
    int problemi = 0, diversi = 0;
    for (const auto& f : file)
        for (int seme = 1; seme <= semi; ++seme) {
            Missione a(f, static_cast<std::uint64_t>(seme)), b(f, static_cast<std::uint64_t>(seme));
            auto& fd = fondali[b.teatro()];
            if (!fd) fd = std::make_shared<const Fondale>(Fondale::carica(dati + "/teatri/" + b.teatro() + ".fdl"));
            b.imposta_fondale(fd, pescaggi);
            std::set<int> urti, tocchi, terra;
            std::map<int, int> integrita;
            for (long i = 0; i < static_cast<long>(durata * 9); ++i) {
                a.avanza(1.0 / 9.0);
                b.avanza(1.0 / 9.0);
                for (const auto& [id, u] : b.unita()) {
                    if (u.giocatore || (u.categoria != "nave" && u.categoria != "sottomarino")) continue;
                    if (u.incagliata || u.sul_fondo) {
                        const auto it = integrita.find(id);
                        (it != integrita.end() && u.integrita < it->second ? urti : tocchi).insert(id);
                    }
                    integrita[id] = u.integrita;
                    if (b.profondita(u.pos) < 0.0) terra.insert(id);
                }
            }
            auto esito = [](const Missione& m) { return m.esito() ? std::to_string(*m.esito()) : std::string("-"); };
            const bool ok = urti.empty() && terra.empty() && b.errori().empty();
            problemi += !ok;
            diversi += esito(a) != esito(b);
            std::printf("%s seme %d variante %d  esito %s/%s  urti %zu  tocchi %zu  collocate %zu  sulla terra %zu%s\n",
                        std::filesystem::path(f).stem().c_str(), seme, b.variante(), esito(a).c_str(), esito(b).c_str(),
                        urti.size(), tocchi.size(), b.spostamenti().size(), terra.size(),
                        !ok ? "  <-- DA VEDERE" : esito(a) != esito(b) ? "  (esito diverso)" : "");
        }
    std::printf("%d partite da vedere, %d con esito diverso\n", problemi, diversi);
    return problemi ? 1 : 0;
}
