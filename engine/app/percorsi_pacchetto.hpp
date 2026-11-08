#pragma once
// Le cartelle del pacchetto di rilascio (tools/rilascio/pacchetto_linux.sh): bin/melchior688 accanto a data/motore,
// data/missioni e data/mt32. Senza il pacchetto, nella build di sviluppo, valgono i percorsi compilati come prima.
// Ordine: l'opzione esplicita (--dati, --cartella-missioni), poi la cartella del pacchetto, poi il predefinito compilato

#include <filesystem>
#include <string>
#include <system_error>

namespace m688 {

// L'eseguibile in corso (/proc/self/exe); vuoto se non si legge
inline std::filesystem::path eseguibile_in_corso() {
    std::error_code ec;
    const auto p = std::filesystem::read_symlink("/proc/self/exe", ec);
    return ec ? std::filesystem::path{} : p;
}

// La cartella `relativa` del pacchetto (per esempio "data/motore") accanto alla cartella dell'eseguibile:
// <pacchetto>/bin/melchior688 -> <pacchetto>/data/motore. Vuota se l'eseguibile non e' in un pacchetto
inline std::filesystem::path cartella_accanto(const std::filesystem::path& eseguibile, const std::string& relativa) {
    if (eseguibile.empty()) return {};
    std::error_code ec;
    const auto c = (eseguibile.parent_path().parent_path() / relativa).lexically_normal();
    return std::filesystem::is_directory(c, ec) ? c : std::filesystem::path{};
}

// La cartella del pacchetto se c'e', se no `compilata` (il predefinito della build). Nella build di rilascio il
// predefinito e' vuoto: resta il percorso atteso nel pacchetto, che i messaggi d'errore nominano
inline std::string cartella_dati(const std::filesystem::path& eseguibile, const std::string& relativa, const std::string& compilata) {
    if (const auto c = cartella_accanto(eseguibile, relativa); !c.empty()) return c.string();
    if (!compilata.empty() || eseguibile.empty()) return compilata;
    return (eseguibile.parent_path().parent_path() / relativa).lexically_normal().string();
}

}  // namespace m688
