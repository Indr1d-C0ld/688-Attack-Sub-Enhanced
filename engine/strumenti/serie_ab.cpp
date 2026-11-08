// L'esperimento A/B del vetrino di Dio (docs/28 §5, D2; nota condivisa trailer1-d2-forma-m688v.md): dal file .m688v due
// serie sugli stessi semi con strumenti/serie_sandbox, A senza gli interventi (melchior688 --senza-interventi) e B con, in
// <uscita>/A e <uscita>/B; poi il confronto a coppie per seme (app/confronto_ab: nucleo/confronto_serie e il punto di
// divergenza) in <uscita>/confronto.toml, le tre pagine della schermata del confronto in BMP e il rapporto scritto in
// <uscita>/rapporto.md.
//
//   serie_ab --esperimento FILE.m688v [--semi N] [--da S] [--lavori J] [--uscita CARTELLA] [--motore melchior688]
//            [--serie serie_sandbox] [--dati data/motore] [--solo-confronto]
//
// --semi e --da sopra quelli del file; --solo-confronto rifa' solo il confronto dalle due serie gia' giocate. Uscita: 0 se
// tutte le coppie ci sono, 1 se ne manca qualcuna, 2 per un errore.
#include <toml.hpp>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "confronto_ab.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/traccia_replay.hpp"
#include "percorsi_pacchetto.hpp"
#include "schermata_confronto.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

std::string virgolette(const std::string& s) {   // un argomento per la shell, tra apici
    std::string o = "'";
    for (const char c : s) o += c == '\'' ? std::string("'\\''") : std::string(1, c);
    return o + "'";
}

void uso() {
    std::fprintf(stderr, "uso: serie_ab --esperimento FILE.m688v [--semi N] [--da S] [--lavori J] [--uscita CARTELLA]\n"
                         "                [--motore PERCORSO] [--serie PERCORSO] [--dati CARTELLA] [--solo-confronto]\n");
}

}  // namespace

int main(int argc, char** argv) {
    fs::path esperimento, uscita = "esperimento", motore, serie;
    std::string dati = cartella_dati(eseguibile_in_corso(), "data/motore", M688_DATI);   // il pacchetto, poi la build
    int semi = 0, lavori = std::max(1u, std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1u);
    std::uint64_t da = 0;
    bool solo_confronto = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        const auto valore = [&]() -> std::string {
            if (i + 1 >= argc) throw std::runtime_error("manca il valore di " + a);
            return argv[++i];
        };
        try {
            if (a == "--esperimento") esperimento = valore();
            else if (a == "--semi") semi = std::stoi(valore());
            else if (a == "--da") da = std::stoull(valore());
            else if (a == "--lavori") lavori = std::max(1, std::stoi(valore()));
            else if (a == "--uscita") uscita = valore();
            else if (a == "--motore") motore = valore();
            else if (a == "--serie") serie = valore();
            else if (a == "--dati") dati = valore();
            else if (a == "--solo-confronto") solo_confronto = true;
            else {
                std::fprintf(stderr, "serie_ab: opzione sconosciuta: %s\n", a.c_str());
                uso();
                return 2;
            }
        } catch (const std::exception& e) {
            std::fprintf(stderr, "serie_ab: %s\n", e.what());
            return 2;
        }
    }
    if (esperimento.empty()) {
        uso();
        return 2;
    }
    // dal file: i testi, i semi (il resto lo legge melchior688)
    DatiConfronto testa;
    try {
        const toml::table t = toml::parse_file(esperimento.string());
        testa.titolo = t["esperimento"]["titolo"].value_or(std::string{});
        testa.mostra = t["esperimento"]["mostra"].value_or(std::string{});
        testa.ipotesi = t["esperimento"]["ipotesi"].value_or(std::string{});
        testa.codice = t["esperimento"]["codice"].value_or(std::string{});
        if (semi <= 0) semi = static_cast<int>(t["esperimento"]["semi"].value_or(20));
        if (da == 0) da = static_cast<std::uint64_t>(t["esperimento"]["da"].value_or(std::int64_t{1}));
    } catch (const std::exception& e) {
        std::fprintf(stderr, "serie_ab: %s: %s\n", esperimento.c_str(), e.what());
        return 2;
    }
    if (semi <= 0) semi = 20;
    if (da == 0) da = 1;
    const fs::path qui = fs::path(argv[0]).parent_path();
    if (motore.empty()) motore = qui / "melchior688";   // accanto allo strumento, nella build
    if (serie.empty()) serie = qui / "serie_sandbox";
    fs::create_directories(uscita);
    const fs::path assoluto = fs::absolute(esperimento);

    // le due serie, una dopo l'altra (ognuna gioca J partite alla volta)
    if (!solo_confronto)
        for (const bool con : {false, true}) {
            const fs::path cartella = uscita / (con ? "B" : "A");
            std::string cmd = virgolette(serie.string()) + " --semi " + std::to_string(semi) + " --da " + std::to_string(da) + " --lavori " +
                              std::to_string(lavori) + " --secondi 90000 --motore " + virgolette(motore.string()) + " --uscita " +
                              virgolette(cartella.string()) + " -- --esperimento " + virgolette(assoluto.string());
            if (!con) cmd += " --senza-interventi";
            std::printf("serie %s: %s\n", con ? "B (con gli interventi)" : "A (senza)", cartella.c_str());
            std::fflush(stdout);
            if (std::system(cmd.c_str()) != 0) std::fprintf(stderr, "serie_ab: la serie %s ha partite mancanti\n", con ? "B" : "A");
        }

    // le coppie per seme: entrambe le tracce, o la coppia resta fuori (il confronto vuole i semi contigui: ci si ferma al buco)
    std::vector<Traccia> a, b;
    std::vector<std::string> na, nb;
    int mancanti = 0;
    for (int k = 0; k < semi; ++k) {
        const std::uint64_t seme = da + static_cast<std::uint64_t>(k);
        const std::string nome = "partita_" + std::to_string(seme) + ".m688tr";
        try {
            Traccia ta = carica_traccia(uscita / "A" / nome), tb = carica_traccia(uscita / "B" / nome);
            a.push_back(std::move(ta));
            b.push_back(std::move(tb));
            na.push_back("A/" + nome);
            nb.push_back("B/" + nome);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "serie_ab: seme %llu: %s; il confronto si ferma qui\n", static_cast<unsigned long long>(seme), e.what());
            mancanti = semi - k;
            break;
        }
    }
    if (a.empty()) {
        std::fprintf(stderr, "serie_ab: nessuna coppia riuscita\n");
        return 1;
    }
    DatiConfronto d = confronta_tracce(a, b, na, nb, da);
    d.titolo = testa.titolo, d.mostra = testa.mostra, d.ipotesi = testa.ipotesi, d.codice = testa.codice;
    {
        const std::time_t ora = std::time(nullptr);
        char s[16];
        std::strftime(s, sizeof s, "%Y-%m-%d", std::localtime(&ora));
        d.data = s;
    }
    scrivi_confronto(uscita / "confronto.toml", d);

    // le tre pagine della schermata del confronto, per il rapporto
    std::vector<std::string> immagini;
    try {
        SchermataConfronto sc(dati, d, uscita);
        const Font f = Font::carica(dati + "/font/originale.fnt");
        Palette p = Palette::carica(dati + "/immagini/MAIN.pal");
        sc.palette(p);
        for (int k = 0; k < 3; ++k) {
            Tela t;
            sc.pagina(k);
            sc.disegna(t, f);
            const std::string nome = "confronto_" + std::to_string(k + 1) + ".bmp";
            t.salva_bmp((uscita / nome).string(), p);
            immagini.push_back(nome);
        }
        sc.scrivi_rapporto(immagini);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "serie_ab: le pagine non si disegnano (%s): il rapporto senza immagini\n", e.what());
        std::ofstream(uscita / "rapporto.md") << rapporto_confronto(d);
    }
    std::printf("confronto: %s (%zu coppie)\nrapporto: %s\n", (uscita / "confronto.toml").c_str(), d.coppie.size(), (uscita / "rapporto.md").c_str());
    const auto pct = [](double p) { return std::fabs(p) < 0.005 ? 0.0 : p * 100.0; };   // niente «-0»
    for (const auto& [codice, e] : d.confronto.esiti)
        std::printf("  esito %d: A %d/%d, B %d/%d, B-A %+.0f%% (%+.0f%%; %+.0f%%)%s\n", codice, e.a.k, e.a.n, e.b.k, e.b.n, pct(e.differenza),
                    pct(e.basso), pct(e.alto), e.significativo() ? " *" : "");
    return mancanti ? 1 : 0;
}
