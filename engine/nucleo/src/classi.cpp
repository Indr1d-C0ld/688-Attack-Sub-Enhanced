#include "nucleo/classi.hpp"

#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

std::map<std::string, ClasseNave> carica_classi(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    std::map<std::string, ClasseNave> out;
    for (auto&& [chiave, nodo] : t) {
        const auto* tab = nodo.as_table();
        if (!tab) continue;
        ClasseNave c;
        c.id = std::string(chiave.str());
        c.nome = (*tab)["nome"].value_or(c.id);
        c.sprite = (*tab)["sprite"].value_or(std::string{});
        c.modello = (*tab)["modello"].value_or(std::string{});
        c.scafo = (*tab)["scafo"].value_or(std::string{});
        c.civile = (*tab)["civile"].value_or(false);
        c.tipo_originale = (*tab)["tipo_originale"].value_or(-1);
        c.lunghezza_m = (*tab)["lunghezza_m"].value_or(0.0);
        c.larghezza_m = (*tab)["larghezza_m"].value_or(0.0);
        c.velocita_max_nodi = (*tab)["velocita_max_nodi"].value_or(0.0);
        c.pescaggio_m = (*tab)["pescaggio_m"].value_or(0.0);
        c.altezza_vela_m = (*tab)["altezza_vela_m"].value_or(0.0);
        c.duomo_m = (*tab)["duomo_m"].value_or(0.0);
        for (int k = 0; k < 2; ++k) {
            c.vela_x_m[k] = (*tab)["vela_x_m"][k].value_or(0.0);
            c.duomo_x_m[k] = (*tab)["duomo_x_m"][k].value_or(0.0);
        }
        c.fonte = (*tab)["fonte"].value_or(std::string{});
        c.affidabilita = (*tab)["affidabilita"].value_or(std::string{});
        if (c.sprite.empty() || c.lunghezza_m <= 0.0)
            throw std::runtime_error(percorso + ": classe '" + c.id + "' senza sprite o lunghezza");
        out.emplace(c.id, std::move(c));
    }
    return out;
}

}  // namespace m688
