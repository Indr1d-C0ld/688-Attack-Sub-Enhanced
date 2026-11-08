#pragma once
// Il file di un esperimento del vetrino di Dio (docs/28 §4-5, D2): `.m688v`, in TOML. Il codice sandbox dice missione,
// regolazioni e seme di partenza; gli interventi programmati sono quelli di missioni/interventi.hpp, con id -1. Lo
// scrive la pagina «interventi programmati», lo leggono `melchior688 --esperimento` e lo strumento serie_ab di trailer-1.
// Forma concordata nella nota condivisa trailer1-d2-forma-m688v.md (31/10/2026).

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "missioni/interventi.hpp"

namespace m688 {

struct Esperimento {
    std::string titolo, mostra, ipotesi;   // per lo spettatore e per il rapporto (l'ipotesi e' facoltativa)
    std::string codice;                    // il codice sandbox: missione, regolazioni, seme di partenza
    std::string impronta_missione;         // per avvisare se la missione e' cambiata (vuota: non controllata)
    int semi = 10;                         // le coppie A/B (10: circa un'ora per la S5 con 3 lavori)
    std::uint64_t da = 1;                  // il primo seme
    std::vector<Intervento> interventi;    // programmati (tempo >= 0), id -1
};

// std::runtime_error se il file non si apre, non e' TOML o manca [esperimento] col codice
Esperimento leggi_esperimento(const std::filesystem::path& p);
void scrivi_esperimento(const std::filesystem::path& p, const Esperimento& e);

}  // namespace m688
