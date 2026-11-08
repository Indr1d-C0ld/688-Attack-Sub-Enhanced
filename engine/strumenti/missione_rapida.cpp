// Missioni rapide procedurali (roadmap punto 10, missioni/missione_rapida.hpp) senza il gioco:
//   missione_rapida --codice 7KQ2-M9A3-PX4F1 [--uscita CARTELLA]      il file della missione e il briefing
//   missione_rapida [--seme N] [--teatro T] [--tipo T] [--alfa] [--forze 1-3] [--paranoia 0-3] [--difficolta 1-5]
//                   [--stagione inverno|primavera|estate|autunno] [--ora alba|giorno|tramonto|notte]
//                   [--meteo realistico] [--senza-asw] [--carriera] [--uscita CARTELLA]
//   missione_rapida --verifica N [--enhanced] [--secondi S]   N semi per tipo: genera, carica e simula S secondi
// Senza --uscita il file va in cartella_missioni_rapide(), dove lo cerca il gioco.
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/missione_rapida.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const std::string DATI = M688_DATI;

TipoRapida tipo_da_nome(const std::string& s) {
    for (int k = 0; k <= 7; ++k)
        if (s == nome_tipo_rapida(static_cast<TipoRapida>(k))) return static_cast<TipoRapida>(k);
    throw std::runtime_error("tipo sconosciuto: " + s);
}

template <class E>
E da_elenco(const std::string& s, std::initializer_list<const char*> nomi) {
    int i = 1;
    for (const char* n : nomi) {
        if (s == n) return static_cast<E>(i);
        ++i;
    }
    throw std::runtime_error("valore sconosciuto: " + s);
}

fs::path scrivi(const MissioneRapida& m, const fs::path& cartella) {
    fs::create_directories(cartella);
    const fs::path f = cartella / (m.id + ".lua");
    std::ofstream(f, std::ios::binary) << m.lua;
    return f;
}

// N semi per tipo: genera, carica, simula; conta gli errori
int verifica(int n, bool enhanced, double secondi) {
    const fs::path cartella = fs::temp_directory_path() / "m688_verifica_rapide";
    const auto classi = carica_classi(DATI + "/classi.toml");
    std::map<std::string, double> pescaggi;
    for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
    int errori = 0, missioni = 0;
    for (int k = 1; k <= 7; ++k) {
        int errori_tipo = 0;
        for (int s = 1; s <= n; ++s) {
            ParametriRapida p;
            p.seme = static_cast<std::uint32_t>(s) * 7919u + static_cast<std::uint32_t>(k);
            p.tipo = static_cast<TipoRapida>(k);
            p.alfa = (p.seme % 3) == 1;
            p.paranoia = static_cast<int>(p.seme % 4);
            p.difficolta = 0;
            p.forze = 0;
            p.asw_aereo = (p.seme % 5) != 0;
            p.meteo_realistico = (p.seme % 2) == 0;
            try {
                const auto r = genera_missione_rapida(p, DATI);
                const std::string f = scrivi(r, cartella).string();
                Missione m(f, p.seme);
                std::unique_ptr<DanniMissione> dm;
                std::unique_ptr<DanniSottomarini> ds;
                if (enhanced) {
                    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
                    m.usa_meteo(p.meteo_realistico ? Missione::ModoMeteo::realistico : Missione::ModoMeteo::semplificato);
                    dm = std::make_unique<DanniMissione>(classi, DATI, p.seme, RegolaDanni::simulazione);
                    ds = std::make_unique<DanniSottomarini>(classi, DATI, p.seme);
                    ds->prepara(m, m.giocatore()->id);
                    constexpr double DT = 1.0 / 9.0;
                    for (long i = 0; i < static_cast<long>(secondi * 9.0) && !m.esito(); ++i) {
                        m.avanza(DT);
                        dm->avanza(m, DT);
                        ds->avanza(m, DT);
                        m.raccogli_danni(ds.get(), dm.get());
                    }
                } else {
                    for (int i = 0; i < static_cast<int>(secondi / 2.0) && !m.esito(); ++i) m.avanza(2.0);
                }
                ++missioni;
                if (!m.errori().empty() || !m.non_implementate().empty()) {
                    ++errori_tipo;
                    std::printf("  %s %s %s: %s\n", r.codice.c_str(), nome_tipo_rapida(r.parametri.tipo), r.parametri.teatro.c_str(),
                                m.errori().empty() ? (*m.non_implementate().begin()).c_str() : m.errori().front().c_str());
                }
                fs::remove(f);
            } catch (const std::exception& e) {
                ++errori_tipo;
                std::printf("  seme %u, %s: %s\n", p.seme, nome_tipo_rapida(p.tipo), e.what());
            }
        }
        std::printf("%-12s %d semi, %d con errori\n", nome_tipo_rapida(static_cast<TipoRapida>(k)), n, errori_tipo);
        errori += errori_tipo;
    }
    std::printf("%d missioni %s, %d con errori\n", missioni, enhanced ? "nell'Enhanced" : "nel classico", errori);
    return errori == 0 ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        ParametriRapida p;
        std::string codice;
        fs::path uscita = cartella_missioni_rapide();
        int n_verifica = 0;
        bool enhanced = false;
        double secondi = 180.0;
        for (int i = 1; i < argc; ++i) {
            const std::string a = argv[i];
            const auto valore = [&]() -> std::string {
                if (i + 1 >= argc) throw std::runtime_error("manca il valore di " + a);
                return argv[++i];
            };
            if (a == "--codice") codice = valore();
            else if (a == "--seme") p.seme = static_cast<std::uint32_t>(std::strtoul(valore().c_str(), nullptr, 10));
            else if (a == "--teatro") p.teatro = valore();
            else if (a == "--tipo") p.tipo = tipo_da_nome(valore());
            else if (a == "--alfa") p.alfa = true;
            else if (a == "--forze") p.forze = std::atoi(valore().c_str());
            else if (a == "--paranoia") p.paranoia = std::atoi(valore().c_str());
            else if (a == "--difficolta") p.difficolta = std::atoi(valore().c_str());
            else if (a == "--stagione") p.stagione = da_elenco<StagioneRapida>(valore(), {"inverno", "primavera", "estate", "autunno"});
            else if (a == "--ora") p.ora = da_elenco<MomentoRapido>(valore(), {"alba", "giorno", "tramonto", "notte"});
            else if (a == "--meteo") p.meteo_realistico = valore() == "realistico";
            else if (a == "--senza-asw") p.asw_aereo = false;
            else if (a == "--carriera") p.carriera = true;
            else if (a == "--uscita") uscita = valore();
            else if (a == "--verifica") n_verifica = std::atoi(valore().c_str());
            else if (a == "--enhanced") enhanced = true;
            else if (a == "--secondi") secondi = std::atof(valore().c_str());
            else throw std::runtime_error("opzione sconosciuta: " + a);
        }
        if (n_verifica > 0) return verifica(n_verifica, enhanced, secondi);
        if (!codice.empty()) {
            const auto q = leggi_codice(codice);
            if (!q) throw std::runtime_error("codice non valido (o di un'altra versione): " + codice);
            p = *q;
        }
        const MissioneRapida m = genera_missione_rapida(p, DATI);
        const fs::path f = scrivi(m, uscita);
        std::printf("%s  %s  %s, %s, %s\n", m.codice.c_str(), m.titolo_it.c_str(), nome_tipo_rapida(m.parametri.tipo),
                    m.parametri.teatro.c_str(), m.parametri.alfa ? "Alfa" : "688");
        std::printf("%s\n\n", f.string().c_str());
        for (const auto& r : m.briefing_it) std::printf("  %s\n", r.c_str());
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "missione_rapida: %s\n", e.what());
        return 2;
    }
}
