#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "../app/data_missione.hpp"
#include "missioni/registro_missione.hpp"
#include "partita.hpp"
#include "regia_audio.hpp"
#include "video_apertura.hpp"

#include <thread>
#include <vector>

using namespace m688;

namespace {
const std::string DATI = M688_DATI;
const std::string REGIA = DATI + "/apertura.toml";
}  // namespace

TEST_CASE("video d'apertura: la regia dura 31 battute a 72, col ping in testa, il nero e il titolo in fondo") {
    const RegiaApertura r = carica_regia_apertura(REGIA);
    CHECK(r.durata_s() == doctest::Approx(31.0 * 4.0 * 60.0 / 72.0));
    REQUIRE(r.inquadrature.size() >= 3);
    CHECK(r.inquadrature.front().ping);
    CHECK(r.inquadrature.back().titolo);
    CHECK(r.inquadrature[r.inquadrature.size() - 2].battello.empty());
    CHECK(std::filesystem::exists(DATI + "/musica/fm/" + r.brano + ".mid"));
    // la regia e le scene dicono la stessa data: e' quella che vale, in partita, per sole, stagione e profilo del suono
    for (const char* scena : {"APERTURA_688", "APERTURA_ALFA"}) {
        const auto d = data_della_missione(DATI + "/apertura", scena);
        REQUIRE(d.has_value());
        CHECK(d->anno == r.inizio.anno);
        CHECK(d->mese == r.inizio.mese);
        CHECK(d->giorno == r.inizio.giorno);
        CHECK(d->ore_utc == doctest::Approx(r.inizio.ore_utc));
    }
}

TEST_CASE("video d'apertura: nessuna scritta d'aiuto: ogni strumento e' ritagliato, solo i periscopi hanno il tramonto") {
    for (const InquadraturaApertura& q : carica_regia_apertura(REGIA).inquadrature) {
        if (q.battello.empty()) continue;
        CHECK(q.ritaglio[2] > 0);
        CHECK(q.ritaglio[3] > 0);
        CHECK((q.tramonto > 0.0) == (q.stazione == 5));
    }
}

TEST_CASE("video d'apertura: l'inquadratura doppia tiene i colori di tutte e due le meta'") {
    Tela a, b, u;
    a.riempi(10);
    b.riempi(10);
    b.rettangolo_pieno(0, 0, 40, 40, 20);
    const Palette base = Palette::carica(DATI + "/palette/giorno.pal");
    Palette pa = base, pb = base;
    pa.imposta(10, 1, 2, 3);
    pb.imposta(10, 200, 100, 50);   // lo stesso indice, due colori
    pb.imposta(20, 9, 9, 9);        // un indice che usa solo b
    const Palette p = affianca(u, a, pa, 0, b, pb, 0);
    CHECK(p.rgb(u.at(0, 0)) == pa.rgb(10));
    CHECK(p.rgb(u.at(LARGHEZZA_TELA / 2, 100)) == pb.rgb(10));
    CHECK(p.rgb(u.at(LARGHEZZA_TELA / 2, 0)) == pb.rgb(20));
    CHECK(p.rgb(u.at(LARGHEZZA_TELA / 2 - 1, 0)) == pa.rgb(10));
}

TEST_CASE("video d'apertura: la coda del film tiene insieme immagine e suono") {
    const Palette pal;
    const auto suono = [](float v) { return std::vector<float>(100, v); };
    CodaFilm c(100, 4, 2);
    Tela t;
    CHECK_FALSE(c.partita());
    CHECK(c.campioni(10) == std::vector<float>(10, 0.0f));   // prima dell'avvio: silenzio, e l'orologio non si muove
    CHECK_FALSE(c.prendi());
    CHECK(c.metti(t, pal, 1, suono(0.1f)));
    CHECK_FALSE(c.partita());
    CHECK(c.metti(t, pal, 2, suono(0.2f)));
    CHECK(c.partita());
    CHECK(c.anticipo() == 2);
    auto f = c.prendi();   // il primo fotogramma e' al campione 0
    REQUIRE(f);
    CHECK(f->numero == 0);
    CHECK(f->inquadratura == 1);
    CHECK_FALSE(c.prendi());   // lo stesso: non e' nuovo
    const auto a = c.campioni(150);
    CHECK(a[0] == doctest::Approx(0.1f));
    CHECK(a[149] == doctest::Approx(0.2f));
    f = c.prendi();
    REQUIRE(f);
    CHECK(f->numero == 1);
    // coda vuota a film non finito: silenzio contato, e l'immagine aspetta il suono
    const auto b = c.campioni(100);
    CHECK(b[49] == doctest::Approx(0.2f));
    CHECK(b[50] == 0.0f);
    CHECK(c.silenzio() == 50);
    CHECK(c.metti(t, pal, 3, suono(0.3f)));
    CHECK(c.campioni(1)[0] == doctest::Approx(0.3f));
    f = c.prendi();
    REQUIRE(f);
    CHECK(f->numero == 2);
    CHECK_FALSE(c.conclusa());
    c.finita();
    (void)c.campioni(99);
    CHECK(c.conclusa());
    // chi prepara aspetta a coda piena, e si libera quando il film si ferma
    CodaFilm piena(100, 1, 1);
    CHECK(piena.metti(t, pal, 1, suono(0.0f)));
    std::thread fabbrica([&] { CHECK_FALSE(piena.metti(t, pal, 2, suono(0.0f))); });
    piena.ferma();
    fabbrica.join();
}

TEST_CASE("video d'apertura: i due battelli lanciano agli istanti della regia e il film finisce senza esiti") {
    const Risorse r = carica_risorse(DATI);
    VideoApertura v(r, DATI, carica_regia_apertura(REGIA), {});
    // l'audio senza disegnare: musica ed effetti, che nel nero del culmine tacciono del tutto e non toccano mai il limite
    RegiaAudio audio(DATI, 48000);
    v.imposta_audio(&audio);
    audio.musica(v.regia().brano, false);
    const int nero = static_cast<int>(v.regia().inquadrature.size()) - 2;
    float picco = 0.0f, picco_nero = 0.0f;
    bool passa_dal_nero = false;
    while (v.avanza(0.1, 0.0)) {
        const auto x = audio.produci(4800);
        for (const float c : x) {
            picco = std::max(picco, std::fabs(c));
            if (v.inquadratura() == nero) picco_nero = std::max(picco_nero, std::fabs(c));
        }
        passa_dal_nero = passa_dal_nero || v.inquadratura() == nero;
    }
    v.imposta_audio(nullptr);
    CHECK(passa_dal_nero);
    CHECK(picco_nero == 0.0f);
    CHECK(picco < 0.98f);
    CHECK(v.tempo() >= v.regia().durata_s() - 0.2);
    const auto lancio = [](const Missione& m, TipoEvento tipo) {
        double t = -1.0;
        for (const auto& e : m.eventi_missione())
            if (e.tipo == tipo && t < 0.0) t = e.tempo;
        return t;
    };
    // il 688 per primo, l'Alfa subito dopo, in tutte e due le scene: il giocatore dagli ordini, l'altro dalla regola
    CHECK(lancio(v.missione(false), TipoEvento::lancio_giocatore) == doctest::Approx(243.6).epsilon(0.01));
    CHECK(lancio(v.missione(true), TipoEvento::lancio_giocatore) == doctest::Approx(247.0).epsilon(0.01));
    CHECK(lancio(v.missione(false), TipoEvento::lancio_nemico) == doctest::Approx(247.0).epsilon(0.01));
    CHECK(lancio(v.missione(true), TipoEvento::lancio_nemico) == doctest::Approx(243.6).epsilon(0.01));
    CHECK_FALSE(v.missione(false).esito());   // il culmine non si risolve: nessuno e' colpito quando viene il nero
    CHECK_FALSE(v.missione(true).esito());
}
