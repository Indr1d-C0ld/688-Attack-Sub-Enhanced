// Acustica dell'artiglieria (G5): picco degli scoppi in superficie (dossier 22 §7.1 e §10), sorgenti della scena,
// salva di Mk 45 al sonar, suoni a bordo immerso e in plancia.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/bordo_audio.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Ambiente mare() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(3);
    return a;
}

double dbfs(const std::vector<float>& v, std::size_t da, std::size_t a) {
    a = std::min(a, v.size());
    double q = 0.0;
    for (std::size_t i = da; i < a; ++i) q += static_cast<double>(v[i]) * v[i];
    return 10.0 * std::log10(std::max(q / static_cast<double>(std::max<std::size_t>(a - da, 1)), 1e-30));
}

EventoArtiglieria evento(TipoEventoArtiglieria tipo, Vec2 pos, double kg = 3.5, double mm = 127.0) {
    EventoArtiglieria e;
    e.tipo = tipo;
    e.pos = pos;
    e.carica_kg = kg;
    e.calibro_mm = mm;
    e.tempo_s = 10.0;
    return e;
}

}  // namespace

TEST_CASE("artiglieria: il picco degli scoppi in superficie (22 §7.1 e §10)") {
    // tabella del §7.1, battello a 18 m: 5" 177 / 155 / 144 dB a 1 / 3 / 5 km; tolleranza 2 dB (arrotondamenti del
    // dossier e coefficienti di Cole riportati a tre cifre)
    CHECK(picco_scoppio_db(3.5, 1000.0, 18.0) == doctest::Approx(177.0).epsilon(2.0 / 177.0));
    CHECK(picco_scoppio_db(3.5, 3000.0, 18.0) == doctest::Approx(155.0).epsilon(2.0 / 155.0));
    CHECK(picco_scoppio_db(3.5, 5000.0, 18.0) == doctest::Approx(144.0).epsilon(2.0 / 144.0));
    CHECK(picco_scoppio_db(0.05, 1000.0, 18.0) == doctest::Approx(172.0).epsilon(2.0 / 172.0));   // 30 mm
    CHECK(picco_scoppio_db(70.0, 1000.0, 18.0) == doctest::Approx(180.0).epsilon(2.0 / 180.0));   // 16"
    // piu' profondo, piu' forte (lo specchio taglia meno)
    CHECK(picco_scoppio_db(3.5, 3000.0, 100.0) > picco_scoppio_db(3.5, 3000.0, 18.0) + 10.0);
    // il modello di propagazione e' tarato a 1 km (picco = livello della voce + 6 dB fra rms e picco - perdita): entro 3 dB
    const auto* v = catalogo().transitorio("art_scoppio_superficie");
    REQUIRE(v != nullptr);
    double tl = 0.0;
    int n = 0;
    for (double f = 1000.0; f <= 8000.0; f *= 1.25, ++n) tl += perdita_trasmissione(f, 1000.0, 0.5, 18.0, mare()).tl_db;
    CHECK(std::fabs(v->livello_db + 6.0 - tl / n - 177.0) < 3.0);
}

TEST_CASE("artiglieria: le sorgenti della scena") {
    const auto s = transitori_artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {3000.0, 0.0}), 990000, 1);
    REQUIRE(s.size() == 1);
    CHECK(s[0].tipo == "art_scoppio_superficie");
    CHECK(s[0].id == 990000);
    CHECK(s[0].quota_m == doctest::Approx(0.5));
    CHECK(s[0].guadagno_db == doctest::Approx(0.0));
    CHECK(transitori_artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {}, 0.75, 76.0), 1, 1)[0].guadagno_db ==
          doctest::Approx(2.4 * std::log10(0.75 / 3.5)));
    CHECK(transitori_artiglieria(evento(TipoEventoArtiglieria::passaggio, {}), 1, 1).empty());
    CHECK(transitori_artiglieria(evento(TipoEventoArtiglieria::illuminante, {}), 1, 1).empty());
    CHECK(std::string(nome_evento_artiglieria(TipoEventoArtiglieria::illuminante)) == "illuminante");
    auto raffica = evento(TipoEventoArtiglieria::raffica, {1500.0, 0.0}, 0.05, 30.0);
    raffica.colpi = 150;
    raffica.intervallo_s = 0.0133;
    const auto r = transitori_artiglieria(raffica, 1, 1);
    CHECK(r.size() == 4);   // 2 s a pezzi da 0,5
    CHECK(r[3].t0 == doctest::Approx(11.5));
    for (const auto t : {TipoEventoArtiglieria::partenza, TipoEventoArtiglieria::scoppio_aria, TipoEventoArtiglieria::tonfo,
                         TipoEventoArtiglieria::rimbalzo, TipoEventoArtiglieria::colpo_testa})
        for (const auto& x : transitori_artiglieria(evento(t, {}), 1, 1)) CHECK_MESSAGE(catalogo().transitorio(x.tipo) != nullptr, x.tipo);
    CHECK(std::string(nome_evento_artiglieria(TipoEventoArtiglieria::raffica)) == "raffica");
}

TEST_CASE("artiglieria: una salva di Mk 45 a 3 km si sente al sonar del 688 a 18 m") {
    Ricevitore r{{0.0, 0.0}, 18.0, 0.0, 5.0, catalogo().sensore("bqq5_sfera")};
    for (int k = 0; k < 4; ++k) {   // quattro colpi a 3 s
        auto e = evento(TipoEventoArtiglieria::scoppio_superficie, {3000.0 + 20.0 * k, 50.0 * k});
        e.tempo_s = 10.0 + 3.0 * k;
        const auto x = transitori_artiglieria(e, 990000 + 10 * k, 5)[0];
        const auto s = sorgente_transitoria(catalogo(), x, e.tempo_s + 0.2, 2.0);
        REQUIRE(s.has_value());
        CHECK(ascolta_transitorio(*s, {r}, mare()).sentito);
    }
    // anche il colpo di partenza della nave a 5 km, prima del proietto (22 §7.3: entro ~5-10 km)
    Ricevitore schiera{{0.0, 0.0}, 100.0, 90.0, 5.0, catalogo().sensore("tb16")};
    const auto p = transitori_artiglieria(evento(TipoEventoArtiglieria::partenza, {0.0, 5000.0}), 1, 1)[0];
    CHECK(ascolta_transitorio(*sorgente_transitoria(catalogo(), p, 10.2, 2.0), {schiera}, mare()).sentito);
}

TEST_CASE("artiglieria: a bordo, immerso e in plancia") {
    const int fs = 22050;
    const auto ascolta = [&](StazioneBordo st, double quota, const std::optional<EventoArtiglieria>& e, double secondi) {
        AmbienteBordo b;
        b.passo({}, {0.0, 0.0}, quota, mare(), st, RegimeBordo::silenzioso, 0.5);
        if (e) b.artiglieria(*e, {0.0, 0.0}, quota, 10.0);
        return b.passo({}, {0.0, 0.0}, quota, mare(), st, RegimeBordo::silenzioso, secondi);
    };
    // immerso a 18 m: lo scoppio di un 5" a 500 m e' una martellata sullo scafo, a 5 km niente
    const auto quiete = ascolta(StazioneBordo::centrale, 18.0, std::nullopt, 1.0);
    const auto vicino = ascolta(StazioneBordo::centrale, 18.0, evento(TipoEventoArtiglieria::scoppio_superficie, {500.0, 0.0}), 1.0);
    CHECK(dbfs(vicino, 0, 2205) > dbfs(quiete, 0, 2205) + 10.0);
    CHECK(ascolta(StazioneBordo::centrale, 18.0, evento(TipoEventoArtiglieria::scoppio_superficie, {5000.0, 0.0}), 1.0) == quiete);
    // la testa del periscopio colpita a quota periscopica: si sente certamente
    const auto testa = ascolta(StazioneBordo::centrale, 18.0, evento(TipoEventoArtiglieria::colpo_testa, {0.0, 0.0}), 2.0);
    CHECK(dbfs(testa, 0, 4410) > dbfs(quiete, 0, 2205) + 20.0);
    // in superficie, in plancia: il boato di un 5" a 5 km arriva dopo 5000 / 337 s, non prima
    const auto e = evento(TipoEventoArtiglieria::partenza, {5000.0, 0.0});
    const auto plancia = ascolta(StazioneBordo::plancia, 0.0, e, 17.0);
    const auto fondo = ascolta(StazioneBordo::plancia, 0.0, std::nullopt, 17.0);
    const auto arrivo = static_cast<std::size_t>(5000.0 / (331.3 + 6.06) * fs);
    CHECK(dbfs(plancia, 0, arrivo - fs) == doctest::Approx(dbfs(fondo, 0, arrivo - fs)));
    CHECK(dbfs(plancia, arrivo, arrivo + fs / 2) > dbfs(fondo, arrivo, arrivo + fs / 2) + 10.0);
    // dentro lo scafo in superficie: lo stesso boato molto piu' debole
    const auto dentro = ascolta(StazioneBordo::centrale, 0.0, e, 17.0);
    const auto dentro_fondo = ascolta(StazioneBordo::centrale, 0.0, std::nullopt, 17.0);
    CHECK(dbfs(dentro, arrivo, arrivo + fs / 2) - dbfs(dentro_fondo, arrivo, arrivo + fs / 2) <
          dbfs(plancia, arrivo, arrivo + fs / 2) - dbfs(fondo, arrivo, arrivo + fs / 2));
    // il proietto che passa a 50 m: lo schiocco in plancia
    auto passa = evento(TipoEventoArtiglieria::passaggio, {50.0, 0.0});
    passa.velocita_m_s = 800.0;
    const auto schiocco = ascolta(StazioneBordo::plancia, 0.0, passa, 1.0);
    const auto pausa = static_cast<std::size_t>(50.0 / 337.4 * fs);
    CHECK(dbfs(schiocco, pausa, pausa + 441) > dbfs(fondo, pausa, pausa + 441) + 10.0);
    CHECK(std::string(nome_stazione_bordo(StazioneBordo::plancia)) == "plancia");
}
