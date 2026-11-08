#include "nucleo/compiti_carico.hpp"

#include <stdexcept>
#include <toml.hpp>

namespace m688 {

std::map<std::string, CompitoCarico> carica_compiti(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    std::map<std::string, CompitoCarico> out;
    if (const auto* compiti = t["compiti"].as_table())
        for (auto&& [tipo, nodo] : *compiti) {
            const auto* c = nodo.as_table();
            if (!c) continue;
            CompitoCarico x;
            x.ordine = (*c)["ordine"].value_or(0);
            x.nome = (*c)["nome"].value_or(std::string{});
            x.nota = (*c)["nota"].value_or(std::string{});
            x.fonte = (*c)["fonte"].value_or(std::string{});
            x.confidenza = (*c)["confidenza"].value_or(std::string{});
            out[std::string(tipo.str())] = std::move(x);
        }
    return out;
}

}  // namespace m688
