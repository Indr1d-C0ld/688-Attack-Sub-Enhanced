#include <doctest.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include "../app/percorsi_pacchetto.hpp"
#include "../app/regia_audio.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il pacchetto di rilascio (tools/rilascio/pacchetto_linux.sh): bin/melchior688 trova i dati, le missioni e i brani MT-32
// accanto a se', in ../data; nella build di sviluppo restano i percorsi compilati e la cache dell'utente.

namespace {

// un WAV a 16 bit stereo, come quelli resi con l'MT-32
void scrivi_wav(const fs::path& p, const std::vector<float>& x, int hz) {
    std::ofstream f(p, std::ios::binary);
    const auto u32 = [&](std::uint32_t v) { f.write(reinterpret_cast<const char*>(&v), 4); };
    const auto u16 = [&](std::uint16_t v) { f.write(reinterpret_cast<const char*>(&v), 2); };
    const auto dati = static_cast<std::uint32_t>(x.size() * 4);
    f.write("RIFF", 4), u32(36 + dati), f.write("WAVEfmt ", 8), u32(16), u16(1), u16(2), u32(static_cast<std::uint32_t>(hz)),
        u32(static_cast<std::uint32_t>(hz) * 4), u16(4), u16(16), f.write("data", 4), u32(dati);
    for (const float v : x) {
        const auto s = static_cast<std::int16_t>(v * 32767.0f);
        f.write(reinterpret_cast<const char*>(&s), 2), f.write(reinterpret_cast<const char*>(&s), 2);
    }
}

float picco(RegiaAudio& r, double s) {
    float p = 0.0f;
    for (int k = 0; k < static_cast<int>(s / 0.05 + 0.5); ++k)
        for (const float v : r.produci(2400)) p = std::max(p, std::fabs(v));
    return p;
}

}  // namespace

TEST_CASE("rilascio: i dati e le missioni accanto all'eseguibile, se no il predefinito della build") {
    const fs::path radice = fs::temp_directory_path() / "m688_prova_pacchetto";
    fs::remove_all(radice);
    const fs::path pacchetto = radice / "pacchetto", eseguibile = pacchetto / "bin" / "melchior688";
    fs::create_directories(pacchetto / "bin");

    // senza data/ nel pacchetto: il predefinito compilato; nella build di rilascio (predefinito vuoto) il percorso atteso
    CHECK(cartella_accanto(eseguibile, "data/motore").empty());
    CHECK(cartella_dati(eseguibile, "data/motore", "/progetto/data/motore") == "/progetto/data/motore");
    CHECK(cartella_dati(eseguibile, "data/motore", "") == (pacchetto / "data" / "motore").string());
    CHECK(cartella_dati({}, "data/motore", "/progetto/data/motore") == "/progetto/data/motore");

    // col pacchetto: le sue cartelle vincono sul predefinito
    fs::create_directories(pacchetto / "data" / "motore");
    fs::create_directories(pacchetto / "data" / "missioni");
    CHECK(cartella_accanto(eseguibile, "data/motore") == pacchetto / "data" / "motore");
    CHECK(cartella_dati(eseguibile, "data/motore", "/progetto/data/motore") == (pacchetto / "data" / "motore").string());
    CHECK(cartella_dati(eseguibile, "data/missioni", "/progetto/data/missioni") == (pacchetto / "data" / "missioni").string());

    // la build di sviluppo (engine/<build>/melchior688): data/ sta due livelli sopra, non accanto; resta il predefinito
    fs::create_directories(radice / "progetto" / "data" / "motore");
    fs::create_directories(radice / "progetto" / "engine" / "build");
    const fs::path di_build = radice / "progetto" / "engine" / "build" / "melchior688";
    CHECK(cartella_accanto(di_build, "data/motore").empty());
    CHECK(cartella_dati(di_build, "data/motore", "/progetto/data/motore") == "/progetto/data/motore");

    CHECK_FALSE(eseguibile_in_corso().empty());   // questo binario di prove
    fs::remove_all(radice);
}

TEST_CASE("rilascio: la musica MT-32 da data/mt32 accanto ai dati, prima della cache dell'utente") {
    const fs::path radice = fs::temp_directory_path() / "m688_prova_pacchetto_mt32";
    fs::remove_all(radice);
    const fs::path dati = radice / "data" / "motore", brani = radice / "data" / "mt32", cache = radice / "cache";
    fs::create_directories(dati);
    const char* prima = std::getenv("XDG_CACHE_HOME");
    const std::string vecchia = prima ? prima : "";
    setenv("XDG_CACHE_HOME", cache.c_str(), 1);

    // senza la cartella del pacchetto: la cache dell'utente, come prima
    CHECK(RegiaAudio::cartella_mt32_predefinita() == (cache / "melchior688" / "mt32").string());
    CHECK(RegiaAudio::cartella_mt32_per(dati.string()) == RegiaAudio::cartella_mt32_predefinita());
    {
        RegiaAudio r(dati.string());   // senza timbri FM: senza i WAV, silenzio
        r.volume_musica(1.0f);
        r.uscita_musica(UscitaMusica::mt32);
        r.musica("menu");
        CHECK(picco(r, 1.0) < 1e-4f);
    }

    // col pacchetto: i suoi WAV, anche se la cache ha lo stesso brano
    fs::create_directories(brani);
    fs::create_directories(cache / "melchior688" / "mt32");
    std::vector<float> tono(32000), zitto(32000, 0.0f);
    for (std::size_t i = 0; i < tono.size(); ++i) tono[i] = 0.1f * static_cast<float>(std::sin(2.0 * 3.14159265 * 220.0 * static_cast<double>(i) / 32000.0));
    scrivi_wav(brani / "menu.wav", tono, 32000);
    scrivi_wav(cache / "melchior688" / "mt32" / "menu.wav", zitto, 32000);
    CHECK(RegiaAudio::cartella_mt32_per(dati.string()) == brani.string());
    {
        RegiaAudio r(dati.string());
        r.volume_musica(1.0f);
        r.uscita_musica(UscitaMusica::mt32);
        r.musica("menu");
        CHECK(picco(r, 1.5) > 0.5f);   // il tono del pacchetto, alzato verso 0,8; quello della cache e' muto
    }

    if (prima) setenv("XDG_CACHE_HOME", vecchia.c_str(), 1);
    else unsetenv("XDG_CACHE_HOME");
    fs::remove_all(radice);
}
