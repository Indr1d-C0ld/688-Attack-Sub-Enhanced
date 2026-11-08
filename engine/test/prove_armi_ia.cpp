// Prove delle armi dell'IA (nucleo/armi_ia.hpp; fase 4 delle interazioni fra IA): carico per classe e anno, scelta
// dell'arma, guida a filo
#include <doctest.h>

#include <cmath>
#include <optional>
#include <string>

#include "nucleo/armi_ia.hpp"

using namespace m688;

namespace {

const CatalogoCarico& carico() {
    static const CatalogoCarico c = carica_carico(M688_DATI "/carico.toml");
    return c;
}
const CatalogoArmiIa& ia() {
    static const CatalogoArmiIa c = carica_armi_ia(M688_DATI "/interazioni.toml");
    return c;
}
const TabellaSiluri& siluri() {
    static const TabellaSiluri t = carica_siluri(M688_DATI "/siluri.toml");
    return t;
}

SorgenteAcustica sottomarino(int id, Vec2 pos, double quota, double rotta, double kn) {
    SorgenteAcustica s;
    s.id = id, s.pos = pos, s.quota = quota, s.rotta = rotta, s.velocita = kn * NODO, s.livello_db = 130.0;
    s.forza_bersaglio_db = 15.0, s.lunghezza_m = 100.0;
    return s;
}

// Un Mk 48 lanciato dal punto (0, 0) con la rotta sbagliata di `errore` gradi contro un bersaglio a 8 km per nord che
// va a 6 kn per est. Con la guida l'IA lo porta sulla stima (la posizione vera con 300 m d'errore). Ritorna se la
// testa ha agganciato il bersaglio entro 12 minuti, e quanto vicino e' passato
struct Corsa {
    bool aggancio = false, taglio = false;
    double minima_m = 1e9;
};
Corsa corsa(std::optional<Esperienza> guida, double errore) {
    ImpostazioniSiluro imp;
    imp.rotta = errore;
    imp.rte_m = 5000.0;
    imp.quota_corsa_m = imp.quota_ricerca_m = 100.0;
    Siluro s(*siluri().siluro("mk48"), imp, {0.0, 0.0}, 100.0, 0, 7);
    std::vector<SorgenteAcustica> v = {sottomarino(2, {0.0, 8000.0}, 100.0, 90.0, 6.0)};
    std::optional<GuidaFiloIa> g;
    if (guida) g.emplace(dottrina_filo(ia(), *guida));
    Corsa c;
    const Lanciatore lanciatore{{0.0, 0.0}, 4.0 * NODO, 0.0};
    for (int k = 0; k < 720 && s.attivo(); ++k) {
        s.avanza(1.0, v, {}, lanciatore);
        auto ev = s.eventi();
        for (const auto& e : ev)
            if (e.tipo == EventoSiluro::Tipo::aggancio && e.sorgente == 2) c.aggancio = true;
        if (g) {
            SituazioneFilo sit;
            sit.t = k;
            sit.siluro = &s;
            sit.eventi = ev;
            sit.bersaglio_stimato = Vec2{v[0].pos.x + 300.0, v[0].pos.y - 200.0};
            for (const auto& cmd : g->passo(sit)) {
                if (cmd.tipo == ComandoFiloTipo::taglia) c.taglio = true;
                s.comanda(cmd);
            }
        }
        v[0].pos.x += v[0].velocita;
        c.minima_m = std::min(c.minima_m, distanza(s.pos(), v[0].pos));
    }
    return c;
}

}  // namespace

TEST_CASE("armi dell'IA: il catalogo e le dottrine del filo") {
    const CatalogoInterazioni tutto = carica_interazioni(M688_DATI "/interazioni.toml");
    CHECK(tutto.tma.dottrine.count("nato") == 1);
    CHECK(tutto.armi.classi.count("akula") == 1);
    CHECK(ia().classi.size() >= 9);
    const DatiArmiIa& v3 = ia().classi.at("victor3");
    CHECK(v3.tubi_533 == 4);
    CHECK(v3.tubi_650 == 2);
    CHECK(v3.tubi.size() == 4u);
    CHECK_FALSE(v3.fonte.empty());
    const DottrinaFilo& r = dottrina_filo(ia(), Esperienza::recluta);
    const DottrinaFilo& e = dottrina_filo(ia(), Esperienza::esperto);
    CHECK_FALSE(r.rifiuta_esche);
    CHECK(e.rifiuta_esche);
    CHECK(e.correzione_s < r.correzione_s);
    CHECK(e.soglia_deg < r.soglia_deg);
}

TEST_CASE("armi dell'IA: carico per classe e anno, con le varianti dell'anno") {
    // il Victor III nel 1989: USET-80 nei tubi, 18 armi da 533, i 650 vuoti
    const CaricoIa v89 = carico_ia(carico(), ia(), "victor3", 1989);
    REQUIRE(v89.tubi.size() == 4u);
    CHECK(v89.tubi[0] == "uset80");
    CHECK(v89.totale("uset80") == 6);
    int tot = 0;
    for (const auto& t : v89.tubi) tot += !t.empty();
    for (const auto& [k, n] : v89.riserva) tot += n;
    CHECK(tot == 18);
    CHECK(v89.tubi_650_vuoti == 2);
    // nel 1978 l'USET-80 (1980) non c'e': diventa la predefinita SET-65
    const CaricoIa v78 = carico_ia(carico(), ia(), "victor3", 1978);
    CHECK(v78.totale("uset80") == 0);
    CHECK(v78.totale("set65") == v89.totale("set65") + v89.totale("uset80"));
    CHECK(v78.tubi[0] == "set65");
    // il Trafalgar prima del Tigerfish Mod 1 (1987): il Mk 48 della stessa famiglia
    const CaricoIa t85 = carico_ia(carico(), ia(), "trafalgar", 1985);
    CHECK(t85.tubi[0] == "mk48");
    CHECK(carico_ia(carico(), ia(), "trafalgar", 1989).tubi[0] == "tigerfish");
    // il 688 e l'Alfa dell'IA: i carichi standard di carico.toml, come il giocatore
    const CaricoIa la = carico_ia(carico(), ia(), "los_angeles", 1989);
    CHECK(la.fonte.find("carico.toml") != std::string::npos);
    CHECK(la.tubi.size() == 4u);
    CHECK(la.totale("mk48") > 10);
    CHECK(carico_ia(carico(), ia(), "alfa", 1989, "antinave").totale("type53_65k") > 0);
    // una classe senza dati: vuoto
    CHECK(carico_ia(carico(), ia(), "peschereccio", 1989).tubi.empty());
    // le what-if non si caricano mai
    CHECK(variante_dell_anno(carico(), "sea_lance", "usa", 1995).empty());
    CHECK(variante_dell_anno(carico(), "mk48_adcap", "usa", 1985) == "mk48");
}

TEST_CASE("armi dell'IA: la scelta dell'arma per bersaglio, portata e filo") {
    const CaricoIa v = carico_ia(carico(), ia(), "victor3", 1989);   // tubi: USET-80, USET-80, TEST-71M, 53-65K
    // contro un sottomarino a 10 km potendo guidare: il TEST-71M filoguidato
    auto a = scegli_arma_ia(v, carico(), BersaglioIa::sottomarino, 10000.0, true);
    REQUIRE(a);
    CHECK(a->variante == "test71m");
    CHECK(a->filoguidata);
    CHECK(a->siluro == "test71m");
    CHECK(a->tipo == TipoArmaCarico::siluro);
    CHECK(a->famiglia == "type53");
    // senza guida, o oltre la portata del TEST-71M (15 km): l'USET-80
    CHECK(scegli_arma_ia(v, carico(), BersaglioIa::sottomarino, 10000.0, false)->variante == "uset80");
    CHECK(scegli_arma_ia(v, carico(), BersaglioIa::sottomarino, 15500.0, true)->variante == "uset80");
    // contro una nave: il 53-65K a scia
    a = scegli_arma_ia(v, carico(), BersaglioIa::nave, 12000.0, true);
    REQUIRE(a);
    CHECK(a->variante == "type53_65k");
    // tutto fuori portata: la migliore lo stesso, segnata
    a = scegli_arma_ia(v, carico(), BersaglioIa::sottomarino, 40000.0, true);
    REQUIRE(a);
    CHECK_FALSE(a->in_portata);
    // nessuna arma contro il bersaglio: niente
    CaricoIa solo_scia;
    solo_scia.tubi = {"type53_65k"};
    CHECK_FALSE(scegli_arma_ia(solo_scia, carico(), BersaglioIa::sottomarino, 5000.0, true));
}

TEST_CASE("armi dell'IA: la guida a filo porta il siluro sul bersaglio e poi taglia") {
    const Corsa libera = corsa(std::nullopt, 35.0);
    const Corsa guidata = corsa(Esperienza::medio, 35.0);
    MESSAGE("Mk 48 lanciato 35 gradi fuori: senza guida passa a " << libera.minima_m << " m, guidato a " << guidata.minima_m
                                                                  << " m");
    CHECK_FALSE(libera.aggancio);
    CHECK(guidata.aggancio);
    CHECK(guidata.taglio);
    CHECK(guidata.minima_m < libera.minima_m);
}

TEST_CASE("armi dell'IA: esche rifiutate dall'esperto, taglio per evadere, vincoli del lanciatore") {
    ImpostazioniSiluro imp;
    imp.rotta = 0.0;
    Siluro s(*siluri().siluro("mk48"), imp, {0.0, 0.0}, 100.0, 0, 3);
    // un'eco stazionaria a 40 gradi dalla stima: la testa la riporta (la si mette a mano nei dati della testa con un passo
    // del siluro su un'esca vicina)
    SorgenteAcustica esca;
    esca.id = 9, esca.pos = {1200.0, 1500.0}, esca.quota = 100.0, esca.velocita = 0.0, esca.esca = true, esca.livello_db = 150.0;
    esca.forza_bersaglio_db = 10.0, esca.lunghezza_m = 0.0;
    imp.rte_m = 100.0;
    Siluro t(*siluri().siluro("mk48"), imp, {0.0, 0.0}, 100.0, 0, 3);
    std::vector<SorgenteAcustica> v = {esca};
    const Lanciatore lanciatore{{0.0, 0.0}, 4.0 * NODO, 0.0};
    for (int k = 0; k < 60 && t.testa().empty(); ++k) t.avanza(1.0, v, {}, lanciatore);
    REQUIRE_FALSE(t.testa().empty());
    SituazioneFilo sit;
    sit.t = 60.0;
    sit.siluro = &t;
    sit.bersaglio_stimato = Vec2{-1500.0, 4000.0};   // il bersaglio vero e' altrove
    GuidaFiloIa esperto(dottrina_filo(ia(), Esperienza::esperto)), recluta(dottrina_filo(ia(), Esperienza::recluta));
    const auto ce = esperto.passo(sit), cr = recluta.passo(sit);
    const auto rifiuta = [](const std::vector<ComandoFilo>& comandi) {
        for (const auto& c : comandi)
            if (c.tipo == ComandoFiloTipo::rifiuta && c.sorgente == 9) return true;
        return false;
    };
    REQUIRE(t.testa().front().stazionaria);
    CHECK(rifiuta(ce));
    CHECK_FALSE(rifiuta(cr));
    // il lanciatore deve evadere: si taglia, e i vincoli cadono
    GuidaFiloIa g(dottrina_filo(ia(), Esperienza::medio));
    SituazioneFilo w;
    w.siluro = &s;
    w.bersaglio_stimato = Vec2{0.0, 5000.0};
    g.passo(w);
    CHECK(g.vincoli().attivi);
    CHECK(g.vincoli().velocita_max_kn == doctest::Approx(Siluro::FILO_VELOCITA_MAX_KN));
    w.lanciatore_evade = true;
    const auto c = g.passo(w);
    REQUIRE(c.size() == 1u);
    CHECK(c.front().tipo == ComandoFiloTipo::taglia);
    CHECK(g.tagliato());
    CHECK_FALSE(g.vincoli().attivi);
    CHECK(g.passo(w).empty());   // dopo il taglio niente
    // senza stima ne' rilevamento non guida
    GuidaFiloIa h(dottrina_filo(ia(), Esperienza::medio));
    SituazioneFilo z;
    z.siluro = &s;
    CHECK(h.passo(z).empty());
    // col solo rilevamento mira alla linea: la rotta comandata e' vicina al rilevamento
    z.rilevamento = 60.0;
    const auto cz = h.passo(z);
    REQUIRE(cz.size() == 1u);
    CHECK(cz.front().tipo == ComandoFiloTipo::rotta);
    CHECK(std::fabs(normalizza_180(cz.front().valore - 60.0)) < 10.0);
}
