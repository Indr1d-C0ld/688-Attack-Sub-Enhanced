// Le serie della sandbox (docs/19 §6, V3): il riassunto serie.toml (app/serie_dati), la schermata delle serie
// (app/schermata_serie) e i vetrini pronti (data/missioni/prove/vetrini.toml, scritti da strumenti/scene_interazioni)
#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <string>

#include <toml.hpp>

#include "../app/schermata_serie.hpp"
#include "../app/serie_dati.hpp"
#include "grafica/font.hpp"
#include "nucleo/statistiche_sandbox.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

// tre partite note: la NATO perde 0, 1, 0 battelli e infligge 1, 1, 2; esiti 21, 2, 21
std::vector<StatisticheSandbox> tre_partite() {
    std::vector<StatisticheSandbox> v(3);
    const int perse[] = {0, 1, 0}, inflitte[] = {1, 1, 2}, esiti[] = {21, 2, 21};
    for (int k = 0; k < 3; ++k) {
        StatisticheSandbox& s = v[static_cast<std::size_t>(k)];
        s.durata_s = 3600.0;
        s.esito = esiti[k];
        s.esito_testo = esiti[k] == 21 ? "Tempo scaduto" : "Battello affondato";
        StatParte& n = s.parti[0x08];
        n.parte = 0x08;
        n.unita = 3;
        n.perse = perse[k];
        n.inflitte = inflitte[k];
        n.scambio = n.perse ? static_cast<double>(n.inflitte) / n.perse : n.inflitte;
        n.ritardo_medio_s = 60.0 * (k + 1);
        StatParte& p = s.parti[0x10];
        p.parte = 0x10;
        p.unita = 2;
        p.perse = inflitte[k];
        p.inflitte = perse[k];
        p.scambio = p.perse ? static_cast<double>(p.inflitte) / p.perse : p.inflitte;
    }
    return v;
}

}  // namespace

TEST_CASE("serie della sandbox: il riassunto si scrive e si rilegge uguale, partite nell'ordine dei semi") {
    DatiSerie d = dati_serie(tre_partite(), {"partita_5.m688tr", "partita_6.m688tr", "partita_7.m688tr"}, 5);
    d.missione = "PROVA_S5_CONVOGLIO";
    d.codice = "ABCDEF-GHIJKL-MNOPQR-STUVWX-YZ2345";
    d.data = "2026-10-27";
    REQUIRE(d.partite.size() == 3);
    CHECK(d.partite[0].seme == 5);
    CHECK(d.partite[2].seme == 7);
    CHECK(d.riassunto.esiti.at(21).volte == 2);
    CHECK(d.riassunto.esiti.at(2).volte == 1);
    const fs::path p = fs::temp_directory_path() / "m688_prova_serie.toml";
    scrivi_serie(p, d);
    const DatiSerie l = leggi_serie(p);
    CHECK(l.missione == d.missione);
    CHECK(l.codice == d.codice);
    CHECK(l.da == 5);
    CHECK(l.semi == 3);
    CHECK(l.riassunto.esiti.at(21).volte == 2);
    CHECK(l.riassunto.esiti.at(21).testo == "Tempo scaduto");
    CHECK(l.riassunto.parti.at(0x08).at("perse").media == doctest::Approx(1.0 / 3.0).epsilon(1e-4));
    CHECK(l.riassunto.parti.at(0x08).at("inflitte").max == doctest::Approx(2.0));
    CHECK(l.riassunto.parti.at(0x08).at("ritardo_medio_s").mediana == doctest::Approx(120.0));
    REQUIRE(l.partite.size() == 3);
    CHECK(l.partite[1].seme == 6);
    CHECK(l.partite[1].esito == 2);
    CHECK(l.partite[1].traccia == "partita_6.m688tr");
    CHECK(l.partite[1].numeri.at(0x08).at("perse") == doctest::Approx(1.0));
    CHECK(l.partite[2].numeri.at(0x10).at("perse") == doctest::Approx(2.0));
    fs::remove(p);
}

TEST_CASE("serie della sandbox: la schermata in tre pagine, il numero con le frecce, il replay di una partita") {
    DatiSerie d = dati_serie(tre_partite(), {"partita_1.m688tr", "partita_2.m688tr", "partita_3.m688tr"}, 1);
    d.missione = "PROVA_S5_CONVOGLIO";
    SchermataSerie s(M688_DATI, d, fs::path("/tmp/serie"));
    CHECK(s.pagina() == 0);
    REQUIRE(!s.numeri().empty());
    CHECK(s.numeri()[static_cast<std::size_t>(s.numero())] == "scambio");
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    Palette pal;
    s.palette(pal);
    for (int k = 0; k < SchermataSerie::PAGINE; ++k) {
        CAPTURE(k);
        t.riempi(0);
        s.disegna(t, f);
        int scritti = 0;
        for (int y = 34; y < 460; ++y)
            for (int x = 0; x < 640; x += 2) scritti += t.dati()[y * LARGHEZZA_TELA + x] != 0;
        CHECK(scritti > 300);
        Comandi tab;
        tab.tab = 1;
        CHECK_FALSE(s.comandi(tab, 0.1));
    }
    CHECK(s.pagina() == 0);
    // le frecce cambiano il numero degli istogrammi
    s.pagina(2);
    const int prima = s.numero();
    Comandi giu;
    giu.su_giu = -1;
    s.comandi(giu, 0.1);
    CHECK(s.numero() != prima);
    // un clic sulla seconda barra delle partite: la sua traccia
    s.disegna(t, f);
    Comandi clic;
    REQUIRE(s.barre().size() >= 2);
    const Rettangolo b = s.barre()[1];
    clic.clic = {{b.x + b.w / 2, b.y + b.h / 2}};
    CHECK(s.comandi(clic, 0.1));
    REQUIRE(s.traccia_scelta());
    CHECK(s.traccia_scelta()->filename() == "partita_2.m688tr");
    Comandi esc;
    esc.esc = true;
    CHECK(SchermataSerie(M688_DATI, d, "/tmp").comandi(esc, 0.1));
}

TEST_CASE("serie della sandbox: i vetrini pronti si leggono e le loro missioni ci sono") {
    const fs::path p = fs::path(M688_MISSIONI) / "prove" / "vetrini.toml";
    REQUIRE(fs::exists(p));
    const toml::table t = toml::parse_file(p.string());
    const auto* v = t["vetrino"].as_array();
    REQUIRE(v);
    CHECK(v->size() >= 7);
    int pronti = 0;
    for (const auto& n : *v) {
        const toml::table& x = *n.as_table();
        const std::string id = x["missione"].value_or(std::string());
        CAPTURE(id);
        CHECK(fs::exists(fs::path(M688_MISSIONI) / "prove" / (id + ".lua")));
        CHECK(!x["titolo"]["it"].value_or(std::string()).empty());
        CHECK(!x["mostra"]["en"].value_or(std::string()).empty());
        CHECK(x["durata_min"].value_or(0) > 0);
        CHECK(x["regolazioni"].as_table());
        pronti += x["pronto"].value_or(false);
    }
    CHECK(pronti >= 7);
}

TEST_CASE("serie della sandbox: la parte del luogo (M7) ha il suo nome nel riassunto") {
    CHECK(std::string(nome_parte_serie(0x18)) == "luogo");
    CHECK(parte_serie("luogo") == 0x18);
    CHECK(std::string(nome_parte_serie(0)) == "neutrali");
    CHECK(parte_da_flag(0x18 | 2) == 0x18);
    CHECK(parti_ostili(0x18, 0x08));
    CHECK_FALSE(parti_ostili(0x18, 0));
}
