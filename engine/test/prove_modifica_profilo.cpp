#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <tuple>

#include "missioni/missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/modifica_profilo.hpp"

using namespace m688;

// Lo strato termico che cambia (D4 del vetrino di Dio, docs/28 §3.3): la geometria del profilo e l'effetto sui raggi

namespace {
const ClimaTeatro& faroe() {
    static const ClimaTeatro c = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    return c;
}
double perdita(const ProfiloSuono& p, double zs, double zr, double d = 8000.0) {
    Ambiente a;
    a.profilo = &p;
    a.fondale_m = 2500.0;
    a.fondo = TipoFondo::fango;
    return perdita_trasmissione(1000.0, d, zs, zr, a).tl_db;
}
}  // namespace

TEST_CASE("modifica del profilo: a frazione zero il profilo di prima, bit per bit") {
    const ProfiloSuono& p = faroe().profilo(2);
    for (auto t : {ModificaStrato::Tipo::sposta, ModificaStrato::Tipo::togli, ModificaStrato::Tipo::rinforza, ModificaStrato::Tipo::canale}) {
        const ProfiloSuono q = modifica_profilo(p, {t, 100.0}, 0.0);
        CHECK(q.z_m == p.z_m);
        CHECK(q.c_m_s == p.c_m_s);
        CHECK(q.sld_m == p.sld_m);
    }
}

TEST_CASE("modifica del profilo: togliere lo strato, spostarlo, rinforzarlo; l'asse del canale") {
    const ProfiloSuono& p = faroe().profilo(2);
    REQUIRE(p.sld_m > 100.0);   // febbraio: lo strato d'inverno a 225 m
    CHECK(modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0}).sld_m == 0.0);
    CHECK(modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0}).gradiente_sotto < 0.0);
    for (double v : {50.0, 100.0, 300.0, 390.0}) {
        CAPTURE(v);
        const ProfiloSuono q = modifica_profilo(p, {ModificaStrato::Tipo::sposta, v});
        CHECK(q.sld_m == doctest::Approx(v).epsilon(0.01));
        CHECK(q.gradiente_strato > 0.0);
        CHECK(q.gradiente_sotto < 0.0);
        // il termoclino si porta dietro l'asse, al piu' di quanto si e' spostato lui; l'acqua profonda resta quella
        CHECK(std::fabs(q.asse_m - p.asse_m) <= std::fabs(v - p.sld_m) + 50.0);
        CHECK(q.c(5000.0) == doctest::Approx(p.c(5000.0)));
    }
    CHECK(modifica_profilo(p, {ModificaStrato::Tipo::sposta, 800.0}).sld_m == doctest::Approx(390.0).epsilon(0.01));
    const ProfiloSuono piu = modifica_profilo(p, {ModificaStrato::Tipo::rinforza, 3.0}), meno = modifica_profilo(p, {ModificaStrato::Tipo::rinforza, 0.5});
    CHECK(piu.gradiente_sotto < p.gradiente_sotto);
    CHECK(meno.gradiente_sotto > p.gradiente_sotto);
    CHECK(piu.sld_m == doctest::Approx(p.sld_m));
    CHECK(meno.sld_m == doctest::Approx(p.sld_m));
    const ProfiloSuono asse = modifica_profilo(p, {ModificaStrato::Tipo::canale, 1000.0});
    CHECK(asse.asse_m == doctest::Approx(1000.0).epsilon(0.05));
    CHECK(asse.sld_m == doctest::Approx(p.sld_m));
    CHECK(modifica_profilo(p, {ModificaStrato::Tipo::canale, 10.0}).asse_m >= p.sld_m + 40.0);   // mai sopra lo strato
    // la transizione sta fra i due
    const ProfiloSuono tutto = modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0}), meta = modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0}, 0.5);
    CHECK(meta.c(0.0) == doctest::Approx(0.5 * (p.c(0.0) + tutto.c(0.0))));
}

TEST_CASE("modifica del profilo: i raggi lo sentono. Attraverso lo strato si perde, sotto lo strato spostato no") {
    const ProfiloSuono& p = faroe().profilo(2);
    // una sorgente a 50 m e chi ascolta a 300 m, a 8 km: oggi attraverso lo strato (225 m). Lo strato a 390 m li mette
    // tutti e due dentro il condotto
    const double attraverso = perdita(p, 50.0, 300.0), dentro = perdita(modifica_profilo(p, {ModificaStrato::Tipo::sposta, 390.0}), 50.0, 300.0);
    MESSAGE("50 m -> 300 m a 8 km: strato a 225 m ", attraverso, " dB, strato a 390 m ", dentro, " dB");
    CHECK(dentro < attraverso - 3.0);
    // viceversa: lo strato a 100 m mette fra loro il termoclino
    const double sopra = perdita(p, 50.0, 200.0), separati = perdita(modifica_profilo(p, {ModificaStrato::Tipo::sposta, 100.0}), 50.0, 200.0);
    MESSAGE("50 m -> 200 m a 8 km: strato a 225 m ", sopra, " dB, strato a 100 m ", separati, " dB");
    CHECK(separati > sopra + 3.0);
    // senza strato il condotto non c'e': da 50 a 50 m si perde di piu'
    const double condotto = perdita(p, 50.0, 50.0, 20000.0), senza = perdita(modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0}), 50.0, 50.0, 20000.0);
    MESSAGE("50 m -> 50 m a 20 km: col condotto ", condotto, " dB, senza ", senza, " dB");
    CHECK(senza > condotto + 3.0);
}

TEST_CASE("modifica del profilo: tutti i teatri e tutti i mesi danno un profilo sano") {
    double peggio_sposta = 0.0, peggio_asse = 0.0;
    std::string dove_sposta, dove_asse;
    int togli_non_zero = 0, profili = 0, polari = 0;
    for (const auto& f : std::filesystem::directory_iterator(M688_DATI "/teatri")) {
        if (f.path().string().find(".suono.toml") == std::string::npos) continue;
        const ClimaTeatro c = carica_clima(f.path().string());
        for (int mese = 1; mese <= 12; ++mese) {
            const ProfiloSuono& p = c.profilo(mese);
            ++profili;
            const auto sano = [&](const ProfiloSuono& q) {
                CHECK(q.z_m.size() == q.c_m_s.size());
                bool ok = q.z_m.front() == 0.0;
                double c_min = 1e9, c_max = 0.0;
                for (std::size_t i = 0; i < q.z_m.size(); ++i) {
                    ok = ok && std::isfinite(q.c_m_s[i]);
                    c_min = std::min(c_min, q.c_m_s[i]);
                    c_max = std::max(c_max, q.c_m_s[i]);
                    if (i > 0) ok = ok && q.z_m[i] > q.z_m[i - 1];
                }
                CAPTURE(c.teatro);
                CAPTURE(mese);
                CAPTURE(c_min);
                CAPTURE(c_max);
                CAPTURE(q.z_m.front());
                CAPTURE(p.sld_m);
                CAPTURE(p.gradiente_sotto);
                CHECK(ok);
                // l'acqua resta quella del teatro: togliere lo strato scalda la superficie di qualche m/s, non di piu'
                const auto [p_min, p_max] = std::minmax_element(p.c_m_s.begin(), p.c_m_s.end());
                CHECK(c_min > *p_min - 10.0);
                CHECK(c_max < *p_max + 40.0);
            };
            const ProfiloSuono senza = modifica_profilo(p, {ModificaStrato::Tipo::togli, 0.0});
            sano(senza);
            togli_non_zero += senza.sld_m != 0.0;
            if (!modifica_applicabile(p, ModificaStrato::Tipo::sposta)) {   // le acque polari: niente da spostare
                ++polari;
                CHECK(modifica_profilo(p, {ModificaStrato::Tipo::sposta, 100.0}).c_m_s == p.c_m_s);
                CHECK(modifica_profilo(p, {ModificaStrato::Tipo::rinforza, 3.0}).c_m_s == p.c_m_s);
            }
            for (double v : {30.0, 150.0, 350.0}) {
                if (!modifica_applicabile(p, ModificaStrato::Tipo::sposta)) break;
                const ProfiloSuono q = modifica_profilo(p, {ModificaStrato::Tipo::sposta, v});
                sano(q);
                if (v < p.z_m.back() && std::fabs(q.sld_m - v) > peggio_sposta) {   // il Baltico ha dati fino a cento metri
                    peggio_sposta = std::fabs(q.sld_m - v);
                    dove_sposta = c.teatro + " mese " + std::to_string(mese) + " v " + std::to_string(v) + " sld " + std::to_string(p.sld_m) + " -> " + std::to_string(q.sld_m);
                }
            }
            sano(modifica_profilo(p, {ModificaStrato::Tipo::rinforza, 3.0}));
            sano(modifica_profilo(p, {ModificaStrato::Tipo::rinforza, 0.3}));
            const ProfiloSuono a = modifica_profilo(p, {ModificaStrato::Tipo::canale, 1200.0});
            sano(a);
            if (modifica_applicabile(p, ModificaStrato::Tipo::canale) && 1200.0 < p.z_m.back() && std::fabs(a.asse_m - 1200.0) > peggio_asse) {
                peggio_asse = std::fabs(a.asse_m - 1200.0);
                dove_asse = c.teatro + " mese " + std::to_string(mese) + " asse " + std::to_string(p.asse_m) + " -> " + std::to_string(a.asse_m);
            }
        }
    }
    MESSAGE(profili, " profili, ", polari, " polari; lo strato piu' lontano dalla quota chiesta: ", peggio_sposta, " m (", dove_sposta, "); l'asse piu' lontano: ", peggio_asse, " m (", dove_asse, ")");
    CHECK(profili >= 12);
    CHECK(togli_non_zero == 0);
    CHECK(peggio_sposta <= 30.0);
    CHECK(peggio_asse <= 100.0);
}

// --- nella missione: il profilo del percorso e la copertura ---

namespace {
std::string scena_d4() {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_d4.lua";
    const std::string sub = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
    std::ofstream(f) << "return { id = 'PROVA_D4', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 2, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 50, rotta = 0, velocita_kn = 0, " << sub << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 6000, y = 0 }, quota_m = 300, rotta = 0,"
                        "  velocita_kn = 5, " << sub << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}
}  // namespace

TEST_CASE("modifica del profilo nella missione: la zona, i passi della transizione con i puntatori stabili, il ritorno") {
    const std::string f = scena_d4();
    Missione m(f, 1, false);
    m.acustica_enhanced(M688_DATI);
    m.meteo_enhanced(M688_DATI);
    m.avanza(1.0);
    const Vec2 qui{0.0, 0.0}, la{6000.0, 0.0}, lontano{200000.0, 0.0};
    const ProfiloSuono* base = m.profilo_in(qui, la);
    REQUIRE(base != nullptr);
    CHECK(m.profilo_in(lontano, lontano) == base);
    CHECK(m.sld_in(qui) == doctest::Approx(base->sld_m));
    CHECK(m.strato_applicabile(ModificaStrato::Tipo::sposta));
    // lo strato a 390 m in un cerchio di 20 km, fra 10 s, in 80 s, per 100 s
    Missione::ZonaStrato z;
    z.modifica = {ModificaStrato::Tipo::sposta, 390.0};
    z.raggio_m = 20000.0;
    z.t0 = m.tempo() + 10.0;
    z.transizione_s = 80.0;
    z.durata_s = 100.0;
    m.aggiungi_strato(z);
    CHECK(m.profilo_in(qui, la) == base);    // non ancora
    m.avanza(50.0);
    const ProfiloSuono* meta = m.profilo_in(qui, la);
    CHECK(meta != base);
    CHECK(m.profilo_in(qui, la) == meta);    // lo stesso puntatore, le cache dei raggi non ricalcolano
    CHECK(m.profilo_in(lontano, lontano) == base);
    m.avanza(45.0);
    const ProfiloSuono* pieno = m.profilo_in(qui, la);
    CHECK(pieno != meta);
    CHECK(pieno->sld_m == doctest::Approx(390.0).epsilon(0.01));
    CHECK(m.sld_in(qui) == doctest::Approx(390.0).epsilon(0.01));
    CHECK(m.sld_in(lontano) == doctest::Approx(base->sld_m));
    m.avanza(190.0);
    CHECK(m.profilo_in(qui, la) == base);    // finita: il profilo di prima
    std::filesystem::remove(f);
}

TEST_CASE("copertura nella missione: senza cambi quella del meteo; la polynya che si chiude su un battello; deterministico") {
    const std::string f = scena_d4();
    Missione m(f, 1, false);
    m.acustica_enhanced(M688_DATI);
    m.meteo_enhanced(M688_DATI);
    m.avanza(1.0);
    const Vec2 qui{0.0, 0.0};
    CHECK(m.copertura_ghiaccio(qui) == 0.0);   // febbraio alle Faroe: mare libero
    CampoGhiaccio* g = m.ghiaccio_da_cambiare();
    REQUIRE(g != nullptr);
    CHECK_FALSE(g->modificato());
    CHECK_FALSE(m.sotto_banchisa(0));
    ZonaGhiaccio z;
    z.centro = qui;
    z.raggio_m = 2000.0;
    z.apri = false;
    z.t0 = m.tempo();
    z.transizione_s = 100.0;
    g->forza_zona(z);
    m.avanza(50.0);
    CHECK(m.copertura_ghiaccio(qui) == doctest::Approx(0.5).epsilon(0.05));
    CHECK(g->in(qui, m.tempo()).nilas);
    CHECK(m.sotto_banchisa(0));                // anche il nilas: periscopio e alberi non lo sfondano
    m.avanza(100.0);
    CHECK(m.copertura_ghiaccio(qui) == 1.0);
    CHECK_FALSE(g->in(qui, m.tempo()).nilas);
    CHECK(m.sotto_banchisa(0));
    CHECK(m.copertura_ghiaccio({5000.0, 0.0}) == 0.0);
    CHECK_FALSE(m.sotto_banchisa(3));
    // lo stesso cambiamento, la stessa partita: lo strato e la zona a istanti fissi
    const auto giro = [&] {
        Missione x(f, 7, false);
        x.acustica_enhanced(M688_DATI);
        x.meteo_enhanced(M688_DATI);
        x.avanza(1.0);
        Missione::ZonaStrato s;
        s.modifica = {ModificaStrato::Tipo::togli, 0.0};
        s.t0 = 60.0;
        s.transizione_s = 120.0;
        x.aggiungi_strato(s);
        ZonaGhiaccio c = z;
        c.t0 = 90.0;
        c.centro = {6000.0, 0.0};
        x.ghiaccio_da_cambiare()->forza_zona(c);
        for (int i = 0; i < 4 * 600; ++i) x.avanza(0.25);
        return std::make_tuple(x.impronta_contatti(), x.unita().at(3).pos.x, x.unita().at(3).pos.y);
    };
    CHECK(giro() == giro());
    std::filesystem::remove(f);
}
