// Le serie della sandbox (docs/19 §6, V3; nota condivisa sandbox-v3-serie-contratto.md): la stessa missione con le
// stesse regolazioni su N semi, senza finestra. Ogni partita la gioca melchior688 --sandbox in un processo suo, come il
// vetrino col suo codice (la preparazione della sandbox resta una sola, in app/main.cpp), e salva la traccia v2; poi le
// statistiche tecniche di ogni traccia (nucleo/statistiche_sandbox), il riassunto della serie (riassunto_partite) e le
// frequenze degli esiti finiscono in <uscita>/serie.toml (formato in app/serie_dati.hpp), che rilegge app/schermata_serie.
//
//   serie_sandbox --semi N [--da S] [--lavori J] [--uscita CARTELLA] [--secondi T] [--motore melchior688]
//                 (--codice <codice sandbox> | -- <altri argomenti di melchior688 --sandbox: --missione, regolazioni...>)
//
// --secondi e' solo un tetto: il vetrino finisce da se' alla durata delle sue regolazioni o all'esito. Uscita: 0 se tutte
// le partite hanno la traccia, 1 altrimenti (le partite mancanti non entrano nel riassunto).
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "nucleo/statistiche_sandbox.hpp"
#include "nucleo/traccia_replay.hpp"
#include "serie_dati.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

std::string virgolette(const std::string& s) {   // un argomento per la shell, tra apici
    std::string o = "'";
    for (const char c : s) o += c == '\'' ? std::string("'\\''") : std::string(1, c);
    return o + "'";
}

void uso() {
    std::fprintf(stderr, "uso: serie_sandbox --semi N [--da S] [--lavori J] [--uscita CARTELLA] [--secondi T] [--motore PERCORSO]\n"
                         "                    (--codice CODICE | -- ARGOMENTI DI melchior688)\n");
}

}  // namespace

int main(int argc, char** argv) {
    int semi = 0, lavori = std::max(1u, std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1u);
    std::uint64_t da = 1;
    double secondi = 4.0 * 3600.0;
    fs::path uscita = "serie", motore;
    std::string codice;
    std::vector<std::string> altri;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        const auto valore = [&]() -> std::string {
            if (i + 1 >= argc) throw std::runtime_error("manca il valore di " + a);
            return argv[++i];
        };
        try {
            if (a == "--semi") semi = std::stoi(valore());
            else if (a == "--da") da = std::stoull(valore());
            else if (a == "--lavori") lavori = std::max(1, std::stoi(valore()));
            else if (a == "--uscita") uscita = valore();
            else if (a == "--secondi") secondi = std::stod(valore());
            else if (a == "--motore") motore = valore();
            else if (a == "--codice") codice = valore();
            else if (a == "--") {
                for (++i; i < argc; ++i) altri.emplace_back(argv[i]);
            } else {
                std::fprintf(stderr, "serie_sandbox: opzione sconosciuta: %s\n", a.c_str());
                uso();
                return 2;
            }
        } catch (const std::exception& e) {
            std::fprintf(stderr, "serie_sandbox: %s\n", e.what());
            return 2;
        }
    }
    if (semi <= 0 || (codice.empty() && altri.empty())) {
        uso();
        return 2;
    }
    if (motore.empty()) motore = fs::path(argv[0]).parent_path() / "melchior688";   // accanto allo strumento, nella build
    fs::create_directories(uscita);

    // le partite, J alla volta: melchior688 --sandbox senza finestra, la traccia e l'uscita di ognuna nella cartella
    std::atomic<int> prossima{0};
    std::mutex stampa;
    const auto gioca = [&] {
        for (int k; (k = prossima++) < semi;) {
            const std::uint64_t seme = da + static_cast<std::uint64_t>(k);
            const fs::path traccia = uscita / ("partita_" + std::to_string(seme) + ".m688tr");
            std::string cmd = virgolette(motore.string()) + " --sandbox";
            if (!codice.empty()) cmd += " --codice " + virgolette(codice);
            for (const auto& x : altri) cmd += " " + virgolette(x);
            cmd += " --seme " + std::to_string(seme) + " --secondi " + std::to_string(secondi) + " --salva-traccia " + virgolette(traccia.string()) +
                   " --istantanea " + virgolette((uscita / ("partita_" + std::to_string(seme) + ".bmp")).string()) + " > " +
                   virgolette((uscita / ("partita_" + std::to_string(seme) + ".txt")).string()) + " 2>&1";
            const auto t0 = std::chrono::steady_clock::now();
            const int r = std::system(cmd.c_str());
            const double s = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
            std::error_code ec;
            fs::remove(uscita / ("partita_" + std::to_string(seme) + ".bmp"), ec);   // l'istantanea serve solo a girare senza finestra
            const std::lock_guard<std::mutex> l(stampa);
            std::printf("  seme %llu: %s (%.0f s)\n", static_cast<unsigned long long>(seme),
                        r == 0 && fs::exists(traccia) ? "fatta" : "ERRORE (vedi il file .txt)", s);
            std::fflush(stdout);
        }
    };
    std::printf("serie: %d semi da %llu, %d alla volta, in %s\n", semi, static_cast<unsigned long long>(da), lavori, uscita.c_str());
    std::vector<std::thread> fili;
    for (int j = 0; j < std::min(lavori, semi); ++j) fili.emplace_back(gioca);
    for (auto& f : fili) f.join();

    // le statistiche di ogni traccia, nell'ordine dei semi; le partite senza traccia restano fuori
    std::vector<StatisticheSandbox> statistiche;
    std::vector<std::string> tracce;
    std::string missione;
    std::uint64_t primo = 0;
    int mancanti = 0;
    for (int k = 0; k < semi; ++k) {
        const std::uint64_t seme = da + static_cast<std::uint64_t>(k);
        const std::string nome = "partita_" + std::to_string(seme) + ".m688tr";
        try {
            const Traccia t = carica_traccia(uscita / nome);
            if (missione.empty()) missione = t.missione;
            if (statistiche.empty()) primo = seme;
            if (!statistiche.empty() && seme != primo + statistiche.size()) {   // un buco: i semi del riassunto devono essere contigui
                std::fprintf(stderr, "serie_sandbox: manca la partita prima del seme %llu; il riassunto si ferma qui\n",
                             static_cast<unsigned long long>(seme));
                ++mancanti;
                break;
            }
            statistiche.push_back(statistiche_sandbox(t));
            tracce.push_back(nome);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "serie_sandbox: seme %llu: %s\n", static_cast<unsigned long long>(seme), e.what());
            ++mancanti;
        }
    }
    if (statistiche.empty()) {
        std::fprintf(stderr, "serie_sandbox: nessuna partita riuscita\n");
        return 1;
    }
    DatiSerie d = dati_serie(statistiche, tracce, primo);
    d.missione = missione;
    d.codice = codice;
    if (d.codice.empty()) {   // il codice del vetrino dall'uscita della prima partita («codice partita: ...»)
        std::ifstream in(uscita / ("partita_" + std::to_string(primo) + ".txt"));
        static const std::regex riga("codice partita: ([A-Z0-9-]+)");
        for (std::string s; std::getline(in, s);) {
            std::smatch m;
            if (std::regex_search(s, m, riga)) {
                d.codice = m[1];
                break;
            }
        }
    }
    {
        const std::time_t ora = std::time(nullptr);
        char b[16];
        std::strftime(b, sizeof b, "%Y-%m-%d", std::localtime(&ora));
        d.data = b;
    }
    scrivi_serie(uscita / "serie.toml", d);
    std::printf("riassunto: %s (%d partite, %zu esiti diversi)\n", (uscita / "serie.toml").c_str(), d.semi, d.riassunto.esiti.size());
    for (const auto& [codice_esito, e] : d.riassunto.esiti)
        std::printf("  esito %d %s: %d\n", codice_esito, e.testo.c_str(), e.volte);
    return mancanti ? 1 : 0;
}
