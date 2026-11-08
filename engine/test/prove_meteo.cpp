#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nucleo/meteo.hpp"

using namespace m688;

// Meteo e mare dell'Enhanced (docs/15_meteo_e_mare.md, M1): scala unica, dati, generatore deterministico, cento anni
// sintetici contro le tabelle ERA5 di meteo.toml, eventi rari, lampi, previsione del 1989, meteo nel punto

namespace {

const DatiMeteo& dati() {
    static const DatiMeteo d = carica_meteo(M688_DATI "/meteo.toml");
    return d;
}

const char* stagione(int mese) {
    return mese == 12 || mese <= 2 ? "inverno" : mese <= 5 ? "primavera" : mese <= 8 ? "estate" : "autunno";
}

// Statistiche di cento «anni» di un mese (100 x 30 giorni di ore)
struct Clima {
    double vento = 0, bft7_base = 0, bft8_base = 0, hs = 0, bagnate = 0, nebbia = 0;
    std::array<double, 4> classi{};      // Douglas 0-2, 3-4, 5-6, 7+
    std::array<double, 4> markov{};      // persistenza a 6 h delle classi di vento (calmo, moderato, forte, burrasca)
    double tau_vento = 0;
};

Clima clima(const std::string& teatro, int mese, std::uint64_t seme) {
    constexpr long ORE = 100 * 30 * 24;
    Meteo m(dati(), teatro, mese, 0.0, seme);
    std::vector<double> u(ORE);
    std::vector<int> cv(ORE);
    Clima c;
    long base = 0;
    for (long h = 0; h < ORE; ++h) {
        m.avanza(static_cast<double>(h) * 3600.0);
        const StatoMeteo& s = m.stato();
        u[static_cast<std::size_t>(h)] = s.vento_m_s;
        cv[static_cast<std::size_t>(h)] = s.vento_m_s < 5.5 ? 0 : s.vento_m_s < 10.8 ? 1 : s.vento_m_s < 17.2 ? 2 : 3;
        c.vento += s.vento_m_s;
        if (s.evento.empty()) { ++base; c.bft7_base += s.vento_m_s >= 13.9; c.bft8_base += s.vento_m_s >= 17.2; }
        c.hs += s.hs_m();
        c.bagnate += s.pioggia_mm_h + s.neve_mm_h >= 0.1;
        c.nebbia += s.nebbia != Nebbia::nessuna;
        const int d = s.stato_mare();
        c.classi[static_cast<std::size_t>(d <= 2 ? 0 : d <= 4 ? 1 : d <= 6 ? 2 : 3)] += 1;
    }
    const double n = ORE;
    c.vento /= n;
    c.bft7_base *= 100.0 / static_cast<double>(std::max(base, 1L));
    c.bft8_base *= 100.0 / static_cast<double>(std::max(base, 1L)); c.hs /= n;
    c.bagnate *= 100.0 / n; c.nebbia *= 100.0 / n;
    for (double& x : c.classi) x *= 100.0 / n;
    std::array<double, 4> rimasti{}, righe{};
    for (long h = 0; h + 6 < ORE; ++h) {
        const auto a = static_cast<std::size_t>(cv[static_cast<std::size_t>(h)]);
        righe[a] += 1;
        rimasti[a] += cv[static_cast<std::size_t>(h + 6)] == cv[static_cast<std::size_t>(h)];
    }
    for (std::size_t k = 0; k < 4; ++k) c.markov[k] = righe[k] > 0 ? 100.0 * rimasti[k] / righe[k] : 0.0;
    // scala di decorrelazione: il primo ritardo con autocorrelazione sotto 1/e
    double mu = 0, var = 0;
    for (const double x : u) mu += x;
    mu /= n;
    for (const double x : u) var += (x - mu) * (x - mu);
    for (int r = 1; r < 200; ++r) {
        double cov = 0;
        for (long h = 0; h + r < ORE; ++h) cov += (u[static_cast<std::size_t>(h)] - mu) * (u[static_cast<std::size_t>(h + r)] - mu);
        if (cov / var < std::exp(-1.0)) { c.tau_vento = r; break; }
    }
    return c;
}

}  // namespace

TEST_CASE("meteo: scala unica di Douglas e Beaufort") {
    CHECK(classe_douglas(0.0) == 0);
    CHECK(classe_douglas(0.05) == 1);
    CHECK(classe_douglas(0.3) == 2);
    CHECK(classe_douglas(1.0) == 3);
    CHECK(classe_douglas(2.0) == 4);
    CHECK(classe_douglas(3.0) == 5);
    CHECK(classe_douglas(5.0) == 6);
    CHECK(classe_douglas(20.0) == 9);
    for (int s = 0; s <= 9; ++s) CHECK(classe_douglas(centro_douglas(s)) == s);
    CHECK(centro_douglas(3) == doctest::Approx(0.875));
    CHECK(beaufort(0.1) == 0);
    CHECK(beaufort(17.2) == 8);
    CHECK(beaufort(35.0) == 12);
    StatoMeteo s;
    s.hs_vento_m = 3.0;
    s.hs_swell_m = 4.0;
    CHECK(s.hs_m() == doctest::Approx(5.0));
    CHECK(s.stato_mare() == 6);
    s.vento_m_s = 10.0;
    CHECK(s.schiuma() == doctest::Approx(0.0099).epsilon(0.05));   // Monahan: ~1% a 10 m/s
    const StatoMeteo f = meteo_semplificato();
    CHECK(f.stato_mare() == 3);
    CHECK(f.pioggia_mm_h == 0.0);
}

TEST_CASE("meteo: i dati dei sette teatri") {
    const auto& d = dati();
    REQUIRE(d.teatri.size() == 7);
    for (const char* t : {"faroe", "barents", "gibilterra", "otranto", "kunashir", "celtica", "baltico"}) {
        CAPTURE(t);
        const TeatroMeteo* T = d.teatro(t);
        REQUIRE(T);
        CHECK(T->stagioni.size() == 4);
        CHECK(T->regimi.size() >= 4);
        CHECK_FALSE(T->punti.empty());
        for (const auto& [nome, R] : T->regimi) CHECK(regime_da_nome(nome).has_value());
    }
    CHECK(d.teatro("faroe")->mesi[0].weibull_k > 1.0);
    CHECK(d.teatro("baltico")->inverni_rigidi.size() == 3);
    CHECK(d.teatro("barents")->eventi.count("polar_low") == 1);
    CHECK_FALSE(d.teatro("barents")->eventi.at("polar_low").prevista);
    CHECK(d.generale.previsione_orizzonti_h.size() == 4);
}

TEST_CASE("meteo: deterministico, un generatore per processo") {
    Meteo a(dati(), "faroe", 1, 0.0, 42), b(dati(), "faroe", 1, 0.0, 42), c(dati(), "faroe", 1, 0.0, 43);
    bool diversi = false;
    for (double t = 0.0; t < 5.0 * 86400.0; t += 1800.0) {
        a.avanza(t);
        b.avanza(t);
        c.avanza(t);
        CHECK(a.stato().vento_m_s == b.stato().vento_m_s);
        CHECK(a.stato().hs_m() == b.stato().hs_m());
        CHECK(a.stato().visibilita_m == b.stato().visibilita_m);
        diversi = diversi || a.stato().vento_m_s != c.stato().vento_m_s;
    }
    CHECK(diversi);
    // interpolato al secondo, senza salti fra un'ora e l'altra
    Meteo m(dati(), "faroe", 1, 0.0, 7);
    m.avanza(3600.0 * 10);
    const double v0 = m.stato().vento_m_s;
    m.avanza(3600.0 * 10 + 1.0);
    CHECK(std::abs(m.stato().vento_m_s - v0) < 0.1);
    // la previsione chiesta in mezzo non cambia il meteo che viene
    Meteo p(dati(), "faroe", 1, 0.0, 42);
    (void)p.previsione(0.0, 72.0 * 3600.0);
    (void)p.lampi(0.0, 86400.0);
    for (double t = 0.0; t < 5.0 * 86400.0; t += 1800.0) {
        p.avanza(t);
        a.avanza(t);
        CHECK(p.stato().vento_m_s == a.stato().vento_m_s);
    }
}

TEST_CASE("meteo: cento anni sintetici contro le tabelle ERA5") {
    // tolleranze del dossier 21 §4 (±20%); le code e le classi rare in assoluto (punti percentuali)
    for (const auto& [teatro, T] : dati().teatri)
        for (const int mese : {1, 7}) {
            const MeseMeteo& M = T.mesi[static_cast<std::size_t>(mese - 1)];
            const StagioneMeteo& S = T.stagioni.at(stagione(mese));
            const Clima c = clima(teatro, mese, 1000 + static_cast<std::uint64_t>(mese));
            CAPTURE(teatro);
            CAPTURE(mese);
            CHECK(c.vento == doctest::Approx(M.vento_media_m_s).epsilon(0.10));
            // vento forte e burrasche fuori dagli eventi rari (che si aggiungono apposta, §3.3); Bft 8 con la
            // correzione delle code di ERA5
            if (M.bft7_pct >= 2.0) CHECK(c.bft7_base == doctest::Approx(M.bft7_pct).epsilon(0.20));
            else CHECK(c.bft7_base < 3.0);
            const double bft8 = M.bft8_pct * T.correzione_burrasche;
            if (bft8 >= 2.0) CHECK(c.bft8_base == doctest::Approx(bft8).epsilon(0.30));
            else CHECK(c.bft8_base < 2.5);
            if (M.ghiaccio_pct < 5.0) CHECK(c.hs == doctest::Approx(M.hs_media_m).epsilon(0.20));   // ERA5: l'acqua libera
            // classi Douglas raggruppate: la dominante col 20%, le altre entro 15 punti
            const std::array<double, 4> dati_classi{M.douglas_pct[0] + M.douglas_pct[1] + M.douglas_pct[2],
                                                    M.douglas_pct[3] + M.douglas_pct[4], M.douglas_pct[5] + M.douglas_pct[6],
                                                    M.douglas_pct[7] + M.douglas_pct[8] + M.douglas_pct[9]};
            const auto dominante = static_cast<std::size_t>(std::max_element(dati_classi.begin(), dati_classi.end()) - dati_classi.begin());
            if (M.ghiaccio_pct < 5.0)
                for (std::size_t k = 0; k < 4; ++k) {
                    CAPTURE(k);
                    if (k == dominante) CHECK(c.classi[k] == doctest::Approx(dati_classi[k]).epsilon(0.25));
                    else CHECK(std::abs(c.classi[k] - dati_classi[k]) < 15.0);
                }
            if (M.precipitazione_pct >= 5.0) CHECK(c.bagnate == doctest::Approx(M.precipitazione_pct).epsilon(0.25));
            // persistenze: la scala del vento e la catena di Markov a 6 h (nota A §6.2-6.3)
            CHECK(c.tau_vento == doctest::Approx(S.tau_vento_h).epsilon(0.30));
            for (std::size_t k = 0; k < 3; ++k)
                if (S.quote_classi_pct[k] >= 5.0 && !std::isnan(S.markov6h_pct[k][k])) {
                    CAPTURE(k);
                    CHECK(std::abs(c.markov[k] - S.markov6h_pct[k][k]) < 20.0);
                }
        }
}

TEST_CASE("meteo: nebbia, ghiaccio e mari chiusi per teatro") {
    // la nebbia delle Curili d'estate, nessuna nebbia a Otranto d'inverno
    const Clima curili = clima("kunashir", 7, 5);
    CHECK(curili.nebbia > 25.0);
    const Clima otranto = clima("otranto", 1, 5);
    CHECK(otranto.nebbia < 2.0);
    // il pack delle Curili a marzo smorza il mare
    Meteo m(dati(), "kunashir", 3, 0.0, 3);
    m.avanza(3600.0 * 50);
    CHECK(m.stato().ghiaccio > 0.5);
    // il Baltico ghiaccia negli inverni rigidi
    Meteo b(dati(), "baltico", 2, 0.0, 3), b85(dati(), "baltico", 2, 0.0, 3);
    b85.imposta_anno(1985);
    b.avanza(3600.0 * 50);
    b85.avanza(3600.0 * 50);
    CHECK(b85.stato().ghiaccio > b.stato().ghiaccio);
    CHECK(b85.stato().hs_m() < b.stato().hs_m());
}

TEST_CASE("meteo: eventi rari nella loro stagione, polar low nel Barents") {
    int barents = 0, luglio = 0;
    std::set<double> inizi;
    for (const int mese : {3, 7}) {
        Meteo m(dati(), "barents", mese, 0.0, 11);
        std::string prima;
        for (long h = 0; h < 50L * 30 * 24; ++h) {
            m.avanza(static_cast<double>(h) * 3600.0);
            const std::string& e = m.stato().evento;
            if (e == "polar_low" && prima != "polar_low") (mese == 3 ? barents : luglio) += 1;
            if (e == "polar_low") CHECK(m.stato().visibilita_m <= 1500.0);
            prima = e;
        }
    }
    // 0,75 al mese, 1,5 volte a marzo (mese_massimo): circa 56 in 50 mesi
    CHECK(barents > 25);
    CHECK(barents < 110);
    CHECK(luglio == 0);
}

TEST_CASE("meteo: celle temporalesche e lampi deterministici") {
    Meteo m(dati(), "baltico", 7, 0.0, 21);   // temporali estivi del Baltico: 6-12 giorni a stagione
    m.avanza(0.0);
    int celle = 0;
    double t_cella = -1.0;
    for (long h = 0; h < 91L * 24; ++h) {
        const auto v = m.celle(static_cast<double>(h) * 3600.0 + 1800.0);
        if (!v.empty() && t_cella < 0.0) t_cella = static_cast<double>(h) * 3600.0 + 1800.0;
        celle += static_cast<int>(v.size());
    }
    REQUIRE(t_cella >= 0.0);
    CHECK(celle > 0);
    const auto c = m.celle(t_cella);
    REQUIRE_FALSE(c.empty());
    CHECK(c[0].raggio_m >= 5000.0);
    CHECK(c[0].raggio_m <= 15000.0);
    // i lampi di un'ora sono quelli dei suoi due mezzi, comunque si chieda
    const auto tutti = m.lampi(t_cella - 1800.0, t_cella + 1800.0);
    const auto a = m.lampi(t_cella - 1800.0, t_cella);
    const auto b = m.lampi(t_cella, t_cella + 1800.0);
    REQUIRE_FALSE(tutti.empty());
    CHECK(tutti.size() == a.size() + b.size());
    double colpi = 0.0;
    int mare = 0;
    for (const auto& l : tutti) {
        CHECK(l.colpi >= 1);
        CHECK(l.t >= t_cella - 1800.0);
        CHECK(l.t < t_cella + 1800.0);
        colpi += l.colpi;
        mare += l.nube_mare;
    }
    CHECK(colpi / static_cast<double>(tutti.size()) > 2.0);
    CHECK(colpi / static_cast<double>(tutti.size()) < 5.5);
    CHECK(mare < static_cast<int>(tutti.size()));
    // nel punto sotto la cella: rovesci, raffiche e lampi
    m.avanza(t_cella);
    const StatoMeteo sotto = m.in(c[0].pos);
    CHECK(sotto.lampi_al_minuto > 0.0);
    CHECK(sotto.pioggia_mm_h > 5.0);
    CHECK(sotto.visibilita_m < m.stato().visibilita_m + 1.0);
}

TEST_CASE("meteo: la previsione del 1989 sbaglia di piu' piu' e' lontana, niente polar low") {
    Meteo m(dati(), "faroe", 1, 0.0, 31);
    double e12 = 0.0, e72 = 0.0;
    int n = 0;
    for (double emessa = 0.0; emessa < 200.0 * 86400.0; emessa += 6.0 * 3600.0) {
        const double v12 = m.previsione(emessa, emessa + 12.0 * 3600.0).vento_m_s;
        const double v72 = m.previsione(emessa, emessa + 72.0 * 3600.0).vento_m_s;
        m.avanza(emessa + 12.0 * 3600.0);
        e12 += std::pow(v12 - m.stato().vento_m_s, 2.0);
        m.avanza(emessa + 72.0 * 3600.0);
        e72 += std::pow(v72 - m.stato().vento_m_s, 2.0);
        ++n;
    }
    e12 = std::sqrt(e12 / n);
    e72 = std::sqrt(e72 / n);
    CHECK(e12 > 1.0);
    CHECK(e12 < e72);
    CHECK(e72 > 3.0);
    // deterministica: lo stesso bollettino
    CHECK(m.previsione(3600.0, 86400.0).vento_m_s == m.previsione(3600.0, 86400.0).vento_m_s);
    // il polar low non c'e' nel bollettino
    Meteo b(dati(), "barents", 3, 0.0, 11);
    bool visto = false;
    for (long h = 0; h < 20L * 30 * 24 && !visto; ++h) {
        b.avanza(static_cast<double>(h) * 3600.0);
        if (b.stato().evento != "polar_low") continue;
        visto = true;
        CHECK(b.previsione(b.tempo() - 12.0 * 3600.0, b.tempo()).evento.empty());
    }
    CHECK(visto);
}

TEST_CASE("meteo: nel punto, stretti e meteo fisso") {
    // Gibilterra: il levante incanalato nello stretto (vicino al centro del teatro)
    Meteo g(dati(), "gibilterra", 7, 0.0, 9);
    bool levante = false;
    for (long h = 0; h < 2000 && !levante; ++h) {
        g.avanza(static_cast<double>(h) * 3600.0);
        if (g.stato().regime != Regime::levante) continue;
        levante = true;
        CHECK(g.in({0.0, 0.0}).vento_m_s > 1.3 * g.stato().vento_m_s);
        CHECK(g.in({80000.0, 80000.0}).vento_m_s < 1.2 * g.stato().vento_m_s);
    }
    CHECK(levante);
    // fisso: lo stesso stato sempre e ovunque, la previsione esatta, niente lampi
    Meteo f(dati(), "faroe", 1, 0.0, 1);
    StatoMeteo s = meteo_semplificato();
    f.fissa(s);
    for (double t = 0.0; t < 86400.0; t += 3600.0) {
        f.avanza(t);
        CHECK(f.stato().vento_m_s == doctest::Approx(5.0));
        CHECK(f.stato().stato_mare() == 3);
        CHECK(f.in({50000.0, -30000.0}).vento_m_s == doctest::Approx(5.0));
    }
    CHECK(f.previsione(0.0, 72.0 * 3600.0).vento_m_s == doctest::Approx(5.0));
    CHECK(f.lampi(0.0, 86400.0).empty());
    // teatro sconosciuto: il semplificato
    Meteo x(dati(), "atlantide", 1, 0.0, 1);
    CHECK(x.fisso());
    CHECK(x.stato().stato_mare() == 3);
}

// --- nella missione (M2) ---

#include <filesystem>
#include <fstream>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"

namespace {

std::string scena_meteo(const std::string& nome, const std::string& teatro, const std::string& data) {
    const auto f = std::filesystem::temp_directory_path() / ("m688_prova_meteo_" + nome + ".lua");
    std::ofstream o(f);
    o << "return { id = 'PROVA_METEO_" << nome << "', teatro = '" << teatro << "', paesaggio_sonoro = false, lato = 'usa',"
      << " data = '" << data << "', ora_utc = 0, script = {},\n"
      << "varianti = { { probabilita = 1, unita = {\n"
      << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
         " quota_m = 60, rotta = 0, velocita_kn = 5, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5,"
         " quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 10 } },\n"
      << "} } } }\n";
    return f.string();
}

}  // namespace

TEST_CASE("meteo missione: il generatore del teatro nell'Enhanced col realistico, niente nel classico, semplificato predefinito") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const std::string file = scena_meteo("faroe", "faroe", "1989-01-15");
    Missione classico(file, 1);
    for (int i = 0; i < 90; ++i) classico.avanza(1.0 / 9.0);
    CHECK(classico.meteo() == nullptr);
    CHECK(classico.stato_mare() == 3);
    // Enhanced: il meteo di gennaio alle Faroe cambia nel tempo; lo stesso seme ridà lo stesso meteo
    const auto gioca = [&](std::uint64_t seme, bool semplificato) {
        auto m = std::make_unique<Missione>(file, seme);
        m->usa_meteo(semplificato ? Missione::ModoMeteo::semplificato : Missione::ModoMeteo::realistico);
        DanniMissione dm(classi, M688_DATI, seme, RegolaDanni::simulazione);
        std::vector<double> mare;
        for (int i = 0; i < 9 * 3600 * 6; ++i) {
            m->avanza(1.0 / 9.0);
            dm.avanza(*m, 1.0 / 9.0);
            if (i % (9 * 1800) == 0) mare.push_back(m->meteo_qui().hs_m());
        }
        return std::make_pair(std::move(m), mare);
    };
    auto [a, mare_a] = gioca(3, false);
    REQUIRE(a->meteo());
    CHECK(a->meteo()->teatro() == "faroe");
    CHECK(a->meteo()->mese() == 1);
    CHECK_FALSE(a->meteo()->fisso());
    CHECK(*std::max_element(mare_a.begin(), mare_a.end()) > *std::min_element(mare_a.begin(), mare_a.end()));
    CHECK(a->meteo_qui().vento_m_s > 0.0);
    auto [b, mare_b] = gioca(3, false);
    CHECK(mare_a == mare_b);
    auto [s, mare_s] = gioca(3, true);
    REQUIRE(s->meteo());
    CHECK(s->meteo()->fisso());
    CHECK(s->stato_mare() == 3);
    for (const double h : mare_s) CHECK(h == doctest::Approx(centro_douglas(3)));
    // Baltico d'estate: un altro teatro e un altro mese
    Missione e(scena_meteo("baltico", "baltico", "1989-07-15"), 2);
    e.usa_meteo(Missione::ModoMeteo::realistico);
    DanniMissione de(classi, M688_DATI, 2, RegolaDanni::simulazione);
    e.avanza(1.0 / 9.0);
    de.avanza(e, 1.0 / 9.0);
    e.avanza(1.0 / 9.0);
    REQUIRE(e.meteo());
    CHECK(e.meteo()->mese() == 7);
    CHECK(e.meteo()->teatro() == "baltico");
}

TEST_CASE("meteo missione: col mare grosso le navi rallentano e la vela affiora") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_meteo_mare_grosso.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_METEO_MARE', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', script = {},\n"
          << "varianti = { { probabilita = 1, unita = {\n"
          << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
             " quota_m = 6.096, rotta = 90, velocita_kn = 2, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5,"
             " quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 10 } },\n"
          << "{ id = 1, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 20000, y = 0 }, quota_m = 0, rotta = 0,"
             " velocita_kn = 28, modello = { massima_kn = 29, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0,"
             " integrita = 100, rumore = 10, sonar = 0 } },\n"
          << "} } } }\n";
    }
    Missione m(f.string(), 4);
    m.usa_meteo(Missione::ModoMeteo::realistico);
    DanniMissione dm(classi, M688_DATI, 4, RegolaDanni::simulazione);
    m.avanza(1.0 / 9.0);
    dm.avanza(m, 1.0 / 9.0);
    REQUIRE(m.meteo());
    StatoMeteo s = meteo_semplificato();
    s.vento_m_s = 22.0;
    s.vento_da_gradi = 0.0;   // da nord: la fregata (rotta 0) ha il mare di prua
    s.hs_vento_m = 7.5;       // mare 7
    m.meteo()->fissa(s);
    const auto& fregata = m.unita().at(1);
    CHECK(m.velocita_col_mare(fregata) == doctest::Approx(12.0).epsilon(0.1));
    bool affiorata = false;
    for (int i = 0; i < 9 * 1800; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        affiorata = affiorata || m.vela_affiorata();
    }
    CHECK(m.stato_mare() == 7);
    CHECK(m.unita().at(1).velocita <= 12.0 * 1.1);
    CHECK(affiorata);
    // col mare 3 niente limite
    m.meteo()->fissa(meteo_semplificato());
    m.avanza(1.0);
    CHECK(m.velocita_col_mare(fregata) > 100.0);
}

TEST_CASE("meteo missione: WEAX e avvisi di burrasca sul broadcast, la previsione chiesta, i nomi sovietici") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto oggetti = [](const Missione& m) {
        std::vector<std::string> v;
        for (int serie = 1; serie < 400; ++serie)
            if (const auto* x = m.radio()->messaggio(serie)) v.push_back(x->oggetto);
        return v;
    };
    // Faroe in gennaio dalle 00Z: i bollettini alle 06, 12, 18 e 00 UTC; la burrasca prima o poi in 30 ore
    bool burrasca = false, risposta = false;
    for (std::uint64_t seme = 1; seme <= 3 && !burrasca; ++seme) {
        Missione m(scena_meteo("radio_faroe_" + std::to_string(seme), "faroe", "1989-01-10"), seme);
        m.usa_meteo(Missione::ModoMeteo::realistico);
        DanniMissione dm(classi, M688_DATI, seme, RegolaDanni::simulazione);
        for (long i = 0; i < 9L * 3600 * 30; ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            if (i == 9L * 3600 && m.radio()) m.applica({m.tempo(), TipoOrdine::radio_trasmetti, 0, 0, static_cast<int>(TipoTrasmissione::previsione)});
        }
        REQUIRE(m.radio());
        const auto v = oggetti(m);
        CHECK(std::count(v.begin(), v.end(), "WEAX") >= 4);
        burrasca = burrasca || std::count(v.begin(), v.end(), "GALE WARNING") + std::count(v.begin(), v.end(), "STORM WARNING") > 0;
        risposta = risposta || std::count(v.begin(), v.end(), "WEAX (AS REQUESTED)") > 0;
    }
    CHECK(burrasca);
    // la richiesta va in onda solo se si e' potuto trasmettere (a 60 m no: niente albero)
    CHECK_FALSE(risposta);
    // dal lato sovietico i nomi del servizio della flotta
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_meteo_radio_urss.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_METEO_URSS', teatro = 'barents', paesaggio_sonoro = false, lato = 'urss', data = '1989-03-10',"
          << " ora_utc = 0, script = {},\n varianti = { { probabilita = 1, unita = {\n"
          << "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = 0, y = 0 },"
             " quota_m = 60, rotta = 0, velocita_kn = 5, modello = { massima_kn = 40, virata_gradi_s = 2, accel_kn_s = 0.5,"
             " quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 10 } },\n"
          << "} } } }\n";
    }
    Missione u(f.string(), 1);
    u.usa_meteo(Missione::ModoMeteo::realistico);
    DanniMissione du(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (long i = 0; i < 9L * 3600 * 7; ++i) {
        u.avanza(1.0 / 9.0);
        du.avanza(u, 1.0 / 9.0);
    }
    REQUIRE(u.radio());
    const auto v = oggetti(u);
    CHECK(std::count(v.begin(), v.end(), "МЕТЕОСВОДКА") >= 1);
}
