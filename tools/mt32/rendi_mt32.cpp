// Le musiche FM del remake rese con un MT-32 emulato (roadmap 38, E): facoltativo, solo con le ROM dell'utente.
// Legge le partiture di data/motore/musica/fm (il lettore MIDI del motore, nucleo/musica_fm) e le suona con
// libmt32emu (Munt, LGPL-2.1+: solo in questo strumento, mai nel gioco). Le ROM le cerca in
// ~/.local/share/melchior688/mt32/rom (o $M688_MT32/rom): il controllo MT-32 1.07 e la PCM dell'MT-32. I WAV vanno in
// ~/.cache/melchior688/mt32/<brano>.wav, dove il gioco li prende se ci sono (se no, l'OPL3). Niente di quello che
// esce di qui entra nel progetto: tools/mt32/controlla_rom.py lo verifica.
//
// L'MT-32 non e' General MIDI: i programmi delle partiture (GM) si portano sui timbri preimpostati dell'MT-32 con la
// tabella qui sotto (solo quelli che le musiche usano; STIMA d'ascolto), e i canali 1-8 sui 2-9, quelli che l'MT-32
// ascolta di fabbrica (il ritmo resta sul 10).
//
//   rendi_mt32 <cartella delle partiture> [brano...]
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <mt32emu.h>

#include "nucleo/musica_fm.hpp"

namespace fs = std::filesystem;

namespace {

// programma GM (0-based) -> timbro preimpostato dell'MT-32 (0-based, dalla tabella dei timbri del manuale dell'MT-32)
const std::map<int, int> GM_A_MT32 = {
    {0, 0},     // pianoforte -> AcouPiano1
    {8, 22},    // celesta -> Celesta 1
    {11, 97},   // vibrafono -> Vibe 1
    {14, 102},  // campane tubolari -> Tube Bell
    {19, 12},   // organo da chiesa -> Pipe Org 1
    {33, 66},   // basso elettrico -> Elec Bass 1
    {38, 28},   // basso sintetico -> Syn Bass 1
    {43, 56},   // contrabbasso -> Contrabass
    {47, 112},  // timpani -> Timpani
    {48, 48},   // archi -> Str Sect 1
    {52, 34},   // coro -> Chorale
    {56, 88},   // tromba -> Trumpet 1
    {60, 92},   // corno -> Fr Horn 1
    {61, 95},   // ottoni -> Brs Sect 1
    {71, 82},   // clarinetto -> Clarinet 1
    {73, 72},   // flauto -> Flute 1
    {80, 47},   // onda quadra -> Square Wave
    {88, 37},   // pad caldo -> Atmosphere
    {89, 36},   // pad freddo -> Soundtrack
    {95, 32},   // pad scuro -> Fantasy
};

fs::path cartella_rom() {
    if (const char* e = std::getenv("M688_MT32")) return fs::path(e) / "rom";
    return fs::path(std::getenv("HOME")) / ".local/share/melchior688/mt32/rom";
}

fs::path trova(const fs::path& base, const std::vector<std::string>& nomi) {
    for (const auto& n : nomi)
        for (const auto& e : fs::recursive_directory_iterator(base))
            if (e.is_regular_file() && e.path().filename() == n) return e.path();
    return {};
}

void scrivi_wav(const fs::path& p, const std::vector<std::int16_t>& stereo, unsigned fs_hz) {
    std::ofstream out(p, std::ios::binary);
    auto u32 = [&](std::uint32_t v) { out.write(reinterpret_cast<const char*>(&v), 4); };
    auto u16 = [&](std::uint16_t v) { out.write(reinterpret_cast<const char*>(&v), 2); };
    const auto byte = static_cast<std::uint32_t>(stereo.size() * 2);
    out.write("RIFF", 4); u32(36 + byte); out.write("WAVEfmt ", 8); u32(16); u16(1); u16(2); u32(fs_hz); u32(fs_hz * 4); u16(4); u16(16);
    out.write("data", 4); u32(byte);
    out.write(reinterpret_cast<const char*>(stereo.data()), static_cast<std::streamsize>(byte));
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "uso: rendi_mt32 <cartella delle partiture> [brano...]\n");
        return 2;
    }
    const fs::path rom = cartella_rom();
    const fs::path controllo = trova(rom, {"ctrl_mt32_1_07.rom", "mt32_1.0.7.ic27.bin"});
    const fs::path pcm = trova(rom, {"pcm_mt32.rom"});
    if (controllo.empty() || pcm.empty()) {
        std::fprintf(stderr, "rendi_mt32: ROM dell'MT-32 non trovate in %s\n", rom.c_str());
        return 1;
    }
    MT32Emu::FileStream fc, fp;
    if (!fc.open(controllo.c_str()) || !fp.open(pcm.c_str())) return 1;
    const MT32Emu::ROMImage* ic = MT32Emu::ROMImage::makeROMImage(&fc);
    const MT32Emu::ROMImage* ip = MT32Emu::ROMImage::makeROMImage(&fp);
    const fs::path uscita = fs::path(std::getenv("HOME")) / ".cache/melchior688/mt32";
    fs::create_directories(uscita);
    std::vector<std::string> brani;
    for (int i = 2; i < argc; ++i) brani.emplace_back(argv[i]);
    if (brani.empty())
        for (const auto& e : fs::directory_iterator(argv[1]))
            if (e.path().extension() == ".mid") brani.push_back(e.path().stem().string());
    for (const auto& nome : brani) {
        const m688::BranoMidi b = m688::leggi_midi_file((fs::path(argv[1]) / (nome + ".mid")).string());
        MT32Emu::Synth s;
        if (!s.open(*ic, *ip)) {
            std::fprintf(stderr, "rendi_mt32: l'MT-32 non parte\n");
            return 1;
        }
        const unsigned fs_hz = s.getStereoOutputSampleRate();
        const double fine = (b.loop_fine_s >= 0.0 ? b.loop_fine_s : b.durata_s) + 4.0;   // la coda del riverbero
        std::vector<std::int16_t> out;
        std::size_t k = 0;
        const auto rendi_fino = [&](double t) {
            const auto n = static_cast<std::size_t>(t * fs_hz);
            if (n * 2 <= out.size()) return;
            const std::size_t nuovi = n - out.size() / 2;
            const std::size_t da = out.size();
            out.resize(da + nuovi * 2);
            s.render(out.data() + da, static_cast<MT32Emu::Bit32u>(nuovi));
        };
        for (; k < b.eventi.size(); ++k) {
            const auto& e = b.eventi[k];
            if (b.loop_fine_s >= 0.0 && e.t_s >= b.loop_fine_s) break;
            rendi_fino(e.t_s);
            std::uint8_t st = e.dati[0];
            int c = st & 0x0F;
            if (c != 9) c = std::min(c + 1, 8);   // i canali 1-8 sui 2-9
            st = static_cast<std::uint8_t>((st & 0xF0) | c);
            std::uint8_t d1 = e.dati[1];
            if ((st & 0xF0) == 0xC0) {
                const auto it = GM_A_MT32.find(d1);
                d1 = static_cast<std::uint8_t>(it != GM_A_MT32.end() ? it->second : 48);
            }
            s.playMsgNow(static_cast<MT32Emu::Bit32u>(st | (d1 << 8) | (e.dati[2] << 16)));
        }
        for (int c = 1; c <= 9; ++c) s.playMsgNow(static_cast<MT32Emu::Bit32u>(0xB0 | c | (123 << 8)));   // tutte le note spente
        rendi_fino(fine);
        s.close();
        scrivi_wav(uscita / (nome + ".wav"), out, fs_hz);
        std::printf("%-22s %5.1f s a %u Hz -> %s\n", nome.c_str(), out.size() / 2.0 / fs_hz, fs_hz, (uscita / (nome + ".wav")).c_str());
    }
    MT32Emu::ROMImage::freeROMImage(ic);
    MT32Emu::ROMImage::freeROMImage(ip);
    return 0;
}
