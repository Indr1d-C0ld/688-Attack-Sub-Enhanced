#include "esperimento_ab.hpp"

#include <toml.hpp>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "nucleo/traccia_replay.hpp"
#include "replay_affiancati.hpp"
#include "schermata_confronto.hpp"

namespace m688 {

namespace {

std::string virgolette(const std::string& s) {
    std::string o = "'";
    for (const char c : s) o += c == '\'' ? std::string("'\\''") : std::string(1, c);
    return o + "'";
}

int tracce_in(const std::filesystem::path& d) {
    std::error_code ec;
    int n = 0;
    for (const auto& e : std::filesystem::directory_iterator(d, ec))
        if (e.path().extension() == ".m688tr") ++n;
    return n;
}

}  // namespace

std::optional<std::filesystem::path> gioca_esperimento(AnelloFinestra& w, Tela& tela, const Font& f, const Palette& pal,
                                                       const std::filesystem::path& motore, const std::filesystem::path& m688v,
                                                       const std::filesystem::path& uscita, bool& chiusa) {
    std::string titolo = m688v.stem().string();
    int semi = 20;
    try {
        const toml::table t = toml::parse_file(m688v.string());
        titolo = t["esperimento"]["titolo"].value_or(titolo);
        semi = static_cast<int>(t["esperimento"]["semi"].value_or(20));
    } catch (const std::exception& e) {
        std::fprintf(stderr, "esperimento: %s: %s\n", m688v.c_str(), e.what());
        return std::nullopt;
    }
    std::error_code ec;
    std::filesystem::create_directories(uscita, ec);
    const int lavori = std::clamp(static_cast<int>(std::thread::hardware_concurrency()) - 1, 1, 4);
    const auto strumento = motore.parent_path() / "serie_ab";
    const std::string cmd = virgolette(strumento.string()) + " --esperimento " + virgolette(std::filesystem::absolute(m688v).string()) +
                            " --lavori " + std::to_string(lavori) + " --motore " + virgolette(motore.string()) + " --serie " +
                            virgolette((motore.parent_path() / "serie_sandbox").string()) + " --uscita " + virgolette(uscita.string()) + " > " +
                            virgolette((uscita / "serie_ab.txt").string()) + " 2>&1";
    std::atomic<bool> fatto{false};
    std::thread filo([&] {
        if (std::system(cmd.c_str()) != 0) std::fprintf(stderr, "esperimento: serie_ab non e' riuscito del tutto (%s)\n", (uscita / "serie_ab.txt").c_str());
        fatto = true;
    });
    while (!fatto) {   // la finestra resta viva; l'esperimento non si interrompe
        if (w.eventi().esci) chiusa = true;
        const int a = tracce_in(uscita / "A"), b = tracce_in(uscita / "B");
        tela.riempi(colori::NERO);
        const std::string righe[] = {"ESPERIMENTO A/B: " + titolo,
                                     "A SENZA INTERVENTI: " + std::to_string(a) + " SU " + std::to_string(semi) + "   B CON: " + std::to_string(b) +
                                         " SU " + std::to_string(semi),
                                     "OGNI PARTITA SI GIOCA SENZA FINESTRA, " + std::to_string(lavori) + " ALLA VOLTA. ATTENDERE."};
        for (int k = 0; k < 3; ++k)
            f.scrivi(tela, LARGHEZZA_TELA / 2 - f.larghezza(righe[k], 1) / 2, 190 + 18 * k, righe[k], k == 0 ? colori::GIALLO : colori::GRIGIO, 1);
        const int bw = 400, bx = (LARGHEZZA_TELA - bw) / 2;
        tela.rettangolo_pieno(bx, 260, bw, 8, colori::GRIGIO_SCURO);
        tela.rettangolo_pieno(bx, 260, bw * std::min(a + b, 2 * semi) / std::max(1, 2 * semi), 8, colori::VERDE_CHIARO);
        w.presenta(tela, pal, "albo");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    filo.join();
    if (!std::filesystem::exists(uscita / "confronto.toml")) return std::nullopt;
    return uscita / "confronto.toml";
}

bool mostra_confronto(AnelloFinestra& w, Tela& tela, const Font& f, const Palette& base, const std::string& dati,
                      const std::filesystem::path& confronto) {
    std::optional<SchermataConfronto> sc;
    try {
        sc.emplace(dati, confronto);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "confronto non disponibile: %s\n", e.what());
        return true;
    }
    for (double t0 = w.secondi();;) {
        const Comandi c = w.eventi();
        if (c.esci) return false;
        const double ora = w.secondi();
        const bool fine = sc->comandi(c, ora - t0);
        t0 = ora;
        if (const auto k = sc->coppia_scelta()) {   // i due replay affiancati, poi di nuovo il confronto sulla stessa pagina
            try {
                const CoppiaConfronto& cp = sc->dati().coppie[static_cast<std::size_t>(*k)];
                ReplayAffiancati ra(dati, carica_traccia(sc->traccia_a(*k)), carica_traccia(sc->traccia_b(*k)), cp.divergono ? cp.t_s : -1.0, cp.seme);
                for (double t1 = w.secondi();;) {
                    const Comandi cr = w.eventi();
                    if (cr.esci) return false;
                    const double adesso = w.secondi();
                    if (ra.comandi(cr, adesso - t1)) break;
                    t1 = adesso;
                    ra.disegna(tela, f);
                    Palette pal = base;
                    ra.palette(pal);
                    w.presenta(tela, pal, "");
                }
            } catch (const std::exception& e) {
                std::fprintf(stderr, "replay affiancati non disponibili: %s\n", e.what());
            }
            continue;
        }
        if (fine) return true;
        sc->disegna(tela, f);
        Palette pal = base;
        sc->palette(pal);
        w.presenta(tela, pal, "albo");
    }
}

}  // namespace m688
