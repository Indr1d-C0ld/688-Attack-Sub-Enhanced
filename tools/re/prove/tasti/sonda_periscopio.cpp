// Sonda del periscopio (06/11/2026): dalla missione vera, con la dinamica dell'Enhanced, un 688 e un Alfa a piu' quote e campane, ordine di quota periscopica (Ctrl+P: 5,5 m):
// quanto ci mette a scendere sotto la soglia del periscopio (quota della missione <= 6,596 m, nucleo/src/comando.cpp) e dove si ferma. La quota della chiglia e' quella a schermo.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
using namespace m688;
namespace fs = std::filesystem;
int main() {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    constexpr double DT = 1.0 / 9.0;
    for (const char* classe : {"los_angeles", "alfa"})
        for (const double q0 : {6.096, 30.0, 60.0, 120.0})
            for (const double kn : {0.0, 5.0, 10.0}) {
                const auto f = fs::temp_directory_path() / "m688_sonda_periscopio.lua";
                std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n{ id = 0, classe = '" << classe
                                 << "', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 }, quota_m = " << q0
                                 << ", rotta = 0, velocita_kn = 0, modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = 160 } }\n}, timer = {} } } }\n";
                Missione m(f.string(), 1, false);
                m.impatti_dell_originale(false);
                m.dinamica_sottomarini(classi, M688_DATI);
                DanniSottomarini ds(classi, M688_DATI, 1);
                const auto passo = [&] { m.avanza(DT); ds.avanza(m, DT); };
                passo();
                const auto* din = m.dinamica(0);
                if (kn > 0.0) m.applica(Ordine{m.tempo(), TipoOrdine::giri, din->giri_per_velocita(kn), 0.0, 0});
                m.applica(Ordine{m.tempo(), TipoOrdine::quota_con_angolo, PostoEnhanced::QUOTA_PERISCOPICA_M, PostoEnhanced::ANGOLO_MAX, 0});
                double t_sotto = -1.0, qmax = 0.0, qmin = 1e9;
                const int N = static_cast<int>(1800 / DT);
                for (int i = 0; i < N; ++i) {
                    passo();
                    const auto& u = m.unita().at(0);
                    if (t_sotto < 0 && u.quota <= PostoDiComando::QUOTA_PERISCOPICA + 0.5) t_sotto = m.tempo();
                    if (i > N - static_cast<int>(600 / DT)) { qmax = std::max(qmax, u.quota); qmin = std::min(qmin, u.quota); }
                }
                const auto& u = m.unita().at(0);
                std::printf("%s da %.0f m, %.0f kn: sotto la soglia dopo %.0f s; ultimi 10 min quota %.2f-%.2f; chiglia a schermo %.1f m, velocita' %.1f kn, attorno alla soglia 6,60\n", classe, q0, kn, t_sotto, qmin, qmax, m.chiglia_vera(u), din->velocita_kn());
                fs::remove(f);
            }
}
