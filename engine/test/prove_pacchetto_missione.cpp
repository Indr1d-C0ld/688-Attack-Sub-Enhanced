#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "../missioni/src/sha256.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Pacchetti delle missioni dell'editor (missioni/pacchetto_missione.hpp, fase E5): esportazione con le impronte,
// importazione con i controlli (impronta, formati, dipendenze, funzioni m688) e la riscrittura dallo scenario

namespace {

const std::string DATI = M688_DATI;
bool ci_sono_dati() { return fs::exists(DATI + "/missioni_rapide.toml") && fs::exists(DATI + "/teatri/faroe.fdl"); }

const DatiScenario& dati() {
    static const DatiScenario d = carica_dati_scenario(DATI);
    return d;
}

ScenarioMissione scenario_editor(std::uint32_t seme) {
    ParametriRapida p;
    p.seme = seme;
    ScenarioMissione s = genera_missione_rapida(p, DATI).scenario;
    s.rapida.reset();
    s.libreria = 2;
    s.id = "EDITOR_PACCHETTO_" + std::to_string(seme);
    s.autore = "Prova";
    return s;
}

// rifa' l'impronta dopo una modifica, come chi altera un pacchetto con cura
std::string rifirma(std::string x) {
    const auto k = x.find("sha256:") + 7;
    x.replace(k, 64, std::string(64, '0'));
    x.replace(k, 64, sha256::esadecimale(x));
    return x;
}

bool contiene(const EsitoImportazione& e, const std::string& x) {
    for (const auto& p : e.problemi)
        if (p.find(x) != std::string::npos) return true;
    return false;
}

}  // namespace

TEST_CASE("pacchetto: SHA-256 sui vettori di FIPS 180-4") {
    CHECK(sha256::esadecimale("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CHECK(sha256::esadecimale("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(sha256::esadecimale("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST_CASE("pacchetto: esportato e importato, la missione e' quella rigenerata" * doctest::skip(!ci_sono_dati())) {
    const auto dest = fs::temp_directory_path() / "m688_prova_importate";
    fs::remove_all(dest);
    const ScenarioMissione s = scenario_editor(21);
    const std::string p = esporta_pacchetto(s, dati(), DATI, "2026-10-25");
    CHECK(p.find("  pacchetto = { formato_pacchetto = 1") != std::string::npos);
    CHECK(p.find("impronta = \"sha256:") != std::string::npos);
    const auto e = importa_pacchetto(p, dati(), DATI, dest);
    INFO((e.problemi.empty() ? std::string() : e.problemi.front()));
    REQUIRE(e.importato);
    CHECK(e.problemi.empty());
    CHECK(e.id == s.id);
    CHECK(e.autore == "Prova");
    REQUIRE(fs::exists(e.file));
    std::ifstream in(e.file, std::ios::binary);
    const std::string file{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    CHECK(file == scrivi_missione_lua(s, dati()));   // il file scritto e' quello dello scrittore, senza il pacchetto
    Missione m(e.file.string(), 1);
    for (int i = 0; i < 30; ++i) m.avanza(2.0);
    CHECK(m.errori().empty());
    // lo stesso pacchetto si puo' esportare di nuovo uguale
    CHECK(esporta_pacchetto(s, dati(), DATI, "2026-10-25") == p);
    fs::remove_all(dest);
}

TEST_CASE("pacchetto: rifiutati i file rovinati, di altre versioni, con dipendenze o funzioni sconosciute" * doctest::skip(!ci_sono_dati())) {
    const auto dest = fs::temp_directory_path() / "m688_prova_rifiutate";
    fs::remove_all(dest);
    const ScenarioMissione s = scenario_editor(22);
    const std::string p = esporta_pacchetto(s, dati(), DATI, "2026-10-25");
    // un solo carattere cambiato: impronta sbagliata
    {
        std::string x = p;
        x[x.find("velocita_kn = ") + 14] = '9';
        const auto e = importa_pacchetto(x, dati(), DATI, dest);
        CHECK_FALSE(e.importato);
        CHECK(contiene(e, "impronta sbagliata"));
    }
    // non e' un pacchetto: la missione senza blocco
    {
        const auto e = importa_pacchetto(scrivi_missione_lua(s, dati()), dati(), DATI, dest);
        CHECK_FALSE(e.importato);
        CHECK(contiene(e, "non e' un pacchetto"));
    }
    // modificato con l'impronta rifatta: una funzione m688 che non esiste
    {
        std::string x = p;
        x.replace(x.find("script.R_esce = function(u)\n"), 28, "script.R_esce = function(u)\n  m688.apri_file(u)\n");
        const auto e = importa_pacchetto(rifirma(x), dati(), DATI, dest);
        CHECK_FALSE(e.importato);
        CHECK(contiene(e, "funzioni m688 sconosciute: apri_file"));
    }
    // un formato piu' nuovo
    {
        std::string x = p;
        x.replace(x.find("formato_pacchetto = 1"), 21, "formato_pacchetto = 2");
        const auto e = importa_pacchetto(rifirma(x), dati(), DATI, dest);
        CHECK_FALSE(e.importato);
        CHECK(contiene(e, "formato del pacchetto 2"));
    }
    // codice ritoccato ma con funzioni conosciute, e un fondale diverso: importato, con gli avvertimenti
    {
        std::string x = p;
        x.replace(x.find("-- fine della rotta di transito"), 31, "-- la rotta di transito finisce");
        const auto k = x.find("impronta_teatro = \"") + 19;
        x.replace(k, 16, "0123456789abcdef");
        const auto e = importa_pacchetto(rifirma(x), dati(), DATI, dest);
        CHECK(e.importato);
        CHECK(contiene(e, "codice del pacchetto e' diverso"));
        CHECK(contiene(e, "fondale del teatro e' diverso"));
        std::ifstream in(e.file, std::ios::binary);
        const std::string file{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
        CHECK(file == scrivi_missione_lua(s, dati()));   // il codice del pacchetto non entra
    }
    // uno scenario che non si carica: troppe istruzioni o troppa memoria
    {
        std::string x = p;
        x.replace(x.find("local script = {}\n"), 18, "local script = {}\nlocal t = {} for i = 1, 1e9 do t[i] = string.rep('x', 1000) end\n");
        const auto e = importa_pacchetto(rifirma(x), dati(), DATI, dest);
        CHECK_FALSE(e.importato);
    }
    // formato piu' nuovo, teatro e classi sconosciuti, funzioni m688 aggiunte: preparati come scenari diversi
    {
        ScenarioMissione t = s;
        t.teatro = "atlantide";
        CHECK_THROWS(esporta_pacchetto(t, dati(), DATI, "2026-10-25"));   // il fondale non c'e' nemmeno per esportare
    }
    {
        ScenarioMissione t = s;
        t.id = "SCENE1A";   // non puo' coprire una missione originale
        const auto e = importa_pacchetto(esporta_pacchetto(t, dati(), DATI, "2026-10-25"), dati(), DATI, dest);
        CHECK_FALSE(e.importato);
        CHECK(contiene(e, "id della missione non valido"));
    }
    fs::remove_all(dest);
}

TEST_CASE("pacchetto: la cartella da importare, importati e rifiutati spostati" * doctest::skip(!ci_sono_dati())) {
    const auto base = fs::temp_directory_path() / "m688_prova_cartella_importa";
    fs::remove_all(base);
    const auto da = base / "da_importare";
    const auto a = base / "importate";
    fs::create_directories(da);
    std::ofstream(da / "buono.m688m", std::ios::binary) << esporta_pacchetto(scenario_editor(23), dati(), DATI, "2026-10-25");
    std::ofstream(da / "cattivo.m688m", std::ios::binary) << "return { pacchetto = 1 }";
    std::ofstream(da / "ignorato.txt", std::ios::binary) << "niente";
    const auto v = importa_cartella(da, a, dati(), DATI);
    REQUIRE(v.size() == 2);
    CHECK(v[0].sorgente == "buono.m688m");
    CHECK(v[0].importato);
    CHECK(v[1].sorgente == "cattivo.m688m");
    CHECK_FALSE(v[1].importato);
    CHECK(fs::exists(da / "importati" / "buono.m688m"));
    CHECK(fs::exists(da / "rifiutati" / "cattivo.m688m"));
    CHECK(fs::exists(da / "rifiutati" / "cattivo.txt"));
    CHECK(fs::exists(da / "ignorato.txt"));
    CHECK(fs::exists(a / "EDITOR_PACCHETTO_23.lua"));
    CHECK(elenco_scenari(a).size() == 1);
    fs::remove_all(base);
}
