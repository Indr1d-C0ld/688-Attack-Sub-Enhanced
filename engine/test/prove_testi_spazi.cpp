#include <doctest.h>

#include <map>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "nucleo/testi.hpp"

using namespace m688;

namespace {

// Il messaggio a capo sulle parole in righe larghe al piu' `w` pixel alla scala 1 (come la barra dei messaggi)
std::vector<std::string> a_capo(const Font& f, const std::string& s, int w) {
    std::vector<std::string> righe(1);
    for (std::size_t a = 0; a <= s.size();) {
        const std::size_t b = std::min(s.find(' ', a), s.size());
        const std::string parola = s.substr(a, b - a);
        const std::string prova = righe.back().empty() ? parola : righe.back() + " " + parola;
        if (f.larghezza(prova, 1) > w && !righe.back().empty()) righe.push_back(parola);
        else righe.back() = prova;
        a = b + 1;
    }
    return righe;
}

}  // namespace

TEST_CASE("testi: ogni messaggio di bordo sta nella barra, al piu' su due righe (punto 21)") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto testi = carica_testi(M688_DATI "/testi/messaggi_bordo.tsv");
    // i segnaposto coi valori piu' lunghi che possono prendere
    const std::map<std::string, std::string> valori{{"{RIL}", "359"}, {"{SIERRA}", "12"}, {"{CLASSE}", "PROBABILE SOTTOMARINO VICTOR III"},
                                                    {"{D}", "1500"}};
    const int w = LARGHEZZA_TELA - 24;   // la barra: 12 pixel di margine per lato
    std::vector<std::string> lunghi;
    for (const auto& [codice, testo] : testi)
        for (const bool sovietico : {false, true}) {
            std::string s = componi_messaggio(testo, sovietico);
            for (const auto& [k, v] : valori)
                for (auto p = s.find(k); p != std::string::npos; p = s.find(k, p)) s.replace(p, k.size(), v);
            if (a_capo(f, s, w).size() > 2) lunghi.push_back(std::to_string(codice) + ": " + s);
        }
    for (const auto& x : lunghi) MESSAGE(x);
    CHECK(lunghi.empty());
}
