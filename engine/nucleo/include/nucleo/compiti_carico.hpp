#pragma once
// I compiti dei carichi standard: nome, nota e fonte per l'elenco della Sala siluri (`[compiti]` di data/motore/carico.toml,
// da tools/pipeline/carico.py). Servono solo a mostrarli: il contenuto dei carichi e' in CatalogoCarico.

#include <map>
#include <string>

namespace m688 {

struct CompitoCarico {
    int ordine = 0;
    std::string nome, nota, fonte, confidenza;
};

// tipo di carico -> compito; std::runtime_error se il file non si legge
std::map<std::string, CompitoCarico> carica_compiti(const std::string& percorso);

}  // namespace m688
