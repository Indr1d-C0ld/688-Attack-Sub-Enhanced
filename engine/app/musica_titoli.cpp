#include "musica_titoli.hpp"

#include <algorithm>
#include <fstream>

#include "nucleo/campioni.hpp"
#include "nucleo/fascicolo.hpp"

namespace m688 {

const char* nome_versione_musica(VersioneMusica v) {
    switch (v) {
        case VersioneMusica::adlib: return "ADLIB";
        case VersioneMusica::tandy: return "TANDY";
        case VersioneMusica::covox: return "COVOX";
        case VersioneMusica::spenta: return "SPENTA";
    }
    return "SPENTA";
}

namespace {
std::filesystem::path file_preferenza() { return cartella_fascicoli().parent_path() / "musica.txt"; }
}  // namespace

VersioneMusica leggi_preferenza_musica() {
    std::ifstream f(file_preferenza());
    std::string p;
    f >> p;
    for (const auto v : {VersioneMusica::adlib, VersioneMusica::tandy, VersioneMusica::covox, VersioneMusica::spenta})
        if (p == nome_versione_musica(v)) return v;
    return VersioneMusica::adlib;
}

void salva_preferenza_musica(VersioneMusica v) {
    std::error_code ec;
    std::filesystem::create_directories(file_preferenza().parent_path(), ec);
    std::ofstream(file_preferenza()) << nome_versione_musica(v) << '\n';
}

bool MusicaTitoli::suona(VersioneMusica v) {
    versione_ = v;
    if (uscita_) uscita_->svuota();
    uscita_.reset();
    campioni_.clear();
    letti_ = 0;
    if (v == VersioneMusica::spenta) return true;
    static constexpr const char* FILE[] = {"titolo_adlib.wav", "titolo_tandy.wav", "titolo_covox.wav"};
    int fs = 0;
    if (!leggi_wav(dati_ + "/musica/" + FILE[static_cast<int>(v)], campioni_, fs) || fs <= 0) return false;
    uscita_ = UscitaAudio::apri(fs);
    if (!uscita_) return false;
    uscita_->volume(0.7f);
    alimenta();
    return true;
}

void MusicaTitoli::alimenta() {
    if (!uscita_ || letti_ >= campioni_.size()) return;
    const std::size_t n = std::min(uscita_->da_accodare(0.25), campioni_.size() - letti_);
    if (!n) return;
    uscita_->accoda(std::vector<float>(campioni_.begin() + static_cast<std::ptrdiff_t>(letti_),
                                       campioni_.begin() + static_cast<std::ptrdiff_t>(letti_ + n)));
    letti_ += n;
}

}  // namespace m688
