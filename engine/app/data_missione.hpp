#pragma once
// La data d'inizio di una missione dell'Enhanced avviata da riga di comando o da un codice di origine «originale»: quella dei
// suoi ordini (file radio R3, «ordini = { ... data = { giorno, mese, anno, ora_utc } }») o, senza, quella scritta nel file della
// missione (`data = "AAAA-MM-GG", ora_utc = H`: libreria e scene di prova). Le originali non hanno una data nel file. Sole,
// luce, meteo della stagione, profilo del suono e DTG seguono questa data.

#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>
#include <string>

#include "nucleo/sole.hpp"

namespace m688 {

inline std::optional<DataOra> data_della_missione(const std::filesystem::path& cartella, const std::string& missione) {
    static const std::regex data_ordini(
        R"re(^\s*ordini\s*=.*data\s*=\s*\{\s*giorno\s*=\s*(\d+)\s*,\s*mese\s*=\s*(\d+)\s*,\s*anno\s*=\s*(\d+)\s*,\s*ora_utc\s*=\s*([\d.]+))re");
    static const std::regex data_file(R"re(^\s*data\s*=\s*["'](\d+)-(\d+)-(\d+)["']\s*,\s*ora_utc\s*=\s*([\d.]+))re");
    std::smatch x;
    std::ifstream radio(cartella / "radio" / (missione + ".lua"));
    for (std::string riga; std::getline(radio, riga);)
        if (std::regex_search(riga, x, data_ordini)) return DataOra{std::stoi(x[3]), std::stoi(x[2]), std::stoi(x[1]), std::stod(x[4])};
    std::ifstream file(cartella / (missione + ".lua"));
    for (std::string riga; std::getline(file, riga);)
        if (std::regex_search(riga, x, data_file)) return DataOra{std::stoi(x[1]), std::stoi(x[2]), std::stoi(x[3]), std::stod(x[4])};
    return std::nullopt;
}

}  // namespace m688
