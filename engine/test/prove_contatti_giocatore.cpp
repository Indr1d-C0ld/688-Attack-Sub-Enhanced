#include <doctest.h>

#include <filesystem>
#include <string>

#include "missioni/contatti_giocatore.hpp"
#include "missioni/missione.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;

// Contatti del giocatore per carta, periscopio e barra del bersaglio (fase C4; sensori_e_navigazione.md §10.5)

TEST_CASE("contatti: colori della carta come 990C") {
    SegnoContatto s;
    CHECK(colore_segno(s) == 14);                        // X: solo il rilevamento
    s.indicatori = SEGNO_TRACCIA;
    CHECK(colore_segno(s) == 13);                        // traccia solida, durata <= 800
    s.indicatori |= SEGNO_CLASSIFICATO;
    CHECK(colore_segno(s) == 1);                         // sottomarino o ignoto
    s.indicatori |= SEGNO_NAVE_AEREO;
    CHECK(colore_segno(s) == 2);
    s.indicatori |= SEGNO_CLASSE;
    CHECK(colore_segno(s) == 10);                        // colori chiari con la classe nota
    s.indicatori = SEGNO_TRACCIA | SEGNO_CLASSIFICATO | SEGNO_CLASSE | SEGNO_ARMA;
    CHECK(colore_segno(s) == 12);
    s.indicatori |= SEGNO_FILOGUIDATO;
    CHECK(colore_segno(s) == 14);                        // il proprio siluro filoguidato, giallo
}

namespace {
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool c_e_scena() { return std::filesystem::exists(SCENA); }
}  // namespace

TEST_CASE("contatti: segni e barra dal punto di vista del giocatore, orfani esclusi" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    const auto& g = *m.giocatore();
    // una nave a 3 km e il periscopio alzato: a vista la traccia diventa solida e lunga
    for (const auto& [id, u] : m.unita())
        if (u.categoria == "nave") {
            m.trova(id)->pos = {g.pos.x + 3000.0, g.pos.y};
            break;
        }
    REQUIRE(m.applica({0, TipoOrdine::periscopio, 0, 0, 1}));
    int solido = -1;
    for (int i = 0; i < 9 * 900 && solido < 0; ++i) {
        m.avanza(1.0 / 9.0);
        for (const auto& [id, c] : g.contatti)
            if (c.qualita >= 800.0 && c.durata >= 1400.0 && m.unita().count(id)) { solido = id; break; }
    }
    REQUIRE(solido >= 0);
    const auto segni = segni_contatti(m, false);
    REQUIRE_FALSE(segni.empty());
    CHECK(segni.size() <= g.contatti.size());
    bool trovato = false;
    for (const auto& s : segni) {
        CHECK(s.lettera >= 'A');                         // le lettere dei giocatori
        if (s.id != solido) continue;
        trovato = true;
        CHECK(s.traccia());
        CHECK((s.indicatori & SEGNO_CLASSE) != 0);
    }
    CHECK(trovato);
    CHECK(segni_contatti(m, true).size() <= segni.size());   // il filtro lascia solo le armi
    for (const auto& s : segni_contatti(m, true)) CHECK((s.indicatori & SEGNO_ARMA) != 0);

    const BarraBersaglio b = barra_bersaglio(m, solido);
    CHECK(b.titolo == BarraBersaglio::Titolo::classe);
    CHECK(b.classe == m.unita().at(solido).classe);
    CHECK(nome_tipo_originale(b.tipo_originale) == "CACCIATORPEDINIERE CLASSE FORREST SHERMAN");   // TORPEX
    CHECK(nome_tipo_originale(0x2A) == "ESCA ACUSTICA");
    CHECK(nome_tipo_originale(-1).empty());
    CHECK(b.dati);
    CHECK(b.rilevamento_relativo == doctest::Approx(normalizza_360(rilevamento(g.pos, m.unita().at(solido).pos) - g.rotta)));
    CHECK(b.distanza_decimi == static_cast<int>(distanza(g.pos, m.unita().at(solido).pos) / MIGLIO * 10.0));
    CHECK(barra_bersaglio(m, 9999).titolo == BarraBersaglio::Titolo::nessuno);

    // chi esce dal gioco lascia un orfano nella lista, che i segni saltano
    m.togli(*m.trova(solido));
    m.avanza(1.0 / 9.0);   // esce dal gioco al passo
    for (const auto& s : segni_contatti(m, false)) CHECK(s.id != solido);
    CHECK(barra_bersaglio(m, solido).titolo == BarraBersaglio::Titolo::nessuno);
}

TEST_CASE("carta: zoom e proiezione come la stazione Navigazione") {
    CartaGiocatore c;
    CHECK(c.scala() == 6);
    std::vector<int> scale;
    while (c.riduci() == 0) scale.push_back(c.scala());
    CHECK(scale == std::vector<int>{8, 10, 12, 16, 22, 30, 40, 52});   // fino a circa 175 x 96 nm
    CHECK(c.riduci() == MSG_MINIMO_INGRANDIMENTO);
    scale.clear();
    while (c.ingrandisci() == 0) scale.push_back(c.scala());
    CHECK(scale == std::vector<int>{40, 30, 22, 16, 12, 10, 8, 6, 4, 2});
    CHECK(c.scala() == 2);
    CHECK(c.fattore() == 1);
    CHECK(c.minuti() == 10);
    for (int i = 0; i < 89; ++i) c.minuti_piu();
    CHECK(c.minuti() == 99);
    c.minuti_piu();
    CHECK(c.minuti() == 1);
    c.minuti_meno();
    CHECK(c.minuti() == 99);
}
