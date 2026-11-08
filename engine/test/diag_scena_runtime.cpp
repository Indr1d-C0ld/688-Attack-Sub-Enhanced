#include <doctest.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
using namespace m688;
TEST_CASE("DIAG scena" * doctest::skip(!std::getenv("DIAG_SCENA"))) {
    const std::string f = std::getenv("DIAG_SCENA");
    const int chi = std::atoi(std::getenv("DIAG_UNITA") ? std::getenv("DIAG_UNITA") : "3");
    const double durata = std::getenv("DIAG_S") ? std::atof(std::getenv("DIAG_S")) : 7200.0;
    auto classi = carica_classi(M688_DATI "/classi.toml");
    const std::uint64_t seme = std::getenv("DIAG_SEME") ? std::strtoull(std::getenv("DIAG_SEME"), nullptr, 10) : 1;
    Missione m(f, seme);
    if (std::getenv("DIAG_SENZA_FORZE")) m.usa_forze(false);
    if (std::getenv("DIAG_SENZA_DIFESA")) m.usa_difesa(false);
    if (std::getenv("DIAG_SENZA_EVASIONE")) m.usa_evasione(false);
    if (std::getenv("DIAG_SENZA_ESCHE_MOBILI")) m.usa_esche_mobili(false);
    std::map<std::string, double> pescaggi;
    for (const auto& [id, k] : classi) pescaggi[id] = k.pescaggio_m;
    const std::string fdl = std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl";
    if (std::filesystem::exists(fdl)) m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(fdl)), pescaggi);
    DanniMissione dm(classi, M688_DATI, seme, RegolaDanni::simulazione);
    DanniSottomarini ds(classi, M688_DATI, seme);
    if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    double prossimo = 0;
    for (long i = 0; i < static_cast<long>(durata * 9); ++i) {
        m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); ds.avanza(m, 1.0 / 9.0); m.raccogli_danni(&ds, &dm);
        if (m.tempo() >= prossimo) {
            prossimo += std::getenv("DIAG_OGNI") ? std::atof(std::getenv("DIAG_OGNI")) : 300.0;
            if (const auto* u = m.trova(chi)) {
                std::printf("t %6.0f  %d pos %.0f,%.0f q %.0f v %.1f rotta %.0f berg %d agg %d", m.tempo(), chi, u->pos.x, u->pos.y, u->quota, u->velocita, u->rotta, u->bersaglio, u->agganciato);
                for (const auto& [k, c] : u->contatti) std::printf(" | c%d q%.0f d%.0f pos%d %s se%.1f dist%.0f", k, c.qualita, c.durata, c.posizione_nota, c.sensore.c_str(), c.eccesso_db, m.trova(k) ? distanza(u->pos, m.trova(k)->pos) : -1.0);
                std::printf("\n");
            }
        }
        if (m.esito()) break;
    }
    if (const auto* k = m.controllore_asw(chi)) {
        std::printf("controllore %d:", chi);
        for (const auto& x : k->storia()) std::printf(" %.0f %s(%s)", x.t, nome_stato(x.a), x.motivo.c_str());
        const auto boe = m.boe();   // per valore: una copia sola per begin ed end
        std::printf("\nboe posate da %d: %d\n", chi, static_cast<int>(std::count_if(boe.begin(), boe.end(), [&](const auto& b) { return b.lanciatore == chi; })));
    }
    int mk46 = 0, colpi = 0;
    for (const auto& l : m.lanci_aerei()) mk46 += l.tipo == "mk46";
    for (const auto& c : m.colpi()) colpi += c.vittima == chi;
    std::printf("RIASSUNTO seme %llu: mk46 %d, colpi su %d: %d, esche %zu\n", static_cast<unsigned long long>(seme), mk46, chi, colpi, m.esche_ia().size());
    std::map<int, int> per_tipo;
    for (const auto& e : m.eventi_artiglieria()) ++per_tipo[static_cast<int>(e.tipo)];
    for (const auto& [t, n] : per_tipo) std::printf("artiglieria tipo %d: %d\n", t, n);
    std::map<int, int> per_tiratore;
    for (const auto& e : m.eventi_artiglieria()) if (e.tipo == TipoEventoArtiglieria::partenza) ++per_tiratore[e.tiratore];
    for (const auto& [t, n] : per_tiratore) std::printf("partenze di %d: %d\n", t, n);
    for (const auto& e : m.eventi_missione())
        if (e.unita == chi || e.altra == chi) std::printf("ev %6.0f %s u%d a%d %s\n", e.tempo, nome_evento(e.tipo), e.unita, e.altra, e.testo_it.c_str());
}
