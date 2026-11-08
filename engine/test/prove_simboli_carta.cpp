// I simboli delle viste dall'alto (grafica/simboli_carta.hpp): posti della palette, forme per parte e dominio.
#include <doctest.h>

#include <cstdlib>
#include <set>

#include "carta.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"

using namespace m688;
namespace s = m688::simboli;

namespace {

// i pixel accesi nella tela, come coppie (dx, dy) attorno a (cx, cy)
std::set<std::pair<int, int>> accesi(const Tela& t, int cx, int cy) {
    std::set<std::pair<int, int>> r;
    for (int y = 0; y < t.altezza(); ++y)
        for (int x = 0; x < t.larghezza(); ++x)
            if (t.at(x, y)) r.insert({x - cx, y - cy});
    return r;
}

}  // namespace

TEST_CASE("simboli: gli stessi posti della palette della carta") {
    CHECK(s::NTDS == colori_carta::NTDS);
    CHECK(s::MATITA == colori_carta::MATITA);
    CHECK(s::CURSORE == colori_carta::CURSORE);
    CHECK(s::EVIDENZA == colori_carta::EVIDENZA);
    CHECK(s::indice_parte(s::Parte::amico) == colori_carta::NTDS);
    CHECK(s::indice_parte(s::Parte::neutrale, true) == colori_carta::NTDS + 7);
    CHECK(s::indice_matita(s::Parte::ignoto, true) == colori_carta::MATITA + 5);   // bianco sul blu della VGA
    CHECK(s::indice_matita(s::Parte::ignoto, false) == colori_carta::MATITA);
    CHECK(s::indice_croce_tma(true) == colori_carta::CURSORE);
}

TEST_CASE("simboli: il telaio APP-6 per parte e per dominio") {
    for (const s::Parte p : {s::Parte::amico, s::Parte::ostile, s::Parte::ignoto, s::Parte::neutrale}) {
        Tela sup(32, 32), sub(32, 32), aer(32, 32);
        sup.riempi(0), sub.riempi(0), aer.riempi(0);
        s::disegna_simbolo_unita(sup, {16, 16}, p, s::Dominio::superficie, 1);
        s::disegna_simbolo_unita(sub, {16, 16}, p, s::Dominio::subacqueo, 1);
        s::disegna_simbolo_unita(aer, {16, 16}, p, s::Dominio::aereo, 1);
        const auto a = accesi(sup, 16, 16), b = accesi(sub, 16, 16), c = accesi(aer, 16, 16);
        CHECK(a.size() > 12);
        for (const auto& [dx, dy] : b) CHECK(dy >= 0);   // il subacqueo: solo la meta' inferiore
        for (const auto& [dx, dy] : c) CHECK(dy <= 0);   // l'aereo: solo la superiore
        CHECK(b.count({-5, 0}) == 1);                     // col tratto orizzontale
        CHECK(a.count({0, 0}) == 0);                      // la superficie e' vuota dentro
        for (const auto& [dx, dy] : a) CHECK(a.count({-dx, dy}) == 1);   // simmetrico
    }
    // le forme sono diverse fra le parti
    std::set<std::set<std::pair<int, int>>> forme;
    for (const s::Parte p : {s::Parte::amico, s::Parte::ostile, s::Parte::ignoto, s::Parte::neutrale}) {
        Tela t(32, 32);
        t.riempi(0);
        s::disegna_simbolo_unita(t, {16, 16}, p, s::Dominio::superficie, 1);
        forme.insert(accesi(t, 16, 16));
    }
    CHECK(forme.size() == 4);
}

TEST_CASE("simboli: il battello del giocatore, le cose in acqua e la legenda") {
    Tela t(64, 64);
    t.riempi(0);
    s::disegna_proprio(t, {32, 32});
    CHECK(t.at(32, 32) == s::VERDE_PROPRIO);
    CHECK(t.at(29, 29) == s::VERDE_PROPRIO_SCURO);
    // il siluro verso est ha la punta a destra
    Tela e(32, 32);
    e.riempi(0);
    s::disegna_oggetto(e, {16, 16}, s::Oggetto::siluro, 3, 90.0);
    CHECK(e.at(19, 16) == 3);
    CHECK(e.at(13, 16) == 3);
    // il biologico e' grigio qualunque colore si chieda
    Tela b(32, 32);
    b.riempi(0);
    s::disegna_oggetto(b, {16, 16}, s::Oggetto::biologico, 3);
    for (const auto& [dx, dy] : accesi(b, 16, 16)) CHECK(b.at(16 + dx, 16 + dy) == colori::GRIGIO);
    // l'ellisse sta nei suoi semiassi
    Tela el(100, 100);
    el.riempi(0);
    s::disegna_ellisse(el, {50, 50}, 30.0, 10.0, 0.0, 5);
    for (const auto& [dx, dy] : accesi(el, 50, 50)) {
        CHECK(std::abs(dx) <= 11);   // l'asse maggiore verso nord
        CHECK(std::abs(dy) <= 31);
    }
}

TEST_CASE("simboli: dai nomi della simulazione al simbolo") {
    CHECK(s::dominio_da_categoria("sottomarino") == s::Dominio::subacqueo);
    CHECK(s::dominio_da_categoria("nave") == s::Dominio::superficie);
    CHECK(s::dominio_da_categoria("aereo", "p3c") == s::Dominio::aereo);
    CHECK(s::dominio_da_categoria("aereo", "sh60b") == s::Dominio::elicottero);
    CHECK(s::dominio_da_categoria("") == s::Dominio::ignoto);
    CHECK_FALSE(s::e_oggetto("nave"));
    CHECK(s::e_oggetto("arma"));
    CHECK(s::e_oggetto("boa"));
    CHECK_FALSE(s::e_oggetto("nessuna"));   // le unita' senza categoria nota: telaio dell'ignoto, non un siluro
    CHECK_FALSE(s::e_oggetto(""));
    CHECK(s::dominio_da_categoria("nessuna") == s::Dominio::ignoto);
    CHECK(s::oggetto_da_classe("arma", "mk48") == s::Oggetto::siluro);
    CHECK(s::oggetto_da_classe("arma", "esca_mobile") == s::Oggetto::esca);
    CHECK(s::oggetto_da_classe("arma", "harpoon") == s::Oggetto::missile);
    CHECK(s::oggetto_da_classe("arma", "mk67") == s::Oggetto::mina);
    CHECK(s::oggetto_da_classe("boa", "ssq53b") == s::Oggetto::boa);
    CHECK(s::oggetto_da_classe("biologico", "") == s::Oggetto::biologico);
    // l'eta' del dato
    CHECK(s::indice_parte_eta(s::Parte::ostile, 60.0) == s::indice_parte(s::Parte::ostile));
    CHECK(s::indice_parte_eta(s::Parte::ostile, 900.0) == s::indice_parte(s::Parte::ostile, true));
    // tutto ignoto: il telaio intero col «?» dentro, diverso dalla superficie ignota
    Tela a(32, 32), b(32, 32);
    a.riempi(0), b.riempi(0);
    s::disegna_simbolo_unita(a, {16, 16}, s::Parte::ignoto, s::Dominio::superficie, 1);
    s::disegna_simbolo_unita(b, {16, 16}, s::Parte::ignoto, s::Dominio::ignoto, 1);
    CHECK(accesi(b, 16, 16).size() > accesi(a, 16, 16).size());
}
